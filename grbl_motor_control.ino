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
max speed 200
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

*/
#include <M5Stack.h>
#include "Module_GRBL_13.2.h"
#include "ClosedCube_TCA9548A.h"
#include "M5Module4EncoderMotor.h"
#include "Unit_RTC.h"

#define PaHub_I2C_ADDRESS (0x77)
ClosedCube::Wired::TCA9548A tca9548a;
// here we manually define the i2c and pahub addresses for each of the motors
// if there is any hardware change then we need to adapt this too
Module_GRBL _GRBL_0 = Module_GRBL(0x70); // defined by switch 4 in the back of the board
Module_GRBL _GRBL_1 = Module_GRBL(0x71);
Module_GRBL _GRBL_2 = Module_GRBL(0x71);
Module_GRBL _GRBL_3 = Module_GRBL(0x70);
Module_GRBL _GRBL_4 = Module_GRBL(0x71);
int pa_hub_address_0 = 0;
int pa_hub_address_1 = 0;
int pa_hub_address_2 = 1;
int pa_hub_address_3 = 1;
int pa_hub_address_4 = 2;
// Encoder addresses
M5Module4EncoderMotor driverA ;
M5Module4EncoderMotor driverB ;
M5Module4EncoderMotor driverC ;
M5Module4EncoderMotor driverD ;
M5Module4EncoderMotor driverE ;
M5Module4EncoderMotor drivers[5] = {driverA, driverB, driverC, driverD, driverE};
uint8_t mod_address_A = 0x1A;
uint8_t mod_address_B = 0x2A;
uint8_t mod_address_C = 0x3A;
uint8_t mod_address_D = 0x4A;
uint8_t mod_address_E = 0x5A;

Unit_RTC RTC;
rtc_time_type RTCtime;
rtc_date_type RTCdate;

bool wasIdle = true;
long notIdleStartTime = 0;
int delay_new_move = 500;
int petal = 0; // startup petal is 0
int32_t encoder_readings[5][3] = {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}};
//tca9548a.selectChannel(0);



void setup() {
    M5.begin();
    M5.Power.begin();
    M5.Display.begin();
    M5.Display.setTextColor(WHITE);
    M5.Display.setTextDatum(top_center);
    M5.Display.setFont(&fonts::FreeSansBold12pt7b);
    M5.Display.setTextSize(1);


    Wire.begin(21, 22);
    tca9548a.address(PaHub_I2C_ADDRESS);  // Set the I2C address

    _GRBL_0.Init(&Wire); // No return value to check
    _GRBL_0.setMode("distance");

    _GRBL_1.Init(&Wire); // No return value to check
    _GRBL_1.setMode("distance");

    _GRBL_2.Init(&Wire); // No return value to check
    _GRBL_2.setMode("distance");

    _GRBL_3.Init(&Wire); // No return value to check
    _GRBL_3.setMode("distance");

    _GRBL_4.Init(&Wire); // No return value to check
    _GRBL_4.setMode("distance");

    Serial.begin(115200);
    Serial.println("Setup complete");
}

void loop() {

    String cmd = ""; // Declare cmd at the beginning of the loop
    if (Serial.available()) {
        // Read the serial input
        Serial.println("");
        cmd = Serial.readStringUntil('\n');
        Serial.print("Active Petal is: ");
        Serial.println(petal);
        Serial.println("cmd: " + cmd);
        //move_all_motors(cmd);
    }
    /*
    long startTime = millis(); - maybe not working - would need to adapt for currently selected motor
    bool isIdle = _GRBL_0.readIdle();
    if (!isIdle && wasIdle) {
        // System just became not idle
        notIdleStartTime = millis();
        wasIdle = false;
    } else if (isIdle && !wasIdle) {
        // System just became idle
        long notIdleEndTime = millis();
        long notIdleDuration = notIdleEndTime - notIdleStartTime;
        Serial.print("Time until idle: ");
        Serial.println(notIdleDuration);
        wasIdle = true;
    }

    if (isIdle) {
        delay(delay_new_move); // Delay for 500 milliseconds
    }
   */
    // changing petal number
    if (cmd[0] == 'P'){
      petal = cmd[1]-'0';
    }
    if (cmd[0] == 'E'){
      int  petal_cmd = cmd[1]-'0';
      int  motor_cmd = cmd[2]-'0';
      int32_t enc  = GetEncoderValue(petal_cmd, motor_cmd);
      Serial.println(enc);
    }


    // Motor movement commands
    // int speed = 10; variable to change to change the speed of execution
    if (cmd[0] == 'a'){
      move_all_motors("G1X-0.8Y0Z0F100", petal); // move x motor one turn to the left in 3.2 s
    Serial.println("function completed");
    }
    if (cmd[0] == 'q'){
      move_all_motors("G1X0.8Y0Z0F100",petal); // move x motor one turn to the right in 3.2 s
    }
    if (cmd[0] == 'w'){
      move_all_motors("G1X0Y0.8Z0F100",petal); // move y motor one turn upwards in 3.2 s
    }
    if (cmd[0] == 's'){
      move_all_motors("G1X0Y-0.8Z0F100",petal); // move y motor one turn downwards in 3.2 s
    }
    if (cmd[0] == 'e'){
      move_all_motors("G1X0Y0Z-0.8F100",petal); // move z motor one turn upwards in 3.2 s
    }
    if (cmd[0] == 'd'){
      move_all_motors("G1X0Y0Z0.8F100",petal); // move z motor one turn downwards in 3.2 s
    }
    if (cmd[0] == 'r'){
      move_all_motors("G1X0.8Y0.8Z-0.8F100",petal); // move all motors one turn upwards in 3.2 s
    }
    if (cmd[0] == 'f'){
      move_all_motors("G1X-0.8Y-0.8Z0.8F100",petal); // move all motors one turn downwards in 3.2 s
    }
    if (cmd[0] == 'G'){
      move_all_motors(cmd,petal);
    }

}

void move_all_motors(String command, int petal) {
  char buffer[command.length() + 1];
  command.toCharArray(buffer, sizeof(buffer));
  struct PetalMapping {
        int petal;
        int pahub_address;
        Module_GRBL* motor;
    };
    PetalMapping petalMap[] = {
        {0,0, &_GRBL_0},
        {1,0, &_GRBL_1},
        {2,1, &_GRBL_2},
        {3,1, &_GRBL_3},
        {4,2, &_GRBL_4}
    };
    if (petal >= 0 && petal < 5) {
        tca9548a.selectChannel(petalMap[petal].pahub_address);
        petalMap[petal].motor->sendGcode(buffer);
    } else {
        Serial.println("Invalid petal number");
    }
}

/*
  if(petal == 0){
    tca9548a.selectChannel(pa_hub_address_0);
    _GRBL_0.sendGcode(buffer); // No return value to check
    Serial.println("Command sent");
    //while (!_GRBL_0.readIdle()) { // Wait for the move to complete - not working 
    //    delay(10);// Optionally, you can add a small delay here to avoid busy-waiting
    //}
    Serial.println("move completed");
  }
  if(petal == 1){
    tca9548a.selectChannel(pa_hub_address_1);
    _GRBL_1.sendGcode(buffer); // No return value to check
    //while (!_GRBL_1.readIdle()) { // Wait for the move to complete
    //    delay(10);// Optionally, you can add a small delay here to avoid busy-waiting
    //}
  }
  if(petal == 2){
    tca9548a.selectChannel(pa_hub_address_2);
    _GRBL_2.sendGcode(buffer); // No return value to check
    //while (!_GRBL_2.readIdle()) { // Wait for the move to complete
    //    delay(10);// Optionally, you can add a small delay here to avoid busy-waiting
    //}
  }
  if(petal == 3){
    tca9548a.selectChannel(pa_hub_address_3);
    _GRBL_3.sendGcode(buffer); // No return value to check
    //while (!_GRBL_3.readIdle()) { // Wait for the move to complete
    //    delay(10);// Optionally, you can add a small delay here to avoid busy-waiting
    //}
  }
  if(petal == 4){
    tca9548a.selectChannel(pa_hub_address_4);
    _GRBL_4.sendGcode(buffer); // No return value to check
    //while (!_GRBL_4.readIdle()) { // Wait for the move to complete
    //    delay(10);// Optionally, you can add a small delay here to avoid busy-waiting
    //}
  }  
}
*/

int32_t GetEncoderValue(int petal, int motor){ // get encoder value
  if (petal == 0){
  encoder_readings[0][motor] = driverA.getEncoderValue(motor);
  }
  if (petal == 1){
  encoder_readings[1][motor] = driverB.getEncoderValue(motor);
  }
  if (petal == 2){
  encoder_readings[2][motor] = driverC.getEncoderValue(motor);
  }
  if (petal == 3){
  encoder_readings[3][motor] = driverD.getEncoderValue(motor);
  }
    if (petal == 4){
  encoder_readings[4][motor] = driverE.getEncoderValue(motor);
  }
    return encoder_readings[petal][motor];
}