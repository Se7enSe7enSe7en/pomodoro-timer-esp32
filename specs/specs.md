# Tech Stack

- C
- ESP-IDF (VSCode extension)

# Hardware

- Datasheet link: https://robu.in/wp-content/uploads/2024/04/Datasheet-3.pdf
- Documentation: https://doc.panel-tag.com/ESP32-S3/index.html#
- MCU: ESP32-S3
- Display: Panlee ZX3D95CE01S-TR-V12
- Display Driver: GC9503V
- Display interface: RGB
- Display resolution: 480(H)x3(RGB)x480(V)
- Touchscreen type: Capacitive touch
- Drive IC model: FT6336U (I2C)

## Pin Mapping

- LCD_BL (LCD backlight control, high level enable): GPIO 5
- RGB Interface (D0-D15):
  - D0: GPIO 45
  - D1: GPIO 48
  - D2: GPIO 47
  - D3: GPIO 0
  - D4: GPIO 21
  - D5: GPIO 14
  - D6: GPIO 13
  - D7: GPIO 12
  - D8: GPIO 11
  - D9: GPIO 16
  - D10: GPIO 17
  - D11: GPIO 18
  - D12: GPIO 8
  - D13: GPIO 3
  - D14: GPIO 46
  - D15: GPIO 10
- RGB Control Signals:
  - VSYNC: GPIO 41
  - HSYNC: GPIO 42
  - DE (Data Enable): GPIO 40
  - PCLK (Pixel Clock): GPIO 39
- Touchscreen (I2C):
  - I2C Data (SDA): GPIO 15
  - I2C Clock (SCL): GPIO 45
  - Interrupt pin (INT): GPIO 4
  - Reset pin (RST): -

# Project

- Pomodoro timer
- User can start, pause, and resume the timer
- the timer is 25min. by default (in the future it should be adjustable)
- statuses:
  - IDLE
  - WORK
  - PAUSE
  - DONE
- simple UI, showing the timer and the state
