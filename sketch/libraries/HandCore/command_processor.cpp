#include "command_processor.h"

using namespace hand;

command_processor::command_processor(command_handler* h, size_t c, Stream& s, Print& p) :
    m_handlers(h),
    m_handlers_count(c),
    m_stream(s),
    m_reply(p)
{}

bool command_processor::process()
{
    unsigned int cmd = m_stream.parseInt();
    if(cmd >= m_handlers_count || !m_handlers[cmd])
    {
        if(cmd) m_reply.println("Invalid command");
        return false;
    }
    return m_handlers[cmd](m_stream, m_reply);
}

bool command_processor::try_process()
{
    if(m_stream.available())
    {
        return process();
    }
    return false;
}
