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
    when "rotate"
      "5 #{cmd[1]} 0"
    when "rotate_r"
      "5 #{cmd[1]} 1"
    when "extend"
      "6 #{cmd[1]}"
    when "capture", "squeeze"
      "7 #{cmd[1]}"
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
