/**
 ****************************************************************************************
 *
 * @file appm_task.c
 *
 * @brief RW APP Task implementation
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APPTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"          // SW configuration

#if (BLE_APP_PRESENT)

#include "rwapp_config.h"
#include "app_task.h"             // Application Manager Task API
#include "app_ble.h"                  // Application Manager Definition
#include "gapc_task.h"            // GAP Controller Task API
#include "gapm_task.h"            // GAP Manager Task API
#include "architect.h"                 // Platform Definitions
#include <string.h>
#include "common_utils.h"
#include "kernel_timer.h"             // Kernel timer
#include "app_ble.h"
#include "gattc_task.h"
#if (BLE_APP_PRESENT && (BLE_CENTRAL))
#include "app_ble_init.h"
#include "sdp_comm_pub.h"
#if (BLE_SDP_CLIENT)
#include "app_sdp.h"
#endif
#endif

#if (BLE_APP_COMM)
#include "app_comm.h"
#endif

#ifdef __func__
#undef __func__
#endif
#define __func__   __FUNCTION__

#if (BLE_CENTRAL) && (CFG_INIT_ENABLE)
extern int app_init_con_dev_timerout_clear(unsigned char conidx);
#endif

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

extern struct app_env_tag app_ble_env;

static uint8_t app_get_handler(const struct app_subtask_handlers *handler_list_desc,
                               kernel_msg_id_t msgid,
                               void *param,
                               kernel_task_id_t src_id)
{
    // Counter
    uint8_t counter;

    // Get the message handler function by parsing the message table
    for (counter = handler_list_desc->msg_cnt; 0 < counter; counter--)
    {
        struct kernel_msg_handler handler
                = (struct kernel_msg_handler)(*(handler_list_desc->p_msg_handler_tab + counter - 1));

        if ((handler.id == msgid) ||
            (handler.id == KERNEL_MSG_DEFAULT_HANDLER))
        {
            // If handler is NULL, message should not have been received in this state
            BLE_ASSERT_ERR(handler.func);

            return (uint8_t)(handler.func(msgid, param, TASK_BLE_APP, src_id));
        }
    }

    // If we are here no handler has been found, drop the message
    return (KERNEL_MSG_CONSUMED);
}

/*
 * MESSAGE HANDLERS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles GAPM_ACTIVITY_CREATED_IND event
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_activity_created_ind_handler(kernel_msg_id_t const msgid,
                                             struct gapm_activity_created_ind const *p_param,
                                             kernel_task_id_t const dest_id,
                                             kernel_task_id_t const src_id)
{
	uint8_t conidx = KERNEL_IDX_GET(dest_id);
	uint8_t actv_idx = (app_ble_env.app_status >> BLE_APP_IDX_POS);

#if BLE_CENTRAL && CFG_INIT_ENABLE
	if(BLE_APP_INITING_CHECK_INDEX(conidx) && (p_param->actv_type == GAPM_ACTV_TYPE_INIT)){
		if (p_param->actv_type == GAPM_ACTV_TYPE_INIT){
			bk_printf("[%s]conidx:%d,idx:%d,p_param->actv_idx:%d\r\n",__func__,conidx,BLE_APP_INITING_GET_INDEX(conidx), p_param->actv_idx);
			// Store the scaning activity index
			#if APP_INIT_REUSE_ACTV_IDX
			appm_set_initing_actv_idx(BLE_APP_INITING_GET_INDEX(conidx),p_param->actv_idx,BLE_INIT_IDX_USED);
			#endif
			app_ble_env.connections[BLE_APP_INITING_GET_INDEX(conidx)].gap_actv_idx = p_param->actv_idx;
			BLE_APP_MASTER_SET_IDX_STATE(actv_idx,APP_INIT_STATE_CREATED);
		}
	}else
#endif
	{
		if (actv_idx >= BLE_ACTIVITY_MAX) {
			bk_printf("unknow actv idx:%d\r\n", actv_idx);
		} else {
			app_ble_env.actvs[actv_idx].gap_advt_idx = p_param->actv_idx;

			if (p_param->actv_type == GAPM_ACTV_TYPE_ADV) {
				BLE_APP_SET_ACTVS_IDX_STATE(actv_idx, ACTV_ADV_CREATED);
			} else if (p_param->actv_type == GAPM_ACTV_TYPE_SCAN) {
				BLE_APP_SET_ACTVS_IDX_STATE(actv_idx, ACTV_SCAN_CREATED);
			} else if (p_param->actv_type == GAPM_ACTV_TYPE_INIT) {
				bk_printf("[%s]actv_idx:%d,conidx:%d error\r\n",__func__,actv_idx,conidx);
			} else if (p_param->actv_type == GAPM_ACTV_TYPE_PER_SYNC) {
				BLE_APP_SET_ACTVS_IDX_STATE(actv_idx, ACTV_PER_SYNC_CREATED);
			} else {
				bk_printf("unknow actv type:%d\r\n", p_param->actv_type);
			}
		}
	}
	return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles GAPM_ACTIVITY_STOPPED_IND event.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_activity_stopped_ind_handler(kernel_msg_id_t const msgid,
                                             struct gapm_activity_stopped_ind const *p_param,
                                             kernel_task_id_t const dest_id,
                                             kernel_task_id_t const src_id)
{
	uint8_t conidx = KERNEL_IDX_GET(dest_id);
	bk_printf("[%s]conidx:%d\r\n",__func__,conidx);

	uint8_t actv_idx = app_ble_find_actv_idx_handle(p_param->actv_idx);

	if (actv_idx >= BLE_ACTIVITY_MAX) {
		bk_printf("unknow actv idx:%d\r\n", actv_idx);
	} else {
		if (p_param->actv_type == GAPM_ACTV_TYPE_ADV) {
			BLE_APP_SET_ACTVS_IDX_STATE(actv_idx, ACTV_ADV_CREATED);
		} else if (p_param->actv_type == GAPM_ACTV_TYPE_SCAN) {
			BLE_APP_SET_ACTVS_IDX_STATE(actv_idx, ACTV_SCAN_CREATED);
		} else if (p_param->actv_type == GAPM_ACTV_TYPE_INIT) {
			#if CFG_INIT_ENABLE
			if(BLE_APP_INITING_CHECK_INDEX(conidx)){
				BLE_APP_MASTER_SET_IDX_STATE(BLE_APP_INITING_GET_INDEX(conidx),APP_INIT_STATE_CREATED);
				#if APP_INIT_REUSE_ACTV_IDX
				appm_set_initing_actv_idx(BLE_APP_INITING_GET_INDEX(conidx),p_param->actv_idx,BLE_INIT_IDX_STOPED);
				#endif
			}
			#endif
		} else if (p_param->actv_type == GAPM_ACTV_TYPE_PER_SYNC) {
			BLE_APP_SET_ACTVS_IDX_STATE(actv_idx, ACTV_PER_SYNC_CREATED);
		} else {
			bk_printf("unknow actv type:%d\r\n", p_param->actv_type);
		}
	}

	return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles GAPM_PROFILE_ADDED_IND event
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_profile_added_ind_handler(kernel_msg_id_t const msgid,
                                          struct gapm_profile_added_ind *param,
                                          kernel_task_id_t const dest_id,
                                          kernel_task_id_t const src_id)
{
	// Current State
	kernel_state_t state = kernel_state_get(dest_id);
	uint8_t conidx = KERNEL_IDX_GET(dest_id);
	uint16_t id = param->prf_task_id;
	uint8_t status = (param->status == GAP_ERR_NO_ERROR) ? ERR_SUCCESS : ERR_CMD_RUN;
	create_db_t ind;

	bk_printf("[%s] prf_task_id:0x%x,prf_task_nb:%d,start_hdl:%d,state:0x%x\r\n",__func__,param->prf_task_id, param->prf_task_nb,param->start_hdl,state);
	bk_printf("conidx:0x%x,role:0x%x,dest_id:0x%x,src_id:0x%x,param->status:0x%x\r\n",conidx,param->role,dest_id,src_id,param->status);

#if (BLE_SDP_CLIENT)
	if((id >= TASK_BLE_ID_SDP) && (id <= TASK_BLE_ID_SDP + BLE_NB_PROFILES) )
	{
		id = TASK_BLE_ID_SDP;
	}
#endif

#if (BLE_COMM_SERVER)
	if((id >= TASK_BLE_ID_COMMON) && (id <= TASK_BLE_ID_COMMON + BLE_NB_PROFILES))
	{
		id = TASK_BLE_ID_COMMON;
	}
#endif

	switch (id)
	{
#if (BLE_COMM_SERVER)
		case TASK_BLE_ID_COMMON:
		{
			kernel_state_set(TASK_BLE_APP, APPM_READY);

			ind.prf_id = param->prf_task_id - TASK_BLE_ID_COMMON;
			ind.status = status;

			if (ble_event_notice)
				ble_event_notice(BLE_5_CREATE_DB, &ind);
		}
		break;
#endif

#if (BLE_SDP_CLIENT)
		case TASK_BLE_ID_SDP:
		{
			///kernel_state_set(dest_id, APPM_CONNECTED);
			uint8_t find;
			find = sdp_enable_all_server_ntf_ind(BLE_APP_INITING_GET_INDEX(BLE_APP_INITING_GET_INDEX(conidx)),1);
			bk_printf("[%s]find = %d\r\n",__FUNCTION__,find);
		}
		break;
#endif
#if BLE_MESH
		case TASK_BLE_ID_MESH:
		{
			kernel_state_set(TASK_BLE_APP, APPM_READY);
		}
		break;
#endif
		default:
		{

		}
		break;
	}

    return KERNEL_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles GAP manager command complete events.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_cmp_evt_handler(kernel_msg_id_t const msgid,
                                struct gapm_cmp_evt const *param,
                                kernel_task_id_t const dest_id,
                                kernel_task_id_t const src_id)
{
#if (NVDS_SUPPORT)
	uint8_t key_len = KEY_LEN;
#endif //(NVDS_SUPPORT)
	uint8_t conidx = KERNEL_IDX_GET(dest_id);
	uint8_t actv_idx = app_ble_env.app_status >> BLE_APP_IDX_POS;
	uint8_t status = (param->status == GAP_ERR_NO_ERROR) ? ERR_SUCCESS : ERR_CMD_RUN;

	bk_printf("[%s] conidx:%d,operation:0x%x,status:0x%x\r\n",__func__,conidx,param->operation,param->status);
	switch (param->operation) {
	// Reset completed
	case (GAPM_RESET)://0
		if (param->status == GAP_ERR_NO_ERROR) {
#if (NVDS_SUPPORT)
			nvds_tag_len_t len = 6;
#endif //(NVDS_SUPPORT)

			// Set Device configuration
			struct gapm_set_dev_config_cmd* cmd = KERNEL_MSG_ALLOC(GAPM_SET_DEV_CONFIG_CMD,
									           TASK_BLE_GAPM, TASK_BLE_APP,
									           gapm_set_dev_config_cmd);
			// Set the operation
			cmd->operation = GAPM_SET_DEV_CONFIG;
			// Set the device role - Peripheral / central
			cmd->role      = GAP_ROLE_ALL;//GAP_ROLE_PERIPHERAL;

#if (BLE_APP_SEC_CON)
			// The Max MTU is increased to support the Public Key exchange
			// HOWEVER, with secure connections enabled you cannot sniff the
			// LEAP and LEAS protocols
			cmd->max_mtu = 160;
			cmd->pairing_mode = GAPM_PAIRING_SEC_CON | GAPM_PAIRING_LEGACY;
#else // !(BLE_APP_SEC_CON)
			// Do not support secure connections
			cmd->pairing_mode = GAPM_PAIRING_LEGACY;
#endif //(BLE_APP_SEC_CON)

			// Set Data length parameters
			cmd->sugg_max_tx_octets = LE_MAX_OCTETS;//LE_MIN_OCTETS;//LE_MAX_OCTETS;
			cmd->sugg_max_tx_time   = LE_MAX_TIME;//LE_MIN_TIME;//LE_MAX_TIME;

			cmd->max_mtu = 527;//ATT_DEFAULT_MTU;

#if (BLE_APP_HID)
			// Enable Slave Preferred Connection Parameters present
			cmd->att_cfg = GAPM_MASK_ATT_SLV_PREF_CON_PAR_EN;
#endif //(BLE_APP_HID)

			// Host privacy enabled by default
			cmd->privacy_cfg = 0;


#if (NVDS_SUPPORT)
			if (rwip_param.get(PARAM_ID_BD_ADDRESS, &len, &cmd->addr.addr[0]) == PARAM_OK) {
				// Check if address is a static random address
				if (cmd->addr.addr[5] & 0xC0) {
					// Host privacy enabled by default
					cmd->privacy_cfg |= GAPM_PRIV_CFG_PRIV_ADDR_BIT;
				}
			} else {
				memcpy(&cmd->addr.addr[0],&co_default_bdaddr.addr[0],BD_ADDR_LEN);
				if (cmd->addr.addr[5] & 0xC0) {
					// Host privacy enabled by default
					cmd->privacy_cfg |= GAPM_PRIV_CFG_PRIV_ADDR_BIT;
				}
			}
#endif //(NVDS_SUPPORT)
			bk_printf("cmd->addr.addr[5] :%x\r\n",cmd->addr.addr[5]);

#if (NVDS_SUPPORT)
			if ((app_sec_get_bond_status()==true) &&
			(nvds_get(NVDS_TAG_LOC_IRK, &key_len, app_ble_env.loc_irk) == NVDS_OK)) {
				memcpy(cmd->irk.key, app_ble_env.loc_irk, 16);
			} else
#endif //(NVDS_SUPPORT)
			{
				memset((void *)&cmd->irk.key[0], 0x00, KEY_LEN);
			}
			// Send message
			kernel_msg_send(cmd);
		} else {
			BLE_ASSERT_ERR(0);
		}
		break;

	case (GAPM_GEN_RAND_NB) ://0x1a
		bk_printf("gapm_cmp_evt:GAPM_GEN_RAND_NB\r\n");
		if (app_ble_env.rand_cnt == 1) {
			// Generate a second random number
			app_ble_env.rand_cnt++;
			struct gapm_gen_rand_nb_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_GEN_RAND_NB_CMD,
										TASK_BLE_GAPM, TASK_BLE_APP,
										gapm_gen_rand_nb_cmd);
			cmd->operation = GAPM_GEN_RAND_NB;
			kernel_msg_send(cmd);
		} else {
			struct gapm_set_irk_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_SET_IRK_CMD,
									TASK_BLE_GAPM, TASK_BLE_APP,
									gapm_set_irk_cmd);
			app_ble_env.rand_cnt = 0;
			///  - GAPM_SET_IRK
			cmd->operation = GAPM_SET_IRK;
			memcpy(&cmd->irk.key[0], &app_ble_env.loc_irk[0], KEY_LEN);
			kernel_msg_send(cmd);
		}
		break;

	case (GAPM_SET_IRK):
		// BLE_ASSERT_INFO(param->status == GAP_ERR_NO_ERROR, param->operation, param->status);

#if (BLE_APP_SEC)
		// If not Bonded already store the generated value in NVDS
		if (app_sec_get_bond_status()==false) {
#if (NVDS_SUPPORT)
			if (nvds_put(NVDS_TAG_LOC_IRK, KEY_LEN, (uint8_t *)&app_ble_env.loc_irk) != NVDS_OK)
#endif //(NVDS_SUPPORT)
			{
				BLE_ASSERT_INFO(0, 0, 0);
			}
		}
#endif //(BLE_APP_SEC)
		app_ble_env.rand_cnt = 0;

		// Go to the create db state
		kernel_state_set(TASK_BLE_APP, APPM_READY);
		bk_printf("gapm_cmp_evt:BLE_STACK_OK\r\n");
		app_ble_env.app_status = APP_BLE_READY;

		if (ble_event_notice) {
			ble_event_notice(BLE_5_STACK_OK, NULL);
		}
		break;

	// Device Configuration updated
	case (GAPM_SET_DEV_CONFIG):
		BLE_ASSERT_INFO(param->status == GAP_ERR_NO_ERROR, param->operation, param->status);
		bk_printf("gapm_cmp_evt:GAPM_SET_DEV_CONFIG\r\n");

#if (BLE_APP_SEC)
		if (app_sec_get_bond_status()==true) {
#if (NVDS_SUPPORT)
			// If Bonded retrieve the local IRK from NVDS
			if (nvds_get(NVDS_TAG_LOC_IRK, &key_len, app_ble_env.loc_irk) == NVDS_OK) {
				// Set the IRK in the GAP
				struct gapm_set_irk_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_SET_IRK_CMD,
										TASK_BLE_GAPM, TASK_BLE_APP,
										gapm_set_irk_cmd);
				///  - GAPM_SET_IRK:
				cmd->operation = GAPM_SET_IRK;
				memcpy(&cmd->irk.key[0], &app_ble_env.loc_irk[0], KEY_LEN);
				kernel_msg_send(cmd);
				bk_printf("gapm_cmp_evt:wait GAPM_SET_IRK\r\n");
			} else
#endif //(NVDS_SUPPORT)
			{
				BLE_ASSERT_ERR(0);
			}
		} else // Need to start the generation of new IRK
#endif //(BLE_APP_SEC)
		{
			struct gapm_gen_rand_nb_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_GEN_RAND_NB_CMD,
										TASK_BLE_GAPM, TASK_BLE_APP,
										gapm_gen_rand_nb_cmd);
			cmd->operation   = GAPM_GEN_RAND_NB;
			app_ble_env.rand_cnt = 1;
			kernel_msg_send(cmd);
			bk_printf("gapm_cmp_evt:wait GAPM_GEN_RAND_NB\r\n");
		}
		break;

#if (BLE_CENTRAL )
		case (GAPM_CREATE_INIT_ACTIVITY)://0xA2
			if(BLE_APP_INITING_CHECK_INDEX(conidx)){
				if(app_ble_env.connections[BLE_APP_INITING_GET_INDEX(conidx)].conn_op_mask & (1 << BLE_OP_CREATE_INIT_POS)){
					BLE_APP_MASTER_CLEAR_IDX_OP_MASK_BITS(BLE_APP_INITING_GET_INDEX(conidx),BLE_OP_CREATE_INIT_POS);
				}
				if((param->status != GAP_ERR_NO_ERROR)){
					BLE_APP_MASTER_SET_IDX_STATE(BLE_APP_INITING_GET_INDEX(conidx),APP_INIT_STATE_IDLE);
				}else{
					BLE_APP_MASTER_SET_IDX_STATE(BLE_APP_INITING_GET_INDEX(conidx),APP_INIT_STATE_CREATED);
					///BLE_APP_MASTER_SET_IDX_CALLBACK_HANDLE(actv_idx,BLE_INIT_CREATE,NULL);
				}
				if(app_ble_env.cmd == BLE_INIT_CREATE){
					app_ble_env.op_mask &= ~(1 << BLE_OP_CREATE_INIT_POS);
					app_ble_next_operation(actv_idx, status);
				}
			}else{
				bk_printf("gapm_cmp_evt:INIT index %d error\r\n",conidx);
			}
			break;
#endif
#if (BLE_OBSERVER || BLE_CENTRAL)
	case (GAPM_CREATE_SCAN_ACTIVITY)://0xA1
		if (actv_idx >= BLE_ACTIVITY_MAX) {
			bk_printf("unknow actv idx:%d\r\n", actv_idx);
		} else {
			app_ble_env.op_mask &= ~(1 << BLE_OP_CREATE_SCAN_POS);
			app_ble_next_operation(actv_idx, status);
		}
		break;
#endif
	case (GAPM_CREATE_ADV_ACTIVITY)://0xA0
		if (actv_idx >= BLE_ACTIVITY_MAX) {
			bk_printf("unknow actv idx:%d\r\n", actv_idx);
		} else {
			app_ble_env.op_mask &= ~(1 << BLE_OP_CREATE_ADV_POS);
			app_ble_next_operation(actv_idx, status);
		}
		break;
	case (GAPM_SET_ADV_DATA)://0xA9
		if (actv_idx >= BLE_ACTIVITY_MAX) {
			bk_printf("unknow actv idx:%d\r\n", actv_idx);
		} else {
			app_ble_env.op_mask &= ~(1 << BLE_OP_SET_ADV_DATA_POS);
			app_ble_next_operation(actv_idx, status);
		}
		break;
	case (GAPM_SET_SCAN_RSP_DATA)://0xAA
		if (actv_idx >= BLE_ACTIVITY_MAX) {
			bk_printf("unknow actv idx:%d\r\n", actv_idx);
		} else {
			app_ble_env.op_mask &= ~(1 << BLE_OP_SET_RSP_DATA_POS);
			app_ble_next_operation(actv_idx, status);
		}
		break;
	case (GAPM_START_ACTIVITY): ///0xA4
		#if (BLE_CENTRAL)
		if(BLE_APP_INITING_CHECK_INDEX(conidx)){
			if(app_ble_env.connections[BLE_APP_INITING_GET_INDEX(conidx)].conn_op_mask & (1 << BLE_OP_INIT_START_POS)){
				BLE_APP_MASTER_CLEAR_IDX_OP_MASK_BITS(BLE_APP_INITING_GET_INDEX(conidx),BLE_OP_INIT_START_POS);
			}
			if((param->status == GAP_ERR_NO_ERROR)){
				bk_printf("[%s]actv_idx:%d,init_state:APP_INIT_STATE_CONECTTING\r\n",__func__,BLE_APP_INITING_GET_INDEX(conidx));
				BLE_APP_MASTER_SET_IDX_STATE(BLE_APP_INITING_GET_INDEX(conidx),APP_INIT_STATE_CONECTTING);
			}else{
				BLE_APP_MASTER_SET_IDX_STATE(BLE_APP_INITING_GET_INDEX(conidx),APP_INIT_STATE_CREATED);
				bk_printf("[%s]actv_idx:%d,init_state:APP_INIT_STATE_CREATED\r\n",__func__,BLE_APP_INITING_GET_INDEX(conidx));
			}

			if (app_ble_env.cmd == BLE_INIT_START_CONN) {
				app_ble_env.op_mask &= ~(1 << BLE_OP_CREATE_INIT_POS);
				app_ble_next_operation(actv_idx, status);
			}
		}else
		#endif
		{
			if (actv_idx >= BLE_ACTIVITY_MAX) {
				bk_printf("unknow actv idx:%d\r\n", actv_idx);
			} else {
				if (status == ERR_SUCCESS) {
					if (app_ble_env.op_mask & (1 << BLE_OP_START_SCAN_POS)) {
						app_ble_env.actvs[actv_idx].actv_status = ACTV_SCAN_STARTED;
						app_ble_env.op_mask &= ~(1 << BLE_OP_START_SCAN_POS);
					} else if (app_ble_env.op_mask & (1 << BLE_OP_START_ADV_POS)) {
						app_ble_env.actvs[actv_idx].actv_status = ACTV_ADV_STARTED;
						app_ble_env.op_mask &= ~ (1 << BLE_OP_START_ADV_POS);
					}
				}
				app_ble_next_operation(actv_idx, status);
			}
		}
		break;
	case (GAPM_STOP_ACTIVITY)://0xA5
		#if (BLE_CENTRAL)
		if(BLE_APP_INITING_CHECK_INDEX(conidx)){
			if((app_ble_env.cmd == BLE_INIT_STOP_CONN)
				|| (app_ble_env.connections[BLE_APP_INITING_GET_INDEX(conidx)].conn_op_mask & (1 << BLE_OP_INIT_STOP_POS))){
				BLE_APP_MASTER_SET_IDX_STATE(BLE_APP_INITING_GET_INDEX(conidx),APP_INIT_STATE_CREATED);
				BLE_APP_MASTER_CLEAR_IDX_OP_MASK_BITS(BLE_APP_INITING_GET_INDEX(conidx),BLE_OP_INIT_STOP_POS);
				bk_printf("[%s]BLE_INIT_STOP_CONN actv_idx:%d,init_state:APP_INIT_STATE_CREATED\r\n",__func__,actv_idx);
			}

			if (app_ble_env.cmd == BLE_INIT_STOP_CONN) {
				app_ble_env.op_mask &= ~(1 << BLE_OP_CREATE_INIT_POS);
				app_ble_next_operation(actv_idx, status);
			}
		}else
		#endif
		{
			if (actv_idx >= BLE_ACTIVITY_MAX) {
				bk_printf("unknow actv idx:%d\r\n", actv_idx);
			} else {
				app_ble_env.op_mask &= ~((1 << BLE_OP_STOP_SCAN_POS) | (1 << BLE_OP_STOP_ADV_POS));
				app_ble_next_operation(actv_idx, status);
			}
		}
		break;
	case (GAPM_DELETE_ACTIVITY)://0xA7
		if (actv_idx >= BLE_ACTIVITY_MAX) {
			bk_printf("unknow actv idx:%d\r\n", actv_idx);
		} else {
			app_ble_env.actvs[actv_idx].actv_status = ACTV_IDLE;
			app_ble_env.op_mask &= ~((1 << BLE_OP_DEL_SCAN_POS) | (1 << BLE_OP_DEL_ADV_POS));
			app_ble_next_operation(actv_idx, status);
		}
		break;

	default:
		break;
    }

    return (KERNEL_MSG_CONSUMED);
}

static int gapc_get_dev_info_req_ind_handler(kernel_msg_id_t const msgid,
        struct gapc_get_dev_info_req_ind const *param,
        kernel_task_id_t const dest_id,
        kernel_task_id_t const src_id)
{
    bk_printf("%s,req:0x%x\r\n",__func__,param->req);
    switch(param->req)
    {
        case GAPC_DEV_NAME:
        {
            struct gapc_get_dev_info_cfm * cfm = KERNEL_MSG_ALLOC_DYN(GAPC_GET_DEV_INFO_CFM,
                                                    src_id, dest_id,
                                                    gapc_get_dev_info_cfm, APP_DEVICE_NAME_MAX_LEN);
            cfm->req = param->req;
            cfm->info.name.length = ble_appm_get_dev_name(cfm->info.name.value, APP_DEVICE_NAME_MAX_LEN);

            bk_printf("length:%d,name:%s\r\n",cfm->info.name.length,cfm->info.name.value);
            // Send message
            kernel_msg_send(cfm);
        } break;

        case GAPC_DEV_APPEARANCE:
        {
            // Allocate message
            struct gapc_get_dev_info_cfm *cfm = KERNEL_MSG_ALLOC(GAPC_GET_DEV_INFO_CFM,
                                                             src_id, dest_id,
                                                             gapc_get_dev_info_cfm);
            cfm->req = param->req;
            // Set the device appearance
            #if (BLE_APP_HT)
            // Generic Thermometer - TODO: Use a flag
            cfm->info.appearance = 728;
            #elif (BLE_APP_HID)
            // HID Mouse
            cfm->info.appearance = 962;
            #else
            // No appearance
            cfm->info.appearance = 0;
            #endif

            // Send message
            kernel_msg_send(cfm);
        } break;

        case GAPC_DEV_SLV_PREF_PARAMS:
        {
            // Allocate message
            struct gapc_get_dev_info_cfm *cfm = KERNEL_MSG_ALLOC(GAPC_GET_DEV_INFO_CFM,
                    src_id, dest_id,
                                                            gapc_get_dev_info_cfm);
            cfm->req = param->req;
            // Slave preferred Connection interval Min
            cfm->info.slv_pref_params.con_intv_min = 8;
            // Slave preferred Connection interval Max
            cfm->info.slv_pref_params.con_intv_max = 10;
            // Slave preferred Connection latency
            cfm->info.slv_pref_params.slave_latency  = 0;
            // Slave preferred Link supervision timeout
            cfm->info.slv_pref_params.conn_timeout    = 200;  // 2s (500*10ms)

            // Send message
            kernel_msg_send(cfm);
        } break;

        default: /* Do Nothing */ break;
    }


    return (KERNEL_MSG_CONSUMED);
}
/**
 ****************************************************************************************
 * @brief Handles GAPC_SET_DEV_INFO_REQ_IND message.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_set_dev_info_req_ind_handler(kernel_msg_id_t const msgid,
        struct gapc_set_dev_info_req_ind const *param,
        kernel_task_id_t const dest_id,
        kernel_task_id_t const src_id)
{
    // Set Device configuration
    struct gapc_set_dev_info_cfm* cfm = KERNEL_MSG_ALLOC(GAPC_SET_DEV_INFO_CFM, src_id, dest_id,
                                                     gapc_set_dev_info_cfm);
    // Reject to change parameters
    cfm->status = GAP_ERR_REJECTED;
    cfm->req = param->req;
    // Send message
    kernel_msg_send(cfm);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles connection complete event from the GAP. Enable all required profiles
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_connection_req_ind_handler(kernel_msg_id_t const msgid,
                                           struct gapc_connection_req_ind const *param,
                                           kernel_task_id_t const dest_id,
                                           kernel_task_id_t const src_id)
{
	uint8_t conidx = KERNEL_IDX_GET(src_id);
	uint8_t conn_idx;
	conn_ind_t conn_info;
	bk_printf("[%s]conidx:%d,dest_id:0x%x\r\n",__func__,conidx,dest_id);

	// Check if the received Connection Handle was valid
	if (conidx != GAP_INVALID_CONIDX) {
		if(param->role != APP_BLE_MASTER_ROLE){
			conn_idx = app_ble_get_idle_conn_idx_handle();
			bk_printf("[%s]ble_slave conn_idx:%d\r\n", __FUNCTION__,conn_idx);
		}else{
			conn_idx = KERNEL_IDX_GET(dest_id);
			if(BLE_APP_INITING_CHECK_INDEX(conn_idx)){
				conn_idx = BLE_APP_INITING_GET_INDEX(conn_idx);
			}else{
				conn_idx = BLE_CONNECTION_MAX;
			}
			bk_printf("[%s]ble_master conn_idx:%d\r\n", __FUNCTION__,conn_idx);
		}
		if (BLE_CONNECTION_MAX == conn_idx) {
			bk_printf("%s:Can't get conn idx\r\n", __FUNCTION__);
		} else {
			// Retrieve the connection info from the parameters
			app_ble_env.connections[conn_idx].conhdl = conidx;
			app_ble_env.connections[conn_idx].con_interval = param->con_interval;
			app_ble_env.connections[conn_idx].con_latency = param->con_latency;
			app_ble_env.connections[conn_idx].sup_to = param->sup_to;
			app_ble_env.connections[conn_idx].clk_accuracy = param->clk_accuracy;
			app_ble_env.connections[conn_idx].peer_addr_type = param->peer_addr_type;
			memcpy(app_ble_env.connections[conn_idx].peer_addr.addr,param->peer_addr.addr,BD_ADDR_LEN);
			app_ble_env.connections[conn_idx].role = param->role;
		}

		// Send connection confirmation
		struct gapc_connection_cfm *cfm = KERNEL_MSG_ALLOC(GAPC_CONNECTION_CFM,
									KERNEL_BUILD_ID(TASK_BLE_GAPC, conidx),
									////KERNEL_BUILD_ID(TASK_BLE_APP,conidx),
									KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conn_idx)),
									gapc_connection_cfm);

#if(BLE_APP_SEC)
		cfm->auth      = app_sec_get_bond_status() ? GAP_AUTH_REQ_NO_MITM_BOND : GAP_AUTH_REQ_NO_MITM_NO_BOND; // TODO [FBE] restore valid data
#else // !(BLE_APP_SEC)
		cfm->auth      = GAP_AUTH_REQ_NO_MITM_NO_BOND;
#endif // (BLE_APP_SEC)
		// Send the message
		kernel_msg_send(cfm);

		conn_info.conn_idx = conidx;
		conn_info.peer_addr_type = param->peer_addr_type;
		memcpy(conn_info.peer_addr, param->peer_addr.addr, GAP_BD_ADDR_LEN);
		if(param->role == APP_BLE_MASTER_ROLE)   ///BLE_MASTER
		{
			bk_printf("KERNEL_IDX:%d,conn_idx:%d\r\n",app_ble_env.connections[conn_idx].conhdl,conn_idx);
			#if (BLE_CENTRAL) && (CFG_INIT_ENABLE)
			app_init_con_dev_timerout_clear(conn_idx);
			#endif
			#if BLE_SDP_CLIENT
			sdp_discover_all_service(conn_idx);
			#endif
			if (ble_event_notice) {
				ble_event_notice(BLE_5_INIT_CONNECT_EVENT, &conn_info);
			}
		}else{
			if (ble_event_notice) {
				ble_event_notice(BLE_5_CONNECT_EVENT, &conn_info);
			}
		}
	}

	return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles connection complete event from the GAP. Enable all required profiles
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_param_update_req_ind_handler(kernel_msg_id_t const msgid,
                                           struct gapc_param_update_req_ind const *param,
                                           kernel_task_id_t const dest_id,
                                           kernel_task_id_t const src_id)
{
	uint8_t conidx = KERNEL_IDX_GET(src_id);

	bk_printf("%s\r\n",__func__);
	// Send connection confirmation
	struct gapc_param_update_cfm *cfm = KERNEL_MSG_ALLOC(GAPC_PARAM_UPDATE_CFM,
								KERNEL_BUILD_ID(TASK_BLE_GAPC, conidx), TASK_BLE_APP,
								gapc_param_update_cfm);

	cfm->accept = 1;//true;
	cfm->ce_len_min = 10;
	cfm->ce_len_max = 20;

	// Send message
	kernel_msg_send(cfm);

	return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief  GAPC_PARAM_UPDATED_IND
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_param_updated_ind_handler (kernel_msg_id_t const msgid, 
							const struct gapc_param_updated_ind  *param,
							kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
	uint8_t conidx = KERNEL_IDX_GET(src_id);
	uint8_t conn_idx = app_ble_find_conn_idx_handle(conidx);

	if (BLE_CONNECTION_MAX == conn_idx) {
		bk_printf("%s:Unknow conntions\r\n", __FUNCTION__);
	} else {
		app_ble_env.connections[conn_idx].con_interval = param->con_interval;
		app_ble_env.connections[conn_idx].con_latency = param->con_latency;
		app_ble_env.connections[conn_idx].sup_to = param->sup_to;
	}

	return KERNEL_MSG_CONSUMED;
}

/*******************************************************************************
 * Function: gapc_le_pkt_size_ind_handler
 * Description: GAPC_LE_PKT_SIZE_IND
 * Input: msgid   -Id of the message received.
 *		  param   -Pointer to the parameters of the message.
 *		  dest_id -ID of the receiving task instance
 *		  src_id  -ID of the sending task instance.
 * Return: If the message was consumed or not.
 * Others: void
*******************************************************************************/
static int gapc_le_pkt_size_ind_handler (kernel_msg_id_t const msgid, 
							const struct gapc_le_pkt_size_ind  *param,
							kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
	uint8_t conidx = KERNEL_IDX_GET(src_id);
	bk_printf("%s msgid:0x%x,dest_id:0x%x,src_id:0x%x\r\n",__func__,msgid,dest_id,src_id);
	bk_printf("conidx:%x,",conidx);
	bk_printf("1max_rx_octets = %d\r\n",param->max_rx_octets);
	bk_printf("1max_rx_time = %d\r\n",param->max_rx_time);
	bk_printf("1max_tx_octets = %d\r\n",param->max_tx_octets);
	bk_printf("1max_tx_time = %d\r\n",param->max_tx_time);

	return KERNEL_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles GAP controller command complete events.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_cmp_evt_handler(kernel_msg_id_t const msgid,
                                struct gapc_cmp_evt const *param,
                                kernel_task_id_t const dest_id,
                                kernel_task_id_t const src_id)
{
	uint8_t conidx = KERNEL_IDX_GET(dest_id);
	uint8_t conn_idx = (app_ble_env.app_status >> BLE_APP_IDX_POS);
	uint8_t status = (param->status == GAP_ERR_NO_ERROR) ? ERR_SUCCESS : ERR_CMD_RUN;

	bk_printf("%s conidx:%d,operation:0x%x,status:%x\r\n", __func__, conidx, param->operation, param->status);

	switch (param->operation) {
	case (GAPC_UPDATE_PARAMS):
		if (conn_idx >= BLE_CONNECTION_MAX) {
			bk_printf("unknow conn idx:%d\r\n", conn_idx);
		} else {
			if((app_ble_env.app_status &  APP_BLE_CMD_RUNNING )
				&& (app_ble_env.connections[conn_idx].conn_op_mask & (1 << BLE_OP_UPDATE_CONN_POS))){
				app_ble_env.connections[conn_idx].conn_op_mask &= ~(1 << BLE_OP_UPDATE_CONN_POS);
				app_ble_next_operation(conn_idx, status);
			}
		}
		break;

	case (GAPC_DISCONNECT):
		if (conn_idx >= BLE_CONNECTION_MAX) {
			bk_printf("unknow conn idx:%d\r\n", conn_idx);
		} else {
			app_ble_env.connections[conn_idx].conn_op_mask &= ~(1 << BLE_OP_UPDATE_CONN_POS);
			app_ble_next_operation(conn_idx, status);
		}
		break;

	default:
		break;
	}

	return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles disconnection complete event from the GAP.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_disconnect_ind_handler(kernel_msg_id_t const msgid,
                                      struct gapc_disconnect_ind const *param,
                                      kernel_task_id_t const dest_id,
                                      kernel_task_id_t const src_id)
{

	uint8_t conidx = KERNEL_IDX_GET(src_id);
	discon_ind_t dis_info;
	uint8_t conn_idx = app_ble_find_conn_idx_handle(conidx);

	bk_printf("[%s]conidx:%d,conhdl:%d,reason:0x%x\r\n",__func__, conidx, param->conhdl, param->reason);

	if (BLE_CONNECTION_MAX == conn_idx) {
		bk_printf("%s:Unknow conntions\r\n", __FUNCTION__);
	} else {
		app_ble_env.connections[conn_idx].conhdl = UNKNOW_CONN_HDL;
	}
	dis_info.reason = param->reason;
	if(BLE_APP_INITING_CHECK_INDEX(conidx)){
		conn_idx = BLE_APP_INITING_GET_INDEX(conidx);
		kernel_state_set(KERNEL_BUILD_ID(TASK_BLE_APP,conidx),APPC_LINK_IDLE);
		dis_info.conn_idx = conn_idx;
		if (ble_event_notice) {
			ble_event_notice(BLE_5_INIT_DISCONNECT_EVENT, &dis_info);
		}
	}else{
		dis_info.conn_idx = conn_idx;
		if (ble_event_notice) {
			ble_event_notice(BLE_5_DISCONNECT_EVENT, &dis_info);
		}
	}
	return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of all messages sent from the lower layers to the application
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int appm_msg_handler(kernel_msg_id_t const msgid,
                            void *param,
                            kernel_task_id_t const dest_id,
                            kernel_task_id_t const src_id)
{
    // Retrieve identifier of the task from received message
    kernel_task_id_t src_task_id = BLE_MSG_T(msgid);
    // Message policy
    uint8_t msg_pol = KERNEL_MSG_CONSUMED;
    uint8_t found_flag = 1;
    switch (src_task_id)
    {
        case (TASK_BLE_ID_GAPC):
        {
            #if (BLE_APP_SEC)
            if ((msgid >= GAPC_BOND_CMD) &&
                (msgid <= GAPC_SECURITY_IND))
            {
                // Call the Security Module
                msg_pol = app_get_handler(&app_sec_handlers, msgid, param, src_id);
            }
            #endif //(BLE_APP_SEC)
            // else drop the message
        } break;

        case (TASK_BLE_ID_GATTC):
        {
            // Service Changed - Drop
        } break;

#if (BLE_APP_COMM)
        case (TASK_BLE_ID_COMMON):
        {
            // Call the Health Thermometer Module
            msg_pol = app_get_handler(&app_comm_table_handler, msgid, param, src_id);
        } break;
#endif //(BLE_APP_COMM)

        default:
        {
            found_flag = 0;
        } break;
    }
    if(!found_flag)
    {
        bk_printf("%s,src_task_id:0x%x,dest_id:0x%x,src_id:0x%x,msgid:0x%x\r\n",__func__,src_task_id,dest_id,src_id,msgid);  
    }
 
    return (msg_pol);
}

/**
 ****************************************************************************************
 * @brief Handles reception of random number generated message
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_gen_rand_nb_ind_handler(kernel_msg_id_t const msgid, struct gapm_gen_rand_nb_ind *param,
                                        kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    if (app_ble_env.rand_cnt==1)      // First part of IRK
    {
        memcpy(&app_ble_env.loc_irk[0], &param->randnb.nb[0], 8);
    }
    else if (app_ble_env.rand_cnt==2) // Second part of IRK
    {
        memcpy(&app_ble_env.loc_irk[8], &param->randnb.nb[0], 8);
    }

    return KERNEL_MSG_CONSUMED;
}

#if (SECURE_CONNECTIONS)
static int gapm_gen_dh_key_ind_handler(kernel_msg_id_t const msgid,
											struct gapm_gen_dh_key_ind const *param,
											kernel_task_id_t const dest_id,
											kernel_task_id_t const src_id)
{
	int msg_status = KERNEL_MSG_CONSUMED;

	return (msg_status);
}
											
static int gapm_get_key_ind_handler(kernel_msg_id_t const msgid,
						struct gapm_pub_key_ind const *param,
						kernel_task_id_t const dest_id,
						kernel_task_id_t const src_id)
{
	int msg_status = KERNEL_MSG_CONSUMED;

	return (msg_status);
}
#endif

#if (BLE_CENTRAL) && (CFG_INIT_ENABLE)
static int app_init_con_dev_timerout_handler(kernel_msg_id_t const msgid,
									 void const *ind,
									 kernel_task_id_t const dest_id,
									 kernel_task_id_t const src_id)
{
	uint8_t conidx = KERNEL_IDX_GET(dest_id);

	#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]conidx:%d\r\n",__func__,conidx);
	#endif

	if((BLE_APP_INITING_CHECK_INDEX(conidx)) && BLE_APP_MASTER_GET_IDX_STATE(BLE_APP_INITING_GET_INDEX(conidx)) == APP_INIT_STATE_CONECTTING)
	{
		appm_stop_connencting(BLE_APP_INITING_GET_INDEX(conidx));
	}

	return (KERNEL_MSG_CONSUMED);
}

int app_init_con_dev_timerout_clear(unsigned char conidx)
{
	unsigned int task_id = KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx));

	if(BLE_CONNECTION_MAX <= conidx){
		return -1;
	}

	if(kernel_timer_active(APP_INIT_CON_DEV_TIMEROUT_TIMER, task_id)){
		kernel_timer_clear(APP_INIT_CON_DEV_TIMEROUT_TIMER, task_id);
	}
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]conidx:%d\r\n",__func__,conidx);
#endif

	return 0;
}

uint16_t app_init_con_intv_policy(unsigned char conidx)
{
	return APP_CONN_KEEP_INTV  + conidx * APP_CONN_IND_DELT_TIME;
}

////APP_INIT_CON_SDP_END_TIMER
static int app_init_con_sdp_end_timer_handler(kernel_msg_id_t const msgid,
										  void const *ind,
										  kernel_task_id_t const dest_id,
										  kernel_task_id_t const src_id)
{
	int conidx = BLE_APP_INITING_GET_INDEX(KERNEL_IDX_GET(dest_id));

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_IMPO)
	bk_printf("[%s]conidx:%d\r\n",__func__,conidx);
#endif

	if(BLE_CONNECTION_MAX > conidx && conidx >= 0){
		struct gapc_conn_param conn_param;
		uint16_t intv_min = app_init_con_intv_policy(conidx);
		conn_param.intv_min = intv_min;
		conn_param.intv_max = intv_min + conidx;
		conn_param.latency = APP_CONN_LATENCY;
		conn_param.time_out = APP_CONN_SUP_TO;
		int ret = app_ble_update_param(conidx, &conn_param);
		if(ret != 0){
			bk_printf("conidx:%d update param failed\r\n",conidx);
		}
	}

	return (KERNEL_MSG_CONSUMED);
}

int app_init_con_sdp_end_timer_start(unsigned char conidx,unsigned int timeout_ms)
{
	unsigned int task_id = KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx));

	if(BLE_CONNECTION_MAX <= conidx){
		return -1;
	}
	if(timeout_ms < 10){
		timeout_ms = 10;
	}
	if(kernel_timer_active(APP_INIT_CON_SDP_END_TIMER, task_id)){
		kernel_timer_clear(APP_INIT_CON_SDP_END_TIMER, task_id);
	}
	kernel_timer_set(APP_INIT_CON_SDP_END_TIMER, task_id, timeout_ms);

	return 0;
}

static int app_init_init_event_handler(kernel_msg_id_t const msgid,
						void const *ind,
						kernel_task_id_t const dest_id,
						kernel_task_id_t const src_id)
{
	uint8_t conidx = KERNEL_IDX_GET(dest_id);
	uint8_t actv_idx = app_ble_env.app_status >> BLE_APP_IDX_POS;

	#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]conidx:%d\r\n",__func__,conidx);
	#endif

	if(BLE_APP_INITING_CHECK_INDEX(conidx)){
		if(app_ble_env.connections[BLE_APP_INITING_GET_INDEX(conidx)].conn_op_mask & (1 << BLE_OP_CREATE_INIT_POS)){
			BLE_APP_MASTER_CLEAR_IDX_OP_MASK_BITS(BLE_APP_INITING_GET_INDEX(conidx),BLE_OP_CREATE_INIT_POS);
		}
		BLE_APP_MASTER_SET_IDX_STATE(BLE_APP_INITING_GET_INDEX(conidx),APP_INIT_STATE_CREATED);
		if(app_ble_env.cmd == BLE_INIT_CREATE){
			app_ble_env.op_mask &= ~(1 << BLE_OP_CREATE_INIT_POS);
			app_ble_next_operation(actv_idx, ERR_SUCCESS);
		}
	}else{
		bk_printf("[%s]INIT index %d error\r\n",__FUNCTION__,conidx);
	}

	return (KERNEL_MSG_CONSUMED);
}

#if APP_INIT_STOP_CONN_TIMER_EVENT
static int app_init_start_timeout_event_handler(kernel_msg_id_t const msgid,
						void const *ind,
						kernel_task_id_t const dest_id,
						kernel_task_id_t const src_id)
{
	struct app_task_start_timeout_event_ind *req = (struct app_task_start_timeout_event_ind*) ind;

	#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s %d]req->task_id:%x,timout_ms:%d\r\n",__FUNCTION__,__LINE__,req->task_id,req->timout_ms);
	#endif
	kernel_timer_set(APP_INIT_CON_DEV_TIMEROUT_TIMER,req->task_id,req->timout_ms);

	return (KERNEL_MSG_CONSUMED);
}
#endif ///APP_INIT_STOP_CONN_TIMER_EVENT
#endif


#if (BLE_OBSERVER || BLE_CENTRAL )
static int gapm_ext_adv_report_ind_handler(kernel_msg_id_t const msgid, struct gapm_ext_adv_report_ind *param,
							kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
	recv_adv_t adv_param;

	adv_param.actv_idx = app_ble_find_actv_idx_handle(param->actv_idx);
	adv_param.evt_type = param->info;
	adv_param.data = &(param->data[0]);
	adv_param.data_len = param->length;
	adv_param.rssi = param->rssi;
	adv_param.adv_addr_type = param->trans_addr.addr_type;
	memcpy(adv_param.adv_addr, param->trans_addr.addr.addr, GAP_BD_ADDR_LEN);

	if (ble_event_notice)
		ble_event_notice(BLE_5_REPORT_ADV, &adv_param);

	return KERNEL_MSG_CONSUMED;
}
#endif

#if (BLE_SDP_CLIENT)
static int gattc_sdp_svc_ind_handler(kernel_msg_id_t const msgid,
                                     struct gattc_sdp_svc_ind const *ind,
                                     kernel_task_id_t const dest_id,
                                     kernel_task_id_t const src_id)
{
    ///uint8_t conidx = KERNEL_IDX_GET(src_id);
	uint8_t conidx = KERNEL_IDX_GET(dest_id);

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
    bk_printf("[%s]appc msgid:0x%x,dest_id:0x%x,src_id:0x%x,conidx:%x\r\n",
								__func__,msgid,dest_id,src_id,conidx);

    bk_printf("ind uuid len:%d,uuid:",ind->uuid_len);
    for(int i = 0;i < ind->uuid_len;i++)
    {
        bk_printf("%02x ",ind->uuid[ind->uuid_len - i - 1]);
    }
	bk_printf("\r\n");
#endif
	app_init_con_sdp_end_timer_start(BLE_APP_INITING_GET_INDEX(conidx),APP_SDP_KEEP_TIME);
	app_sdp_add_element_srv(BLE_APP_INITING_GET_INDEX(conidx),ind);

    return (KERNEL_MSG_CONSUMED);
}

static int app_gattc_event_ind_handler(kernel_msg_id_t const msgid,
                                   struct gattc_event_ind const *param,
                                   kernel_task_id_t const dest_id,
                                   kernel_task_id_t const src_id)
{
    uint8_t state = kernel_state_get(dest_id);
    uint8_t conidx = KERNEL_IDX_GET(dest_id);

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
  //  bk_printf("app gattc_event_ind conidx:%d\r\n",conidx);
	bk_printf("[%s]NOTIF RECIVE handle:%x, length:%d,value = 0x",__func__,param->handle,param->length);
	for(int i = 0; i< param->length; i++)
	{
		bk_printf("%02x ",param->value[i]);
	}
	bk_printf("\r\n");
#endif
	app_sdp_charac_callback_handler(CHARAC_NOTIFY,BLE_APP_INITING_GET_INDEX(conidx),param->handle,param->length,(uint8*)param->value);

	return (KERNEL_MSG_CONSUMED);  
}

static int app_gattc_event_req_ind_handler(kernel_msg_id_t const msgid,
                                       struct gattc_event_ind const *param,
                                       kernel_task_id_t const dest_id,
                                       kernel_task_id_t const src_id)
{
	uint8_t state = kernel_state_get(dest_id);
	uint8_t conidx = KERNEL_IDX_GET(dest_id);

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]dest_id = 0x%x,conidx = 0x%x\r\n",__func__,dest_id,conidx);
	bk_printf("[%s]type = 0x%x,length = 0x%x,handle = 0x%02x\r\n",__func__,param->type,param->length,param->handle);
	bk_printf("src_id:%x,indicate RECIVE value =  0x",src_id);
	for(int i = 0; i< param->length; i++)
	{
		bk_printf("%02x ",param->value[i]);
	}
	bk_printf("\r\n");
#endif
	app_sdp_charac_callback_handler(CHARAC_INDICATE,BLE_APP_INITING_GET_INDEX(conidx),param->handle,param->length,(uint8*)param->value);

	return (KERNEL_MSG_CONSUMED);
}
#endif

/**
 ****************************************************************************************
 * @brief  GATTC_MTU_CHANGED_IND
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_gattc_mtu_changed_ind_handler(kernel_msg_id_t const msgid,
                                     struct gattc_mtu_changed_ind const *ind,
                                     kernel_task_id_t const dest_id,
                                     kernel_task_id_t const src_id)
{
	uint8_t conidx = KERNEL_IDX_GET(src_id);
	mtu_change_t param;

	param.conn_idx = app_ble_find_conn_idx_handle(conidx);
	param.mtu_size = ind->mtu;

	if (ble_event_notice) {
		ble_event_notice(BLE_5_MTU_CHANGE, &param);
	}

 	return (KERNEL_MSG_CONSUMED);
}

static int gapm_scan_request_ind_handler(kernel_msg_id_t const msgid,
                                 struct gapm_scan_request_ind const *p_ind,
                                 kernel_task_id_t const dest_id,
                                 kernel_task_id_t const src_id)
{
	bk_printf("%s\r\n",__func__);
	bk_printf("actv_idx:%d,addr_type:%d\r\n",p_ind->actv_idx,p_ind->trans_addr.addr_type);
	bk_printf("addr ");
	for(int i = 0;i < 6;i++) {
		bk_printf(":%02x ",p_ind->trans_addr.addr.addr[i]);
	}
	bk_printf("\r\n");
	return (KERNEL_MSG_CONSUMED);
}

static int gattc_cmp_evt_handler(kernel_msg_id_t const msgid,
                                 struct gattc_cmp_evt const *param,
                                 kernel_task_id_t const dest_id,
                                 kernel_task_id_t const src_id)
{
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	uint8_t conidx = KERNEL_IDX_GET(src_id);
#endif
#if (BLE_APP_PRESENT && (BLE_CENTRAL) && (BLE_SDP_CLIENT)) || BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN) || CFG_INIT_ENABLE
	uint8_t con_idx = KERNEL_IDX_GET(dest_id);
#endif
	uint8_t conn_idx = (app_ble_env.app_status >> BLE_APP_IDX_POS);
	uint8_t status = (param->status == GAP_ERR_NO_ERROR) ? ERR_SUCCESS : ERR_CMD_RUN;
	#if CFG_INIT_ENABLE
	uint8_t actv_idx = app_ble_env.app_status >> BLE_APP_IDX_POS;
	#endif

	#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("app %s dest_id = %x,conidx:%d,con_idx:%d\r\n",__func__,dest_id,conidx,con_idx);
	bk_printf("operation = 0x%x,status = 0x%x,seq_num = 0x%x\r\n",param->operation,param->status,param->seq_num);
	#endif

	if((param->operation == GATTC_SDP_DISC_SVC_ALL))
	{
		#if CFG_INIT_ENABLE
		kernel_state_set(KERNEL_BUILD_ID(TASK_BLE_APP,con_idx),APPC_SERVICE_CONNECTED);
		#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_IMPO)
		bk_printf("\r\nAPPC_SERVICE_CONNECTED\r\n");
		#endif
		#endif
	 //   sdp_enable_all_server_ntf_ind(conidx,1);
	}
	#if CFG_INIT_ENABLE
	if(param->operation == GATTC_READ)
	{
		#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("\r\nREAD CMP EVT\r\n");
		#endif
		#if (BLE_APP_PRESENT && (BLE_CENTRAL) && (BLE_SDP_CLIENT))
		app_sdp_charac_callback_handler(CHARAC_READ_DONE,BLE_APP_INITING_GET_INDEX(con_idx),0,0,NULL);
		#endif
		if (app_ble_env.cmd == BLE_INIT_READ_CHAR) {
			app_ble_env.op_mask &= ~(1 << BLE_OP_INIT_READ_CHAR_POS);
			app_ble_next_operation(actv_idx, status);
		}
	}
	else if(param->operation == GATTC_WRITE)
	{
		#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("\r\nWrite CMP EVT\r\n");
		#endif
		#if (BLE_APP_PRESENT && (BLE_CENTRAL) && (BLE_SDP_CLIENT))
		app_sdp_charac_callback_handler(CHARAC_WRITE_DONE,BLE_APP_INITING_GET_INDEX(con_idx),0,0,NULL);
		#endif
		if (app_ble_env.cmd == BLE_INIT_WRITE_CHAR) {
			app_ble_env.op_mask &= ~(1 << BLE_OP_INIT_WRITE_CHAR_POS);
			app_ble_next_operation(actv_idx, status);
		}
	}
	#endif
	if ((param->operation == GATTC_MTU_EXCH)) {
		if (conn_idx >= BLE_CONNECTION_MAX) {
			bk_printf("unknow conn idx:%d\r\n", conn_idx);
		} else {
			app_ble_env.connections[conn_idx].conn_op_mask &= ~(1 << BLE_OP_MTU_CHANGE_POS);
			app_ble_next_operation(conn_idx, status);
		}
	}

	return (KERNEL_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

/* Default State handlers definition. */
KERNEL_MSG_HANDLER_TAB(appm)
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KERNEL_MSG_DEFAULT_HANDLER,    (kernel_msg_func_t)appm_msg_handler},

    // GAPM messages
    {GAPM_PROFILE_ADDED_IND,    (kernel_msg_func_t)gapm_profile_added_ind_handler},
    {GAPM_ACTIVITY_CREATED_IND, (kernel_msg_func_t)gapm_activity_created_ind_handler},
    {GAPM_ACTIVITY_STOPPED_IND, (kernel_msg_func_t)gapm_activity_stopped_ind_handler},
    {GAPM_CMP_EVT,              (kernel_msg_func_t)gapm_cmp_evt_handler},
    {GAPM_GEN_RAND_NB_IND,      (kernel_msg_func_t)gapm_gen_rand_nb_ind_handler},

	///{GAPM_ADV_REPORT_IND,       (kernel_msg_func_t)gapm_adv_report_ind_handler},
    #if (BLE_BROADCASTER)    
    {GAPM_SCAN_REQUEST_IND,      (kernel_msg_func_t)gapm_scan_request_ind_handler},
    #endif
    
    #if (BLE_OBSERVER || BLE_CENTRAL )
    {GAPM_EXT_ADV_REPORT_IND,   (kernel_msg_func_t)gapm_ext_adv_report_ind_handler},
    #endif    

	#if (SECURE_CONNECTIONS)
    {GAPM_GEN_DH_KEY_IND,       (kernel_msg_func_t)gapm_gen_dh_key_ind_handler},
    {GAPM_PUB_KEY_IND,          (kernel_msg_func_t)gapm_get_key_ind_handler},
	#endif

    // GAPC messages
    {GAPC_GET_DEV_INFO_REQ_IND, (kernel_msg_func_t)gapc_get_dev_info_req_ind_handler},
    {GAPC_SET_DEV_INFO_REQ_IND, (kernel_msg_func_t)gapc_set_dev_info_req_ind_handler},
    {GAPC_CONNECTION_REQ_IND,   (kernel_msg_func_t)gapc_connection_req_ind_handler},
    {GAPC_PARAM_UPDATE_REQ_IND, (kernel_msg_func_t)gapc_param_update_req_ind_handler},
    {GAPC_PARAM_UPDATED_IND,    (kernel_msg_func_t)gapc_param_updated_ind_handler},
    {GAPC_LE_PKT_SIZE_IND,      (kernel_msg_func_t)gapc_le_pkt_size_ind_handler},
    {GAPC_CMP_EVT,              (kernel_msg_func_t)gapc_cmp_evt_handler},
    {GAPC_DISCONNECT_IND,       (kernel_msg_func_t)gapc_disconnect_ind_handler},
     // GATTC messages
    #if (BLE_SDP_CLIENT)
    {GATTC_SDP_SVC_IND,         (kernel_msg_func_t)gattc_sdp_svc_ind_handler},
    {GATTC_EVENT_IND,           (kernel_msg_func_t)app_gattc_event_ind_handler},
    {GATTC_EVENT_REQ_IND,       (kernel_msg_func_t)app_gattc_event_req_ind_handler},
    #endif
    {GATTC_MTU_CHANGED_IND,     (kernel_msg_func_t)app_gattc_mtu_changed_ind_handler},
    {GATTC_CMP_EVT,             (kernel_msg_func_t)gattc_cmp_evt_handler},

	#if (BLE_CENTRAL) && (CFG_INIT_ENABLE)
	{APP_INIT_CON_DEV_TIMEROUT_TIMER,  (kernel_msg_func_t)app_init_con_dev_timerout_handler},
	{APP_INIT_INIT_EVENT,      (kernel_msg_func_t)app_init_init_event_handler},
	#if APP_INIT_STOP_CONN_TIMER_EVENT
	{APP_INIT_START_TIMEOUT_EVENT,      (kernel_msg_func_t)app_init_start_timeout_event_handler},
	#endif ///#if APP_INIT_STOP_CONN_TIMER_EVENT
	{APP_INIT_CON_SDP_END_TIMER,  (kernel_msg_func_t)app_init_con_sdp_end_timer_handler},
	#endif
};

/* Defines the place holder for the states of all the task instances. */
kernel_state_t appm_state[APP_IDX_MAX];

// Application task descriptor
const struct kernel_task_desc TASK_BLE_DESC_APP = {appm_msg_handler_tab, appm_state, APP_IDX_MAX, ARRAY_LEN(appm_msg_handler_tab)};

#endif //(BLE_APP_PRESENT)

/// @} APPTASK
