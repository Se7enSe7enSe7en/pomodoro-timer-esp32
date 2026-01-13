# Level 2: Improvements & Maxims

## The Clean Coder's Checklist (ESP32 Edition)

### 1. Separation of Concerns

- **Maxim**: "A component should know HOW to do a job, but not WHEN to do it."
- **Check**: Does your display driver know about "Pomodoro"? (It shouldn't. It should only know about "Pixels" or "Widgets").

### 2. No Magic Numbers

- **Maxim**: "Give every constant a name."
- **Check**: Are you using `1000` for a delay? Define `#define TICK_PERIOD_MS 1000`.

### 3. Error Handling

- **Maxim**: "Assume everything will fail."
- **Check**: Did you check the return code of `esp_timer_create`? What if `malloc` returns NULL?

### 4. Global State

- **Maxim**: "Globals are guilty until proven innocent."
- **Check**: Can this variable be `static` inside the function? Can it be passed as a struct pointer?

## Log of Critiques

_(This section will be populated as you write code)_
