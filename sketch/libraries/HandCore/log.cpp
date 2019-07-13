#include "log.h"
#include <Arduino.h>


hand::logger<HardwareSerial> *hand::logger_instance = 0;
