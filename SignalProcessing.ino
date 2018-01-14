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
  sygnal = sygnal / NUM_OF_SAMPLES - 512;
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
