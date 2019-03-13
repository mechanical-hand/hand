#include "ps_gamepad.h"

using namespace hand;

ps2_gamepad::ps2_gamepad(uint8_t ss, bool rumble, bool pressures) :
    m_ss_pin(ss),
    m_rumble(rumble),
    m_pressures(pressures)
{
    m_ss_mask = digitalPinToBitMask(ss);
    m_ss_reg = portOutputRegister(digitalPinToPort(ss));
    for(int i = 0; i < 21; i++)
        m_buffer[i] = 0;
}

ps2_config_result ps2_gamepad::configure()
{
    m_last_read = millis();
    pinMode(m_ss_pin, OUTPUT);
    *m_ss_reg |= m_ss_mask;
    m_read_delay = 10; // TODO

    readGamepad(0, 0);

    switch (m_buffer[1])
    {
        case 0x41:
        case 0x73:
        case 0x79:
            break;
        default:
            return NO_CONTROLLER;
    }

    uint8_t type[sizeof(m_type_read)];

    sendCommand(m_enter_cfg, sizeof(m_enter_cfg));
    sendCommand(m_type_read, type, sizeof(m_type_read));

    m_controller_type = type[3];

    sendCommand(m_set_mode, sizeof(m_set_mode));

    if(m_rumble) sendCommand(m_enable_rumble, sizeof(m_enable_rumble));
    if(m_pressures) sendCommand(m_set_bytes_large, sizeof(m_set_bytes_large));

    sendCommand(m_exit_cfg, sizeof(m_exit_cfg));

    readGamepad(0, 0);

    return (m_buffer[1] == 0x73 && m_pressures) ? NO_CONNECTION : OK;
}

void ps2_gamepad::readGamepad(uint8_t motor1, uint8_t motor2)
{
    m_last_buttons = m_buttons;
    unsigned long last_delay = millis() - m_last_read;

    if(last_delay > 1500) reconfigure();
    if(last_delay < m_read_delay) delay(m_read_delay - last_delay);

    for(int i = 0; i < 21; i++)
    {
        m_buffer[i] = 0;
    }

    m_buffer[0] = 0x01;
    m_buffer[1] = 0x42;
    m_buffer[3] = motor1;
    m_buffer[4] = motor2;

    send(m_buffer, 9);

    if(m_buffer[1] == 0x79) // Full Data Return Mode
        send(m_buffer + 9, 12);

    m_buttons = *(uint16_t*)(m_buffer + 3);

    m_last_read = millis();
}

void ps2_gamepad::reconfigure()
{
    sendCommand(m_enter_cfg, sizeof(m_enter_cfg));
    sendCommand(m_set_mode, sizeof(m_set_mode));
    if(m_rumble)
        sendCommand(m_enable_rumble, sizeof(m_enable_rumble));
    if(m_pressures)
        sendCommand(m_set_bytes_large, sizeof(m_set_bytes_large));
    sendCommand(m_exit_cfg, sizeof(m_exit_cfg));
}

void ps2_gamepad::sendCommand(const uint8_t* buffer, uint8_t* output, uint16_t size)
{
    SPI.beginTransaction(gamepad_settings);
    *m_ss_reg &= ~m_ss_mask;

    for(uint16_t i = 0; i < size; i++)
    {
        uint8_t temp = SPI.transfer(buffer[i]);
        if(output)
            output[i] = temp;
    }

    *m_ss_reg |= m_ss_mask;
    SPI.endTransaction();
}

void ps2_gamepad::sendCommand(const uint8_t* buffer, uint16_t size)
{
    sendCommand(buffer, NULL, size);
}

void ps2_gamepad::send(uint8_t* buffer, uint16_t size)
{
    SPI.beginTransaction(gamepad_settings);
    *m_ss_reg &= ~m_ss_mask;

    SPI.transfer(buffer, size);

    *m_ss_reg |= m_ss_mask;
    SPI.endTransaction();
}
