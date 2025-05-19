void readpos(float pos[5][3]) {
  // Read array from NVS
  preferences.begin("my-app", true); // Initialize preferences with read-only mode
  preferences.getBytes("Actpos", (float*)pos, sizeof(float) * 5 * 3);
  preferences.end(); // Close preferences
}