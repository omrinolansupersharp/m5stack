void MoveTelArms(int32_t pos, int trip_current,int32_t Ecdr){
  int tel_arm_stage = 2;
  pollCurrents();
  pollEncoders();

  int32_t start[5] = {0,0,0,0,0};
  int32_t live_pos[5] = {0,0,0,0,0};
  int32_t steps[5] = {0,0,0,0,0};

  int speeds[5] = {127,127,127,127,127};
  int speed_R[5] = {0,0,0,0,0};
  int test[5] = {0,0,0,0,0};
  int sumtest = 0;
  //int trip = 0;
  //int trip_count = 0;
  /*
  for (uint8_t i = 0; i < 2; i++) {
    live_pos[i] = GetEncoderValue(i,tel_arm_stage);

    //steps[i] = pos - start[i];
    steps[i] = Ecdr;
    Serial.println(steps[i]);

    if(pos<0){     
      speeds[i] = -127;
      steps[i] = -Ecdr;
    }
  }
  */
  

    //for (uint8_t i = 0; i < 4; i++) {
    //MoveMotor(i, tel_arm_stage, speeds[i]);
    //}
    //delay(150);
//////////////// NEW /////////////////////


  //while( trip_count !=2){

    for (uint8_t i = 0; i < 4; i++) {
    MoveMotor(i, tel_arm_stage, speeds[i]);
    }
    delay(1500);


    //sumtest = 0;
    //int32_t test[5] = {0,0,0,0,0};
    //trip_count = trip_count + 1;
    //delay(300);
    while(sumtest !=2){
      for (uint8_t i = 0; i < 2; i++) {

        live_pos[i] = GetEncoderValue(i, tel_arm_stage);

        Serial.print("motor ");
        Serial.print(i);
        Serial.print(" : Encoder Value ");
        Serial.print(live_pos[i]);
        //Serial.print(" : Encoder Value start ");
        //Serial.print(arms_max[i] - live_pos[i]);
        Serial.print(" : Current ");
        Serial.print(currents[i]);
        Serial.print(" : Sumtest ");
        Serial.print(sumtest);
        //Serial.print(" : Retrys ");
        //Serial.print(trip_count);
        Serial.print(" : speed ");
        Serial.println(speeds[i]);
/*
  //////////// stops both motors  when one over currents ////////////
        currents[i] = GetCurrentValue(i);
        if(abs(currents[i])> trip_current){
          MoveMotor(0, tel_arm_stage, 0);
          MoveMotor(1, tel_arm_stage, 0);
          //MoveMotor(2, tel_arm_stage, 0);
          //MoveMotor(3, tel_arm_stage, 0);
          
          Serial.print(i);
          Serial.print(" tripped ");
          Serial.print(currents[i]);
          Serial.println(" mA");
          sumtest = 2;
          arms_max[0] = arms_max[0] - live_pos[0];
          arms_max[1] = arms_max[1] - live_pos[1];
*/

/*
  ///////////// stops only one motor and keeps the rest going still in the while loop /////////////
        currents[i] = GetCurrentValue(i);

        //extra line to change the current value of motor 1 to make it trip at a higher current than motor 0
        //currents[1] = GetCurrentValue(1) - 100;
        if(abs(currents[i])> trip_current){
          MoveMotor(i, tel_arm_stage, 0);
          test[i] = 1;
          sumtest = test[0] + test[1];
          //current_test[i] = 1;
          //sum_current_test = current_test[0] + current_test[1] + current_test[2] + current_test[3];
          Serial.print(i);
          Serial.print(" Current tripped ");
          Serial.print(currents[i]);
          Serial.println(" mA");
        }
        

  ///////////// slow one motor when at encoder diffrence than 400 ////////////////
            if (live_pos[0]> live_pos[1]+200){
            MoveMotor(1, tel_arm_stage, speeds[i]/2);
            Serial.print(" : slow down 1 ");
              } else if (live_pos[0] < live_pos[1] - 100) { 
                MoveMotor(1, tel_arm_stage, speeds[i]); // Return to normal speed
                Serial.print(" : speedup 1 ");
                }

            if (live_pos[1]> live_pos[0]+200){
            MoveMotor(0, tel_arm_stage, speeds[i]/2);
            Serial.print(" : slow down 2 ");
              } else if (live_pos[1] < live_pos[0] - 100) {
                MoveMotor(0, tel_arm_stage, speeds[i]); // Return to normal speed
                Serial.print(" : speedup 2 ");
              }
*/
          

  ///////////// stops one motor when at encoder count ////////////////
      if(abs(live_pos[i]-start[i])> abs(steps[i])){
        MoveMotor(i, tel_arm_stage, 0);
        test[i] = 1;
        sumtest = test[0] + test[1];
        Serial.println(test[0]);
        Serial.println(test[1]);
        Serial.print(" Motor  ");
        Serial.print(i);
        Serial.print(" Encoder Tripped  ");
        }


      }
      }
    
  }
