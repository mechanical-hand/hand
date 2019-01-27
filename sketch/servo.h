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
            m_angle(angle)
        {
            m_servo.attach(pin);
            m_servo.write(initial);
        }

        /**
         * @brief Установка положения сервомашинки
         * @param value Желаемое значение
         */
        void write(int value)
        {
            m_servo.write(value);
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

            if(degrees > half_angle) return writeDegrees(half_angle);

            if(degrees < - half_angle) return writeDegrees(-half_angle);

            int pwm = map(degrees, -half_angle, half_angle, 0, 255);
            write(pwm);
            return true;
        }

        int readDegrees()
        {
            int half_angle = getHalfAngle();
            int pwm = read();
            return map(pwm, 0, 255, -half_angle, half_angle);
        }

        int getAngle()
        {
            return m_angle;
        }

        int getHalfAngle()
        {
            return m_angle/2;
        }
    private:
        Servo m_servo;
        int m_angle;
    };
}
