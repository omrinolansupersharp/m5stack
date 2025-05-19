// Save the pos array to the SD card
void savePosArray(fs::FS &fs, const char * path) {
    String data = posArrayToString(enc_pos);
    writeFile(fs, path, data.c_str());
}