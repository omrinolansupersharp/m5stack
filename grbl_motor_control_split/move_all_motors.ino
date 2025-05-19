void move_all_motors(String command, int petal){
    int32_t start[3] = {0,0,0};
    int32_t init_pos[3] = {0,0,0};
    int32_t live_pos[3] = {0,0,0};
    float steps[3] = {0.0,0.0,0.0};
    int32_t target[3] = {0,0,0};
    float damped_move[3] = {0,0,0};
    int32_t end[3] = {0,0,0};
    float diff[3] = {0,0,0};
    float damp = 1.0;
    int reached[3] = {0,0,0};
    int sum = 0;
    bool allReached = false;
    for (uint8_t i = 0; i < 3; i++) {
      start[i] = Encoder(petal,i);
    }
    damped_move[0] = extractValue(command, 'X')  * damp / extension_over_encoder_counts;
    damped_move[1] = extractValue(command, 'Y')  * damp / extension_over_encoder_counts;
    damped_move[2] = extractValue(command, 'Z')  * damp / extension_over_encoder_counts;
    float f = extractValue(command, 'F');
    target[0] = extractValue(command, 'X') / extension_over_encoder_counts + start[0];
    target[1] = extractValue(command, 'Y') / extension_over_encoder_counts + start[1];
    target[2] = extractValue(command, 'Z') / extension_over_encoder_counts + start[2];

    // loop to check encoder values and stop if reached targets
    for (uint8_t e = 1; e < 3 && !allReached ; e++) {
      //Serial.print("iteration: ");
      //Serial.println(e);
      //Serial.println("-------------");
    for (uint8_t i = 0; i < 3; i++) { // loop to make more precise corrections to movement
      init_pos[i] = Encoder(petal,i);
      if (reached[i] = 0){
        steps[i] = 0;
      }
      else {
        steps[i] = (target[i] - init_pos[i]) * damp / encoder_counts_over_motor_command;
      }
    }
    String corr_command = "G1 X" + String(steps[0]) + " Y" + String(steps[1]) + " Z" + String(steps[2]) + " F" + String(f);
    char corr_buffer[corr_command.length() + 2];
    corr_command.toCharArray(corr_buffer, sizeof(corr_buffer));
    //Serial.println(corr_buffer);
    tca9548a.selectChannel(petalMap[petal].pahub_address);
    petalMap[petal].motor->sendGcode(corr_buffer); 
    petalMap[petal].motor->waitIdle();
    int32_t dummy = Encoder(0,0);
    for (uint8_t i = 0; i < 3; i++) {
      live_pos[i] = Encoder(petal, i);
      //String printstring = "Live posss: " + String(live_pos[i]) + ", target is: " + String(target[i]);
      //Serial.println(printstring);
      if (abs(live_pos[i] - target[i]) <= 10) {
      reached[i] = 1;
      //String printstring =  String(i) + " motor reached target";
      //Serial.println(printstring);
      // now need to add it where it keeps on looping round until they have all been done
      } 
    }
    // here check if all of the motors have reached the endpoints 
    sum = reached[0] + reached[1] + reached[2];
    if (sum >= 3){
    allReached = true;
    //Serial.println("Enc Value reached");
    break;
    }
    }
   
}