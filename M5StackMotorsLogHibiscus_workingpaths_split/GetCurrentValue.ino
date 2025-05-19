float GetCurrentValue(int driver){
  float current = 0.0;
  if (driver == 0){
if (avg_filter_level != 0) {
        amp_record0[record_index0] = driverA.getMotorCurrent();
        record_index0++;
        if (record_index0 >= avg_filter_level) {
            record_index0 = 0;
        }
        amp_value0 = avg_filter(amp_record0, avg_filter_level);
    }
    current = amp_value0; 
    voltages[driver] = driverA.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16; 
}
  if (driver == 1){
if (avg_filter_level != 0) {
        amp_record1[record_index1] = driverB.getMotorCurrent();
        record_index1++;
        if (record_index1 >= avg_filter_level) {
            record_index1 = 0;
        }
        amp_value1 = avg_filter(amp_record1, avg_filter_level);
    }
     current = amp_value1;
     voltages[driver] = driverB.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16; 
}
  if (driver == 2){
if (avg_filter_level != 0) {
        amp_record2[record_index2] = driverC.getMotorCurrent();
        record_index2++;
        if (record_index2 >= avg_filter_level) {
            record_index2 = 0;
        }
        amp_value2 = avg_filter(amp_record2, avg_filter_level);
    }
    current = amp_value2;
  voltages[driver] = driverC.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16; 
 }

  if (driver == 3){
if (avg_filter_level != 0) {
        amp_record3[record_index3] = driverD.getMotorCurrent();
        record_index3++;
        if (record_index3 >= avg_filter_level) {
            record_index3 = 0;
        }
        amp_value3 = avg_filter(amp_record3, avg_filter_level);
    }
    current = amp_value3;
    voltages[driver] = driverD.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16; 
}

  if (driver == 4){
if (avg_filter_level != 0) {
        amp_record4[record_index4] = driverE.getMotorCurrent();
        record_index4++;
        if (record_index4 >= avg_filter_level) {
            record_index4 = 0;
        }
        amp_value4 = avg_filter(amp_record4, avg_filter_level);
    }
    current = amp_value4;
    voltages[driver] = driverE.getAnalogInput(_8bit) / 255.0 * 3.3 / 0.16; 
}

    current = current*1000 + offsets[driver];
    M5.Display.fillRect(20, 40 + 35 * driver, 300, 35, BLACK);
    M5.Display.drawString("CH" + String(driver) + ": " + String(current) + "mA",20, 40 + 35 * driver);
      return current;
}