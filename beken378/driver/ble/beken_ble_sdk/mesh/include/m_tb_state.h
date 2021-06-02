/**
 ****************************************************************************************
 * @file m_tb_state.h
 *
 * @brief Header file for State Manager Toolbox
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_TB_STATE_H_
#define _M_TB_STATE_H_

/**
 ****************************************************************************************
 * @defgroup M_TB_STATE State Manager
 * @ingroup MESH
 * @brief State Manager
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "m_api.h"

/*
 * ENUMERATION
 ****************************************************************************************
 */

/// Provisioning state values
enum m_tb_state_prov_state
{
    /// The node is not provisioned
    M_TB_STATE_PROV_STATE_UNPROV = 0x00,
    /// The node is being provisioned
    M_TB_STATE_PROV_STATE_BEING_PROV,
    /// The node is provisioned
    M_TB_STATE_PROV_STATE_PROV
};

/// Heartbeat state update types
enum m_tb_state_hb_update
{
    /// Heartbeat publication parameters have been updated
    M_TB_STATE_UPDATE_HB_PUB = 0,
    /// Heartbeat subscription parameters have been updated
    M_TB_STATE_UPDATE_HB_SUBS,
    /// State of a feature has been updated and an heartbeat message has to be sent for that reason
    M_TB_STATE_UPDATE_HB_FEATURE
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

#if (BLE_MESH_FRIEND)
/**
 ****************************************************************************************
 * @brief Definition of callback function called by State Manager to communicate with layer responsible
 * for Friend feature
 ****************************************************************************************
 */
typedef void (*m_tb_state_friend_updated_cb) (void);

/// Structure for callback functions used by State Manager to communicate with layer responsible
/// for Friend feature
typedef struct m_tb_state_friend_cb
{
    /// State updated callback
    m_tb_state_friend_updated_cb updated;
} m_tb_state_friend_cb_t;
#endif //(BLE_MESH_FRIEND)

/**
 ****************************************************************************************
 * @brief Definition of callback function called by State Manager to communicate with layer responsible
 * for transmission and reception of heartbeat messages
 *
 * @param[in] upd_type          Update type (@see enum m_tb_state_hb_update)
 ****************************************************************************************
 */
typedef void (*m_tb_state_hb_updated_cb) (uint8_t upd_type);

/// Structure for callback functions used by State Manager to communicate with layer responsible
/// for transmission and reception of heartbeat messages
typedef struct m_tb_state_hb_cb
{
    /// State updated callback
    m_tb_state_hb_updated_cb updated;
} m_tb_state_hb_cb_t;

/// Heartbeat publication parameters structure
typedef struct m_tb_state_hb_pub
{
    /// Destination address
    uint16_t dst_addr;
    /// Number of Heartbeat messages remaining to be sent
    uint16_t count;
    /// Period for sending Heartbeat messages in seconds
    uint16_t period_s;
    /// TTL to be used when sending Heartbeat messages
    uint8_t  ttl;
    /// Network key local index
    uint8_t  net_key_lid;
    /// Bitfield indicating features that trigger Heartbeat messages when changed
    uint16_t features;
} m_tb_state_hb_pub_t;

/// Heartbeat subscription parameters structure
typedef struct m_tb_state_hb_subs
{
    /// Remaining period for processing heartbeat messages in seconds
    uint32_t period_s;
    /// Destination address
    uint16_t dst_addr;
    /// Source address
    uint16_t src_addr;
    /// Number of heartbeat messages received
    uint16_t count;
    /// Minimum hops when receiving Heartbeat messages
    uint8_t  min_hops;
    /// Maximum hops when receiving Heartbeat messages
    uint8_t  max_hops;
} m_tb_state_hb_subs_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Set callback function called each time heartbeat parameters are updated
 ****************************************************************************************
 */
void m_tb_state_set_hb_cb(const m_tb_state_hb_cb_t *p_cb_hb);

#if (BLE_MESH_FRIEND)
/**
 ****************************************************************************************
 * @brief Set callback function called each time friend state is updated
 ****************************************************************************************
 */
void m_tb_state_set_friend_cb(const m_tb_state_friend_cb_t *p_cb_friend);
#endif //(BLE_MESH_FRIEND)

/**
 ****************************************************************************************
 * @brief Set enabled state for the mesh profile
 *
 * @param[in] enabled       True if mesh profile has been enabled, else False
 ****************************************************************************************
 */
void m_tb_state_set_enabled(bool enabled);

/**
 ****************************************************************************************
 * @brief Set provisioning state
 *
 * @param[in] prov_state    Provisioning state (@see m_tb_state_prov_state)
 ****************************************************************************************
 */
void m_tb_state_set_prov_state(uint8_t prov_state);

/**
 ****************************************************************************************
 * @brief Set default TTL state value
 *
 * @param[in] default_ttl   Default TTL state value
 *
 * @return Error status
 ****************************************************************************************
 */
uint16_t m_tb_state_set_default_ttl(uint8_t default_ttl);

/**
 ****************************************************************************************
 * @brief Set secure network beacon state value
 *
 * @param[in] bcn_state     Secure network beacon state value
 *
 * @return Error status
 ****************************************************************************************
 */
void m_tb_state_set_beacon_state(uint8_t bcn_state);

#if (BLE_MESH_GATT_PROXY)
/**
 ****************************************************************************************
 * @brief Set GATT proxy state value
 *
 * @param[in] gatt_proxy_state     GATT proxy state value
 *
 * @return Error status
 ****************************************************************************************
 */
void m_tb_state_set_gatt_proxy_state(uint8_t gatt_proxy_state);
#endif //(BLE_MESH_GATT_PROXY)

#if (BLE_MESH_RELAY)
/**
 ****************************************************************************************
 * @brief Set relay state value
 *
 * @param[in] relay_state          Relay state value
 * @param[in] relay_retx_state     Relay retransmission parameters
 *
 * @return Error status
 ****************************************************************************************
 */
void m_tb_state_set_relay_state(uint8_t relay_state, uint8_t relay_retx_state);
#endif //(BLE_MESH_RELAY)

#if (BLE_MESH_FRIEND)
/**
 ****************************************************************************************
 * @brief Set friend state value
 *
 * @param[in] friend_state        Friend state value
 *
 * @return Error status
 ****************************************************************************************
 */
void m_tb_state_set_friend_state(uint8_t friend_state);
#endif //(BLE_MESH_FRIEND)

/**
 ****************************************************************************************
 * @brief Set network transmit state value
 *
 * @param[in] net_tx_params        Network transmit state
 *
 * @return Error status
 ****************************************************************************************
 */
void m_tb_state_set_net_tx_state(uint8_t net_tx_params);

void m_tb_state_set_net_tx_count(uint8_t tx_count);

uint8_t m_tb_state_get_net_tx_count(void);

/**
 ****************************************************************************************
 * @brief Set attention timer state
 *
 * @param[in] attention_state       Attention timer duration (attention state)
 *
 * @return Error status
 ****************************************************************************************
 */
void m_tb_state_set_attention_state(uint8_t attention_state);

/**
 ****************************************************************************************
 * @brief Set heartbeat publication parameters
 *
 * @param[in] dst_addr      Destination address
 * @param[in] count_log     Number of heartbeat messages to send (log value)
 * @param[in] period_log    Duration between publications (log value)
 * @param[in] ttl           TTL value to use for transmission
 * @param[in] features      Bit field providing features for which state update triggers sending
 * of an heartbeat message
 * @param[in] net_key_lid   Network Key local index
 *
 * @return Error status
 ****************************************************************************************
 */
uint16_t m_tb_state_set_hb_pub_params(uint16_t dst_addr, uint8_t count_log, uint8_t period_log,
                                      uint8_t ttl, uint16_t features, m_lid_t net_key_lid);

/**
 ****************************************************************************************
 * @brief Set heartbeat subscription parameters
 *
 * @param[in] src_addr      Source address of heartbeat message to listen to
 * @param[in] period_log    Duration during which we have to listen to heartbeat messages (log value)
 *
 * @return Error status
 ****************************************************************************************
 */
uint16_t m_tb_state_set_hb_subs_params(uint16_t src_addr, uint16_t dst_addr, uint8_t period_log);

#if (BLE_MESH_LPN)
/**
 ****************************************************************************************
 * @brief Indicate that a friendship has been established with a friend node
 *
 * @param[in] estab     True if friendship has been established, false if it has been lost
 ****************************************************************************************
 */
void m_tb_state_set_lpn_state(bool estab);
#endif //(BLE_MESH_LPN)

/**
 ****************************************************************************************
 * @brief Return provisioning state (@see m_tb_state_prov_state)
 ****************************************************************************************
 */
uint8_t m_tb_state_get_prov_state(void);

/**
 ****************************************************************************************
 * @brief Return default TTL state value
 ****************************************************************************************
 */
uint8_t m_tb_state_get_default_ttl(void);

/**
 ****************************************************************************************
 * @brief Return secure network beacon state value
 ****************************************************************************************
 */
uint8_t m_tb_state_get_beacon_state(void);

/**
 ****************************************************************************************
 * @brief Return GATT proxy state value
 ****************************************************************************************
 */
uint8_t m_tb_state_get_gatt_proxy_state(void);

/**
 ****************************************************************************************
 * @brief Return relay state value
 ****************************************************************************************
 */
uint8_t m_tb_state_get_relay_state(uint8_t *p_relay_retx_state);

/**
 ****************************************************************************************
 * @brief Return friend state value (@see m_tb_state_friend_state)
 ****************************************************************************************
 */
uint8_t m_tb_state_get_friend_state(void);

/**
 ****************************************************************************************
 * @brief Return network transmit state value
 ****************************************************************************************
 */
uint8_t m_tb_state_get_net_tx_state(void);

/**
 ****************************************************************************************
 * @brief Get network transmit state
 ****************************************************************************************
 */
void m_tb_state_get_net_tx_params(uint8_t *p_tx_count, uint16_t *p_intv_slots);

/**
 ****************************************************************************************
 * @brief Get Attention timer state
 ****************************************************************************************
 */
uint8_t m_tb_state_get_attention_state(void);

/**
 ****************************************************************************************
 * @brief Get heartbeat publication parameters
 ****************************************************************************************
 */
m_tb_state_hb_pub_t *m_tb_state_get_hb_pub_params(void);

/**
 ****************************************************************************************
 * @brief Get heartbeat subscription parameters
 ****************************************************************************************
 */
m_tb_state_hb_subs_t *m_tb_state_get_hb_subs_params(void);

/**
 ****************************************************************************************
 * @brief Get number of pages for the Composition Data
 ****************************************************************************************
 */
uint8_t m_tb_state_get_nb_cdata_page(void);

/**
 ****************************************************************************************
 * @brief Get values for the first page of the Composition Data
 *
 * @param[out] p_cid        Pointer to variable that will contain the 16-bit company identifier
 * assigned by the Bluetooth SIG
 * @param[out] p_cid        Pointer to variable that will contain the 16-bit vendor-assigned
 * product identifier
 * @param[out] p_vid        Pointer to variable that will contain the 16-bit vendor-assigned
 * product version identifier
 * @param[out] p_features   Pointer to variable that will contain supported features
 * @param[out] p_loc        Pointer to variable that will contain localization descriptor
 ****************************************************************************************
 */
void m_tb_state_get_compo_info(uint16_t *p_cid, uint16_t *p_pid, uint16_t *p_vid,
                               uint16_t *p_features, uint16_t *p_loc);

#if (BLE_MESH_LPN)
/**
 ****************************************************************************************
 * @brief Return if friendship with a Friend node has been established.
 ****************************************************************************************
 */
bool m_tb_state_get_lpn_state(void);
#endif //(BLE_MESH_LPN)

#if (BLE_MESH_DBG)
/**
 ****************************************************************************************
 * @brief Get bit field of supported features
 *
 * @return feature bit field (@see enum m_api_feat)
 ****************************************************************************************
 */
uint32_t m_tb_state_get_features(void);
#endif //(BLE_MESH_DBG)

/**
 ****************************************************************************************
 * @brief Get bitfield indicating features whose state has been updated since last transmission
 * of an Heartbeat message
 ****************************************************************************************
 */
uint8_t m_tb_state_get_hb_feat_upd(void);

/**
 ****************************************************************************************
 * @brief Return if mesh profile is enabled or not.
 ****************************************************************************************
 */
bool m_tb_state_is_enabled(void);

/**
 ****************************************************************************************
 * @brief Get if a feature is supported or not.
 *
 * @param[in] feature  Mesh feature bit (@see enum m_api_feat)
 *
 * @return True if application supports the feature, False if not supported
 ****************************************************************************************
 */
bool m_tb_state_is_feature_sup(uint32_t feature);

/**
 ****************************************************************************************
 * @brief Check if IV update is authorized (can occur only once each 96 hours)
 ****************************************************************************************
 */
bool m_tb_state_is_iv_update_auth(void);

/**
 ****************************************************************************************
 * @brief Inform the State Manager about time at which IV has been updated. IV update won't
 * be possible during 96 hours after the time
 ****************************************************************************************
 */
void m_tb_state_iv_update_ind(uint32_t time_ms, uint16_t nb_wrap);

/**
 ****************************************************************************************
 * @brief Inform the State Manager that a Network Key has been removed. Publication of heartbeat messages
 * using this key is stopped
 *
 * @param[in] net_key_lid   Network Key local index
 ****************************************************************************************
 */
void m_tb_state_net_key_rem_ind(m_lid_t net_key_lid);

/**
 ****************************************************************************************
 * @brief Inform the State Manager about reception of an Heartbeat message
 *
 * @param[in] hops      Hops value read in the received Heartbeat message
 ****************************************************************************************
 */
void m_tb_state_hb_rx_ind(uint8_t hops);

/**
 ****************************************************************************************
 * @brief Inform the State Manager about transmission of an Heartbeat message
 ****************************************************************************************
 */
void m_tb_state_hb_tx_ind(void);

/**
 ****************************************************************************************
 * @brief Inform the State Manager about update of a feature state so that Heartbeat message
 * can be transmitted if needed
 ****************************************************************************************
 */
void m_tb_state_hb_check_features(void);

/**
 ****************************************************************************************
 * @brief Check if a fixed group addresses is supported based on state of the different
 * features.
 ****************************************************************************************
 */
bool m_tb_state_check_fixed_group_addr(uint16_t addr);

/// @} end of group

#endif //_M_TB_STATE_H_
