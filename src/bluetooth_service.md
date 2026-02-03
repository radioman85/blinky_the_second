# Module: bluetooth_service

## Purpose

Wrap Zephyr Bluetooth initialization and the Nordic UART Service (NUS) into a simple service that can be used to control the application via BLE. Also drives a status LED that reflects connection state.

## Responsibilities

- Initialize Zephyr Bluetooth stack.
- Initialize and configure Nordic UART Service (NUS).
- Start connectable advertising.
- Manage connection callbacks for connect/disconnect events.
- Drive a status LED that indicates connection state.
- Forward received NUS data to a registered application callback.

## Key APIs

- `int init_bluetooth_service(const struct gpio_dt_spec *status_led);`
  - Stores and configures the status LED using `configure_led_pin` (from `led_utils`).
  - Calls `bt_enable` to bring up the BLE stack.
  - Initializes NUS via `bt_nus_init` with `bt_receive_cb` as the receive callback.
  - Starts advertising with `bt_le_adv_start(BT_LE_ADV_CONN_NAME, ...)`.

- `void set_ble_data_callback(ble_data_callback_t callback);`
  - Registers a higher-level callback invoked with raw received data (pointer + length).

## Behavior

- Connection callbacks (`connected` / `disconnected`):
  - On connect: sets the status LED GPIO high.
  - On disconnect: sets the status LED GPIO low.

- `bt_receive_cb`:
  - Prints the received bytes as a raw line using `printk`.
  - If a data callback is registered, forwards the raw payload.

## Notes

- The module itself does not interpret the payload; that is left to the application (e.g., mapping ASCII numbers to PWM positions).
- Uses the standard Zephyr NUS profile for simple UART-like BLE communication.