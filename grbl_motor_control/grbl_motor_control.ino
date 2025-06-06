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

void move_all_motors(String command, int petal){
    int32_t start[3] = {0,0,0};
    int32_t init_pos[3] = {0,0,0};
    int32_t live_pos[3] = {0,0,0};
    float steps[3] = {0.0,0.0,0.0};
    int32_t target[3] = {0,0,0};
    float damped_move[3] = {0,0,0};
    int32_t end[3] = {0,0,0};
    float diff[3] = {0,0,0};
    float damp = 1.0;
    int reached[3] = {0,0,0};
    int sum = 0;
    bool allReached = false;
    for (uint8_t i = 0; i < 3; i++) {
      start[i] = Encoder(petal,i);
    }
    damped_move[0] = extractValue(command, 'X')  * damp / extension_over_encoder_counts;
    damped_move[1] = extractValue(command, 'Y')  * damp / extension_over_encoder_counts;
    damped_move[2] = extractValue(command, 'Z')  * damp / extension_over_encoder_counts;
    float f = extractValue(command, 'F');
    target[0] = extractValue(command, 'X') / extension_over_encoder_counts + start[0];
    target[1] = extractValue(command, 'Y') / extension_over_encoder_counts + start[1];
    target[2] = extractValue(command, 'Z') / extension_over_encoder_counts + start[2];

    // loop to check encoder values and stop if reached targets
    for (uint8_t e = 1; e < 3 && !allReached ; e++) {
      //Serial.print("iteration: ");
      //Serial.println(e);
      //Serial.println("-------------");
    for (uint8_t i = 0; i < 3; i++) { // loop to make more precise corrections to movement
      init_pos[i] = Encoder(petal,i);
      if (reached[i] = 0){
        steps[i] = 0;
      }
      else {
        steps[i] = (target[i] - init_pos[i]) * damp / encoder_counts_over_motor_command;
      }
    }
    String corr_command = "G1 X" + String(steps[0]) + " Y" + String(steps[1]) + " Z" + String(steps[2]) + " F" + String(f);
    char corr_buffer[corr_command.length() + 2];
    corr_command.toCharArray(corr_buffer, sizeof(corr_buffer));
    //Serial.println(corr_buffer);
    tca9548a.selectChannel(petalMap[petal].pahub_address);
    petalMap[petal].motor->sendGcode(corr_buffer); 
    petalMap[petal].motor->waitIdle();
    int32_t dummy = Encoder(0,0);
    for (uint8_t i = 0; i < 3; i++) {
      live_pos[i] = Encoder(petal, i);
      //String printstring = "Live posss: " + String(live_pos[i]) + ", target is: " + String(target[i]);
      //Serial.println(printstring);
      if (abs(live_pos[i] - target[i]) <= 10) {
      reached[i] = 1;
      //String printstring =  String(i) + " motor reached target";
      //Serial.println(printstring);
      // now need to add it where it keeps on looping round until they have all been done
      } 
    }
    // here check if all of the motors have reached the endpoints 
    sum = reached[0] + reached[1] + reached[2];
    if (sum >= 3){
    allReached = true;
    //Serial.println("Enc Value reached");
    break;
    }
    }
   
}


int32_t Encoder(int petal, int motor){ // get encoder value
  //tca9548a.selectChannel(petalMap[petal].pahub_address);
  //enc_counts[0][motor] = petalMap[petal].encoder->getEncoderValue(motor);
  enc_counts[petal][motor] = petalMap[petal].encoder->getEncoderValue(motor);
  enc_pos[petal][motor] = enc_counts[petal][motor] * extension_over_encoder_counts;
  //String printstring = "Enc counts are: " + String(enc_counts[petal][motor]) + " leading to a position of: " + String(enc_pos[petal][motor]);
  //Serial.println(printstring);
  
  //agnostic encoder readings
  //tca9548a.selectChannel(petalMap[petal].pahub_address);
  //enc_counts[petal][motor] = petalMap[petal].encoder->getEncoderValue(motor);
  //enc_pos[petal][motor] = enc_counts[petal][motor] * enc_to_extension_ratio;
  return enc_counts[0][motor];
}


bool isMotorConnected(Module_GRBL* motor) {// this is currently not working - need to figure out what is returned by the status query
    const int maxAttempts = 3;
    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        // Send a simple command to check connection
        char checkCommand[] = "?";
        motor->sendGcode(checkCommand);
        delay(50); // Wait for response

        // Read the response from the motor
        String response = motor->readLine();
        //Serial.print("Motor response: ");
        //Serial.println(response); // Debugging statement

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

void update(int petal,float enc_pos[5][3]){
  // Clear the area where the values are printed
  M5.Lcd.fillRect(20, 80, 200, 20, BLACK); // Clear petal value
  M5.Lcd.fillRect(20, 140, 60, 20, BLACK); // Clear pos[petal][0]
  M5.Lcd.fillRect(100, 140, 60, 20, BLACK); // Clear pos[petal][1]
  M5.Lcd.fillRect(180, 140, 60, 20, BLACK); // Clear pos[petal][2]

  // Print updated values
  M5.Lcd.setCursor(20, 80);  
  M5.Lcd.print(petal);
  M5.Lcd.setCursor(20, 140);  
  M5.Lcd.print(enc_pos[petal][0],4);
  M5.Lcd.setCursor(120, 140);
  M5.Lcd.print(enc_pos[petal][1],4);
  M5.Lcd.setCursor(220, 140);
  M5.Lcd.print(enc_pos[petal][2],4);
  delay(20);
}

// read file from sd card
void readFile(fs::FS &fs, const char * path){ 
    //Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        //Serial.println("Failed to open file for reading");
        return;
    }

    //Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

// write file to sd card
void writeFile(fs::FS &fs, const char * path, const char * message){
    //Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        //Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        //Serial.println("File written");
    } else {
        //Serial.println("Write failed");
    }
    file.close();
}

//change a file on the sd card
void appendFile(fs::FS &fs, const char * path, const char * message){
    //Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        //Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
      //  Serial.println("Message appended");
    } else {
        //Serial.println("Append failed");
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
    String data = posArrayToString(enc_pos);
    writeFile(fs, path, data.c_str());
}

// Read the pos array from the SD card
void loadPosArray(fs::FS &fs, const char * path) {
    File file = fs.open(path);
    if (!file) {
        //Serial.println("Failed to open file for reading");
        return;
    }

    String data = "";
    while (file.available()) {
        data += (char)file.read();
    }
    file.close();

    stringToPosArray(data, pos);
}
