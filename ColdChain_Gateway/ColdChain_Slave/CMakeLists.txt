cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ColdChain_Slave)

target_include_directories(app PRIVATE include)

#target_compile_definitions(app PRIVATE DEMO_MODE)

target_sources(app PRIVATE
    src/main.c
    src/dht11_timer_isr.c
    src/ble_gatt.c
    src/led_control.c
    src/ui.c
)

