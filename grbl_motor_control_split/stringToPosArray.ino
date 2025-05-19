// Function to convert a string to the pos array
void stringToPosArray(String data, float array[5][3]) {
    int row = 0;
    int col = 0;
    char *token = strtok((char*)data.c_str(), ",\n");
    while (token != NULL) {
        array[row][col] = atof(token);
        col++;
        if (col == 3) {
            col = 0;
            row++;
        }
        token = strtok(NULL, ",\n");
    }
}