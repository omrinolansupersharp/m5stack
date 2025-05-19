//change a file on the sd card
void appendFile(fs::FS &fs, const char * path, const char * message){
    //Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        //Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
      //  Serial.println("Message appended");
    } else {
        //Serial.println("Append failed");
    }
    file.close();
}