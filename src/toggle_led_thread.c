#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "toggle_led_thread.h"

#define LED0_NODE DT_ALIAS(led0)

const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void toggle_led_thread(void)
{
    while (1) {
        gpio_pin_toggle_dt(&led);
        k_msleep(SLEEP_TIME_MS);
    }
}

K_THREAD_DEFINE(toggle_led_tid, 1024, toggle_led_thread, NULL, NULL, NULL, 7, 0, 0);

void configure_led(void)
{
    int ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        printk("Failed to configure LED pin\n");
    }
}