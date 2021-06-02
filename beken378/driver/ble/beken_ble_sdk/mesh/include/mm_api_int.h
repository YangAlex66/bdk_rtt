/**
 ****************************************************************************************
 *
 * @file mm_api_int.h
 *
 * @brief Header file for Mesh Model Message Application Programming Interface Internal Definitions
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MM_API_INT_H_
#define _MM_API_INT_H_

/**
 ****************************************************************************************
 * @defgroup MM_API_INT Mesh Model Message Application Programming Interface Internal Definitions
 * @ingroup MESH_MDL
 * @brief Mesh Model Message Application Programming Interface Internal Definitions
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mm_inc.h"        // Mesh Model Include Files

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

#if (BLE_MESH_MDL_SERVER)
/**
 ****************************************************************************************
 * @brief Add a local model to a Model group
 *
 * @param[in] elmt_idx      Index of elements to model belongs to
 * @param[in] mdl_id        Model ID
 * @param[in] grp_lid       Group local index
 ****************************************************************************************
 */
void mm_api_grp_add_local(uint8_t elmt_idx, uint32_t mdl_id, uint8_t grp_lid);
#endif //(BLE_MESH_MDL_SERVER)

/**
 ****************************************************************************************
 * @brief Inform upper application about registered model
 *
 * @param[in] model_id      Model Identifier
 * @param[in] elmt_idx      Element Index
 * @param[in] mdl_lid       Allocated model local index
 ****************************************************************************************
 */
void mm_api_send_register_ind(uint32_t model_id, uint8_t elmt_idx, m_lid_t mdl_lid);

#if (BLE_MESH_MDL_SERVER)
/**
 ****************************************************************************************
 * @brief Inform application about a state update
 *
 * @param[in] state_id          State identifier (@see mm_state_idx)
 * @param[in] elmt_idx          Element Index
 * @param[in] state             New state value or targeted state value depending on
 * transition time
 * @param[in] trans_time_ms     Transition time in milliseconds
 ****************************************************************************************
 */
void mm_api_send_srv_state_upd_ind(uint16_t state_id, uint8_t elmt_idx, uint32_t state,
                                   uint32_t trans_time_ms);

void mm_api_send_srv_array_state_upd_ind(uint16_t state_id, uint8_t elmt_idx, uint8_t len, uint8_t *state,
        uint32_t trans_time_ms);

/**
 ****************************************************************************************
 * @brief Send a request to upper application in order to retrieve a state value not
 * stored locally
 *
 * @param[in] req_ind_code      Request indication code
 * @param[in] elmt_dix          Element Index
 ****************************************************************************************
 */
void mm_api_send_srv_state_req_ind(uint32_t req_ind_code, uint8_t elmt_idx);

/**
 ****************************************************************************************
 * @brief Inform upper application about set global part of Generic Location state value
 *
 * @param[in] elmt_idx      Element Index
 * @param[in] latitude      Global Latitude
 * @param[in] longitude     Global Longitude
 * @param[in] altitude      Global Altitude
 ****************************************************************************************
 */
void mm_api_send_srv_locg_upd_ind(uint8_t elmt_idx, int32_t latitude, int32_t longitude,
                                  int16_t altitude);

/**
 ****************************************************************************************
 * @brief Inform upper application about set local part of Generic Location state value
 *
 * @param[in] elmt_idx      Element Index
 * @param[in] north         Local North
 * @param[in] east          Local East
 * @param[in] altitude      Local Altitude
 * @param[in] floor         Floor Number
 * @param[in] uncertainty   Uncertainty
 ****************************************************************************************
 */
void mm_api_send_srv_locl_upd_ind(uint8_t elmt_idx, int16_t north, int16_t east, int16_t altitude,
                                  uint8_t floor, uint16_t uncertainty);

/**
 ****************************************************************************************
 * @brief Inform application about received get request for a Generic User/Admin/Manufacturer
 * Property. A confirmation is expected from the application.
 *
 * @param[in] elmt_idx      Index of element for which get request has been received
 * @param[in] prop_type     Property type (@see enum mm_prop_type)
 * @param[in] prop_id       Property ID
 ****************************************************************************************
 */
void mm_api_send_srv_prop_get(uint8_t elmt_idx, uint8_t prop_type, uint16_t prop_id);

/**
 ****************************************************************************************
 * @brief Inform application about received set request for a Generic User/Admin/Manufacturer
 * Property. A confirmation is expected from the application.
 *
 * @param[in] elmt_idx      Index of element for which set request has been received
 * @param[in] prop_type     Property type (@see enum mm_prop_type)
 * @param[in] prop_id       Property ID
 * @param[in] ack           True if confirmation is expected, else False
 * @param[in] length        Property value length
 * @param[in] p_val         Pointer to the received property value
 ****************************************************************************************
 */
void mm_api_send_srv_prop_set(uint8_t elmt_idx, uint8_t prop_type, uint16_t prop_id, bool ack,
                              uint16_t length, uint8_t *p_val);
#endif //(BLE_MESH_MDL_SERVER)

#if (BLE_MESH_MDL_CLIENT)
/**
 ****************************************************************************************
 * @brief Inform application about received state value from an element
 *
 * @param[in] src           Address of element for which state value has been received
 * @param[in] state_id      State Identifier
 * @param[in] state_1       State value 1
 * @param[in] state_2       State value 2
 * @param[in] rem_time_ms   Remaining time in milliseconds
 ****************************************************************************************
 */
void mm_api_send_cli_state_ind(uint16_t src, uint16_t state_id, uint32_t state_1,
                               uint32_t state_2, uint32_t rem_time_ms);

/**
 ****************************************************************************************
 * @brief Inform application about received Generic Battery state value from an element
 *
 * @param[in] src               Address of element for which Generic Battery state value has
 * been received
 * @param[in] bat_lvl           Battery level value
 * @param[in] time_discharge    Time to discharge
 * @param[in] time_charge       Time to charge
 * @param[in] flags             Flags
 ****************************************************************************************
 */
void mm_api_send_cli_bat_ind(uint16_t src, uint8_t bat_lvl, uint32_t time_discharge,
                             uint32_t time_charge, uint8_t flags);

/**
 ****************************************************************************************
 * @brief Inform application about received Generic Location Global state value from an element
 *
 * @param[in] src           Address of element for which the Generic Location Global state value
 * has been received
 * @param[in] latitude      Global Latitude
 * @param[in] longitude     Global Longitude
 * @param[in] altitude      Global Altitude
 ****************************************************************************************
 */
void mm_api_send_cli_locg_ind(uint16_t src, int32_t latitude, int32_t longitude, int16_t altitude);

/**
 ****************************************************************************************
 * @brief Inform application about received Generic Location Local state value from an element
 *
 * @param[in] src           Address of element for which the Generic Location Local state value
 * has been received
 * @param[in] north         Local North
 * @param[in] East          Local East
 * @param[in] altitude      Local Altitude
 * @param[in] floor         Floor
 * @param[in] uncertainty   Uncertainty
 ****************************************************************************************
 */
void mm_api_send_cli_locl_ind(uint16_t src, int16_t north, int16_t east, int16_t altitude,
                              uint8_t floor, uint16_t uncertainty);

/**
 ****************************************************************************************
 * @brief Inform application about reception of a Generic User/Admin/Manufacturer Property
 * value
 *
 * @param[in] src           Address of element for which property value has been received
 * @param[in] prop_id       Property ID
 * @param[in] user_access   User access
 * @param[in] length        Property value length
 * @param[in] p_val         Pointer to the property value
 ****************************************************************************************
 */
void mm_api_send_cli_prop_ind(uint16_t src, uint16_t prop_id, uint8_t user_access, uint16_t length,
                              uint8_t *p_val);

/**
 ****************************************************************************************
 * @brief Inform application about reception of list of Generic User/Admin/Manufacturer/Client
 * Properties supported by an element
 *
 * @param[in] src           Address of element for which list of properties has been received
 * @param[in] prop_type     Property type (@see enum mm_prop_type)
 * @param[in] p_prop_ids    Pointer to received list of Property IDs
 ****************************************************************************************
 */
void mm_api_send_cli_prop_list_ind(uint16_t src, uint8_t prop_type, uint16_t nb_prop,
                                   uint16_t *p_prop_ids);
#endif //(BLE_MESH_MDL_CLIENT)

/// @} end of group

#endif //_MM_API_INT_
