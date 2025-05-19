void LogCurrents(String active_cmd){
  if(log_mode !=1){
    return;
  }
  char log_file_buffer[current_log_file.length()];
  current_log_file.toCharArray(log_file_buffer, current_log_file.length()+1);
  String t_sec = String(millis());
  String params = "";
   for(uint8_t i = 0; i < 5; i++){
    params += String(currents[i]);
    params += ", ";
  }
  String line_to_add = t_sec+", "+active_cmd+", "+params+"\n";
  char char_line_to_add[line_to_add.length()+1];
  line_to_add.toCharArray(char_line_to_add, line_to_add.length()+1);
  appendFile(SD, log_file_buffer, char_line_to_add);

}