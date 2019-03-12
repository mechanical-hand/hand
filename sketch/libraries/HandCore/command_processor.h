#pragma once
//#include "command.h"
#include <Arduino.h>


namespace hand
{
    /* Тип обработчика команд (указатель на функцию принимающую ссылки на Stream и Print и возвращащую bool */
    typedef bool(*command_handler)(Stream&, Print&);

    /**
     * @brief Класс-обработчик команд
     */
    class command_processor
    {
    public:

        /**
         * @param h Массив обработчиков команд
         * @param c Число обработчиков команд
         * @param s Поток входных данных
         * @param p Поток выходных данных
        */
        command_processor(command_handler* h, size_t c, Stream& s, Print& p);

        /**
         * @brief Обработчик команд
         *
         * Читает номер команды из файла, затем вызывает соответствующий обработчик.
         * Если обработчик не найден, печатает в поток выходных данных строку "Invalid command".
         * @attention Может заблокировать выполнение кода, поэтому рекомендуется использовать обертку try_process()
         * @returns true, если команда обработана успешно
         */
        bool process();

        /**
         * @brief Функция-обертка над обработчиком команд
         * Данная обертка сначала проверяет, есть ли доступные для обработки данные, затем вызывает process()
         * @returns true, если команда обработана успешно
         */
        bool try_process();
    private:
        command_handler *m_handlers;
        size_t m_handlers_count;
        Stream &m_stream;
        Print &m_reply;
    };
}
