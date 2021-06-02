
/**
 ****************************************************************************************
 * @file mm_vendors.h
 *
 * @brief Header file for Mesh Model Vendor Server Module
 *
 * Copyright (C) Beken 2019-2030
 *
 ****************************************************************************************
 */

#ifndef _MM_VENDORS_H_
#define _MM_VENDORS_H_

#include "mm_api_int.h"       // Mesh Model Application Programming Interface Internal Definitions
#include "mesh_tb_timer.h"    // Mesh Timer Manager Definitions
#include "mesh_api.h"       // Mesh Model API Definitions
#include "mm_defines.h"     // Mesh Model Definitions
#include "mm_tb.h"          // Mesh Model Tool Boxes Definitions
#include "mm_route.h"       // Mesh Model Routing Module Definitions
#include "mesh_tb_buf.h"    // Mesh Buffer Manager Definitions


#define MM_VENDORS_ATTR_MAX_LEN  16

#define MM_VENDORS_ATTR_MAX_NUM  16

#define MM_ID_VENDORS                (0x01A80600)

#define MM_VENDORS_REPLAY_MS         (6000)

#define MM_LIGHT_REPORT_MS           (500)

#define MM_REPORT_MAX_TIMES          (10)


// List of 3-byte opcodes for the VENDOR Server model
enum mm_vendor_3b_opcodes
{
    // Vendor message Attr get
    MM_MSG_VENDOR_ATTR_GET            =   0x01A8D0,
    // Vendor message Attr set
    MM_MSG_VENDOR_ATTR_ATTR_SET,
    // Vendor message command set
    MM_MSG_VENDOR_ATTR_ATTR_SET_UNACK,
    // Vendor message response stauts
    MM_MSG_VENDOR_ATTR_ATTR_STATUS,
    // Vendor message indication
    MM_MSG_VENDOR_ATTR_INDICATION,
    // Vendor message confirmation
    MM_MSG_VENDOR_ATTR_CONFIRMATION,
    // Vendor message Transparent
    MM_MSG_VENDOR_ATTR_TRANSPARENT = 0xA801CF,
};


/// Positions in Vendors Status message
enum mm_vendors_status_pos
{
    /// Present
    MM_VENDORS_STATUS_TID_POS = 0,

    MM_VENDORS_STATUS_ATTR_TYPE_POS = 1,

    MM_VENDORS_STATUS_ATTR_PARAM_POS = 3,
};

typedef struct attr_type_len
{
    uint16_t attr_type;
    uint8_t  attr_len;

} attr_type_len_t;



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
} mm_vendors_env_t;


typedef enum
{
    LIGHT_COLOR = 0x0123,
    LIGHT_SWITCH    = 0x0100,
} vendor_attr_type_t;



void mm_vendor_attr_indication(uint16_t attr_type,uint8_t len,uint8_t *value);

uint16_t mm_vendors_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid);

bool mm_vendors_cfm_recved(void);

void mm_vendors_cfm_clear(void);


#endif // _MM_VENDORS_H_
