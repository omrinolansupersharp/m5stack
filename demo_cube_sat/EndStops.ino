void EndStops(){
  
Serial.println(" Telescopic arms");
int tel_arm_stage = 2;
int32_t endstop[2] = {1,1};
int32_t endcheck = 2;

// Move the arms until endstops
//for (uint8_t i = 0; i < 2; i++) {

  endstop[0] = digitalRead(2);
  endstop[1] = digitalRead(5);
  Serial.print("Endstop 0 is: ");
  Serial.println(endstop[0]);
  Serial.print("Endstop 1 is: ");
  Serial.println(endstop[1]);
  Serial.print("Endstop 2 is: ");
  Serial.println(endstop[2]);

    while(endcheck  == 2){
      //MoveTelArms(-1,200,142000);
      MoveMotor(0, tel_arm_stage, -127);
      MoveMotor(1, tel_arm_stage, -127);
      Serial.println("motors moving");
      endstop[0] = digitalRead(2);
      endstop[1] = digitalRead(5);
      endcheck = endstop[0] + endstop[1] ;
      Serial.println(endcheck);
    }
  MoveMotor(0, tel_arm_stage, 0);
  MoveMotor(1, tel_arm_stage, 0);
  //}
  Serial.println(" Telescopic arms on end stops");
  
}