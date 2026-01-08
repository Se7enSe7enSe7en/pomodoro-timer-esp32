# Refactoring Maxims

## 1. Magic Numbers

- **Bad**: `if (millis() - last_time > 1000)`
- **Good**: `const unsigned long ONE_SECOND = 1000;` ...
- **Why**: 6 months from now, you won't remember what "1000" means.

## 2. Single Responsibility Principle (SRP)

- **Bad**: A `loop()` function that is 100 lines long and handles buttons, LEDs, and timing.
- **Good**: `checkButtons()`, `updateTimer()`, `updateDisplay()`.
- **Why**: If the button breaks, you know exactly where to look.

## 3. Variable Scope

- **Bad**: Global variables for everything.
- **Good**: `static` variables inside functions, or variables passed as arguments.
- **Why**: Keeps your program state clean and predictable.
