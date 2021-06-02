/**
 ****************************************************************************************
 *
 * @file mm_route.h
 *
 * @brief Header file for Mesh Model Routing Module
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MM_ROUTE_H_
#define _MM_ROUTE_H_

/**
 ****************************************************************************************
 * @defgroup MM_API Mesh Model Routing Module
 * @ingroup MESH_MDL
 * @brief Mesh Model Routing Module
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh_tb_buf.h"    // Mesh Buffer Manager Definitions

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Buffer information bitfield
/// 15    3         2       1       0
/// +-----+---------+-------+-------+
/// | RFU | PUBLISH | RELAY | RX/TX |
/// +-----+---------+-------+-------+
enum mm_route_buf_info
{
    /// RX or TX buffer (1 when RX)
    MM_ROUTE_BUF_INFO_RX_POS = 0,
    MM_ROUTE_BUF_INFO_RX_BIT = 0x01,

    /// Has been relayed (RX) or can be relayed (TX, only when response is sent)
    MM_ROUTE_BUF_INFO_RELAY_POS = 1,
    MM_ROUTE_BUF_INFO_RELAY_BIT = 0x02,

    /// Publication or response (for TX)
    MM_ROUTE_BUF_INFO_PUBLISH_POS = 2,
    MM_ROUTE_BUF_INFO_PUBLISH_BIT = 0x04,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Buffer environment when handled by models
typedef struct mm_route_buf_env
{
    /// Opcode
    uint32_t opcode;
    /// Information bitfield (@see enum mm_route_buf_info)
    uint16_t info;
    union
    {
        /// Source address (for RX)
        uint16_t src;
        /// Destination address (for TX)
        uint16_t dst;
    } u_addr;
    /// Application key local index
    m_lid_t app_key_lid;
    /// Model local index
    m_lid_t mdl_lid;
} mm_route_buf_env_t;

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

/// Model Callback Functions
extern const m_api_model_cb_t mm_route_cb;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh Model Routing Module initialization function
 *
 * @param[in] reset     True means SW reset, False means task initialization
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to Mesh Model Configuration Structure provided by the Application
 *
 * @return Size of environment required for this module
 ****************************************************************************************
 */
uint16_t mm_route_init(bool reset, void *p_env, const mm_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Return memory size needed for environment allocation of Mesh Model Routing Module
 *
 * @param[in] p_cfg     Pointer to Mesh Model Configuration Structure provided by the Application
 *
 * @return Size of environment required for this module
 ****************************************************************************************
 */
uint16_t mm_route_get_env_size(const mm_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Allocate a buffer for transmission of a message
 *
 * @param[in] pp_buf     Pointer to pointer at which address of allocated buffer must be written
 * @param[in] data_len   Requested data length
 ****************************************************************************************
 */
uint16_t mm_route_buf_alloc(mesh_tb_buf_t **pp_buf, uint16_t data_len);

/**
 ****************************************************************************************
 * @brief Allocate a buffer for transmission of a status message sent as a direct answer.
 *
 * @param[in] pp_buf     Pointer to pointer at which address of allocated buffer must be written
 * @param[in] data_len   Requested data length
 ****************************************************************************************
 */
uint16_t mm_route_buf_alloc_status(mesh_tb_buf_t **pp_buf, uint16_t data_len,
                                   mm_route_buf_env_t *p_route_env);

/**
 ****************************************************************************************
 * @brief Provide a buffer to the Access Layer of the Mesh Profile for transmission
 *
 * @param[in] p_buf     Pointer to buffer containing the message to send
 ****************************************************************************************
 */
void mm_route_send(mesh_tb_buf_t *p_buf);

/// @} end of group

#endif //_MM_ROUTE_
