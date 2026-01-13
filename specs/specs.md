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
- Drive IC model: FT6336U

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
