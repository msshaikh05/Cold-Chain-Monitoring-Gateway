#include <zephyr/kernel.h>
#include "dht11_timer_isr.h"
#include "led_control.h"
#include "ble_gatt.h"
#include "ui.h"


int main(void)
{
    dht11_timer_init();
    ble_init();

    while (1) {
        led_control_update();
        ble_notify_data();
        serial_ui_print();
        k_sleep(K_SECONDS(5));
    }
}

