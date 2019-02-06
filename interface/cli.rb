#!/usr/bin/ruby
# Интерактивный консольный интерфейс для отправки команд контроллеру
require "./api"

module HandAPI
  class Command
    def initialize(&block)
      @block = block
    end

    def help(help)
      @help = help
      self
    end

    def brief(b)
      @brief = b
      self
    end

    def help!
      @help || ""
    end

    def brief!
      @brief || ""
    end

    def call(serial, cli, cmd)
      data = @block.call(cli, cmd)
      serial.execute data unless data.nil?
    end
  end

  class CLI


    def initialize(&block)
      @commands = {}
      block.call self

    end

    def add_command(name, &cmd)
      puts "! Warning: command #{name} already exists" if @commands.has_key? name
      command = Command.new &cmd
      @commands[name] = command
      command
    end

    def help(cmd)
      if cmd.nil?
        "Help: \n\n" + @commands.map do |k, v|
          "#{k} - #{v.brief!}"
        end.join("\n") + "\n\n use 'help <cmd>' to get help for <cmd>"
      else
        if @commands.has_key? cmd
          @commands[key].help!
        else
          "! Error: No command '#{cmd}'"
        end
      end
    end

    def help!(cmd)
      puts help(cmd)
    end

    def exec_line(line, serial)
      cmd_name = line.split(" ").first

      if @commands.has_key? cmd_name
        puts @commands[cmd_name].call(serial, self, line)
      else
        puts "! Error: Unknown command"
      end
    rescue => e
      puts "! Exception"
      puts e.inspect
      puts e.backtrace.join("\n")
    end

    def loop(serial)
      while true
        putc ">"
        cmd = gets
        exec_line cmd, serial
      end
    end
  end
end

# Порт можно передать аргументом или через переменную среды SERIAL
serial = HandAPI::Serial.new(ARGV.shift || ENV["SERIAL"])
puts "Hand CLI"

cli = HandAPI::CLI.new do |c|
  c.add_command "exit" do |cli, cmd|
    exit 0
  end.brief("exits immediately").help("exit - exit from command loop immediately")

  c.add_command "help" do |cli, cmd|
    arg = cmd.split(" ")[1]
    cli.help! arg
    nil
  end.brief("prints help").help("help - prints this help\nhelp <cmd> - prints help for <cmd>")

  c.add_command "read" do |cli, cmd|
    args = cmd.split(" ")
    raise "Missing required argument" if args.size < 2
    "1 #{args[1].to_i}"
  end.brief("reads servo data").help("read <i> - reads value from servo number <i>")

  c.add_command "write" do |cli, cmd|
    args = cmd.split(" ")
    raise "Missing required arguments" if args.size < 3
    "2 #{args[1].to_i} #{args[2].to_i}"
  end.brief("writes servo data").help("write <i> <value> - assigns <value> to servo number <i>")

  c.add_command "ping" do |cli, cmd|
    time = Time.now
    puts serial.execute "3"
    puts "> Latency: #{((Time.now - time).to_f * 1000).to_i} ms"
  end.brief("sends ping to the controller").help("ping - sends ping and prints latency")

  c.add_command "mw" do |cli, cmd|
    args = cmd.split(" ")
    speed = args[1].to_i
    count = args.size - 2
    servos = args[2..-1].map{ |x| x.sub("=", " ")}
    "4 #{speed} #{count.to_i} #{servos.join(" ")}"
  end.brief("assigns values to multiple servos simultaneously").help("mw <speed> <i>=<value> ... - assigns values to multiple servos (up to 8) simultaneously")

  c.add_command "rotate" do |cli, cmd|
    args = cmd.split(" ")
    raise "Missing required argument" if args.size < 2
    "5 #{args[1].to_i} 0"
  end.brief("rotates the hand")

  c.add_command "rotate_r" do |cli, cmd|
    args = cmd.split(" ")
    raise "Missing required argument" if args.size < 2
    "5 #{args[1].to_i} 1"
  end.brief("rotates the hand relatively")

  c.add_command "extend" do |cli, cmd|
    args = cmd.split(" ")
    raise "Missing required argument" if args.size < 2
    "6 #{args[1].to_i}"
  end.brief("extends the hand relatively")

  c.add_command "capture" do |cli, cmd|
    args = cmd.split(" ")
    raise "Missing required argument" if args.size < 2
    "7 #{args[1].to_i}"
  end.brief("sets the state of claw")

  c.add_command "mw2" do |cli, cmd|
    args = cmd.split(" ")
    speed = args[1].to_i
    count = args.size - 2
    servos = args[2..-1].map{ |x| x.sub("=", " ")}
    "8 #{speed} #{count.to_i} #{servos.join(" ")}"
  end.brief("assigns values to max 2 servos simultaneously").help("mw2 <speed> <i>=<value> ... - assigns values to multiple servos (up to 2) simultaneously")

  c.add_command "mw4" do |cli, cmd|
    args = cmd.split(" ")
    speed = args[1].to_i
    count = args.size - 2
    servos = args[2..-1].map{ |x| x.sub("=", " ")}
    "9 #{speed} #{count.to_i} #{servos.join(" ")}"
  end.brief("assigns values to max 4 servos simultaneously").help("mw <speed> <i>=<value> ... - assigns values to multiple servos (up to 4) simultaneously")

  c.add_command "script" do |cli, cmd|
    args = cmd.split(" ")
    raise "Missing required argument" if args.size < 2
    name = args[1]

    File.open(name) do |f|
        f.each_line("\n") do |line|
          puts ">> #{line}"
          cli.exec_line line, serial
        end
    end
    nil
  end

  c.add_command "joint" do |cli, cmd|
    args = cmd.split(" ")
    raise "Missing required arguments" if args.size < 3

    joint = case args[1]
    when "0", "rotary"
      0
    when "1", "first"
      1
    when "2", "second"
      2
    when "3", "claw"
      3
    else
      raise "Invalid joint #{args[1]}"
    end

    "10 #{joint} #{args[2].to_i}"
  end
end

serial.open do
  cli.loop(serial)
end
