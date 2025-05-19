void Stop(){
  for (uint8_t i = 0; i < 4; i++){
  driverA.setMode(i, NORMAL_MODE);
  driverA.setMotorSpeed(i, 0);
  driverB.setMode(i, NORMAL_MODE);
  driverB.setMotorSpeed(i, 0);
  driverC.setMode(i, NORMAL_MODE);
  driverC.setMotorSpeed(i, 0);
  driverD.setMode(i, NORMAL_MODE);
  driverD.setMotorSpeed(i, 0);
  driverE.setMode(i, NORMAL_MODE);
  driverE.setMotorSpeed(i, 0);}
  pollCurrents();
  pollEncoders();
  LogParams("Stop");
}