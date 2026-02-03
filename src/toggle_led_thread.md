# Module: toggle_led_thread

## Purpose

Demonstrate controlling an LED from a dedicated Zephyr thread that periodically toggles a GPIO.

## Responsibilities

- Own a `k_thread` (defined via `K_THREAD_DEFINE`) that runs `_toggle_led_thread`.
- Periodically toggle a single LED at a fixed interval.

## Key API

- `void init_toggle_led_thread(const struct gpio_dt_spec *led);`
  - Stores the LED descriptor used by the background thread.

## Behavior

- `_toggle_led_thread`:
  - Runs in an infinite loop.
  - If an LED has been assigned, calls `gpio_pin_toggle_dt` on that LED.
  - Sleeps for `SLEEP_TIME_MS` (1 second) between toggles.

## Notes

- Currently not actively used in `main.c` (the call is commented out), but the module is ready to be enabled for experiments with threaded LED control.
- Shares the same LED utility conventions as other modules (`gpio_dt_spec`).