# Module: led_utils

## Purpose

Provide small reusable helpers for configuring LED GPIO pins using Zephyr devicetree abstractions.

## Responsibilities

- Configure a GPIO pin described by `gpio_dt_spec` as an output and set an initial state.

## Key API

- `int configure_led_pin(const struct gpio_dt_spec *led);`
  - Validates that the underlying GPIO device is ready.
  - Calls `gpio_pin_configure_dt` with `GPIO_OUTPUT_ACTIVE`.
  - Returns 0 on success, negative errno on failure.

## Used By

- `main` (for LED2/LED3 and the BLE status LED).
- `bluetooth_service` (to configure the connection status LED).
- Potentially reusable for any future LED-based modules.