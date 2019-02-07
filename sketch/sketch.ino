#include <Servo.h>

#include "command.h"
#include "command_processor.h"

// Используемый последовательный порт
#define SERIAL Serial1
// Используемый последовательный порт для отладки
#define DEBUG_SERIAL Serial
//#define USE_DEBUG_SERIAL 1

// Параметры машинок
#define SERVO_EPSILON 3
#define SERVO_SPEED 50
#define EXTEND_SPEED SERVO_SPEED

// Максимальное значение с АЦП
#define ANALOG_MAX 1023

#include "avr_compat.h"
#include "servo.h"

// Пины для ручного управления
#define MODE_SWITCH_PIN PA7
#define POT1_PIN PA4
#define POT2_PIN PA5
#define POT3_PIN PA6
#define CLAW_PIN PB0

// Список машинок
hand::servo servos[] = {
    //          пин     нач угол мин    макс
    hand::servo(PA0,    0,  270), // Поворот
    hand::servo(PA1,    0,  270, -40,   25), // 1 сустав
    hand::servo(NO_PIN, 0,  270, -25,   40), // 1 сустав
    hand::servo(PA2,    -40,270, -60,   20), // 2 сустав
    hand::servo(NO_PIN, 40, 270, -20,   60), // 2 сустав
    hand::servo(PA3,    40, 270, 30,    270/2)  // клешня
};
// Количество машинок
#define SERVO_COUNT 6
const size_t servo_count = SERVO_COUNT;

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
 * @brief Вспомогательный метод для одновременной установки положения сервомашинок с заданной скоростью
 * @param indices Массив индексов машинок
 * @param positions Массив положений машинок
 * @param count Число машинок
 * @param speed Скорость
 * @param m_reply Поток вывода
 */
template<int N>
bool multi_write_helper(int indices[], int positions[], int count, int speed, Print* m_reply)
{
    long timeout = 1000000/speed * SERVO_EPSILON;
    long last_micros = micros();

    bool completed = false;

    int directions[N];

    // Устанавливаем, в какую сторону нужно двигать машинку
    for(int i = 0; i < count && i < N; i++)
    {
        if(servos[indices[i]].readDegrees() <= positions[i])
            directions[i] = 1;
        else
            directions[i] = -1;
    }

    // Постепенно изменяем углы машинок на SERVO_EPSILON в нужную сторону
    while(!completed)
    {
        // Ждем некоторое время, чтобы двигаться с определенной скоростью
        if(micros() < last_micros + timeout) continue;
        last_micros = micros();

        completed = true;
        for(int i = 0; i < count && i < N; i++)
        {
            if(servos[indices[i]].getPin() == NO_PIN) continue;

            int actual_value = servos[indices[i]].readDegrees();

            if(actual_value == positions[i]) continue;

            if((positions[i] - actual_value)*directions[i] <= SERVO_EPSILON)
            {
                servos[indices[i]].writeDegrees(positions[i]);
            }
            else
            {
                completed = false;
                servos[indices[i]].writeDegrees(actual_value + SERVO_EPSILON * directions[i]);
            }
        }
    }
    if(m_reply) m_reply->println("Success");
    return true;
}

/**
 * @brief Обработчик команды мультизаписи
 *
 * При обращении к несуществующей машинки прерывает выполнение команды и печатает "Invalid servo number N" в поток вывода.
 * При успешном выполнении печатает "Success"
 */
template<int N>
COMMAND_HANDLER(multi_write_handler)
{
    int speed = m_input.parseInt();
    int servos_c = m_input.parseInt();
    int indices[N] = {0};
    int positions[N] = {0};

    for(int i = 0; i < servos_c && i < N; i++)
    {
        indices[i] = m_input.parseInt();
        if(indices[i] >= servo_count)
        {
            m_reply.print("Invalid servo index ");
            m_reply.println(indices[i]);
            return false;
        }
        positions[i] = servos[indices[i]].clamp(m_input.parseInt());
    }

    return multi_write_helper<N>(indices, positions, servos_c, speed, &m_reply);
}

// Высокоуровневые команды
/**
 * @brief Обработчик команды вращения
 */
COMMAND_HANDLER(rotate_handler)
{
    int angle = m_input.parseInt();
    bool relative = m_input.parseInt() != 0;

    if(relative)
        servos[0].writeDegrees(servos[0].readDegrees() + angle);
    else
        servos[0].writeDegrees(angle);

    m_reply.println("Success");
    return true;
}

/**
 * @brief Обработчик команды вытягивания руки
 */
COMMAND_HANDLER(extend_handler)
{
    int delta = m_input.parseInt();

    int joint_1 = servos[1].readDegrees() + delta,
        joint_2 = servos[3].readDegrees() - delta;

    int indices[] = {1, 2, 3, 4};
    int positions[] = {
        servos[1].clamp(joint_1),
        servos[2].clamp(-joint_1),
        servos[3].clamp(joint_2),
        servos[4].clamp(-joint_2)
    };

    return multi_write_helper<4>(indices, positions, 4, EXTEND_SPEED, &m_reply);
}

/**
 * @brief Обработчик команды сжатия клешни
 */
COMMAND_HANDLER(capture_handler)
{
    int delta = m_input.parseInt();

    int joint_1 = servos[5].readDegrees() + delta;

    servos[5].writeDegrees(joint_1);

    m_reply.println("Success");
    return true;
}

/**
 * @brief Обработчик команды управления суставом
 */
COMMAND_HANDLER(joint_handler)
{
    int index = m_input.parseInt();
    int position = m_input.parseInt();

    int indices[2];
    int positions[2];
    int count;

    switch(index)
    {
        case 0:
            indices[0] = 0;
            positions[0] = position;
            count = 1;
            break;
        case 1:
            indices[0] = 1;
            indices[1] = 2;
            positions[0] = position;
            positions[1] = -position;
            count = 2;
            break;
        case 2:
            indices[0] = 3;
            indices[1] = 4;
            positions[0] = position;
            positions[1] = -position;
            count = 2;
            break;
        case 3:
            indices[0] = 5;
            positions[0] = position;
            count = 1;
            break;
        default:
            m_reply.println("Invalid joint number");
            return false;
            break;
    }

    return multi_write_helper<2>(indices, positions, count, SERVO_SPEED, &m_reply);
}

// Список обработчиков команд
hand::command_handler handlers[] = {
    NULL, //0
    &read_handler,//1
    &write_handler,//2
    &ping_handler,//3
    &multi_write_handler<8>,//4
    &rotate_handler,//5
    &extend_handler,//6
    &capture_handler,//7
    &multi_write_handler<2>,
    &multi_write_handler<4>,
    &joint_handler
};

// Количество обработчиков
const size_t handlers_count = 11;

void setup()
{
    analogReadResolution(10);

    #ifdef INITIALIZE_SERVOS_IN_SETUP
        for(int i = 0; i < servo_count; i++) servos[i].init();
    #endif

    pinMode(POT1_PIN, INPUT_ANALOG);
    pinMode(POT2_PIN, INPUT_ANALOG);
    pinMode(POT3_PIN, INPUT_ANALOG);
    pinMode(CLAW_PIN, INPUT);
    pinMode(MODE_SWITCH_PIN, INPUT);

    #if defined(ARDUINO_ARCH_STM32) && defined(USE_DEBUG_SERIAL)
        DEBUG_SERIAL.begin(9600);
    #endif

    SERIAL.setTimeout(50);
    SERIAL.begin(9600);
    SERIAL.println("Initialized");
    SERIAL.flush();
}

/**
 * Обработчик поступающих команд
 */
hand::command_processor processor(handlers, handlers_count, SERIAL, SERIAL);

int analogToServo(int pin, int index)
{
    int raw = analogRead(pin);
    int value = map(raw, 0, ANALOG_MAX, servos[index].getMin(), servos[index].getMax());
    return servos[index].clamp(value);
}

void loop()
{
    static long last_millis = millis();
    if(digitalRead(MODE_SWITCH_PIN) == HIGH)
    {
        int positions[SERVO_COUNT];
        positions[0] = analogToServo(POT1_PIN, 0);
        positions[1] = analogToServo(POT2_PIN, 1);
        positions[2] = -analogToServo(POT2_PIN, 2);
        positions[3] = analogToServo(POT3_PIN, 3);
        positions[4] = -analogToServo(POT3_PIN, 4);
        if(digitalRead(CLAW_PIN) == HIGH)
            positions[5] = servos[5].getMax();
        else
            positions[5] = servos[5].getMin();

        for(int i = 0; i < SERVO_COUNT; i++)
        {
            servos[i].writeDegrees(positions[i]);
        }

        #if defined(ARDUINO_ARCH_STM32) && defined(USE_DEBUG_SERIAL)
            if(millis() - last_millis > 500)
            {
                last_millis = millis();
                DEBUG_SERIAL.print(analogRead(POT1_PIN));
                DEBUG_SERIAL.print(" ");
                DEBUG_SERIAL.print(analogRead(POT2_PIN));
                DEBUG_SERIAL.print(" ");
                DEBUG_SERIAL.print(analogRead(POT3_PIN));
                DEBUG_SERIAL.println();
            }
        #endif
    }
    else
    {
        processor.try_process();
    }
}
