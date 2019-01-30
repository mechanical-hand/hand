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
      puts "! Warning: too many servos (max 8)" if cmd.size > 10
      "4 #{cmd[1]} #{cmd.size - 2} #{cmd[2..-1].map{ |x| x.split("=").join(" ")}.join(" ")}"
    when "rotate"
      "5 #{cmd[1]} 0"
    when "rotate_r"
      "5 #{cmd[1]} 1"
    when "extend"
      "6 #{cmd[1]}"
    when "capture", "squeeze"
      "7 #{cmd[1]}"
    when "mw2"
      puts "! Warning: too many servos (max 2)" if cmd.size > 4
      "8 #{cmd[1]} #{cmd.size - 2} #{cmd[2..-1].map{ |x| x.split("=").join(" ")}.join(" ")}"
    when "mw4"
      puts "! Warning: too many servos (max 4)" if cmd.size > 6
      "9 #{cmd[1]} #{cmd.size - 2} #{cmd[2..-1].map{ |x| x.split("=").join(" ")}.join(" ")}"
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
