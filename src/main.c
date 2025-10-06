
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
    // Convert received data to PWM value
    if (len == 0) {
        return;
    }
    
    uint16_t pwm_value;
    
    // Check if data looks like ASCII digits
    bool is_ascii = true;
    for (int i = 0; i < len; i++) {
        if (data[i] < '0' || data[i] > '9') {
            is_ascii = false;
            break;
        }
    }
    
    if (is_ascii && len > 0) {
        // Parse as ASCII number string
        char temp_str[16];
        int copy_len = (len < 15) ? len : 15;
        memcpy(temp_str, data, copy_len);
        temp_str[copy_len] = '\0';  // Null-terminate
        
        int parsed_value = atoi(temp_str);
        if (parsed_value < 0) parsed_value = 0;
        if (parsed_value > 1000) parsed_value = 1000;
        pwm_value = (uint16_t)parsed_value;
        
        printk("BLE data received: %d bytes, ASCII: '%s', parsed: %d\n", len, temp_str, parsed_value);
    } else if (len == 1) {
        // Single byte - treat as binary value (0-255)
        pwm_value = data[0];
        printk("BLE data received: 1 byte, binary value: %d\n", pwm_value);
    } else {
        // Multiple bytes, non-ASCII - use first byte
        pwm_value = data[0];
        printk("BLE data received: %d bytes, using first byte: %d\n", len, pwm_value);
    }
    
    printk("BLE data received: %d bytes, PWM value: %d\n", len, pwm_value);
    
    // Set RC servo position
    int ret = set_pwm_duty_cycle(pwm_value);
    if (ret == 0) {
        printk("RC Servo set to %d/1000 via BLE\n", pwm_value);
    } else {
        printk("Failed to set RC Servo: %d\n", ret);
    }
}


static struct gpio_dt_spec led_4 = GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios);

void main(void)
{
    int ret;

    // Optional: configure your LEDs as before
    // LED1 (P0.28) is now controlled by hardware PWM - disable GPIO toggle
    // static const struct gpio_dt_spec led_1 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
    // ret = configure_led_pin(&led_1);
    // if (ret < 0) {
    //     printk("Failed to configure LED1\n");
    // }
    // init_toggle_led_thread(&led_1);

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
