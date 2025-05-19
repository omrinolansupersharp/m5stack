int 
TripTelMotors(int motor_cmd, int direction, int trip_current){
  int32_t live_pos[5] = {0,0,0,0,0};
  int trip = 0;
  /*
  for (uint8_t i = 0; i < 5; i++) {
    live_pos[i] = GetEncoderValue(i, motor_cmd);
    Serial.print("motor ");
    Serial.print(i);
    Serial.print(" : Encoder Value ");
    Serial.println(live_pos[i]);
  }
  */
  pollCurrents();
  pollEncoders();
  int drivers[5] = {0,1,2,3,4};
  LogParams("TripTelMotors");
  if( direction == 0){
  MvTArmMotors(motor_cmd, 127);
  }
  if( direction == 1){
 MvTArmMotors(motor_cmd, -127); }
  int test[5] = {0, 0,0,0,0};
  int sumtest = 0;
  delay(300);
  while(sumtest <5){
          for (uint8_t i = 0; i < 5; i++) {
    currents[i] = GetCurrentValue(drivers[i]);
    if(abs(currents[i]) > trip_current){
      MoveMotor(drivers[i], motor_cmd, 0);
      //MoveMotor(0, motor_cmd, 0);
      //MoveMotor(1, motor_cmd, 0);
      //MoveMotor(2, motor_cmd, 0);
      //MoveMotor(3, motor_cmd, 0);
      //MoveMotor(4, motor_cmd, 0);
      //MvTArmMotors(motor_cmd,0);
      test[i] = 1;
      Serial.println(currents[i]);
      Serial.println(drivers[i]);
    }
    if (Serial.available() > 0 && Serial.read() == 'c'){ // manual stop - all motors stop and trip returned
    int tel_arm_stage = motor_cmd;
    MoveMotor(0, tel_arm_stage, 0);
    MoveMotor(1, tel_arm_stage, 0);
    MoveMotor(2, tel_arm_stage, 0);
    MoveMotor(3, tel_arm_stage, 0);
    MoveMotor(4, tel_arm_stage, 0);
    Serial.println("Manual stop");
    trip = 6;
    }
    GetEncoderValue(drivers[i], motor_cmd);
    live_pos[i] = GetEncoderValue(drivers[i], motor_cmd);
}

if(trip==0){ // while the motor is not tripped, set the sum test to be 
  LogParams("TripTelMotors");
  sumtest = test[0]+test[1]+test[2]+test[3]+test[4];
}
else{
  LogParams("TripTelMotors");
  sumtest = trip;
}
}

pollCurrents();
pollEncoders();
LogParams("TripTelMotors");
return trip;}