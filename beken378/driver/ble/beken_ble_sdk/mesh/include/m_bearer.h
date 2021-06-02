/**
 ****************************************************************************************
 *
 * @file m_bearer.h
 *
 * @brief Header file for Mesh Bearer Layer
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_BEARER_H_
#define _M_BEARER_H_

/**
 ****************************************************************************************
 * @defgroup M_BEARER Mesh Bearer Layer Defines
 * @ingroup MESH
 * @brief Mesh Bearer Defines
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mal.h"       // Mesh abstraction layer defines
#include "m_lay.h"     // Mesh layer defines
#include "m_defines.h"
#include "m_config.h"

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Bearer action
enum m_bearer_action
{
    /// Close the bearer
    M_BEARER_ACT_CLOSE = 0,
    /// Pause the bearer
    M_BEARER_ACT_PAUSE,
    /// Resume the bearer
    M_BEARER_ACT_RESUME,
};

/// Supported bearer type
enum m_bearer_type
{
    /// Advertising bearer
    M_BEARER_TYPE_ADV,
#if (BLE_MESH_GATT_PROV)
    /// GATT bearer for provisioning
    M_BEARER_TYPE_PROV_GATT,
#endif // (BLE_MESH_GATT_PROV)
#if (BLE_MESH_GATT_PROXY)
    /// GATT bearer for proxy
    M_BEARER_TYPE_PROXY_GATT,
#endif // (BLE_MESH_GATT_PROXY)

    M_BEARER_TYPE_MAX,
};

/// Supported bearer message type
enum m_bearer_msg
{
    /// Network Message
    M_BEARER_MSG_NET     = 0,
    /// Provisioning Message
    M_BEARER_MSG_PROV    = 1,
    /// Beacon Message
    M_BEARER_MSG_BEACON  = 2,
    /// GATT Proxy Message
    M_BEARER_MSG_PROXY   = 3,
};

/// Supported bearer client
enum m_bearer_cli
{
    /// Network Bearer Client
    M_BEARER_CLI_NET    = 0,
    /// Provisioning Bearer Client
    M_BEARER_CLI_PROV   = 1,
    /// Beacon Bearer Client
    M_BEARER_CLI_BEACON = 2,
#if (BLE_MESH_LPN)
    /// Low Power Node Beacon Client
    M_BEARER_CLI_LPN    = 3,
#endif //(BLE_MESH_LPN)
    /// Number of bearer client
    M_BEARER_CLI_NB
};

/*
 * CALLBACK DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Definition of function to call for transmission of a PDU through a layer.
 *
 * @param[in] client          Bearer Client that requests to send a message (@see enum m_bearer_cli)
 * @param[in] p_buf           Pointer to the buffer containing the received PDU.
 *
 * @return Execution status
 ****************************************************************************************
 */
typedef uint16_t (*m_bearer_send_api) (uint8_t client, mesh_tb_buf_t *p_buf);

/**
 ****************************************************************************************
 * @brief Update the bearer, used to close, pause or resume the bearer
 *
 * @param[in] bearer_lid    Bearer Local identifier
 * @param[in] action        Action to execute (@see enum m_bearer_action)
 *
 * @return Execution status
 ****************************************************************************************
 */
typedef uint16_t (*m_bearer_update_api) (m_lid_t bearer_lid, uint8_t action);
/**
 ****************************************************************************************
 * @brief Definition of callback function to call upon reception of a PDU in order to notify
 * upper layer about the received PDU.
 *
 * @param[in] p_buf             Pointer to the buffer containing the received PDU.
 ****************************************************************************************
 */
typedef void (*m_bearer_rx_cb) (mesh_tb_buf_t *p_buf);

/**
 ****************************************************************************************
 * @brief Definition of callback function to call once PDU provided by an upper layer has
 * properly been sent.
 *
 * @param[in] p_buf             Pointer to the buffer containing the sent PDU.
 * @param[in] status            Transmission status.
 ****************************************************************************************
 */
typedef void (*m_bearer_sent_cb) (mesh_tb_buf_t *p_buf, uint16_t status);

/**
 ****************************************************************************************
 * @brief Definition of callback function to call once a bearer has been opened.
 *
 * @param[in] bearer_lid        Bearer local index.
 * @param[in] type              Bearer type.
 ****************************************************************************************
 */
typedef void (*m_bearer_opened_cb) (uint8_t bearer_lid, uint8_t type);

/**
 ****************************************************************************************
 * @brief Definition of callback function to call once a bearer has been closed.
 *
 * @param[in] bearer_lid        Bearer LID.
 ****************************************************************************************
 */
typedef void (*m_bearer_closed_cb) (m_lid_t bearer_lid);

/**
 ****************************************************************************************
 * @brief Definition of callback function to call once a bearer operation has been performed
 *
 * @param[in] status        Status
 ****************************************************************************************
 */
typedef void (*m_bearer_op_end_cb) (uint16_t status);

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Structure for Mesh Bearer Layer API functions
typedef struct m_bearer_api
{
    /// Send PDU API
    m_bearer_send_api   send;
    /// Update the bearer
    m_bearer_update_api update;
} m_bearer_api_t;

/// Structure for Mesh Bearer Layer callback functions
typedef struct m_bearer_cb_
{
    /// PDU sent callback
    m_bearer_sent_cb   sent;
    /// PDU received callback
    m_bearer_rx_cb     rx;
    /// Bearer opened callback
    m_bearer_opened_cb opened;
    /// Bearer closed callback
    m_bearer_closed_cb closed;
} m_bearer_cb_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh bearer layer initialization
 *
 * @note Called when Mesh stack is initialized or when a SW reset is requested.
 *
 * @param[in] reset True means SW reset, False means task initialization
 * @param[in] p_env Pointer to the environment structure
 * @param[in] p_cfg Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_bearer_init(bool reset, void *p_env, const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Mesh bearer layer memory requirement
 *
 * @note This function is called before init function in order to know memory requirements of the module
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this mesh module
 ****************************************************************************************
 */
uint16_t m_bearer_get_env_size(const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Connect the advertising bearer layer to an abstraction layer API functions.
 *
 * @param[in] p_api          Pointer to abstraction layer API.
 ****************************************************************************************
 */
void m_bearer_adv_connect_api_al(const mal_adv_api_t *p_api);

#if (BLE_MESH_GATT_BEARER)
/**
 ****************************************************************************************
 * @brief Connect the GATT bearer layer to an abstraction layer API functions.
 *
 * @param[in] p_api          Pointer to abstraction layer API.
 ****************************************************************************************
 */
void m_bearer_gatt_connect_api_al(const mal_con_api_t *p_api);
#endif // (BLE_MESH_GATT_BEARER)

/**
 ****************************************************************************************
 * @brief Connect the callback functions used by bearer layer to a specific client
 *
 * @param[in] client        Bearer Client type (@see enum m_bearer_cli)
 * @param[in] p_cb          Pointer to callback functions.
 ****************************************************************************************
 */
void m_bearer_connect_client_cb(uint8_t client, const m_bearer_cb_t *p_cb);

/**
 ****************************************************************************************
 * @brief Start usage of bearer type
 *
 * @param[in] bearer_type   Bearer type to control (see enum m_bearer_type)
 * @param[in] data_len      Advertising data length (valid only for GATT based bearer type)
 * @param[in] p_data        Advertising data used in connectable advertising (valid only for GATT based bearer type)
 * @param[in] end_cb        Callback function called once operation has been performed
 ****************************************************************************************
 */
uint16_t m_bearer_start(uint8_t bearer_type, uint16_t data_len, const uint8_t *p_data, m_bearer_op_end_cb cb);

/**
 ****************************************************************************************
 * @brief Stop usage of bearer type
 *
 * @param[in] bearer_type   Bearer type to control (see enum m_bearer_type)
 * @param[in] end_cb        Callback function called once operation has been performed
 ****************************************************************************************
 */
uint16_t m_bearer_stop(uint8_t bearer_type, m_bearer_op_end_cb cb);

/*
 * API AND CALLBACK FUNCTIONS
 ****************************************************************************************
 */

/// Bearer Layer API functions
extern const m_bearer_api_t m_bearer_api;

/// @} end of group

#endif //_M_BEARER_H_
