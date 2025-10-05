
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
static void handle_ble_data(const uint8_t *data, uint16_t len)
{
    // Convert received data to PWM value (assuming single byte 0-255)
    if (len == 0) {
        return;
    }
    
    // Use first byte as PWM duty cycle (0-255)
    uint8_t pwm_value = data[0];
    
    printk("BLE data received: %d bytes, PWM value: %d\n", len, pwm_value);
    
    // Set hardware PWM duty cycle
    int ret = set_pwm_duty_cycle((uint8_t)pwm_value);
    if (ret == 0) {
        printk("Hardware PWM set to %d via BLE\n", pwm_value);
    } else {
        printk("Failed to set Hardware PWM: %d\n", ret);
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

    // Initialize Hardware PWM controller
    ret = init_pwm_controller();
    if (ret < 0) {
        printk("Failed to initialize Hardware PWM controller\n");
        return;
    }

    // Initialize Bluetooth service with LED4 as connection status indicator
    ret = init_bluetooth_service(&led_4);
    if (ret < 0) {
        printk("Failed to initialize Bluetooth service\n");
        return;
    }
    
    // Set up BLE data callback for Hardware PWM control
    set_ble_data_callback(handle_ble_data);

    printk("Hardware PWM controller ready for BLE control on P0.28\n");
    printk("Send PWM values (0-255) via BLE to control duty cycle\n");
    
    // Start Hardware PWM with 25% duty cycle
    set_pwm_duty_cycle(64);
    
    while (1) {
        k_msleep(SLEEP_TIME_MS);
    }
}
