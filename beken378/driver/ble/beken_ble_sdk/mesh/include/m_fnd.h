/**
 ****************************************************************************************
 * @file m_fnd.h
 *
 * @brief Header file for Mesh Foundation Models
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_FND_H_
#define _M_FND_H_

/**
 ****************************************************************************************
 * @defgroup M_FND Mesh Foundation Models
 * @ingroup MESH
 * @brief Mesh Foundation Models
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh_defines.h"
#include "mesh_api.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh foundation models initialization
 *
 * @note Called when Mesh stack is initialized or when a SW reset is requested.
 *
 * @param[in] reset     True means SW reset, False means task initialization
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_fnd_init(bool reset, void *p_env, const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Mesh foundation models memory requirement
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this mesh module
 ****************************************************************************************
 */
uint16_t m_fnd_get_env_size(const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Handle reception of Registered Fault state for primary element from the application.
 *
 * @param[in] comp_id           Company ID
 * @param[in] test_id           Test ID
 * @param[in] length            Length of fault array
 * @param[in] p_fault_array     Pointer to fault array content
 ****************************************************************************************
 */
void m_fnd_hlths_status_ind(uint16_t comp_id, uint8_t test_id, uint8_t length, uint8_t *p_fault_array);

/**
 ****************************************************************************************
 * @brief Handle reception of Registered Fault state for primary element from the application.
 *
 * @param[in] comp_id           Company ID
 * @param[in] test_id           Test ID
 * @param[in] length            Length of fault array
 * @param[in] p_fault_array     Pointer to fault array content
 ****************************************************************************************
 */
void m_fnd_hlths_cfm(bool accept, uint16_t comp_id, uint8_t test_id, uint8_t length, uint8_t *p_fault_array);

uint16_t m_fnd_confc_send_app_key_add(m_lid_t  dst_lid);

void m_fnd_confc_send_compo_data_get(m_lid_t  dst_lid, uint8_t page);

void m_fnd_confc_send_model_app_bind(m_lid_t  dst_lid, uint8_t isSIG, uint32_t model_id);

void m_fnd_confc_model_subs_add(m_lid_t dst_lid, bool is_sig, m_lid_t model_lid, uint16_t addr);

void m_fnd_confc_model_subs_del(m_lid_t dst_lid, bool is_sig, m_lid_t model_lid, uint16_t addr);

void m_fnd_confc_send_node_reset(m_lid_t dst_lid);

/// @} end of group

#endif //_M_LAY_FND_H_
