void TripMotor(int driver_cmd, int motor_cmd, int direction, int trip_current){
  pollCurrents();
  pollEncoders();
  Serial.println(trip_current);
  LogParams("TripMotor");
  if( direction == 0){
 MoveMotor(driver_cmd, motor_cmd, 127);
  }
  if( direction == 1){
 MoveMotor(driver_cmd, motor_cmd, -127);
  }
  float current = 0.0;
  int test = 0;
  while(test <1){
    currents[driver_cmd] = GetCurrentValue(driver_cmd);
    if(abs(currents[driver_cmd]) > trip_current){
    MoveMotor(driver_cmd, motor_cmd, 0);
    Serial.println(currents[driver_cmd]);
    test = 2;
    }
    if (Serial.available() > 0 && Serial.read() == 'c'){ // manual stop - all motors stop and trip returned
    MoveMotor(driver_cmd, motor_cmd, 0);
    Serial.println("Manual stop");
    test = 5;
    }
    GetEncoderValue(driver_cmd, motor_cmd);
    LogParams("TripMotor");
}
currents[driver_cmd] = GetCurrentValue(driver_cmd);
pollEncoders();
LogParams("TripMotor");
}