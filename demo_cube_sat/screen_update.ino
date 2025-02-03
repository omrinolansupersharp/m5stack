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

//    M5.Lcd.setCursor(10, lcd_y+120);
//    M5.Lcd.println("def_both_speed    :");
//    M5.Lcd.setCursor(220, lcd_y+120);
//    M5.Lcd.println(def_both_speed);

    M5.Lcd.setCursor(10, lcd_y+120);
    M5.Lcd.println("STOP ALL       :");

    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(220, lcd_y+(cursor*20));
    M5.Lcd.println("<---");


}