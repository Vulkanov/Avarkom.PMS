/*
 * В ЭТОМ ФАЙЛЕ НАХОДИТСЯ ВСЕ, ЧТО СВЯЗАНО С ОБРАБОТКОЙ
 * И АНАЛИЗОМ ЗВУКОВЫХ СИГНАЛОВ
 */

//считывание и усреднение сигнала
long processAnalogValue(int channel){
  long sygnal = 0;

  for (int i=0; i < NUM_OF_SAMPLES; i++) {
    
    sygnal += analogRead(channel);
    delayMicroseconds(SAMPLING_DELAY);
  }
  sygnal = sygnal / NUM_OF_SAMPLES - 554;
  if (sygnal < 0) sygnal = -sygnal;
  sygnal = map(sygnal,0,511,0,1023);
  return sygnal ; 
}


bool sourceIsQuiet(int ch1, int ch2){
  return (ch1 <= QUIET_TRESHOLD) || (ch2 <= QUIET_TRESHOLD);
}


bool sourceIsLoud(int ch1, int ch2){
  return (ch1 >= LOUD_TRESHOLD) && (ch2 >= LOUD_TRESHOLD);
}

void levelmetr(int valSensor){
  if (valSensor < 20) lcd.print("HET 3B\5KA!");
  else if (valSensor > 8) {
    uint8_t j=map(valSensor,0,1023,0,12); //  Определяем переменную j которой присваиваем значение valSensor преобразованное от диапазона 0...1023 к диапазону 0...17
    for(uint8_t i=0; i<10; i++){    //  Выполняем цикл 16 раз для вывода шкалы из 16 символов начиная с позиции в которую ранее был установлен курсор
      lcd.write(j>i? 0:1);        //  Выводим на дисплей символ по его коду, либо 1 (символ из 1 ячейки ОЗУ дисплея), либо 32 (символ пробела)   
      }
   }                       
}
