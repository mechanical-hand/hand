#pragma once
#include <SPI.h>

namespace hand
{
    enum ps2_button
    {
        PSB_NONE        = 0x0000,
        PSB_SELECT      = 0x0001,
        PSB_L3          = 0x0002,
        PSB_R3          = 0x0004,
        PSB_START       = 0x0008,
        PSB_PAD_UP      = 0x0010,
        PSB_PAD_RIGHT   = 0x0020,
        PSB_PAD_DOWN    = 0x0040,
        PSB_PAD_LEFT    = 0x0080,
        PSB_L2          = 0x0100,
        PSB_R2          = 0x0200,
        PSB_L1          = 0x0400,
        PSB_R1          = 0x0800,
        PSB_GREEN       = 0x1000,
        PSB_RED         = 0x2000,
        PSB_BLUE        = 0x4000,
        PSB_PINK        = 0x8000,
        PSB_TRIANGLE    = 0x1000,
        PSB_CIRCLE      = 0x2000,
        PSB_CROSS       = 0x4000,
        PSB_SQUARE      = 0x8000
    };

    enum ps2_analog
    {
        PSA_RX = 5,
        PSA_RY,
        PSA_LX,
        PSA_LY,
        PSA_PAD_RIGHT = 9,
        PSA_PAD_UP = 11,
        PSA_PAD_DOWN = 12,
        PSA_PAD_LEFT = 10,
        PSA_L2 = 19,
        PSA_R2 = 20,
        PSA_L1 = 17,
        PSA_R1 = 18,
        PSA_GREEN = 13,
        PSA_RED = 14,
        PSA_BLUE = 15,
        PSA_PINK = 16,
        PSA_TRIANGLE = 13,
        PSA_CIRCLE = 14,
        PSA_CROSS = 15,
        PSA_SQUARE = 16,
    };

    enum ps2_config_result
    {
        OK = 0,
        NO_CONTROLLER,
        NO_CONNECTION
    };

    const SPISettings gamepad_settings(500000, LSBFIRST, SPI_MODE2);

    #ifdef HAND_REGISTER_T
        typedef HAND_REGISTER_T register_t;
    #else
        typedef uint32_t register_t;
    #endif
    const static uint8_t m_enter_cfg[5] = {0x01, 0x43, 0x00, 0x01, 0x00};
    const static uint8_t m_set_mode[] = {0x01, 0x44, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00};
    const static uint8_t m_set_bytes_large[] = {0x01,0x4F,0x00,0xFF,0xFF,0x03,0x00,0x00,0x00};
    const static uint8_t m_exit_cfg[] = {0x01,0x43,0x00,0x00,0x5A,0x5A,0x5A,0x5A,0x5A};
    const static uint8_t m_enable_rumble[] = {0x01,0x4D,0x00,0x00,0x01};
    const static uint8_t m_type_read[] = {0x01,0x45,0x00,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A};

    class ps2_gamepad
    {
    private:
        void sendCommand(const uint8_t* buffer, uint8_t* output, uint16_t size);

        void sendCommand(const uint8_t* buffer, uint16_t size);

        void send(uint8_t* buffer, uint16_t size);

        void reconfigure();

        void readGamepad(uint8_t motor1, uint8_t motor2);


    public:
        ps2_gamepad(uint8_t ss, bool rumble, bool pressures);

        ps2_config_result configure();

        void update(uint8_t m1, uint8_t m2)
        {
            readGamepad(m1, m2);
        }

        bool button(ps2_button btn)
        {
            return ~m_buttons & btn;
        }

        bool changed()
        {
            return m_last_buttons ^ m_buttons;
        }

        bool changed(ps2_button btn)
        {
            return (m_last_buttons ^ m_buttons) & btn;
        }

        bool pressed(ps2_button btn)
        {
            return changed(btn) && button(btn);
        }

        bool released(ps2_button btn)
        {
             return changed(btn) && !button(btn);
        }

        uint8_t analog(ps2_analog data)
        {
            return m_buffer[data];
        }
    private:
        uint8_t m_ss_pin, m_ss_mask;
        volatile register_t *m_ss_reg;

        unsigned long m_last_read;
        bool m_rumble, m_pressures;
        uint8_t m_read_delay;
        uint8_t m_buffer[21], m_controller_type;
        uint16_t m_buttons, m_last_buttons;
    };
}
