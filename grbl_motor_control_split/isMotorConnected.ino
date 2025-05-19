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