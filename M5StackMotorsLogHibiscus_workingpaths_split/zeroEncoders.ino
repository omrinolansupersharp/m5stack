void zeroEncoders()
{
    for (uint8_t i = 0; i < 4; i++) {
        driverA.setEncoderValue(i,0);
    }
        for (uint8_t i = 0; i < 4; i++) {
        driverB.setEncoderValue(i,0);
    }
        for (uint8_t i = 0; i < 4; i++) {
        driverC.setEncoderValue(i,0);
    }
        for (uint8_t i = 0; i < 4; i++) {
        driverD.setEncoderValue(i,0);
    }
  for (uint8_t i = 0; i < 4; i++) {
        driverE.setEncoderValue(i,0);
    }
}