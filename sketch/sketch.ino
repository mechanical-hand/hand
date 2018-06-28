#include <Servo.h>
#include "servo.h"
#include "command.h"
#include "command_processor.h"

// Используемый последовательный порт
#define SERIAL Serial

Servo servo0, servo1;

/**
 * Массив сервомашинок
*/
hand::servo servos[] = {
    hand::servo(servo0, 3, 0),
    hand::servo(servo1, 5, 0)
};

/**
 * Количество сервомашинок
 */
const size_t servo_count = 2;

// Макрос для определения обработчиков команд
#define COMMAND_HANDLER(name) bool name (Stream& m_input, Print& m_reply)

/**
 * @brief Обработчик команды чтения
 * 
 * Документацию к командам см. в command.h.
 * При обращении к несуществующей сервомашинке печатает в поток выходных данных строку "Invalid servo number N", где N - номер сервомашинки.
 * Иначе печатает "Success: N", где N - положение сервомашинки
 */
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


/**
 * @brief Обработчик команды записи
 * 
 * Документацию к командам см. в command.h.
 * При обращении к несуществующей сервомашинке печатает в поток выходных данных строку "Invalid servo number N", где N - номер сервомашинки.
 * Иначе печатает "Success"
 */
COMMAND_HANDLER(write_handler)
{
    int servo_number = m_input.parseInt();
    int position = m_input.parseInt();

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

/**
 * @brief Обработчик пинга
 *
 * Печатает "Pong" в поток вывода
 */
COMMAND_HANDLER(ping_handler)
{
    m_reply.println("Pong");
    return true;
}

/**
 * @brief Обработчик команды мультизаписи
 *
 * При обращении к несуществующей машинки прерывает выполнение команды и печатает "Invalid servo number N" в поток вывода.
 * При успешном выполнении печатает "Success"
 */
COMMAND_HANDLER(multi_write_handler)
{
    int servos_c = m_input.parseInt();
    for(int i = 0; i < servos_c; ++i)
    {
        int servo_number = m_input.parseInt();
        int position = m_input.parseInt();

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

/**
 * Массив обработчиков команд
 */
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

/**
 * Обработчик поступающих команд
 */
hand::command_processor processor(handlers, handlers_count, SERIAL, SERIAL);

void loop()
{
    processor.try_process();
}
