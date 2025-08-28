
#include <zephyr/kernel.h>

#include <zephyr/drivers/gpio.h>
#include "toggle_led_thread.h"
#include "led_utils.h"
#include "button_interrupt_controlled_led.h"
#include "led_shell_cmd_control.h"


#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>

#include <bluetooth/services/nus.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000


static struct gpio_dt_spec led_4 = GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios);

static void connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    if (err) {
        printk("Connection failed: %s, err %u\n", addr, err);
    } else {
        printk("Connected: %s\n", addr);
        gpio_pin_set_dt(&led_4, 1); // Turn on LED4
    }
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printk("Disconnected: %s, reason %u\n", addr, reason);
    gpio_pin_set_dt(&led_4, 0); // Turn off LED4
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
};

static void bt_receive_cb(struct bt_conn *conn, const uint8_t *const data, uint16_t len)
{
    printk("Received data over BLE: %.*s\n", len, data);
}

static struct bt_nus_cb nus_cb = {
    .received = bt_receive_cb,
};

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

    ret = bt_enable(NULL);
    if (ret) {
        printk("Bluetooth init failed (err %d)\n", ret);
        return;
    }
    printk("Bluetooth initialized\n");

    ret = bt_nus_init(&nus_cb);
    if (ret) {
        printk("NUS init failed (err %d)\n", ret);
        return;
    }
    printk("NUS service initialized\n");

    // Start advertising
    ret = bt_le_adv_start(BT_LE_ADV_CONN_NAME, NULL, 0, NULL, 0);
    if (ret) {
        printk("Advertising failed to start (err %d)\n", ret);
        return;
    }
    printk("Advertising successfully started\n");

    // Configure LED4
    ret = configure_led_pin(&led_4);
    if (ret < 0) {
        printk("Failed to configure LED4\n");
    }

    while (1) {
        k_msleep(SLEEP_TIME_MS);
    }
}
