# Module: led_shell_cmd_control

## Purpose

Expose simple shell commands to control an LED from the Zephyr shell (e.g., over UART).

## Responsibilities

- Register a `led` shell command.
- Parse `on` / `off` arguments.
- Drive a configured LED GPIO accordingly.

## Key API

- `void init_led_shell_cmd_control(const struct gpio_dt_spec *led);`
  - Stores the LED descriptor used by the shell command handler.

## Behavior

- `cmd_control_led` handler:
  - Validates argument count (`led <on|off>`).
  - Verifies that an LED has been initialized.
  - On `on`, calls `gpio_pin_set_dt(_led, 1)`.
  - On `off`, calls `gpio_pin_set_dt(_led, 0)`.
  - Prints status or error messages via `shell_print`.
- Registered with `SHELL_CMD_REGISTER(led, ...)`.

## Notes

- Assumes exactly one LED instance; if you need multiple LEDs, you’d introduce subcommands or additional arguments.
- Useful for quick manual testing or scripted control during development.