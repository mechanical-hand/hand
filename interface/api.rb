module HandAPI
  class Serial
    def initialize(dev)
      @dev = dev
    end

    def open(&block)
      File.open @dev, "w+" do |f|
        @file = f
        block.call
      end
    end

    def write(str)
      @file.write str
      @file.flush
      cmd = ""
      while !cmd.to_s.end_with?("\n") || cmd.match(/^Initialized/) || cmd.to_s.size < 2
        if cmd.match(/^Initialized/)
          puts "RETRY"
          @file.write str
          @file.flush
          cmd = ""
        else
          fgets = @file.gets
          #puts fgets if fgets.to_s.size > 0

          cmd = cmd.to_s + fgets.to_s
          cmd += "\n" if fgets.to_s.match("\n") && !cmd.match("\n")
          break if cmd.match(/^Invalid command/)
        end
      end
      cmd
    end

    def execute(cmd)
      puts "> #{cmd}"
      cmd = write cmd
      puts "< #{cmd}"
      raise cmd unless cmd.start_with?("Success")|| cmd.start_with?("Pong")
      if cmd.match(/Success: \d+/)
        cmd.gsub(/\D/, "").to_i
      else
        nil
      end
    end
  end
end
