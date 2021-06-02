/**
 ****************************************************************************************
 *
 * @file mm_genc.h
 *
 * @brief Header file for Mesh Model Generic Client Module
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MM_GENC_H_
#define _MM_GENC_H_

/**
 ****************************************************************************************
 * @defgroup MM_GENC Mesh Model Generic Client Module
 * @ingroup MESH_MDL
 * @brief Mesh Model Generic Client Module
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mm_inc.h"         // Mesh Model Includes
#include "mesh_api.h"       // Mesh Model API Definitions

#if (BLE_MESH_MDL_GENC)

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh Model Generic Client Module initialization function
 *
 * @param[in] reset     True means SW reset, False means task initialization
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to Mesh Model Configuration Structure provided by the Application
 *
 * @return Size of environment required for this module
 ****************************************************************************************
 */
uint16_t mm_genc_init(bool reset, void *p_env, const mm_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Return memory size needed for environment allocation of Mesh Model Generic Client Module
 *
 * @param[in] p_cfg     Pointer to Mesh Model Configuration Structure provided by the Application
 *
 * @return Size of environment required for this module
 ****************************************************************************************
 */
uint16_t mm_genc_get_env_size(const mm_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Register a Generic Client Model
 *
 * @param[in] cmdl_idx      Client Model Index (@see enum mm_cmdl_idx)
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_genc_register(uint8_t cmdl_idx);

/**
 ****************************************************************************************
 * @brief Set global part of Generic Location state of a given node's element.
 *
 * @param[in] p_env             Pointer to the environment allocated for the model
 * @param[in] dst               Address of node's element to which message will be sent
 * @param[in] set_info          Set information
 * @param[in] latitude          Global Latitude
 * @param[in] longitude         Global Longitude
 * @param[in] altitude          Global Altitude
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_genc_loc_set_global(mm_tb_state_mdl_env_t *p_env, uint16_t dst, uint8_t set_info,
                                int32_t latitude, int32_t longitude, int16_t altitude);

/**
 ****************************************************************************************
 * @brief Set local part of Generic Location state of a given node's element.
 *
 * @param[in] p_env             Pointer to the environment allocated for the model
 * @param[in] dst               Address of node's element to which message will be sent
 * @param[in] set_info          Set information
 * @param[in] north             Local North
 * @param[in] east              Local East
 * @param[in] altitude          Local Altitude
 * @param[in] floor             Floor Number
 * @param[in] uncertainty       Uncertainty
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_genc_loc_set_local(mm_tb_state_mdl_env_t *p_env, uint16_t dst, uint8_t set_info,
                               int16_t north, int16_t east, int16_t altitude, uint8_t floor,
                               uint16_t uncertainty);

/**
 ****************************************************************************************
 * @brief Request to send a Generic User/Admin/Manufacturer/Client Property(ies) Get message
 * to an element
 *
 * @param[in] mdl_lid       Model Local Index of Generic Property Client model
 * @param[in] dst           Address of element to which the message must be sent
 * @param[in] prop_id       Property ID
 *
 * @return An handling status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_genc_prop_get(mm_tb_state_mdl_env_t *p_env, uint16_t dst, uint8_t get_type,
                          uint16_t prop_id);

/**
 ****************************************************************************************
 * @brief Request to send a Generic User/Admin/Manufacturer Property Set message to an
 * element
 *
 * @param[in] mdl_lid       Model local Index of Generic Property Client model
 * @param[in] dst           Address of element to which the message must be sent
 * @param[in] set_info      Set information
 * @param[in] prop_id       Property ID
 * @param[in] user_access   User access
 * @param[in] length        Property value length
 * @param[in] p_val         Pointer to the property value
 *
 * @return An handling status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_genc_prop_set(mm_tb_state_mdl_env_t *p_env, uint16_t dst, uint8_t set_info,
                          uint16_t prop_id, uint8_t user_access, uint16_t length, const uint8_t *p_val);

#endif //(BLE_MESH_MDL_GENC)

/// @} end of group

#endif //_MM_GENC_
