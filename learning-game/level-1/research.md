# Research: Core Concepts for ESP32 Pomodoro Timer

## 1. Non-blocking Timing (The "Blink Without Delay" principle)

- **Problem**: `delay(1000)` pauses the _entire_ processor. You can't check buttons while delaying.
- **Solution**: Check the clock constantly in the loop.
- **Resource**: [Arduino: Blink Without Delay](https://docs.arduino.cc/built-in-examples/digital/BlinkWithoutDelay)
- **Resource**: [Adafruit: Multi-tasking the Arduino - Part 1](https://learn.adafruit.com/multi-tasking-the-arduino-part-1)

## 2. Finite State Machines (FSM)

- **Definition**: A model of computation where the system can be in exactly one of a finite number of active states.
- **Relevance**: Your timer is either "Working", "Paused", or "On Break". It can't be both working and on break.
- **Resource**: [Majenko's State Machine Tutorial](https://hackingmajenkoblog.wordpress.com/2016/02/01/the-finite-state-machine/)
- **Resource**: [Programming Electronics Academy: State Machine (Video/Article)](https://www.programmingelectronics.com/state-machine/)

## 3. Button Debouncing

- **Problem**: Mechanical buttons bounce, sending multiple signals in milliseconds.
- **Resource**: [Arduino: Debounce](https://docs.arduino.cc/built-in-examples/digital/Debounce)
- **Resource**: [Espressif GPIO API Guide](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/gpio.html)
