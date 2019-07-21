#include <Servo.h>
#include "config.h"

#include <command_processor.h>

const size_t servo_count = SERVO_COUNT;

#ifdef ENABLE_PS_GAMEPAD
    using hand::ps2_button;
    using hand::ps2_analog;
#endif

// Макрос для определения обработчиков команд
#define COMMAND_HANDLER(name) bool name (Stream& m_input, Print& m_reply)

#ifdef ENABLE_PS_GAMEPAD
    bool manual_mode()
    {
        #ifdef ENABLE_MANUAL_CONTROL_SWITCH
            static volatile hand::register_t *mcs_reg = portInputRegister(digitalPinToPort(MANUAL_CONTROL_SWITCH_PIN));
            static hand::register_t mcs_mask = digitalPinToBitMask(MANUAL_CONTROL_SWITCH_PIN);
            return *mcs_reg & mcs_mask;
        #else
            return true;
        #endif
    }
#endif

/**
 * @brief Обработчик команды чтения
 *
 * Документацию к командам см. в command.h.
 * При обращении к несуществующей сервомашинке печатает в поток выходных данных строку "Invalid servo number N", где N - номер сервомашинки.
 * Иначе печатает "Success: N", где N - положение сервомашинки
 */
COMMAND_HANDLER(read_handler)
{
    unsigned int servo_number = m_input.parseInt();
    if(servo_number >= servo_count)
    {
        m_reply.print("Invalid servo number ");
        m_reply.println(servo_number);
        return false;
    }

    m_reply.print("Success: ");
    int arg = servos[servo_number]->read();
    m_reply.println(arg);
    return true;
}


/**
 * @brief Обработчик команды записи
 *
 * При обращении к несуществующей сервомашинке печатает в поток выходных данных строку "Invalid servo number N", где N - номер сервомашинки.
 * Иначе печатает "Success"
 */
COMMAND_HANDLER(write_handler)
{
    unsigned int servo_number = m_input.parseInt();
    int position = m_input.parseInt();

    if(servo_number >= servo_count)
    {
        m_reply.print("Invalid servo number ");
        m_reply.println(servo_number);
        return false;
    }

    servos[servo_number]->write(position);
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
    #ifdef ENABLE_DEBUG
        logger.println("Test");
    #endif
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
bool multi_write_helper(unsigned int indices[], int positions[], int count, int speed, Print* m_reply)
{
    unsigned long timeout = 1000000/speed * SERVO_EPSILON;
    unsigned long last_micros = micros();

    bool completed = false;

    int directions[N];

    // Устанавливаем, в какую сторону нужно двигать машинку
    for(int i = 0; i < count && i < N; i++)
    {
        if(servos[indices[i]]->readDegrees() <= positions[i])
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
            if(servos[indices[i]]->getPin() == NO_PIN) continue;

            int actual_value = servos[indices[i]]->readDegrees();

            if(actual_value == positions[i]) continue;

            if((positions[i] - actual_value)*directions[i] <= SERVO_EPSILON)
            {
                servos[indices[i]]->writeDegrees(positions[i]);
            }
            else
            {
                completed = false;
                servos[indices[i]]->writeDegrees(actual_value + SERVO_EPSILON * directions[i]);
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
    unsigned int indices[N] = {0};
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
        positions[i] = servos[indices[i]]->clamp(m_input.parseInt());
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
        servos[0]->writeDegrees(servos[0]->readDegrees() + angle);
    else
        servos[0]->writeDegrees(angle);

    m_reply.println("Success");
    return true;
}

/**
 * @brief Обработчик команды вытягивания руки
 */
COMMAND_HANDLER(extend_handler)
{
    int delta = m_input.parseInt();

    int joint_1 = servos[1]->readDegrees() + delta,
        joint_2 = servos[3]->readDegrees() - delta;

    unsigned int indices[] = {1, 2, 3, 4};
    int positions[] = {
        servos[1]->clamp(joint_1),
        servos[2]->clamp(-joint_1),
        servos[3]->clamp(joint_2),
        servos[4]->clamp(-joint_2)
    };

    return multi_write_helper<4>(indices, positions, 4, EXTEND_SPEED, &m_reply);
}

/**
 * @brief Обработчик команды сжатия клешни
 */
COMMAND_HANDLER(capture_handler)
{
    int delta = m_input.parseInt();

    int joint_1 = servos[5]->readDegrees() + delta;

    servos[5]->writeDegrees(joint_1);

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

    unsigned int indices[2];
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

COMMAND_HANDLER(report_handler)
{
    m_reply.print("Success: ");
    #ifdef ENABLE_PS_GAMEPAD
        m_reply.print("gamepad_support:enabled;gamepad_state:");
        m_reply.print(gamepad.lastConfigResult());
        m_reply.print(";gamepad_lx:");
        m_reply.print((int)gamepad.analog(ps2_analog::PSA_LX));
        m_reply.print(";gamepad_ly:");
        m_reply.print((int)gamepad.analog(ps2_analog::PSA_LY));
        m_reply.print(";gamepad_rx:");
        m_reply.print((int)gamepad.analog(ps2_analog::PSA_RX));
        m_reply.print(";gamepad_ry:");
        m_reply.print((int)gamepad.analog(ps2_analog::PSA_RY));
        m_reply.print(";gamepad_buttons:");
        m_reply.print(gamepad.buttonsState(), BIN);
        m_reply.print(";gamepad_buffer:");
        gamepad.dumpBuffer(m_reply);
        m_reply.print(";");
    #else
        m_reply.print("gamepad_support: disabled;");
    #endif

    for(int i = 0; i < servo_count; i++)
    {
        m_reply.print("servo");
        m_reply.print(i);
        m_reply.print(":");
        m_reply.print(servos[i]->readDegrees());
        m_reply.print(";");
    }

    m_reply.print("s0tgt:");
    m_reply.print(servo_rotation.getTarget());

    m_reply.println();
    return true;
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
    &joint_handler,
    &report_handler
};

// Количество обработчиков
const size_t handlers_count = 12;

void setup()
{
    #ifdef ENABLE_POWER_CONTROL

        for(int pin : {POWER_CONTROL_PINS})
        {
            digitalWrite(pin, POWER_CONTROL_DISABLED_LEVEL);
        }
    #endif

    hand::setStepPin(HAND_STEP_PIN);

    #ifdef HARDWARE_SS_PIN
        pinMode(HARDWARE_SS_PIN, OUTPUT);
    #endif

    //analogReadResolution(10);
    #ifdef ENABLE_DEBUG
        hand::logger_instance = &logger;
    #endif

    #ifdef INITIALIZE_SERVOS_IN_SETUP
        for(unsigned int i = 0; i < servo_count; i++) servos[i]->init();
    #endif

    #ifdef ENABLE_PS_GAMEPAD
        pinMode(GAMEPAD_SS_PIN, OUTPUT);
        //SPI.begin();
    #endif

    HAND_SERIAL.setTimeout(50);
    HAND_SERIAL.begin(9600);

    #ifdef ENABLE_POWER_CONTROL
        HAND_SERIAL.println("Log: Initializing power control...");

        for(int pin : {POWER_CONTROL_PINS})
        {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(POWER_CONTROL_TIMEOUT/2);
            digitalWrite(pin, POWER_CONTROL_ENABLED_LEVEL);
            digitalWrite(LED_BUILTIN, LOW);
            delay(POWER_CONTROL_TIMEOUT/2);
        }
    #endif
    HAND_SERIAL.flush();

    #ifdef ENABLE_PS_GAMEPAD
        delay(1000);
        hand::ps2_config_result cfg = gamepad.configure();
        #ifdef ENABLE_DEBUG
            logger.begin();
            logger.print("Gamepad config result : ");
            logger.print((int)cfg);
            logger.end();
        #endif
    #endif
    HAND_SERIAL.println("Initialized");
}

/**
 * Обработчик поступающих команд
 */
hand::command_processor processor(handlers, handlers_count, HAND_SERIAL, HAND_SERIAL);

void loop()
{
    hand::processSteppers();
    gamepad.update(0,0);
    #ifdef ENABLE_PS_GAMEPAD

        static unsigned long last_millis = millis();
        static unsigned long start_times[3];
        if(manual_mode())
        {
            unsigned long delta_time = millis() - last_millis;
            last_millis = millis();

            int rotation = 0;
            if(gamepad.button(ps2_button::PSB_L1))
                rotation += SERVO_SPEED / 3;
            if(gamepad.button(ps2_button::PSB_R1))
                rotation -= SERVO_SPEED / 3;
            if(gamepad.button(ps2_button::PSB_L2))
                rotation += 2 * SERVO_SPEED / 3;
            if(gamepad.button(ps2_button::PSB_R2))
                rotation -= 2 * SERVO_SPEED / 3;

            if(gamepad.pressed(
                ps2_button::PSB_L1 |
                ps2_button::PSB_R1 |
                ps2_button::PSB_L2 |
                ps2_button::PSB_R2 ))
                start_times[0] = millis();
            #ifdef ENABLE_ANALOG_ROTATION
                if(
                    !gamepad.button(
                        ps2_button::PSB_L1 |
                        ps2_button::PSB_R1 |
                        ps2_button::PSB_L2 |
                        ps2_button::PSB_R2 )
                )
                {
                    rotation = map(gamepad.analog(ps2_analog::PSA_LX), 0, 255, -SERVO_SPEED, SERVO_SPEED);
                    if(abs(rotation) < ROTATION_THRESHOLD)
                        rotation = 0;
                }
            #endif

            /*int max_acceleration = MAX_ACCELERATION * (millis() - start_times[0]) / 1000;
            if(rotation < 0)
                rotation = max(rotation, -max_acceleration) * delta_time / 1000;
            else
                rotation = min(rotation, max_acceleration) * delta_time / 1000;

            if(rotation)
                servos[0]->writeDegrees(servos[0]->readDegrees() + rotation);*/

            float last_rd = servos[0]->readDegrees();
            float new_rd = last_rd;
            if(gamepad.button(ps2_button::PSB_R1))
                new_rd += SERVO_SPEED * delta_time / 5;
            if(gamepad.button(ps2_button::PSB_R2))
                new_rd += SERVO_SPEED + delta_time / 3;
            if(gamepad.button(ps2_button::PSB_L1))
                new_rd -= SERVO_SPEED * delta_time / 5;
            if(gamepad.button(ps2_button::PSB_L2))
                new_rd -= SERVO_SPEED * delta_time / 3;

            servos[0]->writeDegrees(new_rd);

            int joint_1 = map(gamepad.analog(ps2_analog::PSA_LY), 0, 255, servos[1]->getMin(), servos[1]->getMax());
            servos[1]->writeDegrees(joint_1);
            //servos[2]->writeDegrees(servos[2]->readDegrees() - joint_1 * delta_time / 100);

            int joint_2 = map(gamepad.analog(ps2_analog::PSA_RY), 0, 255, servos[3]->getMin(), servos[3]->getMax());
            servos[3]->writeDegrees(joint_2);
            //servos[4]->writeDegrees(servos[4]->readDegrees() - joint_2 * delta_time / 100);

            if(gamepad.button(ps2_button::PSB_CIRCLE))
                servos[5]->writeDegrees(servos[5]->getMin());
            if(gamepad.button(ps2_button::PSB_CROSS))
                servos[5]->writeDegrees(servos[5]->getMax());
        }
    #endif
    HAND_SERIAL.flush();
    processor.try_process();
}
