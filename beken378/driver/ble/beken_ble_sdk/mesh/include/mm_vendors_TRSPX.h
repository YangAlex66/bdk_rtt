
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

#ifndef _MM_VENDORS_TRSPX_H_
#define _MM_VENDORS_TRSPX_H_

#include "mm_api_int.h"       // Mesh Model Application Programming Interface Internal Definitions
#include "mesh_tb_timer.h"    // Mesh Timer Manager Definitions
#include "mesh_api.h"       // Mesh Model API Definitions
#include "mm_defines.h"     // Mesh Model Definitions
#include "mm_tb.h"          // Mesh Model Tool Boxes Definitions
#include "mm_route.h"       // Mesh Model Routing Module Definitions
#include "mesh_tb_buf.h"    // Mesh Buffer Manager Definitions
#include "mm_vendors_morenode.h"

#define MM_ID_VENDORS_TRSPX               (0x06A80000) // 0x06A80000 invild id

#define TRSPX_MSG_OPCODE                          (0xC006A8)



uint16_t mm_vendors_trspx_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid);




#endif // _MM_VENDORS_H_
