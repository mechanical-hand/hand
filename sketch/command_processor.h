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
        command_processor(command_handler* h, size_t c, Stream& s, Print& p) :
            m_handlers(h),
            m_handlers_count(c),
            m_stream(s),
            m_reply(p)
        {}

        /**
         * @brief Обработчик команд
         *   
         * Читает номер команды из файла, затем вызывает соответствующий обработчик.
         * Если обработчик не найден, печатает в поток выходных данных строку "Invalid command".
         * @attention Может заблокировать выполнение кода, поэтому рекомендуется использовать обертку try_process()
         * @returns true, если команда обработана успешно
         */
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

        /**
         * @brief Функция-обертка над обработчиком команд
         * Данная обертка сначала проверяет, есть ли доступные для обработки данные, затем вызывает process()
         * @returns true, если команда обработана успешно
         */
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
