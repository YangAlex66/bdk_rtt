/**
 ****************************************************************************************
 *
 * @file m_prov.h
 *
 * @brief Header file for Mesh Provisioning Module
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_PROV_H_
#define _M_PROV_H_

/**
 ****************************************************************************************
 * @defgroup M_PROV Mesh Provisioning Module
 * @ingroup MESH
 * @brief Mesh Provisioning Module
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh_defines.h"
#include "m_bearer.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh Provisioning Initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_prov_init(bool reset, void *p_env, const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Mesh Provisioning Layer.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_prov_get_env_size(const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Start device provisioning - wait for a provisioner to create a bond
 *
 * @return Execution status code
 ****************************************************************************************
 */
uint16_t m_prov_start(void);

/**
 ****************************************************************************************
 * @brief Start device provisioning - wait for a provisioner to create a bond
 *
 * @return Execution status code
 ****************************************************************************************
 */
uint16_t m_prov_pro_start(void);

/**
 ****************************************************************************************
 * @brief Provide provisioning parameters to the provisioning module
 *
 * @param[in] p_param       Provisioning parameters
 ****************************************************************************************
 */
void m_prov_param_rsp(void *p_param);

/**
 ****************************************************************************************
 * @brief Provide authentication data to the provisioning module
 *
 * @param[in] accept      Accept pairing request
 * @param[in] auth_size   Authentication data size (<= requested size else pairing automatically rejected)
 * @param[in] p_auth_data Authentication data (LSB for a number or array of bytes)
 ****************************************************************************************
 */
void m_prov_oob_auth_rsp(bool accept, uint8_t auth_size, const uint8_t *p_auth_data);

/**
 ****************************************************************************************
 * @brief Get the local public key for out of band transmission of local public key
 *
 * @param[in] p_pub_key_x   X Coordinate of public Key (32 bytes LSB)
 * @param[in] p_pub_key_y   Y Coordinate of public Key (32 bytes LSB)
 *
 * @return Execution status code
 ****************************************************************************************
 */
uint16_t m_prov_pub_key_get(uint8_t *p_pub_key_x, uint8_t *p_pub_key_y);

uint8_t m_prov_pro_add_new_dev(uint8_t *dev_uuid, uint16_t oob_info, uint32_t uri_hash);

uint16_t m_prov_start_dev_prov(uint8_t  algorithm, uint8_t  pub_key, uint8_t  auth_method,
                               uint8_t  auth_action, uint8_t  auth_size, uint8_t * dev_pub_key);

#if (BLE_MESH_PROVISIONER)
uint16_t m_prov_net_key_add(uint16_t net_key_id, const uint8_t *p_net_key, uint8_t flags);
#endif //BLE_MESH_PROVISIONER

/// @} end of group

#endif //_M_PROV_H_
