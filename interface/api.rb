module HandAPI
  # Абстракция для коммуникации с контроллером
  class Serial
    # Конструктор
    #
    # @param dev [String] Путь к файлу устройства (например, /dev/ttyUSB0)
    def initialize(dev)
      @dev = dev
    end

    # Открывает последовательный порт и вызывает блок, затем закрывает файл
    #
    # Этот метод устанавливает переменную @file
    # @param block [Proc] Выполняемый блок
    def open(&block)
      File.open @dev, "w+" do |f|
        @file = f
        block.call
      end
    end

    # Отправляет команду в последовательный порт
    #
    # @param str [String] Команда для отправки (например, "4 2 0 60 1 100")
    # @returns Ответ контроллера
    def write(str)
      @file.write str
      @file.flush
      cmd = ""
      while !cmd.to_s.end_with?("\n") || cmd.match(/^Initialized/) || cmd.to_s.size < 2 || cmd.match(/^Log:/)
        if cmd.match(/^Initialized/)
          puts "RETRY"
          @file.write str
          @file.flush
          cmd = ""
        elsif cmd.match(/^Log:/)
          puts cmd
        else
          fgets = @file.gets "\n"
          #puts fgets if fgets.to_s.size > 0

          cmd = cmd.to_s + fgets.to_s
          cmd += "\n" if fgets.to_s.match("\n") && !cmd.match("\n")
          break if cmd.match(/^Invalid command/)
        end
      end
      cmd
    end

    # Обертка над #write.
    #
    # Выводит команду и ответ в консоль
    # @param cmd [String] Команда
    # @throws RuntimeError с текстом ошибки, если контроллер сообщил об ошибке
    # @returns Значение, переданное контроллером (например, в ответ на команду чтения) или nil в случае его отсутствия
    def execute(cmd)
      puts "> #{cmd}"
      cmd = write(cmd).sub("\n", "")
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
