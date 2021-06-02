/**
 ****************************************************************************************
 * @file m_bearer_int.h
 *
 * @brief Header file for Mesh Bearer Layer Internals
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_BEARER_INT_H_
#define _M_BEARER_INT_H_

/**
 ****************************************************************************************
 * @defgroup M_BEARER_INT Mesh Bearer Layer internal Defines
 * @ingroup M_BEARER
 * @brief Mesh Bearer Layer internal Defines
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "m_bearer.h"       // Mesh Bearer Definitions

/*
 * DEFINES
 ****************************************************************************************
 */
/// Default Advertising bearer LID
#define M_BEARER_ADV_LID      (M_BEARER_MAX-1)
/// Invalid GATT bearer type
#define M_BEARER_GATT_INVALID  (0xFF)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Meaning of bits in the status bit field of a bearer information structure
enum m_bearer_state
{
    /// Indicate if bearer instance is opened
    M_BEARER_OPEN_POS     = 0,
    M_BEARER_OPEN_BIT     = COMMON_BIT(M_BEARER_OPEN_POS),
    /// Indicate that bearer is paused and cannot be used
    M_BEARER_PAUSE_POS    = 1,
    M_BEARER_PAUSE_BIT    = COMMON_BIT(M_BEARER_PAUSE_POS),
    /// Indicate that bearer is transmitting something
    M_BEARER_TX_POS       = 2,
    M_BEARER_TX_BIT       = COMMON_BIT(M_BEARER_TX_POS),
    /// Indicate that bearer is updated
    M_BEARER_UPDATE_POS   = 3,
    M_BEARER_UPDATE_BIT   = COMMON_BIT(M_BEARER_UPDATE_POS),
    /// Indicate that bearer has paused its reception
    M_BEARER_RX_PAUSE_POS = 4,
    M_BEARER_RX_PAUSE_BIT = COMMON_BIT(M_BEARER_RX_PAUSE_POS),
    /// Next update required
    M_BEARER_NUPDATE_POS  = 5,
    M_BEARER_NUPDATE_BIT  = COMMON_BIT(M_BEARER_NUPDATE_POS),
};

/*
 * CALLBACK DEFINITIONS
 ****************************************************************************************
 */

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Structure for Bearer information
typedef struct m_bearer
{
    /// delayed job for transmission
    mal_djob_t djob;
    /// TX queue
    common_list_t   tx_queue;
    /// Bearer type (@see enum m_lay_m_bearer_type)
    uint8_t     type;
    /// State bit field (@see enum m_bearer_state)
    uint8_t     state;
} m_bearer_t;

/// Structure for Advertising Bearer environment
typedef struct m_bearer_adv_env
{
    /// Pointer to Abstraction Layer API
    const mal_adv_api_t  *p_api;
    /// Timer used for different purposes
    mesh_tb_timer_t          timer;
    /// Advertising interval in slots
    uint16_t              intv_slots;
    /// Remaining number of transmissions when transmit interval is below minimum BLE advertising value
    uint8_t               rem_tx;
} m_bearer_adv_env_t;

#if (BLE_MESH_GATT_BEARER)
/// Structure for GATT Bearer environment
typedef struct m_bearer_gatt_env
{
    /// Pointer to Abstraction Layer API
    const mal_con_api_t *p_api;
    /// type of GATT mode PROXY or PROV (enum m_bearer_type)
    uint8_t               type;
} m_bearer_gatt_env_t;
#endif // (BLE_MESH_GATT_BEARER)

/// Structure for Bearer Layer environment
typedef struct m_bearer_env
{
    /// Pointer to callback functions available for Network Layer
    const m_bearer_cb_t  *p_cb[M_BEARER_CLI_NB];
    /// Bearer information
    m_bearer_t            bearers[M_BEARER_MAX];
    /// Operation end callback
    m_bearer_op_end_cb    cb_end_gatt;
    m_bearer_op_end_cb    cb_end_adv;
    /// Advertising bearer parameters
    m_bearer_adv_env_t    adv;
#if (BLE_MESH_GATT_BEARER)
    /// GATT bearer layer parameters
    m_bearer_gatt_env_t   gatt;
#endif // (BLE_MESH_GATT_BEARER)
    /// bit field that contains bearer started
    uint8_t               bearers_started;
    /// Number of opened bearers
    uint8_t               nb_bearers;
} m_bearer_env_t;

/*
 * VARIABLES
 ****************************************************************************************
 */
/// Bearer Layer Environment
extern m_bearer_env_t *p_m_bearer_env;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/*
 * API AND CALLBACK FUNCTIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Function called once a bearer type has been started.
 *
 * @param[in] bearer_lid      Local index of bearer instance to be closed.
 * @param[in] status          Status of execution
 ****************************************************************************************
 */
void m_bearer_started(uint8_t bearer_type, uint8_t status);

/**
 ****************************************************************************************
 * @brief Function called once a bearer type has been stopped.
 *
 * @param[in] bearer_lid      Local index of bearer instance to be closed.
 * @param[in] status          Status of execution
 ****************************************************************************************
 */
void m_bearer_stopped(uint8_t bearer_type, uint8_t status);

/**
 ****************************************************************************************
 * @brief Callback function called once a bearer has been opened.
 *
 * @param[in] bearer_lid      Local index of bearer instance to be closed.
 * @param[in] bearer_type     Bearer type (@see enum m_bearer_type)
 ****************************************************************************************
 */
void m_bearer_opened(m_lid_t bearer_lid, uint8_t bearer_type);

/**
 ****************************************************************************************
 * @brief Function called once a bearer has been closed
 *
 * @param[in] bearer_lid      Local index of bearer instance to be closed.
 * @param[in] status          Status of bearer close
 ****************************************************************************************
 */
void m_bearer_closed(m_lid_t bearer_lid, uint16_t status);

/**
 ****************************************************************************************
 * @brief Function called once PDU pushed by bearer layer has been transmitted.
 *
 * @param[in] bearer_lid        Bearer local index
 * @param[in] status            Status of the packet transmission
 ****************************************************************************************
 */
void m_bearer_sent(uint8_t bearer_lid, uint16_t status);

/**
 ****************************************************************************************
 * @brief Function called upon PDU reception by abstraction layer
 *
 * @param[in] bearer_lid    Bearer LID
 * @param[in] msg_type      Message type (@see enum m_bearer_msg)
 * @param[in] rssi          RSSI value of the received ADV report
 * @param[in] len           Data Length
 * @param[in] p_data        Data payload.
 ****************************************************************************************
 */
void m_bearer_rx(m_lid_t bearer_lid, uint8_t msg_type, int8_t rssi, uint8_t len, const uint8_t *p_data);


/**
 ****************************************************************************************
 * @brief Open advertising bearer .
 *
 * @return MESH_ERR_NO_ERROR if bearer instance has been properly opened.
 *         MESH_ERR_COMMAND_DISALLOWED if no more bearer instance can be created.
 ****************************************************************************************
 */
uint16_t m_bearer_adv_start(void);

/**
 ****************************************************************************************
 * @brief Close Advertising bearer instance.
 *
 * @return MESH_ERR_NO_ERROR if bearer instance has been properly closed.
 *         MESH_ERR_COMMAND_DISALLOWED if indicated bearer instance was already closed.
 ****************************************************************************************
 */
uint16_t m_bearer_adv_stop(void);

/**
 ****************************************************************************************
 * @brief Prepare an advertising buffer for transmission
 *
 * @param[in] p_buf   Pointer to the buffer containing the PDU to send
 *
 * @return MESH_ERR_NO_ERROR if PDU has been properly pushed.
 ****************************************************************************************
 */
uint16_t m_bearer_adv_tx_prepare(mesh_tb_buf_t *p_buf);

/**
 ****************************************************************************************
 * @brief Send a PDU through advertising bearer
 *
 * @param[in] p_buf           Pointer to the buffer containing the PDU to send
 *
 * @return MESH_ERR_NO_ERROR if PDU has been properly pushed.
 ****************************************************************************************
 */
uint16_t m_bearer_adv_send(mesh_tb_buf_t *p_buf);

/**
 ****************************************************************************************
 * @brief Update the bearer, used to close, pause or resume the bearer
 *
 * @param[in] action        Action to execute (@see enum m_bearer_action)
 *
 * @return Execution status
 ****************************************************************************************
 */
uint16_t m_bearer_adv_update(uint8_t action);

#if (BLE_MESH_GATT_BEARER)
/**
 ****************************************************************************************
 * @brief Open GATT bearer .
 *
 * @param[in] type          GATT bearer type of GATT mode PROXY or PROV (see enum m_bearer_type)
 * @param[in] data_len      Advertising data length
 * @param[in] p_data        Advertising data used in connectable advertising
 *
 * @return MESH_ERR_NO_ERROR if bearer instance has been properly opened.
 *         MESH_ERR_COMMAND_DISALLOWED if no more bearer instance can be created.
 ****************************************************************************************
 */
uint16_t m_bearer_gatt_start(uint8_t type, uint16_t data_len, const uint8_t *p_data);

/**
 ****************************************************************************************
 * @brief Update GATT bearer status
 *
 * @param[in] bearer_lid    Bearer LID
 * @param[in] action        Action to execute (@see enum m_bearer_action)
 *
 * @return MESH_ERR_NO_ERROR if bearer instance has been properly opened.
 *         MESH_ERR_COMMAND_DISALLOWED if no more bearer instance can be created.
 ****************************************************************************************
 */
uint16_t m_bearer_gatt_update(uint8_t bearer_lid, uint8_t action);

/**
 ****************************************************************************************
 * @brief Stop GATT bearer.
 *
 * @return MESH_ERR_NO_ERROR if bearer instance has been properly closed.
 *         MESH_ERR_COMMAND_DISALLOWED if indicated bearer instance was already closed.
 ****************************************************************************************
 */
uint16_t m_bearer_gatt_stop(void);

/**
 ****************************************************************************************
 * @brief Prepare an advertising GATT for transmission
 *
 * @param[in] p_buf   Pointer to the buffer containing the PDU to send
 *
 * @return MESH_ERR_NO_ERROR if PDU has been properly pushed.
 ****************************************************************************************
 */
uint16_t m_bearer_gatt_tx_prepare(mesh_tb_buf_t *p_buf);

/**
 ****************************************************************************************
 * @brief Send a PDU through GATT bearer
 *
 * @param[in] p_buf           Pointer to the buffer containing the PDU to send
 *
 * @return MESH_ERR_NO_ERROR if PDU has been properly pushed.
 ****************************************************************************************
 */
uint16_t m_bearer_gatt_send(mesh_tb_buf_t *p_buf);
#endif // (BLE_MESH_GATT_BEARER)

/// @} end of group

#endif //_M_BEARER_INT_H_
