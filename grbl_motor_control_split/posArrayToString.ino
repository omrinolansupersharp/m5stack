// Function to convert the pos array to a string
String posArrayToString(float array[5][3]) {
    String result = "";
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            result += String(array[i][j], 6);
            if (j < 2) result += ",";
        }
        result += "\n";
    }
    return result;
}