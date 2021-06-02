#ifndef _BLE_PUB_H_
#define _BLE_PUB_H_

#include "rtos_pub.h"

#define BLE_DEV_NAME		"ble"

#define BLE_CMD_MAGIC              (0xe2a0000)

enum
{
    CMD_BLE_REG_INIT = BLE_CMD_MAGIC + 1,
	CMD_BLE_REG_DEINIT,
	CMD_BLE_SET_CHANNEL,
	CMD_BLE_AUTO_CHANNEL_ENABLE,
	CMD_BLE_AUTO_CHANNEL_DISABLE,
	CMD_BLE_AUTO_SYNCWD_ENABLE,
	CMD_BLE_AUTO_SYNCWD_DISABLE,
	CMD_BLE_SET_PN9_TRX,
	CMD_BLE_SET_GFSK_SYNCWD,
	CMD_BLE_HOLD_PN9_ESTIMATE,
	CMD_BLE_STOP_COUNTING,
	CMD_BLE_START_COUNTING,
	CMD_BLE_START_TX,
	CMD_BLE_STOP_TX,
};

enum
{
    PN9_RX = 0,
    PN9_TX
};

extern uint8_t ble_active;

void ble_init(void);
void ble_exit(void);
void ble_dut_start(void);
UINT8 ble_is_start(void);
UINT8* ble_get_mac_addr(void);
UINT8* ble_get_name(void);
uint8_t if_ble_sleep(void);
void rf_wifi_used_clr(void);
void rf_wifi_used_set(void);
UINT32 if_rf_wifi_used(void );
void ble_ps_dump(void);

#endif /* _BLE_PUB_H_ */
