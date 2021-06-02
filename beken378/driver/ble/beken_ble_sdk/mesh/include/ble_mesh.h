/**
 ****************************************************************************************
 *
 * @file ble_mesh.h
 *
 * @brief Header file for Mesh Stack
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _BLE_MESH_H_
#define _BLE_MESH_H_

/**
 ****************************************************************************************
 * @defgroup MESH Mesh Stack
 * @ingroup MESH
 * @brief Mesh Stack
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh_api.h"      // Mesh Stack API Definitions

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh Stack initialization function
 *
 * @param[in] reset     True means SW reset, False means task initialization
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module
 ****************************************************************************************
 */
uint16_t mesh_init(bool reset, void *p_env, const mesh_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Return memory size needed for environment allocation of Mesh Stack
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module
 ****************************************************************************************
 */
uint16_t mesh_get_env_size(const mesh_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Handler for messages targetting the mesh stack
 ****************************************************************************************
 */
uint8_t mesh_handler(uint16_t msg_id, uint16_t src_id, const void *p_param);

/// @} end of group

#endif //_MESH_
