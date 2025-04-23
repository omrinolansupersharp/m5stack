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
#include <SPI.h>
#include <Preferences.h>
#include <math.h>
Preferences preferences;

// grbl motor address
Module_GRBL _GRBL_A = Module_GRBL(0x70);
// Encoder addresses
M5Module4EncoderMotor driverA ;
uint8_t mod_address_A = 0x24;


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
float initialPos[5][3] = {
    {0.000000, 0.000000, 0.000000},
    {0.000000, 0.000000, 0.000000},
    {0.000000, 0.000000, 0.000000},
    {0.000000, 0.000000, 0.000000},
    {0.000000, 0.000000, 0.000000}
};
float conv = 0.004; // conversion between GCode X1 and distance moved by screw
int wait = 50; //scren refresh time in ms
float steps_per_rev = 200; // motor steps per one motor revolution
float enc_ratio = (131072) / (50 * 32 );


void setup() {
    M5.begin();
    M5.Power.begin();
    M5.Speaker.mute();  
    Serial.begin(115200);
    //Save and read from non-volatile memory
    //savepos(initialPos);
    //readpos(pos);
    // Initialize SD card
    Wire.begin(21, 22);

    while (!driverA.begin(&Wire, mod_address_A, 21, 22)) {
        Serial.println("Driver A Init faild!");
        //M5.Display.drawString("Driver A Init faild!", 160, 7);
        delay(1000);
    }
    Serial.println("Encoder initialised");

    
    _GRBL_A.Init(&Wire,200,200,200,36000); // No return value to check
    _GRBL_A.setMode("distance");
    Serial.println("GRBL motor initialised");

    
    Serial.println("Setup complete");
}

void loop() {
    String cmd = ""; // Declare cmd at the beginning of the loop
    if (Serial.available()) {
        // Read the serial input
        cmd = Serial.readStringUntil('\n');

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
      for (uint8_t i = 0; i < 3; i++) { 
      enc_s[i] = Encoder(0,i);
      String enc_string = " Start Encoder: " + String(i) + ": " + String(enc_s[i]);
      Serial.println(enc_string);
      }
      //Serial.print("Start Encoder: ");
      //Serial.println(enc_s);
      //delay(100);
      move_all_motors(cmd);
      //delay(10000);
      for (uint8_t i = 0; i < 3; i++) { 
      enc_e[i] = Encoder(0,i);
      String enc_string = " End Encoder: " + String(i) + ": " + String(enc_e[i]-enc_s[i]);
      Serial.println(enc_string);
      }
      Serial.println("---------------");
    }
   
}   
}
// END OF LOOP

// FUNCTIONS

void move_all_motors(String command){
    Serial.println(command);
    //variables for a single move    
    int32_t start[3] = {0,0,0};
    int32_t init_pos[3] = {0,0,0};
    int32_t live_pos[3] = {0,0,0};
    int32_t steps[3] = {0,0,0};
    int32_t target[3] = {0,0,0};
    float damped_move[3] = {0,0,0};
    int32_t end[3] = {0,0,0};
    float diff[3] = {0,0,0};
    float damp = 0.8;
    bool allReached = false;

    damped_move[0] = extractValue(command, 'X') * damp;
    damped_move[1] = extractValue(command, 'Y') * damp;
    damped_move[2] = extractValue(command, 'Z') * damp;
    target[0] = extractValue(command, 'X') * enc_ratio;
    target[1] = extractValue(command, 'Y') * enc_ratio;
    target[2] = extractValue(command, 'Z') * enc_ratio;

    for (uint8_t i = 0; i < 3; i++) { 
      start[i] = Encoder(petal,i);
    }
    //make a new string command that will move 80% of the distance
    String first_command = "G1 X" + String(damped_move[0]) + " Y" + String(damped_move[1]) + " Z" + String(damped_move[2]) + " F1200";
    // iterate over the three axes to check for encoder counts while moving
    
    // Convert the command into a char array
    char buffer[first_command.length() + 1];
    first_command.toCharArray(buffer, sizeof(buffer));
    _GRBL_A.sendGcode(buffer); //- original move function
    _GRBL_A.waitIdle();
    
    /*
    //Now poll encoder values and compare them with the target position
    for (uint8_t i = 0; i < 3; i++) { 
      end[i] = Encoder(petal,i);
      steps[i] = end[i] - start[i];
      diff[i] = (target[i] - steps[i]) * damp ; 
      Serial.println(String(i) +" Encoder has moved: "+ steps[i]+ "out of a target: "+ target[i] + ", difference of: "+ diff[i]);
    }
    Serial.flush();
    String corr_command = "G1 X" + String(diff[0]/enc_ratio) + " Y" + String(diff[1]/enc_ratio) + " Z" + String(diff[2]/enc_ratio) + " F1200";
    Serial.println(corr_command);
    char corr_buffer[corr_command.length() + 1];
    corr_command.toCharArray(corr_buffer, sizeof(corr_buffer));
    delay(50);
    
    _GRBL_A.sendGcode(corr_buffer);
    _GRBL_A.waitIdle();
    */
    
    // loop to check encoder values and stop if reached targets
    for (uint8_t e = 1; e < 4 && !allReached ; e++) {
      Serial.print("iteration: ");
      Serial.println(e);
      Serial.println("-------------");
    for (uint8_t i = 0; i < 3; i++) { // loop to make more precise corrections to movement
      init_pos[i] = Encoder(petal,i);
      steps[i] = (target[i] - init_pos[i]) * damp / enc_ratio;
    }
    String corr_command = "G1 X" + String(steps[0]) + " Y" + String(steps[1]) + " Z" + String(steps[2]) + " F1200";
    Serial.println(corr_command);
    //while (!reached[0] || !reached[1] || !reached[2]) {
        char buffer[corr_command.length() + 1];
        corr_command.toCharArray(buffer, sizeof(buffer));
        _GRBL_A.sendGcode(buffer); //- original move function
        _GRBL_A.waitIdle();
    for (uint8_t i = 0; i < 3; i++) {
      live_pos[i] = Encoder(petal, i);
      Serial.print("livee pos is: ");
      Serial.println(live_pos[i]);
      Serial.print("close? - ");
      Serial.println(abs(abs(live_pos[i]) - abs(target[i])));
    if (abs(abs(live_pos[i]) - abs(target[i])) <= 10) {
    //reached[i] = true;
    allReached = true;
    Serial.println("Enc Value reached");
    break;
    }
    
    }
    }
       
}


float extractValue(String command, char axis) {
  int startIndex = command.indexOf(axis);
  if (startIndex == -1) {
    return 0.0; // Return 0 if the axis is not found
  }
  int endIndex = command.indexOf(' ', startIndex);
  if (endIndex == -1) {
    endIndex = command.length();
  }
  String valueStr = command.substring(startIndex + 1, endIndex);
  float motor_distance = valueStr.toFloat();
  
  return motor_distance;
}

int32_t Encoder(int petal, int motor) { // get encoder value
 encoder_readings[0][motor] = driverA.getEncoderValue(motor);
 return encoder_readings[petal][motor];
}
