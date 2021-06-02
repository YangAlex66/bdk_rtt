/**
 ****************************************************************************************
 *
 * @file mm_tb_replay.h
 *
 * @brief Header file for Mesh Model Replay Manager Module
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MM_TB_REPLAY_H_
#define _MM_TB_REPLAY_H_

/**
 ****************************************************************************************
 * @defgroup MM_API Mesh Model Replay Manager Module
 * @ingroup MESH_MDL
 * @brief Mesh Model Replay Manager Module
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mal_lib.h"            // Mesh Abstraction Layer Library
#include "mesh_tb_timer.h"      // Timer Manager

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Structure to allocate as part of model environment for replay protection
typedef struct mm_tb_replay_mdl_env
{
    /// List of replay elements
    common_list_t list_replay;
    /// Timer for replay protection
    mesh_tb_timer_t tmr_replay;
    /// Validity delay in milliseconds for replay elements inserted in the list
    uint16_t delay_ms;
    /// Sum of delays of elements placed after the first replay element in the list
    uint16_t delay_total_ms;
} mm_tb_replay_mdl_env_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief
 *
 * @param[in] p_mdl_env
 * @param[in] src
 * @param[in] tid
 *
 * @return
 ****************************************************************************************
 */
bool mm_tb_replay_is_retx(mm_tb_replay_mdl_env_t *p_mdl_env, uint16_t src, uint8_t tid);

/// @} end of group

#endif //_MM_TB_REPLAY_
