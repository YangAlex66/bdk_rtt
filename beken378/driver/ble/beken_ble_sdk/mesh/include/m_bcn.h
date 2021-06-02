/**
 ****************************************************************************************
 *
 * @file m_bcn.h
 *
 * @brief Header file for Mesh Beaconing Layer
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_BCN_H_
#define _M_BCN_H_

/**
 ****************************************************************************************
 * @defgroup M_BCN Mesh Beaconing Layer
 * @ingroup MESH
 * @brief Mesh Beaconing Layer
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh_defines.h"      // Mesh defines
#include "m_bearer.h"          // Bearer layer definitions


/// Default beacon interval for transmissions of unprovisioned device beacons (in milliseconds)
#define M_BCN_DEFAULT_UNPROV_BCN_INTV_MS    500 // The unprovisioning beacon is better to set to 100ms, if it is too fast, BLE Mesh provisioning performance will be affected.

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh beaconing layer initialization.
 *
 * @param[in] reset True means SW reset, False means task initialization
 * @param[in] p_env Pointer to the environment structure
 * @param[in] p_cfg Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_bcn_init(bool reset, void *p_env, const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Return mesh beaconing layer memory requirement.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this mesh module.
 ****************************************************************************************
 */
uint16_t m_bcn_get_env_size(const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Inform the beaconing layer that a subnet has been added or removed (linked with
 * add/delete network key procedures).
 *
 * @param net_key_lid       Network key local index
 * @param add               True if subnet has been added, else false
 ****************************************************************************************
 */
void m_bcn_subnet_update_ind(m_lid_t net_key_lid, bool add);

/**
 ****************************************************************************************
 * @brief Request to start transmission of unprovisioned device beacons.
 *
 * @param[in] p_dev_uuid            Pointer to device UUID value
 * @param[in] oob_info              OOB information
 * @param[in] uri_hash              URI hash
 * @param[in] uri_hash_present      Indicate if URI Hash is part of the beacon to send
 ****************************************************************************************
 */
void m_bcn_start_tx_unprov_bcn(uint8_t *p_dev_uuid, uint16_t oob_info, uint32_t uri_hash, bool uri_hash_present);

/**
 ****************************************************************************************
 * @brief Request to stop transmission of unprovisioned device beacons.
 ****************************************************************************************
 */
void m_bcn_stop_tx_unprov_bcn(void);

/**
 ****************************************************************************************
 * @brief Update State of the Beacon procedure
 ****************************************************************************************
 */
void m_bcn_state_update(void);

#if (BLE_MESH_GATT_PROXY)
/**
 ****************************************************************************************
 * @brief New Proxy connection created, send secure network beacon of all available subnets
 ****************************************************************************************
 */
void m_bcn_proxy_con_start(m_lid_t bearer_lid);

/**
 ****************************************************************************************
 * @brief Inform the beacon manager about subnet status update
 ****************************************************************************************
 */
void m_bcn_proxy_subnet_update(m_lid_t net_key_lid);

#endif //(BLE_MESH_GATT_PROXY)

/// @} end of group

#endif //_M_BCN_H_
