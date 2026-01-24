#include <zephyr/drivers/gpio.h>
#include <stdbool.h>
#include "dht11_timer_isr.h"
#include "led_control.h"

#define FAN_LED     18
#define HEATER_LED  19
#define ALERT_LED   21

static const struct device *gpio = DEVICE_DT_GET(DT_NODELABEL(gpio0));

bool fan_on = false;
bool heater_on = false;
bool alert_on = false;
bool ble_tx_done = false;


void led_control_update(void)
{
    gpio_pin_configure(gpio, FAN_LED, GPIO_OUTPUT);
    gpio_pin_configure(gpio, HEATER_LED, GPIO_OUTPUT);
    gpio_pin_configure(gpio, ALERT_LED, GPIO_OUTPUT);

    /* Sensor / system failure */
    if (g_sensor_error) {
        gpio_pin_set(gpio, ALERT_LED, 1);

        fan_on = false;
        heater_on = false;
        alert_on = true;

        return;
    }

    /* Normal operation */
    gpio_pin_set(gpio, ALERT_LED, 0);
    alert_on = false;

    if (g_temperature > 8.0) {
        gpio_pin_set(gpio, FAN_LED, 1);
        gpio_pin_set(gpio, HEATER_LED, 0);

        fan_on = true;
        heater_on = false;

    } else if (g_temperature < 2.0) {
        gpio_pin_set(gpio, HEATER_LED, 1);
        gpio_pin_set(gpio, FAN_LED, 0);

        heater_on = true;
        fan_on = false;

    } else {
        gpio_pin_set(gpio, FAN_LED, 0);
        gpio_pin_set(gpio, HEATER_LED, 0);

        fan_on = false;
        heater_on = false;
    }
}

