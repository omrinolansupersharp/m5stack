void pollCurrent(){
    float voltage = -1.0;
    float current = -1.0;
    for (uint8_t i = 0; i < 5; i++) {
    M5.Display.fillRect(20, 40 + 35 * i, 300, 35, BLACK);
    if(i==0){
    voltage = driverA.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16;
    current =  driverA.getMotorCurrent() * 1000;}
    if(i==1){
    voltage = driverB.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16;
    current = driverB.getMotorCurrent() * 1000;}
    if(i==2){
    voltage = driverC.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16;
    current = driverC.getMotorCurrent()* 1000;}
        if(i==3){
    voltage = driverD.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16;
    current = driverD.getMotorCurrent()* 1000;    
    }
    if(i==4){
    voltage = driverE.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16;
    current = driverE.getMotorCurrent()* 1000;    
    }
    M5.Display.drawString("CH" + String(i) + ": " + String(voltage) + "V/" + String(current) + "mA",20, 40 + 35 * i);}

}