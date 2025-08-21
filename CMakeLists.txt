cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(blinky_the_second)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/toggle_led_thread.c)
target_sources(app PRIVATE src/button_interrupt_controlled_led.c)
target_sources(app PRIVATE src/led_utils.c)
