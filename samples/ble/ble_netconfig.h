#ifndef __BLE_CONFIG_H_
#define __BLE_CONFIG_H_

#define BLE_TIMEOUT         500         /* ble timeout */
#define BLE_RESPONSE_LEN    1024
#define AAAA_CHAR_DATA_LEN  128
typedef int (*rt_ble_netconfig_result_cb)(char *ssid, char *password, char*ble_get_openid,void *user_data, void *userdata_len);

enum
{
	AAAAS_IDX_SVC,	 
	AAAAS_IDX_BBB0_VAL_CHAR,
	AAAAS_IDX_BBB0_VAL_VALUE,
	AAAAS_IDX_BBB1_VAL_CHAR,
	AAAAS_IDX_BBB1_VAL_VALUE,
	AAAAS_IDX_BBB1_VAL_NTF_CFG,
	AAAAS_IDX_NB,
};


enum ble_status
{
    START = 0,
    RECVING,
};

enum ble_netconfig_state
{
	BLE_NETCONFIG_STOP=0,
    BLE_NETCONFIG_START,
    BLE_NETCONFIG_RECIVE_INFO,
    
};


struct ble_session
{
    uint16_t len;
    char* response_buf;
    rt_tick_t tick;
    rt_ble_netconfig_result_cb result_cb;
    rt_uint8_t status;
};

int bk_ble_netconfig_start(void);
int bk_ble_netconfig_stop(void);
void ble_send_wifi_connected_to_master(void);
int get_ble_netconfig_state(void);
void ble_send_wifi_connected_to_master();

#endif
