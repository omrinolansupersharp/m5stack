int32_t GetEncoderValue(int driver, int motor){
  
  if (driver == 0){
  encoder_readings[0][motor] = driverA.getEncoderValue(motor);
  }
  if (driver == 1){
  encoder_readings[1][motor] = driverB.getEncoderValue(motor);
  }
  if (driver == 2){
  encoder_readings[2][motor] = driverC.getEncoderValue(motor);
  }
  if (driver == 3){
  encoder_readings[3][motor] = driverD.getEncoderValue(motor);
  }

    return encoder_readings[driver][motor];   
}