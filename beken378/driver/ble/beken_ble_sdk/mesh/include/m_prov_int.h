/**
 ****************************************************************************************
 * @file m_prov_int.h
 *
 * @brief Header file for Mesh Provisioning Internal Defines
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_PROV_INT_H_
#define _M_PROV_INT_H_

/**
 ****************************************************************************************
 * @defgroup M_PROV_INT Mesh Provisioning Internal Defines
 * @ingroup MESH
 * @brief Mesh Provisioning Internal Defines
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh_defines.h"
#include "m_tb.h"
#include "mal.h"
#include "m_bearer.h"

/*
 * DEFINES
 ****************************************************************************************
 */
/// size of confirmation inputs length
#define M_PROV_COMP_DATA_LEN                     (161)

/// Link close reasons
enum m_prov_reason
{
    /// The provisioning was successful
    M_PROV_SUCCESS       = 0x00,
    /// The provisioning transaction timed out
    M_PROV_TIMEOUT       = 0x01,
    /// The provisioning failed
    M_PROV_FAIL          = 0x02,
    /// ADV Bearer closed (Not expected)
    M_PROV_BEARER_CLOSED = 0xFE,
    /// No provisioning reason set
    M_PROV_NO_REASON     = 0xFF,
};


/// Role bit meaning
enum m_prov_role
{
    /// Provisioner Role
    M_PROV_ROLE_PROVISIONER  = 0x01,
    /// Device Role
    M_PROV_ROLE_DEVICE       = 0x00,
};

/// Supported bearer type
enum m_prov_bearer_type
{
    /// No bearer configured
    M_PROV_BEARER_NONE,
    /// Advertising bearer
    M_PROV_BEARER_ADV,
    /// GATT provisioning bearer
    M_PROV_BEARER_GATT,
};

/*
 * CALLBACK DEFINITIONS
 ****************************************************************************************
 */


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


typedef struct m_prov_param
{
    /// Device UUID
    uint8_t  dev_uuid[MESH_DEV_UUID_LEN];
    /// URI hash
    uint32_t  uri_hash;
    /// OOB information
    uint16_t  oob_info;
    /// Supported algorithms and other capabilities
    uint16_t algorithms;
    /// Public key OOB information available
    uint8_t  pub_key_oob;
    /// Static OOB information available
    uint8_t  static_oob;
    /// Maximum size of Output OOB supported
    uint8_t  out_oob_size;
    /// Maximum size in octets of Input OOB supported
    uint8_t  in_oob_size;
    /// Supported Output OOB Actions (@see enum m_prov_out_oob)
    uint16_t out_oob_action;
    /// Supported Input OOB Actions (@see enum m_prov_in_oob)
    uint16_t in_oob_action;
    /// Number of elements
    uint8_t  nb_elt;
    /// Bit field providing additional information (@see enum m_prov_info)
    uint8_t  info;
} m_prov_param_t;

/// Provisioning resources needed
typedef struct m_prov_res
{
    /// Delayed execution job
    mal_djob_t               job;
    /// Timer used to detect a transaction timeout (and stop on-going transmission)
    mesh_tb_timer_t              timer_trans_timeout;
    /// Timer used to detect a link communication timeout
    mesh_tb_timer_t              timer_link_timeout;
    /// Timer used to regenerate the ecdh when it generate failed at the first time;
    mesh_tb_timer_t              timer_ecdh_regen;
    bool                         ecdh_regen_start;
    /// Provisioning configuration
    m_prov_param_t            prov_param;
    /// Device public key over OOB
    bool                      oob_pub_key;
    /// Authentication method
    uint8_t                   auth_method;
    /// Authentication action
    uint8_t                   auth_action;
    /// Authentication size
    uint8_t                   auth_size;

    /// Provisioning algorithm state (@see enum m_prov_state)
    uint8_t                   state;
    /// Bit field to check if expected inputs received (@see enum m_prov_comp_inputs)
    uint8_t                   comp_inputs;
    /// ConfirmationInputs =    ProvisioningInvitePDUValue || ProvisioningCapabilitiesPDUValue
    ///                      || ProvisioningStartPDUValue  || PublicKeyProvisioner || PublicKeyDevice
    /// And other computing data required during provisioning
    uint8_t                   comp_data[M_PROV_COMP_DATA_LEN];
    /// Link close reason
    uint8_t                   reason;
#if (BLE_MESH_PROVISIONER == 1)
    m_lid_t                   nwk_key_lid;
    uint16_t                  nwk_key_id;
    uint16_t                  nwk_addr;
    uint8_t                   net_key[MESH_KEY_LEN];
    m_lid_t                   app_key_lid;
    uint16_t                  app_key_id;
    uint8_t                   app_key[MESH_KEY_LEN];
#endif
} m_prov_res_t;

/// Provisioning specific to ADV Bearer
typedef struct m_prov_adv_res
{
    /// Timer used to randomize delay between two transmissions
    mesh_tb_timer_t              timer_tx;
    /// Transmission queue
    common_list_t                 tx_queue;

    /// Buffer used for packet transmission on PB-ADV
    mesh_tb_buf_t               *p_tx_adv_buf;
    /// Link identifier
    uint32_t                  link_id;
    /// Reception buffer
    mesh_tb_buf_t               *p_rx_buf;
    /// Bit Field that contains link information (@see enum m_prov_link_info)
    uint16_t                  link_info;
    /// Transmission transaction number
    uint8_t                   tx_trans_cursor;
    /// Reception transaction number
    uint8_t                   rx_trans_cursor;
    /// Link close reason
    uint8_t                   close_reason;
} m_prov_adv_res_t;


/// Provisioning specific to GATT Bearer
typedef struct m_prov_gatt_res
{
    /// Buffer used for advertising data
    mesh_tb_buf_t               *p_adv_buf;
    /// Mark that ADV bearer is in closing state
    bool                      closing;
} m_prov_gatt_res_t;

/// Provisioning environment structure
typedef struct m_prov_env
{
    /// Bearer API callback connections
    const m_bearer_api_t     *p_bearer_api;
    /// Provisioning resources needed
    m_prov_res_t             *p_res;
    /// Provisioning advertising resources needed if using PB-ADV
    m_prov_adv_res_t         *p_adv_res;
    /// Provisioning advertising resources needed if using PB-ADV
    m_prov_gatt_res_t        *p_gatt_res;

    /// Type of bearer under use (@see enum m_prov_bearer_type)
    uint8_t                   bearer_type;
    /// Opened Bearer local identifier
    m_lid_t                   bearer_lid;
    /// Advertising Bearer local identifier
    m_lid_t                   adv_bearer_lid;

    /// mesh gatt adv data use custom define
    uint8_t gatt_adv_custom;
    uint8_t gatt_adv_len;
    uint8_t gatt_adv_data[GAP_ADV_DATA_LEN];

} m_prov_env_t;

/// Provisioniong buffer environment
typedef struct m_prov_buf_env
{
    /// PDU Type
    uint8_t pdu_type;

    /// Frame Check Sequence
    uint8_t fcs;
    /// Transmission / Reception segmentation mask
    uint8_t seg_mask;
    /// Number of segment
    uint8_t seg_nb;
    /// cursor use to chose next segment to transmit
    uint8_t seg_cursor;
    /// PDU Transmission number
    uint8_t trans_nb;
} m_prov_buf_env_t;

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

/// Environment structure of provisioning layer
extern m_prov_env_t *p_m_prov_env;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize interface with bearer layer.
 ****************************************************************************************
 */
void m_prov_bearer_init(void);

/**
 ****************************************************************************************
 * @brief Send a packet using PB-ADV or PB-GATT
 *
 * @param[in] p_buf          Buffer to send using advertising bearer
 *
 * @return Execution status
 ****************************************************************************************
 */
uint16_t m_prov_bearer_send(mesh_tb_buf_t *p_buf);

/**
 ****************************************************************************************
 * @brief Unprovisioned device link request from a provisioner
 *
 * @param[in] bearer_type Type of bearer
 * @param[in] bearer_lid  Bearer local identifier
 *
 * @return MESH_ERR_NO_ERROR if link is established, another error code else.
 ****************************************************************************************
 */
uint16_t m_prov_link_open(uint8_t bearer_type, m_lid_t bearer_lid);

/**
 ****************************************************************************************
 * @brief Received provisioning link connection closed by peer device
 *
 * @param[in] reason Reason of link disconnection (@see enum m_prov_reason)
 ****************************************************************************************
 */
void m_prov_link_closed(uint8_t reason);

uint16_t m_prov_link_ack(void);

void m_prov_bearer_open(uint8_t bearer_type, m_lid_t bearer_lid);

/**
 ****************************************************************************************
 * @brief Inform that a buffer has been received
 *
 * @param[in] p_buf Pointer to the buffer that contains received data.
 ****************************************************************************************
 */
void m_prov_pdu_rx(mesh_tb_buf_t *p_buf);

/**
 ****************************************************************************************
 * @brief Inform that PDU has been sent (or failed to be sent)
 *
 * @param[in] p_buf  Pointer to the buffer transmitted
 * @param[in] status Transmission status
 ****************************************************************************************
 */
void m_prov_pdu_sent(mesh_tb_buf_t *p_buf, uint16_t status);

#if (BLE_MESH_GATT_PROV)
/**
 ****************************************************************************************
 * @brief Inform that GATT provisioning bearer type properly closed
 ****************************************************************************************
 */
void m_prov_gatt_closed(void);
#endif // (BLE_MESH_GATT_PROV)

/**
 ****************************************************************************************
 * @brief Clean-up (close) a provisioning link over PB-ADV
 ****************************************************************************************
 */
void m_prov_adv_trans_clean_up(void);

/**
 ****************************************************************************************
 * @brief Request PB-ADV to send a PDU
 *
 * @param[in] p_buf Pointer to the buffer that must be sent
 ****************************************************************************************
 */
uint16_t m_prov_adv_trans_send(mesh_tb_buf_t *p_buf);

/**
 ****************************************************************************************
 * @brief  Send a link close control packet over PB-ADV
 *
 * @param[in] reason reason of Link close
 *
 * @return Execution status
 ****************************************************************************************
 */
uint16_t m_prov_adv_trans_link_close_send(uint8_t reason);

/**
 ****************************************************************************************
 * @brief Handle reception of a Data packet over advertising bearer
 *
 * @param[in] p_buf Pointer to the buffer that contains received data.
 ****************************************************************************************
 */
void m_prov_adv_trans_rx(mesh_tb_buf_t *p_buf);

/**
 ****************************************************************************************
 * @brief Handle information of packet transmission
 *
 * @param[in] p_buf  Pointer to the buffer that contains transmitted data
 * @param[in] status Transmission status
 ****************************************************************************************
 */
void m_prov_adv_trans_sent(mesh_tb_buf_t *p_buf, uint16_t status);

void m_prov_adv_trans_pro_start(void );

/**
 ****************************************************************************************
 * @brief Restart the Scanning
 ****************************************************************************************
 */
void m_prov_bearer_scan_start(void);

/**
 ****************************************************************************************
 * @brief Pause the Scanning
 ****************************************************************************************
 */
void m_prov_bearer_scan_stop(void);

/**
 ****************************************************************************************
 * @brief  Close the active bearer
 *
 * @param[in] reason reason of Link close
 ****************************************************************************************
 */
void m_prov_bearer_close(uint8_t reason);

/**
 ****************************************************************************************
 * @brief Connect the Provisioning layer to bearer functions.
 *
 * @param[in] p_api          Pointer to bearer API.
 ****************************************************************************************
 */
void m_prov_bearer_connect_api(const m_bearer_api_t *p_api);

#if (BLE_MESH_GATT_PROV)
/**
 ****************************************************************************************
 * @brief Open The GATT Provisioning bearer type
 ****************************************************************************************
 */
void m_prov_bearer_gatt_start(void);

void m_prov_bearer_gatt_custom_adv_set(uint8_t *adv_data, uint8_t len);

/**
 ****************************************************************************************
 * @brief Close The GATT Provisioning bearer type
 ****************************************************************************************
 */
void m_prov_bearer_gatt_stop(void);

/**
 ****************************************************************************************
 * @brief Send a PDU over a GATT Provisioning bearer
 *
 * @param[in] p_buf Provisioning PDU to transmit
 *
 * @return Execution status
 ****************************************************************************************
 */
uint16_t m_prov_bearer_gatt_send(mesh_tb_buf_t *p_buf);
#endif // (BLE_MESH_GATT_PROV)

/*
 * API AND CALLBACK FUNCTIONS
 ****************************************************************************************
 */

/// Bearer Layer API functions
extern const m_bearer_cb_t m_prov_bearer_cb;

/// @} end of group

#endif //_M_PROV_INT_H_
