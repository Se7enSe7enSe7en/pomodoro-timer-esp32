# Level 1: Learning Tasks (Unconscious Incompetence -> Conscious Incompetence)

## Objectives

- Understand the hardware constraints and interfaces.
- Design the software architecture (FSM) without writing code.
- Select appropriate libraries.

## Tasks

- [x] **Hardware Research**: Identify the communication interface for the GC9503V display on the Panlee board (SPI, I2S, RGB, or I8080?).
- [x] **Input Research**: How does this board handle user input? (Touch screen controller type? Native buttons?)
- [x] **Graphics Library**: Research "LVGL on ESP32". Why is it the standard? What alternatives exist?
- [x] **Timing**: Compare "FreeRTOS Software Timers" vs "ESP32 Hardware Timers". Which is suitable for a minute-resolution countdown?
- [x] **State Machine**: Draft a state diagram for the Pomodoro timer (IDLE -> WORK -> PAUSE, etc.).
  - IDLE + Start -> WORK
  - WORK + Pause -> PAUSE
  - PAUSE + Resume -> WORK
  - WORK + Timer Expired -> DONE
  - DONE + Reset -> IDLE
