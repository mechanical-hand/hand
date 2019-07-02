#pragma once
#include <servo.h>

namespace hand
{

    extern void processSteppers();

    class stepper_servo : public servo_base
    {
        friend void processSteppers();
    public:
        stepper_servo(int m_enable_pin, int m_direction_pin, float m_step);
        void write(int);
        bool writeDegrees(int);
        int read();
        int readDegrees();
        void init();

    private:
        int m_enable_pin, m_direction_pin, m_step_pin;
        bool m_direction, m_enabled;
        float m_position, m_target_position, m_step;

        stepper_servo *m_next_stepper;

        stepper_servo *process();
        void setEnabled(bool);
        void setDirection(bool);
    };

    extern stepper_servo *first_stepper;

    extern void setStepPin(int pin);
    extern int getStepPin();
}
