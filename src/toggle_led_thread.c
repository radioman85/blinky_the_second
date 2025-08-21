#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "toggle_led_thread.h"
#include "led_utils.h"

#define SLEEP_TIME_MS 1000

static const struct gpio_dt_spec *_led = NULL;

void init_toggle_led_thread(const struct gpio_dt_spec *led) {
    _led = led;
}

void _toggle_led_thread(void *unused0, void *unused1, void *unused2)
{
    while (1) {
        if (_led) {
            gpio_pin_toggle_dt(_led);
        }
        k_msleep(SLEEP_TIME_MS);
    }
}

K_THREAD_DEFINE(toggle_led_tid, 1024, _toggle_led_thread, NULL, NULL, NULL, 7, 0, 0);
