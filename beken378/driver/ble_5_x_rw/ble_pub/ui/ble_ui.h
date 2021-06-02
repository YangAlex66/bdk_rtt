#ifndef _BLE_UI_H_
#define _BLE_UI_H_

#include "rwip_config.h"             // SW configuration
#include <stdint.h>          // Standard Integer Definition
#include "ble_api_5_x.h"
#if (BLE_APP_PRESENT && (BLE_CENTRAL))
#if  (BLE_SDP_CLIENT)
#include "app_sdp.h"
#endif
#include "common_bt_defines.h"
#endif
extern ble_notice_cb_t ble_event_notice;

///////////////////////
extern uint8_t ble_appm_get_dev_name(uint8_t* name, uint32_t buf_len);
extern uint8_t ble_appm_set_dev_name(uint8_t len, uint8_t* name);

#if (BLE_APP_PRESENT && (BLE_CENTRAL))
extern ble_err_t bk_ble_create_init(uint8_t con_idx,
						unsigned short con_interval,
						unsigned short con_latency,
						unsigned short sup_to,
						ble_cmd_cb_t callback);
extern ble_err_t bk_ble_init_start_conn(uint8_t con_idx,ble_cmd_cb_t callback);
extern ble_err_t bk_ble_init_stop_conn(uint8_t con_idx,ble_cmd_cb_t callback);
extern ble_err_t bk_ble_init_set_connect_dev_addr(unsigned char connidx,struct bd_addr *bdaddr,unsigned char addr_type);

extern ble_err_t bk_ble_init_set_conn_dev_timeout(unsigned char con_idx,unsigned int n_10mS);
#if (BLE_APP_PRESENT && (BLE_CENTRAL) && (BLE_SDP_CLIENT))
extern void bk_ble_sdp_register_filt_service_tab(unsigned char service_tab_nb,app_sdp_service_uuid *service_tab);
extern ble_err_t bk_ble_read_service_data_by_handle_req(uint8_t conidx,uint16_t handle,ble_cmd_cb_t callback);
extern ble_err_t bk_ble_write_service_data_req(uint8_t conidx,uint16_t handle,uint16_t data_len,uint8_t *data,ble_cmd_cb_t callback);
#endif
#endif  ///#if (BLE_APP_PRESENT && (BLE_CENTRAL))

#endif
