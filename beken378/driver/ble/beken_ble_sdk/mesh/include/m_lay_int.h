/**
 ****************************************************************************************
 * @file m_lay_int.h
 *
 * @brief Header file for Mesh Layer Internal Defines
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_LAY_INT_H_
#define _M_LAY_INT_H_

/**
 ****************************************************************************************
 * @defgroup M_LAY_INT Mesh Layer Internal Defines
 * @ingroup MESH
 * @brief Mesh Layer Internal Defines
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh_defines.h"
#include "m_defines.h"
#include "m_tb.h"
#include "m_lay.h"
#include "m_bearer.h"

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Flag bit field that contains several information for a buffer environment
enum m_lay_buf_env_flag
{
    /// Bit[0]: 1 = Reception buffer, 0 Transmission buffer
    M_LAY_BUF_RX_POS       = 0,
    M_LAY_BUF_RX_BIT       = 0x01,

    /// Bit[1]: 1 = 64 bits transport MIC, 0 = 32 bits transport MIC
    M_LAY_BUF_SIZE_MIC_POS = 1,
    M_LAY_BUF_SIZE_MIC_BIT = 0x02,

    /// Bit[2]: 1 = Response message, 0 = Publish Message (valid only for transmission)
    M_LAY_BUF_RESP_POS     = 2,
    M_LAY_BUF_RESP_BIT     = 0x04,

    /// Bit[2]: 1 = Received segmented, 0 = Received unsegmented (valid only for reception)
    M_LAY_BUF_RX_SEG_POS   = 2,
    M_LAY_BUF_RX_SEG_BIT   = 0x04,

    /// Bit[3]: 1 = Relayed message, 0 = Normal Message (valid only for transmission)
    M_LAY_BUF_RELAY_POS    = 3,
    M_LAY_BUF_RELAY_BIT    = 0x08,

    /// Bit[4]: 1 = Message sent over a Friendship (valid only for transmission)
    M_LAY_BUF_FRIEND_POS   = 4,
    M_LAY_BUF_FRIEND_BIT   = 0x10,

    /// Bit[5]: 1 = Message targeting a LPN with which friendship is established
    M_LAY_BUF_LPN_POS      = 5,
    M_LAY_BUF_LPN_BIT      = 0x20,

    /// Bit[6]: 1 = Proxy message, 0 = Normal Message (valid only for transmission and reception)
    M_LAY_BUF_PROXY_POS    = 7,
    M_LAY_BUF_PROXY_BIT    = 0x80,
};

/// Interfaces type for Network Layer
enum m_lay_net_intf_type
{
    /// Lower Transport Layer
    M_LAY_NET_INTF_TYPE_LTRANS = 0,
    /// Friend module
    M_LAY_NET_INTF_TYPE_FRIEND,
    /// GATT proxy module
    M_LAY_NET_INTF_TYPE_GATT_PROXY,
};

/// Interfaces type for Upper Transport Layer
enum m_lay_utrans_intf_type
{
    /// Access layer
    M_LAY_UTRANS_INTF_TYPE_ACCESS = 0,
    /// Heartbeat module
    M_LAY_UTRANS_INTF_TYPE_HB,
    /// Low Power Node module
    M_LAY_UTRANS_INTF_TYPE_LPN,
    /// Friend module
    M_LAY_UTRANS_INTF_TYPE_FRIEND,
};

#if (BLE_MESH_GATT_PROXY)
/// Network Message Type received
enum m_lay_proxy_msg_type
{
    /// Mesh Proxy Protocol message
    M_LAY_PROXY_MSG_PROTOCOL = 0,
    /// Message received from a bearer - Encrypted content
    M_LAY_PROXY_MSG_RX,
    /// Message transmitted to advertising bearer - Encrypted content - Not relayed
    M_LAY_PROXY_MSG_ADV_TX,
    /// Message relayed from a proxy connection
    M_LAY_PROXY_MSG_RELAY,
};
#endif // (BLE_MESH_GATT_PROXY)

/// Source layer identifier for control message
enum m_lay_ctl
{
    /// Control message sent by Lower Transport Layer
    M_LAY_CTL_LTRANS = 0,
    /// Control message sent by Heartbeat Layer
    M_LAY_CTL_HB,
    /// Control message sent by Friend Layer
    M_LAY_CTL_FRIEND,
    /// Control message sent by Low Power Node Layer
    M_LAY_CTL_LPN,
    /// Control message sent by Proxy Layer
    M_LAY_CTL_PROXY,
};

/*
 * TYPE DEFINITIONS (API AND CALLBACKS)
 ****************************************************************************************
 */

/// Structure for Mesh Network Layer API functions
typedef struct m_lay_net_api_
{
    /// Send PDU API function
    m_lay_trans_send_api send;
} m_lay_net_api_t;

/// Structure for Mesh Network Layer callback functions
typedef struct m_lay_net_cb_
{
    /// PDU sent callback
    m_lay_sent_cb      sent;
    /// PDU received callback
    m_lay_rx_cb        rx;
    /// Bearer opened callback
    m_bearer_opened_cb opened;
    /// Bearer closed callback
    m_bearer_closed_cb closed;
} m_lay_net_cb_t;

/// Structure for Mesh Lower Transport Layer API functions
typedef struct m_lay_ltrans_api_
{
    /// PDU send callback
    m_lay_send_api send;
} m_lay_ltrans_api_t;

/// Structure for Mesh callback functions used by lower Transport Layer
typedef struct m_lay_ltrans_cb_
{
    /// PDU sent callback
    m_lay_sent_cb sent;
    /// PDU received callback
    m_lay_rx_cb   rx;
} m_lay_ltrans_cb_t;

/// Structure for Mesh Upper Transport Layer API functions
typedef struct m_lay_utrans_api_
{
    /// PDU send callback
    m_lay_trans_send_api send;
} m_lay_utrans_api_t;

/// Structure for Mesh callback functions used by upper transport layer
typedef struct m_lay_utrans_cb_
{
    /// PDU sent callback
    m_lay_sent_cb sent;
    /// PDU received callback
    m_lay_rx_cb   rx;
} m_lay_utrans_cb_t;

/// Structure for Mesh callback functions used by lower transport layer for friendship purpose
typedef struct m_lay_ltrans_friend_cb
{
    /// Received callback
    m_lay_rx_cb   rx;
} m_lay_ltrans_friend_cb_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh Network Layer Initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_lay_net_init(bool reset, void *p_env);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Mesh Network Layer.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_lay_net_get_env_size(const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Connect the network layer to bearer layer API functions.
 *
 * @param[in] p_api          Pointer to abstraction layer API.
 ****************************************************************************************
 */
void m_lay_net_connect_api_bearer(const m_bearer_api_t *p_api);

/**
 ****************************************************************************************
 * @brief Connect the callback functions of the network layer (from lower transport layer).
 *
 * @param[in] p_cb          Pointer to network layer callback functions.
 ****************************************************************************************
 */
void m_lay_net_connect_cb(const m_lay_net_cb_t *p_cb, uint8_t intf_type);

#if (BLE_MESH_LPN)
/**
 ****************************************************************************************
 * @brief Connect the Network Layer to Low Power Node module callback functions.
 *
 * @param[in] p_cb          Pointer to Low Power Node module callback functions.
 ****************************************************************************************
 */
void m_lay_net_connect_cb_lpn_rx(const m_lay_net_cb_t *p_cb);
#endif //(BLE_MESH_LPN)

#if (BLE_MESH_DBG)
/**
 ****************************************************************************************
 * @brief DEBUG PURPOSE - Disable check on received destination address in order to be able
 * to test network layer behavior without registration of any models
 ****************************************************************************************
 */
void m_lay_net_disable_check_dst(void);
#endif //(BLE_MESH_DBG)

/**
 ****************************************************************************************
 * @brief Mesh Lower Transport Layer Initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_lay_ltrans_init(bool reset, void *p_env);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Mesh Lower Transport Layer.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_lay_ltrans_get_env_size(const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Connect the lower transport layer to network layer API functions.
 *
 * @param[in] p_api          Pointer to abstraction layer API.
 ****************************************************************************************
 */
void m_lay_ltrans_connect_api_net(const m_lay_net_api_t *p_api);

/**
 ****************************************************************************************
 * @brief Connect the callback functions used by lower transport layer
 *
 * @param[in] p_cb          Pointer to callback functions.
 ****************************************************************************************
 */
void m_lay_ltrans_connect_cb(const m_lay_ltrans_cb_t *p_cb);

#if (BLE_MESH_FRIEND)
/**
 ****************************************************************************************
 * @brief Connect the callback functions used by lower transport layer for Friend purpose
 *
 * @param[in] p_cb          Pointer to callback functions.
 ****************************************************************************************
 */
void m_lay_ltrans_connect_cb_seg(const m_lay_ltrans_friend_cb_t *p_cb);
#endif //(BLE_MESH_FRIEND)

/**
 ****************************************************************************************
 * @brief Mesh Upper Transport Layer Initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_lay_utrans_init(bool reset, void *p_env);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Mesh Upper Transport Layer.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_lay_utrans_get_env_size(const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Connect the upper transport layer to lower transport layer API functions.
 *
 * @param[in] p_api          Pointer to abstraction layer API.
 ****************************************************************************************
 */
void m_lay_utrans_connect_api_ltrans(const m_lay_ltrans_api_t *p_api);

/**
 ****************************************************************************************
 * @brief Connect the callback functions used by upper transport layer.
 *
 * @param[in] p_cb          Pointer to callback functions.
 * @param[in] intf_type     Interface type (@see enum m_lay_utrans_intf_type)
 ****************************************************************************************
 */
void m_lay_utrans_connect_cb(const m_lay_utrans_cb_t *p_cb, uint8_t intf_type);

/**
 ****************************************************************************************
 * @brief Heartbeat module initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_lay_hb_init(bool reset, void *p_env);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Heartbeat module.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_lay_hb_get_env_size(void);

/**
 ****************************************************************************************
 * @brief Connect the Heartbeat module to Upper Transport Layer API functions.
 *
 * @param[in] p_api          Pointer to abstraction layer API.
 ****************************************************************************************
 */
void m_lay_hb_connect_api_utrans(const m_lay_utrans_api_t *p_api);

#if (BLE_MESH_LPN)
/**
 ****************************************************************************************
 * @brief Low Power Node module initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_lay_lpn_init(bool reset, void *p_env);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Low Power Node module.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_lay_lpn_get_env_size(void);

/**
 ****************************************************************************************
 * @brief Connect the Low Power Node module to Upper Transport Layer API functions.
 *
 * @param[in] p_api          Pointer to abstraction layer API.
 ****************************************************************************************
 */
void m_lay_lpn_connect_api_utrans(const m_lay_utrans_api_t *p_api);

/**
 ****************************************************************************************
 * @brief Connect the Low Power Node module to Bearer API functions in order to start and
 * stop scan.
 *
 * @param[in] p_api          Pointer to bearer API functions
 ****************************************************************************************
 */
void m_lay_lpn_connect_api_bearer(const m_bearer_api_t *p_api);
#endif //(BLE_MESH_LPN)

#if (BLE_MESH_FRIEND)
/**
 ****************************************************************************************
 * @brief Friend module initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_lay_friend_init(bool reset, void *p_env);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Friend Module.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_lay_friend_get_env_size(void);

/**
 ****************************************************************************************
 * @brief Connect the Friend Module to Upper Transport Layer API functions.
 *
 * @param[in] p_api          Pointer to Upper Transport Layer API.
 ****************************************************************************************
 */
void m_lay_friend_connect_api_utrans(const m_lay_utrans_api_t *p_api);

/**
 ****************************************************************************************
 * @brief Connect the Friend Module to Network Layer API functions.
 *
 * @param[in] p_api          Pointer to Lower Transport Layer API.
 ****************************************************************************************
 */
void m_lay_friend_connect_api_net(const m_lay_net_api_t *p_api);
#endif //(BLE_MESH_FRIEND)

/**
 ****************************************************************************************
 * @brief Mesh Access Layer Initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_lay_access_init(bool reset, void *p_env);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Mesh Access Layer.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_lay_access_get_env_size(const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Connect the access layer to upper transport layer API functions.
 *
 * @param[in] p_api          Pointer to abstraction layer API.
 ****************************************************************************************
 */
void m_lay_access_connect_api_utrans(const m_lay_utrans_api_t *p_api);

#if (BLE_MESH_GATT_PROXY)
/**
 ****************************************************************************************
 * @brief Mesh GATT Proxy Initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_lay_proxy_init(bool reset, void *p_env, const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Mesh GATT Proxy.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_lay_proxy_get_env_size(const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Connect the GATT Proxy to bearer functions.
 *
 * @param[in] p_api          Pointer to bearer API.
 ****************************************************************************************
 */
void m_lay_proxy_bearer_connect_api(const m_bearer_api_t *p_api);

/**
 ****************************************************************************************
 * @brief Connect the GATT Proxy to network functions.
 *
 * @param[in] p_api          Pointer to network API.
 ****************************************************************************************
 */
void m_lay_proxy_connect_api_net(const m_lay_net_api_t *p_api);

#endif // (BLE_MESH_GATT_PROXY)

void m_lay_ltrans_set_nb_set_retrans(uint8_t nb_retrans);

uint8_t m_lay_ltrans_get_nb_retrans(void);
/*
 * API AND CALLBACK FUNCTIONS
 ****************************************************************************************
 */

/// Network Layer API functions
extern const m_lay_net_api_t    m_lay_net_api;
/// Network Layer callback functions
extern const m_bearer_cb_t      m_lay_net_cb;

/// Lower Transport Layer API functions
extern const m_lay_ltrans_api_t m_lay_ltrans_api;
/// Lower Transport Layer callback functions
extern const m_lay_net_cb_t     m_lay_ltrans_cb;

/// Upper Transport Layer API functions
extern const m_lay_utrans_api_t m_lay_utrans_api;
/// Upper Transport Layer callback functions
extern const m_lay_ltrans_cb_t  m_lay_utrans_cb;

/// Access Layer callback functions
extern const m_lay_utrans_cb_t  m_lay_access_cb;

/// Heartbeat module callback functions
extern const m_lay_utrans_cb_t  m_lay_hb_cb;
/// Heartbeat module callback functions
extern const m_tb_state_hb_cb_t m_lay_hb_state_cb;

#if (BLE_MESH_LPN)
/// Low Power Node module callback functions for Upper Transport Layer
extern const m_lay_utrans_cb_t  m_lay_lpn_utrans_cb;
/// Low Power Node module callback function for Network Layer
extern const m_lay_net_cb_t     m_lay_lpn_net_cb;
/// Low Power Node module callback function for Bearer Layer
extern const m_bearer_cb_t      m_lay_lpn_bearer_cb;
#endif //(BLE_MESH_LPN)

#if (BLE_MESH_FRIEND)
/// Friend module callback functions for Upper Transport Layer
extern const m_lay_utrans_cb_t m_lay_friend_cb_utrans;
/// Friend module callback functions for Network Layer
extern const m_lay_net_cb_t m_lay_friend_cb_net;
/// Friend module segment reception callback function for Lower Transport Layer
extern const m_lay_ltrans_friend_cb_t m_lay_friend_cb_ltrans;
/// Friend module callback functions
extern const m_tb_state_friend_cb_t m_lay_friend_state_cb;
#endif //(BLE_MESH_FRIEND)

#if (BLE_MESH_GATT_PROXY)
/// Proxy module callback functions for Network Layer
extern const m_lay_net_cb_t m_lay_proxy_cb_net;
#endif //(BLE_MESH_GATT_PROXY)
/// @} end of group

#endif //_M_LAY_INT_H_
