#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/atomic.h>

#define DHT_GPIO 4

float g_temperature;
float g_humidity;
bool g_sensor_error;

static atomic_t lock;
static struct k_timer dht_timer;

static const struct device *gpio = DEVICE_DT_GET(DT_NODELABEL(gpio0));

/* ================= DEMO MODE LOGIC ================= */
#ifdef DEMO_MODE
static float demo_temp = 1.0;
static int direction = 1;

static void demo_temperature_update(void)
{
    demo_temp += direction * 0.5;

    if (demo_temp >= 10.0) direction = -1;
    if (demo_temp <= -1.0) direction = 1;

    g_temperature = demo_temp;
    g_humidity = 60.0;
    g_sensor_error = false;
}
#endif
/* =================================================== */

static void dht_timer_isr(struct k_timer *timer)
{
    atomic_set(&lock, 1);

    if (!device_is_ready(gpio)) {
        g_sensor_error = true;
        atomic_clear(&lock);
        return;
    }

#ifdef DEMO_MODE
    /* DEMO MODE: simulated temperature ramp */
    demo_temperature_update();
#else
    /* REAL SENSOR LOGIC (mock until DHT arrives) */
    g_temperature = 5.0;
    g_humidity = 60.0;
    g_sensor_error = false;
#endif

    atomic_clear(&lock);
}

void dht11_timer_init(void)
{
    gpio_pin_configure(gpio, DHT_GPIO, GPIO_INPUT);
    k_timer_init(&dht_timer, dht_timer_isr, NULL);
    k_timer_start(&dht_timer, K_SECONDS(5), K_SECONDS(5));
}

