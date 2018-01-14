/*
 * В ЭТОМ ФАЙЛЕ НАХОДИТСЯ ВСЕ, ЧТО РЕАЛИЗУЕТ
 * ВЗАИМОДЕЙСТВИЕ УСТРОЙСТВА С ПОЛЬЗОВАТЕЛЬСКИМ
 * ИНТЕРФЕЙСОМ ЧЕРЕЗ ETHERNET
 */

bool receiveCommand(EthernetClient client){
  if (commandIndex >= MAX_COMMAND_LENGTH - 1){
    return true;
  }
 
  while (client.connected()) {
    if (client.available()) {
      char incomingByte = client.read();  
      command[commandIndex++] = incomingByte;      
      
      if (incomingByte == '\r' || commandIndex >= MAX_COMMAND_LENGTH){
        return true;  
      }
    }   
  }
  return false;
}


void executeCommand(EthernetClient client){
  char reply[12];
  Serial.println(command);
  // Возможные команды:
  // PRIM - использовать первичный источник независимо от уровня звука
  // SCND - использовать вторичный источник независимо от уровня звука
  // AUTO - переключать источники автоматически
  // STAT - вернуть состояние устройства 
  char setToPrimary[] = "PRIM\r";
  char setToSecondary[] = "SCND\r";
  char setToAutoMode[] = "AUTO\r";
  char getState[] = "STAT\r";

  if (strcmp(command, setToPrimary) == 0){
    sprintf(reply, "%s", "prim mode");
    CONTROL_TYPE = PRIMARY_SOURCE;  
    changeSourceTo(PRIMARY_SOURCE);
  }
  else if (strcmp(command, setToSecondary) == 0){
    sprintf(reply, "%s", "scnd mode");
    CONTROL_TYPE = SECONDARY_SOURCE;  
    changeSourceTo(SECONDARY_SOURCE);
  }
  else if (strcmp(command, setToAutoMode) == 0){
    sprintf(reply, "%s", "auto mode");
    CONTROL_TYPE = AUTO;
  }
  else if (strcmp(command, getState) == 0){
    int primLeft = processAnalogValue(PRIMARY_SOURCE_LEFT_INPUT);
    int primRight = processAnalogValue(PRIMARY_SOURCE_RIGHT_INPUT);
    int scndLeft = processAnalogValue(SECONDARY_SOURCE_LEFT_INPUT);
    int scndRight = processAnalogValue(SECONDARY_SOURCE_RIGHT_INPUT);
    
    sprintf(reply, "%04d:%04d:%1d:%1d", primLeft, primRight, CONTROL_TYPE, CURRENT_SOURCE);
  }

  client.println(reply);
  delay(1);
  Serial.println(reply);

  commandIndex = 0;
}
