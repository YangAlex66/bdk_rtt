/**
 ****************************************************************************************
 *
 * @file mm_lights.h
 *
 * @brief Header file for Mesh Model Lighting Server Module
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MM_LIGHTS_H_
#define _MM_LIGHTS_H_

/**
 ****************************************************************************************
 * @defgroup MM_LIGHTS Mesh Model Lighting Server Module
 * @ingroup MESH_MDL
 * @brief Mesh Model Lighting Server Module
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mm_inc.h"       // Mesh Model Includes
#include "mesh_api.h"       // Mesh Model API Definitions

#if (BLE_MESH_MDL_LIGHTS)

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh Model Lighting Server Module initialization function
 *
 * @param[in] reset     True means SW reset, False means task initialization
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to Mesh Model Configuration Structure provided by the Application
 *
 * @return Size of environment required for this module
 ****************************************************************************************
 */
uint16_t mm_lights_init(bool reset, void *p_env, const mm_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Return memory size needed for environment allocation of Mesh Model Lighting Server Module
 *
 * @param[in] p_cfg     Pointer to Mesh Model Configuration Structure provided by the Application
 *
 * @return Size of environment required for this module
 ****************************************************************************************
 */
uint16_t mm_lights_get_env_size(const mm_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Register a Lighting Server model configuration.
 *
 * @param[in] elmt_idx      Index of element on which models must be registered
 * @param[in] mdl_cfg_idx   Model Configuration Index (@see mm_cfg_idx)
 * @param[in] main          Indicate if the configuration contains a main model
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t mm_lights_register(uint8_t elmt_idx, uint8_t mdl_cfg_idx, bool main);

#endif //(BLE_MESH_MDL_LIGHTS)

/// @} end of group

#endif //_MM_LIGHTS_
