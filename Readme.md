## Blinky the Second

This project is a Zephyr-based firmware for Nordic Semiconductor nRF hardware that explores different ways of controlling LEDs, including button interrupts, shell commands, PWM, and dedicated threads.

The source code for the application lives in the `src` directory and is built using CMake and Zephyr's build system.

---

## Architecture Overview

At a high level, the firmware is structured as a small set of modules, all wired together in the application entry point in [src/main.c](src/main.c):

- **main**: initializes hardware and high-level services and registers callbacks.
- **led_utils**: shared helpers to configure LED GPIO pins.
- **button_interrupt_controlled_led**: toggles an LED from a button interrupt (SW0).
- **led_shell_cmd_control**: exposes a `led on/off` shell command.
- **pwm_controller**: provides a 0–1000 abstraction over a 50 Hz RC-servo-style PWM output.
- **bluetooth_service**: wraps Zephyr Bluetooth + Nordic UART Service (NUS) and forwards received data to the application.
- **toggle_led_thread**: optional background thread that can periodically toggle an LED.

The firmware uses Zephyr devicetree aliases (e.g. `led0`–`led3`, `sw0`) to bind logical LEDs and the button to the actual board pins.

### Data and Control Flow

1. **Startup**
	 - main configures LED GPIOs via led_utils.
	 - main initializes:
		 - button_interrupt_controlled_led (button → LED2)
		 - led_shell_cmd_control (shell → LED3)
		 - pwm_controller (hardware PWM on LED1 / P0.28)
		 - bluetooth_service (BLE + status LED4)
	 - main registers a BLE data callback that parses incoming data and passes a 0–1000 value to pwm_controller.

2. **Runtime interactions**
	 - A **button press** on SW0 fires a GPIO interrupt that toggles LED2.
	 - A **shell command** `led on` / `led off` toggles LED3.
	 - A **BLE central** writes bytes to the NUS RX characteristic:
		 - bluetooth_service receives data and calls the registered callback.
		 - The callback in main interprets the payload as either ASCII digits or a raw byte.
		 - The parsed value is clamped and forwarded to pwm_controller to update the PWM duty cycle (servo position / LED brightness style output).
	 - BLE connection state turns **LED4** on when connected and off when disconnected.

### Block Diagram

```mermaid
flowchart LR

		subgraph Board
				BTN[SW0 Button]
				LED1[LED1 (PWM / P0.28)]
				LED2[LED2]
				LED3[LED3]
				LED4[LED4 (BLE status)]
		end

		subgraph Firmware
				MAIN[main.c]
				LEDU[led_utils]
				BTNMOD[button_interrupt_controlled_led]
				SHELLMOD[led_shell_cmd_control]
				PWM[pwm_controller]
				BLE[bluetooth_service]
		end

		BTN -- GPIO interrupt --> BTNMOD
		BTNMOD -- toggle --> LED2

		SHELLMOD -- shell command --> LED3

		BLE -- status LED control --> LED4
		MAIN -- configure pins via --> LEDU

		MAIN -- init & use --> BTNMOD
		MAIN -- init & use --> SHELLMOD
		MAIN -- init & use --> PWM
		MAIN -- init & use --> BLE

		BLE -- NUS RX data --> MAIN
		MAIN -- duty value (0-1000) --> PWM
		PWM -- PWM signal --> LED1
```

---

## Module Documentation

Each module in the [src](src) directory has a corresponding markdown file next to it that documents its purpose, responsibilities, and key APIs, for example:

- [src/main.md](src/main.md)
- [src/led_utils.md](src/led_utils.md)
- [src/button_interrupt_controlled_led.md](src/button_interrupt_controlled_led.md)
- [src/led_shell_cmd_control.md](src/led_shell_cmd_control.md)
- [src/pwm_controller.md](src/pwm_controller.md)
- [src/bluetooth_service.md](src/bluetooth_service.md)
- [src/toggle_led_thread.md](src/toggle_led_thread.md)

These files are intended as living documentation of the code structure and behavior.

