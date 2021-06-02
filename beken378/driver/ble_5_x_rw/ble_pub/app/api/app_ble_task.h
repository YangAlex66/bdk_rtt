#ifndef _APP_BLE_TASK_H_
#define _APP_BLE_TASK_H_

#include "rwip_config.h"     // SW configuration
#if (BLE_APP_PRESENT)

#include "app_ble.h"
#include "ble_ui.h"
#include "app_comm.h"                //  Application Module Definitions


/**
 ****************************************************************************************
 * @brief Start/stop advertising
 *
 * @param[in] start     True if advertising has to be started, else false
 ****************************************************************************************
 */
void appm_update_adv_state(char start);

/**
 ****************************************************************************************
 * @brief Send a disconnection request
 ****************************************************************************************
 */
void appm_disconnect(unsigned char reason);




#endif
#endif
