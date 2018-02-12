/*
 * В ЭТОМ ФАЙЛЕ НАХОДИТСЯ ВСЕ, ЧТО СВЯЗАНО С 
 * МЕНЮ НА LCD ДИСПЛЕЕ
 */

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
    EEPROM.update(EPR_silence_timeout,silence_timeout);
    EEPROM.update(EPR_sound_timeout,sound_timeout);
    EEPROM.update(EPR_quiet_treshold,quiet_treshold);
    EEPROM.update(EPR_loud_treshold,loud_treshold);
    EEPROM.update(EPR_PORT, PORT);
    EEPROM.update(EPR_Ip1, Ip1);
    EEPROM.update(EPR_Ip2, Ip2);
    EEPROM.update(EPR_Ip3, Ip3);
    EEPROM.update(EPR_Ip4, Ip4);
    EEPROM.update(EPR_Mask1, Mask1);
    EEPROM.update(EPR_Mask2, Mask2);
    EEPROM.update(EPR_Mask3, Mask3);
    EEPROM.update(EPR_Mask4, Mask4); 
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
}

// --------------Окно №0------------------
void frame_0(){ 
 // считываем входные значения уровней звука для индикаторов уровня
 long ch0 = processAnalogValue(PRIMARY_SOURCE_LEFT_INPUT);
 long ch1 = processAnalogValue(PRIMARY_SOURCE_RIGHT_INPUT);
 long ch2 = processAnalogValue(SECONDARY_SOURCE_LEFT_INPUT);
 long ch3 = processAnalogValue(SECONDARY_SOURCE_RIGHT_INPUT);
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
      else quiet_treshold = 5;
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
      if (quiet_treshold > 5) quiet_treshold = quiet_treshold - 5; 
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
      else loud_treshold = 5;
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
      if (loud_treshold > 5) loud_treshold = loud_treshold -5; 
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



