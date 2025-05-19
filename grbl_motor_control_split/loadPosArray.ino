// Read the pos array from the SD card
void loadPosArray(fs::FS &fs, const char * path) {
    File file = fs.open(path);
    if (!file) {
        //Serial.println("Failed to open file for reading");
        return;
    }

    String data = "";
    while (file.available()) {
        data += (char)file.read();
    }
    file.close();

    stringToPosArray(data, pos);
}