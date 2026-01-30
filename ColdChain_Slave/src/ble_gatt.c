#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <string.h>
#include <stdbool.h>

#include "dht11_timer_isr.h"

/* UI status flag */
extern bool ble_tx_done;

static uint8_t payload[8];

BT_GATT_SERVICE_DEFINE(coldchain_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_128(
        0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,
        0xf0,0xde,0xbc,0x9a,0x78,0x56,0x34,0x12)),
    BT_GATT_CHARACTERISTIC(
        BT_UUID_DECLARE_128(
            0xab,0xcd,0xef,0x01,0x23,0x45,0x67,0x89,
            0x89,0x67,0x45,0x23,0x01,0xef,0xcd,0xab),
        BT_GATT_CHRC_NOTIFY,
        BT_GATT_PERM_NONE,
        NULL, NULL, payload),
);

void ble_notify_data(void)
{
    memcpy(payload, &g_temperature, 4);
    memcpy(payload + 4, &g_humidity, 4);

    bt_gatt_notify(NULL, &coldchain_svc.attrs[1],
                   payload, sizeof(payload));

    /* Mark BLE transmission done */
    ble_tx_done = true;
}

void ble_init(void)
{
    bt_enable(NULL);
    bt_le_adv_start(BT_LE_ADV_CONN_NAME, NULL, 0, NULL, 0);
}

