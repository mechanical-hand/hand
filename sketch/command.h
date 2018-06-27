#pragma once

namespace hand
{
    enum command
    {
        NONE,
        READ,
        WRITE,
        PING
    };

    enum reply
    {
        NO_REPLY = 0,
        SUCCESS,
        INVALID_COMMAND,
        INVALID_ARGUMENT,
        PONG,
        VALUE
    };

    typedef char command_t, reply_t;
    typedef char int_arg_t;
}
