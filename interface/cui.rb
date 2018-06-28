#!/usr/bin/ruby
# Консольный интерфейс для отправки до 3 различных команд контроллеру
# При задании нескольких команд отправляет сначала Ping, затем Read, затем Write
# Команда MultiWrite не поддерживается

require "optparse"
require "./api"

write = nil
read = false
ping = false
verbose = false

# Парсер опций командной строки
OptionParser.new do |o|
  o.banner = "Hand CUI. Usage : #{__FILE__} [OPTIONS] <serial> [<servo>]"
  o.on("-wVALUE", "--write=VALUE", "Write value") { |v| write = v }
  o.on("-r", "--read", "Read value") { read = true }
  o.on("-p", "--ping", "Ping") { ping = true }
  o.on("-v", "--verbose", "Human readable output") { verbose = true }
  o.on("-h", "--help", "Print this help"){ puts o; exit }
end.parse!

# Адрес порта первым аргументом (парсер опций удаляет из ARGV флаги и их аргументы)
serial = ARGV.shift

# Для команд read и write нужен номер сервомашинки
addr = (read || write ? ARGV.shift : nil)

# По умолчанию посылается команда ping
ping = true if !read && !write

actions = []
actions << "3" if ping
actions << "1 #{addr}" if read
actions << "2 #{addr} #{write}" if write

# Открывается порт
s = HandAPI::Serial.new serial
s.open do
  # Затем для каждой команды
  actions.each do |a|
    # Выводится команда на экран
    puts "COMMAND : #{a}" if verbose
    # Затем посылается контроллеру
    cmd = s.write a
    # Затем выводится ответ
    if verbose
      puts "REPLY   : #{cmd}"
    else
      puts cmd
    end
  end
end
