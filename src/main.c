
#include <zephyr/kernel.h>

#include <zephyr/drivers/gpio.h>
#include "toggle_led_thread.h"
#include "led_utils.h"
#include "button_interrupt_controlled_led.h"
#include "led_shell_cmd_control.h"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

void main(void)
{
    int ret;

    static const struct gpio_dt_spec led_1 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
    ret = configure_led_pin(&led_1);
    if (ret < 0) {
        printk("Failed to configure LED1\n");
    }
    init_toggle_led_thread(&led_1);

    static const struct gpio_dt_spec led_2 = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
    ret = configure_led_pin(&led_2);
    if (ret < 0) {
        printk("Failed to configure LED2\n");
    }
    init_button_interrupt_led(&led_2);

    static const struct gpio_dt_spec led_3 = GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios);
    ret = configure_led_pin(&led_3);
    if (ret < 0) {
        printk("Failed to configure LED3\n");
    }
    init_led_shell_cmd_control(&led_3);

    while (1) {
        k_msleep(SLEEP_TIME_MS);
    }
}
