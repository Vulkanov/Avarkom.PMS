/*
 * В ЭТОМ ФАЙЛЕ НАХОДИТСЯ ВСЕ, ЧТО СВЯЗАНО С 
 * МЕНЮ НА LCD ДИСПЛЕЕ
 */

#define Pin4  4  //          Up
#define Pin5  5  //          Dwn
#define Pin6  6  //         Left
#define Pin7  7  //         Rght
#define btnRIGHT  0// кнопка RIGHT
#define btnUP     1// кнопка  UP
#define btnDOWN   2// кнопка  DOWN
#define btnLEFT   3// кнопка  LEFT
#define btnNONE   5// кнопки  не нажаты

//byte bukva_B[8]   = {B11110,B10000,B10000,B11110,B10001,B10001,B11110,B00000,}; // Буква "Б"
//byte bukva_G[8]   = {B11111,B10001,B10000,B10000,B10000,B10000,B10000,B00000,}; // Буква "Г"
byte bukva_D[8]   = {B01111,B00101,B00101,B01001,B10001,B11111,B10001,B00000,}; // Буква "Д"
byte bukva_ZH[8]  = {B10101,B10101,B10101,B11111,B10101,B10101,B10101,B00000,}; // Буква "Ж"
//byte bukva_Z[8]   = {B01110,B10001,B00001,B00010,B00001,B10001,B01110,B00000,}; // Буква "З"
//byte bukva_I[8]   = {B10001,B10011,B10011,B10101,B11001,B11001,B10001,B00000,}; // Буква "И"
//byte bukva_IY[8]  = {B01110,B00000,B10001,B10011,B10101,B11001,B10001,B00000,}; // Буква "Й"
byte bukva_L[8]   = {B00011,B00111,B00101,B00101,B01101,B01001,B11001,B00000,}; // Буква "Л"
//byte bukva_P[8]   = {B11111,B10001,B10001,B10001,B10001,B10001,B10001,B00000,}; // Буква "П"
byte bukva_Y[8]   = {B10001,B10001,B10001,B01010,B00100,B01000,B10000,B00000,}; // Буква "У"
//byte bukva_F[8]   = {B00100,B11111,B10101,B10101,B11111,B00100,B00100,B00000,}; // Буква "Ф"
//byte bukva_TS[8]  = {B10010,B10010,B10010,B10010,B10010,B10010,B11111,B00001,}; // Буква "Ц"
byte bukva_CH[8]  = {B10001,B10001,B10001,B01111,B00001,B00001,B00001,B00000,}; // Буква "Ч"
//byte bukva_Sh[8]  = {B10101,B10101,B10101,B10101,B10101,B10101,B11111,B00000,}; // Буква "Ш"
//byte bukva_Shch[8]= {B10101,B10101,B10101,B10101,B10101,B10101,B11111,B00001,}; // Буква "Щ"
//byte bukva_Mz[8]  = {B10000,B10000,B10000,B11110,B10001,B10001,B11110,B00000,}; // Буква "Ь"
byte bukva_IYI[8] = {B10001,B10001,B10001,B11001,B10101,B10101,B11001,B00000,}; // Буква "Ы"
//byte bukva_Yu[8]  = {B10010,B10101,B10101,B11101,B10101,B10101,B10010,B00000,}; // Буква "Ю"
//byte bukva_Ya[8]  = {B01111,B10001,B10001,B01111,B00101,B01001,B10001,B00000,}; // Буква "Я"

// Переменные Меню
byte D4 =4; //кнопка right
byte D5 =5; //кнопка down
byte D6 =6; //кнопка up
byte D7 =7; //кнопка left
long previousMillis = 0; //счетчик прошедшего времени для AutoMainScreen
long interval = 60000; //задержка автовозврата к MainScreen 3сек
unsigned long currentMillis; // текущее сохраненное значение времени
byte key_N=5; //номер нажатой кнопки
byte frame_N=0; // номер показ окна

//индикатор уровня
uint8_t symbol0[8] = {B00000,B10101,B10101,B10101,B10101,B10101,B00000,B00000,}; //Определяем массив который содержит полностью закрашенный символ
uint8_t symbol1[8] = {B00000,B00000,B00000,B10101,B00000,B00000,B00000,B00000,};

void initializeMenu(){
  lcd.init();                     
  lcd.backlight();// Включаем подсветку дисплея
  lcd.createChar(0, symbol0);   
  lcd.createChar(1, symbol1);  //  Загружаем символ из массива symbol0 в нулевую ячейку ОЗУ дисплея
 
  
  pinMode(D4, INPUT);
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  pinMode(D7, INPUT);

  lcd.createChar(2, bukva_Y);      // Создаем символ под номером 2
  lcd.createChar(3, bukva_CH);
//  lcd.createChar(4, bukva_G);      // Создаем символ под номером 4
  lcd.createChar(5, bukva_IYI);
  lcd.createChar(6, bukva_D); 
  lcd.createChar(7, bukva_L);  
}

//драйвер кнопок
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
}

void keys(){       // выполнять процедуру раз в цикл 
 key_N= btn();          // перем. key_N номер наж кнопки

//Если счетчик достиг интервала и мы не находимся в главном меню, то отобразить главное меню 
  if(currentMillis - previousMillis > interval && frame_N != 0)  
  {
    previousMillis = currentMillis;
    lcd.clear();
    EEPROM.update(EPR_quiet_timeout, QUIET_TIMEOUT);
    EEPROM.update(EPR_loud_timeout, LOUD_TIMEOUT);
    EEPROM.update(EPR_quiet_treshold, QUIET_TRESHOLD);
    EEPROM.update(EPR_loud_treshold, LOUD_TRESHOLD);
    EEPROM.update(EPR_PORT, PORT);
    EEPROM.update(EPR_USE_DHCP, USE_DHCP);
    updateOctetsInEEPROM(ip, EPR_Ip);
    updateOctetsInEEPROM(netmask, EPR_Mask);
    /*
    EEPROM.update(EPR_Ip1, Ip1);
    EEPROM.update(EPR_Ip2, Ip2);
    EEPROM.update(EPR_Ip3, Ip3);
    EEPROM.update(EPR_Ip4, Ip4);
    EEPROM.update(EPR_Mask1, Mask1);
    EEPROM.update(EPR_Mask2, Mask2);
    EEPROM.update(EPR_Mask3, Mask3);
    EEPROM.update(EPR_Mask4, Mask4); 
    */
    frame_N = 0;
  }

  switch (frame_N) {
    case 0:frame_0();
      break;
    case 1:frame_1();
      break;
    case 2:frame_2();
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
    case 55:frame_55();
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
}

// --------------Окно №0------------------
void frame_0(){ 
 // считываем входные значения уровней звука для индикаторов уровня
 int ch0 = processAnalogValue(PRIMARY_SOURCE_LEFT_INPUT);
 int ch1 = processAnalogValue(PRIMARY_SOURCE_RIGHT_INPUT);
 int ch2 = processAnalogValue(SECONDARY_SOURCE_LEFT_INPUT);
 int ch3 = processAnalogValue(SECONDARY_SOURCE_RIGHT_INPUT);
 // усредняем данные для отображения правый + левый канал
 int chMAIN = (ch0 + ch1) / 2;
 int chRESERV = (ch2 + ch3) / 2;
 lcd.setCursor(0,0);
 lcd.print("1");
 lcd.setCursor(1,0);
 // отправляем данные на иникатор уровня. выводим индикатор на дисплей
 levelmetr(chMAIN);
 // Serial.println(ch0); // для отладки - смотрим уровень в ком порте
 lcd.setCursor(0,1);
 lcd.print("2");
 levelmetr(chRESERV);
 lcd.setCursor(12,0);
 if ((CONTROL_TYPE == 0) || (CONTROL_TYPE == 1)) {
  lcd.print("P\2\3H");
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
        if (CONTROL_TYPE < 2) CONTROL_TYPE++;
        else (CONTROL_TYPE=0);
        changeStateTo(CONTROL_TYPE);
      break;   
    case 1:
      lcd.clear();
      frame_N=130;   // up
      break;
    case 2:
      lcd.clear();
      frame_N=1;  // down
      break;
    case 3:
      lcd.clear();// left
        if (CONTROL_TYPE >= 1) CONTROL_TYPE--;
        else (CONTROL_TYPE=2);
        changeStateTo(CONTROL_TYPE);
      break;
 } 
};

// --------------Окно №1 индикатор уровня канал 1 (левый/Правый)------------------
void frame_1(){ 
 // считываем входные значения уровней звука для индикаторов уровня
 int ch0 = processAnalogValue(PRIMARY_SOURCE_LEFT_INPUT);
 int ch1 = processAnalogValue(PRIMARY_SOURCE_RIGHT_INPUT);
 lcd.setCursor(0,0);
 lcd.print("L");
 lcd.setCursor(1,0);
 // отправляем данные на иникатор уровня. выводим индикатор на дисплей
 levelmetr(ch0);
 // Serial.println(ch0); // для отладки - смотрим уровень в ком порте
 lcd.setCursor(0,1);
 lcd.print("R");
 levelmetr(ch1);
 lcd.setCursor(12,0);
  lcd.print("\2POB");
  lcd.setCursor(12,1);
  lcd.print("KAH1");
 
switch (key_N) {
    case 0:// right
      break;   
    case 1:
      lcd.clear();
      frame_N=0;   // up
      break;
    case 2:
      lcd.clear();
      frame_N=2;  // down
      break;
    case 3:// left
      break;
 } 
};

// --------------Окно №2 индикатор уровня канал 2 (левый/Правый)------------------
void frame_2(){ 
 // считываем входные значения уровней звука для индикаторов уровня
 int ch2 = processAnalogValue(SECONDARY_SOURCE_LEFT_INPUT);
 int ch3 = processAnalogValue(SECONDARY_SOURCE_RIGHT_INPUT);
 lcd.setCursor(0,0);
 lcd.print("L");
 lcd.setCursor(1,0);
 // отправляем данные на иникатор уровня. выводим индикатор на дисплей
 levelmetr(ch2);
 // Serial.println(ch0); // для отладки - смотрим уровень в ком порте
 lcd.setCursor(0,1);
 lcd.print("R");
 levelmetr(ch3);
 lcd.setCursor(12,0);
  lcd.print("\2POB");
  lcd.setCursor(12,1);
  lcd.print("KAH2");
 
switch (key_N) {
    case 0:// right
      break;   
    case 1:
      lcd.clear();
      frame_N=1;   // up
      break;
    case 2:
      lcd.clear();
      frame_N=10;  // down
      break;
    case 3:// left
      break;
 } 
};

// --------------Окно 10 silense_timeout------------------
void frame_10(){ 
 lcd.setCursor(0,0);
 lcd.print(">HA PE3EPB ");
 lcd.print(QUIET_TIMEOUT, DEC);
 lcd.setCursor(13, 0);
 lcd.print("CEK");
 lcd.setCursor(0, 1);
 lcd.print(" HA OCHOBH ");
 lcd.print(LOUD_TIMEOUT, DEC);
 lcd.setCursor(13,1);
 lcd.print("CEK");
   switch (key_N) {
    case 0:// right
      lcd.clear();
      if (QUIET_TIMEOUT < 20) QUIET_TIMEOUT++; 
      else QUIET_TIMEOUT=1;
      break;   
    case 1:
      lcd.clear();
      EEPROM.update(EPR_quiet_timeout, QUIET_TIMEOUT);
      frame_N=2; // up
      break;
    case 2:
      lcd.clear();
      EEPROM.update(EPR_quiet_timeout, QUIET_TIMEOUT);
      frame_N=20; // down
      break;
    case 3:  // left
      lcd.clear();
      if (QUIET_TIMEOUT > 1) QUIET_TIMEOUT--; 
      else QUIET_TIMEOUT=20;   
      break;
 } 
};
// --------------Окно 20 LOUD_TIMEOUT------------------
void frame_20(){ 
 lcd.setCursor(0,0);
 lcd.print(" HA PE3EPB ");
 lcd.print(QUIET_TIMEOUT,DEC);
 lcd.setCursor(13,0);
 lcd.print("CEK");
 lcd.setCursor(0,1);
 lcd.print(">HA OCHOBH ");
 lcd.print(LOUD_TIMEOUT,DEC);
 lcd.setCursor(13,1);
 lcd.print("CEK");
   switch (key_N) {
    case 0:// right
      lcd.clear();
      if (LOUD_TIMEOUT <= 19) LOUD_TIMEOUT++; 
      else LOUD_TIMEOUT = 1;
      break;   
    case 1:// up
      lcd.clear();
      EEPROM.update(EPR_loud_timeout, LOUD_TIMEOUT);
      frame_N=10; 
      break;
    case 2: // down
      lcd.clear();
      EEPROM.update(EPR_loud_timeout, LOUD_TIMEOUT);
      frame_N=30;
      break;
    case 3: // left
      lcd.clear(); 
      if (LOUD_TIMEOUT > 2) LOUD_TIMEOUT-- ;
      else LOUD_TIMEOUT = 20;
      break;
 } 
};
// --------------Окно 30 QUIET_TRESHOLD------------------
void frame_30(){ 
 lcd.setCursor(0,0);
 lcd.print(">");
 lcd.print("\6ETEKTOP  ");
 lcd.print(QUIET_TRESHOLD,DEC);
 lcd.setCursor(14,0);
 lcd.print("%");
 lcd.setCursor(0,1);
 lcd.print(" ");
 lcd.print("BO3BPAT   ");
 lcd.print(LOUD_TRESHOLD,DEC);
 lcd.setCursor(14,1);
 lcd.print("%");
   switch (key_N) {
    case 0:// right
      lcd.clear();
      if (QUIET_TRESHOLD < 100) QUIET_TRESHOLD = QUIET_TRESHOLD + 5; 
      else QUIET_TRESHOLD = 5;
      break;   
    case 1:// up
      lcd.clear();
      EEPROM.update(EPR_quiet_treshold, QUIET_TRESHOLD);
      frame_N=20; 
      break;
    case 2:// down
      lcd.clear();
      EEPROM.update(EPR_quiet_treshold, QUIET_TRESHOLD);
      frame_N=40; 
      break;
    case 3: // left
      lcd.clear();
      if (QUIET_TRESHOLD > 5) QUIET_TRESHOLD = QUIET_TRESHOLD - 5; 
      else QUIET_TRESHOLD = 100;
      break;   
 } 
};
// --------------Окно 40-LOUD_TRESHOLD-----------------
void frame_40(){ 
 lcd.setCursor(0,0);
 lcd.print(" ");
 lcd.print("\6ETEKTOP  ");
 lcd.print(QUIET_TRESHOLD,DEC);
 lcd.setCursor(14,0);
 lcd.print("%");
 lcd.setCursor(0,1);
 lcd.print(">");
 lcd.print("BO3BPAT   ");
 lcd.print(LOUD_TRESHOLD,DEC);
 lcd.setCursor(14,1);
 lcd.print("%");
   switch (key_N) {
    case 0:// right
      if (LOUD_TRESHOLD < 100) LOUD_TRESHOLD = LOUD_TRESHOLD + 5; 
      else LOUD_TRESHOLD = 5;
      lcd.clear();
      break;   
    case 1:
      lcd.clear();
      frame_N=30; // up
      EEPROM.update(EPR_loud_treshold, LOUD_TRESHOLD);
      break;
    case 2:
      lcd.clear();
      frame_N=50; // down
      EEPROM.update(EPR_loud_treshold, LOUD_TRESHOLD);
      break;
    case 3: // left
      if (LOUD_TRESHOLD > 5) LOUD_TRESHOLD = LOUD_TRESHOLD -5; 
      else LOUD_TRESHOLD = 100;
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
      frame_N=55; // down
      break;
    case 3: PORT-- ; // left
      break;
 } 
};

// --------------Окно 55 DHCP------------------
void frame_55(){ 
 lcd.setCursor(0,0);
 lcd.print("DHCP :          ");
 lcd.setCursor(0,1);
 lcd.print(">");
 if (USE_DHCP > 0) lcd.print("BK\7");
 else lcd.print("B\5K\7");
  lcd.print("            ");
  switch (key_N) {
    case 0: // right
    if (USE_DHCP < 1) USE_DHCP++; 
    else USE_DHCP = 0; 
      break;   
    case 1: // up
      lcd.clear();
      EEPROM.update(EPR_USE_DHCP, USE_DHCP);
      /*
      EEPROM.update(EPR_Ip1, Ip1);
      EEPROM.update(EPR_Ip2, Ip2);
      EEPROM.update(EPR_Ip3, Ip3);
      EEPROM.update(EPR_Ip4, Ip4);
      */
      frame_N=50;
      break;
    case 2:// down
      lcd.clear();
      EEPROM.update(EPR_USE_DHCP, USE_DHCP);
      /*
      EEPROM.update(EPR_Ip1, Ip1);
      EEPROM.update(EPR_Ip2, Ip2);
      EEPROM.update(EPR_Ip3, Ip3);
      EEPROM.update(EPR_Ip4, Ip4);
      */
      frame_N=60;
      break;
    case 3: // left
    if (USE_DHCP > 0) USE_DHCP--; 
    else USE_DHCP = 1; 
      break;
 } 
};

// --------------Окно 60------------------
void frame_60(){ 
 lcd.setCursor(0,0);
 lcd.print("IP ADRESS :     ");
 lcd.setCursor(0,1);
 lcd.print(">");
 lcd.print(ip[0],DEC);
 lcd.print(".");
 lcd.print(ip[1],DEC);
 lcd.print(".");
 lcd.print(ip[2],DEC);
 lcd.print(".");
 lcd.print(ip[3],DEC);
 lcd.print("    ");
  switch (key_N) {
    case 0:ip[0]++; // right
      break;   
    case 1: // up
      lcd.clear();
      updateOctetsInEEPROM(ip, EPR_Ip);
      /*
      EEPROM.update(EPR_Ip1, Ip1);
      EEPROM.update(EPR_Ip2, Ip2);
      EEPROM.update(EPR_Ip3, Ip3);
      EEPROM.update(EPR_Ip4, Ip4);
      */
      frame_N=55;
      break;
    case 2:// down
      lcd.clear();
      updateOctetsInEEPROM(ip, EPR_Ip);
      /*
      EEPROM.update(EPR_Ip1, Ip1);
      EEPROM.update(EPR_Ip2, Ip2);
      EEPROM.update(EPR_Ip3, Ip3);
      EEPROM.update(EPR_Ip4, Ip4);
      */
      frame_N=70;
      break;
    case 3: ip[0]-- ; // left
      break;
 } 
};

// --------------Окно 70------------------
void frame_70(){ 
 lcd.setCursor(0,0);
 lcd.print("IP ADRESS :     ");
 lcd.setCursor(0,1);
 lcd.print(ip[0],DEC);
 lcd.print(".>");
 lcd.print(ip[1],DEC);
 lcd.print(".");
 lcd.print(ip[2],DEC);
 lcd.print(".");
 lcd.print(ip[3],DEC);
 lcd.print("    ");
   switch (key_N) {
    case 0:ip[1]++; // right
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
      ip[1]-- ; // left
      break;
 } 
};

// --------------Окно 80------------------
void frame_80(){ 
 lcd.setCursor(0,0);
 lcd.print("IP ADRESS :     ");
 lcd.setCursor(0,1);
 lcd.print(ip[0],DEC);
 lcd.print(".");
 lcd.print(ip[1],DEC);
 lcd.print(".>");
 lcd.print(ip[2],DEC);
 lcd.print(".");
 lcd.print(ip[3],DEC);
 lcd.print("    ");
   switch (key_N) {
    case 0:ip[2]++; // right
      break;   
    case 1:// up
      lcd.clear();
      updateOctetsInEEPROM(ip, EPR_Ip);
      /*
      EEPROM.update(EPR_Ip1, Ip1);
      EEPROM.update(EPR_Ip2, Ip2);
      EEPROM.update(EPR_Ip3, Ip3);
      EEPROM.update(EPR_Ip4, Ip4);
      */
      frame_N=70; 
      break;
    case 2:// down
      lcd.clear();
      updateOctetsInEEPROM(ip, EPR_Ip);
      /*
      EEPROM.update(EPR_Ip1, Ip1);
      EEPROM.update(EPR_Ip2, Ip2);
      EEPROM.update(EPR_Ip3, Ip3);
      EEPROM.update(EPR_Ip4, Ip4);
      */
      frame_N=90; 
      break;
    case 3: ip[2]-- ; // left
      break;
 } 
};

// --------------Окно 90------------------
void frame_90(){ 
 lcd.setCursor(0,0);
 lcd.print("IP ADRESS :     ");
 lcd.setCursor(0,1);
 lcd.print(ip[0],DEC);
 lcd.print(".");
 lcd.print(ip[1],DEC);
 lcd.print(".");
 lcd.print(ip[2],DEC);
 lcd.print(".>");
 lcd.print(ip[3],DEC);
 lcd.print("      ");
   switch (key_N) {
    case 0: // right
      ip[3]++ ; 
      break;   
    case 1: // up
    lcd.clear();
    updateOctetsInEEPROM(ip, EPR_Ip);
    /*
      EEPROM.update(EPR_Ip1, Ip1);
      EEPROM.update(EPR_Ip2, Ip2);
      EEPROM.update(EPR_Ip3, Ip3);
      EEPROM.update(EPR_Ip4, Ip4); 
      */  
      frame_N=80;
      break;
    case 2:// down
    lcd.clear();
    updateOctetsInEEPROM(ip, EPR_Ip);
    /*
      EEPROM.update(EPR_Ip1, Ip1);
      EEPROM.update(EPR_Ip2, Ip2);
      EEPROM.update(EPR_Ip3, Ip3);
      EEPROM.update(EPR_Ip4, Ip4);
      */
      frame_N=100; 
      break;
    case 3: // left
      ip[3]--; 
      break;
 } 
};
// --------------Окно 100------------------
void frame_100(){ 
 lcd.setCursor(0,0);
 lcd.print("MASK :          ");
 lcd.setCursor(0,1);
 lcd.print(">");
 lcd.print(netmask[0],DEC);
 lcd.print(".");
 lcd.print(netmask[1],DEC);
 lcd.print(".");
 lcd.print(netmask[2],DEC);
 lcd.print(".");
 lcd.print(netmask[3],DEC);
 lcd.print("        ");
   switch (key_N) {
    case 0:// right
      netmask[0]++; 
      break;   
    case 1:// up
      lcd.clear();
      updateOctetsInEEPROM(netmask, EPR_Mask);
      /*
      EEPROM.update(EPR_Mask1, Mask1);
      EEPROM.update(EPR_Mask2, Mask2);
      EEPROM.update(EPR_Mask3, Mask3);
      EEPROM.update(EPR_Mask4, Mask4); 
      */
      frame_N=90;
    break;
    case 2:// down
      updateOctetsInEEPROM(netmask, EPR_Mask);
    /*
      EEPROM.update(EPR_Mask1, Mask1);
      EEPROM.update(EPR_Mask2, Mask2);
      EEPROM.update(EPR_Mask3, Mask3);
      EEPROM.update(EPR_Mask4, Mask4); 
      */
      frame_N=110;
      lcd.clear();
      break;
    case 3: // left
      netmask[0]-- ; 
      break;
 } 
};
// --------------Окно 110------------------
void frame_110(){ 
 lcd.setCursor(0,0);
 lcd.print("MASK :          ");
 lcd.setCursor(0,1);
 lcd.print(netmask[0],DEC);
 lcd.print(".>");
 lcd.print(netmask[1],DEC);
 lcd.print(".");
 lcd.print(netmask[2],DEC);
 lcd.print(".");
 lcd.print(netmask[3],DEC);
 lcd.print("       ");
   switch (key_N) {
    case 0:// right
      netmask[1]++; 
      break;   
    case 1:// up
      lcd.clear();
      updateOctetsInEEPROM(netmask, EPR_Mask);
      /*
      EEPROM.update(EPR_Mask1, Mask1);
      EEPROM.update(EPR_Mask2, Mask2);
      EEPROM.update(EPR_Mask3, Mask3);
      EEPROM.update(EPR_Mask4, Mask4); 
      */
      frame_N=100; 
      break;
    case 2:// down
      lcd.clear();
      updateOctetsInEEPROM(netmask, EPR_Mask);
      /*
      EEPROM.update(EPR_Mask1, Mask1);
      EEPROM.update(EPR_Mask2, Mask2);
      EEPROM.update(EPR_Mask3, Mask3);
      EEPROM.update(EPR_Mask4, Mask4); 
      */
      frame_N=120; 
      break;
    case 3: // left
      netmask[1]-- ; 
      break;
 } 
};
// --------------Окно 120------------------
void frame_120(){ 
 lcd.setCursor(0,0);
 lcd.print("MASK :          ");
 lcd.setCursor(0,1);
 lcd.print(netmask[0],DEC);
 lcd.print(".");
 lcd.print(netmask[1],DEC);
 lcd.print(".>");
 lcd.print(netmask[2],DEC);
 lcd.print(".");
 lcd.print(netmask[3],DEC);
 lcd.print("        ");
   switch (key_N) {
    case 0:// right
      netmask[2]++; 
      break;   
    case 1:// up
      lcd.clear();
      updateOctetsInEEPROM(netmask, EPR_Mask);
      /*
      EEPROM.update(EPR_Mask1, Mask1);
      EEPROM.update(EPR_Mask2, Mask2);
      EEPROM.update(EPR_Mask3, Mask3);
      EEPROM.update(EPR_Mask4, Mask4); 
      */
      frame_N=110; 
      break;
    case 2:// down
      lcd.clear();
      updateOctetsInEEPROM(netmask, EPR_Mask);
      /*
      EEPROM.update(EPR_Mask1, Mask1);
      EEPROM.update(EPR_Mask2, Mask2);
      EEPROM.update(EPR_Mask3, Mask3);
      EEPROM.update(EPR_Mask4, Mask4);
      */ 
    frame_N=130; 
      break;
    case 3: // left
      netmask[2]-- ; 
      break;
 } 
};
// --------------Окно 130------------------
void frame_130(){ 
 lcd.setCursor(0,0);
 lcd.print("MASK :          ");
 lcd.setCursor(0,1);
 lcd.print(netmask[0],DEC);
 lcd.print(".");
 lcd.print(netmask[1],DEC);
 lcd.print(".");
 lcd.print(netmask[2],DEC);
 lcd.print(".>");
 lcd.print(netmask[3],DEC);
 lcd.print("        ");
   switch (key_N) {
    case 0:// right
      netmask[3]++; 
      break;   
    case 1:// up
      lcd.clear();
      updateOctetsInEEPROM(netmask, EPR_Mask);
      /*
      EEPROM.update(EPR_Mask1, Mask1);
      EEPROM.update(EPR_Mask2, Mask2);
      EEPROM.update(EPR_Mask3, Mask3);
      EEPROM.update(EPR_Mask4, Mask4); 
      */
      frame_N=120; 
      break;
    case 2: // down
      lcd.clear();
      updateOctetsInEEPROM(netmask, EPR_Mask);
      /*
      EEPROM.update(EPR_Mask1, Mask1);
      EEPROM.update(EPR_Mask2, Mask2);
      EEPROM.update(EPR_Mask3, Mask3);
      EEPROM.update(EPR_Mask4, Mask4);
      */ 
      frame_N=0;
      break;
    case 3: // left
      netmask[3]-- ; 
      break;
 } 
};



