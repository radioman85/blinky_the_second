cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(blinky_the_second)

target_sources(app PRIVATE
    src/main.c
    src/toggle_led_thread.c
    src/button_interrupt_controlled_led.c
    src/led_utils.c
    src/led_shell_cmd_control.c
    src/bluetooth_service.c
    src/pwm_controller.c
)
