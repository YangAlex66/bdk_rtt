/**
 ****************************************************************************************
 *
 * @file app.h
 *
 * @brief Application entry point
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef _APP_BLE_H_
#define _APP_BLE_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration
#if (BLE_APP_PRESENT)

#include <stdint.h>          // Standard Integer Definition
#include <common_bt.h>           // Common BT Definitions
#include "architect.h"            // Platform Definitions
#include "gapc_task.h"       // GAPC Definitions
#include "ble_ui.h"
#include "ble_api_5_x.h"

#if (NVDS_SUPPORT)
#include "nvds.h"
#endif // (NVDS_SUPPORT)

#define BLE_APP_STATUS_MASK    0x3U
#define UNKNOW_CONN_HDL        0xFFU
#define USED_CONN_HDL          0xFEU
#define UNKNOW_ACT_IDX         0xFFU
#define BLE_APP_EXCEP_MASK     0x80U
#define BLE_APP_EXCEP_ENABLE   0x80U
#define BLE_APP_EXCEP_DISABLE  0x00U
#define BLE_APP_IDX_POS        2
#define BLE_APP_INITING_INDEX(con_idx)      ((con_idx) + BLE_ACTIVITY_MAX)
#define BLE_APP_INITING_GET_INDEX(conidx)   ((conidx) - BLE_ACTIVITY_MAX)
#define BLE_APP_INITING_CHECK_INDEX(conidx)   (((conidx) >= BLE_ACTIVITY_MAX) && ((conidx) < APP_IDX_MAX))

/*
 * DEFINES
 ****************************************************************************************
 */

#define BLE_APP_SET_ACTVS_IDX_STATE(idx, status)	app_ble_env.actvs[(idx)].actv_status = (status)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

#if (NVDS_SUPPORT)
/// List of Application NVDS TAG identifiers
enum app_nvds_tag
{
	/// BD Address
	NVDS_TAG_BD_ADDRESS                 = 0x01,
	NVDS_LEN_BD_ADDRESS                 = 6,

	/// Device Name
	NVDS_TAG_DEVICE_NAME                = 0x02,
	NVDS_LEN_DEVICE_NAME                = 62,

	/// BLE Application Advertising data
	NVDS_TAG_APP_BLE_ADV_DATA           = 0x0B,
	NVDS_LEN_APP_BLE_ADV_DATA           = 32,

	/// BLE Application Scan response data
	NVDS_TAG_APP_BLE_SCAN_RESP_DATA     = 0x0C,
	NVDS_LEN_APP_BLE_SCAN_RESP_DATA     = 32,

	/// Mouse Sample Rate
	NVDS_TAG_MOUSE_SAMPLE_RATE          = 0x38,
	NVDS_LEN_MOUSE_SAMPLE_RATE          = 1,

	/// Peripheral Bonded
	NVDS_TAG_PERIPH_BONDED              = 0x39,
	NVDS_LEN_PERIPH_BONDED              = 1,

	/// Mouse NTF Cfg
	NVDS_TAG_MOUSE_NTF_CFG              = 0x3A,
	NVDS_LEN_MOUSE_NTF_CFG              = 2,

	/// Mouse Timeout value
	NVDS_TAG_MOUSE_TIMEOUT              = 0x3B,
	NVDS_LEN_MOUSE_TIMEOUT              = 2,

	/// Peer Device BD Address
	NVDS_TAG_PEER_BD_ADDRESS            = 0x3C,
	NVDS_LEN_PEER_BD_ADDRESS            = 7,

	/// Mouse Energy Safe
	NVDS_TAG_MOUSE_ENERGY_SAFE          = 0x3D,
	NVDS_LEN_MOUSE_SAFE_ENERGY          = 2,

	/// EDIV (2bytes), RAND NB (8bytes),  LTK (16 bytes), Key Size (1 byte)
	NVDS_TAG_LTK                        = 0x3E,
	NVDS_LEN_LTK                        = 28,

	/// PAIRING
	NVDS_TAG_PAIRING                    = 0x3F,
	NVDS_LEN_PAIRING                    = 54,

	/// Audio mode 0 task
	NVDS_TAG_AM0_FIRST                  = 0x90,
	NVDS_TAG_AM0_LAST                   = 0x9F,

	/// Local device Identity resolving key
	NVDS_TAG_LOC_IRK                    = 0xA0,
	NVDS_LEN_LOC_IRK                    = KEY_LEN,

	/// Peer device Resolving identity key (+identity address)
	NVDS_TAG_PEER_IRK                   = 0xA1,
	NVDS_LEN_PEER_IRK                   = sizeof(struct gapc_irk),
};
#endif // (NVDS_SUPPORT)

typedef enum {
	BLE_OP_CREATE_ADV_POS = 0,
	BLE_OP_SET_ADV_DATA_POS,
	BLE_OP_SET_RSP_DATA_POS,
	BLE_OP_START_ADV_POS,
	BLE_OP_STOP_ADV_POS,
	BLE_OP_DEL_ADV_POS,
	BLE_OP_ADV_MAX,
}ble_adv_op;

typedef enum {
	BLE_OP_CREATE_SCAN_POS,
	BLE_OP_START_SCAN_POS,
	BLE_OP_STOP_SCAN_POS,
	BLE_OP_DEL_SCAN_POS,
	BLE_OP_SCAN_MAX,
}ble_scan_op;

typedef enum {
	BLE_OP_UPDATE_CONN_POS,
	BLE_OP_MTU_CHANGE_POS,
	BLE_OP_DIS_CONN_POS,

	BLE_OP_CONN_MAX,
}ble_conn_op;

typedef enum{
	BLE_OP_CREATE_INIT_POS = 0,
	BLE_OP_INIT_START_POS,
	BLE_OP_INIT_STOP_POS,
	BLE_OP_INIT_DIS_CONN_POS,

	BLE_OP_INIT_READ_CHAR_POS,
	BLE_OP_INIT_WRITE_CHAR_POS,
	BLE_OP_INIT_MAX,
}ble_init_op;

typedef enum {
	ACTV_IDLE,
	/////adv
	ACTV_ADV_CREATED,
	ACTV_ADV_STARTED,
	////////scan
	ACTV_SCAN_CREATED,
	ACTV_SCAN_STARTED,

	ACTV_INIT_CREATED,
	ACTV_PER_SYNC_CREATED,
} actv_state_t;

/// Initing state machine
enum app_init_state
{
	/// Iint activity does not exists
	APP_INIT_STATE_IDLE = 0,
	/// Creating Iint activity
	APP_INIT_STATE_CREATING,
	/// Iint activity created
	APP_INIT_STATE_CREATED,

	/// WAIT Start Iint activity
	APP_INIT_STATE_WAIT_CONECTTING = 3,
	/// Starting Iint activity
	APP_INIT_STATE_CONECTTING = 4,
	/// Iint activity conected
	APP_INIT_STATE_CONECTED = 5,
	/// Stopping Iint activity
	APP_INIT_STATE_STOPPING = 6,
};

enum{
	APP_BLE_MASTER_ROLE = 0,
	APP_BLE_SLAVE_ROLE = 1,
	APP_BLE_NONE_ROLE,
};

struct conn_info {
	uint32_t conn_op_mask;
	ble_cmd_cb_t conn_op_cb;
	/// Connection handle
	uint16_t conhdl;

	uint8_t gap_actv_idx;
	/// Connection interval
	uint16_t con_interval;
	/// Connection latency
	uint16_t con_latency;
	/// Link supervision timeout
	uint16_t sup_to;
	/// Clock accuracy
	uint8_t clk_accuracy;
	/// Peer address type
	uint8_t peer_addr_type;
	union{
		struct{
			/// Current init state (@see enum app_init_state)
			uint8_t init_state;
			/// Scan interval
			uint16_t scan_intv;
			/// Scan window
			uint16_t scan_wd;

			uint16_t conn_dev_to;
		}master;
	}u;
	/// Peer BT address
	struct bd_addr peer_addr;
	/// Role of device in connection (0 = Master / 1 = Slave)
	uint8_t role;
};

struct actv_info {
	/* *
	 * activity status:
	 * ACTV_IDLE
	 * ACTV_ADV_CREATED
	 * ACTV_ADV_STARTED
	 * ACTV_SCAN_CREATED
	 * ACTV_SCAN_STARTED
	 */
	uint8_t actv_status;
	uint8_t gap_advt_idx;
	union actv_param {
		struct adv_param adv;
		struct scan_param scan;
	}param;
};

typedef enum {
	APP_BLE_IDLE = 0,
	APP_BLE_READY = 1,
	APP_BLE_CMD_RUNNING = 2,
} ble_status_t;

/// Application environment structure
struct app_env_tag {
	/* *
	 *    7       6	  5    4   3    2   1	 0
	 * +---------+----+----+----+----+----+----+----+
	 * |exception|     CMD_ACTV_ID    |  STATUS |
	 * +---------+----+----+----+----+----+----+----+
	 *
	 * STATUS:(0=APP_BLE_IDLE 1=APP_BLE_READY 2=APP_BLE_CMD_RUNNING)
	 */
	uint8_t app_status;

	/// Running command
	ble_cmd_t cmd;
	uint32_t op_mask;
	ble_cmd_cb_t op_cb;
	/// Device Name length
	uint8_t dev_name_len;
	/// Device Name
	uint8_t dev_name[APP_DEVICE_NAME_MAX_LEN];
	/// Local device IRK
	uint8_t loc_irk[KEY_LEN];
	/// Counter used to generate IRK
	uint8_t rand_cnt;

	struct actv_info actvs[BLE_ACTIVITY_MAX];

	struct conn_info connections[BLE_CONNECTION_MAX];
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Structure containing information about the handlers for an application subtask
struct app_subtask_handlers
{
	/// Pointer to the message handler table
	const struct kernel_msg_handler *p_msg_handler_tab;
	/// Number of messages handled
	uint16_t msg_cnt;
};

ble_status_t app_ble_env_state_get(void);

/**
 ****************************************************************************************
 * @brief Initialize the BLE demo application.
 ****************************************************************************************
 */
void appm_init(void);

extern struct app_env_tag app_ble_env;

ble_status_t app_ble_env_state_get(void);
actv_state_t app_ble_actv_state_get(uint8_t actv_idx);
uint8_t app_ble_get_idle_conn_idx_handle(void);
uint8_t app_ble_find_conn_idx_handle(uint16_t conhdl);
uint8_t app_ble_find_actv_idx_handle(uint16_t gap_actv_idx);
uint8_t app_ble_get_connhdl(int conn_idx);
void app_ble_run(uint8_t idx, ble_cmd_t cmd, uint32_t op_mask, ble_cmd_cb_t callback);
void app_ble_reset(void);
ble_err_t app_ble_create_advertising(uint8_t actv_idx, uint8_t chnl_map, uint32_t intv_min, uint32_t intv_max);
ble_err_t app_ble_start_advertising(uint8_t actv_idx, uint16 duration);
ble_err_t app_ble_stop_advertising(uint8_t actv_idx);
ble_err_t app_ble_delete_advertising(uint8_t actv_idx);
ble_err_t app_ble_set_adv_data(uint8_t actv_idx, unsigned char* adv_buff, unsigned char adv_len);
ble_err_t app_ble_set_scan_rsp_data(uint8_t actv_idx, unsigned char* scan_buff, unsigned char scan_len);
ble_err_t app_ble_update_param(uint8_t conn_idx, struct gapc_conn_param *conn_param);
ble_err_t app_ble_disconnect(uint8_t conn_idx, uint8_t reason);
ble_err_t app_ble_gatt_mtu_change(uint8_t conn_idx);
ble_err_t app_ble_create_scaning(uint8_t actv_idx);
ble_err_t app_ble_start_scaning(uint8_t actv_idx, uint16_t scan_intv, uint16_t scan_wd);
ble_err_t app_ble_stop_scaning(uint8_t actv_idx);
ble_err_t app_ble_delete_scaning(uint8_t actv_idx);
void app_ble_next_operation(uint8_t idx, uint8_t status);

#define BLE_APP_MASTER_GET_CONN_IDX_OP_MASK(conn_idx)   app_ble_env.connections[(conn_idx)].conn_op_mask
#define BLE_APP_MASTER_CLEAR_IDX_OP_MASK_BITS(conn_idx,bit_ops)  app_ble_env.connections[(conn_idx)].conn_op_mask &= (~(1 << bit_ops))

#define BLE_APP_MASTER_GET_IDX_STATE(con_idx)    app_ble_env.connections[(con_idx)].u.master.init_state
#define BLE_APP_MASTER_SET_IDX_STATE(con_idx,new_state)    app_ble_env.connections[(con_idx)].u.master.init_state  = (new_state)
#define BLE_APP_MASTER_SET_IDX_CMD_SET_STATE(bit_ops,con_idx,callback)     \
												app_ble_env.connections[con_idx].conn_op_mask = 1 << bit_ops;\
												app_ble_env.connections[con_idx].conn_op_cb = callback;\
												app_ble_env.app_status = ((con_idx) << 2) | APP_BLE_CMD_RUNNING

#define BLE_APP_MASTER_CHECK_IDX_HANDLE(cmd_type_save,actv_idx,op_pos,cmd_type)     if(BLE_APP_MASTER_GET_CONN_IDX_OP_MASK(actv_idx)&(1<<op_pos)){\
																						cmd_type_save = cmd_type;\
																						BLE_APP_MASTER_CLEAR_IDX_OP_MASK_BITS(actv_idx,op_pos);\
																					}

#define BLE_APP_MASTER_SET_IDX_CALLBACK_HANDLE(conn_idx,cmd,param)    \
													do{\
														if(BLE_APP_MASTER_GET_CONN_IDX_OP_MASK(conn_idx) == 0){\
															ble_cmd_cb_t conn_op_cb;\
															conn_op_cb = app_ble_env.connections[(conn_idx)].conn_op_cb;\
															app_ble_env.connections[(conn_idx)].conn_op_cb = NULL;\
															\
															if(conn_op_cb)\
																conn_op_cb(cmd,param);	 \
														}else{\
															\
														}\
													}while(0)
#endif //(BLE_APP_PRESENT)

#endif // APP_H_
