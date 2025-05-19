void savepos(float pos[5][3]) {
  // Store modified array in NVS
  preferences.begin("my-app", false); // Initialize preferences with read/write mode
  preferences.putBytes("Actpos", (float*)pos, sizeof(float) * 5 * 3);
  preferences.end(); // Close preferences
}