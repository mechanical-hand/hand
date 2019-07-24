#include "stepper.h"
#include <math.h>
#include <log.h>
#include <Arduino.h>


static int stepPin;

#ifndef HAND_STEP_TIMEOUT
    #define HAND_STEP_TIMEOUT 10
#endif

void hand::stepper_servo::setEnabled(bool b)
{
    m_enabled = b;
    #ifndef TREAT_ENABLE_AS_STEP_PIN
        #ifdef INVERTED_STEPPER_ENABLE
            digitalWrite(m_enable_pin, b ? LOW : HIGH);
        #else
            digitalWrite(m_enable_pin, b ? HIGH : LOW);
        #endif
    #endif
}

void hand::stepper_servo::setDirection(bool b)
{
    digitalWrite(m_direction_pin, b ? HIGH : LOW);
    m_direction = b;
}

hand::stepper_servo* hand::stepper_servo::process()
{
    if(m_enabled)
    {
        setDirection(m_target_position > m_position);
        if(fabs(m_target_position - m_position) < m_step + 0.0001)
            setEnabled(false);

        m_position += m_step * (m_direction ? 1 : -1);
        #ifdef TREAT_ENABLE_AS_STEP_PIN
            digitalWrite(m_enable_pin, HIGH);
            delay(HAND_STEP_TIMEOUT);
            digitalWrite(m_enable_pin, LOW);
            delay(HAND_STEP_TIMEOUT);
        #endif
    }
    return m_next_stepper;
}

void hand::processSteppers()
{
    hand::stepper_servo* s = hand::first_stepper;
    if(!s) return;
    while(s = s->process());
    #ifndef TREAT_ENABLE_AS_STEP_PIN
        #error Please use TREAT_ENABLE_AS_STEP_PIN
        digitalWrite(stepPin, HIGH);
        delay(HAND_STEP_TIMEOUT);
        digitalWrite(stepPin, LOW);
    #endif
}

void hand::setStepPin(int pin)
{
    stepPin = pin;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

int hand::getStepPin()
{
    return stepPin;
}

hand::stepper_servo::stepper_servo(int e, int d, float step) :
    m_enable_pin(e),
    m_direction_pin(d),
    m_step(step),
    m_position(0.0),
    m_target_position(0.0),
    m_enabled(false)
{
    #ifndef INITIALIZE_SERVOS_IN_SETUP
        init();
    #endif
}

void hand::stepper_servo::init()
{
    m_next_stepper = hand::first_stepper;
    hand::first_stepper = this;
    pinMode(m_enable_pin, OUTPUT);
    pinMode(m_direction_pin, OUTPUT);
}

void hand::stepper_servo::write(int degrees)
{
    m_target_position = degrees;
    setEnabled(true);
}

bool hand::stepper_servo::writeDegrees(int d)
{
    write(d);
    return true;
}

int hand::stepper_servo::read()
{
    return round(m_position);
}

int hand::stepper_servo::readDegrees()
{
    return read();
}



namespace hand
{
    stepper_servo* first_stepper = 0;
}
