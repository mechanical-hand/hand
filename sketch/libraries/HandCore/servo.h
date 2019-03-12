#pragma once
#define NO_PIN 0
#include <Servo.h>

namespace hand
{
    /**
     * @brief Класс-обертка над классом управления сервомашинками
     */
    class servo
    {
    public:
        servo(int pin, int initial, int angle);

        servo(int pin, int initial, int angle, int min, int max);

        /**
         * @brief Установка положения сервомашинки
         * @param value Желаемое значение
         */
        void write(int value);

        void init();

        /**
         * @brief Чтение текущего положения сервомашинки
         * @returns Положение сервомашинки
         */
        int read();

        bool writeDegrees(int degrees);

        int readDegrees();



        int clamp(int value, int min, int max);

        int clamp(int value, bool use_restrictions);

        int clamp(int value);

        int getMin(){ return m_min; }
        int getMax(){ return m_max; }
        int getPin(){ return m_pin; }
        int getAngle(){ return m_angle; }

        int getHalfAngle(){ return m_angle/2; }
    private:
        Servo m_servo;
        int m_angle, m_initial, m_pin, m_min, m_max;
    };
}
