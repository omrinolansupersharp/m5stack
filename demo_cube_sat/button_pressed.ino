void button_pressed(void)
//*************************************************
{
  int32_t endstop[2] = {1,1};
  endstop[0] = digitalRead(2);
  endstop[1] = digitalRead(5);
  int32_t retracted = endstop[0] + endstop[1];
        if (cursor == 0) { // Retract Arms
            M5.Speaker.tone(700, 500);
            delay(25);
            retractTelArms();            
        }

        if (cursor == 1) { // Deploy Arms
            if (retracted < 2){
            M5.Speaker.tone(700, 500);
            delay(25);
            deployTelArms();
            }
            else{
              M5.Speaker.tone(540, 200);
              M5.Speaker.tone(200, 200);
            }
        } 

        if (cursor == 2) { // Retract Petal
            M5.Speaker.tone(700, 500); 
            delay(25);
            retractPetArms();           
        }

        if (cursor == 3) { // Deploy Petal
            M5.Speaker.tone(700, 500);
            delay(25);
            deployPetArms();   
        }

        if (cursor == 4) { // Full Deploy
            if (retracted < 2){
            M5.Speaker.tone(700, 500);
            deployPetArms();
            delay(1000);
            deployTelArms();
            
            }
            else{
             M5.Speaker.tone(540, 200);
            M5.Speaker.tone(200, 200);
            }
        }

        if (cursor == 5) { // full Retract
            M5.Speaker.tone(NOTE_DH2, 200);
            retractTelArms();
            delay(1500);
            retractPetArms();
            
            
        }

        if (cursor == 6) {
            M5.Speaker.tone(NOTE_DH2, 200);
          Stop();
        }

}