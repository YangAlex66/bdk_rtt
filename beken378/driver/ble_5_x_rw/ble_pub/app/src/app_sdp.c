#include "rwip_config.h"             // SW configuration


#if (BLE_APP_PRESENT && (BLE_CENTRAL) && (BLE_SDP_CLIENT))
#include "app_sdp.h"
#include "rwapp_config.h"
#include "sdp_comm.h"
#include "sdp_comm_pub.h"
#include "sdp_comm_task.h"
#include "app_task.h"
#include "string.h"
#include "stdlib.h"
#include "app_ble.h"


app_sdp_env_tag app_sdp_env = {0};
extern struct app_env_tag app_ble_env;
void register_app_sdp_characteristic_callback(app_sdp_callback cb)
{
	app_sdp_env.sdp_cb = cb;
}

void register_app_sdp_charac_callback(app_sdp_charac_callback cb)
{
	app_sdp_env.charac_cb = cb;
}

void register_app_sdp_service_tab(unsigned char service_tab_nb,app_sdp_service_uuid *service_tab)
{
	app_sdp_env.service_tab_nb = service_tab_nb;
	app_sdp_env.service_tab = service_tab;
}

void app_sdp_service_filtration(int en)
{
	app_sdp_env.filtration = (en > 0) ? 1 : 0;
}

void app_sdp_characteristic_callback_handler(unsigned char conidx,uint16_t chars_val_hdl,unsigned char uuid_len,unsigned char *uuid)
{
	if (app_sdp_env.sdp_cb)
	{
		app_sdp_env.sdp_cb(conidx,chars_val_hdl,uuid_len,uuid);
	}
}

void app_sdp_charac_callback_handler(CHAR_TYPE type,uint8 conidx,uint16_t hdl,uint16_t len,uint8 *data)
{
	if (app_sdp_env.charac_cb)
	{
		app_sdp_env.charac_cb(type,conidx,hdl,len,data);
	}
}

////////////////////////////////////////////////////////////////
uint8_t app_sdp_add_element_srv(uint8_t conidx,struct gattc_sdp_svc_ind const *ind)
{
	struct prf_sdp_db_env *prf_db_env = NULL;
	unsigned char stus = 1;
	int i;
	uint8_t conhdl = app_ble_env.connections[conidx].conhdl;

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]conidx:%d,conhdl:%d,service UUID:0x%02x%02x\r\n",__func__,conidx,conhdl,ind->uuid[1],ind->uuid[0]);
#endif

	if (app_sdp_env.filtration)
	{
		for (i=0; i<app_sdp_env.service_tab_nb; i++)
		{
			if ((app_sdp_env.service_tab[i].uuid_len == ind->uuid_len)
				&&(memcmp((unsigned char*)&app_sdp_env.service_tab[i].uuid,ind->uuid,ind->uuid_len) == 0))
			{
				break;
			}
		}

		if (i < app_sdp_env.service_tab_nb)
		{
			app_sdp_service_uuid svr_uuid;
			svr_uuid.uuid_len = (ind->uuid_len <= ATT_UUID_128_LEN) ? ind->uuid_len : ATT_UUID_128_LEN;
			memcpy(svr_uuid.uuid,ind->uuid,svr_uuid.uuid_len);
			sdp_notice_event(SDP_NTC_FILTRATION_SVR,NULL);
			return -1;
		}
	}

	prf_db_env = sdp_extract_svc_info(conhdl,ind);
	if(prf_db_env != NULL)
	{
		#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("char nb:%d\r\n",prf_db_env->sdp_cont->chars_nb);
		#endif
		for(i = 0;i < prf_db_env->sdp_cont->chars_nb;i++)
		{
			uint16_t val_hdl = prf_db_env->sdp_cont->chars_descs_inf.chars_inf[i].val_hdl;
			#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
			bk_printf("char valhdl:%d[0x%02x]\r\n",val_hdl,val_hdl);
			#endif
			app_sdp_characteristic_callback_handler(conidx,val_hdl,
				prf_db_env->sdp_cont->chars_descs_inf.chars_inf[i].uuid_len,
				prf_db_env->sdp_cont->chars_descs_inf.chars_inf[i].uuid);
		}

		sdp_add_profiles(conidx,prf_db_env);
		stus = 0;
	}

	return  stus;
}


#define APPM_ERROR_NO_ERROR    0
#define APPM_ERROR_STATE       (0xFF)

uint8_t appc_write_service_data_req(uint8_t conidx,uint16_t handle,uint16_t data_len,uint8_t *data)
{
	if(BLE_CONNECTION_MAX <= conidx){
		return APPM_ERROR_STATE;
	}
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]state = %x,",__func__,kernel_state_get(KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx))));
	bk_printf("conidx:%d,handle = 0x%04x\r\n",conidx,handle);
#endif
	uint8_t ret = 0;
	uint8_t conhdl = app_ble_env.connections[conidx].conhdl;
	struct prf_sdp_db_env *sdp_db_env;
	sdp_db_env = sdp_profiles_sdp_db_env(conhdl,handle);

	if((kernel_state_get(KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx))) == APPC_SERVICE_CONNECTED) && sdp_db_env)
	{
		struct sdp_write_info_req *req = KERNEL_MSG_ALLOC_DYN(SDP_WRITE_VALUE_INFO_REQ,
										prf_get_task_from_id(KERNEL_BUILD_ID((TASK_BLE_ID_SDP+sdp_db_env->prf_idx),0)),
										KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx)),
										sdp_write_info_req, data_len);

		// Fill in the parameter structure
		req->conidx = conhdl;
		req->handle = handle;
		req->length = data_len;
		memcpy(req->data,data,data_len);
		#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("dest = 0x%04x\r\n",prf_get_task_from_id(KERNEL_BUILD_ID((TASK_BLE_ID_SDP+conhdl),0)));
		bk_printf("data len:%d,",data_len);
		for(int i = 0;i<data_len;i++)
		{
			bk_printf("%02x ",data[i]);
		}
		bk_printf("\r\n");
		#endif

		// Send the message
		kernel_msg_send(req);
	}
	else
	{
		ret = APPM_ERROR_STATE;
		#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_IMPO)
		bk_printf("kernel_state_get(TASK_BLE_APP) = %x\r\n",kernel_state_get(KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx))));
		#endif
	}

	return ret;
}

uint8_t appc_write_service_ntf_cfg_req(uint8_t conidx,uint16_t handle,uint16_t ntf_cfg,uint16_t seq_num)
{
	if(BLE_CONNECTION_MAX <= conidx){
		return APPM_ERROR_STATE;
	}
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]state = %x,",__func__,kernel_state_get(KERNEL_BUILD_ID(TASK_BLE_APP,conidx)));
	bk_printf("conidx:%d,handle = 0x%04x,ntf_cfg = 0x%x\r\n",conidx,handle,ntf_cfg);
#endif
	uint8_t ret = APPM_ERROR_NO_ERROR;
	uint8_t conhdl = app_ble_env.connections[conidx].conhdl;
	struct prf_sdp_db_env *sdp_db_env;
	sdp_db_env = sdp_profiles_sdp_db_env(conhdl,handle);

	if((kernel_state_get(KERNEL_BUILD_ID(TASK_BLE_APP,conidx)) != APPC_LINK_IDLE) && sdp_db_env)
	{
		struct sdp_write_ntf_cfg_req *req = KERNEL_MSG_ALLOC(SDP_WRITE_NTF_CFG_REQ,
											prf_get_task_from_id(KERNEL_BUILD_ID((TASK_BLE_ID_SDP+sdp_db_env->prf_idx),0)),
											KERNEL_BUILD_ID(TASK_BLE_APP,conidx),
											sdp_write_ntf_cfg_req);
	#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("dest = 0x%04x\r\n",prf_get_task_from_id(KERNEL_BUILD_ID((TASK_BLE_ID_SDP+conhdl),0)));
	#endif
		// Fill in the parameter structure
		req->conidx = conidx;
		req->handle = handle;
		req->ntf_cfg = ntf_cfg;
		req->seq_num = seq_num;
		// Send the message
		kernel_msg_send(req);
	}
	else
	{
		ret = APPM_ERROR_STATE;
		#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_IMPO)
		bk_printf("kernel_state_get(TASK_APPC) = %x\r\n",kernel_state_get(KERNEL_BUILD_ID(TASK_BLE_APP,conidx)));
		#endif
	}

	return ret;
}

uint8_t appm_read_service_data_by_uuid_req(uint8_t conidx,uint8_t uuid_len,uint8_t* uuid)
{
	if(BLE_CONNECTION_MAX <= conidx){
		return APPM_ERROR_STATE;
	}
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]\r\n",__func__);
#endif
	uint8_t ret = APPM_ERROR_NO_ERROR;
	uint8_t conhdl = app_ble_env.connections[conidx].conhdl;
	struct prf_sdp_db_env *sdp_db_env;

	sdp_db_env = sdp_profiles_sdp_db_env_from_uuid(conhdl,uuid_len,uuid);

	if((kernel_state_get(KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx))) == APPC_SERVICE_CONNECTED) && sdp_db_env)
	{
		struct sdp_read_info_req *req = KERNEL_MSG_ALLOC(SDP_READ_INFO_REQ,
											prf_get_task_from_id(KERNEL_BUILD_ID((TASK_BLE_ID_SDP+sdp_db_env->prf_idx),0)),
											KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx)),
											sdp_read_info_req);
		// Fill in the parameter structure
		req->uuid_len = uuid_len;
		memcpy(req->uuid,uuid,uuid_len);

		req->info = SDPC_CHAR_VAL;
		req->type = SDPC_OPERATE_UUID;
		// Send the message
		kernel_msg_send(req);
	}
	else
	{
		ret = APPM_ERROR_STATE;
		#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_IMPO)
		bk_printf("kernel_state_get(TASK_BLE_APP) = %x\r\n",kernel_state_get(TASK_BLE_APP));
		#endif
	}

    return ret;
}

uint8_t appm_read_service_data_by_handle_req(uint8_t conidx,uint16_t handle)
{
	if(BLE_CONNECTION_MAX <= conidx){
		return APPM_ERROR_STATE;
	}

	uint8_t ret = APPM_ERROR_NO_ERROR;
	uint8_t conhdl = app_ble_env.connections[conidx].conhdl;
	struct prf_sdp_db_env *sdp_db_env = NULL;

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]conidx:%d,conhdl:%d,handle:0x%x,status:0x%x\r\n",__func__,conidx,conhdl,handle,KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx)));
#endif
	sdp_db_env = sdp_profiles_sdp_db_env(conhdl,handle);

	if( (kernel_state_get(KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx))) == APPC_SERVICE_CONNECTED)
		&& (sdp_db_env != NULL))
	{
		struct sdp_read_info_req *req = KERNEL_MSG_ALLOC(SDP_READ_INFO_REQ,
										prf_get_task_from_id(KERNEL_BUILD_ID((TASK_BLE_ID_SDP+sdp_db_env->prf_idx),0)),
										KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx)),
										sdp_read_info_req);
		// Fill in the parameter structure
		req->handle = handle;
		req->info = SDPC_CHAR_VAL;
		req->type = SDPC_OPERATE_HANDLE;

		// Send the message
		kernel_msg_send(req);
	}
	else
	{
		ret = APPM_ERROR_STATE;
		#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_IMPO)
		bk_printf("kernel_state_get(TASK_BLE_APP) = %x,sdp_db_env:%x\r\n",kernel_state_get(KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx))),sdp_db_env);
		#endif
	}

    return ret;
}

uint8_t appm_read_service_ntf_ind_cfg_by_handle_req(uint8_t conidx,uint16_t handle)
{
	if(BLE_CONNECTION_MAX <= conidx){
		return APPM_ERROR_STATE;
	}
	uint8_t ret = APPM_ERROR_NO_ERROR;
	uint8_t conhdl = app_ble_env.connections[conidx].conhdl;
	struct prf_sdp_db_env *sdp_db_env;

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]\r\n",__func__);
#endif
	sdp_db_env = sdp_profiles_sdp_db_env(conhdl,handle);
	if((kernel_state_get(KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx))) == APPC_SERVICE_CONNECTED) && sdp_db_env)
	{
		struct sdp_read_info_req *req = KERNEL_MSG_ALLOC(SDP_READ_INFO_REQ,
										prf_get_task_from_id(KERNEL_BUILD_ID((TASK_BLE_ID_SDP+sdp_db_env->prf_idx),0)),
										KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx)),
										sdp_read_info_req);
		// Fill in the parameter structure
		req->handle = handle;
		req->info = SDPC_CHAR_NTF_CFG;
		req->type = SDPC_OPERATE_HANDLE;
		// Send the message
		kernel_msg_send(req);
	}
	else
	{
		ret = APPM_ERROR_STATE;
		#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_IMPO)
		bk_printf("kernel_state_get(TASK_BLE_APP) = %x\r\n",kernel_state_get(KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx))));
		#endif
	}

	return ret;
}

uint8_t appm_read_service_userDesc_by_handle_req(uint8_t conidx,uint16_t handle)
{
	if(BLE_CONNECTION_MAX <= conidx){
		return APPM_ERROR_STATE;
	}
	uint8_t ret = APPM_ERROR_NO_ERROR;
	uint8_t conhdl = app_ble_env.connections[conidx].conhdl;
	struct prf_sdp_db_env *sdp_db_env;

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]\r\n",__func__);
#endif
	sdp_db_env = sdp_profiles_sdp_db_env(conhdl,handle);
	if((kernel_state_get(KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx))) == APPC_SERVICE_CONNECTED) && sdp_db_env)
	{
		struct sdp_read_info_req *req = KERNEL_MSG_ALLOC(SDP_READ_INFO_REQ,
										prf_get_task_from_id(KERNEL_BUILD_ID((TASK_BLE_ID_SDP+sdp_db_env->prf_idx),0)),
										KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx)),
										sdp_read_info_req);

		// Fill in the parameter structure
		req->handle = handle;
		req->info = SDPC_CHAR_USER_DESC_VAL;
		req->type = SDPC_OPERATE_HANDLE;

		// Send the message
		kernel_msg_send(req);
	}
	else
	{
		ret = APPM_ERROR_STATE;
		#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_IMPO)
		bk_printf("kernel_state_get(TASK_BLE_APP) = %x\r\n",kernel_state_get(KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx))));
		#endif
	}

	return ret;
}


#endif

