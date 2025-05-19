// read file from sd card
void readFile(fs::FS &fs, const char * path){ 
    //Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        //Serial.println("Failed to open file for reading");
        return;
    }

    //Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}