void screen_update(void)
//*************************************************
{
  
    M5.Lcd.clear();
    int lcd_y = 80;

    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setBrightness(100);

    M5.Lcd.setCursor(80, 10);
    M5.Lcd.println("Demo CubeSat");

    M5.Lcd.setCursor(50, 40);
    M5.Lcd.println("Btn A/B Open/Close");

    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10, lcd_y);
    M5.Lcd.println("Retract Arms   :");
    M5.Lcd.setCursor(220, lcd_y);
//    M5.Lcd.println(ratio);

    M5.Lcd.setCursor(10, lcd_y+20);
    M5.Lcd.println("Deploy Arms    :");
//    if (world_run) {
//      M5.Lcd.setCursor(220, lcd_y+20);
//        M5.Lcd.println("True");
//    } else {
//      M5.Lcd.setCursor(220, lcd_y+20);
//        M5.Lcd.println("False");
//    }

    M5.Lcd.setCursor(10, lcd_y+40);
    M5.Lcd.println("Retract Petal  :");
    M5.Lcd.setCursor(220, lcd_y+40);
//    M5.Lcd.println(both_speed);


    M5.Lcd.setCursor(10, lcd_y+60);
    M5.Lcd.println("Deploy Petal   :");
//    if (ifs_run) {
//      M5.Lcd.setCursor(220, lcd_y+60);
//        M5.Lcd.println("True");
//    } else {
//      M5.Lcd.setCursor(220, lcd_y+60);
//        M5.Lcd.println("False");
//    }

    M5.Lcd.setCursor(10, lcd_y+80);
    M5.Lcd.println("Full Deploy    :");
//    M5.Lcd.setCursor(220, lcd_y+80);
//    M5.Lcd.println(revolutions);

    M5.Lcd.setCursor(10, lcd_y+100);
    M5.Lcd.println("Full Retract   :");
//    M5.Lcd.setCursor(220, lcd_y+100);
//    M5.Lcd.println(both_speed);

    M5.Lcd.setCursor(10, lcd_y+120);
    M5.Lcd.println("Custom 1       :");
    //M5.Lcd.setCursor(220, lcd_y+120);
    //M5.Lcd.println(def_both_speed);

    M5.Lcd.setCursor(10, lcd_y+140);
    M5.Lcd.println("Custom 2      :");

    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(220, lcd_y+(cursor*20));
    M5.Lcd.println("<---");


}


void smiley_face(void){
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(15);
  M5.Lcd.setCursor(50, 50);
  M5.Lcd.print(":)");
}

void showFireworks(unsigned long duration) {
  M5.Lcd.fillScreen(BLACK);
  unsigned long startTime = millis();
  while (millis() - startTime < duration) {
    drawFirework(random(0, 320), random(0, 240), random(10, 50));
    delay(500); // Adjust the delay as needed
  }
}

void drawFirework(int x, int y, int size) {
  uint16_t baseColor = randomColor();
  for (int i = 0; i < 10; i++) {
    int angle = random(0, 360);
    int length = random(size / 2, size);
    int xEnd = x + length * cos(radians(angle));
    int yEnd = y + length * sin(radians(angle));
    uint16_t color = blendColor(baseColor, randomColor(), i / 10.0);
    M5.Lcd.drawLine(x, y, xEnd, yEnd, color);
    drawSparkles(xEnd, yEnd, color);
  }
}

void drawSparkles(int x, int y, uint16_t color) {
  for (int i = 0; i < 5; i++) {
    int xOffset = random(-5, 5);
    int yOffset = random(-5, 5);
    M5.Lcd.drawPixel(x + xOffset, y + yOffset, color);
  }
}

uint16_t blendColor(uint16_t color1, uint16_t color2, float ratio) {
  uint8_t r1 = (color1 >> 11) & 0x1F;
  uint8_t g1 = (color1 >> 5) & 0x3F;
  uint8_t b1 = color1 & 0x1F;
  uint8_t r2 = (color2 >> 11) & 0x1F;
  uint8_t g2 = (color2 >> 5) & 0x3F;
  uint8_t b2 = color2 & 0x1F;
  uint8_t r = r1 * (1 - ratio) + r2 * ratio;
  uint8_t g = g1 * (1 - ratio) + g2 * ratio;
  uint8_t b = b1 * (1 - ratio) + b2 * ratio;
  return (r << 11) | (g << 5) | b;
}

uint16_t randomColor() {
  return M5.Lcd.color565(random(0, 256), random(0, 256), random(0, 256));
}