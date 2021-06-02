/**
 ****************************************************************************************
 * @file mm_lights_int.h
 *
 * @brief Header file for Mesh Model Light Lighting Server Module Internal Definitions
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MM_LIGHTS_INT_H_
#define _MM_LIGHTS_INT_H_

/**
 ****************************************************************************************
 * @defgroup MM_LIGHTS Mesh Model Lighting Server Module
 * @ingroup MESH_MDL
 * @brief Mesh Model Lighting Server Module Internal Definitions
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mm_lights.h"        // Mesh Model Lighting Server Module Definitions
#include "mm_api_int.h"       // Mesh Model Application Programming Interface Internal Definitions
#include "mesh_tb_timer.h"    // Mesh Timer Manager Definitions

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Compure integer square root
 *
 * @param[in] n     Value
 *
 * @return Integer square root of n
 ****************************************************************************************
 */
uint32_t mm_lights_isqrt(uint32_t n);

/**
 ****************************************************************************************
 * @brief Register Light Lightness Server model for a given local element
 *
 * @param[in] elmt_idx      Element Index
 * @param[in] p_mdl_lid     Pointer to the variable in which allocated model local index
 * will be written
 *
 * @return An error status (@see mesh_err)
 ****************************************************************************************
 */
uint16_t mm_lights_ln_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid);

/**
 ****************************************************************************************
 * @brief Callback function provided to the Binding Manager so that Light Lighting Server
 * model can be informed about group event
 *
 * @param[in] mdl_lid       Model Local Index
 * @param[in] event         Event
 * @param[in] info          Information (depends on the event type)
 ****************************************************************************************
 */
void mm_lights_ln_cb_grp_event(m_lid_t mdl_lid, uint8_t event, uint8_t info);

/**
 ****************************************************************************************
 * @brief Callback function provided to the Binding Manager so that Light Lighting state
 * value can be set by main model of the group
 *
 * @param[in] mdl_lid       Model Local Index
 * @param[in] type          Type
 * @param[in] state         State value
 ****************************************************************************************
 */
void mm_lights_ln_cb_trans_req(m_lid_t main_mdl_lid, uint32_t req_model_id, uint8_t trans_type,
                               uint32_t state_delta);

/**
 ****************************************************************************************
 * @brief Callback function provided to the Binding Manager so that Light Lightness state
 * value can be set by main model of the group
 *
 * @param[in] mdl_lid       Model Local Index
 * @param[in] type          Type
 * @param[in] state         State value
 ****************************************************************************************
 */
void mm_lights_ln_cb_set_state(m_lid_t mdl_lid, uint8_t type, uint32_t state);

/**
 ****************************************************************************************
 * @brief Get Light Lightness Range or Light Lightness Default value of an element
 *
 * @param[in] elmt_idx      Element index
 * @param[in] state_id      State Identifier
 ****************************************************************************************
 */
uint16_t mm_lights_ln_get(uint8_t elmt_idx, uint32_t state_id);

/**
 ****************************************************************************************
 * @brief Set Light Lightness Default value of an element
 *
 * @param[in] elmt_idx      Element index
 * @param[in] ln_dflt       Light Lightness Default value
 ****************************************************************************************
 */
void mm_lights_ln_set_dflt(uint8_t elmt_idx, uint16_t ln_dflt);

/**
 ****************************************************************************************
 * @brief Register Light CTL Server, Light CTL Setup Server models on a given element and
 * Light CTL Temperature Server model on next element
 *
 * @param[in] elmt_idx      Element Index
 * @param[in] p_mdl_lid     Pointer to the variable in which allocated model local index
 * will be written
 *
 * @return An error status (@see mesh_err)
 ****************************************************************************************
 */
uint16_t mm_lights_ctl_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid, m_lid_t *p_mdl_lid_temp);

/**
 ****************************************************************************************
 * @brief Callback function provided to the Binding Manager so that Light CTL Server
 * model can be informed about group event
 *
 * @param[in] mdl_lid       Model Local Index
 * @param[in] event         Event
 * @param[in] info          Information (depends on the event type)
 ****************************************************************************************
 */
void mm_lights_ctl_cb_grp_event(m_lid_t mdl_lid, uint8_t event, uint8_t info);

/**
 ****************************************************************************************
 * @brief Callback function provided to the Binding Manager so that Light CTL Temperature Server
 * model can be informed about group event
 *
 * @param[in] mdl_lid       Model Local Index
 * @param[in] event         Event
 * @param[in] info          Information (depends on the event type)
 ****************************************************************************************
 */
void mm_lights_ctl_cb_grp_event_temp(m_lid_t mdl_lid, uint8_t event, uint8_t info);

/**
 ****************************************************************************************
 * @brief Callback function provided to the Binding Manager so that Light Lighting state
 * value can be set by main model of the group
 *
 * @param[in] mdl_lid       Model Local Index
 * @param[in] type          Type
 * @param[in] state         State value
 ****************************************************************************************
 */
void mm_lights_ctl_cb_trans_req(m_lid_t main_mdl_lid, uint32_t req_model_id, uint8_t trans_type,
                                uint32_t state_delta);

/**
 ****************************************************************************************
 * @brief Inform Light CTL Temperature Server model that a transition has been requested
 * using Generic Level model
 *
 * @param[in] main_mdl_lid      Light CTL Temperature Server model local index
 * @param[in] req_model_id      Model ID of model requesting the transition
 * @param[in] trans_type        Transition type
 * @param[in] state_delta       State or delta value
 ****************************************************************************************
 */
void mm_lights_ctl_cb_trans_req_temp(m_lid_t main_mdl_lid, uint32_t req_model_id, uint8_t trans_type,
                                     uint32_t state_delta);

/**
 ****************************************************************************************
 * @brief Register Light HSL Server, Light HSL Setup Server, Light HSL Hue Server and Light
 * HSL Saturation Server models on a given element
 *
 * @param[in] elmt_idx          Index of element on which models must be registered
 * @param[out] p_mdl_lid        Address at which model local index allocated for Light HSL Server
 * model
 * @param[out] p_mdl_lid_hue    Address at which model local index allocated for Light HSL Hue
 * Server model
 * @param[out] p_mdl_lid_sat    Address at which model local index allocated for Light HSL Saturation
 * Server model
 ****************************************************************************************
 */
uint16_t mm_lights_hsl_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid, m_lid_t *p_mdl_lid_hue,
                                m_lid_t *p_mdl_lid_sat);

/**
 ****************************************************************************************
 * @brief Callback function provided to the Binding Manager so that Light HSL Server
 * model can be informed about group event
 *
 * @param[in] mdl_lid       Model Local Index
 * @param[in] event         Event
 * @param[in] info          Information (depends on the event type)
 ****************************************************************************************
 */
void mm_lights_hsl_cb_grp_event(m_lid_t mdl_lid, uint8_t event, uint8_t info);

/**
 ****************************************************************************************
 * @brief Callback function provided to the Binding Manager so that Light HSL Hue or
 * Saturation Server model can be informed about group event
 *
 * @param[in] mdl_lid       Model Local Index
 * @param[in] event         Event
 * @param[in] info          Information (depends on the event type)
 ****************************************************************************************
 */
void mm_lights_hsl_cb_grp_event_hue_sat(m_lid_t mdl_lid, uint8_t event, uint8_t info);

/**
 ****************************************************************************************
 * @brief Callback function provided to the Binding Manager so that Light HSL Hue or Light
 * HSL Saturation state value can be set by main model of the group
 *
 * @param[in] main_mdl_lid       Model Local Index
 * @param[in] req_model_id       Model ID of model that has requested the transition
 * @param[in] trans_type         Transition type
 * @param[in] state_delta        State value or delta value
 ****************************************************************************************
 */
void mm_lights_hsl_cb_trans_req(m_lid_t main_mdl_lid, uint32_t req_model_id, uint8_t trans_type,
                                uint32_t state_delta);

/**
 ****************************************************************************************
 * @brief Callback function provided to the Binding Manager so that Light HSL Hue or Light
 * HSL Saturation state value can be set by main model of the group
 *
 * @param[in] main_mdl_lid       Model Local Index
 * @param[in] req_model_id       Model ID of model that has requested the transition
 * @param[in] trans_type         Transition type
 * @param[in] state_delta        State value or delta value
 ****************************************************************************************
 */
void mm_lights_hsl_cb_trans_req_hue_sat(m_lid_t main_mdl_lid, uint32_t req_model_id, uint8_t trans_type,
                                        uint32_t state_delta);

/// @} end of group

#endif //_MM_LIGHTS_INT_
