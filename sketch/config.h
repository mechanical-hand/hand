#pragma once

#ifndef HAND_SERIAL
    #define HAND_SERIAL Serial1
#endif

#ifndef HAND_PLATFORMIO_CONFIG
    #define ENABLE_DEBUG
#endif

#ifdef ENABLE_DEBUG
    #include <log.h>
    hand::logger<HardwareSerial> logger = hand::make_logger(HAND_SERIAL);
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

#ifdef ENABLE_PS_GAMEPAD
    #if defined(ENABLE_SOFTWARE_SPI)
        #include <software_spi_driver.h>
    #elif !defined(ENABLE_PS2X_SUPPORT)
        #include <hardware_spi_driver.h>
    #endif

    #include <ps_gamepad.h>

    #ifndef GAMEPAD_SS_PIN
        #define GAMEPAD_SS_PIN PB0
    #endif

    #if defined(ENABLE_SOFTWARE_SPI) || defined(ENABLE_PS2X_SUPPORT)
        #ifndef GAMEPAD_MISO_PIN
            #define GAMEPAD_MISO_PIN PA6
        #endif

        #ifndef GAMEPAD_MOSI_PIN
            #define GAMEPAD_MOSI_PIN PA7
        #endif

        #ifndef GAMEPAD_SCK_PIN
            #define GAMEPAD_SCK_PIN PA5
        #endif
    #endif

    #ifdef ENABLE_MANUAL_CONTROL_SWITCH
        #ifndef MANUAL_CONTROL_SWITCH_PIN
            #define MANUAL_CONTROL_SWITCH_PIN PA5
        #endif
    #endif

    #ifndef MAX_ACCELERATION
        #define MAX_ACCELERATION 25
    #endif

    #ifndef HARDWARE_SS_PIN
        #define HARDWARE_SS_PIN PB12
    #endif

    #ifdef ENABLE_PS2X_SUPPORT
        hand::ps2x_gamepad gamepad(
            GAMEPAD_MISO_PIN,
            GAMEPAD_MOSI_PIN,
            GAMEPAD_SCK_PIN,
            GAMEPAD_SS_PIN
        );
    #else
        #ifdef ENABLE_SOFTWARE_SPI
            hand::software_spi_driver driver(
                GAMEPAD_MOSI_PIN,
                GAMEPAD_MISO_PIN,
                GAMEPAD_SCK_PIN,
                GAMEPAD_SS_PIN
            );
        #else
            hand::hardware_spi_driver driver(
                hand::gamepad_settings,
                GAMEPAD_SS_PIN
            );
        #endif

        hand::ps2_gamepad gamepad(driver, false, false);
    #endif

    #ifndef ROTATION_THRESHOLD
        #define ROTATION_THRESHOLD 20
    #endif
#endif

#ifndef HAND_STEP_PIN
    #define HAND_STEP_PIN PB11
#endif

#ifdef ENABLE_POWER_CONTROL
    #ifndef POWER_CONTROL_PINS
        #define POWER_CONTROL_PINS PB12,PB13,PB14
    #endif

    #ifndef POWER_CONTROL_TIMEOUT
        #define POWER_CONTROL_TIMEOUT 1000
    #endif

    #if defined(POWER_CONTROL_ENABLED_LEVEL) && POWER_CONTROL_ENABLED_LEVEL == LOW
        #define POWER_CONTROL_DISABLED_LEVEL HIGH
    #else
        #if !defined(POWER_CONTROL_ENABLED_LEVEL)
            #define POWER_CONTROL_ENABLED_LEVEL HIGH
        #endif

        #define POWER_CONTROL_DISABLED_LEVEL LOW
    #endif

#endif

#include <servo.h>
#include <stepper.h>

hand::dummy_servo servo_dummy;
hand::stepper_servo servo_rotation(PB1, PB10, 10);
hand::servo servo_first(PA1,    0,  270, -40,   25);
hand::servo servo_second(PA2,    -40,270, -60,   20);
hand::servo servo_claw(PA3,    40, 270, 30,    270/2);

// Список машинок
hand::servo_base* servos[] = {
    //          пин     нач угол мин    макс
    /*hand::servo(PA0,    0,  270), // Поворот
    hand::servo(PA1,    0,  270, -40,   25), // 1 сустав
    hand::servo(NO_PIN, 0,  270, -25,   40), // 1 сустав
    hand::servo(PA2,    -40,270, -60,   20), // 2 сустав
    hand::servo(NO_PIN, 40, 270, -20,   60), // 2 сустав
    hand::servo(PA3,    40, 270, 30,    270/2)  // клешня*/
    &servo_rotation,
    &servo_first,
    &servo_dummy,
    &servo_second,
    &servo_dummy,
    &servo_claw
};
// Количество машинок
#define SERVO_COUNT 6
