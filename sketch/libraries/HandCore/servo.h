#pragma once
#define NO_PIN 0
#include <Servo.h>

namespace hand
{

    class servo_base
    {
    public:
        virtual void write(int value) = 0;

        virtual void init() = 0;

        /**
         * @brief Чтение текущего положения сервомашинки
         * @returns Положение сервомашинки
         */
        virtual int read() = 0;

        virtual bool writeDegrees(int degrees) = 0;

        virtual int readDegrees() = 0;



        virtual int clamp(int value, int min, int max);

        virtual int clamp(int value, bool use_restrictions);

        virtual int clamp(int value);

        int getMin(){ return m_min; }
        int getMax(){ return m_max; }
        int getPin(){ return m_pin; }
        int getAngle(){ return m_angle; }

        int getHalfAngle(){ return m_angle/2; }
    protected:
        int m_min, m_max, m_angle, m_pin;
    };


    class dummy_servo : public servo_base
    {
    public:
        dummy_servo()
        {
            this->m_min = 0;
            this->m_max = 360;
            this->m_angle = 360;
            this->m_pin = NO_PIN;
        }

        virtual int read() { return 0; }
        virtual void write(int) {}
        virtual void init() {}
        virtual bool writeDegrees(int){ return true; }
        virtual int readDegrees() { return 0; }
    };

    /**
     * @brief Класс-обертка над классом управления сервомашинками
     */
    class servo : public servo_base
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
    private:
        Servo m_servo;
        int m_initial;
    };
}
