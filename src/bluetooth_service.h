#ifndef BLUETOOTH_SERVICE_H
#define BLUETOOTH_SERVICE_H

#include <zephyr/drivers/gpio.h>
#include <stdint.h>

/**
 * @brief Callback function type for BLE data reception
 * 
 * @param data Pointer to received data
 * @param len Length of received data
 */
typedef void (*ble_data_callback_t)(const uint8_t *data, uint16_t len);

/**
 * @brief Initialize the Bluetooth service
 * 
 * This function initializes Bluetooth, sets up the Nordic UART Service (NUS),
 * starts advertising, and configures the connection status LED.
 * 
 * @param status_led Pointer to GPIO spec for the connection status LED
 * @return int 0 on success, negative error code on failure
 */
int init_bluetooth_service(const struct gpio_dt_spec *status_led);

/**
 * @brief Set callback function for BLE data reception
 * 
 * @param callback Function to call when BLE data is received
 */
void set_ble_data_callback(ble_data_callback_t callback);

#endif // BLUETOOTH_SERVICE_H