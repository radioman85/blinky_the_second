#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "button_interrupt_controlled_led.h"
#include "led_utils.h"


#define SW0_NODE DT_ALIAS(sw0)
const struct gpio_dt_spec sw_0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

static const struct gpio_dt_spec *_led = NULL;

//--- Private Function Prototype --------------------------------
void _configure_button_interrupt(void);

//--- Public Function Definition -------------------------------
void init_button_interrupt_led(const struct gpio_dt_spec *led) {
    _led = led;
    _configure_button_interrupt();
}

void _button_pressed_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    if (_led) {
        gpio_pin_toggle_dt(_led);
    }
}

static struct gpio_callback button_cb_data;

void _configure_button_interrupt(void)
{
    int ret;

    ret = gpio_pin_configure_dt(&sw_0, GPIO_INPUT);
    if (ret != 0) {
        printf("Error %d: failed to configure button pin %s\n", ret, sw_0.port->name);
        return;
    }

    ret = gpio_pin_interrupt_configure_dt(&sw_0, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printf("Error %d: failed to configure interrupt on %s pin %d\n", ret, sw_0.port->name, sw_0.pin);
        return;
    }

    gpio_init_callback(&button_cb_data, _button_pressed_isr, BIT(sw_0.pin));
    gpio_add_callback(sw_0.port, &button_cb_data);
    printf("Set up button at %s pin %d\n", sw_0.port->name, sw_0.pin);
}
