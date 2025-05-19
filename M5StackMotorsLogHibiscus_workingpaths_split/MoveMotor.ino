void MoveMotor(int driver_cmd, int motor_cmd, int speed){ 
  if (driver_cmd == 0){
  driverA.setMode(motor_cmd, NORMAL_MODE);
  driverA.setMotorSpeed(motor_cmd, speed);}
  
  if (driver_cmd == 1){
  driverB.setMode(motor_cmd, NORMAL_MODE);
  driverB.setMotorSpeed(motor_cmd, speed);}

  if (driver_cmd == 2){
  driverC.setMode(motor_cmd, NORMAL_MODE);
  driverC.setMotorSpeed(motor_cmd, speed);}

  if (driver_cmd == 3){
  driverD.setMode(motor_cmd, NORMAL_MODE);
  driverD.setMotorSpeed(motor_cmd, speed);}

   if (driver_cmd == 4){
  driverE.setMode(motor_cmd, NORMAL_MODE);
  driverE.setMotorSpeed(motor_cmd, speed);}

}