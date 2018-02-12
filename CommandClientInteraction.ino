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
        command[commandIndex] = '\0';
        return true;  
      }
    }   
  }
  return false;
}


void executeCommand(EthernetClient client){
  const int BODY_LENGTH = 4;
  const int SETPOINT_LENGTH = MAX_COMMAND_LENGTH - BODY_LENGTH;
  bool hasSetpoint = false;
  char cmdBody[BODY_LENGTH + 1]; // еше 1 для терминального нуля
  char setpoint[SETPOINT_LENGTH];
  char reply[12];
  // Команда состоит из тела (присутствует обязательно) и уставки (может отсутствовать).
  // Тело от уставки отделяется знаком '='. В случае если уставки нет, знак равенства
  // также отсутствует, а команда считается запросом. Примеры команд:
  //
  // PRIM
  // LTHR=20
  //
  // Тело команды ВСЕГДА состоит ровно из BODY_LENGTH символов.
  // На любоую команду формируется ответ, содержащий либо значение параметра,
  // к которому относится данная команда, либо другую информациюю

  memcpy(cmdBody, command, BODY_LENGTH);
  memcpy(setpoint, command + BODY_LENGTH + 1, SETPOINT_LENGTH);
  cmdBody[BODY_LENGTH] = '\0';
  hasSetpoint = strchr(command, '=') != NULL;
  
  // Возможные тела команды:
  char setToPrimary[] = "PRIM"; // использовать первичный источник независимо от уровня звука
  char setToSecondary[] = "SCND"; // использовать вторичный источник независимо от уровня звука
  char setToAutoMode[] = "AUTO"; // переключать источники автоматически
  char getState[] = "STAT"; // вернуть состояние устройства 
  char loudThreshold[] = "LTHR"; // установить/получить уровень, считающийся присутствием звука
  char quietThreshold[] = "QTHR"; // установить/получить уровень, считающийся тишиной
  char loudTimeout[] = "LTIM"; // установить/получить величину задержки перед переключением в режим "звук присутствует"
  char quietTimeout[] = "QTIM"; // установить/получить величину задержки перед переключением в режим "тишина в канале"
  char ipAddr[] = "ADDR"; // установить/получить IP устройства
  char netMask[] = "MASK"; // установить/получить маску подсети устройства
  char gateway[] = "GATE"; // установить/получить шлюз устройства

  if (strcmp(cmdBody, setToPrimary) == 0){
    sprintf(reply, "%s", "prim mode");
    CONTROL_TYPE = PRIMARY_SOURCE;  
    changeSourceTo(PRIMARY_SOURCE);
  }
  else if (strcmp(cmdBody, setToSecondary) == 0){
    sprintf(reply, "%s", "scnd mode");
    CONTROL_TYPE = SECONDARY_SOURCE;  
    changeSourceTo(SECONDARY_SOURCE);
  }
  else if (strcmp(cmdBody, setToAutoMode) == 0){
    sprintf(reply, "%s", "auto mode");
    CONTROL_TYPE = AUTO;
  }
  else if (strcmp(cmdBody, getState) == 0){
    int primLeft = processAnalogValue(PRIMARY_SOURCE_LEFT_INPUT);
    int primRight = processAnalogValue(PRIMARY_SOURCE_RIGHT_INPUT);
    int scndLeft = processAnalogValue(SECONDARY_SOURCE_LEFT_INPUT);
    int scndRight = processAnalogValue(SECONDARY_SOURCE_RIGHT_INPUT);
    
    sprintf(reply, "%04d:%04d:%1d:%1d", primLeft, primRight, CONTROL_TYPE, CURRENT_SOURCE);
  }
  else if (strcmp(cmdBody, loudThreshold) == 0){
    processDecimalParam(hasSetpoint, setpoint, reply, LOUD_TRESHOLD);
  }
  else if (strcmp(cmdBody, quietThreshold) == 0){
    processDecimalParam(hasSetpoint, setpoint, reply, QUIET_TRESHOLD);
  }
  else if (strcmp(cmdBody, loudTimeout) == 0){
    processDecimalParam(hasSetpoint, setpoint, reply, LOUD_TRESHOLD);
  }
  else if (strcmp(cmdBody, quietTimeout) == 0){
    processDecimalParam(hasSetpoint, setpoint, reply, LOUD_TRESHOLD);
  }
  else if (strcmp(cmdBody, ipAddr) == 0){

  }
  else if (strcmp(cmdBody, netMask) == 0){
   
  }
  else if (strcmp(cmdBody, gateway) == 0){
  
  }
  else {
    sprintf(reply, "%s", "unknown command");
  }

  if(__DEBUG__){
    Serial.print("Received command: ");
    Serial.println(command);
    Serial.print("Command body: ");
    Serial.println(cmdBody);
    if (hasSetpoint){
      Serial.print("Received command has setpoint: ");
      Serial.println(setpoint);
    }
    Serial.print("Device reply: ");
    Serial.println(reply);
  }

  client.println(reply);
  delay(1);

  commandIndex = 0;
  command[0] = '\0';
}

void processDecimalParam(bool hasSetpoint, char* setpoint, char* reply, int& value){
  if (hasSetpoint){
      value = atoi(setpoint);
    }
    sprintf(reply, "%d", value);
}

