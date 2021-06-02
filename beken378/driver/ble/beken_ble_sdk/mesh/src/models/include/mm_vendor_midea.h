/**
 ****************************************************************************************
 * @file mm_vendors_midea.h
 *
 * @brief Header file for Midea Mesh Model Vendor Server Module
 *
 * Copyright (C) Midea 2019-2030
 *
 * Author : Astron (hanyh2@midea.com)
 ****************************************************************************************
 */

#ifndef _MM_VENDOR_MIDEA_H_
#define _MM_VENDOR_MIDEA_H_

#include "mm_api_int.h"       // Mesh Model Application Programming Interface Internal Definitions
#include "mesh_tb_timer.h"    // Mesh Timer Manager Definitions
#include "mesh_api.h"       // Mesh Model API Definitions
#include "mm_defines.h"     // Mesh Model Definitions
#include "mm_tb.h"          // Mesh Model Tool Boxes Definitions
#include "mm_route.h"       // Mesh Model Routing Module Definitions
#include "mesh_tb_buf.h"    // Mesh Buffer Manager Definitions
#include "mm_tb_state.h"
#include "mm_tb_replay.h"

#define MM_MIDEA_VENDOR_ATTR_MAX_LEN  16

#define MM_MIDEA_VENDOR_ATTR_MAX_NUM  16

#define MM_MIDEA_VENDOR_REPLAY_MS         (6000)

#define MM_MIDEA_REPORT_MAX_TIMES          (10)

#define MM_VENDORS_ATTR_MAX_LEN  16

#define MM_VENDORS_ATTR_MAX_NUM  16

typedef struct vendors_attr
{
    uint16_t attr_type;
    uint16_t attr_len;
    uint8_t  attr_param[MM_VENDORS_ATTR_MAX_LEN];
    uint8_t  tgt_attr_param[MM_VENDORS_ATTR_MAX_LEN];

} vendors_attr_t;

/// Structure for Vendor Server model environment
typedef struct mm_vendors_env
{
    /// Basic model environment - Must be first element in the structure - DO NOT MOVE
    mm_tb_state_mdl_env_t env;
    /// Timer for sending of publications
    mesh_tb_timer_t tmr_publi;
    /// Publication period in milliseconds
    uint32_t publi_period_ms;

    /// Environment for replay protection mechanism
    mm_tb_replay_mdl_env_t replay_env;

    uint8_t tid;
    uint16_t cur_attr_type;
    vendors_attr_t attr[MM_VENDORS_ATTR_MAX_NUM];

    /// Address to which a Generic OnOff Status message must be sent (unassigned address if no
    /// status must be sent)
    uint16_t status_dst_addr;
    /// Application key local index to be used for transmission of Generic OnOff Status message
    m_lid_t status_app_key_lid;
    /// Relaying of sent Generic OnOff Status authorized
    bool status_relay;
} mm_vendor_env_t;
uint16_t mm_vendor_midea_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid);

#endif