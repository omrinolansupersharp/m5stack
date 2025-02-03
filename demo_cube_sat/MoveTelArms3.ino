void MoveTelArms3(){
  int tel_arm_stage = 2;
  pollCurrents();
  pollEncoders();

  int32_t start[5] = {0,0,0,0,0};
  int32_t live_pos[5] = {0,0,0,0,0};
  int32_t steps[5] = {0,0,0,0,0};

  int speeds[5] = {127,127,127,127,127};
  int speed_R[5] = {0,0,0,0,0};
  int test[5] = {0,0,0,0,0};
  int sumtest = 0;
  //int trip = 0;
  //int trip_count = 0;
  
}