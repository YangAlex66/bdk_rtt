/**
 ****************************************************************************************
 *
 * @file mm_tb.h
 *
 * @brief Header file for Mesh Model Tool Boxes
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MM_TB_H_
#define _MM_TB_H_

/**
 ****************************************************************************************
 * @defgroup MM_TB Mesh Model Tool Boxes
 * @ingroup MESH
 * @brief Mesh Model Tool Boxes
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mm_tb_state.h"     // Mesh Model State Manager Definitions
#include "mm_tb_bind.h"      // Mesh Model Binding Manager Definitions
#include "mm_tb_replay.h"    // Mesh Model Replay Manager Definitions

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of Mesh Model Tool Boxes
 *
 * @param[in] reset     True for SW reset, False for task initialization
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to Mesh Model initialization parameters
 *
 * @return Required memory size for Mesh Model Tool Boxes environments
 ****************************************************************************************
 */
uint16_t mm_tb_init(bool reset, void *p_env, const mm_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Return required memory size for Mesh Model Tool Boxes environments
 *
 * @param[in] p_cfg    Pointer to Mesh Model initialization parameters*
 ****************************************************************************************
 */
uint16_t mm_tb_get_env_size(const mm_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Convert a transition time value in milliseconds in its packed format
 *
 * @param[in] trans_time_ms       Transition time in milliseconds
 *
 * @return Packed transition time
 ****************************************************************************************
 */
uint8_t mm_tb_get_trans_time(uint32_t trans_time_ms);

/**
 ****************************************************************************************
 * @brief Convert a transition time value in its packed format into a millisecond value
 *
 * @param[in] time_tt       Transition time (packed format)
 *
 * @return Transition time in milliseconds
 ****************************************************************************************
 */
uint32_t mm_tb_get_trans_time_ms(uint8_t time_tt);

/// @} end of group

#endif //_MM_TB_H_
