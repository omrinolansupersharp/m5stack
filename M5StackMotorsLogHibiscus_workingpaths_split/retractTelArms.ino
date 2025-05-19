int retractTelArms(int n, int tel_arm_stage){
//int tel_arm_stage = 2;
pollCurrents();
pollEncoders();
// code to check the current position of the arms
int32_t live_pos[5] = {0,0,0,0,0};
int32_t int_pos[5] = {0,0,0,0,0};
int32_t max_value = int_pos[0];
for (uint8_t i = 0; i < 5; i++) {
    live_pos[i] = GetEncoderValue(i,tel_arm_stage);
    int_pos[i] = live_pos[i]/10000000;
    if (int_pos[i] > max_value){
    max_value = int_pos[i];
  }    
}
//LogParams("RetractTelArms"+String(n*1000000)+" Steps");
//for (uint8_t i = 0; i < 5; i++) {
//  zeroEncoder(i, tel_arm_stage);
//}
LogParams("RetractTelArms");
int trip_test = 0;
for(int8_t i = max_value+1; i < n+1; i++){
  trip_test = MoveTelArms((i*10000000),tel_arm_stage);
  if(trip_test > 4){
    Serial.println("Current tripped or keyboard interrupt");
    break;
  }
}
pollCurrents();
pollEncoders();
LogParams("RetractTelArms");
return trip_test;
}