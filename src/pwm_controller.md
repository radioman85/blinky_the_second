# Module: pwm_controller

## Purpose

Provide a hardware PWM-based RC servo control interface (50 Hz, 1–2 ms pulse width) for a servo or LED on P0.28.

## Responsibilities

- Discover and initialize a suitable PWM device (pwm2, fallback to pwm1 or pwm0).
- Generate 50 Hz PWM with standard RC servo pulse widths.
- Expose an abstract 0–1000 position/duty range to the rest of the application.
- Track current position and enable/disable state.

## Key APIs

- `int init_pwm_controller(void);`
  - Locates a usable PWM device.
  - Checks readiness with `device_is_ready`.
  - Sets initial center position (1.5 ms, 50%).

- `int set_pwm_duty_cycle(uint16_t position);`
  - Converts `position` (0–1000) to a pulse width between 1 ms and 2 ms.
  - Calls `pwm_set` with a fixed 20 ms period.
  - Updates the internal `current_position`.

- `uint16_t get_pwm_duty_cycle(void);`
  - Returns the last position set.

- `int enable_pwm(bool enable);`
  - Re-enables PWM at the last stored position, or disables it by setting 0 duty.

## Notes

- Designed to be driven by BLE data via `handle_ble_data` in `main.c`.
- The numeric range (0–1000) is an application-level convention and matches the ASCII parsing in the BLE handler.