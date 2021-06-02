#ifndef APP_COMM_H_
#define APP_COMM_H_

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_COMM)
#include <stdint.h>          // Standard Integer Definition
#include "ble.h"
#include "ble_pub.h"
#include "ble_api_5_x.h"
#include "kernel_task.h"         // Kernel Task Definition

extern const struct app_subtask_handlers app_comm_table_handler;

#endif

#endif
