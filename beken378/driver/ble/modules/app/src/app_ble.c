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
#include "include.h"

#include "rwip_config.h"             // SW configuration

#if (BLE_APP_PRESENT)
#include <string.h>
#include "rwapp_config.h"
#include "app_task.h"                // Application task Definition
#include "application.h"                     // Application Definition
#include "gap.h"                     // GAP Definition
#include "gapm_task.h"               // GAP Manager Task API
#include "gapc_task.h"               // GAP Controller Task API

#include "common_bt.h"                   // Common BT Definition
#include "common_math.h"                 // Common Maths Definition
#include "kernel_timer.h"

#include "mem_pub.h"
#include "user_config.h"

#if (BLE_MESH)
#include "app_mesh.h"
#include "common_math.h"
#endif

#include "ble.h"
#include "ble_pub.h"
#include "ble_api.h"
#include "gapm_task.h"
#include "mesh_log.h"
char app_dflt_dev_name[APP_DEVICE_NAME_LENGTH_MAX] = {0};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef void (*appm_add_svc_func_t)(void);

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// List of service to add in the database
enum appm_svc_list
{
#if (BLE_MESH)
    APPM_SVC_MESH,
#endif

    APPM_SVC_LIST_STOP ,
};

/*
 * LOCAL VARIABLES DEFINITIONS
 ****************************************************************************************
 */

/// Application Task Descriptor
static const struct kernel_task_desc TASK_BLE_DESC_APP = {NULL, &appm_default_handler,
                                                  appm_state, APPM_STATE_MAX, APP_IDX_MAX};

adv_info_t adv_info = {
    .channel_map = 7,
    .interval_min = APP_ADV_INT_MIN,
    .interval_max = APP_ADV_INT_MAX
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Application Environment Structure
struct app_env_tag app_env;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void appm_init()
{
    // Reset the application manager environment
    memset(&app_env, 0, sizeof(app_env));

    // Create APP task
    kernel_task_create(TASK_BLE_APP, &TASK_BLE_DESC_APP);

    // Initialize Task state
    kernel_state_set(TASK_BLE_APP, APPM_INIT);

    /*------------------------------------------------------
     * INITIALIZE ALL MODULES
     *------------------------------------------------------*/

#if (BLE_MESH)
	{
	    uint8_t key_len = KEY_LEN;
	    if (nvds_get(NVDS_TAG_LOC_IRK, &key_len, app_env.loc_irk) != NVDS_OK)
	    {
	        uint8_t counter;

	        // generate a new IRK
	        for (counter = 0; counter < KEY_LEN; counter++)
	        {
	            app_env.loc_irk[counter]    = (uint8_t)common_rand_word();
	        }

	        // Store the generated value in NVDS
	        if (nvds_put(NVDS_TAG_LOC_IRK, KEY_LEN, (uint8_t *)&app_env.loc_irk) != NVDS_OK)
	        {
	            //ASSERT_INFO(0, NVDS_TAG_LOC_IRK, 0);
	        }
	    }
	    app_mesh_init();
	}
#endif
		
}

/*设备主动断开连接函数*/
void appm_disconnect(void)
{
	if (kernel_state_get(TASK_BLE_APP) == APPM_CONNECTED)
	{
        struct gapc_disconnect_cmd *cmd = KERNEL_MSG_ALLOC(GAPC_DISCONNECT_CMD,
                                                       KERNEL_BUILD_ID(TASK_BLE_GAPC, app_env.conidx), TASK_BLE_APP,
                                                       gapc_disconnect_cmd);

        cmd->operation = GAPC_DISCONNECT;
        cmd->reason    = COMMON_ERROR_REMOTE_USER_TERM_CON;

        // Send the message
        kernel_msg_send(cmd);
	}
}

/**
 ****************************************************************************************
 * Advertising Functions
 ****************************************************************************************
 */


/* 设备发起定向广播函数*/
void appm_start_direct_dvertising(void)
{
    appm_start_advertising();	
	
    return;
	// Check if the advertising procedure is already is progress
    if (kernel_state_get(TASK_BLE_APP) == APPM_READY)
    {
        struct gapm_start_advertise_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_START_ADVERTISE_CMD,
                                                            TASK_BLE_GAPM, TASK_BLE_APP,
                                                            gapm_start_advertise_cmd);

        cmd->op.addr_src    = GAPM_STATIC_ADDR;
        cmd->channel_map    = APP_ADV_CHMAP;

        cmd->intv_min = APP_ADV_FAST_INT;
        cmd->intv_max = APP_ADV_FAST_INT;

        cmd->op.code        = GAPM_ADV_UNDIRECT;
		
        cmd->info.host.mode = GAP_GEN_DISCOVERABLE;
		 
		/*
		 * If the peripheral is already bonded with a central device, use the direct advertising
		 * procedure (BD Address of the peer device is stored in NVDS.
		 */
		if(app_sec_get_bond_status())
		{
			#if (NVDS_SUPPORT)
			uint8_t bd_len = NVDS_LEN_PEER_BD_ADDRESS;
			#endif
			cmd->op.code   = GAPM_ADV_DIRECT_LDC;
			//cmd->info.direct.addr_type = 1;
			
	
			#if (NVDS_SUPPORT)
			if (nvds_get(NVDS_TAG_PEER_BD_ADDRESS, &bd_len,
	            		(uint8_t *)cmd->info.direct.addr.addr) != NVDS_OK)
			{
			    // An error has occurred during access to the NVDS
			    BLE_ASSERT_INFO(0,NVDS_TAG_PEER_BD_ADDRESS,bd_len);
			}
			#endif

			kernel_msg_send(cmd);

#if !(SYSTEM_SLEEP)
			kernel_msg_send_basic(APP_PERIOD_TIMER,TASK_BLE_APP,TASK_BLE_APP);
#endif

		//	UART_PRINTF("appm start direct advertising\r\n");
		}
		else
		{
			kernel_msg_free(kernel_param2msg(cmd));
            appm_start_advertising();		
 		//	UART_PRINTF("appm start general advertising\r\n");
		}

	    kernel_state_set(TASK_BLE_APP, APPM_ADVERTISING);	
	}		
}


extern /*const */struct bd_addr common_default_bdaddr;

/* 设备发起广播函数*/
ble_err_t appm_start_advertising(void)
{
    ble_err_t status = ERR_SUCCESS;
#if !(SYSTEM_SLEEP)
    kernel_msg_send_basic(APP_PERIOD_TIMER,TASK_BLE_APP,TASK_BLE_APP);
#endif

    if(BLE_ROLE_MASTER == ble_get_role_mode())
    {
        bk_printf("current role should not be master\r\n");
        return ERR_ADV_FAIL;
    }

    // Check if the advertising procedure is already is progress
    if (kernel_state_get(TASK_BLE_APP) == APPM_READY)
    {				  
        // Prepare the GAPM_START_ADVERTISE_CMD message
        struct gapm_start_advertise_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_START_ADVERTISE_CMD,
                                                            TASK_BLE_GAPM, TASK_BLE_APP,
                                                            gapm_start_advertise_cmd);

        cmd->op.addr_src    = GAPM_STATIC_ADDR;
        cmd->channel_map    = adv_info.channel_map;
        cmd->intv_min 		= adv_info.interval_min;
        cmd->intv_max 		= adv_info.interval_max;	
        cmd->op.code        = GAPM_ADV_UNDIRECT;
		
        cmd->info.host.mode = GAP_GEN_DISCOVERABLE;

        cmd->info.host.adv_filt_policy = ADV_ALLOW_SCAN_ANY_CON_ANY;

 		/*-----------------------------------------------------------------------------------
         * Set the Advertising Data and the Scan Response Data
         *---------------------------------------------------------------------------------*/
        // Flag value is set by the GAP
        cmd->info.host.adv_data_len       = adv_info.advDataLen;
		os_memcpy(cmd->info.host.adv_data, adv_info.advData, cmd->info.host.adv_data_len);
        cmd->info.host.scan_rsp_data_len  = adv_info.respDataLen;
		os_memcpy(cmd->info.host.scan_rsp_data, adv_info.respData, cmd->info.host.scan_rsp_data_len);
        
        
        // Send the message
        kernel_msg_send(cmd);
        bk_printf("appm start advertising\r\n");

        ble_init_over = 1;
        ble_set_role_mode(BLE_ROLE_SLAVE);

        // Set the state of the task to APPM_ADVERTISING
        kernel_state_set(TASK_BLE_APP, APPM_ADVERTISING);	
    }
    else
    {
        status = ERR_ADV_FAIL;
    }
    return status;
    // else ignore the request
}

/* 设备主动停止广播函数*/
ble_err_t appm_stop_advertising(void)
{
	
	ble_err_t status = ERR_SUCCESS;
    
    if (kernel_state_get(TASK_BLE_APP) == APPM_ADVERTISING)
    {
        // Go in ready state
        kernel_state_set(TASK_BLE_APP, APPM_READY);

        // Prepare the GAPM_CANCEL_CMD message
        struct gapm_cancel_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_CANCEL_CMD,
                                                   TASK_BLE_GAPM, TASK_BLE_APP,
                                                   gapm_cancel_cmd);
        cmd->operation = GAPM_CANCEL;

        ble_set_role_mode(BLE_ROLE_NONE);

        // Send the message
        kernel_msg_send(cmd);	
    }
    else
    {
        status = ERR_STOP_ADV_FAIL;	
    }
	
    return  status;
    // else ignore the request
}

void appm_update_adv_data( uint8_t* adv_buff, uint8_t adv_len, uint8_t* scan_buff, uint8_t scan_len)
{  
    if (kernel_state_get(TASK_BLE_APP) == APPM_ADVERTISING
        && (adv_len <= ADV_DATA_LEN) && (scan_len <= ADV_DATA_LEN))  
    {
        struct gapm_update_advertise_data_cmd *cmd =  KERNEL_MSG_ALLOC(
            GAPM_UPDATE_ADVERTISE_DATA_CMD,
            TASK_BLE_GAPM,
            TASK_BLE_APP,
            gapm_update_advertise_data_cmd);

        cmd->operation = GAPM_UPDATE_ADVERTISE_DATA;
        cmd->adv_data_len = adv_len;
        cmd->scan_rsp_data_len = scan_len;

        //memcpy
        memcpy(&cmd->adv_data[0], adv_buff, adv_len);
        memcpy(&cmd->scan_rsp_data[0], scan_buff, scan_len);

        // Send the message
        kernel_msg_send(cmd);
    }
}

void appm_update_param(struct gapc_conn_param *conn_param)
{
    // Prepare the GAPC_PARAM_UPDATE_CMD message
    struct gapc_param_update_cmd *cmd = KERNEL_MSG_ALLOC(GAPC_PARAM_UPDATE_CMD,
                                                     KERNEL_BUILD_ID(TASK_BLE_GAPC, app_env.conidx), TASK_BLE_APP,
                                                     gapc_param_update_cmd);

    cmd->operation  = GAPC_UPDATE_PARAMS;
    cmd->intv_min   = conn_param->intv_min;
    cmd->intv_max   = conn_param->intv_max;
    cmd->latency    = conn_param->latency;
    cmd->time_out   = conn_param->time_out;

    // not used by a slave device
    cmd->ce_len_min = 0xFFFF;
    cmd->ce_len_max = 0xFFFF;

    // Send the message
    kernel_msg_send(cmd);
}


uint8_t appm_get_dev_name(uint8_t* name)
{
    // copy name to provided pointer
    memcpy(name, app_env.dev_name, app_env.dev_name_len);
    // return name length
    return app_env.dev_name_len;
}

void appm_calc_psk(uint8_t *p_pub_key)
{
    struct gapm_gen_dh_key_cmd *dh_key_cmd = KERNEL_MSG_ALLOC(GAPM_GEN_DH_KEY_CMD,
                                                            TASK_BLE_GAPM, TASK_BLE_APP,
                                                            gapm_gen_dh_key_cmd);
    dh_key_cmd->operation = GAPM_GEN_DH_KEY;
    memcpy(dh_key_cmd->operand_1, p_pub_key, GAP_P256_KEY_LEN);
    memcpy(dh_key_cmd->operand_2, p_pub_key + GAP_P256_KEY_LEN, GAP_P256_KEY_LEN);

    kernel_msg_send(dh_key_cmd);
}

void appm_get_key(void)
{
    struct gapm_get_pub_key_cmd *get_key_cmd = KERNEL_MSG_ALLOC(GAPM_GET_PUB_KEY_CMD,
                                                            TASK_BLE_GAPM, TASK_BLE_APP,
                                                            gapm_get_pub_key_cmd);

    get_key_cmd->operation = GAPM_GET_PUB_KEY;
    kernel_msg_send(get_key_cmd);
}

uint8_t appm_get_app_status(void)
{
	return kernel_state_get(TASK_BLE_APP);
}
#endif //(BLE_APP_PRESENT)



/// @} APP


