#pragma once

namespace hand
{
    /* 
        Список возможных команд.
        Оставлен для справки. 
    */
    enum command
    {
        /* Нет команды */
        NONE,
        /* Команда чтения (аргумент - номер сервомашинки) */
        READ,
        /* Команда записи (аргументы - номер сервомашинки и ее желаемое положение */
        WRITE,
        /* Команда пинга (аргументов нет, контроллер должен отвечать "Pong") */
        PING,
        /* Команда для одновременного изменения положения машинок. Аргументы - число N (количество значений) и N пар чисел "машинка-положение") */
        MULTI_WRITE
    };

    /* Список возможных ответов. Оставлен для справки */
    enum reply
    {
        NO_REPLY = 0,
        SUCCESS,
        INVALID_COMMAND,
        INVALID_ARGUMENT,
        PONG,
        VALUE
    };

       
    //typedef char command_t, reply_t;
    //typedef char int_arg_t;
}