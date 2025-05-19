int MoveTelArms(int32_t pos,int tel_arm_stage){
  int trip_current=70; // this is crucial, this is the trip current that we use for the majority of the movement, needs to be enough to overcome minor jams
  int warning_current=50;
  Serial.print("Target position is: ");
  Serial.println(pos);
  pollCurrents();
  pollEncoders();
  String mv_tel_cmd = "MoveTelArms Trip: "+String(trip_current);
  LogParams(mv_tel_cmd);
  int32_t start[5] = {0,0,0,0,0};
  int32_t live_pos[5] = {0,0,0,0,0};
  int32_t steps[5] = {0,0,0,0,0};
  int speeds[5] = {-127,-127,-127,-127,-127}; // tel arm deployment is negative current for move motor
  int direction = 1; // default direction is deployment
  int test[5] = {0,0,0,0,0};
  int sumtest = 0;
  int trip = 0;
  for (uint8_t i = 0; i < 5; i++) {
    start[i] = GetEncoderValue(i,tel_arm_stage);
    Serial.print("Start position is: ");
    Serial.print(start[i]);
    Serial.print(" for motor ");
    Serial.println(i);
    live_pos[i] = start[i];
    steps[i] = pos - start[i];
    //if(steps[i]>0){
    //  if(tel_arm_stage=3){ // top tel arm stage encoder values go down and become negative as arm deploys
    //    direction = 0; // therefore positive steps means that the top arm stage is retracting
    //    speeds[i] = 127;
    //  }
    //}
    if(steps[i]>0){
        direction = 0; // positive steps means that the arm is retracting
        speeds[i] = 127;
        }
    }
      for (uint8_t i = 0; i < 5; i++) {
    MoveMotor(i, tel_arm_stage, speeds[i]);
      }
  while(sumtest <5){
    for (uint8_t i = 0; i < 5; i++) {
      pollCurrents();
      pollEncoders();
      live_pos[i] = GetEncoderValue(i, tel_arm_stage);
      currents[i] = GetCurrentValue(i);
  /*
  if (live_pos[i] = 0){ // encoder count is zero, means that the encoder is disconnected (doesn't account for it disconnecting midway through motion)
    Serial.print(i);
    Serial.println(" motor encoder is disconnected or faulty");
    trip = 5;
  }
  */
  if(((abs(live_pos[i]))-abs(start[i]))> abs(steps[i])){ // encoder count reached for one motor
    MoveMotor(i, tel_arm_stage, 0);
    test[i] = 1;
    //Serial.print("Motor ");
    //Serial.print(i);
    //Serial.print(" encoder stopped at: ");
    //Serial.println(live_pos[i]);
  }
  if(abs(currents[i])> warning_current){
    Serial.print("Warning: Current is above ");
    Serial.println(warning_current);
  }
  if(abs(currents[i])> trip_current){ // current tripped for one motor - all motors stop and trip returned
    MoveMotor(0, tel_arm_stage, 0);
    MoveMotor(1, tel_arm_stage, 0);
    MoveMotor(2, tel_arm_stage, 0);
    MoveMotor(3, tel_arm_stage, 0);
    MoveMotor(4, tel_arm_stage, 0);
    Serial.print("Motor ");
    Serial.print(i);
    Serial.print(" tripped ");
    Serial.print(currents[i]);
    Serial.println(" mA");
    trip = 6;
  }
  if (Serial.available() > 0 && Serial.read() == 'c'){ // manual stop - all motors stop and trip returned
    MoveMotor(0, tel_arm_stage, 0);
    MoveMotor(1, tel_arm_stage, 0);
    MoveMotor(2, tel_arm_stage, 0);
    MoveMotor(3, tel_arm_stage, 0);
    MoveMotor(4, tel_arm_stage, 0);
    Serial.println("Manual stop");
    trip = 7;
  }  
}
if(trip==0){ // while the motor is not tripped, set the sum test to be 
  LogParams(mv_tel_cmd);
  sumtest = test[0]+test[1]+test[2]+test[3]+test[4];
}
else{
  LogParams(mv_tel_cmd);
  sumtest = trip;
}
pollCurrents();
pollEncoders();
/*
if(sumtest==5){ //after all encoder values are the same check that all arms are caught up to each other - do we need this if we are already driving up to a certain encoder value?
int32_t maxVal = 0;
for (uint8_t i = 0; i < 5; i++) {
  if (live_pos[i] > maxVal) {
        maxVal = live_pos[i];
      }
}
for (uint8_t i = 0; i < 5; i++){
    // if any motors are lagging behind the maximum value
    if (live_pos[i] < (maxVal)){
      int32_t steps = live_pos[i] - maxVal;
      int32_t start = live_pos[i];
      Serial.print("Steps: ");
      Serial.println(steps);
      Serial.print("start: ");
      Serial.println(start);
      MoveMotor(i, tel_arm_stage, 127);
      int test = 0;
      while (test !=1){
        int32_t live_pos_corr = GetEncoderValue(i, tel_arm_stage);
        if(abs(live_pos_corr - start) > abs(steps)){
          MoveMotor(i, tel_arm_stage, 0);
          test = 1;
          Serial.println(" Correction completed ");
          }
        }
    }
}
}
*/
}
pollCurrents();
pollEncoders();
LogParams(mv_tel_cmd);
Serial.println(sumtest);
Serial.println(trip);
return trip; // finished, trip will either be 0 if move has completed smoothly or 5 if there is a current trip or keyboard interrupt
}