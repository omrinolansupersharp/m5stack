int MovePairMotors(int driver_k, int motor_k, int32_t pos_k, int driver_l, int motor_l, int32_t pos_l, int min_speed = 95){
  pollCurrents();
  pollEncoders();
  LogParams("MoveMotorPair");
  int32_t start_l = GetEncoderValue(driver_l, motor_l);
  int32_t start_k = GetEncoderValue(driver_k, motor_k);
  int32_t live_pos_l = GetEncoderValue(driver_l, motor_l);
  int32_t live_pos_k = GetEncoderValue(driver_k, motor_k);
  int32_t steps_l = pos_l - start_l ;
  int32_t steps_k = pos_k - start_k;
  int dir_k = 1;
  int dir_l = 1;
  float speed_k = 0;
  float speed_l = 0;
  int8_t max_speed = 127;
  //int8_t min_speed = 95;
  if(steps_k < 0){
    dir_k=-1;
  }
  if(steps_l < 0){
    dir_l=-1;
  }
  if(abs(steps_l) > abs(steps_k)){
    speed_l = max_speed;
    speed_k = max_speed * float(abs(steps_k))/float(abs(steps_l));
    if(speed_k < min_speed){
      speed_k = min_speed;
    }
    speed_k = speed_k;
  }
  else {
    speed_k = max_speed;
    speed_l = max_speed * float(abs(steps_l))/float(abs(steps_k));
    if(speed_l < min_speed){
      speed_l = min_speed;
    }
    speed_l = speed_l;
  }
  //Serial.println(speed_k);
  //Serial.println(speed_l);
  MoveMotor(driver_l, motor_l, dir_l * speed_l);
  MoveMotor(driver_k, motor_k, dir_k * speed_k);
  int test[2] = {0,0};
  int sumtest = 0;
  int trip = 0;
  while(sumtest < 2){
  currents[driver_l] = GetCurrentValue(driver_l);
  currents[driver_k] = GetCurrentValue(driver_k);
  live_pos_l = GetEncoderValue(driver_l, motor_l);
  live_pos_k = GetEncoderValue(driver_k, motor_k);
  if(abs(live_pos_l-start_l) > abs(steps_l)){
    MoveMotor(driver_l, motor_l, 0);
    test[0]=1;
  }
  if(abs(live_pos_k-start_k) > abs(steps_k)){
    MoveMotor(driver_k, motor_k, 0);
    test[1]=1;
  }
  if(abs(currents[driver_l])> 1500){
    MoveMotor(driver_l, motor_l, 0);
    MoveMotor(driver_k, motor_k, 0);
    trip = 3;
  }
  if(abs(currents[driver_k])> 1500){
    MoveMotor(driver_l, motor_l, 0);
    MoveMotor(driver_k, motor_k, 0);
    trip = 3;
  }
  if (Serial.available() > 0 && Serial.read() == 'c'){
    MoveMotor(driver_l, motor_l, 0);
    MoveMotor(driver_k, motor_k, 0);
    Serial.println("Manual stop");
    trip = 6; // manual stop always has trip of 6
  }
  if(trip==0){
  LogParams("MoveMotorPair");
  sumtest = test[0]+test[1];
  }
  else{
  LogParams("MoveMotorPair");
  sumtest = trip;
  }
}

pollCurrents();
pollEncoders();
LogParams("MoveMotorPair");
return sumtest;
}
