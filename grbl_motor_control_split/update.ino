// update M5 screen
void update(int petal,float enc_pos[5][3]){
  // Clear the area where the values are printed
  M5.Lcd.fillRect(20, 80, 200, 20, BLACK); // Clear petal value
  M5.Lcd.fillRect(20, 140, 60, 20, BLACK); // Clear pos[petal][0]
  M5.Lcd.fillRect(100, 140, 60, 20, BLACK); // Clear pos[petal][1]
  M5.Lcd.fillRect(180, 140, 60, 20, BLACK); // Clear pos[petal][2]

  // Print updated values
  M5.Lcd.setCursor(20, 80);  
  M5.Lcd.print(petal);
  M5.Lcd.setCursor(20, 140);  
  M5.Lcd.print(enc_pos[petal][0],4);
  M5.Lcd.setCursor(120, 140);
  M5.Lcd.print(enc_pos[petal][1],4);
  M5.Lcd.setCursor(220, 140);
  M5.Lcd.print(enc_pos[petal][2],4);
  delay(20);
}