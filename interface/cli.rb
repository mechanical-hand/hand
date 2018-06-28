#!/usr/bin/ruby
# Интерактивный консольный интерфейс для отправки команд контроллеру
require "./api"

# Порт можно передать аргументом или через переменную среды SERIAL
s = HandAPI::Serial.new(ARGV.shift || ENV["SERIAL"])
puts "Hand CLI"
s.open do
  while true
    putc ">"
    
    # Получаем команду
    cmd = gets.split(" ")
    
    # Выходим из цикла, если команда - exit
    break if cmd.first == "exit"
    command = case cmd.first
    # Если команда - read, формируем команду чтения. Аналогично для остальных команд
    when "read"
      "1 #{cmd[1]}"
    when "write"
      "2 #{cmd[1]} #{cmd[2]}"
    when "ping"
      "3"
    when "mw"
      # Команда multiwrite
      "4 #{cmd.size - 1} #{cmd[1..-1].map{ |x| x.split("=").join(" ")}.join(" ")}"
    else
      puts "Unknown command"
      next
    end
    begin
      # Выполняем команду
      puts s.execute command
    rescue => e
      # Выводим исключение
      puts "Exception : #{e.inspect}"
    end
  end
end
