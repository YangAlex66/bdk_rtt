/**
 ****************************************************************************************
 *
 * @file mesh_tb.h
 *
 * @brief Header file for Mesh Stack Managers
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MESH_TB_H_
#define _MESH_TB_H_

/**
 ****************************************************************************************
 * @defgroup MESH_TB Mesh Stack Managers
 * @ingroup MESH
 * @brief Mesh Stack Managers
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh_inc.h"        // Mesh Stack Includes

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh Stack Managers Module Initialization
 *
 * @param[in] reset     True means SW reset, False means task initialization
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t mesh_tb_init(bool reset, void *p_env, const void *p_cfg);

/**
 ****************************************************************************************
 * @brief Get size needed for environments used by Mesh Stack Managers Module
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this mesh module
 ****************************************************************************************
 */
uint16_t mesh_tb_get_env_size(const void *p_cfg);

/// @} end of group

#endif //_MESH_TB_H_
