#include "pwm_controller.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/device.h>

// PWM configuration - P0.04 using PWM0 channel 0
#define PWM_PERIOD_NS 100000000U  // 100ms period (10Hz)
#define PWM_DUTY_50_NS 50000000U  // 50ms duty (50%)

// PWM device tree specification - get first available PWM device
static const struct device *pwm_dev;
static const uint32_t pwm_channel = 0;

// Static variables
static uint8_t current_duty_cycle = 0;
static bool pwm_enabled = false;

int init_pwm_controller(void)
{
    printk("Attempting to initialize Hardware PWM...\n");
    
    // Try to get PWM2 device first
    pwm_dev = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(pwm2));
    if (!pwm_dev) {
        printk("PWM2 not available, trying PWM1...\n");
        pwm_dev = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(pwm1));
        if (!pwm_dev) {
            printk("PWM1 not available, trying PWM0...\n");
            pwm_dev = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(pwm0));
            if (!pwm_dev) {
                printk("Error: No PWM device available\n");
                return -ENODEV;
            }
        }
    }
    
    printk("PWM device: %p\n", pwm_dev);
    printk("PWM channel: %d\n", pwm_channel);
    
    // Check if PWM device is ready
    if (!device_is_ready(pwm_dev)) {
        printk("Error: PWM device not ready\n");
        return -ENODEV;
    }
    printk("PWM device is ready!\n");

    printk("Hardware PWM controller initialized on P0.28 (channel %d)\n", pwm_channel);
    
    // Start with hard-coded 50% duty cycle at 10Hz
    printk("Setting PWM: period=%u ns, duty=%u ns\n", PWM_PERIOD_NS, PWM_DUTY_50_NS);
    int ret = pwm_set(pwm_dev, pwm_channel, PWM_PERIOD_NS, PWM_DUTY_50_NS, PWM_POLARITY_NORMAL);
    if (ret < 0) {
        printk("Error setting PWM: %d\n", ret);
        return ret;
    }
    
    printk("PWM started: 10Hz, 50%% duty cycle (50ms ON, 50ms OFF)\n");
    current_duty_cycle = 128; // 50% = 128/255
    pwm_enabled = true;
    
    return 0;
}

int set_pwm_duty_cycle(uint8_t duty_cycle)
{
    if (!pwm_enabled || !pwm_dev) {
        printk("PWM not initialized\n");
        return -ENODEV;
    }
    
    // Convert 0-255 to duty cycle in nanoseconds
    uint64_t temp_calc = (uint64_t)PWM_PERIOD_NS * duty_cycle;
    uint32_t duty_ns = temp_calc / 255;
    
    printk("PWM calculation: period=%u, duty_cycle=%d\n", PWM_PERIOD_NS, duty_cycle);
    printk("temp_calc=%llu, duty_ns=%u\n", temp_calc, duty_ns);
    printk("Setting Hardware PWM duty cycle: %d/255 (%u ns)\n", duty_cycle, duty_ns);
    
    int ret = pwm_set(pwm_dev, pwm_channel, PWM_PERIOD_NS, duty_ns, PWM_POLARITY_NORMAL);
    if (ret < 0) {
        printk("Error setting PWM duty cycle: %d\n", ret);
        return ret;
    }
    
    current_duty_cycle = duty_cycle;
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