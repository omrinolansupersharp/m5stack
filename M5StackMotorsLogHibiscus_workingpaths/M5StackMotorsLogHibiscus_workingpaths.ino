
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

void Stop(){
  for (uint8_t i = 0; i < 4; i++){
  driverA.setMode(i, NORMAL_MODE);
  driverA.setMotorSpeed(i, 0);
  driverB.setMode(i, NORMAL_MODE);
  driverB.setMotorSpeed(i, 0);
  driverC.setMode(i, NORMAL_MODE);
  driverC.setMotorSpeed(i, 0);
  driverD.setMode(i, NORMAL_MODE);
  driverD.setMotorSpeed(i, 0);
  driverE.setMode(i, NORMAL_MODE);
  driverE.setMotorSpeed(i, 0);}
  pollCurrents();
  pollEncoders();
  LogParams("Stop");
}

void MoveMotor(int driver_cmd, int motor_cmd, int speed){ 
  if (driver_cmd == 0){
  driverA.setMode(motor_cmd, NORMAL_MODE);
  driverA.setMotorSpeed(motor_cmd, speed);}
  
  if (driver_cmd == 1){
  driverB.setMode(motor_cmd, NORMAL_MODE);
  driverB.setMotorSpeed(motor_cmd, speed);}

  if (driver_cmd == 2){
  driverC.setMode(motor_cmd, NORMAL_MODE);
  driverC.setMotorSpeed(motor_cmd, speed);}

  if (driver_cmd == 3){
  driverD.setMode(motor_cmd, NORMAL_MODE);
  driverD.setMotorSpeed(motor_cmd, speed);}

   if (driver_cmd == 4){
  driverE.setMode(motor_cmd, NORMAL_MODE);
  driverE.setMotorSpeed(motor_cmd, speed);}

}

void MvTArmMotors(int motor_cmd, int speed){
 
  driverA.setMode(motor_cmd, NORMAL_MODE);
  driverA.setMotorSpeed(motor_cmd, speed);
  driverB.setMode(motor_cmd, NORMAL_MODE);
  driverB.setMotorSpeed(motor_cmd, speed);
  driverC.setMode(motor_cmd, NORMAL_MODE);
  driverC.setMotorSpeed(motor_cmd, speed);
  driverD.setMode(motor_cmd, NORMAL_MODE);
  driverD.setMotorSpeed(motor_cmd, speed);
  driverE.setMode(motor_cmd, NORMAL_MODE);
  driverE.setMotorSpeed(motor_cmd, speed);
}


void TripMotor(int driver_cmd, int motor_cmd, int direction, int trip_current){
  pollCurrents();
  pollEncoders();
  Serial.println(trip_current);
  LogParams("TripMotor");
  if( direction == 0){
 MoveMotor(driver_cmd, motor_cmd, 127);
  }
  if( direction == 1){
 MoveMotor(driver_cmd, motor_cmd, -127);
  }
  float current = 0.0;
  int test = 0;
  while(test <1){
    currents[driver_cmd] = GetCurrentValue(driver_cmd);
    if(abs(currents[driver_cmd]) > trip_current){
    MoveMotor(driver_cmd, motor_cmd, 0);
    Serial.println(currents[driver_cmd]);
    test = 2;
    }
    if (Serial.available() > 0 && Serial.read() == 'c'){ // manual stop - all motors stop and trip returned
    MoveMotor(driver_cmd, motor_cmd, 0);
    Serial.println("Manual stop");
    test = 5;
    }
    GetEncoderValue(driver_cmd, motor_cmd);
    LogParams("TripMotor");
}
currents[driver_cmd] = GetCurrentValue(driver_cmd);
pollEncoders();
LogParams("TripMotor");
}

int 
TripTelMotors(int motor_cmd, int direction, int trip_current){
  int32_t live_pos[5] = {0,0,0,0,0};
  int trip = 0;
  /*
  for (uint8_t i = 0; i < 5; i++) {
    live_pos[i] = GetEncoderValue(i, motor_cmd);
    Serial.print("motor ");
    Serial.print(i);
    Serial.print(" : Encoder Value ");
    Serial.println(live_pos[i]);
  }
  */
  pollCurrents();
  pollEncoders();
  int drivers[5] = {0,1,2,3,4};
  LogParams("TripTelMotors");
  if( direction == 0){
  MvTArmMotors(motor_cmd, 127);
  }
  if( direction == 1){
 MvTArmMotors(motor_cmd, -127); }
  int test[5] = {0, 0,0,0,0};
  int sumtest = 0;
  delay(300);
  while(sumtest <5){
          for (uint8_t i = 0; i < 5; i++) {
    currents[i] = GetCurrentValue(drivers[i]);
    if(abs(currents[i]) > trip_current){
      MoveMotor(drivers[i], motor_cmd, 0);
      //MoveMotor(0, motor_cmd, 0);
      //MoveMotor(1, motor_cmd, 0);
      //MoveMotor(2, motor_cmd, 0);
      //MoveMotor(3, motor_cmd, 0);
      //MoveMotor(4, motor_cmd, 0);
      //MvTArmMotors(motor_cmd,0);
      test[i] = 1;
      Serial.println(currents[i]);
      Serial.println(drivers[i]);
    }
    if (Serial.available() > 0 && Serial.read() == 'c'){ // manual stop - all motors stop and trip returned
    int tel_arm_stage = motor_cmd;
    MoveMotor(0, tel_arm_stage, 0);
    MoveMotor(1, tel_arm_stage, 0);
    MoveMotor(2, tel_arm_stage, 0);
    MoveMotor(3, tel_arm_stage, 0);
    MoveMotor(4, tel_arm_stage, 0);
    Serial.println("Manual stop");
    trip = 6;
    }
    GetEncoderValue(drivers[i], motor_cmd);
    live_pos[i] = GetEncoderValue(drivers[i], motor_cmd);
}

if(trip==0){ // while the motor is not tripped, set the sum test to be 
  LogParams("TripTelMotors");
  sumtest = test[0]+test[1]+test[2]+test[3]+test[4];
}
else{
  LogParams("TripTelMotors");
  sumtest = trip;
}
}

pollCurrents();
pollEncoders();
LogParams("TripTelMotors");
return trip;}

int MovePairMotors(int driver_k, int motor_k, int32_t pos_k, int driver_l, int motor_l, int32_t pos_l, int min_speed = 95){
  pollCurrents();
  pollEncoders();
  LogParams("MoveMotorPair");
  int32_t start_l = GetEncoderValue(driver_l, motor_l);
  int32_t start_k = GetEncoderValue(driver_k, motor_k);
  int32_t live_pos_l = GetEncoderValue(driver_l, motor_l);
  int32_t live_pos_k = GetEncoderValue(driver_k, motor_k);
  int32_t steps_l = pos_l - start_l ;
  int32_t steps_k = pos_k - start_k;
  int dir_k = 1;
  int dir_l = 1;
  float speed_k = 0;
  float speed_l = 0;
  int8_t max_speed = 127;
  //int8_t min_speed = 95;
  if(steps_k < 0){
    dir_k=-1;
  }
  if(steps_l < 0){
    dir_l=-1;
  }
  if(abs(steps_l) > abs(steps_k)){
    speed_l = max_speed;
    speed_k = max_speed * float(abs(steps_k))/float(abs(steps_l));
    if(speed_k < min_speed){
      speed_k = min_speed;
    }
    speed_k = speed_k;
  }
  else {
    speed_k = max_speed;
    speed_l = max_speed * float(abs(steps_l))/float(abs(steps_k));
    if(speed_l < min_speed){
      speed_l = min_speed;
    }
    speed_l = speed_l;
  }
  //Serial.println(speed_k);
  //Serial.println(speed_l);
  MoveMotor(driver_l, motor_l, dir_l * speed_l);
  MoveMotor(driver_k, motor_k, dir_k * speed_k);
  int test[2] = {0,0};
  int sumtest = 0;
  int trip = 0;
  while(sumtest < 2){
  currents[driver_l] = GetCurrentValue(driver_l);
  currents[driver_k] = GetCurrentValue(driver_k);
  live_pos_l = GetEncoderValue(driver_l, motor_l);
  live_pos_k = GetEncoderValue(driver_k, motor_k);
  if(abs(live_pos_l-start_l) > abs(steps_l)){
    MoveMotor(driver_l, motor_l, 0);
    test[0]=1;
  }
  if(abs(live_pos_k-start_k) > abs(steps_k)){
    MoveMotor(driver_k, motor_k, 0);
    test[1]=1;
  }
  if(abs(currents[driver_l])> 1500){
    MoveMotor(driver_l, motor_l, 0);
    MoveMotor(driver_k, motor_k, 0);
    trip = 3;
  }
  if(abs(currents[driver_k])> 1500){
    MoveMotor(driver_l, motor_l, 0);
    MoveMotor(driver_k, motor_k, 0);
    trip = 3;
  }
  if (Serial.available() > 0 && Serial.read() == 'c'){
    MoveMotor(driver_l, motor_l, 0);
    MoveMotor(driver_k, motor_k, 0);
    Serial.println("Manual stop");
    trip = 6; // manual stop always has trip of 6
  }
  if(trip==0){
  LogParams("MoveMotorPair");
  sumtest = test[0]+test[1];
  }
  else{
  LogParams("MoveMotorPair");
  sumtest = trip;
  }
}

pollCurrents();
pollEncoders();
LogParams("MoveMotorPair");
return sumtest;
}

void BalanceArms(int motor, int direction){
  // need to implement the direction changing the direction of this movement
  MoveMotor(2, 3, -127);
  MoveMotor(2, 2, 127);
  MoveMotor(3, 3, -127);
  MoveMotor(3, 2, 127);
  MoveMotor(4, 3, -127);
  MoveMotor(4, 2, 127);
  MoveMotor(0, 3, -127);
  MoveMotor(0, 2, 127);
}


int MoveTelArms(int32_t pos,int tel_arm_stage){
  int trip_current=70; // this is crucial, this is the trip current that we use for the majority of the movement, needs to be enough to overcome minor jams
  int warning_current=50;
  Serial.print("Target position is: ");
  Serial.println(pos);
  pollCurrents();
  pollEncoders();
  String mv_tel_cmd = "MoveTelArms Trip: "+String(trip_current);
  LogParams(mv_tel_cmd);
  int32_t start[5] = {0,0,0,0,0};
  int32_t live_pos[5] = {0,0,0,0,0};
  int32_t steps[5] = {0,0,0,0,0};
  int speeds[5] = {-127,-127,-127,-127,-127}; // tel arm deployment is negative current for move motor
  int direction = 1; // default direction is deployment
  int test[5] = {0,0,0,0,0};
  int sumtest = 0;
  int trip = 0;
  for (uint8_t i = 0; i < 5; i++) {
    start[i] = GetEncoderValue(i,tel_arm_stage);
    Serial.print("Start position is: ");
    Serial.print(start[i]);
    Serial.print(" for motor ");
    Serial.println(i);
    live_pos[i] = start[i];
    steps[i] = pos - start[i];
    //if(steps[i]>0){
    //  if(tel_arm_stage=3){ // top tel arm stage encoder values go down and become negative as arm deploys
    //    direction = 0; // therefore positive steps means that the top arm stage is retracting
    //    speeds[i] = 127;
    //  }
    //}
    if(steps[i]>0){
        direction = 0; // positive steps means that the arm is retracting
        speeds[i] = 127;
        }
    }
      for (uint8_t i = 0; i < 5; i++) {
    MoveMotor(i, tel_arm_stage, speeds[i]);
      }
  while(sumtest <5){
    for (uint8_t i = 0; i < 5; i++) {
      pollCurrents();
      pollEncoders();
      live_pos[i] = GetEncoderValue(i, tel_arm_stage);
      currents[i] = GetCurrentValue(i);
  /*
  if (live_pos[i] = 0){ // encoder count is zero, means that the encoder is disconnected (doesn't account for it disconnecting midway through motion)
    Serial.print(i);
    Serial.println(" motor encoder is disconnected or faulty");
    trip = 5;
  }
  */
  if(((abs(live_pos[i]))-abs(start[i]))> abs(steps[i])){ // encoder count reached for one motor
    MoveMotor(i, tel_arm_stage, 0);
    test[i] = 1;
    //Serial.print("Motor ");
    //Serial.print(i);
    //Serial.print(" encoder stopped at: ");
    //Serial.println(live_pos[i]);
  }
  if(abs(currents[i])> warning_current){
    Serial.print("Warning: Current is above ");
    Serial.println(warning_current);
  }
  if(abs(currents[i])> trip_current){ // current tripped for one motor - all motors stop and trip returned
    MoveMotor(0, tel_arm_stage, 0);
    MoveMotor(1, tel_arm_stage, 0);
    MoveMotor(2, tel_arm_stage, 0);
    MoveMotor(3, tel_arm_stage, 0);
    MoveMotor(4, tel_arm_stage, 0);
    Serial.print("Motor ");
    Serial.print(i);
    Serial.print(" tripped ");
    Serial.print(currents[i]);
    Serial.println(" mA");
    trip = 6;
  }
  if (Serial.available() > 0 && Serial.read() == 'c'){ // manual stop - all motors stop and trip returned
    MoveMotor(0, tel_arm_stage, 0);
    MoveMotor(1, tel_arm_stage, 0);
    MoveMotor(2, tel_arm_stage, 0);
    MoveMotor(3, tel_arm_stage, 0);
    MoveMotor(4, tel_arm_stage, 0);
    Serial.println("Manual stop");
    trip = 7;
  }  
}
if(trip==0){ // while the motor is not tripped, set the sum test to be 
  LogParams(mv_tel_cmd);
  sumtest = test[0]+test[1]+test[2]+test[3]+test[4];
}
else{
  LogParams(mv_tel_cmd);
  sumtest = trip;
}
pollCurrents();
pollEncoders();
/*
if(sumtest==5){ //after all encoder values are the same check that all arms are caught up to each other - do we need this if we are already driving up to a certain encoder value?
int32_t maxVal = 0;
for (uint8_t i = 0; i < 5; i++) {
  if (live_pos[i] > maxVal) {
        maxVal = live_pos[i];
      }
}
for (uint8_t i = 0; i < 5; i++){
    // if any motors are lagging behind the maximum value
    if (live_pos[i] < (maxVal)){
      int32_t steps = live_pos[i] - maxVal;
      int32_t start = live_pos[i];
      Serial.print("Steps: ");
      Serial.println(steps);
      Serial.print("start: ");
      Serial.println(start);
      MoveMotor(i, tel_arm_stage, 127);
      int test = 0;
      while (test !=1){
        int32_t live_pos_corr = GetEncoderValue(i, tel_arm_stage);
        if(abs(live_pos_corr - start) > abs(steps)){
          MoveMotor(i, tel_arm_stage, 0);
          test = 1;
          Serial.println(" Correction completed ");
          }
        }
    }
}
}
*/
}
pollCurrents();
pollEncoders();
LogParams(mv_tel_cmd);
Serial.println(sumtest);
Serial.println(trip);
return trip; // finished, trip will either be 0 if move has completed smoothly or 5 if there is a current trip or keyboard interrupt
}



int deployTelArms(int n, int tel_arm_stage ){
//int tel_arm_stage = 2;
pollCurrents();
pollEncoders();
int32_t live_pos[5] = {0,0,0,0,0};
int32_t int_pos[5] = {0,0,0,0,0};
int32_t max_value = int_pos[0];
//for (uint8_t i = 0; i < 5; i++) {
//  zeroEncoder(i, tel_arm_stage);
//}
for (uint8_t i = 0; i < 5; i++) {
    live_pos[i] = GetEncoderValue(i,tel_arm_stage);
    int_pos[i] = live_pos[i]/10000000;
    if (int_pos[i] < max_value){
    max_value = int_pos[i];
  }    
}
LogParams("DeployTelArms");
int trip_test = 0;
for(int32_t i = max_value-1; i > n-1; i--){ //previously starting at i = -1
  Serial.print(i);
  Serial.print("/");
  Serial.print(n);
  Serial.println(" steps");
  int trip_test = MoveTelArms((i*10000000), tel_arm_stage);
  if(trip_test > 4){
    Serial.println("Current tripped or keyboard interrupt");
    break;
  }
} 
pollCurrents();
pollEncoders();
LogParams("DeployTelArms");
return trip_test;
}

int retractTelArms(int n, int tel_arm_stage){
//int tel_arm_stage = 2;
pollCurrents();
pollEncoders();
// code to check the current position of the arms
int32_t live_pos[5] = {0,0,0,0,0};
int32_t int_pos[5] = {0,0,0,0,0};
int32_t max_value = int_pos[0];
for (uint8_t i = 0; i < 5; i++) {
    live_pos[i] = GetEncoderValue(i,tel_arm_stage);
    int_pos[i] = live_pos[i]/10000000;
    if (int_pos[i] > max_value){
    max_value = int_pos[i];
  }    
}
//LogParams("RetractTelArms"+String(n*1000000)+" Steps");
//for (uint8_t i = 0; i < 5; i++) {
//  zeroEncoder(i, tel_arm_stage);
//}
LogParams("RetractTelArms");
int trip_test = 0;
for(int8_t i = max_value+1; i < n+1; i++){
  trip_test = MoveTelArms((i*10000000),tel_arm_stage);
  if(trip_test > 4){
    Serial.println("Current tripped or keyboard interrupt");
    break;
  }
}
pollCurrents();
pollEncoders();
LogParams("RetractTelArms");
return trip_test;
}


int deployPetal(int petal){
String log_cmd = "DeployPetal "+String(petal);
pollCurrents();
pollEncoders();
LogParams(log_cmd);
int trip_test = 0;
int driver_k = petal_drivers_motors[petal][0];
int driver_l = petal_drivers_motors[petal][1];
int motor_k = petal_drivers_motors[petal][2];
int motor_l = petal_drivers_motors[petal][3];
//zeroEncoder(driver_k, motor_k);
//zeroEncoder(driver_l, motor_l);
// code to take the current position of the petals and compare to steps
int32_t live_pos[2] = {0,0};
live_pos[0] = GetEncoderValue(driver_k,motor_k);
live_pos[1] = GetEncoderValue(driver_l,motor_l);
int knextStep = findNextStep(ksteps, ksize, live_pos[0]);
int lnextStep = findNextStep(lsteps, lsize, live_pos[1]);
int nextstep = 1;
if (knextStep != -1) {
        Serial.print("The next step larger than the current position is: ");
        Serial.println(knextStep);
        nextstep = (knextStep);
    } 
    //else {
    //    Serial.println("petal is already fully deployed");
    //    trip_test = 2;
        //break;
    //}
pollCurrents();
pollEncoders();
LogParams(log_cmd);
int min_speed = 95;
for (int step = nextstep; step < (ksize); step++){
  if(step>14){
    min_speed = 127;
  }
  else{
    min_speed=95;
  }
  trip_test = MovePairMotors(driver_k, motor_k, ksteps[step], driver_l, motor_l, lsteps[step], min_speed);
  if(trip_test > 2){
    Serial.println("Current tripped or keyboard interrupt");
    break;
  }
  Serial.print(step);
  Serial.println(trip_test);
}
if(trip_test < 4){ // if the path finished without manual interruption
TripMotor(driver_l, motor_l,0, 500);
TripMotor(driver_k, motor_k,0, 500);
TripMotor(driver_l, motor_l,0, 1000);
TripMotor(driver_k, motor_k,0, 1000);
}
pollCurrents();
pollEncoders();
LogParams(log_cmd);
return trip_test; // returns the trip reason
}


int retractPetal(int petal){
String log_cmd = "RetractPetal "+String(petal);
pollCurrents();
pollEncoders();
LogParams(log_cmd);
int driver_k = petal_drivers_motors[petal][0];
int driver_l = petal_drivers_motors[petal][1];
int motor_k = petal_drivers_motors[petal][2];
int motor_l = petal_drivers_motors[petal][3];
int min_speed = 95;
//zeroEncoder(driver_k, motor_k);
//zeroEncoder(driver_l, motor_l);
pollCurrents();
pollEncoders();
int trip_test = 0;
LogParams(log_cmd);
int32_t live_pos[2] = {0,0};
live_pos[0] = GetEncoderValue(driver_k,motor_k);
live_pos[1] = GetEncoderValue(driver_l,motor_l);
int krtnnextStep = findNextStep(ksteps_rtn, ksize, -1* live_pos[0]);
int lrtnnextStep = findNextStep(lsteps_rtn, lsize, -1* live_pos[1]);
int rtnnextstep = 0;
if (krtnnextStep != -1) {
        Serial.print("The next k step larger than the current position is: ");
        Serial.println(krtnnextStep);
        Serial.print("The next l step larger than the current position is: ");
        Serial.println(lrtnnextStep);
        rtnnextstep = (krtnnextStep);
    } 
    //else {
    //    Serial.println("petal is already fully deployed");
    //    trip_test = 2;
        //break;
    //}
for (int step = rtnnextstep; step < (krtnsize); step++){

Serial.println(lsteps_rtn[step]);
Serial.println(step);
if(step<35){
  min_speed = 127;
}
else{
  min_speed=95;
}
trip_test = MovePairMotors(driver_k, motor_k, -1*ksteps_rtn[step], driver_l, motor_l, -1*lsteps_rtn[step], min_speed);
if(trip_test > 2){
    Serial.println("Current tripped or keyboard interrupt");
    break;
}
}
if(trip_test < 3){ // if the path finished without manual interruption then trip to the end of the loop
TripMotor(driver_l, motor_l,1, 500);
TripMotor(driver_k, motor_k,1, 500);
//TripMotor(driver_l, motor_l,1, 1000);
//TripMotor(driver_k, motor_k,1, 1000);
}
pollCurrents();
pollEncoders();
LogParams(log_cmd);
return trip_test;
}


int32_t GetEncoderValue(int driver, int motor){
  if (driver == 0){
  encoder_readings[0][motor] = driverA.getEncoderValue(motor);
  }
  if (driver == 1){
  encoder_readings[1][motor] = driverB.getEncoderValue(motor);
  }
  if (driver == 2){
  encoder_readings[2][motor] = driverC.getEncoderValue(motor);
  }
  if (driver == 3){
  encoder_readings[3][motor] = driverD.getEncoderValue(motor);
  }
    if (driver == 4){
  encoder_readings[4][motor] = driverE.getEncoderValue(motor);
  }
    return encoder_readings[driver][motor];
}

float GetCurrentValue(int driver){
  float current = 0.0;
  if (driver == 0){
if (avg_filter_level != 0) {
        amp_record0[record_index0] = driverA.getMotorCurrent();
        record_index0++;
        if (record_index0 >= avg_filter_level) {
            record_index0 = 0;
        }
        amp_value0 = avg_filter(amp_record0, avg_filter_level);
    }
    current = amp_value0; 
    voltages[driver] = driverA.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16; 
}
  if (driver == 1){
if (avg_filter_level != 0) {
        amp_record1[record_index1] = driverB.getMotorCurrent();
        record_index1++;
        if (record_index1 >= avg_filter_level) {
            record_index1 = 0;
        }
        amp_value1 = avg_filter(amp_record1, avg_filter_level);
    }
     current = amp_value1;
     voltages[driver] = driverB.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16; 
}
  if (driver == 2){
if (avg_filter_level != 0) {
        amp_record2[record_index2] = driverC.getMotorCurrent();
        record_index2++;
        if (record_index2 >= avg_filter_level) {
            record_index2 = 0;
        }
        amp_value2 = avg_filter(amp_record2, avg_filter_level);
    }
    current = amp_value2;
  voltages[driver] = driverC.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16; 
 }

  if (driver == 3){
if (avg_filter_level != 0) {
        amp_record3[record_index3] = driverD.getMotorCurrent();
        record_index3++;
        if (record_index3 >= avg_filter_level) {
            record_index3 = 0;
        }
        amp_value3 = avg_filter(amp_record3, avg_filter_level);
    }
    current = amp_value3;
    voltages[driver] = driverD.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16; 
}

  if (driver == 4){
if (avg_filter_level != 0) {
        amp_record4[record_index4] = driverE.getMotorCurrent();
        record_index4++;
        if (record_index4 >= avg_filter_level) {
            record_index4 = 0;
        }
        amp_value4 = avg_filter(amp_record4, avg_filter_level);
    }
    current = amp_value4;
    voltages[driver] = driverE.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16; 
}

    current = current*1000 + offsets[driver];
    M5.Display.fillRect(20, 40 + 35 * driver, 300, 35, BLACK);
    M5.Display.drawString("CH" + String(driver) + ": " + String(current) + "mA",20, 40 + 35 * driver);
      return current;
}



void pollEncoders()
{
    for (uint8_t i = 0; i < 5; i++) {
    for (uint8_t j = 0; j < 4; j++) {
      GetEncoderValue(i,  j);
    }
}}

void zeroEncoders()
{
    for (uint8_t i = 0; i < 4; i++) {
        driverA.setEncoderValue(i,0);
    }
        for (uint8_t i = 0; i < 4; i++) {
        driverB.setEncoderValue(i,0);
    }
        for (uint8_t i = 0; i < 4; i++) {
        driverC.setEncoderValue(i,0);
    }
        for (uint8_t i = 0; i < 4; i++) {
        driverD.setEncoderValue(i,0);
    }
  for (uint8_t i = 0; i < 4; i++) {
        driverE.setEncoderValue(i,0);
    }
}

void zeroEncoder(int driver, int motor){
    if(driver == 0) {
        driverA.setEncoderValue(motor,0);
    }
    if(driver == 1)  {
        driverB.setEncoderValue(motor, 0);
    }
    if(driver == 2)  {
        driverC.setEncoderValue(motor, 0);
    }
    if(driver == 3)  {
        driverD.setEncoderValue(motor, 0);
    }
    
    if(driver == 4)  {
        driverE.setEncoderValue(motor, 0);
    }
}

void pollCurrents(){
  for (uint8_t i = 0; i < 5; i++) {
currents[i] = GetCurrentValue(i);}
}


void pollCurrent(){
    float voltage = -1.0;
    float current = -1.0;
    for (uint8_t i = 0; i < 5; i++) {
    M5.Display.fillRect(20, 40 + 35 * i, 300, 35, BLACK);
    if(i==0){
    voltage = driverA.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16;
    current =  driverA.getMotorCurrent() * 1000;}
    if(i==1){
    voltage = driverB.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16;
    current = driverB.getMotorCurrent() * 1000;}
    if(i==2){
    voltage = driverC.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16;
    current = driverC.getMotorCurrent()* 1000;}
        if(i==3){
    voltage = driverD.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16;
    current = driverD.getMotorCurrent()* 1000;    
    }
    if(i==4){
    voltage = driverE.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16;
    current = driverE.getMotorCurrent()* 1000;    
    }
    M5.Display.drawString("CH" + String(i) + ": " + String(voltage) + "V/" + String(current) + "mA",20, 40 + 35 * i);}

}



void pollAddress()
{
    
    uint8_t  address = driverA.getI2CAddress();

    M5.Display.fillRect(20, 40 + 35 * 4, 300, 35, BLACK);
    M5.Display.drawString(
        "Address: " + String(address), 20,40 + 35 * 4);

}




void StepMotor(int driver_cmd, int motor_cmd, int32_t enc_cmd){
 if (driver_cmd == 0){
  driverA.setMode(motor_cmd, POSITION_MODE);
  driverA.setEncoderValue(motor_cmd, 0);
  driverA.setPostionPIDMaxSpeed(motor_cmd, 127);
  driverA.setPositionPoint(motor_cmd, enc_cmd);}
  
  if (driver_cmd == 1){
  driverB.setMode(motor_cmd, POSITION_MODE);
  driverB.setEncoderValue(motor_cmd, 0);
  driverB.setPostionPIDMaxSpeed(motor_cmd, 127);
  driverB.setPositionPoint(motor_cmd, enc_cmd);}

  if (driver_cmd == 2){
  driverC.setMode(motor_cmd, POSITION_MODE);
  driverC.setEncoderValue(motor_cmd, 0);
  driverC.setPostionPIDMaxSpeed(motor_cmd, 127);
  driverC.setPositionPoint(motor_cmd, enc_cmd);}

  if (driver_cmd == 3){
  driverD.setMode(motor_cmd, POSITION_MODE);
  driverD.setEncoderValue(motor_cmd, 0);
  driverD.setPostionPIDMaxSpeed(motor_cmd, 127);
  driverD.setPositionPoint(motor_cmd, enc_cmd);}
}


void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    //Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
      //  Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void LogString(String string_to_log){
  if(log_mode !=1){
    return;
  }
  char log_file_buffer[current_log_file.length()];
  current_log_file.toCharArray(log_file_buffer, current_log_file.length()+1);
  String t_sec = String(millis());
  String line_to_add = t_sec+", "+string_to_log+"\n";
  char char_line_to_add[line_to_add.length()+1];
  line_to_add.toCharArray(char_line_to_add, line_to_add.length()+1);
  appendFile(SD, log_file_buffer, char_line_to_add);
  }


void LogParams(String active_cmd){
  if(log_mode !=1){
    return;
  }
  char log_file_buffer[current_log_file.length()];
  current_log_file.toCharArray(log_file_buffer, current_log_file.length()+1);
  String t_sec = String(millis());
  String params = "";
  for(uint8_t i = 0; i < 4; i++){
    for(uint8_t j = 0; j < 5; j++){
    params += String(encoder_readings[j][i]);
    params += ", ";
  }}
   for(uint8_t i = 0; i < 5; i++){
    params += String(currents[i]);
    params += ", ";
  }
   for(uint8_t i = 0; i < 5; i++){
    params += String(voltages[i]);
    params += ", ";
  }
  String line_to_add = t_sec+", "+active_cmd+", "+params+"\n";
  char char_line_to_add[line_to_add.length()+1];
  line_to_add.toCharArray(char_line_to_add, line_to_add.length()+1);
  appendFile(SD, log_file_buffer, char_line_to_add);
}

void LogCurrents(String active_cmd){
  if(log_mode !=1){
    return;
  }
  char log_file_buffer[current_log_file.length()];
  current_log_file.toCharArray(log_file_buffer, current_log_file.length()+1);
  String t_sec = String(millis());
  String params = "";
   for(uint8_t i = 0; i < 5; i++){
    params += String(currents[i]);
    params += ", ";
  }
  String line_to_add = t_sec+", "+active_cmd+", "+params+"\n";
  char char_line_to_add[line_to_add.length()+1];
  line_to_add.toCharArray(char_line_to_add, line_to_add.length()+1);
  appendFile(SD, log_file_buffer, char_line_to_add);

}


/*
void waitForSerialInput(char expectedInput) {
  while (true) {
    if (Serial.available() > 0) {
      char input = Serial.read();
      if (input == expectedInput) {
        break; // Exit the loop when the expected input is received
      }
    }
  }
}
*/

int findNextStep(const float steps[], int size, float currentPosition){
    int nextStepIndex = -1;
    float nextStep = std::numeric_limits<float>::max(); // Declare and initialize nextStep
    for (int i = 0; i < size; i++) {
        if (steps[i] > currentPosition && steps[i] < nextStep) {
            nextStep = steps[i]; // Update nextStep
            nextStepIndex = i;
        }
    }
    return nextStepIndex;
}

void serialEvent() {
  while (Serial.available()) {
    char input = Serial.read();
    if (input == 'S') { // 's' is the stop command
      Stop();
      Serial.println("All motors stopped: Restart when ready");
    }
  }
}
