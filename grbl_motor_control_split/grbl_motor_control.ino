/*
Documentation for GRBL stack is at https://docs.m5stack.com/en/module/grbl13.2
mOTOR XYZ = ABC

MOVE MOTOR COMMAND: G1X(mm)Y(mm)Z(mm)F(mm/min (velocity))
stepper motor has 200 steps per revolution  
250 steps per mm of movement 
1 mm movement is 1.25 revolutions / 1 turn = 0.8mm

E.g. to do 1 turn on all motors in 1 minute: G1X0.8Y0.8Z0.8F0.8
E.g. to do 1 turn on 1 motor in 1 minute: G1X0.8F0.8

Limits on F speed when moving 1 motor:
max speed 300
min speed 1 (anything less than this moves at the same velocity)

Motion times for F commands:(G1 X0 Y0.8 Z0 F(a))
F1 - 20 s
F2 - 20 s
F3 - 20 s
F5 - 12 s
F10 - 6.4 s
F20 - 3.2 s
F100 - 0.7 s

current testing setup on petal 2 
Motor A up direction is +
Motor B up direction is +
Motoc C up direction is -

Example command:
G1X0.8Y0Z0F20 - turns the x motor one revolution in the positive direction in 3.2 seconds
G1X-0.8Y0Z0F20 - turns the x motor one revolution in the positive direction in 3.2 seconds

#0.004 is one step
#0.8 (mm) is one full turn of the motor
#1:50 is the ratio for the worm gear 
#0.25mm is the thread
#ratio between motor turns and height of actuator is:
actuator height = motor distance  * gear ratio * thread pitch 
actuator height = motor distance  * 0.02 * 0.25mm
actuator height = motor distance * 0.005mm
e.g. for 1mm motor movement:
actuator height change = 0.004mm
#200 steps in one full motor turn
actuator height change per one step = 0.004 *0.005 = 0.00002mm = 20nm

*/
#include <M5Stack.h>
#include "Module_GRBL_13.2.h"
#include "ClosedCube_TCA9548A.h"
#include "M5Module4EncoderMotor.h"
#include "Unit_RTC.h"
#include "SD.h"
#include <SPI.h>
#include <Preferences.h>
Preferences preferences;


#define PaHub_I2C_ADDRESS (0x77)
ClosedCube::Wired::TCA9548A tca9548a;
// here we manually define the i2c and pahub addresses for each of the motors
// if there is any hardware change then we need to adapt this too
Module_GRBL _GRBL_0 = Module_GRBL(0x70); // defined by switch 4 in the back of the board
Module_GRBL _GRBL_1 = Module_GRBL(0x71);
Module_GRBL _GRBL_2 = Module_GRBL(0x70);
Module_GRBL _GRBL_3 = Module_GRBL(0x71);
Module_GRBL _GRBL_4 = Module_GRBL(0x71);
int pa_hub_address_0 = 0;
int pa_hub_address_1 = 0;
int pa_hub_address_2 = 1;
int pa_hub_address_3 = 1;
int pa_hub_address_4 = 2;
// Encoder addresses
M5Module4EncoderMotor driver0 ;
M5Module4EncoderMotor driver1 ;
M5Module4EncoderMotor driver2 ;
M5Module4EncoderMotor driver3 ;
M5Module4EncoderMotor driver4 ;
M5Module4EncoderMotor drivers[5] = {driver0, driver1, driver2, driver3, driver4};
uint8_t mod_address_0 = 0x1A;
uint8_t mod_address_1 = 0x2A;
uint8_t mod_address_2 = 0x3A;
uint8_t mod_address_3 = 0x4A;
uint8_t mod_address_4 = 0x5A;

//Petal channel mapping
  struct PetalMapping {
        int petal;
        int pahub_address;
        Module_GRBL* motor;
        M5Module4EncoderMotor* encoder;
    };
    PetalMapping petalMap[] = {
        {0,0, &_GRBL_0, &driver0},
        {1,0, &_GRBL_1, &driver1},
        {2,1, &_GRBL_2, &driver2},
        {3,1, &_GRBL_3, &driver3},
        {4,2, &_GRBL_4, &driver4}
    };

Unit_RTC RTC;
rtc_time_type RTCtime;
rtc_date_type RTCdate;
unsigned long previousMillis = 0;
const long interval = 50; // Interval to check motor status (in milliseconds)
unsigned long timeout = 5000; // Timeout period (in milliseconds)
unsigned long startTime = millis();


int petal = 0; // startup petal is 0
int32_t encoder_readings[5][3] = {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}};
float pos[5][3];
float step_pos[5][3];
int32_t enc_counts[5][3];
float enc_pos[5][3];
float initialPos[5][3];

float steps_per_rev = 200; // motor steps per one motor revolution
float screw_pitch = 0.25; //(mm)
int microsteps = 32; //microsteps per full step
int gear_ratio = 50; // 50:1 for motor movement to screw turn
int32_t encoder_counts_per_rev = 131072;
float encoder_counts_over_motor_command = (encoder_counts_per_rev) / (gear_ratio * microsteps ); //ratio betweeen encoder count and g code
float extension_over_encoder_counts = screw_pitch / (encoder_counts_per_rev);
float motor_command_over_extension = (microsteps * gear_ratio)  / screw_pitch;

void setup() {
    M5.begin();
    M5.Power.begin();
    M5.Speaker.mute();  
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(20, 20); 
    M5.Lcd.print("Hibiscus Actuator Control");
    M5.Lcd.setCursor(20, 50); 
    M5.Lcd.print("Active Petal is:");
    M5.Lcd.setCursor(20, 80);  
    M5.Lcd.print(petal);
    M5.Lcd.setCursor(20, 110); 
    M5.Lcd.print("Encoder Positions are: ");

    //Save and read from non-volatile memory
    //savepos(initialPos);
    //readpos(pos);
    

    // Initialize SD card
    Serial.println("Initializing SD card...");
    if (!SD.begin(4, SPI, 10000000)) {
        M5.Lcd.println("Card failed, or not present");
        Serial.println("Card Mount Failed");
        // Endlessly loop if the card cannot be initialized.
        while (1);
    }
    Serial.println("TF card initialized.");
    // Check SD card size
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);


    // Save the pos array to the SD card
    //savePosArray(SD, "/pos.txt");
    // Load the pos array from the SD card
    loadPosArray(SD, "/pos.txt");
    Wire.begin(21, 22);
    tca9548a.address(PaHub_I2C_ADDRESS);  // Set the I2C address

    tca9548a.selectChannel(0);
    delay(1000);
    _GRBL_0.Init(&Wire,200,200,200,36000); // No return value to check
    _GRBL_0.setMode("distance");
    while (!driver0.begin(&Wire, mod_address_0,21,22)) {
        Serial.println("Encoder A Init faild!");
        delay(1000);
    }
    tca9548a.selectChannel(0);
    delay(1000);
    _GRBL_1.Init(&Wire,200,200,200,36000); // No return value to check
    _GRBL_1.setMode("distance");
    while (!driver1.begin(&Wire, mod_address_1,21,22)) {
        Serial.println("Encoder B Init faild!");
        delay(1000);
    }
    
    tca9548a.selectChannel(1);
    delay(1000);
    Wire.begin(21, 22);
    _GRBL_2.Init(&Wire,200,200,200,36000); // No return value to check
    _GRBL_2.setMode("distance");
    while (!driver2.begin(&Wire, mod_address_2,21,22)) {
        Serial.println("Encoder C Init faild!");
        delay(1000);
    }
    tca9548a.selectChannel(1);
    delay(1000);
    _GRBL_3.Init(&Wire,200,200,200,36000); // No return value to check
    _GRBL_3.setMode("distance");
    while (!driver3.begin(&Wire, mod_address_3,21,22)) {
        Serial.println("Encoder D Init faild!");
        delay(1000);
    }
    tca9548a.selectChannel(2);
    delay(1000);
    Wire.begin(21, 22);
    _GRBL_4.Init(&Wire,200,200,200,36000); // No return value to check
    _GRBL_4.setMode("distance");
    while (!driver4.begin(&Wire, mod_address_4,21,22)) {
        Serial.println("Encoder E Init faild!");
        delay(1000);
    }
    
    Serial.begin(115200);
    Serial.println("Setup complete");
}
void loop() {
    update(petal,enc_pos);
    String cmd = ""; // Declare cmd at the beginning of the loop
    if (Serial.available()) {
        // Read the serial input
        cmd = Serial.readStringUntil('\n');
        //delay(50);
        //Serial.print("Command recieved: ");
        //Serial.println(cmd); 
        //delay(50);      
        
    // changing petal number
    if (cmd[0] == 'P'){
      petal = cmd[1]-'0';
      tca9548a.selectChannel(petalMap[petal].pahub_address);
      savePosArray(SD, "/pos.txt");
      //Serial.print("Active Petal changed to: ");
      //Serial.println(petal);
    }

    // Query position - returns the three encoder values of the active petal
    if (cmd[0] == 'Q'){
      //Serial.println("The current position is: ");
      //Serial.println(enc_pos[petal][0]);
      String positionString = "";
      for (int j = 0; j < 3; ++j) {
          enc_counts[petal][j] = Encoder(petal,j);
          //float enc_pos[petal][j] = enc_count[petal][j] * conv / enc_ratio;// - we are trying to do this in the function already
          // positionString += String(pos[petal][j], 6);
          positionString += String(enc_pos[petal][j],6);
          if (j < 2) positionString += ",";
      }
      Serial.println(positionString);
      //Serial.println(positionString);
      Serial.flush(); 
    }

    // Reset encoder positions to 0
    if (cmd[0] == 'R'){
      //int newValues[5][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
      for (int i = 0; i < 5; ++i) {
        tca9548a.selectChannel(petalMap[i].pahub_address);
        for (int j = 0; j < 3; ++j) {
            petalMap[i].encoder->setEncoderValue(j,0);
            //pos[i][j] = newValues[i][j];
        }
      }
      //savePosArray(SD, "/pos.txt");
    }  

    // Set a specific encoder position e.g. "S(petal)(motor)(position float)
    if (cmd[0] == 'S'){
      int petal_cmd = cmd[1]-'0';
      int motor_cmd = cmd[2]-'0';
      String valueString = cmd.substring(3);
      float newValue = valueString.toFloat();
      //add setencodervalue here
      //pos[petal_cmd][motor_cmd] = newValue;
      savePosArray(SD, "/pos.txt");
    } 

    // reading encoders - need to save until we actually have encoders
    if (cmd[0] == 'E'){
      int  petal_cmd = cmd[1]-'0';
      int  motor_cmd = cmd[2]-'0';
      int32_t enc  = Encoder(petal_cmd, motor_cmd);
      Serial.println(enc);
    }

    if (cmd[0] == 'G'){ // move motor command - used for encoder counting
      int32_t enc_s[3] = {0,0,0};
      int32_t enc_e[3] = {0,0,0};
      //Serial.println("Start: ");
      //Serial.print("Start Encoder: ");
      //Serial.println(enc_s);
      //delay(100);
      move_all_motors(cmd,petal);
      //delay(1000);
      //Serial.println("End:");
     // Double encoder count is added to correct for the wierd bug where:
     // encoder A seems to not update on the first read 
     
      // after a move
      for (uint8_t i = 0; i < 3; i++) { 
      enc_e[i] = Encoder(petal,i);
      enc_e[i] = Encoder(petal,i);
      }
      //Serial.println("-------done--------");
      //print in a serial command when finished move
      Serial.println("D");
      Serial.flush();
    }


    if (cmd[0] == 'V'){ // Save positions to sd card
      savePosArray(SD, "/pos.txt");
    }
   
}   
}
// END OF LOOP

// FUNCTIONS
