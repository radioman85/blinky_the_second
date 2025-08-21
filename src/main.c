
#include <zephyr/kernel.h>
#include "toggle_led_thread.h"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

void main(void)
{
        int ret;

        configure_led();

        while (1) {
                k_msleep(SLEEP_TIME_MS);
        }
}
