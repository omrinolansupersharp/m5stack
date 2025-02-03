void pollCurrents(){
    for (uint8_t i = 0; i <3; i++) {
    currents[i] = GetCurrentValue(i);}
  }