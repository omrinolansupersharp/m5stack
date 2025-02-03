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

*/
#include <M5Stack.h>
#include "Module_GRBL_13.2.h"

#define STEPMOTOR_I2C_ADDR_2 0x71

bool wasIdle = true;
long notIdleStartTime = 0;
int delay_new_move = 500;

Module_GRBL _GRBL_B = Module_GRBL(STEPMOTOR_I2C_ADDR_2);

void setup() {
    M5.begin();
    M5.Power.begin();
    Wire.begin(21, 22);

    _GRBL_B.Init(&Wire); // No return value to check
    _GRBL_B.setMode("distance");
    Serial.begin(115200);

    Serial.println("Setup complete");
}

void loop() {
    String cmd = ""; // Declare cmd at the beginning of the loop

    if (Serial.available()) {
        // Read the serial input
        Serial.println("");
        cmd = Serial.readStringUntil('\n');
        Serial.println("cmd: " + cmd);

        move_all_motors(cmd);
    }

    long startTime = millis();

    bool isIdle = _GRBL_B.readIdle();

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
}

void move_all_motors(String command) {
    char buffer[command.length() + 1];
    command.toCharArray(buffer, sizeof(buffer));

    _GRBL_B.sendGcode(buffer); // No return value to check

    // Wait for the move to complete
    while (!_GRBL_B.readIdle()) {
        // Optionally, you can add a small delay here to avoid busy-waiting
        delay(10);
    }
}