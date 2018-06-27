#pragma once


namespace hand
{
    class servo
    {
    public:
        servo(Servo s, int pin, int initial) :
            m_servo(s)
        {
            m_servo.attach(pin);
            m_servo.write(initial);
        }

        void write(int value)
        {
            m_servo.write(value);
        }

        int read()
        {
            return m_servo.read();
        }
    private:
        Servo m_servo;
    };
}
