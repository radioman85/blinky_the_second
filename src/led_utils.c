#include "led_utils.h"
#include <zephyr/kernel.h>

int configure_led_pin(const struct gpio_dt_spec *led)
{
    if (!device_is_ready(led->port)) {
        printk("LED device not ready\n");
        return -ENODEV;
    }
    int ret = gpio_pin_configure_dt(led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        printk("Failed to configure LED pin\n");
    }
    return ret;
}
