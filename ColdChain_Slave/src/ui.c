#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdbool.h>
#include "ui.h"

/* Extern globals */
extern float g_temperature;
extern float g_humidity;
extern bool g_sensor_error;
extern bool fan_on;
extern bool heater_on;
extern bool alert_on;
extern bool ble_tx_done;

#define WAREHOUSE_ID  "WH-PUNE-01"
#define BOX_ID        "BOX-CC-007"

void serial_ui_print(void)
{
    printk("\n");
    printk("=========================================\n");
    printk("   ❄️  COLD CHAIN MONITORING GATEWAY ❄️\n");
    printk("=========================================\n");
    printk(" Warehouse ID  : %s\n", WAREHOUSE_ID);
    printk(" Box ID        : %s\n", BOX_ID);
    printk("-----------------------------------------\n");
    printk(" Temperature   : %d.%02d °C\n",
       (int)g_temperature,
       (int)((g_temperature - (int)g_temperature) * 100));

    printk(" Humidity      : %d.%02d %%\n",
       (int)g_humidity,
       (int)((g_humidity - (int)g_humidity) * 100));

    printk("-----------------------------------------\n");
    printk(" Fan Status    : %s\n", fan_on ? "ON" : "OFF");
    printk(" Heater Status : %s\n", heater_on ? "ON" : "OFF");
    printk(" Alert Status  : %s\n", alert_on ? "ON" : "OFF");
    printk("-----------------------------------------\n");
    printk(" BLE Adv       : ACTIVE\n");
    printk(" BLE TX        : %s\n", ble_tx_done ? "DATA SENT" : "IDLE");
    printk(" Sensor Health : %s\n", g_sensor_error ? "FAULT ❌" : "OK ✅");
    printk("=========================================\n");
}

