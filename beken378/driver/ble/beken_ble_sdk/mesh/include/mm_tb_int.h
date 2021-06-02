/**
 ****************************************************************************************
 * @file mm_tb_int.h
 *
 * @brief Header file for Mesh Model Tool Boxes Internal Definitions
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MM_TB_INT_H_
#define _MM_TB_INT_H_

/**
 ****************************************************************************************
 * @defgroup MM_TB_INT Mesh Model Tool Boxes Internal Definitions
 * @ingroup MM_TB
 * @brief Mesh Model Tool Boxes
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mal_lib.h"        // Mesh Abstraction Layer Library
#include "mesh_api.h"       // Mesh Stack Application Programming Interface Definitions
#include "mm_defines.h"     // Mesh Model Definitions

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of Mesh Model State Manager
 *
 * @param[in] reset     True for SW reset, False for task initialization
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to Mesh Model initialization parameters
 *
 * @return Required memory size for Mesh Model State Manager environment
 ****************************************************************************************
 */
uint16_t mm_tb_state_init(bool reset, void *p_env, const mm_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Return required memory size for Mesh Model State Manager environment
 *
 * @param[in] p_cfg    Pointer to Mesh Model initialization parameters*
 ****************************************************************************************
 */
uint16_t mm_tb_state_get_env_size(const mm_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Initialization of Mesh Model Binding Manager
 *
 * @param[in] reset     True for SW reset, False for task initialization
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to Mesh Model initialization parameters
 *
 * @return Required memory size for Mesh Model Binding Manager environment
 ****************************************************************************************
 */
uint16_t mm_tb_bind_init(bool reset, void *p_env, const mm_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Return required memory size for Mesh Model Binding Manager environment
 *
 * @param[in] p_cfg    Pointer to Mesh Model initialization parameters*
 ****************************************************************************************
 */
uint16_t mm_tb_bind_get_env_size(const mm_cfg_t *p_cfg);

#if (BLE_MESH_MDL_SERVER)
/**
 ****************************************************************************************
 * @brief Initialization of Mesh Model Replay Manager
 *
 * @param[in] reset     True for SW reset, False for task initialization
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to Mesh Model initialization parameters
 *
 * @return Required memory size for Mesh Model Replay Manager environment
 ****************************************************************************************
 */
uint16_t mm_tb_replay_init(bool reset, void *p_env, const mm_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Return required memory size for Mesh Model Replay Manager environment
 *
 * @param[in] p_cfg    Pointer to Mesh Model initialization parameters*
 ****************************************************************************************
 */
uint16_t mm_tb_replay_get_env_size(const mm_cfg_t *p_cfg);
#endif //(BLE_MESH_MDL_SERVER)

/// @} end of group

#endif //_MM_TB_INT_H_
