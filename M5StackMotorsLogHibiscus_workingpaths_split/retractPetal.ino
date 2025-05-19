int retractPetal(int petal){
String log_cmd = "RetractPetal "+String(petal);
pollCurrents();
pollEncoders();
LogParams(log_cmd);
int driver_k = petal_drivers_motors[petal][0];
int driver_l = petal_drivers_motors[petal][1];
int motor_k = petal_drivers_motors[petal][2];
int motor_l = petal_drivers_motors[petal][3];
int min_speed = 95;
//zeroEncoder(driver_k, motor_k);
//zeroEncoder(driver_l, motor_l);
pollCurrents();
pollEncoders();
int trip_test = 0;
LogParams(log_cmd);
int32_t live_pos[2] = {0,0};
live_pos[0] = GetEncoderValue(driver_k,motor_k);
live_pos[1] = GetEncoderValue(driver_l,motor_l);
int krtnnextStep = findNextStep(ksteps_rtn, ksize, -1* live_pos[0]);
int lrtnnextStep = findNextStep(lsteps_rtn, lsize, -1* live_pos[1]);
int rtnnextstep = 0;
if (krtnnextStep != -1) {
        Serial.print("The next k step larger than the current position is: ");
        Serial.println(krtnnextStep);
        Serial.print("The next l step larger than the current position is: ");
        Serial.println(lrtnnextStep);
        rtnnextstep = (krtnnextStep);
    } 
    //else {
    //    Serial.println("petal is already fully deployed");
    //    trip_test = 2;
        //break;
    //}
for (int step = rtnnextstep; step < (krtnsize); step++){

Serial.println(lsteps_rtn[step]);
Serial.println(step);
if(step<35){
  min_speed = 127;
}
else{
  min_speed=95;
}
trip_test = MovePairMotors(driver_k, motor_k, -1*ksteps_rtn[step], driver_l, motor_l, -1*lsteps_rtn[step], min_speed);
if(trip_test > 2){
    Serial.println("Current tripped or keyboard interrupt");
    break;
}
}
if(trip_test < 3){ // if the path finished without manual interruption then trip to the end of the loop
TripMotor(driver_l, motor_l,1, 500);
TripMotor(driver_k, motor_k,1, 500);
//TripMotor(driver_l, motor_l,1, 1000);
//TripMotor(driver_k, motor_k,1, 1000);
}
pollCurrents();
pollEncoders();
LogParams(log_cmd);
return trip_test;
}