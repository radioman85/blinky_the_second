#include "pwm_controller.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

// GPIO configuration - Try P0.04
static const struct gpio_dt_spec test_pin = {
    .port = DEVICE_DT_GET(DT_NODELABEL(gpio0)),
    .pin = 4,
    .dt_flags = GPIO_ACTIVE_HIGH
};

// Static variables
static uint8_t current_duty_cycle = 0;
static bool pwm_enabled = false;

int init_pwm_controller(void)
{
    // Check if GPIO device is ready
    if (!device_is_ready(test_pin.port)) {
        printk("Error: GPIO device not ready\n");
        return -ENODEV;
    }

    // Configure as output
    int ret = gpio_pin_configure_dt(&test_pin, GPIO_OUTPUT);
    if (ret < 0) {
        printk("Error configuring GPIO pin: %d\n", ret);
        return ret;
    }

    // Start with pin LOW
    gpio_pin_set_dt(&test_pin, 0);

    printk("PWM controller initialized (GPIO mode) on P0.%d\n", test_pin.pin);
    
    return 0;
}

int set_pwm_duty_cycle(uint8_t duty_cycle)
{
    if (!device_is_ready(test_pin.port)) {
        printk("Error: GPIO not initialized\n");
        return -ENODEV;
    }

    // For now, just toggle the pin for any non-zero value
    // This will let us see activity on the logic analyzer
    if (duty_cycle > 0) {
        gpio_pin_set_dt(&test_pin, 1);  // Set HIGH
        printk("GPIO pin set HIGH (duty_cycle: %d)\n", duty_cycle);
    } else {
        gpio_pin_set_dt(&test_pin, 0);  // Set LOW
        printk("GPIO pin set LOW\n");
    }

    current_duty_cycle = duty_cycle;
    pwm_enabled = (duty_cycle > 0);
    
    return 0;
}

uint8_t get_pwm_duty_cycle(void)
{
    return current_duty_cycle;
}

int enable_pwm(bool enable)
{
    if (enable && !pwm_enabled) {
        // Enable with current duty cycle
        return set_pwm_duty_cycle(current_duty_cycle);
    } else if (!enable && pwm_enabled) {
        // Disable by setting to 0
        uint8_t saved_duty = current_duty_cycle;
        int ret = set_pwm_duty_cycle(0);
        current_duty_cycle = saved_duty;  // Restore for next enable
        pwm_enabled = false;
        return ret;
    }
    
    return 0;  // Already in desired state
}