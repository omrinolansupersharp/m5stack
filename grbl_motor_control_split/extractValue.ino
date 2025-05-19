float extractValue(String command, char axis) {
  int startIndex = command.indexOf(axis);
  if (startIndex == -1) {
    return 0.0; // Return 0 if the axis is not found
  }
  int endIndex = command.indexOf(' ', startIndex);
  if (endIndex == -1) {
    endIndex = command.length();
  }
  String valueStr = command.substring(startIndex + 1, endIndex);
  return valueStr.toFloat();
}