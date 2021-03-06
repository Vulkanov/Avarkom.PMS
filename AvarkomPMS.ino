#include <SPI.h> // возможно, необходима для сетевого шилда

// использовать одну из следующих библиотек 
//в зависимости от версии сетевого шилда
//#include <Ethernet.h>
#include <Ethernet2.h> 

#include <stdio.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define __DEBUG__ true  // для включения/выключения вывода в последовательный порт отладочной информации

// текущий источник
byte CONTROL_TYPE;
#define PRIMARY_SOURCE 0 // реализовать перечислением
#define SECONDARY_SOURCE 1
#define AUTO 2

// адреса сохраненных в EEPROM значений
#define EPR_USE_DHCP 40
#define EPR_PORT 30
#define EPR_Ip 10 // ВНИМАНИЕ! IP занимает этот адрес и три следующих!
#define EPR_Mask 14 // ВНИМАНИЕ! маска подсети занимает этот адрес и три следующих!
#define EPR_Gate 18 // ВНИМАНИЕ! шлюз занимает этот адрес и три следующих!

#define EPR_quiet_treshold 50
#define EPR_loud_treshold 51
#define EPR_quiet_timeout 52
#define EPR_loud_timeout 53

// логика переключения источников
long timeMark;

// TODO: УБРАТЬ ОТСЮДА В CommandClientInteraction!
#define BAD_CMD_TIMEOUT 2000 // в миллисекундах!
bool badCmdReceived = false;
long badCmdReceiveTime = 0;

byte QUIET_TIMEOUT;
byte LOUD_TIMEOUT;
byte QUIET_TRESHOLD;
byte LOUD_TRESHOLD;

byte SOURCE_STATE;
#define SOUND  0
#define SILENCE  1
#define SOUND_DISAPPEARED  2
#define SOUND_APPEARED  3
#define INITIAL_STATE 4

// входы устройства
#define PRIMARY_SOURCE_LEFT_INPUT  A0
#define PRIMARY_SOURCE_RIGHT_INPUT  A1 
#define SECONDARY_SOURCE_LEFT_INPUT  A2 
#define SECONDARY_SOURCE_RIGHT_INPUT  A3

// выходы устройства
#define SOURCE_OUTPUT_1  2
#define SOURCE_OUTPUT_2  3
#define RELAY  8  // управление внешним реле
#define AUTO_STATE_INDICATOR  9 // диод, загорающийся, если устройство работает в автоматическом режиме
byte CURRENT_SOURCE;

// параметры сети
byte USE_DHCP;
byte PORT;  // Порт для работы с приложением (интерфейсом)
byte ip[4];
byte netmask[4];
byte gateway[4];

EthernetServer *webServer;
EthernetServer *commandServer;

LiquidCrystal_I2C lcd(0x3F,16,2);  // Устанавливаем дисплей

//==========================================================FUNCTIONS==========================================================
// переключение состояний
void changeStateTo(int state){
  if (state == AUTO){ // если переходим в автоматический режим из ручного
    CONTROL_TYPE = AUTO;
    SOURCE_STATE = INITIAL_STATE;
    return;
  }
  
  CONTROL_TYPE = state;
  if (state != AUTO){
    changeSourceTo(state);
  }
}


// переключение источников звука
void changeSourceTo(int source){
  digitalWrite(SOURCE_OUTPUT_1, source);
  digitalWrite(SOURCE_OUTPUT_2, source);
  CURRENT_SOURCE = source;  
}


void getOctetsFromEEPROM(byte* octets, byte addr){
  for (int i=0; i<4; i++){
    octets[i] = EEPROM.read(addr + i);
  }
}


void updateOctetsInEEPROM(byte addr, byte* octets){
  for (int i=0; i<4; i++){
    EEPROM.update(addr + i, octets[i]);
  }
}


bool octetsAreValid(byte* octets){
  // проверка на наличие ошибок из-за незаписанного EEPROM
  // например, устройство не будет работать с IP 255.255.255.255
  for (byte n = 1; n < 4; n++){ // проверка на равенство всех октетов
    if (octets[0] != octets[n]){
      return true;
    }
  }

  // октеты оказались равны, проверяем их значения
  if (octets[0] == 255 || octets[0] == 0){
    // исключаем адреса 0.0.0.0 или 255.255.255.255
    return false;
  }
  else {
    return true;
  }
}


void initializeDeviceParameters(){
  // читаем октеты IP адреса
  getOctetsFromEEPROM(ip, EPR_Ip);
  // читаем маску подсети
  getOctetsFromEEPROM(netmask, EPR_Mask);
  getOctetsFromEEPROM(gateway, EPR_Gate);
  // читаем порт
  PORT=EEPROM.read(EPR_PORT); 
  USE_DHCP = EEPROM.read(EPR_USE_DHCP);

  byte deviceMac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xD0, 0x99 };
  IPAddress deviceIp(ip[0], ip[1], ip[2], ip[3]);
  //if (USE_DHCP > 0) Ethernet.begin(deviceMac);
  //else Ethernet.begin(deviceMac, deviceIp);
  Ethernet.begin(deviceMac, deviceIp);

  commandServer = new EthernetServer(PORT);
  commandServer->begin();
  webServer = new EthernetServer(80);
  webServer->begin();

  QUIET_TIMEOUT = EEPROM.read(EPR_quiet_timeout);  // в секундах
  LOUD_TIMEOUT = EEPROM.read(EPR_loud_timeout);  // в секундах
  QUIET_TRESHOLD = EEPROM.read(EPR_quiet_treshold); // в процентах
  LOUD_TRESHOLD = EEPROM.read(EPR_loud_treshold); // в процентах
}

//==========================================================PROGRAMM==========================================================

void setup() {
  initializeDeviceParameters();
  analogReference(DEFAULT);
  initializeMenu();

  pinMode(RELAY, OUTPUT);
  pinMode(AUTO_STATE_INDICATOR, OUTPUT);
  pinMode(PRIMARY_SOURCE_LEFT_INPUT, INPUT);
  pinMode(PRIMARY_SOURCE_RIGHT_INPUT, INPUT);
  pinMode(SECONDARY_SOURCE_LEFT_INPUT, INPUT);
  pinMode(SECONDARY_SOURCE_RIGHT_INPUT, INPUT);
  pinMode(SOURCE_OUTPUT_1, OUTPUT);
  pinMode(SOURCE_OUTPUT_2, OUTPUT);

  SOURCE_STATE = INITIAL_STATE;
  changeStateTo(AUTO);

  if (__DEBUG__){
    Serial.begin(9600);
    Serial.println("End of SETUP method.");
    char ipInfo[32];
    sprintf(ipInfo, "Avarkom IP is %d.%d.%d.%d:%d dhcp: %d", 
            ip[0], ip[1], ip[2], ip[3], PORT, bool(USE_DHCP));
    Serial.println(ipInfo);
    Serial.println("=========================");
  }
}

 
void loop() {
  // сигнализируем, ОБ АВАРИИ (если в автоматическом режиме и источник 2)
  if (CONTROL_TYPE == AUTO && SOURCE_STATE == SILENCE){
    digitalWrite(AUTO_STATE_INDICATOR, HIGH);
  }
  else{
    digitalWrite(AUTO_STATE_INDICATOR, LOW);
  }
  
  keys(); // ввод

  // обрабатываем следующую команду, если предыдущая была правильной или если предыдущая была неправильной, но таймаут истек
  bool processNextComand = !badCmdReceived || millis() - badCmdReceiveTime >= BAD_CMD_TIMEOUT;

  if (processNextComand){
    badCmdReceived = false;
    EthernetClient commandClient = commandServer->available();
    const byte REPLY_SIZE = 14;
    char reply[REPLY_SIZE];
    if (receiveCommand(commandClient)){
      executeCommand(commandClient, reply);
      sendReply(commandClient, reply, REPLY_SIZE);
    }
  }
  processWebClient();
  
  if (CONTROL_TYPE != AUTO){
    return;
  }

  int primLeft = processAnalogValue(PRIMARY_SOURCE_LEFT_INPUT);
  int primRight = processAnalogValue(PRIMARY_SOURCE_RIGHT_INPUT);

  bool soundIsAbsent = sourceIsQuiet(primLeft, primRight);
  bool soundIsPresent = sourceIsLoud(primLeft, primRight);
  
  switch (SOURCE_STATE){
    case INITIAL_STATE:
      if (soundIsPresent){
        changeSourceTo(PRIMARY_SOURCE);
        SOURCE_STATE = SOUND;
      }
      else if (soundIsAbsent){
        changeSourceTo(SECONDARY_SOURCE);
        SOURCE_STATE = SILENCE;
      }
      break;
      
    case SOUND:
      if (soundIsAbsent){  
         SOURCE_STATE = SOUND_DISAPPEARED;
         timeMark = millis(); 
      }
      return;
      
    case SILENCE:
      if (soundIsPresent){  
         SOURCE_STATE = SOUND_APPEARED;
         timeMark = millis(); 
      }
      return;
      
    case SOUND_DISAPPEARED:
      if ((millis() - timeMark) >= (QUIET_TIMEOUT * 1000)){ // значения таймауток хранятся в секундах
         changeSourceTo(SECONDARY_SOURCE);
         SOURCE_STATE = SILENCE;
      }
      else if (soundIsPresent){
         SOURCE_STATE = SOUND;
      }
      return;
      
    case SOUND_APPEARED:
      if ((millis() - timeMark) >= (LOUD_TIMEOUT * 1000)){ // значения таймаутов хранятся в секундах
         changeSourceTo(PRIMARY_SOURCE);
         SOURCE_STATE = SOUND;
      }
      else if (soundIsAbsent){
         SOURCE_STATE = SILENCE;
      }
      return;
  }
}


