/*
 * В ЭТОМ ФАЙЛЕ НАХОДИТСЯ ВСЕ, ЧТО СВЯЗАНО С ОБРАБОТКОЙ
 * И АНАЛИЗОМ ЗВУКОВЫХ СИГНАЛОВ
 */
 
const byte SAMPLING_DELAY = 0; // микросекунд
const long NUM_OF_SAMPLES = 32;

//считывание и усреднение сигнала
long processAnalogValue(int channel){
  long sygnal = 0;

  for (int i=0; i < NUM_OF_SAMPLES; i++) {
    
    sygnal += analogRead(channel);
    delayMicroseconds(SAMPLING_DELAY);
  }
  sygnal = sygnal / NUM_OF_SAMPLES;
  //if (sygnal < 0) sygnal = -sygnal;
  //sygnal = map(sygnal,0,511,0,1023);
  return sygnal ; 
}


bool sourceIsQuiet(int ch1, int ch2){
  // Значения границ хранятся в процентах
  float coeff = 10.23;
  return (ch1 <= round(QUIET_TRESHOLD * coeff)) || (ch2 <= round(QUIET_TRESHOLD * coeff));
}


bool sourceIsLoud(int ch1, int ch2){
  // Значения границ хранятся в процентах
  float coeff = 10.23;
  return (ch1 >= round(LOUD_TRESHOLD * coeff)) && (ch2 >= round(LOUD_TRESHOLD * coeff));
}


