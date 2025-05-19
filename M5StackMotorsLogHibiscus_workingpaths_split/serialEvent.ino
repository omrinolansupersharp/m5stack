void serialEvent() {
  while (Serial.available()) {
    char input = Serial.read();
    if (input == 'S') { // 's' is the stop command
      Stop();
      Serial.println("All motors stopped: Restart when ready");
    }
  }
}