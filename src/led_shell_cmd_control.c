#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/shell/shell.h>
#include "led_shell_cmd_control.h"


static const struct gpio_dt_spec *_led = NULL;

void init_led_shell_cmd_control(const struct gpio_dt_spec *led) {
    _led = led;
}

static int cmd_control_led(const struct shell *shell, size_t argc, char **argv)
{
    if (argc != 2) {
        shell_print(shell, "Usage: led <on|off>");
        return -EINVAL;
    }

    if (!_led) {
        shell_print(shell, "LED not initialized");
        return -ENODEV;
    }
    if (strcmp(argv[1], "on") == 0) {
        gpio_pin_set_dt(_led, 1);
        shell_print(shell, "LED turned on");
    } else if (strcmp(argv[1], "off") == 0) {
        gpio_pin_set_dt(_led, 0);
        shell_print(shell, "LED turned off");
    } else {
        shell_print(shell, "Invalid argument: %s", argv[1]);
        return -EINVAL;
    }

    return 0;
}

SHELL_CMD_REGISTER(led, NULL, "Control LED (on/off)", cmd_control_led);

