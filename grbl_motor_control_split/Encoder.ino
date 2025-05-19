int32_t Encoder(int petal, int motor){ // get encoder value
  //tca9548a.selectChannel(petalMap[petal].pahub_address);
  //enc_counts[0][motor] = petalMap[petal].encoder->getEncoderValue(motor);
  enc_counts[petal][motor] = petalMap[petal].encoder->getEncoderValue(motor);
  enc_pos[petal][motor] = enc_counts[petal][motor] * extension_over_encoder_counts;
  //String printstring = "Enc counts are: " + String(enc_counts[petal][motor]) + " leading to a position of: " + String(enc_pos[petal][motor]);
  //Serial.println(printstring);
  
  //agnostic encoder readings
  //tca9548a.selectChannel(petalMap[petal].pahub_address);
  //enc_counts[petal][motor] = petalMap[petal].encoder->getEncoderValue(motor);
  //enc_pos[petal][motor] = enc_counts[petal][motor] * enc_to_extension_ratio;
  return enc_counts[0][motor];
}