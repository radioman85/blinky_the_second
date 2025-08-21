#ifndef LED_UTILS_H
#define LED_UTILS_H

#include <zephyr/drivers/gpio.h>

int configure_led_pin(const struct gpio_dt_spec *led);

#endif // LED_UTILS_H
