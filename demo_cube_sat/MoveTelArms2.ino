void MoveTelArms2(int32_t Ecdr, int32_t direction){
  // 
  int tel_arm_stage = 2;
  pollCurrents();
  pollEncoders();
  
  int32_t start[5] = {0,0,0,0,0};
  int32_t live_pos[5] = {0,0,0,0,0};
  int32_t test[5] = {0,0,0,0,0};
  int sumtest = 0;
  int32_t current = 127 * direction;
 
  for (uint8_t i = 0; i < 4; i++) {
    start[i] = GetEncoderValue(i, tel_arm_stage);
    Serial.println(start[i]);
    currents[i] = GetCurrentValue(i);
    Serial.print(" : Current ");
    Serial.println(currents[i]);
  }

  while(sumtest !=2){
        for (uint8_t i = 0; i < 2; i++) {
          live_pos[i] = GetEncoderValue(i, tel_arm_stage);
          
          MoveMotor(i, tel_arm_stage, current);
          currents[i] = GetCurrentValue(i);
          Serial.print(" --- motor ");
          Serial.print(i);
          Serial.print(" : Current ");
          Serial.print(currents[i]);
          
          if(abs(live_pos[0]-start[0])> abs(Ecdr)){
            MoveMotor(0, tel_arm_stage, 0);
            MoveMotor(1, tel_arm_stage, 0);
            test[i] = 1;
            sumtest = test[0] + test[1] + test[2] + test[3] + test[4];
            //Serial.println(test[0]);
            //Serial.println(test[1]);

            for (uint8_t i = 0; i < 2; i++) {
            Serial.print("motor ");
            Serial.print(i);
            Serial.print(" : Encoder Value ");
            Serial.println(live_pos[i]);
            
            
            }
          }
          Serial.println("----------");
      }
    }
  }