void retractPetArms(){
int tel_arm_stage = 0;
pollCurrents();
pollEncoders();
//LogParams("RetractTelArms "+String(n*1000000)+" Steps");
zeroEncoder(0, tel_arm_stage);
zeroEncoder(1, tel_arm_stage);
zeroEncoder(2, tel_arm_stage);
zeroEncoder(3, tel_arm_stage);

pollCurrents();
pollEncoders();

// direction currant encoder counts
//MovePetArms(1,200,14200);
//Serial.println("firstmovedone")
MovePetArms(1,150,1420000);
MovePetArms(1,150,1420000);

pollCurrents();
pollEncoders();

}