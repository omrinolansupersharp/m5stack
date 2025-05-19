void pollEncoders(){
    for (uint8_t i = 0; i < 5; i++) {
    for (uint8_t j = 0; j < 4; j++) {
      GetEncoderValue(i,  j);
    }
  }
}