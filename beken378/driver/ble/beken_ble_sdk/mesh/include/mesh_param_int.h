/**
 ****************************************************************************************
 *
 * @file "mesh_param_int.h
 *
 * @brief Header file for Mesh stack param init Includes
 *
 * Copyright (C) BeKen 2019-2020
 *
 ****************************************************************************************
 */

#ifndef MESH_PARAM_INT_H_
#define MESH_PARAM_INT_H_
#include <stdint.h>

typedef struct mesh_stack_param_int
{
    /// Default beacon interval for transmissions of unprovisioned device beacons (in milliseconds)
    uint32_t m_bcn_default_unprov_bcn_intv_ms; // M_BCN_DEFAULT_UNPROV_BCN_INTV_MS
    /// Default TTL to use for transmission  TODO Need to be managed by Foundation layer
    uint32_t m_ttl_default; //M_TTL_DEFAULT
    /// Number of advertising transmission to perform
    uint32_t m_adv_nb_tx;  // M_ADV_NB_TX
    /// Number of advertising transmission to perform for the network packet.
    uint32_t m_adv_nb_net_tx;
    /// Advertising interval slots
    uint32_t m_adv_interval; //M_ADV_INTERVAL
    /// Advertising interval slots for the network packet.
    uint32_t m_adv_net_interval;
    /// Advertising connectable interval slots
    uint32_t m_adv_con_interval;//M_ADV_CON_INTERVAL
    /// Scanning interval  slots
    uint32_t m_adv_scan_interval;//M_ADV_SCAN_INTERVAL
    /// Duration between two update of Connectable advertising data
    uint32_t m_proxy_con_adv_update_dur; //M_PROXY_CON_ADV_UPDATE_DUR

    uint32_t m_prov_link_timeout; //M_PROV_LINK_TIMEOUT

    uint32_t m_proxy_con_adv_always_on;

    uint32_t m_proxy_conn_keep_en;//PROXY_KEEP_CONN_EN;

} mesh_stack_param_int_t;


extern mesh_stack_param_int_t m_stack_param;
void mesh_stack_param_init(void);

#endif // 
