#pragma once

#define SERIAL Serial1

#ifndef HAND_PLATFORMIO_CONFIG
    #define ENABLE_DEBUG
#endif

#ifdef ENABLE_DEBUG
    #include <log.h>
    hand::logger<HardwareSerial> logger = hand::make_logger(SERIAL);
#endif

#ifndef SERVO_EPSILON
    #define SERVO_EPSILON 3
#endif

#ifndef SERVO_SPEED
    #define SERVO_SPEED 50
#endif

#ifndef EXTEND_SPEED
    #define EXTEND_SPEED SERVO_SPEED
#endif

#include "avr_compat.h"
#include <servo.h>

// Список машинок
hand::servo servos[] = {
    //          пин     нач угол мин    макс
    hand::servo(PA0,    0,  270), // Поворот
    hand::servo(PA1,    0,  270, -40,   25), // 1 сустав
    hand::servo(NO_PIN, 0,  270, -25,   40), // 1 сустав
    hand::servo(PA2,    -40,270, -60,   20), // 2 сустав
    hand::servo(NO_PIN, 40, 270, -20,   60), // 2 сустав
    hand::servo(PA3,    40, 270, 30,    270/2)  // клешня
};
// Количество машинок
#define SERVO_COUNT 6
