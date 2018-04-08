/*
 * В ЭТОМ ФАЙЛЕ НАХОДИТСЯ ВСЕ, ЧТО РЕАЛИЗУЕТ
 * ВЗАИМОДЕЙСТВИЕ УСТРОЙСТВА С ПОЛЬЗОВАТЕЛЬСКИМ
 * ИНТЕРФЕЙСОМ ЧЕРЕЗ ETHERNET
 */


// максимальную длину имеют команды установки сетевых параметров (например, ip)
// три стартовых байта, один байт команды, 4 байта октетов и возврат строки. 
#define COMMAND_LENGTH 9

byte startBytes[] = {0x06, 0x16, 0x06};

byte command[COMMAND_LENGTH];
byte commandIndex = 0;

// байты команд
enum COMMAND_CODE{CMD_SET_PRIM, // 0
                  CMD_SET_SCND, // 1
                  CMD_SET_AUTO, // 2
                  CMD_STATE, // 3
                  CMD_NETWORK_STATE, // 4
                  CMD_COMMUTATION_STATE, // 5
                  CMD_LOUD_THR, // 6
                  CMD_QUIET_THR, // 7
                  CMD_LOUD_TIM, // 8
                  CMD_QUIET_TIM, // 9
                  CMD_DHCP, // 10
                  CMD_IP_ADDR, // 11
                  CMD_PORT, // 12
                  CMD_NETMASK, // 13
                  CMD_GATEWAY, // 14
                  CMD_RELAY}; // 15

bool receiveCommand(EthernetClient client){
  while (client.connected()) {
    if (client.available()) {
      char incomingByte = client.read();
      
      if (__DEBUG__){
        Serial.print("Received cmd byte: ");
        Serial.println((unsigned byte)incomingByte, DEC);
        Serial.println("=========");
      }
        
      command[commandIndex++] = incomingByte;      
      
      if (commandIndex >= COMMAND_LENGTH){
        commandIndex = 0;
        return true;  
      }
    }
  }
  return false;
}


void executeCommand(EthernetClient client, char* reply){
  // TODO: Возможно, использовать массив reply то как char, то как byte не очень хорошее решение
  bool badCmdBeginning = false;
  for (byte i = 0; i < 3; i++) {
    if (command[i] != startBytes[i]){
      badCmdBeginning = true;
      break;
    }
  }

  if (badCmdBeginning || command[COMMAND_LENGTH - 1] != '\r' ){
    strcpy(reply, "bad cmd");
    badCmdReceived = true;
    badCmdReceiveTime = millis();
    return;
  }

  // на случай если эта команда - установка адреса, маски или шлюза
  byte octets[4];
  for (byte i = 0; i < 4; i++){
    octets[i] = command[i + 4];
  }

  switch ((COMMAND_CODE)command[3]){
    case CMD_SET_PRIM:
      changeStateTo(PRIMARY_SOURCE);
      break;
    case CMD_SET_SCND:
      changeStateTo(SECONDARY_SOURCE);
      break;
    case CMD_SET_AUTO:
      changeStateTo(AUTO);
      break;
    case CMD_STATE: {
      // ответ выглядит следующим образом:
      //
      //       левыйПервый:правыйПервый:левыйВторой:правыйВторой:текущийРежим:текущийИсточник:состояниеРеле
      // байты:    2      +    2       +    2      +    2       +      1        +     1      +      1       = 11 байт
      //
      // где первые 8 байт - уровень сигнала на левом и правом канале первого и второго источника,
      // далее идет текущий режим (один из PRIMARY_SOURCE, SECONDARY_SOURCE, AUTO),
      // текущий источник) и состояние реле.
      // sizeof(int) == 2
      // sizeof(long) == 4

      byte currentByte = 0;
            
      writeIntToReply(reply, currentByte, processAnalogValue(PRIMARY_SOURCE_LEFT_INPUT)); 
      writeIntToReply(reply, currentByte, processAnalogValue(PRIMARY_SOURCE_RIGHT_INPUT)); 
      writeIntToReply(reply, currentByte, processAnalogValue(SECONDARY_SOURCE_LEFT_INPUT)); 
      writeIntToReply(reply, currentByte, processAnalogValue(SECONDARY_SOURCE_RIGHT_INPUT)); 

      reply[currentByte++] = (char)CONTROL_TYPE;
      reply[currentByte++] = (char)CURRENT_SOURCE;
      reply[currentByte++] = (char)digitalRead(RELAY);
      
      return;
      }
    case CMD_NETWORK_STATE:{
      // ip, порт, маска, шлюз, DHCP - как последовательность байт
      // 4 + 1 + 4 + 4 + 1 = 14 байт
      byte currentByte = 0;
      for (byte i = 0; i < 4; i++, currentByte++){
        reply[currentByte] = ip[i];
      }
      reply[currentByte] = PORT;
      currentByte++;
      for (byte i = 0; i < 4; i++, currentByte++){
        reply[currentByte] = netmask[i];
      }
      for (byte i = 0; i < 4; i++, currentByte++){
        reply[currentByte] = gateway[i];
      }
      reply[currentByte] = USE_DHCP;
      currentByte++;
      return;
      }
    case CMD_COMMUTATION_STATE:
      reply[0] = LOUD_TRESHOLD;
      reply[1] = QUIET_TRESHOLD;
      reply[2] = LOUD_TIMEOUT;
      reply[3] = QUIET_TIMEOUT;
      return;
    case CMD_LOUD_THR:
      LOUD_TRESHOLD = command[4];
      EEPROM.update(EPR_loud_treshold, command[4]);
      break;
    case CMD_QUIET_THR:
      QUIET_TRESHOLD = command[4];
      EEPROM.update(EPR_quiet_treshold, command[4]);
      break;
    case CMD_LOUD_TIM:
      LOUD_TIMEOUT = command[4];
      EEPROM.update(EPR_loud_timeout, command[4]);
      break;
    case CMD_QUIET_TIM:
      QUIET_TIMEOUT = command[4];
      EEPROM.update(EPR_quiet_timeout, command[4]);
      break;
    case CMD_DHCP:
      EEPROM.update(EPR_USE_DHCP, command[4]);
      break;
    case CMD_IP_ADDR:
      updateOctetsInEEPROM(EPR_Ip, octets);
      getOctetsFromEEPROM(ip, EPR_Ip);
      break;
    case CMD_PORT:
      PORT = command[4];
      EEPROM.update(EPR_PORT, command[4]);
      break;
    case CMD_NETMASK:
      updateOctetsInEEPROM(EPR_Mask, octets);
      getOctetsFromEEPROM(netmask, EPR_Mask);
      break;
    case CMD_GATEWAY:
      updateOctetsInEEPROM(EPR_Gate, octets);
      getOctetsFromEEPROM(gateway, EPR_Gate);
      break;
    case CMD_RELAY:
      digitalWrite(RELAY, command[4]&1); // побитовое И, чтобы гарантировать, что запишем 0 или 1
      break;
  }
  sprintf(reply, "%s", "Got it dude");  
}

void sendReply(EthernetClient client, char* reply, byte replaySize){
  byte sent = client.write(reply, replaySize);
  delay(1);
  
  if(__DEBUG__){
    Serial.print("********cmd is bad: ");
    Serial.println(badCmdReceived);
    Serial.print("||||||||||Command code (dec): ");
    Serial.println(command[3], DEC);
    Serial.println("Comand arg bytes (dec):");
    for (byte i = 4; i < COMMAND_LENGTH; i++){
      Serial.println((unsigned byte)command[i], DEC);
    }

    if (sent == replaySize){
      Serial.println("Sent complete reply");
      for (byte i = 0; i < replaySize - 1; i++){
      Serial.println((unsigned byte)reply[i], DEC);
    }
    }
    Serial.println("----------------");
  }
}

void writeIntToReply(char* reply, byte& currentByte, int val){
  char* ptr = (char*) &val;
  reply[currentByte++] = *(ptr++);
  reply[currentByte++] = *(ptr++);
}

