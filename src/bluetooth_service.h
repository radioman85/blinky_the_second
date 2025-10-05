#ifndef BLUETOOTH_SERVICE_H
#define BLUETOOTH_SERVICE_H

#include <zephyr/drivers/gpio.h>

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

#endif // BLUETOOTH_SERVICE_H