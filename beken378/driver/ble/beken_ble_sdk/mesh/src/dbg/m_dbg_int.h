/**
 ****************************************************************************************
 *
 * @file m_dbg_int.h
 *
 * @brief Header file for Mesh Debug Module internals
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef M_DBG_INT_
#define M_DBG_INT_

/**
 ****************************************************************************************
 * @defgroup M_DBG_INT Mesh Debug Layer internals
 * @ingroup M_DBG_INT
 * @brief  Mesh Debug Layer internals
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "m_config.h"   // Mesh Configuration

#if (BLE_MESH_DBG)

#include "mesh_api_msg.h"      // Mesh Message Application Programming Interface
#include "m_dbg.h"          // Mesh Debug Module
#include "m_tb.h"           // Toolbox for Timer Manager

/*
 * DEFINES
 ****************************************************************************************
 */

/// Debug bearer for advertising
#define M_DBG_DATA_ADV_BEARER_LID    (0xFE)


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Simplified bearer definition
typedef struct m_dbg_bearer
{
    /// Used
    bool    used;
    /// Bearer type
    uint8_t type;
} m_dbg_bearer_t;

/// Environment structure for Debug message (note only one request should be handled)
typedef struct m_dbg_env
{
    /// Pointer to the active command message
    const m_dbg_cmd_t *p_cmd_msg;
    /// Pointer to the command complete message to send (as reply of command request)
    m_dbg_cmp_evt_t   *p_cmp_evt_msg;
    /// List of buffers
    common_list_t         list_buffers;
    /// Bearer instances
    m_dbg_bearer_t    bearers[M_BEARER_MAX];
} m_dbg_env_t;

/*
 * VARIABLES DEFINITIONS
 ****************************************************************************************
 */

/// Mesh debug environment
extern m_dbg_env_t *p_m_dbg_env;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Send a basic Debug command complete message to the application
 *
 * @param[in] cmd_code  Debug Command Operation code (@see enum m_dbg_cmd_code)
 * @param[in] src_id    Source identifier of task that request command execution
 * @param[in] status    Status error code of the command execution (@see enum hl_err)
 ****************************************************************************************
 */
void m_dbg_basic_cmp_evt_send(uint32_t cmd_code, uint16_t src_id, uint16_t status);

/**
 ****************************************************************************************
 * @brief Handle the debug commands for Model IO manager module
 *
 * @param[in] src_id    Command source id
 * @param[in] p_cmd     Command parameters of the message received
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_dbg_mio_exec_cmd(uint16_t src_id, const m_dbg_cmd_t *p_cmd);

/**
 ****************************************************************************************
 * @brief Handle the key manager command request
 *
 * @param[in] p_cmd   Command parameters of the message received
 ****************************************************************************************
 */
void m_dbg_key_exec_cmd(uint16_t src_id, const m_dbg_cmd_t *p_cmd);

/**
 ****************************************************************************************
 * @brief Handle the layer command request
 *
 * @param[in] p_cmd   Command parameters of the message received
 ****************************************************************************************
 */
uint16_t m_dbg_lay_exec_cmd(uint16_t src_id, const m_dbg_cmd_t *p_cmd);

/**
 ****************************************************************************************
 * @brief Send an advertising report to the bearer layer
 *
 * @param[in] rssi          RSSI value of the received ADV report
 * @param[in] adv_len       Length of ADV report packet (without source address)
 * @param[in] p_data        Advertising data payload.
 ****************************************************************************************
 */
void m_dbg_lay_adv_report_send(int8_t rssi, uint8_t adv_len, const uint8_t *p_data);

/**
 ****************************************************************************************
 * @brief Handle the data command request
 *
 * @param[in] p_cmd   Command parameters of the message received
 ****************************************************************************************
 */
uint16_t m_dbg_data_exec_cmd(uint16_t src_id, const m_dbg_cmd_t *p_cmd);

/**
 ****************************************************************************************
 * @brief Handle the state command request
 *
 * @param[in] p_cmd   Command parameters of the message received
 ****************************************************************************************
 */
uint16_t m_dbg_state_exec_cmd(uint16_t src_id, const m_dbg_cmd_t *p_cmd);

/**
 ****************************************************************************************
 * @brief Handle the provisioning command request
 *
 * @param[in] p_cmd   Command parameters of the message received
 ****************************************************************************************
 */
uint16_t m_dbg_prov_exec_cmd(uint16_t src_id, const m_dbg_cmd_t *p_cmd);

/**
 ****************************************************************************************
 * @brief Handle the beaconing command request
 *
 * @param[in] p_cmd   Command parameters of the message received
 ****************************************************************************************
 */
uint16_t m_dbg_bcn_exec_cmd(uint16_t src_id, const m_dbg_cmd_t *p_cmd);

/**
 ****************************************************************************************
 * @brief Handle the storage command request
 *
 * @param[in] p_cmd   Command parameters of the message received
 ****************************************************************************************
 */
uint16_t m_dbg_store_exec_cmd(uint16_t src_id, const m_dbg_cmd_t *p_cmd);

#if (BLE_MESH_LPN)
/**
 ****************************************************************************************
 * @brief Handle the Low Power Node command request
 *
 * @param[in] p_cmd   Command parameters of the message received
 ****************************************************************************************
 */
uint16_t m_dbg_lpn_exec_cmd(uint16_t src_id, const m_dbg_cmd_t *p_cmd);
#endif //(BLE_MESH_LPN)

#if (BLE_MESH_FRIEND)
/**
 ****************************************************************************************
 * @brief Handle the Friend command request
 *
 * @param[in] p_cmd   Command parameters of the message received
 ****************************************************************************************
 */
uint16_t m_dbg_friend_exec_cmd(uint16_t src_id, const m_dbg_cmd_t *p_cmd);
#endif //(BLE_MESH_FRIEND)

#endif //(BLE_MESH_DBG)

/// @} M_DBG_INT

#endif /* M_DBG_INT_ */
