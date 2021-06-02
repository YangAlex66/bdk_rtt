/**
 ****************************************************************************************
 * @file mm_tb_bind.h
 *
 * @brief Header file for Mesh Model Binding Manager
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MM_TB_BIND_H_
#define _MM_TB_BIND_H_

/**
 ****************************************************************************************
 * @defgroup MM_TB_BIND Mesh Model Binding Manager
 * @ingroup MESH
 * @brief Mesh Model Binding Manager
 * @{
 ****************************************************************************************
 */

/*
 * CALLBACK TYPE DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Definition of callback function called by the Binding Manager to inform the main model
 * when one of the model composing a model group has requested transition to a new state.
 *
 * @param[in] main_mdl_lid      Main model local index
 * @param[in] req_model_id      Model ID of model that has requested the transition
 * @param[in] cur_state         Current state value for the model that requests transition to a new state
 * @param[in] tgt_state         Targeted state value for the model that requests transition to a new state
 * (if delta value is 0)
 * @param[in] delta             Delta value to be applied on the provided current state
 * @param[in] trans_time        Transition time to be provided back if transition is accepted
 * @param[in] delay             Delay duration in multiple of 5m to be provided back if transition
 * is accepted TODO
 ****************************************************************************************
 */
typedef void (*mm_tb_bind_trans_req_cb) (m_lid_t main_mdl_lid, uint32_t req_model_id, uint8_t trans_type,
        uint32_t state_delta);

typedef void (*mm_tb_bind_trans_array_req_cb) (m_lid_t main_mdl_lid, uint32_t req_model_id, uint8_t trans_type,
        uint8_t *buf);

/**
 ****************************************************************************************
 * @brief Definition of callback function called by the Binding Manager to inform a model about
 * transition or publication event
 *
 * @param[in] mdl_lid           Model local index
 * @param[in] event             Event (@see enum mesh_mdl_grp_event)
 ****************************************************************************************
 */
typedef void (*mm_tb_bind_grp_event_cb) (m_lid_t mdl_lid, uint8_t event, uint8_t info);

/**
 ****************************************************************************************
 * @brief TODO [LT]
 ****************************************************************************************
 */
typedef void (*mm_tb_bind_set_state_cb) (m_lid_t mdl_lid, uint8_t type, uint32_t state);

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Add a group of bound models
 *
 * @param[in] nb_mdl            Number of models bound with the main model
 * @param[in] elmt_idx          Index of element the models belong to
 * @param[out] p_grp_lid        Pointer to variable in which group local index will be returned
 * @param[in] main_mdl_lid      Local index of the main model
 * @param[in] cb_grp_event      Callback function for communication with the model when a group
 * event occurs
 * @param[in] cb_trans_req      Callback function used to inform the main model about a transition
 * request received by one of its bound model
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t mm_tb_bind_add_group(uint8_t nb_mdl, uint8_t elmt_idx, m_lid_t *p_grp_lid,
                              m_lid_t main_mdl_lid,
                              mm_tb_bind_grp_event_cb cb_grp_event,
                              mm_tb_bind_trans_req_cb cb_trans_req);

/**
 ****************************************************************************************
 * @brief Add a model to a group of bound models
 *
 * @param[in] grp_lid           Group local index
 * @param[in] mdl_lid           Local index of the model
 * @param[in] model_id          Model ID of the model
 * @param[in] cb_grp_event      Callback function for communication with the model when a group
 * event occurs
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t mm_tb_bind_group_add_mdl(m_lid_t grp_lid, m_lid_t mdl_lid, uint32_t model_id,
                                  mm_tb_bind_grp_event_cb cb_grp_event,
                                  mm_tb_bind_set_state_cb cb_set_state);

/**
 ****************************************************************************************
 * @brief Set default transition time value for groups belonging to a given element
 *
 * @param[in] elmt_idx          Element index
 * @param[in] dft_trans_time    Default transition time value
 ****************************************************************************************
 */
void mm_tb_bind_set_dft_trans_time(uint8_t elmt_idx, uint8_t dft_trans_time);

/**
 ****************************************************************************************
 * @brief TODO [LT]
 ****************************************************************************************
 */
uint16_t mm_tb_bind_set_state(m_lid_t grp_lid, uint8_t type, uint32_t model_id, uint32_t state);

/**
 ****************************************************************************************
 * @brief Function used by a bound model to request a new transition.
 *
 * @param[in] grp_lid        Group local index
 * @param[in] mdl_lid        Model local index of model that requires a transition
 * @param[in] trans_type     Transition Type (@see enum mm_trans_type)
 * @param[in] state_delta    Either new state value or delta value to be applied
 * @param[in] trans_time     Transition Time (packed format)
 * @param[in] delay          Delay
 *
 * @return An error status (@see mesh_err)
 ****************************************************************************************
 */
uint16_t mm_tb_bind_trans_req(m_lid_t grp_lid, m_lid_t mdl_lid,
                              uint8_t trans_type, uint32_t state_delta,  uint8_t trans_time,
                              uint8_t delay);

uint16_t mm_tb_bind_trans_array_req(m_lid_t grp_lid, m_lid_t mdl_lid,
                                    uint8_t trans_type, uint8_t *buf,
                                    uint8_t trans_time, uint8_t delay);

/**
 ****************************************************************************************
 * @brief Function called by a main model in order to initiate a new transition
 *
 * @param[in] grp_lid       Group local index
 * @param[in] trans_time    Transition time
 * @param[in] delay         Delay duration in multiple of 5ms
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t mm_tb_bind_trans_new(m_lid_t grp_lid, uint8_t trans_type, uint8_t trans_time, uint8_t delay);

/**
 ****************************************************************************************
 * @brief Function called by a main model in order to reject a transition required by one of its
 * bound models
 *
 * @param[in] grp_lid       Group local index
 ****************************************************************************************
 */
void mm_tb_bind_trans_reject(m_lid_t grp_lid);

/**
 ****************************************************************************************
 * @brief Function called by a main model once all target states have been set in order to start
 * the transition
 *
 * @param[in] grp_lid       Group local index
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t mm_tb_bind_trans_start(m_lid_t grp_lid);

/**
 ****************************************************************************************
 * @brief Check if transition is currently in progress for a group and retrieve the remaining
 * transition time if needed
 *
 * @param[in] grp_lid       Group local index
 * @param[in] p_rem_time    Pointer to the variable in which remaining time will be written
 * if a transition is in progress
 *
 * @return true if transition is in progress else false
 ****************************************************************************************
 */
uint16_t mm_tb_bind_get_trans_info(m_lid_t grp_lid, uint8_t *p_trans_type, uint8_t *p_rem_time);

/// @} end of group

#endif //_MM_TB_BIND
