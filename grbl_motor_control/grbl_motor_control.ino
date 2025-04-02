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

#0.004 is one step
#0.8 is one full turn of the motor
#1:50 is the ratio for the worm gear 
#0.25mm is the thread

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

//Petal channel mapping
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

Unit_RTC RTC;
rtc_time_type RTCtime;
rtc_date_type RTCdate;


int petal = 0; // startup petal is 0
int32_t encoder_readings[5][3] = {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}};
int32_t pos[5][3] = {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}};

void setup() {
    M5.begin();
    M5.Power.begin();

    M5.Lcd.setTextSize(2);
    //M5.Lcd.setFont(&FreeSansBold12pt7b);
    //M5.Lcd.setCursor(50, 100); // Set cursor position to (50, 100)
    //M5.Lcd.print("Hello, M5Stack!");
    M5.Lcd.setCursor(20, 20); 
    M5.Lcd.print("Actuator Control");
    M5.Lcd.setCursor(20, 50); 
    M5.Lcd.print("Active Petal is:");
    M5.Lcd.setCursor(20, 80);  
    M5.Lcd.print(petal);
    M5.Lcd.setCursor(20, 110); 
    M5.Lcd.print("Encoder Positions are: ");
    M5.Lcd.setCursor(20, 140);  
    M5.Lcd.print(pos[petal][0]);
    M5.Lcd.setCursor(100, 140);
    M5.Lcd.print(pos[petal][1]);
    M5.Lcd.setCursor(180, 140);
    M5.Lcd.print(pos[petal][2]);


    Wire.begin(21, 22);
    tca9548a.address(PaHub_I2C_ADDRESS);  // Set the I2C address

    tca9548a.selectChannel(0);
    _GRBL_0.Init(&Wire); // No return value to check
    _GRBL_0.setMode("distance");

    tca9548a.selectChannel(0);
    _GRBL_1.Init(&Wire); // No return value to check
    _GRBL_1.setMode("distance");

    tca9548a.selectChannel(1);
    _GRBL_2.Init(&Wire); // No return value to check
    _GRBL_2.setMode("distance");

    tca9548a.selectChannel(1);
    _GRBL_3.Init(&Wire); // No return value to check
    _GRBL_3.setMode("distance");

    tca9548a.selectChannel(2);
    _GRBL_4.Init(&Wire); // No return value to check
    _GRBL_4.setMode("distance");

    Serial.begin(115200);
    Serial.println("Setup complete");
}

void loop() {
    M5.update();
    String cmd = ""; // Declare cmd at the beginning of the loop
    if (Serial.available()) {
        // Read the serial input
        Serial.println("");
        cmd = Serial.readStringUntil('\n');       
    }

    // changing petal number
    if (cmd[0] == 'P'){
      petal = cmd[1]-'0';
    }

    // Query position
    if (cmd[0] == 'Q'){
      for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 3; ++j) {
            Serial.print(pos[i][j]);
        }
      }
      Serial.println(" ");
    }

    // Reset encoder positions
    if (cmd[0] == 'R'){
      int newValues[5][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
      for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 3; ++j) {
            pos[i][j] = newValues[i][j];
        }
      }
    } 

    // Set encoder positions e.g. "S 1,2,3 4,5,6 7,8,9 10,11,12 13,14,15"
    if (cmd[0] == 'S'){
      int newValues[5][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
      for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 3; ++j) {
            pos[i][j] = newValues[i][j];
        }
      }
    } 

    // Checking if Idle
    if (cmd[0] == 'I'){
      int  petal_cmd = cmd[1]-'0';
      bool isIdle = petalMap[petal_cmd].motor->readIdle();
      Serial.println(isIdle);
    }

    // reading encoders - need to save until we actually have encoders
    if (cmd[0] == 'E'){
      int  petal_cmd = cmd[1]-'0';
      int  motor_cmd = cmd[2]-'0';
      int32_t enc  = GetEncoderValue(petal_cmd, motor_cmd);
      Serial.println(enc);
    }

    if (cmd[0] == 'G'){ // move motor command
      //bool 
      move_all_motors(cmd,petal);
    }
    String data = Serial.readString();
    Serial.println(data);
    
}

void move_all_motors(String command, int petal) {
  char buffer[command.length() + 1];
  command.toCharArray(buffer, sizeof(buffer));
  tca9548a.selectChannel(petalMap[petal].pahub_address);
  petalMap[petal].motor->sendGcode(buffer);

  //wait for motor to become idle before finishing move
  while (!petalMap[petal].motor->readIdle()) {
  delay(10);
  }
}

void set_all_motors(int x = 0, int y = 0, int z = 0, int speed = 300, int petal = 0) {
  tca9548a.selectChannel(petalMap[petal].pahub_address);
  petalMap[petal].motor->setMotor(x,y,z,speed);
  //wait for motor to become idle before finishing move
  while (!petalMap[petal].motor->readIdle()) {
  delay(10);
  }
  //update pos variable
  pos[petal][0] += x;
  pos[petal][1] += y;
  pos[petal][2] += z;
}

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
