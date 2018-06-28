#pragma once


namespace hand
{
    /**
     * @brief Класс-обертка над классом управления сервомашинками
     */
    class servo
    {
    public:
        /** 
         * @brief Конструктор
         * @param s Экземпляр класса управления сервомашинкой
         * @param pin Номер пина, к которому подключена сервомашинка
         * @param initial Начальное положение
         */
        servo(Servo s, int pin, int initial) :
            m_servo(s)
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
    private:
        Servo m_servo;
    };
}
