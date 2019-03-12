#include "servo.h"
#include <Arduino.h>

using namespace hand;

servo::servo(int pin, int initial, int angle) :
    m_angle(angle),
    m_pin(pin),
    m_initial(initial),
    m_min(-angle/2),
    m_max(angle/2)
{
    #ifndef INITIALIZE_SERVOS_IN_SETUP
        init();
    #endif
}

servo::servo(int pin, int initial, int angle, int min, int max) :
    m_angle(angle),
    m_initial(initial),
    m_pin(pin),
    m_min(min),
    m_max(max)
{
    #ifndef INITIALIZE_SERVOS_IN_SETUP
        init();
    #endif
}

void servo::write(int value)
{
    if(m_pin == NO_PIN) return;
    m_servo.write(value);
}

void servo::init()
{
    if(m_pin == NO_PIN) return;
    m_servo.attach(m_pin);
    writeDegrees(m_initial);
}

int servo::read()
{
    if(m_pin == NO_PIN) return 0;
    return m_servo.read();
}

bool servo::writeDegrees(int degrees)
{
    int half_angle = getHalfAngle();
    int pwm = map(clamp(degrees), -half_angle, half_angle, 0, 180);
    write(pwm);
    return true;
}

int servo::readDegrees()
{
    int half_angle = getHalfAngle();
    int pwm = read();
    return map(pwm, 0, 180, -half_angle, half_angle);
}

int servo::clamp(int value, int min, int max)
{
    if(value > max) return max;
    if(value < min) return min;
    return value;
}

int servo::clamp(int value, bool use_restrictions)
{
    int half = getHalfAngle();
    int min = (use_restrictions ? -half : m_min);
    int max = (use_restrictions ? half : m_max);
    return clamp(value, min, max);
}

int servo::clamp(int value)
{
    return clamp(value, true);
}
