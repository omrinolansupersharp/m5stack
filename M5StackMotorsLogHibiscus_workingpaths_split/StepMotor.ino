void StepMotor(int driver_cmd, int motor_cmd, int32_t enc_cmd){
 if (driver_cmd == 0){
  driverA.setMode(motor_cmd, POSITION_MODE);
  driverA.setEncoderValue(motor_cmd, 0);
  driverA.setPostionPIDMaxSpeed(motor_cmd, 127);
  driverA.setPositionPoint(motor_cmd, enc_cmd);}
  
  if (driver_cmd == 1){
  driverB.setMode(motor_cmd, POSITION_MODE);
  driverB.setEncoderValue(motor_cmd, 0);
  driverB.setPostionPIDMaxSpeed(motor_cmd, 127);
  driverB.setPositionPoint(motor_cmd, enc_cmd);}

  if (driver_cmd == 2){
  driverC.setMode(motor_cmd, POSITION_MODE);
  driverC.setEncoderValue(motor_cmd, 0);
  driverC.setPostionPIDMaxSpeed(motor_cmd, 127);
  driverC.setPositionPoint(motor_cmd, enc_cmd);}

  if (driver_cmd == 3){
  driverD.setMode(motor_cmd, POSITION_MODE);
  driverD.setEncoderValue(motor_cmd, 0);
  driverD.setPostionPIDMaxSpeed(motor_cmd, 127);
  driverD.setPositionPoint(motor_cmd, enc_cmd);}
}