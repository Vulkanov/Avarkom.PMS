/*
 * В ЭТОМ ФАЙЛЕ НАХОДИТСЯ ВСЕ, ЧТО РЕАЛИЗУЕТ
 * ВЗАИМОДЕЙСТВИЕ УСТРОЙСТВА С ПОЛЬЗОВАТЕЛЬСКИМ
 * ИНТЕРФЕЙСОМ ЧЕРЕЗ ETHERNET
 */

const byte MAX_COMMAND_LENGTH = 21;
char command[MAX_COMMAND_LENGTH];
byte commandIndex = 0;

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
  char reply[21];
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
  char port[] = "PORT"; // установить/получить порт для соединения с интерфейсом (приложением)
  char netMask[] = "MASK"; // установить/получить маску подсети устройства
  char gateWay[] = "GATE"; // установить/получить шлюз устройства
  char relay[] = "RLAY"; // управление выходом реле

  if (strcmp(cmdBody, setToPrimary) == 0){
    sprintf(reply, "%s", "prim mode");
    changeStateTo(PRIMARY_SOURCE);
  }
  else if (strcmp(cmdBody, setToSecondary) == 0){
    sprintf(reply, "%s", "scnd mode");  
    changeStateTo(SECONDARY_SOURCE);
  }
  else if (strcmp(cmdBody, setToAutoMode) == 0){
    sprintf(reply, "%s", "auto mode");
    changeStateTo(AUTO);
  }
  else if (strcmp(cmdBody, getState) == 0){
    int primLeft = processAnalogValue(PRIMARY_SOURCE_LEFT_INPUT);
    int primRight = processAnalogValue(PRIMARY_SOURCE_RIGHT_INPUT);
    int scndLeft = processAnalogValue(SECONDARY_SOURCE_LEFT_INPUT);
    int scndRight = processAnalogValue(SECONDARY_SOURCE_RIGHT_INPUT);
    
    sprintf(reply, "%04d:%04d:%1d:%1d", primLeft, primRight, CONTROL_TYPE, CURRENT_SOURCE);
  }
  else if (strcmp(cmdBody, loudThreshold) == 0){
    processDecimalParam(LOUD_TRESHOLD, EPR_loud_treshold, reply, hasSetpoint, setpoint);
  }
  else if (strcmp(cmdBody, quietThreshold) == 0){
    processDecimalParam(QUIET_TRESHOLD, EPR_quiet_treshold, reply, hasSetpoint, setpoint);  
  }
  else if (strcmp(cmdBody, loudTimeout) == 0){
    processDecimalParam(LOUD_TIMEOUT, EPR_loud_timeout, reply, hasSetpoint, setpoint);  
  }
  else if (strcmp(cmdBody, quietTimeout) == 0){
    processDecimalParam(QUIET_TIMEOUT, EPR_quiet_timeout, reply, hasSetpoint, setpoint);  
  }
  else if (strcmp(cmdBody, ipAddr) == 0){
    processOctetsString(ip, EPR_Ip, reply, hasSetpoint, setpoint);
  }
  else if (strcmp(cmdBody, port) == 0){
    processDecimalParam(PORT, EPR_PORT, reply, hasSetpoint, setpoint);
  }
  else if (strcmp(cmdBody, netMask) == 0){
    processOctetsString(netmask, EPR_Mask, reply, hasSetpoint, setpoint);
  }
  else if (strcmp(cmdBody, gateWay) == 0){
    processOctetsString(gateway, EPR_Gate, reply, hasSetpoint, setpoint);
  }
  else if (strcmp(cmdBody, relay) == 0){
    if (hasSetpoint){
      digitalWrite(RELAY, atoi(setpoint));
    }
    sprintf(reply, "%d", digitalRead(RELAY));
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

void processDecimalParam(byte& value, byte eeprom_addr, char* reply, bool hasSetpoint, char* setpoint){
  if (hasSetpoint){
    value = atoi(setpoint);
    EEPROM.update(eeprom_addr, value);
  }
  sprintf(reply, "%d", value);
}

void processOctetsString(byte* octets, byte eeprom_addr, char* reply, bool hasSetpoint, char* setpoint){
  if (hasSetpoint){
    byte newOctets[4];
    char *charOctet;
    byte controlCounter = 0;

    charOctet = strtok(setpoint, ".");
    while (charOctet != NULL){
      newOctets[controlCounter] = atoi(charOctet);
      controlCounter++;
      if (controlCounter > 3){
        break;
      }
      charOctet = strtok(NULL, ".");
    }

    if (__DEBUG__){
      Serial.println("Received octets: ");
      Serial.println(newOctets[0]);
      Serial.println(newOctets[1]);
      Serial.println(newOctets[2]);
      Serial.println(newOctets[3]);
      Serial.print("Control counter value: ");
      Serial.println(controlCounter);
    }

    if (controlCounter == 4){ // если пришли все октеты
      for (int i = 0; i < 4; i++){
        octets[i] = newOctets[i];
      }
      updateOctetsInEEPROM(octets, eeprom_addr);
    }
  }
  sprintf(reply, "%d.%d.%d.%d", octets[0], octets[1], octets[2], octets[3]);
}

