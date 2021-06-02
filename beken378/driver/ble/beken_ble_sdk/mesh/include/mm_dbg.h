/**
 ****************************************************************************************
 *
 * @file mm_dbg.h
 *
 * @brief Header file for Mesh Model Debug Module
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef MM_DBG_
#define MM_DBG_

/**
 ****************************************************************************************
 * @defgroup MM_DBG Mesh Model Debug Module
 * @ingroup MESH_MDL
 * @brief  Mesh Model Debug Module
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mm_inc.h"    // Mesh Model Include Files

#if (BLE_MESH_DBG)

/*
 * MESSAGE DEFINITIONS
 ****************************************************************************************
 */

/// Debug command required structure (without parameters)
typedef struct mm_dbg_cmd
{
    /// Debug Command code (@see enum mm_dbg_cmd_code)
    uint32_t cmd_code;
} mm_dbg_cmd_t;

/// Debug command complete event required structure (without parameters)
typedef struct mm_dbg_cmp_evt
{
    /// Debug Command code (@see enum mm_dbg_cmd_code)
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;
} mm_dbg_cmp_evt_t;

/// Debug indication required structure (without parameters)
typedef struct mm_dbg_ind
{
    /// Indication Code (@see enum mm_dbg_ind_code)
    uint32_t ind_code;
} mm_dbg_ind_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh Model Debug Module initialization
 *
 * @param[in] reset     True means SW reset, False means task initialization
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t mm_dbg_init(bool reset, void *p_env, const mm_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Get size of environment required by Mesh Model Debug Module
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t mm_dbg_get_env_size(const mm_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Handler for Mesh Model Debug messages
 *
 * @param[in] msg_id        Identifier of the message received
 * @param[in] src_id        Identifier of the task that issue message
 * @param[in] p_param       Message parameters
 *
 * @return Status of message after handling (@see enum mal_msg_status)
 ****************************************************************************************
 */
uint8_t mm_dbg_handler(uint16_t msg_id, uint16_t src_id, const void *p_param);

#endif //(BLE_MESH_DBG)

/// @} MM_DBG

#endif /* MM_DBG_ */
