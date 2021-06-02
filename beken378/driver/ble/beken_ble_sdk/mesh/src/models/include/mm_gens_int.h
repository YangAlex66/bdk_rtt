/**
 ****************************************************************************************
 * @file mm_gens_int.h
 *
 * @brief Header file for Mesh Model Generic Server Module Internal Definitions
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MM_GENS_INT_H_
#define _MM_GENS_INT_H_

/**
 ****************************************************************************************
 * @defgroup MM_GENS Mesh Model Generic Server Module
 * @ingroup MESH_MDL
 * @brief Mesh Model Generic Server Module Internal Definitions
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mm_gens.h"          // Mesh Model Generic Server Module Definitions
#include "mm_api_int.h"       // Mesh Model Application Programming Interface Internal Definitions
#include "mesh_tb_timer.h"    // Mesh Timer Manager Definitions

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Register Generic OnOff Server model for a given local element
 *
 * @param[in] elmt_idx      Element Index
 * @param[in] p_mdl_lid     Pointer to the variable in which allocated model local index
 * will be written
 *
 * @return An error status (@see mesh_err)
 ****************************************************************************************
 */
uint16_t mm_gens_oo_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid);

/**
 ****************************************************************************************
 * @brief Callback function provided to the Binding Manager so that Generic OnOff Server
 * model can be informed about group event
 *
 * @param[in] mdl_lid       Model Local Index
 * @param[in] event         Event
 * @param[in] info          Information (depends on the event type)
 ****************************************************************************************
 */
void mm_gens_oo_cb_grp_event(m_lid_t mdl_lid, uint8_t event, uint8_t info);

/**
 ****************************************************************************************
 * @brief Callback function provided to the Binding Manager so that Gens OnOff state
 * value can be set by main model of the group
 *
 * @param[in] mdl_lid       Model Local Index
 * @param[in] type          Type
 * @param[in] state         State value
 ****************************************************************************************
 */
void mm_gens_oo_cb_trans_req(m_lid_t main_mdl_lid, uint32_t req_model_id, uint8_t trans_type,
                             uint32_t state_delta);
/**
 ****************************************************************************************
 * @brief Callback function provided to the Binding Manager so that Generic OnOff state
 * value can be set by main model of the group
 *
 * @param[in] mdl_lid       Model Local Index
 * @param[in] type          Type
 * @param[in] state         State value
 ****************************************************************************************
 */
void mm_gens_oo_cb_set_state(m_lid_t mdl_lid, uint8_t type, uint32_t state);


void mm_gens_oo_cb_trans_req(m_lid_t main_mdl_lid, uint32_t req_model_id, uint8_t trans_type,
                             uint32_t state_delta);

/**
 ****************************************************************************************
 * @brief Register Generic Level Server model for a given local element
 *
 * @param[in] elmt_idx      Element Index
 * @param[in] p_mdl_lid     Pointer to the variable in which allocated model local index
 * will be written
 *
 * @return An error status (@see mesh_err)
 ****************************************************************************************
 */
uint16_t mm_gens_lvl_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid);

/**
 ****************************************************************************************
 * @brief Callback function provided to the Binding Manager so that Generic Level Server
 * model can be informed about group event
 *
 * @param[in] mdl_lid       Model Local Index
 * @param[in] event         Event
 * @param[in] info          Information (depends on the event type)
 ****************************************************************************************
 */
void mm_gens_lvl_cb_grp_event(m_lid_t mdl_lid, uint8_t event, uint8_t info);

/**
 ****************************************************************************************
 * @brief Callback function provided to the Binding Manager so that Generic Level state
 * value can be set by main model of the group
 *
 * @param[in] mdl_lid       Model Local Index
 * @param[in] type          Type
 * @param[in] state         State value
 ****************************************************************************************
 */
void mm_gens_lvl_cb_set_state(m_lid_t mdl_lid, uint8_t type, uint32_t state);

/**
 ****************************************************************************************
 * @brief Register Generic Power Level Server and Generic Power Level Setup Setup models
 * for a given local element
 *
 * @param[in] elmt_idx      Element Index
 * @param[in] p_mdl_lid     Pointer to the variable in which allocated model local index
 * for the Generic Power Level Server model will be written
 *
 * @return An error status (@see mesh_err)
 ****************************************************************************************
 */
uint16_t mm_gens_plvl_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid);

/**
 ****************************************************************************************
 * @brief Callback function provided to the Binding Manager so that Generic Power Level Server
 * model can be informed about group event
 *
 * @param[in] mdl_lid       Model Local Index
 * @param[in] event         Event
 * @param[in] info          Information (depends on the event type)
 ****************************************************************************************
 */
void mm_gens_plvl_cb_grp_event(m_lid_t mdl_lid, uint8_t event, uint8_t info);

/**
 ****************************************************************************************
 * @brief Callback function provided to the Binding Manager so that Generic Power Level state
 * value can be set by main model of the group
 *
 * @param[in] mdl_lid       Model Local Index
 * @param[in] type          Type
 * @param[in] state         State value
 ****************************************************************************************
 */
void mm_gens_plvl_cb_trans_req(m_lid_t main_mdl_lid, uint32_t req_model_id, uint8_t trans_type,
                               uint32_t state_delta);

/**
 ****************************************************************************************
 * @brief Register Generic Default Transition Time Server model for a given local element
 *
 * @param[in] elmt_idx      Element Index
 *
 * @return An error status (@see mesh_err)
 ****************************************************************************************
 */
uint16_t mm_gens_dtt_register(uint8_t elmt_idx);

/**
 ****************************************************************************************
 * @brief Register Generic Power OnOff Server and Generic Power OnOff Setup Server model
 * for a given local element
 *
 * @param[in] elmt_idx      Element Index
 *
 * @return An error status (@see mesh_err)
 ****************************************************************************************
 */
uint16_t mm_gens_poo_register(uint8_t elmt_idx);

/**
 ****************************************************************************************
 * @brief Register Generic Battery Server model for a given element
 *
 * @param[in] elmt_idx    Element ID
 *
 * @return An error status (@see mesh_err)
 ****************************************************************************************
 */
uint16_t mm_gens_bat_register(uint8_t elmt_idx);

/**
 ****************************************************************************************
 * @brief Inform Generic Battery Server model about Generic Battery state value received
 * from the application
 *
 * @param[in] p_env             Pointer to model environment
 * @param[in] status            Confirmation status
 * @param[in] bat_lvl           Battery level
 * @param[in] time_discharge    Time to discharge
 * @param[in] time_charge       Time to charge
 * @param[in] flags             Flags
 ****************************************************************************************
 */
void mm_gens_bat_cfm(mm_tb_state_mdl_env_t *p_env, uint16_t status, uint8_t bat_lvl,
                     uint32_t time_discharge, uint32_t time_charge, uint8_t flags);

/**
 ****************************************************************************************
 * @brief Register Generic Location Server and Generic Location Setup Server models for
 * a given local element
 *
 * @param[in] elmt_idx      Element Index
 *
 * @return An error status (@see mesh_err)
 ****************************************************************************************
 */
uint16_t mm_gens_loc_register(uint8_t elmt_idx);

/**
 ****************************************************************************************
 * @brief Inform Generic Location Server model about Generic Location Global state value received
 * from the application
 *
 * @param[in] p_env             Pointer to model environment
 * @param[in] status            Confirmation status
 * @param[in] latitude          Global Latitude
 * @param[in] longitude         Global Longitude
 * @param[in] altitude          Global Altitude
 ****************************************************************************************
 */
void mm_gens_loc_global_cfm(mm_tb_state_mdl_env_t *p_env, uint16_t status, int32_t latitude,
                            int32_t longitude, int16_t altitude);

/**
 ****************************************************************************************
 * @brief Inform Generic Location Server model about Generic Location Local state value received
 * from the application
 *
 * @param[in] p_env             Pointer to model environment
 * @param[in] status            Confirmation status
 * @param[in] north             Local North
 * @param[in] east              Local East
 * @param[in] altitude          Local Altitude
 * @param[in] east              East
 * @param[in] altitude          Altitude
 ****************************************************************************************
 */
void mm_gens_loc_local_cfm(mm_tb_state_mdl_env_t *p_env, uint16_t status, int16_t north,
                           int16_t east, int16_t altitude, uint8_t floor, uint16_t uncertainty);

/// @} end of group

#endif //_MM_GENS_INT_
