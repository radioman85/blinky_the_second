#include "pwm_controller.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/device.h>

// RC Servo PWM configuration
#define PWM_PERIOD_NS 20000000U   // 20ms period (50Hz) - RC servo standard
#define PWM_MIN_PULSE_NS 1000000U // 1ms minimum pulse (0%)
#define PWM_MAX_PULSE_NS 2000000U // 2ms maximum pulse (100%)
#define PWM_CENTER_PULSE_NS 1500000U // 1.5ms center pulse (50%)

// PWM device tree specification - get first available PWM device
static const struct device *pwm_dev;
static const uint32_t pwm_channel = 0;

// Static variables
static uint16_t current_position = 500; // Center position (1.5ms)
static bool pwm_enabled = false;

int init_pwm_controller(void)
{
    // Try to get PWM2 device first
    pwm_dev = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(pwm2));
    if (!pwm_dev) {
        pwm_dev = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(pwm1));
        if (!pwm_dev) {
            pwm_dev = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(pwm0));
            if (!pwm_dev) {
                printk("Error: No PWM device available\n");
                return -ENODEV;
            }
        }
    }

    // Check if PWM device is ready
    if (!device_is_ready(pwm_dev)) {
        printk("Error: PWM device not ready\n");
        return -ENODEV;
    }
    // Start with center position (50% = 1.5ms pulse)
    int ret = pwm_set(pwm_dev, pwm_channel, PWM_PERIOD_NS, PWM_CENTER_PULSE_NS, PWM_POLARITY_NORMAL);
    if (ret < 0) {
        printk("Error setting PWM: %d\n", ret);
        return ret;
    }
    current_position = 500; // Center position = 500/1000
    pwm_enabled = true;
    
    return 0;
}

int set_pwm_duty_cycle(uint16_t position)
{
    if (!pwm_enabled || !pwm_dev) {
        printk("PWM not initialized\n");
        return -ENODEV;
    }
    
    // Convert 0-1000 to RC servo pulse width (1ms to 2ms)
    uint32_t pulse_ns = PWM_MIN_PULSE_NS + ((PWM_MAX_PULSE_NS - PWM_MIN_PULSE_NS) * position) / 1000;
    
    int ret = pwm_set(pwm_dev, pwm_channel, PWM_PERIOD_NS, pulse_ns, PWM_POLARITY_NORMAL);
    if (ret < 0) {
        printk("Error setting PWM duty cycle: %d\n", ret);
        return ret;
    }
    
    current_position = position;
    return 0;
}

uint16_t get_pwm_duty_cycle(void)
{
    return current_position;
}

int enable_pwm(bool enable)
{
    if (enable && !pwm_enabled) {
        // Enable with current duty cycle
        return set_pwm_duty_cycle(current_position);
    } else if (!enable && pwm_enabled) {
        // Disable by setting to 0
        uint16_t saved_position = current_position;
        int ret = set_pwm_duty_cycle(0);
        current_position = saved_position;  // Restore for next enable
        pwm_enabled = false;
        return ret;
    }
    
    return 0;  // Already in desired state
}