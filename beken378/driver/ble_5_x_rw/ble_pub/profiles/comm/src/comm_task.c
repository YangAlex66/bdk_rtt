#include "rwip_config.h"

#if (BLE_COMM_SERVER)

#include "gap.h"
#include "gattc_task.h"
#include "attm.h"
#include "atts.h"
#include "common_utils.h"
#include "kernel_mem.h"
#include "comm.h"
#include "comm_task.h"

#include "prf_utils.h"
#include "ble.h"
#include "ble_pub.h"
#include "ble_ui.h"
#include "app_ble.h"
#include "ble_api_5_x.h"

extern struct prf_env_tag prf_env;;

static uint8_t bk_ble_get_prf_by_handler(uint16_t handler, struct prf_task_env **env)
{
    uint8_t status = PRF_APP_ERROR;
    struct bk_ble_env_tag* ble_env = NULL;
    uint32_t i;
    for(i = 0; i < BLE_NB_PROFILES ; i++)
    {
        ble_env = (struct bk_ble_env_tag*)(prf_env.prf[i].env);
        if(((ble_env->start_hdl) <= handler) && ((ble_env->start_hdl + ble_env->att_db_nb) >= handler))
        {
            *env = &(prf_env.prf[i]);
            status = GAP_ERR_NO_ERROR;
            break;
        }
    }

    return status;
}

static uint8_t bk_ble_get_prf_by_task(kernel_task_id_t task, struct prf_task_env **env)
{
    uint8_t status = PRF_APP_ERROR;
    uint32_t i;
    for(i = 0; i < BLE_NB_PROFILES ; i++)
    {
        if(prf_env.prf[i].task == task)
        {
            *env = &(prf_env.prf[i]);
            status = GAP_ERR_NO_ERROR;
            break;
        }
    }

    return status;
}

static void bk_ble_ntf_val(struct bk_ble_env_tag* ble_env,struct bk_ble_ntf_upd_req const *param)
{
	
	//  UART_PRINTF("%s\r\n",__func__);
    //Allocate the GATT notification message
    struct gattc_send_evt_cmd *ntf_value = KERNEL_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
            KERNEL_BUILD_ID(TASK_BLE_GATTC, 0), prf_src_task_get(&(ble_env->prf_env),0),
            gattc_send_evt_cmd, param->length);

    //Fill in the parameter structure
    ntf_value->operation = GATTC_NOTIFY;
    ntf_value->handle = ble_env->start_hdl + param->att_id;
    // pack measured value in databaseS
    ntf_value->length = param->length;
    ntf_value->seq_num = param->att_id;
  
    //UART_PRINTF("fed6_value->handle = 0x%x\r\n",fed6_value->handle);
    memcpy(&ntf_value->value[0],param->value,ntf_value->length);

    //send notification to peer device
    kernel_msg_send(ntf_value);
}

static int bk_ble_ntf_upd_req_handler(kernel_msg_id_t const msgid,
                                            struct bk_ble_ntf_upd_req const *param,
                                            kernel_task_id_t const dest_id,
                                            kernel_task_id_t const src_id)
{
    int msg_status = KERNEL_MSG_CONSUMED;
    uint8_t state = kernel_state_get(dest_id);

    struct prf_task_env *prf_env = NULL;
    struct bk_ble_env_tag* ble_env = NULL;
    
    // retrieve handle information
    uint8_t status = bk_ble_get_prf_by_task(dest_id, &prf_env);
    // retrieve handle information
    ble_env = (struct bk_ble_env_tag*)(prf_env->env);
    if(GAP_ERR_NO_ERROR == status)
    {
        // check state of the task
        if(state == BLE_IDLE)
        {	
            // update the battery level value
            kernel_state_set(dest_id, BLE_BUSY);       

            bk_ble_ntf_val(ble_env, param);
            //	ke_state_set(dest_id, FEB3S_IDLE);   
            msg_status =  KERNEL_MSG_CONSUMED;							       
        }
        else
        {
            //UART_PRINTF("KE_MSG_SAVED6\r\n");
            msg_status = KERNEL_MSG_SAVED;
        }
    }

    return (msg_status);

}

static void bk_ble_ind_val(struct bk_ble_env_tag* ble_env,struct bk_ble_ind_upd_req const *param)
{
	
	//  UART_PRINTF("%s\r\n",__func__);
    //Allocate the GATT notification message
    struct gattc_send_evt_cmd *ntf_value = KERNEL_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
            KERNEL_BUILD_ID(TASK_BLE_GATTC, 0), prf_src_task_get(&(ble_env->prf_env),0),
            gattc_send_evt_cmd, param->length);

    //Fill in the parameter structure
    ntf_value->operation = GATTC_INDICATE;
    ntf_value->handle = ble_env->start_hdl + param->att_id;
    // pack measured value in databaseS
    ntf_value->length = param->length;
    ntf_value->seq_num = param->att_id;
  
    //UART_PRINTF("fed6_value->handle = 0x%x\r\n",fed6_value->handle);
    memcpy(&ntf_value->value[0],param->value,ntf_value->length);

    //send notification to peer device
    kernel_msg_send(ntf_value);
}

static int bk_ble_ind_upd_req_handler(kernel_msg_id_t const msgid,
                                            struct bk_ble_ind_upd_req const *param,
                                            kernel_task_id_t const dest_id,
                                            kernel_task_id_t const src_id)
{
    int msg_status = KERNEL_MSG_CONSUMED;
    uint8_t state = kernel_state_get(dest_id);

    struct prf_task_env *prf_env = NULL;
    struct bk_ble_env_tag* ble_env = NULL;
    
    // retrieve handle information
    uint8_t status = bk_ble_get_prf_by_task(dest_id, &prf_env);
    // retrieve handle information
    ble_env = (struct bk_ble_env_tag*)(prf_env->env);
    if(GAP_ERR_NO_ERROR == status)
    {
        // check state of the task
        if(state == BLE_IDLE)
        {	
            // update the battery level value
            kernel_state_set(dest_id, BLE_BUSY);       

            bk_ble_ind_val(ble_env, param);
            //	ke_state_set(dest_id, FEB3S_IDLE);   
            msg_status =  KERNEL_MSG_CONSUMED;							       
        }
        else
        {
            //UART_PRINTF("KE_MSG_SAVED6\r\n");
            msg_status = KERNEL_MSG_SAVED;
        }
    }

    return (msg_status);

}

static int gattc_att_info_req_ind_handler(kernel_msg_id_t const msgid,
							struct gattc_att_info_req_ind *param,
							kernel_task_id_t const dest_id,
							kernel_task_id_t const src_id)
{
	struct gattc_att_info_cfm * cfm;
	att_info_req_t att_info;
	// retrieve handle information
	struct prf_task_env *prf_env = NULL;
	struct bk_ble_env_tag* ble_env = NULL;

	// retrieve handle information
	uint8_t status = bk_ble_get_prf_by_handler(param->handle, &prf_env);
	uint8_t conidx = KERNEL_IDX_GET(src_id);

	ble_env = (struct bk_ble_env_tag*)(prf_env->env);
	att_info.att_idx = param->handle - ble_env->start_hdl;
	att_info.prf_id = prf_env->id - TASK_BLE_ID_COMMON;
	att_info.conn_idx = app_ble_find_conn_idx_handle(conidx);
	//Send write response
	cfm = KERNEL_MSG_ALLOC(GATTC_ATT_INFO_CFM, src_id, dest_id, gattc_att_info_cfm);
	cfm->handle = param->handle;

	if (status == GAP_ERR_NO_ERROR) {
		if (ble_event_notice)
			ble_event_notice(BLE_5_ATT_INFO_REQ, &att_info);
	}

	cfm->length = att_info.length;
	cfm->status = att_info.status;
	kernel_msg_send(cfm);

	return (KERNEL_MSG_CONSUMED);
}

static int gattc_read_req_ind_handler(kernel_msg_id_t const msgid, struct gattc_read_req_ind const *param,
						kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
	struct gattc_read_cfm * cfm;
	read_req_t read_req;

	struct prf_task_env *prf_env = NULL;
	struct bk_ble_env_tag* ble_env = NULL;

	// retrieve handle information
	uint8_t status = bk_ble_get_prf_by_handler(param->handle, &prf_env);
	uint8_t conidx = KERNEL_IDX_GET(src_id);

	read_req.conn_idx = app_ble_find_conn_idx_handle(conidx);
	read_req.value = kernel_malloc(BLE_CHAR_DATA_LEN, KERNEL_MEM_KERNEL_MSG);
	read_req.size = BLE_CHAR_DATA_LEN;
	ble_env = (struct bk_ble_env_tag*)(prf_env->env);
	read_req.att_idx = param->handle - ble_env->start_hdl;
	read_req.prf_id = prf_env->id - TASK_BLE_ID_COMMON;
	// If the attribute has been found, status is GAP_ERR_NO_ERROR
	if (status == GAP_ERR_NO_ERROR) {
		if (ble_event_notice)
			ble_event_notice(BLE_5_READ_EVENT, &read_req);
	}

	cfm = KERNEL_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, read_req.length);

	cfm->length = read_req.length;
	cfm->handle = param->handle;
	cfm->status = status;
	memcpy(cfm->value,read_req.value,cfm->length);

	kernel_msg_send(cfm);

	kernel_free(read_req.value);

	return (KERNEL_MSG_CONSUMED);
}

static int gattc_write_req_ind_handler(kernel_msg_id_t const msgid, struct gattc_write_req_ind const *param,
							kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
	struct gattc_write_cfm * cfm;
	struct prf_task_env *prf_env = NULL;
	struct bk_ble_env_tag* ble_env = NULL;

	// retrieve handle information
	uint8_t status = bk_ble_get_prf_by_handler(param->handle, &prf_env);
	uint8_t conidx = KERNEL_IDX_GET(src_id);
	// retrieve handle information
	ble_env = (struct bk_ble_env_tag*)(prf_env->env);

	// If the attribute has been found, status is GAP_ERR_NO_ERROR
	if (status == GAP_ERR_NO_ERROR) {
		// Allocate the alert value change indication
		struct bk_ble_write_ind *ind = KERNEL_MSG_ALLOC_DYN(BK_BLE_WRITE_REQ_IND,
									prf_dst_task_get(&(ble_env->prf_env), conidx),
									dest_id, bk_ble_write_ind, param->length);

		// Fill in the parameter structure
		memcpy(ind->value,&param->value[0],param->length);
		ind->att_id = param->handle - ble_env->start_hdl;;
		ind->prf_id = prf_env->id - TASK_BLE_ID_COMMON;
		ind->conidx = conidx;
		ind->length = param->length;
		// Send the message
		kernel_msg_send(ind);
	}

	//Send write response
	cfm = KERNEL_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id, gattc_write_cfm);
	cfm->handle = param->handle;
	cfm->status = status;
	kernel_msg_send(cfm);

	return (KERNEL_MSG_CONSUMED);
}

static int gattc_cmp_evt_handler(kernel_msg_id_t const msgid,  struct gattc_cmp_evt const *param,
                                 kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
  	//UART_PRINTF("%s\r\n",__func__);
  	struct prf_task_env *prf_env = NULL;
    struct bk_ble_env_tag* ble_env = NULL;
    
    // retrieve handle information
    uint8_t status = bk_ble_get_prf_by_task(dest_id, &prf_env);
    // retrieve handle information
    ble_env = (struct bk_ble_env_tag*)(prf_env->env);

    if (status == GAP_ERR_NO_ERROR)
    {
        // continue operation execution
		struct bk_ble_gattc_cmp_evt *evt = KERNEL_MSG_ALLOC(BK_BLE_GATTC_CMP_EVT,
        prf_dst_task_get(&(ble_env->prf_env), 0),
        dest_id, bk_ble_gattc_cmp_evt);
			
		evt->operation = param->operation;
		evt->status = param->status;
        evt->prf_id = prf_env->id - TASK_BLE_ID_COMMON;
		evt->att_id = param->seq_num;
				
		kernel_state_set(dest_id, BLE_IDLE); 
		kernel_msg_send(evt);					
    }
		
    return (KERNEL_MSG_CONSUMED);
}

/// Default State handlers definition
const struct kernel_msg_handler bk_ble_default_handler[] =
{
    {BK_BLE_NTF_UPD_REQ,            (kernel_msg_func_t) bk_ble_ntf_upd_req_handler},
    {BK_BLE_IND_UPD_REQ,            (kernel_msg_func_t) bk_ble_ind_upd_req_handler},
    {GATTC_ATT_INFO_REQ_IND,        (kernel_msg_func_t) gattc_att_info_req_ind_handler},
    {GATTC_READ_REQ_IND,            (kernel_msg_func_t) gattc_read_req_ind_handler},
    {GATTC_WRITE_REQ_IND,           (kernel_msg_func_t) gattc_write_req_ind_handler},
    {GATTC_CMP_EVT,                 (kernel_msg_func_t) gattc_cmp_evt_handler},
};

void comm_task_init(struct kernel_task_desc *task_desc, kernel_state_t *state)
{
    // Get the address of the environment
    task_desc->msg_handler_tab = bk_ble_default_handler;
    task_desc->msg_cnt         = ARRAY_LEN(bk_ble_default_handler);
    task_desc->state           = state;
    task_desc->idx_max         = BLE_CONN_IDX_MAX;
}

#endif
