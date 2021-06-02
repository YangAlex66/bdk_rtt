#include "rwip_config.h"     // SW configuration

#if (BLE_APP_COMM)
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <string.h>
#include "app_comm.h"                //  Application Module Definitions
#include "app_ble.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
#include "comm_task.h"               // health thermometer functions
#include "common_bt.h"
#include "prf_types.h"               // Profile common types definition
#include "architect.h"                    // Platform Definitions
#include "prf.h"
#include "comm.h"
#include "attm.h"
#include "kernel_timer.h"
#include "ble_ui.h"

static uint8_t bk_ble_get_prf_by_id(uint16_t id, struct prf_task_env **env)
{
    uint8_t status = PRF_APP_ERROR;
    uint32_t i;
    for(i = 0; i < BLE_NB_PROFILES ; i++)
    {
        if(prf_env.prf[i].id == id)
        {
            *env = &(prf_env.prf[i]);
            status = GAP_ERR_NO_ERROR;
            break;
        }
    }

    return status;
}

ble_err_t bk_ble_create_db (struct bk_ble_db_cfg* ble_db_cfg)
{
    ble_err_t ret = ERR_SUCCESS;
    bk_printf("ble create new db\r\n");

    if (kernel_state_get(TASK_BLE_APP) == APPM_READY)
    {
        struct bk_ble_db_cfg *db_cfg;

        struct gapm_profile_task_add_cmd *req = KERNEL_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                  TASK_BLE_GAPM, TASK_BLE_APP,
                                                  gapm_profile_task_add_cmd, sizeof(struct bk_ble_db_cfg));
        // Fill message
        req->operation = GAPM_PROFILE_TASK_ADD;
        req->sec_lvl = ble_db_cfg->svc_perm;
        req->prf_task_id = TASK_BLE_ID_COMMON + ble_db_cfg->prf_task_id;
        req->app_task = TASK_BLE_APP;
		req->role = PRF_SERVICE;
		req->start_hdl = 0; //dynamically allocated

        req->start_hdl = ble_db_cfg->start_hdl; //req->start_hdl = 0; dynamically allocated

        //Set parameters
        db_cfg = (struct bk_ble_db_cfg* ) req->param;
        memcpy(db_cfg, ble_db_cfg, sizeof(struct bk_ble_db_cfg));

        kernel_state_set(TASK_BLE_APP, APPM_CREATE_DB);
        //Send the message
        kernel_msg_send(req);
    }
    else
    {
        ret = ERR_CREATE_DB;
    }

    return ret;
}

static int app_comm_msg_dflt_handler(kernel_msg_id_t const msgid,
                                     void const *param,
                                     kernel_task_id_t const dest_id,
                                     kernel_task_id_t const src_id)
{
    // Drop the message
    return (KERNEL_MSG_CONSUMED);
}

ble_err_t bk_ble_send_ntf_value(uint32_t len, uint8_t *buf, uint16_t prf_id, uint16_t att_idx)
{
    ble_err_t ret = ERR_SUCCESS;
    uint16_t prf_task_id = prf_id + TASK_BLE_ID_COMMON;
    struct prf_task_env *prf_env = NULL;

    uint8_t status = bk_ble_get_prf_by_id(prf_task_id, &prf_env);

    if(status == GAP_ERR_NO_ERROR)
    {
        // Allocate the message
        struct bk_ble_ntf_upd_req * req = KERNEL_MSG_ALLOC_DYN(BK_BLE_NTF_UPD_REQ,
                                                        prf_env->task,
                                                        TASK_BLE_APP,
                                                        bk_ble_ntf_upd_req,len);

        req->length = len;
        memcpy(req->value, buf, len);
        req->att_id = att_idx;
        req->conidx = 0;

        kernel_msg_send(req);
    }
    else
    {
        ret = ERR_PROFILE;
    }

    return ret;
}

ble_err_t bk_ble_send_ind_value(uint32_t len, uint8_t *buf, uint16_t prf_id, uint16_t att_idx)
{
	ble_err_t ret = ERR_SUCCESS;
	uint16_t prf_task_id = prf_id + TASK_BLE_ID_COMMON;
	struct prf_task_env *prf_env = NULL;

	uint8_t status = bk_ble_get_prf_by_id(prf_task_id, &prf_env);

	if (status == GAP_ERR_NO_ERROR) {
		// Allocate the message
		struct bk_ble_ind_upd_req * req = KERNEL_MSG_ALLOC_DYN(BK_BLE_IND_UPD_REQ,
									prf_env->task,
									TASK_BLE_APP,
									bk_ble_ind_upd_req,len);

		req->length = len;
		memcpy(req->value, buf, len);
		req->att_id = att_idx;
		req->conidx = 0;

		kernel_msg_send(req);
	} else {
		ret = ERR_PROFILE;
	}

	return ret;
}

static int bk_ble_write_req_ind_handler(kernel_msg_id_t const msgid,
							struct bk_ble_write_ind *param,
							kernel_task_id_t const dest_id,
							kernel_task_id_t const src_id)
{
	write_req_t write_req;

	write_req.conn_idx = app_ble_find_conn_idx_handle(param->conidx);
	write_req.att_idx = param->att_id;
	write_req.len = param->length;
	write_req.prf_id = param->prf_id;
	write_req.value = &(param->value[0]);

	if (ble_event_notice)
		ble_event_notice(BLE_5_WRITE_EVENT, &write_req);

	return (KERNEL_MSG_CONSUMED);
}

static int bk_ble_gattc_cmp_evt_handler(kernel_msg_id_t const msgid,  struct bk_ble_gattc_cmp_evt const *param,
                                 kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
	uint8_t conn_idx = (app_ble_env.app_status >> BLE_APP_IDX_POS);
	uint8_t status = (param->status == GAP_ERR_NO_ERROR) ? ERR_SUCCESS : ERR_CMD_RUN;

	if( param->operation == GATTC_INDICATE || param->operation == GATTC_NOTIFY) {
		bk_printf("[%s]\r\n",__FUNCTION__);
		if (ble_event_notice)
			ble_event_notice(BLE_5_TX_DONE, NULL);
		app_ble_next_operation(conn_idx, status);
	}
	return KERNEL_MSG_CONSUMED;
}

const struct kernel_msg_handler app_comm_msg_handler_list[] =
{
	// Note: first message is latest message checked by kernel so default is put on top.
	{KERNEL_MSG_DEFAULT_HANDLER,         (kernel_msg_func_t)app_comm_msg_dflt_handler},
	{BK_BLE_WRITE_REQ_IND,               (kernel_msg_func_t)bk_ble_write_req_ind_handler},
	{BK_BLE_GATTC_CMP_EVT,               (kernel_msg_func_t)bk_ble_gattc_cmp_evt_handler},
};

const struct app_subtask_handlers app_comm_table_handler =
	{&app_comm_msg_handler_list[0], (sizeof(app_comm_msg_handler_list)/sizeof(struct kernel_msg_handler))};

#endif

