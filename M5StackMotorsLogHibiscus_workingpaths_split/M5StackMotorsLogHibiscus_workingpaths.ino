
#include "M5Unified.h"
#include "M5GFX.h"
#include "M5Module4EncoderMotor.h"
#include "SD.h"
#include "Unit_RTC.h"
#include <iostream>
#include <cmath> // for std::abs
#include <limits> // for std::numeric_limits
#include <iterator> // for std::size

M5Module4EncoderMotor driverA ;
M5Module4EncoderMotor driverB ;
M5Module4EncoderMotor driverC ;
M5Module4EncoderMotor driverD ;
M5Module4EncoderMotor driverE ;


//need to tweak retraction profile so petal sits in better, use lsteps return but may be due to backlash (different for each petal?)...
// current trip for kbase on deploy could be better
// still to log currents and implement safety trip on petals to prevent jams ideally use sd card to get log data
// have alterd get encoder value and any instance oif current to put readings in global variable now write log command that logs commands sent/text, times, global currents, encoder readings, could use an RTC. Would have, create new log cmd, log_section/text/comment cmd and log_params_command.
//implement buttons? button a to cycle between petals vs tel arms, b to start/stop? c to deploy vs retract? or jsut to move motors manually?


//For the 4 petal telescope:
//motors plug in as follows
// Petals (driverk, motork) (driverl, motorl)
// Petal 0: (0,0) (1,0)
// Petal 1: (0,1) (1,1)
// Petal 2: (2,0) (3,0)
// Petal 3: (2,1) (3,1)

//Telescopic arms 
// Arm 0 (0,2)
// Arm 1 (1,2)
// Arm 2 (2,2)
// Arm 3 (3,2)

//For Hibiscus motors

// Petals (driverk, motork) (driverl, motorl)
// Petal 1: (3,0) (4,0)
// Petal 2: (1,0) (2,0)
// Petal 3: (0,0) (4,1)
// Petal 4: (2,1) (3,1)
// Petal 5: (0,1) (1,1)

// Telescopic arms
// Arm 1: (Channel 2 (all drivers))
// Arm 2: (Channel 3 (all drivers)) - currently not connected

M5Module4EncoderMotor drivers[4] = {driverA, driverB, driverC, driverD};
Unit_RTC RTC;
rtc_time_type RTCtime;
rtc_date_type RTCdate;

float currents[5] = {0.0,0.0,0.0,0.0,0.0};
float voltages[5] = {0.0,0.0,0.0,0.0,0.0};
int32_t encoder_readings[5][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}};
int start_force = 0;
int petal_trip_tests[5] = {0, 0, 0, 0, 0};
const float telarms_backlash_down[5] = {0, 0, 0, 0, 0};


const float ksteps[41] = {0,121021,221163,305109,376940,440183,497853,552492,606213,660732,717411,777289,841114,909380,
982353,1060100,1142518,1229358,1285195,1327558,1369921,1408632,1447342,1486053,1524763,1563474,1602185,1640895,1679606,
1718316,1757027,1914786,2072545,2230304,2388063,2545822,2703581,2861340,3019099,3135149,3251199};//,3279416,3307634,3335851,3378552,3392286};


const float lsteps[41]= {0,66016,132031,198047,264063,330079,396095,462110,528126,594142,660157,726173,792189,858204,
924221,990236,1056252,1122268,1188283,1254299,1320315,1386330,1452346,1518362,1584378,1650394,1716409,1782425,1848441,1914456,1980472,
2046488,2112504,2178520,2244535,2310551,2376567,2442582,2508598,2609157,2761530};//,2855948,2907763,2942306,2991050,3022523};

// Old retraction paths - top of the petal maybe clashes with frame
/*
const float ksteps_rtn[46] = {0,13734,56435,84652,112870,141087,257137,373187,530946,688705,846464,1004223,1161982,1319741,1477500,1635259,1673970,1712680,1751391,
1790101,1828812,1867523,1906233,1944944,1983654,2022365,2064728,2107091,2162928,2249768,2332186,
2409933,2482906,2501172,2534997,2554875,2571554,2626073,2679794,2724433,2772103,2825346,2877177,3171123,3271265,3392286};

const float lsteps_rtn[46]= {0,31473,80217,114760,166575,260993,413366,513925,579941,645956,711972,777988,
844003,910019,976035,1042051,1108067,1174082,1240098,1306114,1372129,1438145,1504161,1570177,1636193,1702208,1768224,
1834240,1900255,1966271,2032287,2098302,2164319,2230334,2296350,2362366,2448381,2524397,2610413,2706428,2802444,2908460,2924476,2950492,2956507,3022523};
*/

// Test paths for retraction added by Omri 05/12/2024
/*
const float ksteps_rtn[44] = {0,13734,56435,84652,112870,141087,257137,373187,530946,688705,846464,1004223,1161982,1319741,1477500,1635259,1673970,1712680,1751391,
1790101,1828812,1867523,1906233,1944944,1983654,2022365,2064728,2107091,2162928,2249768,2332186,
2409933,2482906,2541172,2634997,2674875,2731554,2816073,2959794,3014433,3092103,3153460,3217177,3271123};

const float lsteps_rtn[44]= {0,31473,80217,114760,166575,260993,413366,513925,579941,645956,711972,777988,
844003,910019,976035,1042051,1108067,1174082,1240098,1306114,1372129,1438145,1504161,1570177,1636193,1702208,1768224,
1834240,1900255,1966271,2032287,2098302,2164319,2230334,2296350,2362366,2448381,2524397,2610413,2706428,2802444,2908460,2924476,2950492};
*/
// Test paths for retraction added by Omri - attempting to correct for bottom of mirror hitting frame

const float ksteps_rtn[44] = {0,13734,56435,84652,112870,141087,257137,373187,530946,688705,846464,1004223,1161982,1319741,1477500,1635259,1673970,1712680,1751391,
1790101,1828812,1867523,2006233,2044944,2183654,2222365,2264728,2307091,2362928,2399768,2412186,
2439933,2482906,2541172,2634997,2674875,2711554,2756073,2855794,2914433,3092103,3153460,3217177,3271123};

const float lsteps_rtn[44]= {0,31473,80217,114760,166575,260993,413366,513925,579941,645956,711972,777988,
844003,910019,976035,1042051,1108067,1174082,1240098,1306114,1372129,1438145,1504161,1570177,1636193,1702208,1768224,
1834240,1900255,1966271,2032287,2098302,2164319,2230334,2296350,2362366,2448381,2524397,2610413,2706428,2802444,2908460,2924476,2950492};

const int lsize = sizeof(lsteps) / sizeof(lsteps[0]); 
const int ksize = sizeof(ksteps) / sizeof(ksteps[0]); 
const int lrtnsize = sizeof(lsteps_rtn) / sizeof(lsteps_rtn[0]); 
const int krtnsize = sizeof(ksteps_rtn) / sizeof(ksteps_rtn[0]); 

//petals {{petal_driver_k, petal_driver_l, petal_0_channel_k, channel_l}....}
int petal_drivers_motors[5][4] = {{0,1,1,1},{3,4,0,0}, {1,2,0,0}, {4,0,1,0}, {2,3,1,1}};

#define MAX_RECORD_SIZE 256

float amp_record0[MAX_RECORD_SIZE] = {0};
uint8_t record_index0              = 0;
float amp_value0                   = 0.0f;

float amp_record1[MAX_RECORD_SIZE] = {0};
uint8_t record_index1              = 0;
float amp_value1                   = 0.0f;

float amp_record2[MAX_RECORD_SIZE] = {0};
uint8_t record_index2              = 0;
float amp_value2                   = 0.0f;

float amp_record3[MAX_RECORD_SIZE] = {0};
uint8_t record_index3              = 0;
float amp_value3                   = 0.0f;

float amp_record4[MAX_RECORD_SIZE] = {0};
uint8_t record_index4              = 0;
float amp_value4                   = 0.0f;

float offsets[5] = {12.0, 18.5, 15.0, 14.0, 20.5};

uint8_t n_channels =5;
String cmd;

uint8_t avg_filter_level = 20;
uint8_t mod_address_A = 0x1A;
uint8_t mod_address_B = 0x2A;
uint8_t mod_address_C = 0x3A;
uint8_t mod_address_D = 0x4A;
uint8_t mod_address_E = 0x5A;

int log_mode = 0;
String current_log_file = "";

float avg_filter(float *data, int len) {
    float sum = 0;
    float min = data[0];
    float max = data[0];
    for (int i = 0; i < len; i++) {
        if (data[i] < min) {
            min = data[i];
        }
        if (data[i] > max) {
            max = data[i];
        }
        sum += data[i];
    }
    sum -= min;
    sum -= max;
    return sum / (len - 2);
}

void setup() {
    M5.begin();
     M5.Display.begin();
    M5.Display.setTextColor(WHITE);
    M5.Display.setTextDatum(top_center);
    M5.Display.setFont(&fonts::FreeSansBold12pt7b);
    M5.Display.setTextSize(1);

    RTC.begin();  // Example Initialize the RTC clock.  
    RTCtime.Hours   = 15;  // Set the RTC clock time. 
    RTCtime.Minutes = 35;
    RTCtime.Seconds = 00;
    RTCdate.WeekDay = 4;  // Set the RTC clock date.  
    RTCdate.Month   = 10;
    RTCdate.Date    = 2;
    RTCdate.Year    = 2024;
    //RTC.setTime(&RTCtime);  // Example Synchronize the set time to the RTC.
    //RTC.setDate(&RTCdate);  // Synchronize the set date to the RTC.

    Serial.begin(115200);

     if (!SD.begin(4, SPI, 10000000)) {
    M5.Lcd.println("Card failed, or not present");
    Serial.println("Card Mount Failed");
    // Endlessly loop if the card cannot be initialized.
    while (1);
     }
    Serial.println("TF card initialized.");
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
     //File f = SD.open("/hello.txt", FILE_READ);
  writeFile(SD, "/hello.txt", "Hello ");
  readFile(SD, "/hello.txt");
  appendFile(SD, "/hello.txt", "World!\n");
  readFile(SD, "/hello.txt");


     while (!driverA.begin(&Wire, mod_address_A, 21, 22)) {
        Serial.println("Driver A Init faild!");
        M5.Display.drawString("Driver A Init faild!", 160, 7);
        delay(1000);
    }

         while (!driverB.begin(&Wire, mod_address_B, 21, 22)) {
        Serial.println("Driver B Init faild!");
        M5.Display.drawString("Driver B Init faild!", 160, 7);
        delay(1000);
    }

         while (!driverC.begin(&Wire, mod_address_C, 21, 22)) {
        Serial.println("Driver C Init faild!");
        M5.Display.drawString("Driver C Init faild!", 160, 7);
        delay(1000);
    }

         while (!driverD.begin(&Wire, mod_address_D, 21, 22)) {
        Serial.println("Driver D Init faild!");
        M5.Display.drawString("Driver D Init faild!", 160, 7);
        delay(1000);
    }
        while (!driverE.begin(&Wire, mod_address_E, 21, 22)) {
        Serial.println("Driver D Init faild!");
        M5.Display.drawString("Driver E Init faild!", 160, 7);
        delay(1000);
    }
    
    for (uint8_t i = 0; i < 4; i++) {
        driverA.setMode(i, NORMAL_MODE);
        driverA.setMotorSpeed(i, 0);
        driverB.setMode(i, NORMAL_MODE);
        driverB.setMotorSpeed(i, 0);
        driverC.setMode(i, NORMAL_MODE);
        driverC.setMotorSpeed(i, 0);
        driverD.setMode(i, NORMAL_MODE);
        driverD.setMotorSpeed(i, 0);
        driverE.setMode(i, NORMAL_MODE);
        driverE.setMotorSpeed(i, 0);
    }

    Serial.println("Normal Mode Set!");
    M5.Display.clear();
    M5.Display.fillRect(0, 0, 320, 35, 0x27f);
    M5.Display.drawString("Hibiscus Motor Ctl", 160, 7);
    M5.Display.setTextDatum(top_left);
    M5.Display.drawString("NORMAL MODE", 20, 40 + 35 * 5);

}



void loop() {
M5.update();
pollCurrents();
pollEncoders();
//LogParams("Loop");
LogCurrents("Loop");


 // put your main code here, to run repeatedly:

if (Serial.available())
{
  cmd = Serial.readStringUntil('\n');;
Serial.println(cmd);

// Wrapper commands 

// Deploy Petals
if (cmd[0] == '1'){
  for (int i = 0; i < 5; i++) {
    petal_trip_tests[i] = deployPetal(i);
  }  
}

//Retract Petals
if (cmd[0] == '2'){
  bool ready = false;  
  for (uint8_t i = 0; i < 5; i++) {
    Serial.print("Locks need to be manually disengaged before proceeding with retraction for petal: ");
    Serial.println(i);
    Serial.print("Disengage and press Enter when ready");
    if (Serial.available() > 0 && Serial.read() == '\n') {  // Wait for serial input (Enter key) before proceeding
      ready = true;
    }
    if (ready == true) {  
      petal_trip_tests[i] =retractPetal(i);
      ready = false;
    }
  }
}

//Deploy telescopic arms
if (cmd[0] == '3'){
  int telarmstage = cmd[1]-'0';
  int trip_test = deployTelArms(-125,telarmstage);
  if (trip_test<6){
  TripTelMotors(telarmstage, 1, 70);
  }
  
  //1375875388

}

// Retract telescopic arms
if (cmd[0] == '4'){
  int  telarmstage = cmd[1]-'0';
  int trip_test = retractTelArms(125,telarmstage);
  if (trip_test<6){
  TripTelMotors(telarmstage, 0, 70);
  }
  //TripTelMotors(3, 0, 50);
  //retractTelArms(450,2);
  //TripTelMotors(2, 0, 50);
  
}

//Deploy all (from full retraction)
if (cmd[0] == '5'){
  int trip_test = 0;
  while(trip_test < 5){
    trip_test = deployTelArms(-20,3);// first move the top tel arms out to clear path for petals
  for (int i = 0; i < 5; i++) {
    trip_test = deployPetal(i);
  }
  trip_test = deployTelArms(-105,3); // then deploy the arms to the top
  trip_test = TripTelMotors(3, 1, 50);
  trip_test = deployTelArms(125,2);
  trip_test = TripTelMotors(2, 1, 50);
  Serial.println("Fully deployed");
  trip_test = 5;
  }
  Serial.println("Deployment manually stopped");
}

// Retract all (from full deployment)
if (cmd[0] == '6') {
  int arm_trip_test = deployTelArms(-125, 2);
  //TripTelMotors(2, 0, 50);
  //deployTelArms(1000, 3); 
  bool ready = false;  
  for (uint8_t i = 0; i < 1; i++) {
    Serial.print("Locks need to be manually disengaged before proceeding with retraction for petal: ");
    Serial.println(i);
    Serial.print("Disengage and press Enter when ready");
    if (Serial.available() > 0 && Serial.read() == '\n') {  // Wait for serial input (Enter key) before proceeding
      ready = true;
    }
    if (ready == true) {  
      petal_trip_tests[i] = retractPetal(i);
      ready = false;
    }
  }
  //deployTelArms(300, 3);
  //TripTelMotors(3, 0, 50);
  Serial.println("Fully retracted");
}

//custom command to balance arms
if (cmd[0]=='7'){
  int  motor_cmd = cmd[1]-'0';
  int direction = cmd[2]-'0';
  BalanceArms(motor_cmd, direction);
}

// cmd polls encoders 'E'
if (cmd[0] == 'E'){
  int  driver_cmd = cmd[1]-'0';
  int  motor_cmd = cmd[2]-'0';
  int32_t enc  = GetEncoderValue(driver_cmd, motor_cmd);
  Serial.println(enc);
  }

if (cmd[0] == 'C'){
  int  driver_cmd = cmd[1]-'0';
  currents[driver_cmd]  = GetCurrentValue(driver_cmd);
  Serial.println(currents[driver_cmd]);

}

if (cmd[0] == 'V'){
int force_ind_cmd = cmd.indexOf('\n');
if(force_ind_cmd>1){
  String force_str_cmd = (cmd.substring(1,force_ind_cmd));
  LogCurrents(force_str_cmd);
  start_force = force_str_cmd.toInt();}
else{
  start_force = start_force+2;
  String force_str_cmd = String(start_force);
  LogCurrents(force_str_cmd);
}
}


if (cmd[0] == 'M'){
  int  driver_cmd = cmd[1]-'0';
  int  motor_cmd = cmd[2]-'0';
  int speed_ind_cmd = cmd.indexOf('\n');
  String speed_str_cmd = (cmd.substring(3,speed_ind_cmd));
  int8_t  speed_cmd = speed_str_cmd.toInt();
MoveMotor(driver_cmd, motor_cmd, speed_cmd);
  
}


//if (cmd[0] == 's'){
//   Stop();
//   }

if (cmd[0] == 'L'){
  log_mode = cmd[1]-'0';
  int log_ind_cmd = cmd.indexOf('\n');
  String file_str_cmd = (cmd.substring(2,log_ind_cmd));
  if(log_mode == 1){
    RTC.getTime(&RTCtime);  // To get the time.
    RTC.getDate(&RTCdate);
    String current_time = String(RTCtime.Hours)+"-"+String(RTCtime.Minutes)+"-"+String(RTCtime.Seconds);
    String current_date = String(RTCdate.Year)+"-"+String(RTCdate.Month)+"-"+String(RTCdate.Date);
    current_log_file = "/"+file_str_cmd+"_"+current_date+"_"+current_time+".csv";
    char log_file_buffer[current_log_file.length()];
    current_log_file.toCharArray(log_file_buffer, current_log_file.length()+1);
    String t_sec = String(millis());
    String line_to_add = "Log file date: " +current_date+"\n Log file time: " +current_time+"\n Log start millis: "+t_sec+"\n";
    char char_line_to_add[line_to_add.length()+1];
    line_to_add.toCharArray(char_line_to_add, line_to_add.length()+1);
    writeFile(SD, log_file_buffer, char_line_to_add);
  }
}

if (cmd[0] == 'F'){
  Serial.println(current_log_file);
  char log_file_buffer[current_log_file.length()];
  current_log_file.toCharArray(log_file_buffer, current_log_file.length()+1);
    readFile(SD, log_file_buffer);
  }

if (cmd[0] == 'A'){
  LogParams("Append");
  }


if (cmd[0] == 'T'){
  zeroEncoders();
  int  driver_cmd = cmd[1]-'0';
  int  motor_cmd = cmd[2]-'0';
  int  direction_cmd = cmd[3]-'0';
  int current_ind_cmd = cmd.indexOf('\n');
  String current_str_cmd = (cmd.substring(4,current_ind_cmd));
  int  current_cmd = current_str_cmd.toInt();
TripMotor(driver_cmd, motor_cmd, direction_cmd, current_cmd);
  }

if (cmd[0] == 'B'){
  //zeroEncoders();
  
  int  motor_cmd = cmd[1]-'0';
  int  direction_cmd = cmd[2]-'0';
  int current_ind_cmd = cmd.indexOf('\n');
  String current_str_cmd = (cmd.substring(3,current_ind_cmd));
  int8_t  current_cmd = current_str_cmd.toInt();
  for (uint8_t i = 0; i < 1; i++) {
    int trip_test = TripTelMotors(motor_cmd, direction_cmd, current_cmd);
    if(trip_test == 5){
      Serial.println("keyboard interrupt");
      break;
    //}
  }
  }
}

if (cmd[0] == 'D'){
  int  petal = cmd[1]-'0';
  petal_trip_tests[petal] = deployPetal(petal);

  }

if (cmd[0] == 'R'){
  int  direction = cmd[1]-'0';
  zeroEncoder(0,2);
  zeroEncoder(1,2);
  zeroEncoder(2,2);
  zeroEncoder(3,2);
  //if(direction == 1){
  //MoveTelArms2(telarms_backlash_down[0], telarms_backlash_down[1], telarms_backlash_down[2], telarms_backlash_down[3], telarms_backlash_down[4]);}
  //if(direction == 0){
  //MoveTelArms2(-1*telarms_backlash_down[0], -1*telarms_backlash_down[1], -1*telarms_backlash_down[2], -1*telarms_backlash_down[3], -1*telarms_backlash_down[4]);}
  //}
  }

if (cmd[0] == 'U'){
  int  petal = cmd[1]-'0';
  petal_trip_tests[petal] = retractPetal(petal);
}
/*
  bool ready = false;
  Serial.print("Locks need to be manually disengaged before proceeding with retraction for petal: ");
  Serial.println(petal);
  Serial.print("Disengage and press Enter when ready");
  for (uint8_t i = 0; i < 1; i++) {
  if (Serial.available() > 0 && Serial.read() == '\n') {  // Wait for serial input (Enter key) before proceeding
    ready = true;
  }
  if (ready == true) {  
    petal_trip_tests[petal] = retractPetal(petal);
    ready = false;
  }
  }
  }
*/

if (cmd[0] == 'X'){
  int  tel_arm_stage = cmd[1]-'0';
  int end_ind_cmd = cmd.indexOf('\n');
  String enc_str_cmd = (cmd.substring(1,end_ind_cmd));
  int32_t  enc_cmd = enc_str_cmd.toInt();
  deployTelArms(enc_cmd, tel_arm_stage);
  }

if (cmd[0] == 'Q'){
  int  tel_arm_stage = cmd[1]-'0';
  //int end_ind_cmd = cmd.indexOf('\n');
  //String enc_str_cmd = (cmd.substring(1,end_ind_cmd));
  //int32_t  enc_cmd = enc_str_cmd.toInt();
  retractTelArms(2,tel_arm_stage);
  }



if (cmd[0] == 'N'){
  int  motor_cmd = cmd[1]-'0';
  int speed_ind_cmd = cmd.indexOf('\n');
  String speed_str_cmd = (cmd.substring(2,speed_ind_cmd));
  int8_t  speed_cmd = speed_str_cmd.toInt();
  MvTArmMotors( motor_cmd, speed_cmd);
  }

if (cmd[0] == 'Z'){
  zeroEncoders();
  }

if (cmd[0] == 'W'){
  RTC.getTime(&RTCtime);  // To get the time.
  RTC.getDate(&RTCdate);
  String current_time = String(RTCtime.Hours)+":"+String(RTCtime.Minutes)+":"+String(RTCtime.Seconds);
  String current_date = String(RTCdate.Date)+"-"+String(RTCdate.Month)+"-"+String(RTCdate.Year);
  Serial.println(current_time);
  Serial.println(current_date);
    } 
  }
}


























