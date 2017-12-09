#include <SPI.h> // probably needed for ethernet shield
#include <Ethernet.h>
#include <stdio.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,16,2);  // Устанавливаем дисплей

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


byte CONTROL_TYPE=2;
const byte PRIMARY_SOURCE = 0;
const byte SECONDARY_SOURCE = 1;
const byte AUTO = 2;

const byte MAX_COMMAND_LENGTH = 200;
char command[MAX_COMMAND_LENGTH];
byte commandIndex = 0;

byte EPR_PORT = 30;
byte EPR_Ip1 = 10;
byte EPR_Ip2 = 11;
byte EPR_Ip3 = 12;
byte EPR_Ip4 = 13;
byte EPR_Mask1 = 14;
byte EPR_Mask2 = 15;
byte EPR_Mask3 = 16;
byte EPR_Mask4 = 17;
byte EPR_quiet_treshold = 50;
byte EPR_loud_treshold = 51;
byte EPR_silence_timeout = 52;
byte EPR_sound_timeout = 53;


byte PORT=EEPROM.read(EPR_PORT);  // Порт
byte Ip1=EEPROM.read(EPR_Ip1);   // Ip1
byte Ip2=EEPROM.read(EPR_Ip2);   // Ip2
byte Ip3=EEPROM.read(EPR_Ip3);   // Ip3
byte Ip4=EEPROM.read(EPR_Ip4);   // Ip4
byte Mask1=EEPROM.read(EPR_Mask1);   // Mask1
byte Mask2=EEPROM.read(EPR_Mask2);   // Mask2
byte Mask3=EEPROM.read(EPR_Mask3);   // Mask3
byte Mask4=EEPROM.read(EPR_Mask4);   // Mask4


long timeMark;
const byte SAMPLING_DELAY = 3; // usec
byte silence_timeout = EEPROM.read(EPR_silence_timeout);
long SILENCE_TIMEOUT = silence_timeout*1000;  // в секунды
byte sound_timeout = EEPROM.read(EPR_sound_timeout);
long SOUND_TIMEOUT = sound_timeout*1000;  // в секунды
byte quiet_treshold = EEPROM.read(EPR_quiet_treshold);
int QUIET_TRESHOLD = map(quiet_treshold,0,100,0,1023);    //0;   //порог перехода на резерв
byte loud_treshold = EEPROM.read(EPR_loud_treshold);
int LOUD_TRESHOLD = map(loud_treshold,0,100,0,1023); //60;   //порог восстановления на основной
const byte NUM_OF_SAMPLES = 80;

byte SOURCE_STATE;
const byte SOUND = 0;
const byte SILENCE = 1;
const byte SOUND_DISAPPEARED = 2;
const byte SOUND_APPEARED = 3;

const byte PRIMARY_SOURCE_LEFT_INPUT = 0; 
const byte PRIMARY_SOURCE_RIGHT_INPUT = 1; 
const byte SECONDARY_SOURCE_LEFT_INPUT = 2; 
const byte SECONDARY_SOURCE_RIGHT_INPUT = 3; 

const byte SOURCE_OUTPUT_1 = 2;
const byte SOURCE_OUTPUT_2 = 3;
byte CURRENT_SOURCE;



// Переменные Меню
byte frame_N=0; // номер показ окна

EthernetServer webServer(80);
EthernetServer commandServer(PORT);

//=============FUNCTIONS====================
//========== драйвер кнопок ========

byte D4 =4; //right
byte D5 =5; //down
byte D6 =6; //up
byte D7 =7; //left
long previousMillis = 0; //счетчик прошедшего времени для AutoMainScreen
long interval = 30000; //задержка автовозврата к MainScreen 3сек
unsigned long currentMillis;
byte key_N=5 ;
int btn(){
  currentMillis = millis();
if (digitalRead(D4) == HIGH) {
  delay(200);
  previousMillis = currentMillis; 
  return btnRIGHT;    //  0
}
if (digitalRead(D5) == HIGH) {
  delay(200);
  previousMillis = currentMillis; 
  return btnUP;       //  1
}
if (digitalRead(D6) == HIGH) {
  delay(200);
  previousMillis = currentMillis;
  return btnDOWN;  //  2
}
if (digitalRead(D7) == HIGH) {
  delay(200);
  previousMillis = currentMillis;
  return btnLEFT;   //  3 
}
if (digitalRead(D4) == LOW || digitalRead(D5) == LOW || digitalRead(D6) == LOW || digitalRead(D7) == LOW) {
  return btnNONE; // не нажата 5              
}
};

//индикатор уровня
uint8_t           symbol[8] = {B10101,B10101,B10101,B10101,B10101,B10101,B10101,B00000,}; //  Определяем массив который содержит полностью закрашенный символ
void levelmetr(int valSensor){
  uint8_t j=map(valSensor,0,255,0,12);                //  Определяем переменную j которой присваиваем значение valSensor преобразованное от диапазона 0...1023 к диапазону 0...17
  for(uint8_t i=0; i<10; i++){                         //  Выполняем цикл 16 раз для вывода шкалы из 16 символов начиная с позиции в которую ранее был установлен курсор
        lcd.write(j>i? 1:32);                            //  Выводим на дисплей символ по его коду, либо 1 (символ из 1 ячейки ОЗУ дисплея), либо 32 (символ пробела)
    }                           
}

void changeSourceTo(int source){
  digitalWrite(SOURCE_OUTPUT_1, source);
  digitalWrite(SOURCE_OUTPUT_2, source);
  CURRENT_SOURCE = source;
}


int processAnalogValue(int channel){
  long sygnal = 0;

  for (int i=0; i < NUM_OF_SAMPLES; i++) {
    sygnal += analogRead(channel);
    delayMicroseconds(SAMPLING_DELAY);
  }

  return sygnal / NUM_OF_SAMPLES; 
}


bool sourceIsQuiet(int ch1, int ch2){
  return (ch1 <= QUIET_TRESHOLD) || (ch2 <= QUIET_TRESHOLD);
}


bool sourceIsLoud(int ch1, int ch2){
  return (ch1 >= LOUD_TRESHOLD) || (ch2 >= LOUD_TRESHOLD);
}


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
  Serial.println(command);
  // comands can be:
  // PRIM - go to manual mode and use primary source
  // SCND - go to manual mode and use secondary source
  // AUTO - go to auto mode
  // STAT - get state
  char setToPrimary[] = "PRIM\r";
  char setToSecondary[] = "SCND\r";
  char setToAutoMode[] = "AUTO\r";
  char getState[] = "STAT\r";

  if (strcmp(command, setToPrimary) == 0){
    Serial.println("prim mode");
    CONTROL_TYPE = PRIMARY_SOURCE;  
    changeSourceTo(PRIMARY_SOURCE);
  }
  else if (strcmp(command, setToSecondary) == 0){
    Serial.println("sec mode");
    CONTROL_TYPE = SECONDARY_SOURCE;  
    changeSourceTo(SECONDARY_SOURCE);
  }
  else if (strcmp(command, setToAutoMode) == 0){
    Serial.println("auto mode");
    CONTROL_TYPE = AUTO;
  }
  else if (strcmp(command, getState) == 0){
    int primLeft = processAnalogValue(PRIMARY_SOURCE_LEFT_INPUT);
    int primRight = processAnalogValue(PRIMARY_SOURCE_RIGHT_INPUT);
    int scndLeft = processAnalogValue(SECONDARY_SOURCE_LEFT_INPUT);
    int scndRight = processAnalogValue(SECONDARY_SOURCE_RIGHT_INPUT);
    
    char reply[12];
    sprintf(reply, "%04d:%04d:%1d:%1d", primLeft, primRight, CONTROL_TYPE, CURRENT_SOURCE);
    client.println(reply);
    delay(1);
    Serial.println(reply);
  }

  commandIndex = 0;
}


void processWebClient(){
  EthernetClient client = webServer.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          
          client.println("<cke:html><cke:body bgcolor=#FFFFFF>AVARKOM PMS WEB INTERFACE</cke:body></cke:html>");        

          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}



//=============PROGRAMM====================

void setup() {
  lcd.init();                     
  lcd.backlight();// Включаем подсветку дисплея
  lcd.createChar(1, symbol);                           //  Загружаем символ из массива symbol в первую ячейку ОЗУ дисплея
  analogReference(INTERNAL);
  pinMode(SOURCE_OUTPUT_1, OUTPUT);
  pinMode(SOURCE_OUTPUT_2, OUTPUT);
  pinMode(D4, INPUT);
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  pinMode(D7, INPUT);


  lcd.createChar(2, bukva_I);      // Создаем символ под номером 2
  lcd.createChar(3, bukva_CH);
  lcd.createChar(4, bukva_ZH);      // Создаем символ под номером 4
  lcd.createChar(5, bukva_Y);
  lcd.createChar(6, bukva_D);  

  
  byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xD0, 0x99 };
  IPAddress ip(Ip1,Ip2,Ip3,Ip4);
  Ethernet.begin(mac, ip);
  
  commandServer.begin();
  webServer.begin();
  
  CONTROL_TYPE = AUTO;
  SOURCE_STATE = SOUND;
  
  changeSourceTo(PRIMARY_SOURCE);

  // for easy debugging
  Serial.begin(9600);
}

void keys(){       // выполнять процедуру раз в цикл 
 key_N= btn();          // перем. key_N номер наж кнопки
 };
 
void loop() {
  
  keys(); // ввод
  if(currentMillis - previousMillis > interval && frame_N != 0)  //Если счетчик достиг интервала то отобразить главный экран 
  {
    previousMillis = currentMillis;
    lcd.clear();
    frame_N = 0;
   
  }

  switch (frame_N) {
    case 0:frame_0();
      break;
    case 10:frame_10();
      break;
    case 20:frame_20();
      break;
    case 30:frame_30();
      break;
    case 40:frame_40();
      break;
    case 50:frame_50();
      break;
    case 60:frame_60();
      break;
    case 70:frame_70();
      break;
    case 80:frame_80();
      break;
    case 90:frame_90();
      break;
    case 100:frame_100();
      break;
    case 110:frame_110();
      break;
    case 120:frame_120();
      break;
    case 130:frame_130();
      break;
 }
 
  EthernetClient commandClient = commandServer.available();
  if (receiveCommand(commandClient)){
    executeCommand(commandClient);
  }
  
  processWebClient();
  
  if (CONTROL_TYPE != AUTO){
    return;
  }

  int primLeft = processAnalogValue(PRIMARY_SOURCE_LEFT_INPUT);
  int primRight = processAnalogValue(PRIMARY_SOURCE_RIGHT_INPUT);

  primRight = primLeft; //FOR DEBUG!!!!!
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
      if ((millis() - timeMark) >= SILENCE_TIMEOUT){
         changeSourceTo(SECONDARY_SOURCE);
         SOURCE_STATE = SILENCE;
      }
      else if (soundIsPresent){
         SOURCE_STATE = SOUND;
      }
      return;
      
    case SOUND_APPEARED:
      if ((millis() - timeMark) >= SOUND_TIMEOUT){
         changeSourceTo(PRIMARY_SOURCE);
         SOURCE_STATE = SOUND;
      }
      else if (soundIsAbsent){
         SOURCE_STATE = SILENCE;
      }
      return;
  }
}
void changeStateTo(int state){
CONTROL_TYPE = state;
if (state != AUTO){
changeSourceTo(state);
}
}

// --------------Окно №0------------------
void frame_0(){ 
 int ch1 = (processAnalogValue(PRIMARY_SOURCE_LEFT_INPUT) / 4);
 int ch2 = (processAnalogValue(PRIMARY_SOURCE_RIGHT_INPUT) / 4);
 lcd.setCursor(0,0);
 lcd.print("1");
 lcd.setCursor(1,0);
 levelmetr(ch1);
 lcd.setCursor(0,1);
 lcd.print("2");
 levelmetr(ch2);
 lcd.setCursor(12,0);
 // lcd.print("PE\4\2M ");
 if ((CONTROL_TYPE == 0) || (CONTROL_TYPE == 1)) {
  lcd.print("P\5\3H");
  lcd.setCursor(12,1);
  lcd.print("KAH");
 lcd.print(CURRENT_SOURCE+1);  
 }
 else if (CONTROL_TYPE == 2) {
  lcd.print("ABTO");
  lcd.setCursor(12,1); 
  lcd.print("KAH");
  lcd.print(CURRENT_SOURCE+1);
 }

switch (key_N) {
    case 0:
      lcd.clear();// right
        if (CONTROL_TYPE <= 1) CONTROL_TYPE++;
        else (CONTROL_TYPE=0);
        changeStateTo(CONTROL_TYPE);
      break;   
    case 1:
      lcd.clear();
      frame_N=130;   // up
      break;
    case 2:
      lcd.clear();
      frame_N=10;  // down
      break;
    case 3:
      lcd.clear();// left
        if (CONTROL_TYPE >= 1) CONTROL_TYPE--;
        else (CONTROL_TYPE=2);
        changeStateTo(CONTROL_TYPE);
      break;
 } 
};

// --------------Окно 10 silense_timeout------------------
void frame_10(){ 
 lcd.setCursor(0,0);
 lcd.print(">HA PE3EPB ");
 lcd.print(silence_timeout,DEC);
 lcd.setCursor(13,0);
 lcd.print("CEK");
 lcd.setCursor(0,1);
 lcd.print(" HA OCHOBH ");
 lcd.print(sound_timeout,DEC);
 lcd.setCursor(13,1);
 lcd.print("CEK");
   switch (key_N) {
    case 0:// right
      lcd.clear();
      if (silence_timeout < 20) silence_timeout++; 
      else silence_timeout=1;
      SILENCE_TIMEOUT = silence_timeout*1000;
      break;   
    case 1:
      lcd.clear();
      EEPROM.update(EPR_silence_timeout,silence_timeout);
      frame_N=0; // up
      break;
    case 2:
      lcd.clear();
      EEPROM.update(EPR_silence_timeout,silence_timeout);
      frame_N=20; // down
      break;
    case 3:  // left
      lcd.clear();
      if (silence_timeout > 1) silence_timeout--; 
      else silence_timeout=20;   
      SILENCE_TIMEOUT = silence_timeout*1000;
      break;
 } 
};
// --------------Окно 20 sound_timeout------------------
void frame_20(){ 
 lcd.setCursor(0,0);
 lcd.print(" HA PE3EPB ");
 lcd.print(silence_timeout,DEC);
 lcd.setCursor(13,0);
 lcd.print("CEK");
 lcd.setCursor(0,1);
 lcd.print(">HA OCHOBH ");
 lcd.print(sound_timeout,DEC);
 lcd.setCursor(13,1);
 lcd.print("CEK");
   switch (key_N) {
    case 0:// right
      lcd.clear();
      if (sound_timeout <= 19) sound_timeout++; 
      else sound_timeout = 1;
      SOUND_TIMEOUT = sound_timeout*1000;
      break;   
    case 1:// up
      lcd.clear();
      EEPROM.update(EPR_sound_timeout,sound_timeout);
      frame_N=10; 
      break;
    case 2: // down
      lcd.clear();
      EEPROM.update(EPR_sound_timeout,sound_timeout);
      frame_N=30;
      break;
    case 3: // left
      lcd.clear(); 
      if (sound_timeout > 2) sound_timeout-- ;
      else sound_timeout = 20;
      SOUND_TIMEOUT = sound_timeout*1000;
      break;
 } 
};
// --------------Окно 30 QUIET_TRESHOLD------------------
void frame_30(){ 
 lcd.setCursor(0,0);
 lcd.print(">");
 lcd.print("\6ETEKTOP  ");
 lcd.print(quiet_treshold,DEC);
 lcd.setCursor(14,0);
 lcd.print("%");
 lcd.setCursor(0,1);
 lcd.print(" ");
 lcd.print("BO3BPAT   ");
 lcd.print(loud_treshold,DEC);
 lcd.setCursor(14,1);
 lcd.print("%");
   switch (key_N) {
    case 0:// right
      lcd.clear();
      if (quiet_treshold < 100) quiet_treshold = quiet_treshold + 5; 
      else quiet_treshold = 0;
      QUIET_TRESHOLD = map(quiet_treshold,0,100,0,1023);
      break;   
    case 1:// up
      lcd.clear();
      EEPROM.update(EPR_quiet_treshold,quiet_treshold);
      frame_N=20; 
      break;
    case 2:// down
      lcd.clear();
      EEPROM.update(EPR_quiet_treshold,quiet_treshold);
      frame_N=40; 
      break;
    case 3: // left
      lcd.clear();
      if (quiet_treshold > 1) quiet_treshold = quiet_treshold - 5; 
      else quiet_treshold = 100;
      QUIET_TRESHOLD = map(quiet_treshold,0,100,0,1023);
      break;   
 } 
};
// --------------Окно 40-LOUD_TRESHOLD-----------------
void frame_40(){ 
 lcd.setCursor(0,0);
 lcd.print(" ");
 lcd.print("\6ETEKTOP  ");
 lcd.print(quiet_treshold,DEC);
 lcd.setCursor(14,0);
 lcd.print("%");
 lcd.setCursor(0,1);
 lcd.print(">");
 lcd.print("BO3BPAT   ");
 lcd.print(loud_treshold,DEC);
 lcd.setCursor(14,1);
 lcd.print("%");
   switch (key_N) {
    case 0:// right
      if (loud_treshold < 100) loud_treshold = loud_treshold + 5; 
      else loud_treshold = 0;
      LOUD_TRESHOLD = map(loud_treshold,0,100,0,1023);
      lcd.clear();
      break;   
    case 1:
      lcd.clear();
      frame_N=30; // up
      EEPROM.update(EPR_loud_treshold,loud_treshold);
      break;
    case 2:
      lcd.clear();
      frame_N=50; // down
      EEPROM.update(EPR_loud_treshold,loud_treshold);
      break;
    case 3: // left
      if (loud_treshold > 1) loud_treshold = loud_treshold -5; 
      else loud_treshold = 100;
      LOUD_TRESHOLD = map(loud_treshold,0,100,0,1023);
      lcd.clear();
      break;
 } 
};
// --------------Окно50 Port------------------
void frame_50(){ 
 //lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("PORT:           ");
 lcd.setCursor(0,1);
 lcd.print(">");
 lcd.print(PORT,DEC);
 lcd.print("            ");
  switch (key_N) {
    case 0:PORT++; // right
      break;   
      case 1:
    frame_N=40; // up
      EEPROM.update(EPR_PORT, PORT);
      break;
    case 2:
      EEPROM.update(EPR_PORT, PORT);
      frame_N=60; // down
      break;
    case 3: PORT-- ; // left
      break;
 } 
};

// --------------Окно 60------------------
void frame_60(){ 
 lcd.setCursor(0,0);
 lcd.print("IP ADRESS :     ");
 lcd.setCursor(0,1);
 lcd.print(">");
 lcd.print(Ip1,DEC);
 lcd.print(".");
 lcd.print(Ip2,DEC);
 lcd.print(".");
 lcd.print(Ip3,DEC);
 lcd.print(".");
 lcd.print(Ip4,DEC);
 lcd.print("    ");
  switch (key_N) {
    case 0:Ip1++; // right
      break;   
    case 1: // up
      lcd.clear();
      EEPROM.update(EPR_Ip1, Ip1);
      EEPROM.update(EPR_Ip2, Ip2);
      EEPROM.update(EPR_Ip3, Ip3);
      EEPROM.update(EPR_Ip4, Ip4);
      frame_N=50;
      break;
    case 2:// down
      lcd.clear();
      EEPROM.update(EPR_Ip1, Ip1);
      EEPROM.update(EPR_Ip2, Ip2);
      EEPROM.update(EPR_Ip3, Ip3);
      EEPROM.update(EPR_Ip4, Ip4);
      frame_N=70;
      break;
    case 3: Ip1-- ; // left
      break;
 } 
};

// --------------Окно 70------------------
void frame_70(){ 
 lcd.setCursor(0,0);
 lcd.print("IP ADRESS :     ");
 lcd.setCursor(0,1);
 lcd.print(Ip1,DEC);
 lcd.print(".>");
 lcd.print(Ip2,DEC);
 lcd.print(".");
 lcd.print(Ip3,DEC);
 lcd.print(".");
 lcd.print(Ip4,DEC);
 lcd.print("    ");
   switch (key_N) {
    case 0:Ip2++; // right
      break;   
    case 1:// up
      lcd.clear();
      frame_N=60;
      break;
    case 2:// down
      lcd.clear();
      frame_N=80;
      break;
    case 3: 
      Ip2-- ; // left
      break;
 } 
};

// --------------Окно 80------------------
void frame_80(){ 
 lcd.setCursor(0,0);
 lcd.print("IP ADRESS :     ");
 lcd.setCursor(0,1);
 lcd.print(Ip1,DEC);
 lcd.print(".");
 lcd.print(Ip2,DEC);
 lcd.print(".>");
 lcd.print(Ip3,DEC);
 lcd.print(".");
 lcd.print(Ip4,DEC);
 lcd.print("    ");
   switch (key_N) {
    case 0:Ip3++; // right
      break;   
    case 1:// up
      lcd.clear();
      EEPROM.update(EPR_Ip1, Ip1);
      EEPROM.update(EPR_Ip2, Ip2);
      EEPROM.update(EPR_Ip3, Ip3);
      EEPROM.update(EPR_Ip4, Ip4);
      frame_N=70; 
      break;
    case 2:// down
      lcd.clear();
      EEPROM.update(EPR_Ip1, Ip1);
      EEPROM.update(EPR_Ip2, Ip2);
      EEPROM.update(EPR_Ip3, Ip3);
      EEPROM.update(EPR_Ip4, Ip4);
      frame_N=90; 
      break;
    case 3: Ip3-- ; // left
      break;
 } 
};

// --------------Окно 90------------------
void frame_90(){ 
 lcd.setCursor(0,0);
 lcd.print("IP ADRESS :     ");
 lcd.setCursor(0,1);
 lcd.print(Ip1,DEC);
 lcd.print(".");
 lcd.print(Ip2,DEC);
 lcd.print(".");
 lcd.print(Ip3,DEC);
 lcd.print(".>");
 lcd.print(Ip4,DEC);
 lcd.print("      ");
   switch (key_N) {
    case 0: // right
      Ip4++ ; 
      break;   
    case 1: // up
    lcd.clear();
      EEPROM.update(EPR_Ip1, Ip1);
      EEPROM.update(EPR_Ip2, Ip2);
      EEPROM.update(EPR_Ip3, Ip3);
      EEPROM.update(EPR_Ip4, Ip4);   
      frame_N=80;
      break;
    case 2:// down
    lcd.clear();
      EEPROM.update(EPR_Ip1, Ip1);
      EEPROM.update(EPR_Ip2, Ip2);
      EEPROM.update(EPR_Ip3, Ip3);
      EEPROM.update(EPR_Ip4, Ip4);
      frame_N=100; 
      break;
    case 3: // left
      Ip4--; 
      break;
 } 
};
// --------------Окно 100------------------
void frame_100(){ 
 lcd.setCursor(0,0);
 lcd.print("MASK :          ");
 lcd.setCursor(0,1);
 lcd.print(">");
 lcd.print(Mask1,DEC);
 lcd.print(".");
 lcd.print(Mask2,DEC);
 lcd.print(".");
 lcd.print(Mask3,DEC);
 lcd.print(".");
 lcd.print(Mask4,DEC);
 lcd.print("        ");
   switch (key_N) {
    case 0:// right
      Mask1++; 
      break;   
    case 1:// up
      lcd.clear();
      EEPROM.update(EPR_Mask1, Mask1);
      EEPROM.update(EPR_Mask2, Mask2);
      EEPROM.update(EPR_Mask3, Mask3);
      EEPROM.update(EPR_Mask4, Mask4); 
      frame_N=90;
    break;
    case 2:// down
      EEPROM.update(EPR_Mask1, Mask1);
      EEPROM.update(EPR_Mask2, Mask2);
      EEPROM.update(EPR_Mask3, Mask3);
      EEPROM.update(EPR_Mask4, Mask4); 
      frame_N=110;
      lcd.clear();
      break;
    case 3: // left
      Mask1-- ; 
      break;
 } 
};
// --------------Окно 110------------------
void frame_110(){ 
 lcd.setCursor(0,0);
 lcd.print("MASK :          ");
 lcd.setCursor(0,1);
 lcd.print(Mask1,DEC);
 lcd.print(".>");
 lcd.print(Mask2,DEC);
 lcd.print(".");
 lcd.print(Mask3,DEC);
 lcd.print(".");
 lcd.print(Mask4,DEC);
 lcd.print("       ");
   switch (key_N) {
    case 0:// right
      Mask2++; 
      break;   
    case 1:// up
      lcd.clear();
      EEPROM.update(EPR_Mask1, Mask1);
      EEPROM.update(EPR_Mask2, Mask2);
      EEPROM.update(EPR_Mask3, Mask3);
      EEPROM.update(EPR_Mask4, Mask4); 
      frame_N=100; 
      break;
    case 2:// down
      lcd.clear();
      EEPROM.update(EPR_Mask1, Mask1);
      EEPROM.update(EPR_Mask2, Mask2);
      EEPROM.update(EPR_Mask3, Mask3);
      EEPROM.update(EPR_Mask4, Mask4); 
      frame_N=120; 
      break;
    case 3: // left
      Mask2-- ; 
      break;
 } 
};
// --------------Окно 120------------------
void frame_120(){ 
 lcd.setCursor(0,0);
 lcd.print("MASK :          ");
 lcd.setCursor(0,1);
 lcd.print(Mask1,DEC);
 lcd.print(".");
 lcd.print(Mask2,DEC);
 lcd.print(".>");
 lcd.print(Mask3,DEC);
 lcd.print(".");
 lcd.print(Mask4,DEC);
 lcd.print("        ");
   switch (key_N) {
    case 0:// right
      Mask3++; 
      break;   
    case 1:// up
      lcd.clear();
      EEPROM.update(EPR_Mask1, Mask1);
      EEPROM.update(EPR_Mask2, Mask2);
      EEPROM.update(EPR_Mask3, Mask3);
      EEPROM.update(EPR_Mask4, Mask4); 
      frame_N=110; 
      break;
    case 2:// down
      lcd.clear();
      EEPROM.update(EPR_Mask1, Mask1);
      EEPROM.update(EPR_Mask2, Mask2);
      EEPROM.update(EPR_Mask3, Mask3);
      EEPROM.update(EPR_Mask4, Mask4); 
    frame_N=130; 
      break;
    case 3: // left
      Mask3-- ; 
      break;
 } 
};
// --------------Окно 130------------------
void frame_130(){ 
 lcd.setCursor(0,0);
 lcd.print("MASK :          ");
 lcd.setCursor(0,1);
 lcd.print(Mask1,DEC);
 lcd.print(".");
 lcd.print(Mask2,DEC);
 lcd.print(".");
 lcd.print(Mask3,DEC);
 lcd.print(".>");
 lcd.print(Mask4,DEC);
 lcd.print("        ");
   switch (key_N) {
    case 0:// right
      Mask4++; 
      break;   
    case 1:// up
      lcd.clear();
      EEPROM.update(EPR_Mask1, Mask1);
      EEPROM.update(EPR_Mask2, Mask2);
      EEPROM.update(EPR_Mask3, Mask3);
      EEPROM.update(EPR_Mask4, Mask4); 
      frame_N=120; 
      break;
    case 2: // down
      lcd.clear();
      EEPROM.update(EPR_Mask1, Mask1);
      EEPROM.update(EPR_Mask2, Mask2);
      EEPROM.update(EPR_Mask3, Mask3);
      EEPROM.update(EPR_Mask4, Mask4); 
      frame_N=0;
      break;
    case 3: // left
      Mask4-- ; 
      break;
 } 
};

