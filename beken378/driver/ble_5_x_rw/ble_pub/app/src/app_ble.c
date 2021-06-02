/**
 ****************************************************************************************
 *
 * @file app.c
 *
 * @brief Application entry point
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "user_config.h"  

#include "rwip_config.h"             // SW configuration

#if (BLE_APP_PRESENT)
#include "rwapp_config.h"
#include <string.h>

#include "rwip.h"
#include "app_task.h"                // Application task Definition
#include "app_ble.h"                     // Application Definition
#include "gap.h"                     // GAP Definition
#include "gapm_task.h"               // GAP Manager Task API
#include "gapc_task.h"               // GAP Controller Task API
#include "gattc_task.h"

#include "common_bt.h"                   // Common BT Definition
#include "common_math.h"                 // Common Maths Definition
#if ((BLE_APP_PRESENT) && (BLE_CENTRAL))
#include "app_ble_init.h"
#if BLE_SDP_CLIENT
#include "app_sdp.h"
#include "sdp_comm.h"
#include "sdp_comm_pub.h"
#include "sdp_comm_task.h"
#endif
#endif

#if (NVDS_SUPPORT)
#include "nvds.h"                    // NVDS Definitions
#endif //(NVDS_SUPPORT)
/*
 * DEFINES
 ****************************************************************************************
 */

#define DEVICE_NAME        "BK DEVICE"

#define DEVICE_NAME_SIZE    sizeof(DEVICE_NAME)

/*
 * LOCAL VARIABLES DEFINITIONS
 ****************************************************************************************
 */

/// Application Task Descriptor
extern const struct kernel_task_desc TASK_BLE_DESC_APP;

/// Application Environment Structure
struct app_env_tag app_ble_env;

static uint32_t ble_sleep_enable = 1;

#define BLE_APP_CHECK_ACTVS_IDX(actv_idx) \
	if (actv_idx >= BLE_ACTIVITY_MAX) {\
		bk_printf("[%s]unknow actv_idx:%d\r\n",__FUNCTION__,actv_idx);\
		return ERR_UNKNOW_IDX;\
	}

#define BLE_APP_CHECK_CONN_IDX(conn_idx) \
	if (conn_idx >= BLE_CONNECTION_MAX) {\
		bk_printf("[%s]unknow conn_idx:%d\r\n",__FUNCTION__,conn_idx);\
		return ERR_UNKNOW_IDX;\
	}

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

ble_status_t app_ble_env_state_get(void)
{
	return app_ble_env.app_status & BLE_APP_STATUS_MASK;
}

actv_state_t app_ble_actv_state_get(uint8_t actv_idx)
{
	return app_ble_env.actvs[actv_idx].actv_status;
}

uint8_t app_ble_get_idle_actv_idx_handle(void)
{
	uint8_t index;

	for (index = 0; index < BLE_ACTIVITY_MAX; index++) {
		if (app_ble_env.actvs[index].actv_status == ACTV_IDLE) {
			break;
		}
	}

	if (index == BLE_ACTIVITY_MAX) {
		bk_printf("Don't have free actv\r\n");
		return UNKNOW_ACT_IDX;
	}

	return index;
}

uint8_t app_ble_get_idle_conn_idx_handle(void)
{
	uint8_t conn_idx;

	for (conn_idx = 0; conn_idx < BLE_CONNECTION_MAX; conn_idx++) {
		if (app_ble_env.connections[conn_idx].conhdl == UNKNOW_CONN_HDL) {
			break;
		}
	}

	if(conn_idx >= BLE_CONNECTION_MAX)
		return UNKNOW_CONN_HDL;

	return conn_idx;
}

uint8_t app_ble_find_conn_idx_handle(uint16_t conhdl)
{
	uint8_t conn_idx;

	for (conn_idx = 0; conn_idx < BLE_CONNECTION_MAX; conn_idx++) {
		if (app_ble_env.connections[conn_idx].conhdl == conhdl) {
			break;
		}
	}

	return conn_idx;
}

uint8_t app_ble_find_actv_idx_handle(uint16_t gap_actv_idx)
{
	uint8_t actv_idx;

	for (actv_idx = 0; actv_idx < BLE_ACTIVITY_MAX; actv_idx++) {
		if (app_ble_env.actvs[actv_idx].gap_advt_idx == gap_actv_idx) {
			break;
		}
	}

	return actv_idx;
}


uint8_t app_ble_get_connhdl(int conn_idx)
{
	return (app_ble_env.connections[conn_idx].conhdl);
}

void app_ble_run(uint8_t idx, ble_cmd_t cmd, uint32_t op_mask, ble_cmd_cb_t callback)
{
	app_ble_env.op_mask = op_mask;
	app_ble_env.op_cb = callback;
	app_ble_env.cmd = cmd;
	app_ble_env.app_status = (idx << BLE_APP_IDX_POS) | APP_BLE_CMD_RUNNING;
}

void app_ble_reset(void)
{
	app_ble_env.op_mask = 0;
	app_ble_env.op_cb = NULL;
	app_ble_env.app_status = APP_BLE_READY;
}

ble_err_t app_ble_create_advertising(uint8_t actv_idx,
						uint8_t chnl_map,
						uint32_t intv_min,
						uint32_t intv_max)
{
	ble_err_t ret = ERR_SUCCESS;

	BLE_APP_CHECK_ACTVS_IDX(actv_idx);

	if (app_ble_actv_state_get(actv_idx) == ACTV_IDLE) {
		struct gapm_activity_create_adv_cmd *p_cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD,
										TASK_BLE_GAPM, TASK_BLE_APP,
										gapm_activity_create_adv_cmd);

		if (p_cmd) {
			/// Set operation code
			p_cmd->operation = GAPM_CREATE_ADV_ACTIVITY;

			// Fill the allocated kernel message
			p_cmd->own_addr_type = GAPM_STATIC_ADDR;
			p_cmd->adv_param.type = GAPM_ADV_TYPE_LEGACY;//GAPM_ADV_TYPE_EXTENDED;//;
			p_cmd->adv_param.prop = GAPM_ADV_PROP_UNDIR_CONN_MASK ;//| GAPM_ADV_PROP_SCAN_REQ_NTF_EN_BIT;//GAPM_ADV_PROP_BROADCAST_NON_SCAN_MASK;//GAPM_ADV_PROP_UNDIR_CONN_MASK;
			p_cmd->adv_param.filter_pol = ADV_ALLOW_SCAN_ANY_CON_ANY;
			p_cmd->adv_param.prim_cfg.chnl_map = chnl_map;
			p_cmd->adv_param.prim_cfg.phy = GAP_PHY_LE_1MBPS;

			p_cmd->adv_param.disc_mode = GAPM_ADV_MODE_GEN_DISC;
			p_cmd->adv_param.prim_cfg.adv_intv_min = intv_min;
			p_cmd->adv_param.prim_cfg.adv_intv_max = intv_max;

			// Send the message
			kernel_msg_send(p_cmd);

			ret = ERR_SUCCESS;
		} else {
			ret = ERR_NO_MEM;
		}
	} else {
		bk_printf("actv[%d] is not idle\r\n", actv_idx);
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t app_ble_start_advertising(uint8_t actv_idx, uint16 duration)
{
	ble_err_t ret = ERR_SUCCESS;

	BLE_APP_CHECK_ACTVS_IDX(actv_idx);

	if (app_ble_actv_state_get(actv_idx) == ACTV_ADV_CREATED) {
		// Prepare the GAPM_ACTIVITY_START_CMD message
		struct gapm_activity_start_cmd *p_cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_START_CMD,
									TASK_BLE_GAPM, TASK_BLE_APP,
									gapm_activity_start_cmd);

		if (p_cmd) {
			p_cmd->operation = GAPM_START_ACTIVITY;
			p_cmd->actv_idx = app_ble_env.actvs[actv_idx].gap_advt_idx;
			p_cmd->u_param.adv_add_param.duration = duration;
			p_cmd->u_param.adv_add_param.max_adv_evt = 0;

			// Send the message
			kernel_msg_send(p_cmd);

			ret = ERR_SUCCESS;
		} else {
			ret = ERR_NO_MEM;
		}
	} else {
		bk_printf("actv[%d] is not created\r\n", actv_idx);
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t app_ble_stop_advertising(uint8_t actv_idx)
{
	ble_err_t ret = ERR_SUCCESS;

	BLE_APP_CHECK_ACTVS_IDX(actv_idx);

	if (app_ble_actv_state_get(actv_idx) == ACTV_ADV_STARTED) {
		struct gapm_activity_stop_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_STOP_CMD,
									TASK_BLE_GAPM, TASK_BLE_APP,
									gapm_activity_stop_cmd);

		if (cmd) {
			// Fill the allocated kernel message
			cmd->operation = GAPM_STOP_ACTIVITY;
			cmd->actv_idx = app_ble_env.actvs[actv_idx].gap_advt_idx;

			// Send the message
			kernel_msg_send(cmd);

			ret = ERR_SUCCESS;
		} else {
			ret = ERR_NO_MEM;
		}
	} else {
		bk_printf("actv[%d] is not started\r\n", actv_idx);
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t app_ble_delete_advertising(uint8_t actv_idx)
{
	ble_err_t ret = ERR_SUCCESS;

	BLE_APP_CHECK_ACTVS_IDX(actv_idx);

	if (app_ble_actv_state_get(actv_idx) == ACTV_ADV_CREATED) {
		// Prepare the GAPM_ACTIVITY_CREATE_CMD message
		struct gapm_activity_delete_cmd *p_cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_DELETE_CMD,
									TASK_BLE_GAPM, TASK_BLE_APP,
									gapm_activity_delete_cmd);

		if (p_cmd) {
			// Set operation code
			p_cmd->operation = GAPM_DELETE_ACTIVITY;
			p_cmd->actv_idx = app_ble_env.actvs[actv_idx].gap_advt_idx;

			// Send the message
			kernel_msg_send(p_cmd);

			ret = ERR_SUCCESS;
		} else {
			ret = ERR_NO_MEM;
		}
	} else {
		bk_printf("actv[%d] is not created\r\n", actv_idx);
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t app_ble_set_adv_data(uint8_t actv_idx, unsigned char* adv_buff, unsigned char adv_len)
{
	ble_err_t ret = ERR_SUCCESS;

	BLE_APP_CHECK_ACTVS_IDX(actv_idx);

	if ((app_ble_actv_state_get(actv_idx) == ACTV_ADV_CREATED)
		|| (app_ble_actv_state_get(actv_idx) == ACTV_ADV_STARTED)) {
		// Prepare the GAPM_SET_ADV_DATA_CMD message
		struct gapm_set_adv_data_cmd *p_cmd = KERNEL_MSG_ALLOC_DYN(GAPM_SET_ADV_DATA_CMD,
										TASK_BLE_GAPM, TASK_BLE_APP,
										gapm_set_adv_data_cmd,
										ADV_DATA_LEN);

		if (p_cmd) {
			// Fill the allocated kernel message
			p_cmd->operation = GAPM_SET_ADV_DATA;
			p_cmd->actv_idx = app_ble_env.actvs[actv_idx].gap_advt_idx;

			memcpy(&p_cmd->data[0], adv_buff, adv_len);
			// Update advertising data length
			p_cmd->length = adv_len;

			// Send the message
			kernel_msg_send(p_cmd);

			ret = ERR_SUCCESS;
		} else {
			ret = ERR_NO_MEM;
		}
	} else {
		bk_printf("actv[%d] is not created\r\n", actv_idx);
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t app_ble_set_scan_rsp_data(uint8_t actv_idx, unsigned char* scan_buff, unsigned char scan_len)
{
	ble_err_t ret = ERR_SUCCESS;

	BLE_APP_CHECK_ACTVS_IDX(actv_idx);

	if ((app_ble_actv_state_get(actv_idx) == ACTV_ADV_CREATED)
		|| (app_ble_actv_state_get(actv_idx) == ACTV_ADV_STARTED)) {
		// Prepare the GAPM_SET_ADV_DATA_CMD message
		struct gapm_set_adv_data_cmd *p_cmd = KERNEL_MSG_ALLOC_DYN(GAPM_SET_ADV_DATA_CMD,
										TASK_BLE_GAPM, TASK_BLE_APP,
										gapm_set_adv_data_cmd,
										ADV_DATA_LEN);

		if (p_cmd) {
			// Fill the allocated kernel message
			p_cmd->operation = GAPM_SET_SCAN_RSP_DATA;
			p_cmd->actv_idx = app_ble_env.actvs[actv_idx].gap_advt_idx;

			p_cmd->length = scan_len;
			memcpy(&p_cmd->data[0], scan_buff, scan_len);

			// Send the message
			kernel_msg_send(p_cmd);

			ret = ERR_SUCCESS;
		} else {
			ret = ERR_NO_MEM;
		}
	} else {
		bk_printf("actv[%d] is not created\r\n", actv_idx);
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t app_ble_update_param(uint8_t conn_idx, struct gapc_conn_param *conn_param)
{
	ble_err_t ret = ERR_SUCCESS;

	BLE_APP_CHECK_CONN_IDX(conn_idx);

	if (app_ble_get_connhdl(conn_idx) != UNKNOW_CONN_HDL) {
		// Prepare the GAPC_PARAM_UPDATE_CMD message
		struct gapc_param_update_cmd *cmd = KERNEL_MSG_ALLOC(GAPC_PARAM_UPDATE_CMD,
									 KERNEL_BUILD_ID(TASK_BLE_GAPC, app_ble_get_connhdl(conn_idx)),
									 KERNEL_BUILD_ID(TASK_BLE_APP, app_ble_get_connhdl(conn_idx)),
									 gapc_param_update_cmd);

		if (cmd) {
			cmd->operation	= GAPC_UPDATE_PARAMS;
			cmd->intv_min	= conn_param->intv_min;
			cmd->intv_max	= conn_param->intv_max;
			cmd->latency	= conn_param->latency;
			cmd->time_out	= conn_param->time_out;

			// not used by a slave device
			cmd->ce_len_min = 0x10;  ///0xFFFF
			cmd->ce_len_max = 0x20;  ///0xFFFF

			// Send the message
			kernel_msg_send(cmd);

			ret = ERR_SUCCESS;
		} else {
			ret = ERR_NO_MEM;
		}
	} else {
		bk_printf("unknow conhdl");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t app_ble_disconnect(uint8_t conn_idx, uint8_t reason)
{
	ble_err_t ret = ERR_SUCCESS;

	BLE_APP_CHECK_CONN_IDX(conn_idx);

	if (app_ble_get_connhdl(conn_idx) != UNKNOW_CONN_HDL) {
		struct gapc_disconnect_cmd *cmd = KERNEL_MSG_ALLOC(GAPC_DISCONNECT_CMD,
									KERNEL_BUILD_ID(TASK_BLE_GAPC, conn_idx),
									KERNEL_BUILD_ID(TASK_BLE_APP, conn_idx),
									gapc_disconnect_cmd);

		if (cmd) {
			cmd->operation = GAPC_DISCONNECT;
			cmd->reason    = reason;

			// Send the message
			kernel_msg_send(cmd);

			ret = ERR_SUCCESS;
		} else {
			ret = ERR_NO_MEM;
		}
	} else {
		bk_printf("unknow conhdl");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t app_ble_gatt_mtu_change(uint8_t conn_idx)
{
	ble_err_t ret = ERR_SUCCESS;

	BLE_APP_CHECK_CONN_IDX(conn_idx);

	if (app_ble_get_connhdl(conn_idx) != UNKNOW_CONN_HDL) {
		// Prepare the GAPC_PARAM_UPDATE_CMD message
		struct gattc_exc_mtu_cmd  *cmd = KERNEL_MSG_ALLOC(GATTC_EXC_MTU_CMD,
								KERNEL_BUILD_ID(TASK_BLE_GATTC, conn_idx),
								KERNEL_BUILD_ID(TASK_BLE_APP, conn_idx),
								gattc_exc_mtu_cmd);

		if (cmd) {
			cmd->operation  = GATTC_MTU_EXCH;
			cmd->seq_num = 0;

			// Send the message
			kernel_msg_send(cmd);

			ret = ERR_SUCCESS;
		} else {
			ret = ERR_NO_MEM;
		}
	} else {
		bk_printf("unknow conhdl");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t app_ble_create_scaning(uint8_t actv_idx)
{
	ble_err_t ret = ERR_SUCCESS;

	BLE_APP_CHECK_ACTVS_IDX(actv_idx);

	if (app_ble_actv_state_get(actv_idx) == ACTV_IDLE) {
		// Prepare the GAPM_ACTIVITY_CREATE_CMD message
		struct gapm_activity_create_cmd *p_cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD,
									TASK_BLE_GAPM, TASK_BLE_APP,
									gapm_activity_create_cmd);

		if (p_cmd) {
			// Set operation code
			p_cmd->operation = GAPM_CREATE_SCAN_ACTIVITY;

			// Fill the allocated kernel message
			p_cmd->own_addr_type = GAPM_STATIC_ADDR;

			// Send the message
			kernel_msg_send(p_cmd);

			ret = ERR_SUCCESS;
		} else {
			ret = ERR_NO_MEM;
		}
	} else {
		bk_printf("actv[%d] is not idle\r\n", actv_idx);
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t app_ble_start_scaning(uint8_t actv_idx, uint16_t scan_intv, uint16_t scan_wd)
{
	ble_err_t ret = ERR_SUCCESS;

	BLE_APP_CHECK_ACTVS_IDX(actv_idx);

	if (app_ble_actv_state_get(actv_idx) == ACTV_SCAN_CREATED) {
		// Prepare the GAPM_ACTIVITY_START_CMD message
		struct gapm_activity_start_cmd *p_cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_START_CMD,
									TASK_BLE_GAPM, TASK_BLE_APP,
									gapm_activity_start_cmd);

		if (p_cmd) {
			p_cmd->operation = GAPM_START_ACTIVITY;

			p_cmd->u_param.scan_param.type = GAPM_SCAN_TYPE_OBSERVER;//GAPM_SCAN_TYPE_GEN_DISC;//GAPM_SCAN_TYPE_OBSERVER;//;
			p_cmd->u_param.scan_param.prop = GAPM_SCAN_PROP_PHY_1M_BIT ;//| GAPM_SCAN_PROP_ACTIVE_1M_BIT;
			p_cmd->u_param.scan_param.scan_param_1m.scan_intv = scan_intv;
			p_cmd->u_param.scan_param.scan_param_1m.scan_wd = scan_wd;
			p_cmd->u_param.scan_param.dup_filt_pol = 0;
			p_cmd->u_param.scan_param.duration = 0;
			p_cmd->u_param.scan_param.period = 10;

			p_cmd->actv_idx = app_ble_env.actvs[actv_idx].gap_advt_idx;

			// Send the message
			kernel_msg_send(p_cmd);

			ret = ERR_SUCCESS;
		} else {
			ret = ERR_NO_MEM;
		}
	} else {
		bk_printf("actv[%d] is not created\r\n", actv_idx);
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t app_ble_stop_scaning(uint8_t actv_idx)
{
	ble_err_t ret = ERR_SUCCESS;

	BLE_APP_CHECK_ACTVS_IDX(actv_idx);

	if (app_ble_actv_state_get(actv_idx) == ACTV_SCAN_STARTED) {
		// Prepare the GAPM_ACTIVITY_STOP_CMD message
		struct gapm_activity_stop_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_STOP_CMD,
									TASK_BLE_GAPM, TASK_BLE_APP,
									gapm_activity_stop_cmd);

		if (cmd) {
			// Fill the allocated kernel message
			cmd->operation = GAPM_STOP_ACTIVITY;
			cmd->actv_idx = app_ble_env.actvs[actv_idx].gap_advt_idx;

			// Send the message
			kernel_msg_send(cmd);

			ret = ERR_SUCCESS;
		} else {
			ret = ERR_NO_MEM;
		}
	} else {
		bk_printf("actv[%d] is not started\r\n", actv_idx);
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t app_ble_delete_scaning(uint8_t actv_idx)
{
	ble_err_t ret = ERR_SUCCESS;

	BLE_APP_CHECK_ACTVS_IDX(actv_idx);

	if (app_ble_actv_state_get(actv_idx) == ACTV_SCAN_CREATED) {
		// Prepare the GAPM_ACTIVITY_CREATE_CMD message
		struct gapm_activity_delete_cmd *p_cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_DELETE_CMD,
									TASK_BLE_GAPM, TASK_BLE_APP,
									gapm_activity_delete_cmd);

		if (p_cmd) {
			// Set operation code
			p_cmd->operation = GAPM_DELETE_ACTIVITY;
			p_cmd->actv_idx = app_ble_env.actvs[actv_idx].gap_advt_idx;

			// Send the message
			kernel_msg_send(p_cmd);

			ret = ERR_SUCCESS;
		} else {
			ret = ERR_NO_MEM;
		}
	} else {
		bk_printf("actv[%d] is not created\r\n", actv_idx);
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

void app_ble_next_operation(uint8_t idx, uint8_t status)
{
	uint8_t op_idx;
	ble_cmd_param_t cmd_ret;
	ble_cmd_cb_t cmd_op_cb = app_ble_env.op_cb;
	ble_cmd_t cmd = app_ble_env.cmd;

	for (op_idx = 0; op_idx < 32; op_idx++) {
		if ((app_ble_env.op_mask >> op_idx) & 0x1)
			break;
	}

	switch (cmd) {
	case BLE_CREATE_ADV:
	case BLE_SET_ADV_DATA:
	case BLE_SET_RSP_DATA:
	case BLE_START_ADV:
	case BLE_STOP_ADV:
	case BLE_DELETE_ADV:
	case BLE_CREATE_SCAN:
	case BLE_START_SCAN:
	case BLE_STOP_SCAN:
	case BLE_DELETE_SCAN:
	case BLE_CONN_UPDATE_MTU:
	case BLE_CONN_UPDATE_PARAM:
	case BLE_CONN_DIS_CONN:
		app_ble_reset();
		if (cmd_op_cb) {
			cmd_ret.cmd_idx = idx;
			cmd_ret.status = status;
			cmd_op_cb(cmd, &cmd_ret);
		}
		break;
	case BLE_INIT_ADV:
		if ((app_ble_env.op_mask) && (status == ERR_SUCCESS)) {
			switch (op_idx) {
			case BLE_OP_SET_ADV_DATA_POS:
				app_ble_set_adv_data(idx, app_ble_env.actvs[idx].param.adv.advData, app_ble_env.actvs[idx].param.adv.advDataLen);
				break;
			case BLE_OP_SET_RSP_DATA_POS:
				app_ble_set_scan_rsp_data(idx, app_ble_env.actvs[idx].param.adv.respData, app_ble_env.actvs[idx].param.adv.respDataLen);
				break;
			case BLE_OP_START_ADV_POS:
				app_ble_start_advertising(idx, app_ble_env.actvs[idx].param.adv.duration);
				break;
			default:
				bk_printf("Cmd[%d] have err operation[%d]\r\n", app_ble_env.cmd, op_idx);
				break;
			}
		} else {
			if (cmd_op_cb) {
				app_ble_env.op_mask = 0;
				app_ble_env.op_cb = NULL;
				cmd_ret.cmd_idx = idx;
				cmd_ret.status = status;
				cmd_op_cb(cmd, &cmd_ret);
			}
			if (status != ERR_SUCCESS) {
				if (app_ble_actv_state_get(idx) == ACTV_ADV_CREATED) {
					app_ble_delete_advertising(idx);
				} else {
					app_ble_reset();
				}
			} else {
				app_ble_reset();
			}
		}
		break;
	case BLE_DEINIT_ADV:
		if (app_ble_env.op_mask) {
			if (op_idx == BLE_OP_DEL_ADV_POS) {
				app_ble_delete_advertising(idx);
			} else {
				bk_printf("Cmd[%d] have err operation[%d]\r\n", app_ble_env.cmd, op_idx);
			}
		} else {
			app_ble_reset();
			if (cmd_op_cb) {
				cmd_ret.cmd_idx = idx;
				cmd_ret.status = status;
				cmd_op_cb(cmd, &cmd_ret);
			}
		}
		break;
	case BLE_INIT_SCAN:
		if ((app_ble_env.op_mask) && (status == ERR_SUCCESS)) {
			if (op_idx == BLE_OP_START_SCAN_POS) {
				app_ble_start_scaning(idx, app_ble_env.actvs[idx].param.scan.interval, app_ble_env.actvs[idx].param.scan.window);
			} else {
				bk_printf("Cmd[%d] have err operation[%d]\r\n", app_ble_env.cmd, op_idx);
			}
		} else {
			if (cmd_op_cb) {
				app_ble_env.op_mask = 0;
				app_ble_env.op_cb = NULL;
				cmd_ret.cmd_idx = idx;
				cmd_ret.status = status;
				cmd_op_cb(cmd, &cmd_ret);
			}
			if (status != ERR_SUCCESS) {
				if (app_ble_actv_state_get(idx) == ACTV_SCAN_CREATED) {
					app_ble_delete_scaning(idx);
				} else {
					app_ble_reset();
				}
			} else {
				app_ble_reset();
			}
		}
		break;
	case BLE_DEINIT_SCAN:
		if (app_ble_env.op_mask) {
			if (op_idx == BLE_OP_DEL_SCAN_POS) {
				app_ble_delete_scaning(idx);
			} else {
				bk_printf("Cmd[%d] have err operation[%d]\r\n", app_ble_env.cmd, op_idx);
			}
		} else {
			app_ble_reset();
			if (cmd_op_cb) {
				cmd_ret.cmd_idx = idx;
				cmd_ret.status = status;
				cmd_op_cb(cmd, &cmd_ret);
			}
		}
		break;
	case BLE_INIT_CREATE:
		bk_printf("Cmd[%d]operation[%d]BLE_INIT_CREATE\r\n", app_ble_env.cmd, op_idx);
		app_ble_reset();
		if (cmd_op_cb) {
			cmd_ret.cmd_idx = idx;
			cmd_ret.status = status;
			cmd_op_cb(cmd, &cmd_ret);
		}
		break;
	case BLE_INIT_START_CONN:
		bk_printf("Cmd[%d]operation[%d]BLE_INIT_START_CONN\r\n", app_ble_env.cmd, op_idx);
		app_ble_reset();
		if (cmd_op_cb) {
			cmd_ret.cmd_idx = idx;
			cmd_ret.status = status;
			cmd_op_cb(cmd, &cmd_ret);
		}
		break;
	case BLE_INIT_STOP_CONN:
		bk_printf("Cmd[%d]operation[%d]BLE_INIT_STOP_CONN\r\n", app_ble_env.cmd, op_idx);
		app_ble_reset();
		if (cmd_op_cb) {
			cmd_ret.cmd_idx = idx;
			cmd_ret.status = status;
			cmd_op_cb(cmd, &cmd_ret);
		}
		break;
	case BLE_INIT_DIS_CONN:
		break;
	case BLE_INIT_READ_CHAR:
	case BLE_INIT_WRITE_CHAR:
		bk_printf("Cmd[%d]operation[%d] READ/Write CHAR\r\n", app_ble_env.cmd, op_idx);
		app_ble_reset();
		if (cmd_op_cb) {
			cmd_ret.cmd_idx = idx;
			cmd_ret.status = status;
			cmd_op_cb(cmd, &cmd_ret);
		}
		break;
	default:
		bk_printf("unknow ble app command:%d\r\n", app_ble_env.cmd);
		break;
	}
}

/////////////// other /////////////////////////////////////////////
void ble_appm_send_gapm_reset_cmd(void)
{
	//// Reset the stack
	struct gapm_reset_cmd *p_cmd = KERNEL_MSG_ALLOC(GAPM_RESET_CMD,
							TASK_BLE_GAPM,
							TASK_BLE_APP,
							gapm_reset_cmd);

	if (p_cmd) {
		p_cmd->operation = GAPM_RESET;
		kernel_msg_send(p_cmd);
	}
}

void appm_init( void )
{
	// Reset the application manager environment
	memset(&app_ble_env, 0, sizeof(struct app_env_tag));
	for (int i = 0; i < BLE_CONNECTION_MAX; i++) {
		app_ble_env.connections[i].conhdl = UNKNOW_CONN_HDL;
		app_ble_env.connections[i].role = APP_BLE_NONE_ROLE;
	}
	// Create APP task
	kernel_task_create(TASK_BLE_APP, &TASK_BLE_DESC_APP);

	// Initialize Task state
	for (int i = 0; i < BLE_CONNECTION_MAX; i++) {
		kernel_state_set(KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(i)), APPC_LINK_IDLE);
	}
	for (int i = 0; i < BLE_ACTIVITY_MAX; i++) {
		kernel_state_set(KERNEL_BUILD_ID(TASK_BLE_APP,i), APPM_INIT);
	}
	kernel_state_set(TASK_BLE_APP, APPM_INIT);
	/*------------------------------------------------------
	* INITIALIZE ALL MODULES
	*------------------------------------------------------*/
#if (BLE_APP_PRESENT && (BLE_CENTRAL)  && BLE_SDP_CLIENT)
	sdp_service_init();
	#if APP_INIT_REUSE_ACTV_IDX
	app_ble_initing_init();
	#endif
#endif

	// Reset the stack
	ble_appm_send_gapm_reset_cmd();
}

void ble_ps_enable_set(void)
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    ble_sleep_enable = 1;
    GLOBAL_INT_RESTORE();
}

void ble_ps_enable_clear(void)
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    ble_sleep_enable = 0;
    GLOBAL_INT_RESTORE();
}

UINT32 ble_ps_enabled(void )
{
    uint32_t value = 0;
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    value =  ble_sleep_enable;
    GLOBAL_INT_RESTORE();
    return value;
}
#endif //(BLE_APP_PRESENT)

/// @} APP
