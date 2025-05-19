void zeroEncoder(int driver, int motor){
    if(driver == 0) {
        driverA.setEncoderValue(motor,0);
    }
    if(driver == 1)  {
        driverB.setEncoderValue(motor, 0);
    }
    if(driver == 2)  {
        driverC.setEncoderValue(motor, 0);
    }
    if(driver == 3)  {
        driverD.setEncoderValue(motor, 0);
    }
    
    if(driver == 4)  {
        driverE.setEncoderValue(motor, 0);
    }
}