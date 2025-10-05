#include "bluetooth_service.h"
#include "led_utils.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <bluetooth/services/nus.h>

// Static variables for Bluetooth service
static const struct gpio_dt_spec *status_led = NULL;

static void connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    if (err) {
        printk("Connection failed: %s, err %u\n", addr, err);
    } else {
        printk("Connected: %s\n", addr);
        if (status_led) {
            gpio_pin_set_dt(status_led, 1); // Turn on LED4
        }
    }
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printk("Disconnected: %s, reason %u\n", addr, reason);
    if (status_led) {
        gpio_pin_set_dt(status_led, 0); // Turn off LED4
    }
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

int init_bluetooth_service(const struct gpio_dt_spec *led)
{
    int ret;
    
    // Store the status LED reference
    status_led = led;
    
    // Configure the status LED and set initial state to OFF (not connected)
    if (status_led) {
        ret = configure_led_pin(status_led);
        if (ret < 0) {
            printk("Failed to configure Bluetooth status LED\n");
            return ret;
        }
        // Ensure LED starts in OFF state (not connected)
        gpio_pin_set_dt(status_led, 0);
    }
    
    // Enable Bluetooth
    ret = bt_enable(NULL);
    if (ret) {
        printk("Bluetooth init failed (err %d)\n", ret);
        return ret;
    }
    printk("Bluetooth initialized\n");

    // Initialize Nordic UART Service
    ret = bt_nus_init(&nus_cb);
    if (ret) {
        printk("NUS init failed (err %d)\n", ret);
        return ret;
    }
    printk("NUS service initialized\n");

    // Start advertising
    ret = bt_le_adv_start(BT_LE_ADV_CONN_NAME, NULL, 0, NULL, 0);
    if (ret) {
        printk("Advertising failed to start (err %d)\n", ret);
        return ret;
    }
    printk("Advertising successfully started\n");
    
    return 0;
}