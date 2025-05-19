void pollAddress(){
    
    uint8_t  address = driverA.getI2CAddress();

    M5.Display.fillRect(20, 40 + 35 * 4, 300, 35, BLACK);
    M5.Display.drawString(
        "Address: " + String(address), 20,40 + 35 * 4);

}