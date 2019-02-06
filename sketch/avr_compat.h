#pragma once
/*
Код для тестов на Arduino Uno
*/

#ifndef PA0
  // Меняем пины PA0-PA3 на аппаратные ШИМ выходы Arduino
  #define PA0 3
  #define PA1 5
  #define PA2 6
  #define PA3 9

  // Меняем пины на аналоговые
  #define PA4 A0
  #define PA5 A1
  #define PB0 A2
  #define PB1 A3

  // Меняем пины на аппаратные ШИМ выходы
  #define PA6 10
  #define PA7 11

  // Меняем пин на любой свободный цифровой (для переключателя)
  #define PB10 12
  #define PB11 13

  // Меняем Serial1 на Serial
  #undef SERIAL
  #define SERIAL Serial

  // Меняем режим для аналоговых пинов
  #define INPUT_ANALOG INPUT

  #define ANALOG_MAX 1023
  #define INITIALIZE_SERVOS_IN_SETUP
#else
  #define ANALOG_MAX 4095
#endif
