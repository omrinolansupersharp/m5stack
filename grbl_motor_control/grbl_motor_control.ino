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
unsigned long previousMillis = 0;
const long interval = 10; // Interval to check motor status (in milliseconds)
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
int wait = 300; //scren refresh time in ms

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
    // Write to file
    Serial.println("Writing to file...");
    writeFile(SD, "/hello.txt", "Hello ");
    Serial.println("Reading from file...");
    readFile(SD, "/hello.txt");
    Serial.println("Appending to file...");
    appendFile(SD, "/hello.txt", "World!\n");
    Serial.println("Reading from file again...");
    readFile(SD, "/hello.txt");


    // Save the pos array to the SD card
    //savePosArray(SD, "/pos.txt");
    // Load the pos array from the SD card
    loadPosArray(SD, "/pos.txt");

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
    update(petal,pos); // function to update screen

    String cmd = ""; // Declare cmd at the beginning of the loop
    if (Serial.available()) {
        // Read the serial input
        Serial.println("");
        cmd = Serial.readStringUntil('\n');
        Serial.println(cmd); 
        delay(10);      
    

    // changing petal number
    if (cmd[0] == 'P'){
      petal = cmd[1]-'0';
      Serial.print("Active Petal changed to: ");
      Serial.println(petal);
    }

    // Query position - returns the three encoder values of the active petal
    if (cmd[0] == 'Q'){
      //Serial.println("The current position is: ");
      for (int j = 0; j < 3; ++j) {
            Serial.print(pos[petal][j], 6);
            if (j < 2) Serial.print(",");
    }
    }

    // Reset encoder positions to 0
    if (cmd[0] == 'R'){
      int newValues[5][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
      for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 3; ++j) {
            pos[i][j] = newValues[i][j];
        }
      }
      savePosArray(SD, "/pos.txt");
    } 

    // Set a specific encoder position e.g. "S(petal)(motor)(position float)
    if (cmd[0] == 'S'){
      int petal_cmd = cmd[1]-'0';
      int motor_cmd = cmd[2]-'0';
      String valueString = cmd.substring(3);
      float newValue = valueString.toFloat();
      pos[petal_cmd][motor_cmd] = newValue;
      savePosArray(SD, "/pos.txt");
    } 

    // Checking if Idle
    if (cmd[0] == 'I'){
      int  petal_cmd = petal;//(cmd[1]-'0');
      if (petal_cmd >= 0 && petal_cmd < 5) {
        auto idleStatus = petalMap[petal_cmd].motor->readIdle();
        // Check if idleStatus is a boolean
        if (idleStatus == 0 || idleStatus == 1) {
            bool isIdle = (idleStatus == 0); // Convert to boolean
            Serial.print("Motor idle status: ");
            Serial.println(isIdle); // Debugging statement
        } else {
            Serial.println("Invalid return type from readIdle()"); // Error handling
        }
        }
         else {
            Serial.println("Invalid petal command"); // Debugging statement
        }
    
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
      Serial.println("Motion finished");
    }

    if (cmd[0] == 'M'){ // move motor command
      //bool 
      //set_all_motors(,petal);
    }
    if (cmd[0] == 'V'){ // Save positions to sd card
      savePosArray(SD, "/pos.txt");
    }
    
    
}   
}
// END OF LOOP

// FUUNCTIONS

void move_all_motors(String command, int petal) {
    // Convert the command into a char array
    char buffer[command.length() + 1];
    command.toCharArray(buffer, sizeof(buffer));
    Serial.print("Command buffer: ");
    Serial.println(buffer); // Debugging statement
    // extract the position variable from the G command
    float x = extractValue(command, 'X');
    float y = extractValue(command, 'Y');
    float z = extractValue(command, 'Z');
    int f = extractValue(command, 'F');
    // Select the correct PAHub address for the selected petal
    tca9548a.selectChannel(petalMap[petal].pahub_address);
    Serial.print("Selected PAHub channel: ");
    Serial.println(petalMap[petal].pahub_address); // Debugging statement

    // Check if the motor is connected by calling the isMotorConnected function
    if (isMotorConnected(petalMap[petal].motor)) {
        // Send the command to the selected petal motor
        petalMap[petal].motor->sendGcode(buffer);
        Serial.println("G-code command sent"); // Debugging statement
        //while (!petalMap[petal].motor->readIdle()) {
          /*
        // Wait for motor to become idle before finishing move
        // we don't wanna do this because we want to return to the loop
          unsigned long currentMillis = millis();        
          if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            Serial.println("Motor is not idle, waiting..."); // Debugging statement
          }  
          // Check if timeout period has elapsed
          if (currentMillis - startTime >= (timeout*30*(x+y+z)/f)){
            Serial.println("Timeout reached, exiting loop.");
            break;
          }
          */
          //delay(100);
        //}
    } else {
        Serial.println("Motor is not connected"); // Debugging statement
    } 
    //update pos variable
    pos[petal][0] += (x * conv);
    pos[petal][1] += (y * conv);
    pos[petal][2] += (z * conv);
    savePosArray(SD, "/pos.txt");
}

/*
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

bool isMotorConnected(Module_GRBL* motor) { // this is currently not working - need to figure out what is returned by the status query
    const int maxAttempts = 3;
    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        // Send a simple command to check connection
        char checkCommand[] = "?";
        motor->sendGcode(checkCommand);
        delay(100); // Wait for response

        // Read the response from the motor
        String response = motor->readLine();
        Serial.print("Motor response: ");
        Serial.println(response); // Debugging statement

        // Check if the response contains "ok" and does not contain corrupted data
        if (response.length() > 0 && response.indexOf("�") == -1 ) { //&& response.indexOf("ok") != -1
            return true; // Valid response indicates a connection
        }
    }
    return false; // After maxAttempts, conclude the motor is not connected
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
  return valueStr.toFloat();
}

void savepos(float pos[5][3]) {
  // Store modified array in NVS
  preferences.begin("my-app", false); // Initialize preferences with read/write mode
  preferences.putBytes("Actpos", (float*)pos, sizeof(float) * 5 * 3);
  preferences.end(); // Close preferences
}

void readpos(float pos[5][3]) {
  // Read array from NVS
  preferences.begin("my-app", true); // Initialize preferences with read-only mode
  preferences.getBytes("Actpos", (float*)pos, sizeof(float) * 5 * 3);
  preferences.end(); // Close preferences
}

void update(int petal,float pos[5][3]){
  delay(wait);
  // Clear the area where the values are printed
  M5.Lcd.fillRect(20, 80, 200, 20, BLACK); // Clear petal value
  M5.Lcd.fillRect(20, 140, 60, 20, BLACK); // Clear pos[petal][0]
  M5.Lcd.fillRect(100, 140, 60, 20, BLACK); // Clear pos[petal][1]
  M5.Lcd.fillRect(180, 140, 60, 20, BLACK); // Clear pos[petal][2]

  // Print updated values
  M5.Lcd.setCursor(20, 80);  
  M5.Lcd.print(petal);
  M5.Lcd.setCursor(20, 140);  
  M5.Lcd.print(pos[petal][0],4);
  M5.Lcd.setCursor(100, 140);
  M5.Lcd.print(pos[petal][1],4);
  M5.Lcd.setCursor(180, 140);
  M5.Lcd.print(pos[petal][2],4);

}

// read file from sd card
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

// write file to sd card
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

//change a file on the sd card
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

// Function to convert the pos array to a string
String posArrayToString(float array[5][3]) {
    String result = "";
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            result += String(array[i][j], 6);
            if (j < 2) result += ",";
        }
        result += "\n";
    }
    return result;
}

// Function to convert a string to the pos array
void stringToPosArray(String data, float array[5][3]) {
    int row = 0;
    int col = 0;
    char *token = strtok((char*)data.c_str(), ",\n");
    while (token != NULL) {
        array[row][col] = atof(token);
        col++;
        if (col == 3) {
            col = 0;
            row++;
        }
        token = strtok(NULL, ",\n");
    }
}

// Save the pos array to the SD card
void savePosArray(fs::FS &fs, const char * path) {
    String data = posArrayToString(pos);
    writeFile(fs, path, data.c_str());
}

// Read the pos array from the SD card
void loadPosArray(fs::FS &fs, const char * path) {
    File file = fs.open(path);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    String data = "";
    while (file.available()) {
        data += (char)file.read();
    }
    file.close();

    stringToPosArray(data, pos);
}







