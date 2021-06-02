/**
 ****************************************************************************************
 *
 * @file app_task.h
 *
 * @brief Header file - APPTASK.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef APP_TASK_H_
#define APP_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup APPTASK Task
 * @ingroup APP
 * @brief Routes ALL messages to/from APP block.
 *
 * The APPTASK is the block responsible for bridging the final application with the
 * RWBLE software host stack. It communicates with the different modules of the BLE host,
 * i.e. @ref SMP, @ref GAP and @ref GATT.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration

#if (BLE_APP_PRESENT)

#include <stdint.h>         // Standard Integer
#include "rwip_task.h"      // Task definitions
#include "kernel_task.h"        // Kernel Task
#include "ble_ui.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Number of APP Task Instances
#define APP_IDX_MAX                 (BLE_CONNECTION_MAX + BLE_ACTIVITY_MAX)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */
enum appm_state
{
    /// Initialization state
    APPM_INIT,
    /// Database create state
    APPM_CREATE_DB,
    /// Ready State
    APPM_READY,
    /// Number of defined states.
    APPM_STATE_MAX
};

enum appc_state
{
    APPC_LINK_IDLE = 0,

    APPC_LINK_CONNECTED,

    APPC_SDP_DISCOVERING,

    APPC_SERVICE_CONNECTED,
    /// Number of defined states.
    APPC_STATE_MAX
};

/// APP Task messages
/*@TRACE*/
enum app_msg_id
{
    APPM_DUMMY_MSG = TASK_BLE_FIRST_MSG(TASK_BLE_ID_APP),

    #if (BLE_APP_HT)
    /// Timer used to refresh the temperature measurement value
    APP_HT_MEAS_INTV_TIMER,
    #endif //(BLE_APP_HT)

    #if (BLE_APP_HID)
    /// Timer used to disconnect the moue if no activity is detecter
    APP_HID_MOUSE_TIMEOUT_TIMER,
    #endif //(BLE_APP_HID)
    APP_DEVIATION_CLAC_TIMER,
#if (BLE_CENTRAL)
    APP_INIT_CON_DEV_TIMEROUT_TIMER,
    APP_INIT_INIT_EVENT,
    APP_INIT_START_TIMEOUT_EVENT,
    APP_INIT_CON_SDP_END_TIMER,
#endif
};

struct app_task_event_ind
{
    /// Event Type
    uint8_t type;
};

struct app_task_start_timeout_event_ind
{
	/// Event Type
	kernel_task_id_t task_id;
	unsigned int timout_ms;
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

/// @} APPTASK

#endif //(BLE_APP_PRESENT)

#endif // APP_TASK_H_
