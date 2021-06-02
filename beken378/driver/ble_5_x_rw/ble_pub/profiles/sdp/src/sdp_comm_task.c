#include "rwip_config.h"             // SW configuration

#if (BLE_APP_PRESENT && (BLE_CENTRAL) && (BLE_SDP_CLIENT))
#include "sdp_comm.h"
#include "sdp_comm_pub.h"
#include "sdp_comm_task.h"
#include "app_task.h"
#include "gattc_task.h"
#include "common_utils.h"
#include "app_sdp.h"
#include "app_ble.h"

extern struct app_env_tag app_ble_env;
/**
*******************************************************************************
*********
 * @brief Handles reception of all messages sent from the lower layers to the
application
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
*******************************************************************************
*********
 */
static int sdp_default_msg_handler(kernel_msg_id_t const msgid,
                            void *param,
                            kernel_task_id_t const dest_id,
                            kernel_task_id_t const src_id)
{
	uint8_t conidx = KERNEL_IDX_GET(src_id);

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]SDP msgid:0x%x,dest_id:0x%x,src_id:0x%x\r\n",__func__,msgid,dest_id,src_id);
	bk_printf("conidx:%x\r\n",conidx);
#endif
	return (KERNEL_MSG_CONSUMED);
}
/*
* TYPE DEFINITIONS
****************************************************************************************
*/
/*
* GLOBAL FUNCTIONS DEFINITIONS
****************************************************************************************
*/
/**
****************************************************************************************
* @brief Handles reception of the @ref BASC_ENABLE_REQ message.
* The handler enables the NTF Service Client Role.
* @param[in] msgid Id of the message received (probably unused).
* @param[in] param Pointer to the parameters of the message.
* @param[in] dest_id ID of the receiving task instance (probably unused).
* @param[in] src_id ID of the sending task instance.
* @return If the message was consumed or not.
****************************************************************************************
*/
static int sdp_enable_req_handler(kernel_msg_id_t const msgid,
									struct sdp_enable_req const *param,
									kernel_task_id_t const dest_id,
									kernel_task_id_t const src_id)
{
	// Status
	int msg_status = KERNEL_MSG_CONSUMED;

	return (msg_status);
}
/**
****************************************************************************************
* @brief Handles reception of the @ref SDP_READ_INFO_REQ message.
* @param[in] msgid Id of the message received (probably unused).
* @param[in] param Pointer to the parameters of the message.
* @param[in] dest_id ID of the receiving task instance (probably unused).
* @param[in] src_id ID of the sending task instance.
* @return If the message was consumed or not.
****************************************************************************************
*/
static int sdp_read_info_req_handler(kernel_msg_id_t const msgid,
									struct sdp_read_info_req const *param,
									kernel_task_id_t const dest_id,
									kernel_task_id_t const src_id)
{
	int msg_status = KERNEL_MSG_CONSUMED;
	uint8_t con_idx = BLE_APP_INITING_GET_INDEX(KERNEL_IDX_GET(src_id));
	uint8_t char_idx;
	uint16_t handle = param->handle;
	uint8_t desc_nb;
	struct sdp_env_tag *sdp_env = NULL;
	uint8_t conhdl = app_ble_env.connections[con_idx].conhdl;

	#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]con_idx:%d,conidx:%d\r\n",__func__,con_idx,conhdl);
	#endif
	if(SDPC_OPERATE_HANDLE == param->type)
	{
		#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("prf_read_char_send \r\n");
		#endif
		sdp_env =(struct sdp_env_tag *) prf_env_get_from_handle(conhdl,handle);
		if(sdp_env != NULL)
		{
			prf_read_char_send(&(sdp_env->prf_env), conhdl,handle, handle, handle);
		}else{
			bk_printf("[%s]sdp_env == NULL\r\n",__FUNCTION__);
		}
	}
	else if(SDPC_OPERATE_UUID == param->type)
	{
		sdp_env =(struct sdp_env_tag *) prf_env_get_from_uuid(conhdl,param->uuid_len,param->uuid);;
		if(sdp_env != NULL)
		{
			char_idx = sdp_env->prf_db_env->sdp_cont->char_idx;
			desc_nb = sdp_env->prf_db_env->sdp_cont->descs_nb;
			handle = sdp_env->prf_db_env->sdp_cont->chars_descs_inf.chars_inf[char_idx].val_hdl;
			uint16_t val_prop =  sdp_env->prf_db_env->sdp_cont->chars_descs_inf.chars_inf[char_idx].prop;
			uint16_t desc_uuid;

			#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
			bk_printf("desc_nb = 0x%x,char_idx = 0x%x,value_handle = 0x%x\r\n",desc_nb,char_idx,handle);
			#endif
			if(SDPC_CHAR_NTF_CFG == param->info)
			{
				for(int i = 0; i < desc_nb; i++)
				{
					desc_uuid = co_read16(sdp_env->prf_db_env->sdp_cont->chars_descs_inf.descs_inf[i].uuid);
					if((ATT_DESC_CLIENT_CHAR_CFG == desc_uuid) && (sdp_env->prf_db_env->sdp_cont->chars_descs_inf.descs_inf[i].char_code == char_idx))
					{
						handle = sdp_env->prf_db_env->sdp_cont->chars_descs_inf.descs_inf[i].desc_hdl;
						val_prop |= ATT_CHAR_PROP_RD;
						break;
					}
				}
			}
			else if(SDPC_CHAR_USER_DESC_VAL == param->info)
			{

				for(int i = 0; i < desc_nb; i++)
				{
					desc_uuid = co_read16(sdp_env->prf_db_env->sdp_cont->chars_descs_inf.descs_inf[i].uuid);
					if((ATT_DESC_CHAR_USER_DESCRIPTION == desc_uuid) && (sdp_env->prf_db_env->sdp_cont->chars_descs_inf.descs_inf[i].char_code == char_idx))
					{
						handle = sdp_env->prf_db_env->sdp_cont->chars_descs_inf.descs_inf[i].desc_hdl;;
						val_prop |= ATT_CHAR_PROP_RD;
						break;
					}
				}
			}

			#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
			bk_printf("prop = 0x%x,char_idx = 0x%x\r\n",val_prop,char_idx);
			bk_printf("handle = 0x%x\r\n",handle);
			#endif
			if((val_prop & ATT_CHAR_PROP_RD) == ATT_CHAR_PROP_RD ) // ATT_CHAR_PROP_RD
			{
				#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
				bk_printf("prf_read_char_send \r\n");
				#endif
				prf_read_char_send(&(sdp_env->prf_env), conhdl,
				handle, handle, handle);
			}
			else
			{
				bk_printf("val_prop not read prop!!!! \r\n");
			}
		}
		else
		{
			bk_printf("param unvalid uuid =  0x%02x\r\n",param->uuid);
		}
	}

	return (msg_status);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref BASC_BATT_LEVEL_NTF_CFG_REQ message.
 * It allows configuration of the peer ntf/stop characteristic for Battery Level Characteristic.
 * Will return an error code if that cfg char does not exist.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int sdp_write_ntf_cfg_req_handler(kernel_msg_id_t const msgid,
		struct sdp_write_ntf_cfg_req const *param,
		kernel_task_id_t const dest_id,
		kernel_task_id_t const src_id)
{
	int msg_status = KERNEL_MSG_CONSUMED;
	uint8_t conidx = param->conidx;
	uint8_t char_idx;

	// put value in air format
	#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]param->handle = %x\r\n",__func__,param->handle);
	#endif
	struct sdp_env_tag *sdp_env =(struct sdp_env_tag *) prf_env_get_from_handle(conidx,param->handle + 1);
	if(sdp_env != NULL)
	{
		char_idx = sdp_env->prf_db_env->sdp_cont->char_idx;
		///uint16_t uuid = co_read16p(sdp_env->prf_db_env->sdp_cont->chars_descs_inf.descs_inf[char_idx].uuid);
		uint16_t uuid = sdp_env->prf_db_env->sdp_cont->chars_descs_inf.descs_inf[char_idx].uuid[0] | 
							(sdp_env->prf_db_env->sdp_cont->chars_descs_inf.descs_inf[char_idx].uuid[1] << 8);
		#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("ntf handle = 0x%x\r\n",param->handle + 1);
		#endif
		if(uuid == ATT_DESC_CLIENT_CHAR_CFG)
		{
			prf_gatt_write_ntf_ind(&(sdp_env->prf_env), conidx,  param->handle + 1, param->ntf_cfg);///,param->seq_num);
		}
		else
		{
			bk_printf("ntf_ind not cfg char :  0x%04x\r\n",uuid);
		}
	}
	else
	{
		bk_printf("param unvalid handle =  0x%02x\r\n",param->handle);
	}

	return (msg_status);
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @ref BASC_BATT_LEVEL_NTF_CFG_REQ message.
 * It allows configuration of the peer ntf/stop characteristic for Battery Level Characteristic.
 * Will return an error code if that cfg char does not exist.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int sdp_write_value_info_req_handler(kernel_msg_id_t const msgid,
		struct sdp_write_info_req const *param,
		kernel_task_id_t const dest_id,
		kernel_task_id_t const src_id)
{
	int msg_status = KERNEL_MSG_CONSUMED;
	uint8_t conidx = param->conidx;
	uint8_t char_idx;
	struct sdp_env_tag *sdp_env =(struct sdp_env_tag *) prf_env_get_from_handle(conidx,param->handle);

	if(sdp_env != NULL)
	{
		char_idx = sdp_env->prf_db_env->sdp_cont->char_idx;
		uint16_t handle = sdp_env->prf_db_env->sdp_cont->chars_descs_inf.chars_inf[char_idx].val_hdl;
		uint16_t length = param->length;
		uint16_t val_prop =  sdp_env->prf_db_env->sdp_cont->chars_descs_inf.chars_inf[char_idx].prop;
		uint8_t *buf = (uint8_t *)(&param->data[0]);
		uint8_t operation = GATTC_WRITE_NO_RESPONSE;

		//	UART_PRINTF("handle = 0x%x\r\n",handle);
		if((val_prop & ATT_CHAR_PROP_WR_NO_RESP) == ATT_CHAR_PROP_WR_NO_RESP)
		{
			operation = GATTC_WRITE_NO_RESPONSE;
		}
		//else if(((val_prop & ATT_CHAR_PROP_WR) == ATT_CHAR_PROP_WR)&& (operation == GATTC_WRITE)) // ATT_CHAR_PROP_WR
		else if((val_prop & ATT_CHAR_PROP_WR) == ATT_CHAR_PROP_WR)
		{
			operation = GATTC_WRITE;
		}
		else
		{
			uint8_t con_idx = BLE_APP_INITING_GET_INDEX(KERNEL_IDX_GET(src_id));
			bk_printf("param operation or val_prop error(conidx:%d) =  0x%02x,0x%02x\r\n",con_idx,operation,val_prop);
			app_ble_next_operation(con_idx,ERR_ATTC_WRITE);
			return (msg_status);
		}
		#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("operation = %x\r\n",operation);
		#endif
		prf_gatt_write(&(sdp_env->prf_env),conidx,handle, buf,length,operation);///,0);
	}
	else
	{
		uint8_t con_idx = BLE_APP_INITING_GET_INDEX(KERNEL_IDX_GET(src_id));
		bk_printf("param unvalid handle(conidx:%d) =  0x%02x\r\n",con_idx,param->handle);
		app_ble_next_operation(con_idx,ERR_ATTC_WRITE_UNREGISTER);
	}

	return (msg_status);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_CMP_EVT message.
 * This generic event is received for different requests, so need to keep track.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int sdp_task_gattc_cmp_evt_handler(kernel_msg_id_t const msgid,
								 struct gattc_cmp_evt const *param,
								 kernel_task_id_t const dest_id,
								 kernel_task_id_t const src_id)
{
	uint8_t state = kernel_state_get(dest_id);
	uint8_t conhdl = KERNEL_IDX_GET(src_id);
	uint8_t conidx = app_ble_find_conn_idx_handle(conhdl);

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]SDP dest_id = %x,conidx:%d,conhdl:%d\r\n",__func__,dest_id,conidx,conhdl);
	bk_printf("sdp operation = 0x%x,status = 0x%x,seq_num = 0x%x\r\n",param->operation,param->status,param->seq_num);
#endif
	struct gattc_cmp_evt *cmp_evt  = KERNEL_MSG_ALLOC(GATTC_CMP_EVT,
											KERNEL_BUILD_ID(TASK_BLE_APP, BLE_APP_INITING_INDEX(conidx)),
											dest_id,
											gattc_cmp_evt);
	cmp_evt->operation	= param->operation;
	cmp_evt->status = param->status;
	cmp_evt->seq_num = param->seq_num ;

	kernel_msg_send(cmp_evt);

	return (KERNEL_MSG_CONSUMED);
}
/**
****************************************************************************************
* @brief Handles reception of the @ref GATTC_READ_IND message.
* Generic event received after every simple read command sent to peer server.
* @param[in] msgid Id of the message received (probably unused).
* @param[in] param Pointer to the parameters of the message.
* @param[in] dest_id ID of the receiving task instance (probably unused).
* @param[in] src_id ID of the sending task instance.
* @return If the message was consumed or not.
****************************************************************************************
*/
static int sdp_task_gattc_read_ind_handler(kernel_msg_id_t const msgid,
										struct gattc_read_ind const *param,
										kernel_task_id_t const dest_id,
										kernel_task_id_t const src_id)
{
	uint8_t state = kernel_state_get(dest_id);
	uint8_t conhdl = KERNEL_IDX_GET(src_id);
	uint8_t conidx = app_ble_find_conn_idx_handle(conhdl);

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_ALL)
	bk_printf("appm %s \r\n",__func__);
	bk_printf("handle = 0x%x\r\n",param->handle);
	bk_printf("length = 0x%x\r\n",param->length);
	bk_printf("offset = 0x%x\r\n",param->offset);
	bk_printf("value = 0x");
	for(int i =0 ; i < param->length; i++)
	{
		bk_printf("%x ",param->value[i]);
	}
	bk_printf("\r\n");
#endif

	app_sdp_charac_callback_handler(CHARAC_READ,conidx,param->handle,param->length,(uint8*)param->value);

	return (KERNEL_MSG_CONSUMED);
}

/**
****************************************************************************************
* @brief Handles reception of the @ref GATTC_EVENT_IND message.
* @param[in] msgid Id of the message received (probably unused).
* @param[in] param Pointer to the parameters of the message.
* @param[in] dest_id ID of the receiving task instance (probably unused).
* @param[in] src_id ID of the sending task instance.
* @return If the message was consumed or not.
****************************************************************************************
*/
static int sdp_gattc_event_ind_handler(kernel_msg_id_t const msgid,
								   struct gattc_event_ind const *param,
								   kernel_task_id_t const dest_id,
								   kernel_task_id_t const src_id)
{
	uint8_t state = kernel_state_get(dest_id);
	uint8_t conhdl = KERNEL_IDX_GET(src_id);
	uint8_t conidx = app_ble_find_conn_idx_handle(conhdl);

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_ALL)
	bk_printf("[%s]sdp gattc_event_ind :%d:%d,conidx:%d\r\n",__func__, dest_id, src_id,conidx);
	bk_printf("NOTIF RECIVE length:%d,value = 0x",param->length);
	for(int i = 0; i< param->length; i++)
	{
		bk_printf("%02x ",param->value[i]);
	}
	bk_printf("\r\n");
#endif
	struct gattc_event_ind *ind;

	// allocate indication
	ind = KERNEL_MSG_ALLOC_DYN(GATTC_EVENT_IND,
				KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx)),
				dest_id,
				gattc_event_ind, param->length);

	// fill in parameters
	ind->type = GATTC_NOTIFY;
	ind->length = param->length;

	ind->handle = param->handle;
	memcpy(ind->value, param->value, param->length);

#if (BLE_SYNC_TIME_EN)  ///1128
	/// Information about the latest found synchronization
	ind->basetime_cnt = param->basetime_cnt;
	ind->finetime_cnt = param->finetime_cnt;
	ind->evt_cnt = param->evt_cnt;
	ind->rxrssi = param->rxrssi;
	ind->usedchidx = param->usedchidx;
#endif

	/* send the message to app*/
	kernel_msg_send(ind);

	return (KERNEL_MSG_CONSUMED);
}

static int sdp_gattc_event_req_ind_handler(kernel_msg_id_t const msgid,
									struct gattc_event_ind const *param,
									kernel_task_id_t const dest_id,
									kernel_task_id_t const src_id)
{
	uint8_t state = kernel_state_get(dest_id);
	uint8_t conhdl = KERNEL_IDX_GET(src_id);
	uint8_t conidx = app_ble_find_conn_idx_handle(conhdl);

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_ALL)
	bk_printf("[%s]sdp conidx:%d,conhdl:%d indicate RECIVE value = 0x",__func__,conidx,conhdl);
	for(int i = 0; i< param->length; i++)
	{
		bk_printf("%02x ",param->value[i]);
	}
	bk_printf("\r\n");
#endif

	struct gattc_event_ind *ind;
	// allocate indication
	ind = KERNEL_MSG_ALLOC_DYN(GATTC_EVENT_REQ_IND ,
								KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx)),
								dest_id,
								gattc_event_ind,param->length);

	// fill in parameters
	ind->type = GATTC_INDICATE;
	ind->length = param->length;

#if (BLE_SYNC_TIME_EN)  ///1128
	/// Information about the latest found synchronization
	ind->basetime_cnt = param->basetime_cnt;
	ind->finetime_cnt = param->finetime_cnt;
	ind->evt_cnt = param->evt_cnt;
	ind->rxrssi = param->rxrssi;
	ind->usedchidx = param->usedchidx;
#endif

	ind->handle = param->handle;
	memcpy(ind->value, param->value, param->length);

	/* send the message to app*/
	kernel_msg_send(ind);

	struct gattc_event_cfm *cfm	= KERNEL_MSG_ALLOC(GATTC_EVENT_CFM, src_id, dest_id, gattc_event_cfm);

	cfm->handle = param->handle;

	kernel_msg_send(cfm);

	return (KERNEL_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
/// Default State handlers definition
//const struct ke_msg_handler sdp_default_state[] =
///KERNEL_MSG_HANDLER_TAB(sdp)
const struct kernel_msg_handler sdp_msg_handler_tab[] =
{
	{KERNEL_MSG_DEFAULT_HANDLER,        (kernel_msg_func_t)sdp_default_msg_handler},
	{SDP_ENABLE_REQ,                    (kernel_msg_func_t)sdp_enable_req_handler},
	{SDP_READ_INFO_REQ,                 (kernel_msg_func_t)sdp_read_info_req_handler},
	{SDP_WRITE_VALUE_INFO_REQ,          (kernel_msg_func_t)sdp_write_value_info_req_handler},
	{SDP_WRITE_NTF_CFG_REQ,             (kernel_msg_func_t)sdp_write_ntf_cfg_req_handler},
	{GATTC_READ_IND,                    (kernel_msg_func_t)sdp_task_gattc_read_ind_handler},
	{GATTC_EVENT_IND,                   (kernel_msg_func_t)sdp_gattc_event_ind_handler},
	{GATTC_EVENT_REQ_IND,               (kernel_msg_func_t)sdp_gattc_event_req_ind_handler},
	{GATTC_CMP_EVT,                     (kernel_msg_func_t)sdp_task_gattc_cmp_evt_handler},////mark
};

void sdp_task_init(struct kernel_task_desc *task_desc,void* sdp_env)
{
	// Get the address of the environment
	///struct sdp_env_tag *srv_env = PRF_ENV_GET(SDP, sdp);
	struct sdp_env_tag *srv_env =  (struct sdp_env_tag *)sdp_env;
	bk_printf("[sdp_task_init]conidx:%d,srv_env:%p,task_id:%d\r\n",srv_env->conidx,srv_env,(TASK_BLE_ID_SDP + srv_env->conidx));

	task_desc->msg_handler_tab = sdp_msg_handler_tab;
	task_desc->msg_cnt         = ARRAY_LEN(sdp_msg_handler_tab);
	task_desc->state           = srv_env->state;
	task_desc->idx_max         = SDP_IDX_MAX;
}

#endif

