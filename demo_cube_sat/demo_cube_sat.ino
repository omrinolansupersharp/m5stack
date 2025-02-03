/**
 * @file DriverSample.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief Module 4EncoderMotor Test Demo.
 * @version 0.1
 * @date 2024-01-19
 *
 *
 * @Hardwares: M5Core + Module 4EncoderMotor
 * @Platform Version: Arduino M5Stack Board Manager v2.1.0
 * @Dependent Library:
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Module4EncoderMotor: https://github.com/m5stack/M5Module-4EncoderMotor
 */

#include <M5Stack.h>
//#include "M5Unified.h"
//#include "M5GFX.h"
#include "M5Module4EncoderMotor.h"

M5Module4EncoderMotor driverA ;
M5Module4EncoderMotor driverB ;
M5Module4EncoderMotor driverC ;
M5Module4EncoderMotor driverD ;

uint8_t avg_filter_level = 20;

uint8_t mod_address_A = 0x1A;
uint8_t mod_address_B = 0x2A;
uint8_t mod_address_C = 0x3A;
uint8_t mod_address_D = 0x4A;

#define MAX_RECORD_SIZE 256
#define NOTE_DH2 661
float currents[5] = {0.0,0.0,0.0,0.0,0.0};
int32_t encoder_readings[5][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}};
int32_t arms_encoder_readings [2];
int32_t arms_max[5] = {142000,142000,142000,142000};

// Encoder max values
// bottom motor0 value -85,881,
// top motor 0 value -290,524,
// motor 0 encoder steps = 205,000

// bottom motor 1 value -75,068, 
// top motor 1 value -253,039, 
//  motor 1 encoder steps = 175,000
int32_t Ecdr = 0;
float voltages[5] = {0.0,0.0,0.0,0.0,0.0};

const float telarms_backlash_down[5] = {0, 0, 0, 0, 0};

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

float offsets[4] = {12.0, 18.5, 15.0, 14.0};

M5Module4EncoderMotor drivers[4] = {driverA, driverB, driverC, driverD};

uint8_t n_channels =4;
String cmd;
int cursor = 0;

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
    M5.Power.begin();
    Serial.begin(115200);
    screen_update();
   
     while (!driverA.begin(&Wire, mod_address_A, 21, 22)) {
        Serial.println("Driver A Init faild!");
        //M5.Display.drawString("Driver A Init faild!", 160, 7);
        delay(1000);
    }

         while (!driverB.begin(&Wire, mod_address_B, 21, 22)) {
        Serial.println("Driver B Init faild!");
        //M5.Display.drawString("Driver B Init faild!", 160, 7);
        delay(1000);
    }

         while (!driverC.begin(&Wire, mod_address_C, 21, 22)) {
        Serial.println("Driver C Init faild!");
        //M5.Display.drawString("Driver C Init faild!", 160, 7);
        delay(1000);
    }

         while (!driverD.begin(&Wire, mod_address_D, 21, 22)) {
        Serial.println("Driver D Init faild!");
        //M5.Display.drawString("Driver D Init faild!", 160, 7);
        delay(1000);   
    }
    pinMode(2, INPUT_PULLUP);    // sets the digital pin as input
    pinMode(5, INPUT_PULLUP);    // sets the digital pin as input

}

bool direction = true;
int mode       = NORMAL_MODE;

void loop() {

//    if button A pressed
    if (M5.BtnA.wasPressed()) {
        cursor -= 1;
        if (cursor < 0) {
            cursor = 6;
        }
    screen_update();
    }

//    if button B pressed
    if (M5.BtnB.wasPressed()) {

//        cursor += 1;
//        if (cursor > 5) {
//            cursor = 0;
//        }
delay(50);  
    //screen_update();
    button_pressed();    
    }

//    if button C pressed
    if (M5.BtnC.wasPressed()) {
        cursor += 1;
        if (cursor > 6) {
            cursor = 0;
        }
    screen_update();
    }

    M5.update();





}