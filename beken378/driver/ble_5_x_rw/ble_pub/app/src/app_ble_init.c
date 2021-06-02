#include "rwip_config.h"             // SW configuration


#if ((BLE_APP_PRESENT) && (BLE_CENTRAL) && CFG_INIT_ENABLE)
#include "rwapp_config.h"
#include <string.h>

#include "rwip.h"
#include "app_ble_init.h"
#include "app_sdp.h"
#include "sdp_comm_pub.h"

#include "kernel_timer.h"
#include "gap.h"                     // GAP Definition
#include "gapm_task.h"               // GAP Manager Task API
#include "gapc_task.h"               // GAP Controller Task API
#include "gattc_task.h"
///#include "co_bt.h"                   // Common BT Definition
///#include "co_math.h"                 // Common Maths Definition
//#include "sdp_service_task.h"
//#include "sdp_service.h"
//#include "prf_utils.h"

#include "ble_api_5_x.h" ///#include "ble_api.h"
#include "app_ble.h"
#include "app_ble_init.h"

#define APP_BLE_INIT_CHECK_CONN_IDX(conn_idx)	if (conn_idx >= BLE_CONNECTION_MAX) {\
													bk_printf("[%s]unknow conn_idx:%d\r\n",__FUNCTION__,conn_idx);\
													return ERR_UNKNOW_IDX;\
												}

extern struct app_env_tag app_ble_env;

#if APP_INIT_REUSE_ACTV_IDX
app_ble_initing_env_t app_ble_initing_env = {0};

void app_ble_initing_init(void)
{
	int i;

	for(i=0;i<BLE_CONNECTION_MAX;i++){
		appm_set_initing_actv_idx(i,UNKNOW_CONN_HDL,BLE_INIT_IDX_NONE);
	}
}

void appm_set_initing_actv_idx(unsigned char conidx,unsigned char actv_idx,unsigned char state)
{
	if(conidx >= BLE_CONNECTION_MAX){
		bk_printf("[%s]conidx:%d error\r\n",__FUNCTION__,conidx);
		return;
	}
	#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]conidx:%d actv_idx:%d,state:%d\r\n",__FUNCTION__,conidx,actv_idx,state);
	#endif
	app_ble_initing_env.init_idx[conidx].actv_idx = actv_idx;
	app_ble_initing_env.init_idx[conidx].state = state;
}

unsigned char appm_get_stop_init_actv_idx(void)
{
	int i;

	for(i = 0; i < BLE_CONNECTION_MAX; i++){
		if(app_ble_initing_env.init_idx[i].actv_idx != UNKNOW_CONN_HDL){
			if(app_ble_initing_env.init_idx[i].state == BLE_INIT_IDX_STOPED){
				app_ble_initing_env.init_idx[i].state = BLE_INIT_IDX_USED;
				return app_ble_initing_env.init_idx[i].actv_idx;
			}
		}
	}

	return UNKNOW_CONN_HDL;
}
#endif

int appm_set_connect_dev_addr(unsigned char connidx,struct bd_addr *bdaddr,unsigned char addr_type)
{
	APP_BLE_INIT_CHECK_CONN_IDX(connidx);
	memcpy(app_ble_env.connections[connidx].peer_addr.addr,bdaddr->addr,BD_ADDR_LEN);
	app_ble_env.connections[connidx].peer_addr_type = addr_type;
	return 0;
}

struct bd_addr *appm_get_connect_dev_addr(unsigned char connidx)
{
	return &app_ble_env.connections[connidx].peer_addr;
}

unsigned char appm_get_connect_dev_addr_type(unsigned char connidx)
{
	return app_ble_env.connections[connidx].peer_addr_type;
}


ble_err_t set_app_ble_master_conn_dev_timeout(unsigned char con_idx,unsigned int n_10mS)
{
	APP_BLE_INIT_CHECK_CONN_IDX(con_idx);
	app_ble_env.connections[con_idx].u.master.conn_dev_to = n_10mS;
	return ERR_SUCCESS;
}

void app_ble_create_initing(uint8_t con_idx)
{
	// Prepare the GAPM_ACTIVITY_CREATE_CMD message
	struct gapm_activity_create_cmd *p_cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD,
	                                                      TASK_BLE_GAPM,
	                                                      KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(con_idx)),
	                                                      gapm_activity_create_cmd);

	// Set operation code
	p_cmd->operation = GAPM_CREATE_INIT_ACTIVITY;

	// Fill the allocated kernel message
	p_cmd->own_addr_type = GAPM_STATIC_ADDR;

	// Keep the current operation
	BLE_APP_MASTER_SET_IDX_STATE(con_idx,APP_INIT_STATE_CREATING);

	// Send the message
	kernel_msg_send(p_cmd);
}

ble_err_t appm_create_initing(uint8_t con_idx,unsigned short con_interval,
						unsigned short con_latency,unsigned short sup_to)
{
	APP_BLE_INIT_CHECK_CONN_IDX(con_idx);
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]init_state:%d\r\n",__func__,BLE_APP_MASTER_GET_IDX_STATE(con_idx));
#endif

	if (BLE_APP_MASTER_GET_IDX_STATE(con_idx) == APP_INIT_STATE_IDLE)
	{
		// And the next expected operation code for the command completed event
		app_ble_env.connections[con_idx].conhdl = USED_CONN_HDL;
		app_ble_env.connections[con_idx].con_interval = (con_interval) ? con_interval : APP_CONN_INTV;
		app_ble_env.connections[con_idx].con_latency = (con_latency) ? con_latency : APP_CONN_LATENCY;
		app_ble_env.connections[con_idx].sup_to = (sup_to) ? sup_to : APP_CONN_SUP_TO;
		app_ble_env.connections[con_idx].u.master.scan_intv = APP_CONN_SCAN_INTV;
		app_ble_env.connections[con_idx].u.master.scan_wd = APP_CONN_SCAN_WD;
		app_ble_env.connections[con_idx].u.master.conn_dev_to = APP_CONN_DRV_TO;
		app_ble_env.connections[con_idx].conn_op_mask = 1 << BLE_OP_CREATE_INIT_POS;
		app_ble_env.connections[con_idx].conn_op_cb = NULL;
		app_ble_env.connections[con_idx].con_interval += (con_idx * APP_CONN_IND_DELT_TIME);
	#if APP_INIT_REUSE_ACTV_IDX
		unsigned char unused_init_actv = appm_get_stop_init_actv_idx();
		#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("unused_init_actv:%x\r\n", unused_init_actv);
		#endif
		if(unused_init_actv == UNKNOW_CONN_HDL){
			app_ble_create_initing(con_idx);
		}else{
			app_ble_env.connections[con_idx].gap_actv_idx = unused_init_actv;
			struct app_task_event_ind *p_cmd = KERNEL_MSG_ALLOC(APP_INIT_INIT_EVENT,
	                                                      KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(con_idx)),
	                                                      KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(con_idx)),
	                                                      app_task_event_ind);
			p_cmd->type = 0;
			kernel_msg_send(p_cmd);
		}
	#else
		app_ble_create_initing(con_idx);
	#endif
		return ERR_SUCCESS;
	}
	else
	{
		bk_printf("connections[%d] is not idle\r\n", con_idx);
		return ERR_INIT_STATE;
	}
}

ble_err_t appm_start_connecting(uint8_t con_idx)
{
	APP_BLE_INIT_CHECK_CONN_IDX(con_idx);
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s],init_state:%d\r\n",__func__,BLE_APP_MASTER_GET_IDX_STATE(con_idx));
#endif

	int ret = ERR_SUCCESS;

	struct bd_addr *bdaddr = appm_get_connect_dev_addr(con_idx);
	unsigned char addr_type = appm_get_connect_dev_addr_type(con_idx);

	if (BLE_APP_MASTER_GET_IDX_STATE(con_idx) == APP_INIT_STATE_CREATED)
	{
		// Prepare the GAPM_ACTIVITY_START_CMD message
		struct gapm_activity_start_cmd *p_cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_START_CMD,
		                                         TASK_BLE_GAPM,
		                                         KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(con_idx)),
		                                         gapm_activity_start_cmd);

		p_cmd->operation = GAPM_START_ACTIVITY;
		p_cmd->actv_idx = app_ble_env.connections[con_idx].gap_actv_idx;

		p_cmd->u_param.init_param.type = GAPM_INIT_TYPE_DIRECT_CONN_EST;///GAPM_INIT_TYPE_NAME_DISC;///GAPM_INIT_TYPE_AUTO_CONN_EST;///GAPM_INIT_TYPE_DIRECT_CONN_EST;

		p_cmd->u_param.init_param.prop = GAPM_INIT_PROP_1M_BIT;

		p_cmd->u_param.init_param.conn_param_1m.ce_len_min = 10;
		p_cmd->u_param.init_param.conn_param_1m.ce_len_max = 20;
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("conn_intv:%d,conn_latency:%d,conn_super_to:%d\r\n",
			app_ble_env.connections[con_idx].gap_actv_idx,app_ble_env.connections[con_idx].con_latency,app_ble_env.connections[con_idx].sup_to);
#endif
		p_cmd->u_param.init_param.conn_param_1m.conn_intv_min = app_ble_env.connections[con_idx].con_interval;
		p_cmd->u_param.init_param.conn_param_1m.conn_intv_max = app_ble_env.connections[con_idx].con_interval;
		p_cmd->u_param.init_param.conn_param_1m.conn_latency = app_ble_env.connections[con_idx].con_latency;
		p_cmd->u_param.init_param.conn_param_1m.supervision_to = app_ble_env.connections[con_idx].sup_to;

		p_cmd->u_param.init_param.conn_to = 1000;
		///p_cmd->u_param.init_param.peer_addr.addr_type = 0;   ///ADDR_PUBLIC
		p_cmd->u_param.init_param.peer_addr.addr_type = addr_type;
		//  {{0x0f, 0x43, 0x45, 0x67, 0x89, 0xAB}}
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("con address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
			bdaddr->addr[0], bdaddr->addr[1], bdaddr->addr[2],
			bdaddr->addr[3], bdaddr->addr[4], bdaddr->addr[5]);
#endif

		memcpy(p_cmd->u_param.init_param.peer_addr.addr.addr,bdaddr->addr,GAP_BD_ADDR_LEN);

		p_cmd->u_param.init_param.scan_param_1m.scan_intv = app_ble_env.connections[con_idx].u.master.scan_intv;
		p_cmd->u_param.init_param.scan_param_1m.scan_wd = app_ble_env.connections[con_idx].u.master.scan_wd;

		/////Keep the current operation
		BLE_APP_MASTER_SET_IDX_STATE(con_idx,APP_INIT_STATE_WAIT_CONECTTING);
		app_ble_env.connections[con_idx].conn_op_mask = 1 << BLE_OP_INIT_START_POS;
		app_ble_env.connections[con_idx].conn_op_cb = NULL;
		////BLE_APP_MASTER_SET_IDX_CMD_SET_STATE(BLE_OP_INIT_START_POS,con_idx,callback);

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("conn_dev_to:%d\r\n",app_ble_env.connections[con_idx].u.master.conn_dev_to);
#endif
#if (APP_INIT_SET_STOP_CONN_TIMER && (!APP_INIT_STOP_CONN_TIMER_EVENT))
		kernel_timer_set(APP_INIT_CON_DEV_TIMEROUT_TIMER,
							KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(con_idx)),
							app_ble_env.connections[con_idx].u.master.conn_dev_to);
#elif (APP_INIT_SET_STOP_CONN_TIMER && (APP_INIT_STOP_CONN_TIMER_EVENT))
		struct app_task_start_timeout_event_ind *pt_cmd = KERNEL_MSG_ALLOC(APP_INIT_START_TIMEOUT_EVENT,
		                                                      KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(con_idx)),
		                                                      KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(con_idx)),
		                                                      app_task_start_timeout_event_ind);
		pt_cmd->timout_ms = app_ble_env.connections[con_idx].u.master.conn_dev_to;
		pt_cmd->task_id = KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(con_idx));
		kernel_msg_send(pt_cmd);
#endif
		///// Send the message
		kernel_msg_send(p_cmd);
	}
	else
	{
		ret = ERR_INIT_STATE;
	}

	return ret;
}

ble_err_t appm_stop_connencting(uint8_t con_idx)
{
	uint8_t ret = ERR_SUCCESS;

	APP_BLE_INIT_CHECK_CONN_IDX(con_idx);
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]status:%d\r\n",__func__,BLE_APP_MASTER_GET_IDX_STATE(con_idx));
#endif

	if (BLE_APP_MASTER_GET_IDX_STATE(con_idx) == APP_INIT_STATE_CONECTTING)
	{
		/////Prepare the GAPM_ACTIVITY_STOP_CMD message
		struct gapm_activity_stop_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_STOP_CMD,
													  TASK_BLE_GAPM,
													  KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(con_idx)),
													  gapm_activity_stop_cmd);

		/////Fill the allocated kernel message
		cmd->operation = GAPM_STOP_ACTIVITY;
		cmd->actv_idx = app_ble_env.connections[con_idx].gap_actv_idx;
		app_ble_env.connections[con_idx].conn_op_mask = 1 << BLE_OP_INIT_STOP_POS;
		app_ble_env.connections[con_idx].conn_op_cb = NULL;

		/////Update advertising state
		BLE_APP_MASTER_SET_IDX_STATE(con_idx,APP_INIT_STATE_STOPPING);

		/////Send the message
		kernel_msg_send(cmd);
	}
	else
	{
		ret = ERR_INIT_STATE;
	}

	return ret;
}

int app_ble_master_appm_disconnect(uint8_t conidx)
{
	APP_BLE_INIT_CHECK_CONN_IDX(conidx);
	return app_ble_disconnect(conidx, COMMON_ERROR_REMOTE_USER_TERM_CON);
}
#endif


