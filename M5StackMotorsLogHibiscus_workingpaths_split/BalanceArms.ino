void BalanceArms(int motor, int direction){
  // need to implement the direction changing the direction of this movement
  MoveMotor(2, 3, -127);
  MoveMotor(2, 2, 127);
  MoveMotor(3, 3, -127);
  MoveMotor(3, 2, 127);
  MoveMotor(4, 3, -127);
  MoveMotor(4, 2, 127);
  MoveMotor(0, 3, -127);
  MoveMotor(0, 2, 127);
}