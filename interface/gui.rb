require "shoes"
require "./api"

serial = ARGV.shift || ENV["SERIAL"]
MIN = 0
MAX = 180
SERVOS = 5

s = HandAPI::Serial.new serial
s.open do
  Shoes.app do

    stack margin: 8 do
      SERVOS.times do |i|

        position = MIN

        b_prev = button "<"
        label = caption "Servo #{i} : #{position}"
        b_next = button ">"

        b_prev.click do
            position -= 5
            position = MIN if position < MIN
            label.replace "Servo #{i} : #{position}"
            s.execute "2 #{i} #{position}"
        end

        b_next.click do
            position += 5
            position = MAX if position > MAX
            label.replace "Servo #{i} : #{position}"
            s.execute "2 #{i} #{position}"
        end

        b_upd = button "Update"
        b_upd.click do
          position = s.execute "1 #{i}"
          label.replace "Servo #{i} : #{position}"
        end
      end

    end

  end
end
