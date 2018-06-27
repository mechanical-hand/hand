require "./api"

s = HandAPI::Serial.new(ARGV.shift || ENV["SERIAL"])
puts "Hand CLI"
s.open do

  while true
    putc ">"
    cmd = gets.split(" ")
    break if cmd.first == "exit"
    command = case cmd.first
    when "read"
      "1 #{cmd[1]}"
    when "write"
      "2 #{cmd[1]} #{cmd[2]}"
    when "ping"
      "3"
    when "mw"
      "4 #{cmd.size - 1} #{cmd[1..-1].map{ |x| x.split("=").join(" ")}.join(" ")}"
    else
      puts "Unknown command"
      next
    end
    begin
      puts s.execute command
    rescue => e
      puts "Exception : #{e.inspect}"
    end
  end
end
