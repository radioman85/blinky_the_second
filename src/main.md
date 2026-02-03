# Module: main

## Purpose

Application entry point for the Zephyr firmware. Wires together the different LED control mechanisms (button interrupt, shell command, Bluetooth + PWM) and initializes the hardware peripherals.

## Responsibilities

- Configure logical LEDs via devicetree aliases.
- Initialize per-feature modules:
  - Button-controlled LED (interrupt driven)
  - Shell-command controlled LED
  - Hardware PWM controller
  - Bluetooth NUS-based service for remote control
- Register the BLE data callback that forwards received data to the PWM controller.
- Maintain the main thread sleep loop.

## Key Interactions

- Uses `configure_led_pin` from `led_utils` to configure GPIOs.
- Calls `init_button_interrupt_led` from `button_interrupt_controlled_led`.
- Calls `init_led_shell_cmd_control` from `led_shell_cmd_control`.
- Calls `init_pwm_controller` and `set_pwm_duty_cycle` from `pwm_controller`.
- Calls `init_bluetooth_service` and `set_ble_data_callback` from `bluetooth_service`.

## Notes

- LED1 (alias `led0`) is reserved for hardware PWM and is not toggled via GPIO in `main`.
- The BLE callback interprets incoming data either as ASCII digits (0-1000) or as a raw byte, then clamps and forwards it to the PWM controller.