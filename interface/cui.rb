require "optparse"
require "./api"

write = nil
read = false
ping = false
verbose = false

OptionParser.new do |o|
  o.banner = "Hand CUI. Usage : #{__FILE__} [OPTIONS] <serial> [<servo>]"
  o.on("-wVALUE", "--write=VALUE", "Write value") { |v| write = v }
  o.on("-r", "--read", "Read value") { read = true }
  o.on("-p", "--ping", "Ping") { ping = true }
  o.on("-v", "--verbose", "Human readable output") { verbose = true }
end.parse!

serial = ARGV.shift
addr = (read || write ? ARGV.shift : nil)

ping = true if !read && !write

actions = []
actions << "3" if ping
actions << "1 #{addr}" if read
actions << "2 #{addr} #{write}" if write

s = HandAPI::Serial.new serial
s.open do
  actions.each do |a|
    puts "COMMAND : #{a}" if verbose
    cmd = s.write a
    if verbose
      puts "REPLY   : #{cmd}"
    else
      puts cmd
    end
  end
end
