#pragma once
#include "command.h"
#include <Arduino.h>


namespace hand
{
    typedef bool(*command_handler)(Stream&, Print&);
    class command_processor
    {
    public:
        command_processor(command_handler* h, size_t c, Stream& s, Print& p) :
            m_handlers(h),
            m_handlers_count(c),
            m_stream(s),
            m_reply(p)
        {}

        bool process()
        {
            int cmd = m_stream.parseInt();
            if(cmd >= m_handlers_count || !m_handlers[cmd])
            {
                if(cmd) m_reply.println("Invalid command");
                return false;
            }
            return m_handlers[cmd](m_stream, m_reply);
        }

        bool try_process()
        {
            if(m_stream.available())
            {
                return process();
            }
            return false;
        }
    private:
        command_handler *m_handlers;
        size_t m_handlers_count;
        Stream &m_stream;
        Print &m_reply;
    };
}
