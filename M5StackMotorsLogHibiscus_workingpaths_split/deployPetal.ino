int deployPetal(int petal){
String log_cmd = "DeployPetal "+String(petal);
pollCurrents();
pollEncoders();
LogParams(log_cmd);
int trip_test = 0;
int driver_k = petal_drivers_motors[petal][0];
int driver_l = petal_drivers_motors[petal][1];
int motor_k = petal_drivers_motors[petal][2];
int motor_l = petal_drivers_motors[petal][3];
//zeroEncoder(driver_k, motor_k);
//zeroEncoder(driver_l, motor_l);
// code to take the current position of the petals and compare to steps
int32_t live_pos[2] = {0,0};
live_pos[0] = GetEncoderValue(driver_k,motor_k);
live_pos[1] = GetEncoderValue(driver_l,motor_l);
int knextStep = findNextStep(ksteps, ksize, live_pos[0]);
int lnextStep = findNextStep(lsteps, lsize, live_pos[1]);
int nextstep = 1;
if (knextStep != -1) {
        Serial.print("The next step larger than the current position is: ");
        Serial.println(knextStep);
        nextstep = (knextStep);
    } 
    //else {
    //    Serial.println("petal is already fully deployed");
    //    trip_test = 2;
        //break;
    //}
pollCurrents();
pollEncoders();
LogParams(log_cmd);
int min_speed = 95;
for (int step = nextstep; step < (ksize); step++){
  if(step>14){
    min_speed = 127;
  }
  else{
    min_speed=95;
  }
  trip_test = MovePairMotors(driver_k, motor_k, ksteps[step], driver_l, motor_l, lsteps[step], min_speed);
  if(trip_test > 2){
    Serial.println("Current tripped or keyboard interrupt");
    break;
  }
  Serial.print(step);
  Serial.println(trip_test);
}
if(trip_test < 4){ // if the path finished without manual interruption
TripMotor(driver_l, motor_l,0, 500);
TripMotor(driver_k, motor_k,0, 500);
TripMotor(driver_l, motor_l,0, 1000);
TripMotor(driver_k, motor_k,0, 1000);
}
pollCurrents();
pollEncoders();
LogParams(log_cmd);
return trip_test; // returns the trip reason
}