void retractTelArms(){

int32_t encoder;
int32_t live_pos[5] = {0,0,0,0,0};
int tel_arm_stage = 2;
//LogParams("RetractTelArms "+String(n*1000000)+" Steps");
zeroEncoder(0, tel_arm_stage);
zeroEncoder(1, tel_arm_stage);
zeroEncoder(2, tel_arm_stage);
zeroEncoder(3, tel_arm_stage);


pollCurrents();
pollEncoders();

/*
for (uint8_t i = 0; i < 2; i++) {
live_pos[i] = GetEncoderValue(i, tel_arm_stage);
Serial.print("Start position for motor ");
Serial.print(i);
Serial.print(" is: ");
Serial.println(live_pos[i]);
}
*/
EndStops();
// direction currant encoder counts
//MoveTelArms(1,200,142000);

//MoveTelArms2(20000,1);

for (uint8_t i = 0; i < 2; i++) {
live_pos[i] = GetEncoderValue(i, tel_arm_stage);
Serial.print("Live position for motor ");
Serial.print(i);
Serial.print(" is: ");
Serial.println(live_pos[i]);

}

}
/*

for (uint8_t i = 0; i < 2; i++) {
  live_pos[i] = GetEncoderValue(i, tel_arm_stage);
}
  delay(200);

  // If motor 1 is lagging
  if (live_pos[0]> live_pos[1]+200){
    int32_t steps = live_pos[0] - live_pos[1];
    int32_t start = live_pos[1];
    //int start = GetEncoderValue(1, tel_arm_stage);
    MoveMotor(1, tel_arm_stage, 127);
    int test = 0;
    while (test !=1){
      int32_t live_pos_corr = GetEncoderValue(1, tel_arm_stage);
      if(abs(live_pos_corr - start) > abs(steps)){
          MoveMotor(1, tel_arm_stage, 0);
          test = 1;
      }
    }
 }

 // If motor 0 is lagging
 if (live_pos[1]> live_pos[0] +200){
    int32_t steps = live_pos[1] - live_pos[0];
    int32_t start = live_pos[0];
    MoveMotor(1, tel_arm_stage, 127);
    int test = 0;
    while (test !=1){
      int32_t live_pos_corr = GetEncoderValue(0, tel_arm_stage);
      if(abs(live_pos_corr - start) > abs(steps)){
          MoveMotor(0, tel_arm_stage, 0);
          test = 1;      
      }
    }
 }


Serial.print(" Telescopic arms fully retracted ");

for (uint8_t i = 0; i < 2; i++) {
live_pos[i] = GetEncoderValue(i, tel_arm_stage);
Serial.print("Live position for motor ");
Serial.print(i);
Serial.print(" is: ");
Serial.println(live_pos[i]);
}
*/

/*
while(live_pos[i] <= arms_max[i]){
  // set the encoder value to move to to be 1/10 of the max value to do corrections half way up
  //MoveTelArms(-1,200,14200);

  MoveTelArms2(140000, 1 );
  live_pos[i] = GetEncoderValue(i, tel_arm_stage);
  delay(200);

  // If motor 0 is lagging
  if (live_pos[0]> live_pos[1]+400){
    int32_t steps = live_pos[0] - live_pos[1];
    int32_t start = live_pos[1];
    //int start = GetEncoderValue(1, tel_arm_stage);
    MoveMotor(0, tel_arm_stage, 127);
    int test = 0;
    while (test !=1){
      int32_t live_pos_corr = GetEncoderValue(0, tel_arm_stage);
      if(abs(live_pos_corr - start) > abs(steps)){
          MoveMotor(0, tel_arm_stage, 0);
          test = 1;
      }
    }
 }

 // If motor 1 is lagging
 if (live_pos[1]> live_pos[0] +400){
    int32_t steps = live_pos[1] - live_pos[0];
    int32_t start = live_pos[0];
    MoveMotor(1, tel_arm_stage, 127);
    int test = 0;
    while (test !=1){
      int32_t live_pos_corr = GetEncoderValue(1, tel_arm_stage);
      if(abs(live_pos_corr - start) > abs(steps)){
          MoveMotor(1, tel_arm_stage, 0);
          test = 1;      
      }
    }
 }

 Serial.print(live_pos[i]);
 delay(200);
}
}

Serial.print(" Telescopic arms fully retracted ");

pollCurrents();
pollEncoders();

}
*/