#pragma once
#include <Arduino.h>

namespace hand
{
    template<typename TStream>
    class logger
    {
    public:
        logger(TStream& print) :
            m_print(print)
        {}

        void begin()
        {
            if(m_nest_count > 0) m_print.println();
            m_print.print("Log: ");
            m_nest_count++;
        }

        void end()
        {
            m_print.println();
            m_print.flush();
            m_nest_count--;
        }

        template<typename T>
        void print(T data)
        {
            if(!m_nest_count) return;
            m_print.print(data);
        }

        template<typename T, typename T1>
        void print(T data, T1 format)
        {
            if(!m_nest_count) return;
            m_print.print(data, format);
        }

        template<typename T>
        void println(T data)
        {
            begin();
            print(data);
            end();
        }


    private:
        TStream &m_print;
        uint8_t m_nest_count;
    };

    template<typename T>
    logger<T> make_logger(T& stream)
    {
        return logger<T>(stream);
    }

    extern logger<HardwareSerial> *logger_instance;
}
