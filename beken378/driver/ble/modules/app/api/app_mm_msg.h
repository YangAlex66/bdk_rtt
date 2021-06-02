/**
 ****************************************************************************************
 *
 * @file app_mm_msg.h
 *
 * @brief mesh Application Module entry point
 *
 * @auth  gang.cheng
 *
 * @date  2019.03.31
 *
 * Copyright (C) Beken 2009-2020
 *
 *
 ****************************************************************************************
 */

#ifndef APP_MM_MSG_H_
#define APP_MM_MSG_H_
/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup BEKEN
 *
 * @brief OADS Application Module entry point
 *
 * @{
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration


#include <stdint.h>          // Standard Integer Definition
#include "kernel_task.h"         // Kernel Task Definition
#include "m_tb_state.h"
#include "mesh_tb_timer.h"
#if (NVDS_SUPPORT)
#include "nvds.h"
#endif
#include "application.h"

int app_models_msg_pro_handler(kernel_msg_id_t const msgid,
                               void const *param,
                               kernel_task_id_t const dest_id,
                               kernel_task_id_t const src_id);

void app_mesh_add_models_server(void);

#endif // APP_MM_MSG_H_

