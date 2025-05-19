void MvTArmMotors(int motor_cmd, int speed){
 
  driverA.setMode(motor_cmd, NORMAL_MODE);
  driverA.setMotorSpeed(motor_cmd, speed);
  driverB.setMode(motor_cmd, NORMAL_MODE);
  driverB.setMotorSpeed(motor_cmd, speed);
  driverC.setMode(motor_cmd, NORMAL_MODE);
  driverC.setMotorSpeed(motor_cmd, speed);
  driverD.setMode(motor_cmd, NORMAL_MODE);
  driverD.setMotorSpeed(motor_cmd, speed);
  driverE.setMode(motor_cmd, NORMAL_MODE);
  driverE.setMotorSpeed(motor_cmd, speed);
}