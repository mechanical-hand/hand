#pragma once


namespace hand
{
    /**
     * @brief Класс-обертка над классом управления сервомашинками
     */
    class servo
    {
    public:
        servo(int pin, int initial, int angle) :
            m_angle(angle),
            m_pin(pin),
            m_initial(initial),
            m_min(0),
            m_max(angle)
        {}

        servo(int pin, int initial, int angle, int min, int max) :
            m_angle(angle),
            m_initial(initial),
            m_pin(pin),
            m_min(min),
            m_max(max)
        {}

        /**
         * @brief Установка положения сервомашинки
         * @param value Желаемое значение
         */
        void write(int value)
        {
            m_servo.write(value);
        }

        void init()
        {
            m_servo.attach(m_pin);
            m_servo.write(m_initial);
        }

        /**
         * @brief Чтение текущего положения сервомашинки
         * @returns Положение сервомашинки
         */
        int read()
        {
            return m_servo.read();
        }

        bool writeDegrees(int degrees)
        {
            int half_angle = getHalfAngle();
            int pwm = map(clamp(degrees), -half_angle, half_angle, 0, 180);
            write(pwm);
            return true;
        }

        int readDegrees()
        {
            int half_angle = getHalfAngle();
            int pwm = read();
            return map(pwm, 0, 180, -half_angle, half_angle);
        }

        int getAngle()
        {
            return m_angle;
        }

        int getHalfAngle()
        {
            return m_angle/2;
        }

        int clamp(int value)
        {
            int half_angle = getHalfAngle();
            if(value > half_angle) return half_angle;
            if(value < -half_angle) return -half_angle;
            return value;
        }
    private:
        Servo m_servo;
        int m_angle, m_initial, m_pin, m_min, m_max;
    };
}
