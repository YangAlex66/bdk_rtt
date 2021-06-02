/**
 ****************************************************************************************
 * @file mm_gens.h
 *
 * @brief Header file for Mesh Model Generic Server Module
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MM_GENS_H_
#define _MM_GENS_H_

/**
 ****************************************************************************************
 * @defgroup MM_GENS Mesh Model Generic Server Module
 * @ingroup MESH_MDL
 * @brief Mesh Model Generic Server Module
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh_api.h"       // Mesh Model API Definitions
#include "mm_defines.h"     // Mesh Model Definitions
#include "mm_tb.h"          // Mesh Model Tool Boxes Definitions
#include "mm_route.h"       // Mesh Model Routing Module Definitions
#include "mesh_tb_buf.h"    // Mesh Buffer Manager Definitions

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh Model Generic Server Module initialization function
 *
 * @param[in] reset     True means SW reset, False means task initialization
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to Mesh Model Configuration Structure provided by the Application
 *
 * @return Size of environment required for this module
 ****************************************************************************************
 */
uint16_t mm_gens_init(bool reset, void *p_env, const mm_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Return memory size needed for environment allocation of Mesh Model Generic Server Module
 *
 * @param[in] p_cfg     Pointer to Mesh Model Configuration Structure provided by the Application
 *
 * @return Size of environment required for this module
 ****************************************************************************************
 */
uint16_t mm_gens_get_env_size(const mm_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Register a Generic Server model configuration.
 *
 * @param[in] elmt_idx      Index of element on which models must be registered
 * @param[in] mdl_cfg_idx   Model Configuration Index (@see mm_cfg_idx)
 * @param[in] main          Indicate if the configuration contains a main model
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t mm_gens_register(uint8_t elmt_idx, uint8_t mdl_cfg_idx, bool main);

/**
 ****************************************************************************************
 * @brief TODO [LT]
 ****************************************************************************************
 */
void mm_gens_add_to_grp(uint8_t elmt_idx, uint32_t model_id, uint8_t grp_lid);

/**
 ****************************************************************************************
 * @brief Function called upon reception of Generic Battery state value for a local element.
 *
 * @param[in] status            Confirmation status
 * @param[in] elmt_idx          Index of element for which Battery information are provided
 * @param[in] bat_lvl           Battery Level
 * @param[in] time_charge       Time to charge in minutes
 * @param[in] time_discharge    Time to discharge in minutes
 * @param[in] flags             Flags
 ****************************************************************************************
 */
void mm_gens_cfm_bat(uint16_t status, uint8_t elmt_idx, uint8_t bat_lvl, uint32_t time_charge,
                     uint32_t time_discharge, uint8_t flags);

/**
 ****************************************************************************************
 * @brief Function called upon reception of global part of Generic Location state value
 * for a local element.
 *
 * @param[in] status            Confirmation status
 * @param[in] elmt_idx          Index of element for which Location information are provided
 * @param[in] latitude          Global Latitude
 * @param[in] longitude         Global Longitude
 * @param[in] altitude          Global Altitude
 ****************************************************************************************
 */
void mm_gens_cfm_locg(uint16_t status, uint8_t elmt_idx, int32_t latitude, int32_t longitude,
                      int16_t altitude);

/**
 ****************************************************************************************
 * @brief Function called upon reception of local part of Generic Location state value
 * for a local element.
 *
 * @param[in] status            Confirmation status
 * @param[in] elmt_idx          Index of element for which Location information are provided
 * @param[in] north             Local North
 * @param[in] east              Local East
 * @param[in] altitude          Local Altitude
 * @param[in] floor             Floor Number
 * @param[in] uncertainty       Uncertainty
 ****************************************************************************************
 */
void mm_gens_cfm_locl(uint16_t status, uint8_t elmt_idx, int16_t north, int16_t east,
                      int16_t altitude, uint8_t floor, uint16_t uncertainty);

/**
 ****************************************************************************************
 * @brief Register Generic Property models on an element
 *
 * @param[in] elmt_idx          Index of element on which models must be registered
 * @param[in] req_queue_len     Number of received messages that can be queued when model
 * is waiting for application confirmation
 * @param[in] nb_prop_user      Number of Generic User Properties
 * @param[in] nb_prop_admin     Number of Generic Admin Properties
 * @param[in] nb_prop_manuf     Number of Generic Manufacturer Properties
 * @param[in] nb_prop_cli       Number of Generic Client Properties
 * @param[in] p_props           Pointer to list of Generic Property information
 *
 * @return An handling error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_gens_prop_register(uint8_t elmt_idx, uint8_t req_queue_len,
                               uint8_t nb_prop_user, uint8_t nb_prop_admin,
                               uint8_t nb_prop_manuf, uint8_t nb_prop_cli, const mm_prop_t *p_props);

/**
 ****************************************************************************************
 * @brief Confirmation functions for get and set request indication received by the application
 *
 * @param[in] status        Confirmation status
 * @param[in] elmt_idx      Element index
 * @param[in] prop_type     Property Type (@see enum mm_prop_type)
 * @param[in] prop_id       Property ID
 * @param[in] length        Property Length
 * @param[in] p_val         Pointer to the property value
 ****************************************************************************************
 */
void mm_gens_prop_cfm(uint16_t status, uint8_t elmt_idx, uint8_t prop_type, uint16_t prop_id,
                      uint16_t length, const uint8_t *p_val);

/// @} end of group

#endif //_MM_GENS_
