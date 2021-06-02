/**
 ****************************************************************************************
 *
 * @file mesh_tb_int.h
 *
 * @brief Header file for Mesh Common Managers Internal Definitions
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MESH_TB_INT_H_
#define _MESH_TB_INT_H_

/**
 ****************************************************************************************
 * @defgroup MESH_TB_INT Mesh Common Managers Internal Definitions
 * @ingroup MESH_TB
 * @brief Mesh Common Managers Internal Definitions
 * @{
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh Security Manager initialization
 *
 * @param[in] reset     True means SW reset, False means task initialization
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t mesh_tb_sec_init(bool reset, void *p_env, const void *p_cfg);

/**
 ****************************************************************************************
 * @brief Mesh Security Manager memory requirement
 *
 * @note this function is called before init function in order to know memory requirements of the module
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this mesh module
 ****************************************************************************************
 */
uint16_t mesh_tb_sec_get_env_size(const void *p_cfg);

/**
 ****************************************************************************************
 * @brief Mesh Timer Manager initialization
 *
 * @note Called when Mesh stack is initialized or when a SW reset is requested.
 *
 * @param[in] reset True means SW reset, False means task initialization
 * @param[in] p_env Pointer to the environment structure
 * @param[in] p_cfg Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t mesh_tb_timer_init(bool reset, void *p_env, const void *p_cfg);

/**
 ****************************************************************************************
 * @brief Mesh Timer Manager memory requirement
 *
 * @note this function is called before init function in order to know memory requirements of the module
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this mesh module
 ****************************************************************************************
 */
uint16_t mesh_tb_timer_get_env_size(const void *p_cfg);

/**
 ****************************************************************************************
 * @brief Mesh Buffer Manager Initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t mesh_tb_buf_init(bool reset, void *p_env, const void *p_cfg);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Mesh Buffer Manager
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t mesh_tb_buf_get_env_size(const void *p_cfg);

/// @} end of group

#endif //_MESH_TB_INT_H_
