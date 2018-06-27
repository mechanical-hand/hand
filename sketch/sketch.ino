#include <Servo.h>
#include "servo.h"
#include "command.h"
#include "command_processor.h"

#define SERIAL Serial

Servo servo0, servo1;

hand::servo servos[] = {
    hand::servo(servo0, 3, 0),
    hand::servo(servo1, 5, 0)
};

const size_t servo_count = 2;

#define COMMAND_HANDLER(name) bool name (Stream& m_input, Print& m_reply)

COMMAND_HANDLER(read_handler)
{
    int servo_number = m_input.parseInt();
    if(servo_number >= servo_count)
    {
        m_reply.print("Invalid servo number ");
        m_reply.println(servo_number);
        return false;
    }

    m_reply.print("Success: ");
    int arg = servos[servo_number].read();
    m_reply.println(arg);
    return true;
}

COMMAND_HANDLER(write_handler)
{
    int servo_number = m_input.parseInt();;
    int position = m_input.parseInt();;

    if(servo_number >= servo_count)
    {
        m_reply.print("Invalid servo number ");
        m_reply.println(servo_number);
        return false;
    }

    servos[servo_number].write(position);
    m_reply.println("Success");

    return true;
}

COMMAND_HANDLER(ping_handler)
{
    m_reply.println("Pong");
    return true;
}

COMMAND_HANDLER(multi_write_handler)
{
    int servos_c = m_input.parseInt();
    for(int i = 0; i < servos_c; ++i)
    {
        int servo_number = m_input.parseInt();;
        int position = m_input.parseInt();;

        if(servo_number >= servo_count)
        {
            m_reply.print("Invalid servo number ");
            m_reply.println(servo_number);
            return false;
        }

        servos[servo_number].write(position);
    }
    m_reply.println("Success");

    return true;
}


hand::command_handler handlers[] = {
    NULL,
    &read_handler,
    &write_handler,
    &ping_handler,
    &multi_write_handler
};

const size_t handlers_count = 5;


void setup()
{
    SERIAL.begin(9600);
    SERIAL.println("Initialized");
}

hand::command_processor processor(handlers, handlers_count, SERIAL, SERIAL);

void loop()
{
    processor.try_process();
}
