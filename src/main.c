
#include <zephyr/kernel.h>

#include <zephyr/drivers/gpio.h>
#include "toggle_led_thread.h"
#include "led_utils.h"
#include "button_interrupt_controlled_led.h"
#include "led_shell_cmd_control.h"
#include "bluetooth_service.h"
#include "pwm_controller.h"
#include <stdlib.h>
#include <string.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000
#define FAST_TOGGLE_MS  10   // 10ms = 50Hz toggle rate

// BLE data handler for PWM control
void handle_ble_data(const uint8_t *data, uint16_t len)
{
    char temp_buffer[16];
    
    // Ensure we don't overflow the buffer
    if (len >= sizeof(temp_buffer)) {
        printk("BLE data too long, truncating\n");
        len = sizeof(temp_buffer) - 1;
    }
    
    // Copy data and null terminate
    memcpy(temp_buffer, data, len);
    temp_buffer[len] = '\0';
    
    // Convert to integer (0-255 PWM value)
    int pwm_value = atoi(temp_buffer);
    
    // Clamp to valid range
    if (pwm_value < 0) pwm_value = 0;
    if (pwm_value > 255) pwm_value = 255;
    
    // Set PWM duty cycle
    int ret = set_pwm_duty_cycle((uint8_t)pwm_value);
    if (ret == 0) {
        printk("PWM set to %d via BLE\n", pwm_value);
    } else {
        printk("Failed to set PWM: %d\n", ret);
    }
}


static struct gpio_dt_spec led_4 = GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios);

void main(void)
{
    int ret;

    // Optional: configure your LEDs as before
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

    static const struct gpio_dt_spec led_3 = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);
    ret = configure_led_pin(&led_3);
    if (ret < 0) {
        printk("Failed to configure LED3\n");
    }
    init_led_shell_cmd_control(&led_3);

    // Initialize PWM controller
    ret = init_pwm_controller();
    if (ret < 0) {
        printk("Failed to initialize PWM controller\n");
        return;
    }

    // Initialize Bluetooth service with LED4 as connection status indicator
    ret = init_bluetooth_service(&led_4);
    if (ret < 0) {
        printk("Failed to initialize Bluetooth service\n");
        return;
    }
    
    // Set up BLE data callback for PWM control
    set_ble_data_callback(handle_ble_data);

    printk("PWM controller ready for BLE control on P0.04\n");
    printk("Send PWM values (0-255) via BLE to control the pin\n");
    
    // Initialize with PWM off
    set_pwm_duty_cycle(0);
    
    while (1) {
        k_msleep(SLEEP_TIME_MS);
    }
}
