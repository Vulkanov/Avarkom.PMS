#include <SPI.h> // возможно, необходима для сетевого шилда

// использовать одну из следующих библиотек 
//в зависимости от версии сетевого шилда
#include <Ethernet.h>
//#include <Ethernet2.h> 

#include <stdio.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define __DEBUG__ true  // для включения/выключения вывода в последовательный порт отладочной информации

#define Pin4  4  //          Up
#define Pin5  5  //          Dwn
#define Pin6  6  //         Left
#define Pin7  7  //         Rght
#define btnRIGHT  0// кнопка RIGHT
#define btnUP     1// кнопка  UP
#define btnDOWN   2// кнопка  DOWN
#define btnLEFT   3// кнопка  LEFT
#define btnNONE   5// кнопки  не нажаты

byte bukva_B[8]   = {B11110,B10000,B10000,B11110,B10001,B10001,B11110,B00000,}; // Буква "Б"
byte bukva_G[8]   = {B11111,B10001,B10000,B10000,B10000,B10000,B10000,B00000,}; // Буква "Г"
byte bukva_D[8]   = {B01111,B00101,B00101,B01001,B10001,B11111,B10001,B00000,}; // Буква "Д"
byte bukva_ZH[8]  = {B10101,B10101,B10101,B11111,B10101,B10101,B10101,B00000,}; // Буква "Ж"
byte bukva_Z[8]   = {B01110,B10001,B00001,B00010,B00001,B10001,B01110,B00000,}; // Буква "З"
byte bukva_I[8]   = {B10001,B10011,B10011,B10101,B11001,B11001,B10001,B00000,}; // Буква "И"
byte bukva_IY[8]  = {B01110,B00000,B10001,B10011,B10101,B11001,B10001,B00000,}; // Буква "Й"
byte bukva_L[8]   = {B00011,B00111,B00101,B00101,B01101,B01001,B11001,B00000,}; // Буква "Л"
byte bukva_P[8]   = {B11111,B10001,B10001,B10001,B10001,B10001,B10001,B00000,}; // Буква "П"
byte bukva_Y[8]   = {B10001,B10001,B10001,B01010,B00100,B01000,B10000,B00000,}; // Буква "У"
byte bukva_F[8]   = {B00100,B11111,B10101,B10101,B11111,B00100,B00100,B00000,}; // Буква "Ф"
byte bukva_TS[8]  = {B10010,B10010,B10010,B10010,B10010,B10010,B11111,B00001,}; // Буква "Ц"
byte bukva_CH[8]  = {B10001,B10001,B10001,B01111,B00001,B00001,B00001,B00000,}; // Буква "Ч"
byte bukva_Sh[8]  = {B10101,B10101,B10101,B10101,B10101,B10101,B11111,B00000,}; // Буква "Ш"
byte bukva_Shch[8]= {B10101,B10101,B10101,B10101,B10101,B10101,B11111,B00001,}; // Буква "Щ"
byte bukva_Mz[8]  = {B10000,B10000,B10000,B11110,B10001,B10001,B11110,B00000,}; // Буква "Ь"
byte bukva_IYI[8] = {B10001,B10001,B10001,B11001,B10101,B10101,B11001,B00000,}; // Буква "Ы"
byte bukva_Yu[8]  = {B10010,B10101,B10101,B11101,B10101,B10101,B10010,B00000,}; // Буква "Ю"
byte bukva_Ya[8]  = {B01111,B10001,B10001,B01111,B00101,B01001,B10001,B00000,}; // Буква "Я"


byte CONTROL_TYPE;
const byte PRIMARY_SOURCE = 0;
const byte SECONDARY_SOURCE = 1;
const byte AUTO = 2;

const byte MAX_COMMAND_LENGTH = 21;
char command[MAX_COMMAND_LENGTH];
byte commandIndex = 0;

byte EPR_PORT = 30;
byte EPR_Ip = 10; // ВНИМАНИЕ! IP занимает этот адрес и три следующих!
byte EPR_Mask = 14; // ВНИМАНИЕ! маска подсети занимает этот адрес и три следующих!

byte EPR_quiet_treshold = 50;
byte EPR_loud_treshold = 51;
byte EPR_silence_timeout = 52;
byte EPR_sound_timeout = 53;

long timeMark;
const byte SAMPLING_DELAY = 0; // микросекунд

byte SILENCE_TIMEOUT;
byte SOUND_TIMEOUT;
byte QUIET_TRESHOLD;
byte LOUD_TRESHOLD;
const long NUM_OF_SAMPLES = 64;

byte SOURCE_STATE;
const byte SOUND = 0;
const byte SILENCE = 1;
const byte SOUND_DISAPPEARED = 2;
const byte SOUND_APPEARED = 3;

const byte PRIMARY_SOURCE_LEFT_INPUT = A0; 
const byte PRIMARY_SOURCE_RIGHT_INPUT = A1; 
const byte SECONDARY_SOURCE_LEFT_INPUT = A2; 
const byte SECONDARY_SOURCE_RIGHT_INPUT = A3; 

const byte SOURCE_OUTPUT_1 = 2;
const byte SOURCE_OUTPUT_2 = 3;
const byte AutoStateIndicatorLED = 8;
byte CURRENT_SOURCE;

// Переменные Меню
byte D4 =4; //кнопка right
byte D5 =5; //кнопка down
byte D6 =6; //кнопка up
byte D7 =7; //кнопка left
long previousMillis = 0; //счетчик прошедшего времени для AutoMainScreen
long interval = 30000; //задержка автовозврата к MainScreen 3сек
unsigned long currentMillis; // текущее сохраненное значение времени
byte key_N=5; //номер нажатой кнопки
byte frame_N=0; // номер показ окна

//индикатор уровня
uint8_t symbol0[8] = {B00000,B10101,B10101,B10101,B10101,B10101,B00000,B00000,}; //Определяем массив который содержит полностью закрашенный символ
uint8_t symbol1[8] = {B00000,B00000,B00000,B10101,B00000,B00000,B00000,B00000,};

// параметры сети
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


void updateOctetsInEEPROM(byte* octets, byte addr){
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
  // читаем порт
  PORT=EEPROM.read(EPR_PORT); 

  byte deviceMac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xD0, 0x99 };
  IPAddress deviceIp(ip[0], ip[1], ip[2], ip[3]);
  Ethernet.begin(deviceMac, deviceIp);

  commandServer = new EthernetServer(PORT);
  commandServer->begin();
  webServer = new EthernetServer(80);
  webServer->begin();

  SILENCE_TIMEOUT = EEPROM.read(EPR_silence_timeout);  // в секундах
  SOUND_TIMEOUT = EEPROM.read(EPR_sound_timeout);  // в секундах
  QUIET_TRESHOLD = EEPROM.read(EPR_quiet_treshold); // в процентах
  LOUD_TRESHOLD = EEPROM.read(EPR_loud_treshold); // в процентах
}

//==========================================================PROGRAMM==========================================================

void setup() {
  initializeDeviceParameters();
  
  analogReference(DEFAULT);
  
  delay(1000);
  lcd.init();                     
  lcd.backlight();// Включаем подсветку дисплея
  lcd.createChar(0, symbol0);   
  lcd.createChar(1, symbol1);  //  Загружаем символ из массива symbol0 в нулевую ячейку ОЗУ дисплея
 
  pinMode(SOURCE_OUTPUT_1, OUTPUT);
  pinMode(SOURCE_OUTPUT_2, OUTPUT);
  pinMode(D4, INPUT);
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  pinMode(D7, INPUT);
  pinMode(AutoStateIndicatorLED, OUTPUT);
  pinMode(PRIMARY_SOURCE_LEFT_INPUT, INPUT);
  pinMode(PRIMARY_SOURCE_RIGHT_INPUT, INPUT);
  pinMode(SECONDARY_SOURCE_LEFT_INPUT, INPUT);
  pinMode(SECONDARY_SOURCE_RIGHT_INPUT, INPUT);


  lcd.createChar(2, bukva_I);      // Создаем символ под номером 2
  lcd.createChar(3, bukva_CH);
  lcd.createChar(4, bukva_G);      // Создаем символ под номером 4
  lcd.createChar(5, bukva_Y);
  lcd.createChar(6, bukva_D); 
  lcd.createChar(7, bukva_L);  
  
  SOURCE_STATE = SOUND;
  changeStateTo(AUTO);

  if (__DEBUG__){
    Serial.begin(9600);
    Serial.println("End of SETUP method.");
    char ipInfo[32];
    sprintf(ipInfo, "Avarkom IP is %d.%d.%d.%d:%d", ip[0], ip[1], ip[2], ip[3], PORT);
    Serial.println(ipInfo);
    Serial.println("=========================");
  }
}


 
void loop() {
  // сигнализируем, если в автоматическом режиме
  if (CONTROL_TYPE == AUTO){
    digitalWrite(AutoStateIndicatorLED, HIGH);
  }
  else{
    digitalWrite(AutoStateIndicatorLED, LOW);
  }
  
  keys(); // ввод
  
  EthernetClient commandClient = commandServer->available();
  if (receiveCommand(commandClient)){
    executeCommand(commandClient);
  }
  
  processWebClient();
  
  if (CONTROL_TYPE != AUTO){
    return;
  }

  int primLeft = processAnalogValue(PRIMARY_SOURCE_LEFT_INPUT);
  int primRight = processAnalogValue(PRIMARY_SOURCE_RIGHT_INPUT);

  //primRight = primLeft; //FOR DEBUG!!!!!
  bool soundIsAbsent = sourceIsQuiet(primLeft, primRight);
  bool soundIsPresent = sourceIsLoud(primLeft, primRight);
  
  switch (SOURCE_STATE){
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
      if ((millis() - timeMark) >= (SILENCE_TIMEOUT * 1000)){ // значения таймауток хранятся в секундах
         changeSourceTo(SECONDARY_SOURCE);
         SOURCE_STATE = SILENCE;
      }
      else if (soundIsPresent){
         SOURCE_STATE = SOUND;
      }
      return;
      
    case SOUND_APPEARED:
      if ((millis() - timeMark) >= (SOUND_TIMEOUT * 1000)){ // значения таймаутов хранятся в секундах
         changeSourceTo(PRIMARY_SOURCE);
         SOURCE_STATE = SOUND;
      }
      else if (soundIsAbsent){
         SOURCE_STATE = SILENCE;
      }
      return;
  }
}


