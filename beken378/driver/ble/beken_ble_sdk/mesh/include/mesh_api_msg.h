/**
 ****************************************************************************************
 *
 * @file mesh_api_msg.h
 *
 * @brief Header file for Mesh Stack Message Application Programming Interface
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef MESH_API_MSG_
#define MESH_API_MSG_

/**
 ****************************************************************************************
 * @defgroup MESH_API_MSG_ Mesh Stack Message Application Programming Interface
 * @ingroup MESH
 * @brief  Mesh Stack Message Application Programming Interface
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh_defines.h"
#include "mesh_config.h"
#include "mesh_api.h"

/*
 * MACROS
 ****************************************************************************************
 */

/// Retrieve message value from its index
#define MESH_API_MSG_VAL(id)       (TASK_BLE_FIRST_MSG(TASK_BLE_ID_MESH) + (id))

/*
 * ENUMERATIONS FOR MESH STACK
 ****************************************************************************************
 */

/// Mesh Stack Message Indexes
enum mesh_api_msg_id
{
    /* ******************************** */
    /* *         MESH PROFILE         * */
    /* ******************************** */

    /// Mesh Command
    MESH_API_FIRST                           = MESH_API_MSG_VAL(0),
    MESH_API_CMD                             = MESH_API_FIRST,
    /// Command Complete event
    MESH_API_CMP_EVT                         = MESH_API_MSG_VAL(1),

    /// Request model if a specific operation code is supported (shall be confirmed with
    /// MESH_MODEL_OPCODE_CFM message)
    MESH_API_MODEL_OPCODE_REQ_IND            = MESH_API_MSG_VAL(10),
    /// Used to answer if a model support or not a specific operation code.
    MESH_API_MODEL_OPCODE_CFM                = MESH_API_MSG_VAL(11),
    /// Inform reception of a specific mesh message
    MESH_API_MODEL_MSG_IND                   = MESH_API_MSG_VAL(12),
    /// Inform transmission status of message issued by local model
    MESH_API_MODEL_MSG_SENT_IND              = MESH_API_MSG_VAL(13),
    /// Inform about new publication parameters for a model
    MESH_API_MODEL_PUBLISH_PARAM_IND         = MESH_API_MSG_VAL(14),

    /// Inform about updated attention state
    MESH_API_ATTENTION_UPDATE_IND            = MESH_API_MSG_VAL(20),
    /// Inform about requested node reset
    MESH_API_NODE_RESET_IND                  = MESH_API_MSG_VAL(21),

    /// Request a page of the composition data
    MESH_API_COMPO_DATA_REQ_IND              = MESH_API_MSG_VAL(30),
    /// Response containing the required composition data page
    MESH_API_COMPO_DATA_CFM                  = MESH_API_MSG_VAL(31),
    /// Configuration update indication
    MESH_API_UPDATE_IND                      = MESH_API_MSG_VAL(32),

    /// Configuration model app bind indication
    MESH_API_MODEL_APP_BIND_IND                   = MESH_API_MSG_VAL(33),

    MESH_API_KEY_IND                         = MESH_API_MSG_VAL(34),

    /// Configuration Model Subscription indication
    MESH_API_MODEL_SUBS_IND                  = MESH_API_MSG_VAL(35),


    /// Request to get the current Registered Fault state identified by Company ID for primary element
    MESH_API_FAULT_GET_REQ_IND               = MESH_API_MSG_VAL(40),
    /// Request to start a test procedure of primary element
    MESH_API_FAULT_TEST_REQ_IND              = MESH_API_MSG_VAL(41),
    /// Response containing current Registered Fault state for primary element
    MESH_API_FAULT_CFM                       = MESH_API_MSG_VAL(42),
    /// Inform that clear of Registered Fault state identified by Company ID has been received
    /// for primary element
    MESH_API_FAULT_CLEAR_IND                 = MESH_API_MSG_VAL(43),
    /// Inform about new publication period for Current Health state of primary element
    MESH_API_FAULT_PERIOD_IND                = MESH_API_MSG_VAL(44),

    /// Mesh Provisioning state change indication
    MESH_API_PROV_STATE_IND                  = MESH_API_MSG_VAL(50),
    /// Mesh Provisioning parameters request
    MESH_API_PROV_PARAM_REQ_IND              = MESH_API_MSG_VAL(51),
    /// Mesh Provisioning parameters response
    MESH_API_PROV_PARAM_CFM                  = MESH_API_MSG_VAL(52),
    /// Mesh Provisioning Authentication Data Request
    MESH_API_PROV_AUTH_DATA_REQ_IND          = MESH_API_MSG_VAL(53),
    /// Mesh Provisioning Authentication Data response
    MESH_API_PROV_AUTH_DATA_CFM              = MESH_API_MSG_VAL(54),

    /// Mesh Friend Offer reception indication
    MESH_API_LPN_OFFER_IND                   = MESH_API_MSG_VAL(60),
    /// Mesh Friendship status indication
    MESH_API_LPN_STATUS_IND                  = MESH_API_MSG_VAL(61),

    /// Mesh Proxy advertising state update indication
    MESH_API_PROXY_ADV_UPDATE_IND         = MESH_API_MSG_VAL(70),

    /// Mesh Provisioner Data Reports
    MESH_API_PROV_UNPROV_BCN_IND              = MESH_API_MSG_VAL(80),
    /// Mesh prov capability PDU Reports
    MESH_API_PROV_CAPAVILITY_IND              = MESH_API_MSG_VAL(81),
    /// Mesh new device added
    MESH_API_DEV_ADDED_IND                    = MESH_API_MSG_VAL(82),
    /// Mesh new device added
    MESH_API_DEV_COMPO_DATA_STS_IND           = MESH_API_MSG_VAL(83),
    /// Mesh new device added
    MESH_API_APP_KEY_STS_IND                  = MESH_API_MSG_VAL(84),

    MESH_API_MODEL_APP_STS_IND                = MESH_API_MSG_VAL(85),
    MESH_API_MODEL_SUBS_STS_IND               = MESH_API_MSG_VAL(86),
    MESH_API_PROV_START_IND                   = MESH_API_MSG_VAL(87),
    MESH_API_LAST                             = MESH_API_MODEL_SUBS_STS_IND,

    /* ******************************** */
    /* *       MESH PROFILE DEBUG     * */
    /* ******************************** */

    /// Mesh Profile Debug Command
    MESH_DBG_FIRST                           = MESH_API_MSG_VAL(90),
    MESH_DBG_CMD                             = MESH_DBG_FIRST,
    /// Mesh Profile Debug Command Complete Event
    MESH_DBG_CMP_EVT                         = MESH_API_MSG_VAL(91),
    /// Mesh Debug Data Received Indication
    MESH_DBG_DATA_RX_IND                     = MESH_API_MSG_VAL(92),
    /// Mesh Debug Data Send Indication
    MESH_DBG_DATA_SEND_IND                   = MESH_API_MSG_VAL(93),
    /// Mesh Debug Data Sent Indication
    MESH_DBG_DATA_SENT_IND                   = MESH_API_MSG_VAL(94),
    /// Mesh Debug Bearer Opened Indication
    MESH_DBG_BEARER_OPENED_IND               = MESH_API_MSG_VAL(95),
    /// Mesh Debug Bearer Close Indication
    MESH_DBG_BEARER_CLOSED_IND               = MESH_API_MSG_VAL(96),
    /// Mesh Debug Friendship Indication
    MESH_DBG_FRIEND_LPN_IND                  = MESH_API_MSG_VAL(97),
    MESH_DBG_LAST                            = MESH_DBG_FRIEND_LPN_IND,

    /* ******************************** */
    /* *         MESH STACK           * */
    /* ******************************** */

    /// Mesh Stack Command
    MESH_COMMON_API_FIRST                    = MESH_API_MSG_VAL(110),
    MESH_COMMON_API_CMD                      = MESH_COMMON_API_FIRST,
    /// Mesh Stack Command Complete Event
    MESH_COMMON_API_CMP_EVT                  = MESH_API_MSG_VAL(111),
    /// Mesh Stack Indication
    MESH_COMMON_API_IND                      = MESH_API_MSG_VAL(112),
    /// Mesh Stack Request Indication
    MESH_COMMON_API_REQ_IND                  = MESH_API_MSG_VAL(113),
    /// Mesh Stack Confirmation
    MESH_COMMON_API_CFM                      = MESH_API_MSG_VAL(114),
    MESH_COMMON_API_LAST                     = MESH_COMMON_API_CFM,

    /* ******************************** */
    /* *       MESH STACK DEBUG       * */
    /* ******************************** */

    /// Mesh Stack Debug Command
    MESH_COMMON_DBG_FIRST                    = MESH_API_MSG_VAL(120),
    MESH_COMMON_DBG_CMD                      = MESH_COMMON_DBG_FIRST,
    /// Mesh Stack Debug Command Complete Event
    MESH_COMMON_DBG_CMP_EVT                  = MESH_API_MSG_VAL(121),
    /// Mesh Stack Debug Indication
    MESH_COMMON_DBG_IND                      = MESH_API_MSG_VAL(122),
    MESH_COMMON_DBG_LAST                     = MESH_COMMON_DBG_IND,

    /* ******************************** */
    /* *           MESH MODEL         * */
    /* ******************************** */

    /// Mesh Model Command
    MESH_MDL_API_FIRST                       = MESH_API_MSG_VAL(130),
    MESH_MDL_API_CMD                         = MESH_MDL_API_FIRST,
    /// Mesh Model Command Complete Event
    MESH_MDL_API_CMP_EVT                     = MESH_API_MSG_VAL(131),
    /// Mesh Model Indication
    MESH_MDL_API_IND                         = MESH_API_MSG_VAL(132),
    /// Mesh Model Request Indication
    MESH_MDL_API_REQ_IND                     = MESH_API_MSG_VAL(133),
    /// Mesh Model Confirm
    MESH_MDL_API_CFM                         = MESH_API_MSG_VAL(134),
    MESH_MDL_API_LAST                        = MESH_MDL_API_CFM,

    /* ******************************** */
    /* *       MESH MODEL DEBUG       * */
    /* ******************************** */

    /// Mesh Stack Debug Command
    MESH_MDL_DBG_FIRST                       = MESH_API_MSG_VAL(140),
    MESH_MDL_DBG_CMD                         = MESH_MDL_DBG_FIRST,
    /// Mesh Stack Debug Command Complete Event
    MESH_MDL_DBG_CMP_EVT                     = MESH_API_MSG_VAL(141),
    /// Mesh Stack Debug Indication
    MESH_MDL_DBG_IND                         = MESH_API_MSG_VAL(142),
    MESH_MDL_DBG_LAST                        = MESH_MDL_DBG_IND,

    /* ******************************** */
    /* *            INTERNAL          * */
    /* ******************************** */

    /// Message ID for first internal message, all following IDs are reserved
    MESH_INT_MSG                             = MESH_API_MSG_VAL(200),
};

/// Command codes for MESH_COMMON_API_CMD and MESH_COMMON_API_CMP_EVT messages
enum mesh_api_cmd_code
{
    /// Get elapsed time since first device up time
    MESH_API_GET_RUN_TIME               = 0,
    /// Set elapsed time since first device up time
    MESH_API_SET_RUN_TIME               = 1,
    /// Get Mesh Stack version
    MESH_API_GET_VERSION                = 2,

    /// Allocate a new block of buffers
    MESH_API_BUF_ALLOC_BLOCK            = 10,
    /// Free a block of buffers
    MESH_API_BUF_FREE_BLOCK             = 11,
};

/*
 * MESSAGE DEFINITIONS FOR MESH STACK
 ****************************************************************************************
 */

/// Command required structure (without parameters)
typedef struct mesh_api_cmd
{
    /// Command code (@see enum mesh_api_cmd_code)
    uint32_t cmd_code;
} mesh_api_cmd_t;

/// Command complete event required structure (without parameters)
typedef struct mesh_api_cmp_evt
{
    /// Command code (@see enum mesh_cmd_code)
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;
} mesh_api_cmp_evt_t;

/// Set run time command structure
typedef struct mesh_api_set_run_time_cmd
{
    /// Command code (@see enum mesh_cmd_code)
    /// - MESH_API_SET_RUN_TIME: Set run time
    uint32_t cmd_code;
    /// Current clock value in milliseconds
    uint32_t clock_ms;
    /// Number of time the clock has wrapped since up time
    uint16_t nb_wrap;
} mesh_api_set_run_time_cmd_t;

/// Get run time command complete event structure
typedef struct mesh_api_get_run_time_cmp_evt
{
    /// Command code (@see  enum mesh_api_cmd_code)
    /// - MESH_API_GET_RUN_TIME: Get run time
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;

    /// Number of time the clock has wrapped since up time
    uint16_t nb_wrap;
    /// Current clock value in milliseconds
    uint32_t clock_ms;
} mesh_api_get_run_time_cmp_evt_t;

/// Get version command complete event structure
typedef struct mesh_api_get_version_cmp_evt
{
    /// Command code (@see  enum mesh_api_cmd_code)
    /// - MESH_API_GET_VERSION: Get version
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;

    /// Mesh Versions
    mesh_version_t version;
} mesh_api_get_version_cmp_evt_t;

/// Allocate block of buffers command structure
typedef struct mesh_api_buf_alloc_block_cmd
{
    /// Command code (@see enum mesh_cmd_code)
    /// - MESH_API_BUF_ALLOC_BLOCK: Allocate a new block of buffers
    uint32_t cmd_code;
    /// Number of buffers
    uint8_t nb_bufs;
    /// Small (1) or long buffers (0)
    uint8_t small;
} mesh_api_buf_alloc_block_cmd_t;

/// Allocate new block of buffers command complete event structure
typedef struct mesh_api_buf_alloc_block_cmp_evt
{
    /// Command code (@see enum mesh_cmd_code)
    /// - MESH_API_BUF_ALLOC_BLOCK: Allocate a new block of buffers
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;

    /// Block ID
    uint8_t block_id;
} mesh_api_buf_alloc_block_cmp_evt_t;

/// Free block of buffers command structure
typedef struct mesh_api_buf_free_block_cmd
{
    /// Command code (@see enum m_cmd_code)
    /// - MESH_API_BUF_FREE_BLOCK: Free a block of buffers
    uint32_t cmd_code;
    /// Block ID
    uint8_t block_id;
} mesh_api_buf_free_block_cmd_t;

/// Free block of buffers command complete event structure
typedef struct mesh_api_buf_free_block_cmp_evt
{
    /// Command code (@see enum mesh_cmd_code)
    /// - MESH_API_BUF_FREE_BLOCK: Free a block of buffers
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;

    /// Block ID
    uint8_t block_id;
} mesh_api_buf_free_block_cmp_evt_t;

/*
 * ENUMERATIONS FOR MESH PROFILE
 ****************************************************************************************
 */

/// Command codes for MESH_API_CMD and MESH_API_CMP_EVT messages
enum m_api_cmd_code
{
    /// Enable Mesh profile
    M_API_ENABLE                        = 0,
    /// Disable Mesh profile
    M_API_DISABLE                       = 1,
    /// Register a model instance
    M_API_REGISTER_MODEL                = 4,

    M_API_START_UNPROV                  = 10,

    M_API_STOP_UNPROV                   = 11,

    M_API_START_CUS_ADV                 = 12,

    M_API_STOP_CUS_ADV                  = 13,

    M_API_START_CUS_PB_ADV              = 14,

    M_API_STOP_CUS_PB_ADV               = 15,

    /// Model request to publish a new message
    M_API_MODEL_PUBLISH                 = 20,
    /// Model send a response message
    M_API_MODEL_RSP_SEND                = 21,

    /// Publish health current status of primary element
    M_API_HEALTH_STATUS_SEND            = 30,

    /// Read Device public key
    M_API_PROV_PUB_KEY_READ             = 40,

    M_API_PROV_ADD_NEW_DEV             = 41,

    M_API_PROV_START_DEV_PROV         = 42,

    /// IV Update test mode
    M_API_IV_UPD_TEST_MODE              = 50,

    /// Require to start use of Low Power Node feature
    M_API_LPN_START                     = 60,
    /// Require to stop use of Low Power Node feature
    M_API_LPN_STOP                      = 61,
    /// Require to establish a friendship with a given node
    M_API_LPN_SELECT_FRIEND             = 62,

    /// Control the Proxy Connectable mode
    M_API_PROXY_CTL                     = 70,

    /// Load stored information
    M_API_STORAGE_LOAD                  = 80,
    /// Request to force storage of information
    M_API_STORAGE_SAVE                  = 81,
    /// Configure storage module behavior
    M_API_STORAGE_CONFIG                = 82,

    M_API_CONFC_DEV_COMPO_DATA_GET      = 90,
    M_API_CONFC_DEV_APP_KEY_ADD         = 91,
    M_API_CONFC_DEV_APP_BIND            = 92,
    M_API_CONFC_DEV_APP_SUBS_ADD        = 93,
    M_API_CONFC_DEV_APP_SUBS_DEL        = 94,
    M_API_CONFC_DEV_APP_NODE_RESET      = 95,
    
};

/*
 * MESSAGE DEFINITIONS FOR MESH PROFILE
 ****************************************************************************************
 */

/// Command required structure (without parameters)
typedef struct m_api_cmd
{
    /// Command code (@see enum m_cmd_code)
    uint32_t cmd_code;
} m_api_cmd_t;

/// Command complete event required structure (without parameters)
typedef struct m_api_cmp_evt
{
    /// Command code (@see enum m_cmd_code)
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;
} m_api_cmp_evt_t;

/// Register a model instance command structure
typedef struct m_api_register_model_cmd
{
    /// Command code (@see enum m_cmd_code)
    /// - M_API_REGISTER_MODEL: Register a model instance
    uint32_t cmd_code;

    /// Model ID
    uint32_t model_id;
    /// Index of element the models belongs to
    uint8_t elmt_idx;
    /// Configuration
    uint8_t config;
} m_api_register_model_cmd_t;

/// Register a model instance command complete event structure
typedef struct m_api_register_model_cmp_evt
{
    /// Command code (@see enum m_cmd_code)
    /// - M_API_REGISTER_MODEL: Register a model instance
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;

    /// Model LID
    m_lid_t model_lid;
} m_api_register_model_cmp_evt_t;

/// Load stored information command structure
typedef struct m_api_storage_load_cmd
{
    /// Command code (@see enum m_cmd_code)
    /// - M_API_STORAGE_LOAD: Load stored information
    uint32_t cmd_code;

    /// Length
    uint32_t length;
    /// Stored information
    uint8_t data[__ARRAY_EMPTY];
} m_api_storage_load_cmd_t;

/// Configure storage module command structure
typedef struct m_api_storage_config_cmd
{
    /// Command code (@see enum m_cmd_code)
    /// - M_API_STORAGE_CONFIG: Configure storage module
    uint32_t cmd_code;

    /// Configuration (meaning depends on compiled storage module)
    uint32_t config;
} m_api_storage_config_cmd_t;

/// Model request to publish a new message
typedef struct m_api_model_publish_cmd
{
    /// Command code (@see enum m_cmd_code)
    /// - M_API_MODEL_PUBLISH: Model request to publish a new message
    uint32_t cmd_code;
    /// Model Local Identifier
    m_lid_t  model_lid;
    /// Handle value used by model to retrieve which message has been sent
    uint8_t  tx_hdl;
    /// 1 = Segmented PDU force transport MIC to 64 bits ; 0 = 32 bits transport MIC
    uint8_t  trans_mic_64;
    /// Operation code of the message
    uint32_t opcode;
    /// Message length
    uint16_t len;
    /// Message content
    uint8_t  msg[__ARRAY_EMPTY];
} m_api_model_publish_cmd_t;

/// Model send a response message
typedef struct m_api_model_rsp_send_cmd
{
    /// Command code (@see enum m_cmd_code)
    /// - M_API_MODEL_RSP_SEND: Model send a response message
    uint32_t cmd_code;
    /// Model Local Identifier
    m_lid_t  model_lid;
    /// Key information.
    /// If key_lid & 0x80 != 0, key_lid & 0x7F = network key local index
    /// else key_lid & 0x7F = application key local index.
    m_lid_t  key_lid;
    /// Handle value used by model to retrieve which message has been sent
    uint8_t  tx_hdl;
    /// 1 = Segmented PDU force transport MIC to 64 bits ; 0 = 32 bits transport MIC
    uint8_t  trans_mic_64;
    /// Operation code of the message
    uint32_t opcode;
    /// Unicast destination address of the message (source address parameter of received request message)
    uint16_t dst;
    /// 1, send message to an immediate peer; 0, accept message to be relayed
    uint8_t  not_relay;
    /// Message length
    uint16_t len;
    /// Message content
    uint8_t  msg[__ARRAY_EMPTY];

} m_api_model_rsp_send_cmd_t;

/// Model send a response message
typedef struct m_api_health_status_send_cmd
{
    /// Command code (@see enum m_cmd_code)
    /// - M_API_SEND_HEALTH_STATUS: Model send a response message
    uint32_t cmd_code;
    /// Company ID
    uint16_t comp_id;
    /// Test ID
    uint8_t  test_id;
    /// Length of fault array
    uint8_t  length;
    /// Fault array
    uint8_t  fault_array[__ARRAY_EMPTY];
} m_api_health_status_send_cmd_t;

/// Request model if a specific operation code is supported (shall be confirmed with MESH_MODEL_OPCODE_CFM message)
typedef struct m_api_model_opcode_req_ind
{
    /// Model Local Identifier
    m_lid_t  model_lid;
    /// Mesh message operation code (can be 1, 2 or 3 octet operation code)
    uint32_t opcode;
} m_api_model_opcode_req_ind_t;

/// Used to answer if a model support or not a specific operation code.
typedef struct m_api_model_opcode_cfm
{
    /// Model Local Identifier
    m_lid_t  model_lid;
    /// MESH_ERR_OR_NO_ERROR if supported, else rejected
    uint16_t status;
    /// Mesh message operation code (can be 1, 2 or 3 octet operation code)
    uint32_t opcode;
} m_api_model_opcode_cfm_t;

typedef struct m_api_cus_adv_msg
{
    /// Command code (@see enum m_cmd_code)
    uint32_t cmd_code;
    uint32_t adv_data_len;
    uint8_t adv_data[31];
    uint32_t rsp_data_len;
    uint8_t rsp_data[31];
} m_api_cus_adv_msg_t;

/// Inform reception of a specific mesh message
typedef struct m_api_model_msg_ind
{
    /// Model Local Identifier
    m_lid_t  model_lid;
    /// Application Key Local identifier (Required for a response)
    m_lid_t  app_key_lid;
    /// Measured RSSI level for the received PDU.
    int8_t   rssi;
    /// 1 = if message have been received by an immediate peer; 0 = it can have been relayed
    uint8_t  not_relayed;
    /// Mesh message operation code (can be 1, 2 or 3 octet operation code)
    uint32_t opcode;
    /// Source address of the message (Required for a response)
    uint16_t src;
    /// Message length
    uint16_t msg_len;
    /// Message content
    uint8_t  msg[__ARRAY_EMPTY];
} m_api_model_msg_ind_t;

/// Inform transmission status of message issued by local model
typedef struct m_api_model_msg_sent_ind
{
    /// Model Local Identifier
    m_lid_t  model_lid;
    /// Handle value configured by model when message has been requested to be sent
    uint8_t  tx_hdl;
    /// Transmission status
    uint16_t status;
} m_api_model_msg_sent_ind_t;

/// Inform about new publication parameters for a model
typedef struct m_api_model_publish_param_ind
{
    /// Model Local Identifier
    m_lid_t  model_lid;
    /// Publish period in milliseconds
    uint32_t period_ms;
    /// Publication address
    uint16_t addr;
} m_api_model_publish_param_ind_t;

/// Inform about attention state update
typedef struct m_api_attention_update_ind
{
    /// Attention state
    uint8_t attention_state;
} m_api_attention_update_ind_t;

/// Request a page of the composition data
typedef struct m_api_compo_data_req_ind
{
    /// Page
    uint8_t page;
} m_api_compo_data_req_ind_t;

/// Response containing the required composition data page
typedef struct m_api_compo_data_cfm
{
    /// Page
    uint8_t page;
    /// Length
    uint8_t length;
    /// Data
    uint8_t data[__ARRAY_EMPTY];
} m_api_compo_data_cfm_t;

/// Configuration update indication message structure
typedef struct m_api_update_ind
{
    /// Update type
    uint8_t upd_type;
    /// Entry length
    uint8_t length;
    /// Entry value
    uint8_t data[__ARRAY_EMPTY];
} m_api_update_ind_t;

/// Configuration model app bind indication message structure
typedef struct m_api_model_app_bind_ind
{
    uint8_t status;
    uint32_t model_id;
    uint16_t element_addr;
    m_lid_t model_lid;
    m_lid_t app_key_lid;
} m_api_model_app_bind_ind_t;


/// Configuration Model Subscription indication message structure
typedef struct m_api_model_subs_ind
{
    uint8_t status;
    uint32_t model_id;
    uint16_t element_addr;
    uint16_t subs_addr;
    m_lid_t model_lid;
} m_api_model_subs_ind_t;
/// Request to get the current Registered Fault state identified by Company ID for primary element
typedef struct m_api_fault_get_req_ind
{
    /// Company ID
    uint16_t comp_id;
} m_api_fault_get_req_ind_t;

/// Request to start a test procedure of primary element
typedef struct m_api_fault_test_req_ind
{
    /// Company ID
    uint16_t comp_id;
    /// Test ID
    uint8_t  test_id;
    /// Indicate if MESH_FAULT_CFM message is expected
    bool     cfm_needed;
} m_api_fault_test_req_ind_t;

/// Response containing current Registered Fault state for primary element
typedef struct m_api_fault_cfm
{
    /// Accept (true) or reject (false) the request
    bool     accept;
    /// Company ID
    uint16_t comp_id;
    /// Test ID
    uint8_t  test_id;
    /// Length of fault array
    uint8_t  length;
    /// Fault array
    uint8_t  fault_array[__ARRAY_EMPTY];
} m_api_fault_cfm_t;

/// Inform that clear of Registered Fault state identified by Company ID has been received for primary element
typedef struct m_api_fault_clear_ind
{
    /// Company ID
    uint16_t comp_id;
} m_api_fault_clear_ind_t;

/// Inform about new publication period for Current Health state of primary element
typedef struct m_api_fault_period_ind
{
    /// Publication period in milliseconds when no fault is known
    uint32_t period_ms;
    /// Publication period in milliseconds when one or several fault are known
    uint32_t period_fault_ms;
} m_api_fault_period_ind_t;

/// Mesh Provisioning state change indication
typedef struct m_api_prov_state_ind
{
    /// Provisioning procedure state (@see enum m_prov_state)
    uint8_t state;
    /// Relevant only for provisioning failed (failed reason)
    uint16_t status;
} m_api_prov_state_ind_t;

/// Mesh Provisioning state change indication
typedef struct m_api_prov_start_ind
{
    /// Provisioning PDU Type values
    ///  - M_PROV_PDU_START
    uint8_t  type;

    /// The algorithm used for provisioning
    uint8_t  algorithm;
    /// Public Key used
    uint8_t  pub_key;
    /// Authentication Method used
    uint8_t  auth_method;
    /// Selected Output OOB Action or Input OOB Action or 0x00
    uint8_t  auth_action;
    /// Size of the Output OOB used or size of the Input OOB used or 0x00
    uint8_t  auth_size;
} m_api_prov_start_ind_t;

/// Mesh Provisioning Parameters Response
typedef struct m_api_prov_param_cfm
{
    /// Device UUID
    uint8_t  dev_uuid[MESH_DEV_UUID_LEN];
    /// URI hash
    uint32_t uri_hash;
    /// OOB information
    uint16_t oob_info;
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
} m_api_prov_param_cfm_t;

/// Mesh Provisioning Authentication Data Request
typedef struct m_api_prov_auth_data_req_ind
{
    /// Authentication method (@see enum m_prov_auth_method)
    uint8_t  auth_method;
    /// expected authentication maximum data size
    uint8_t  auth_size;
    /// Authentication Action:
    ///   - M_PROV_AUTH_NO_OOB     = Prohibited
    ///   - M_PROV_AUTH_STATIC_OOB = 16 bytes LSB static out of band data required
    ///   - M_PROV_AUTH_OUTPUT_OOB = @see enum m_prov_out_oob, 1 bit set.
    ///   - M_PROV_AUTH_INPUT_OOB  = @see enum m_prov_in_oob, 1 bit set.
    uint16_t auth_action;
} m_api_prov_auth_data_req_ind_t;

/// Mesh Provisioning Authentication Data Response
typedef struct m_api_prov_auth_data_cfm
{
    /// 1, Accept pairing request, 0 reject
    uint8_t  accept;
    /// Authentication data size (<= requested size else pairing automatically rejected)
    uint8_t  auth_size;
    /// Authentication data (LSB for a number or array of bytes)
    uint8_t  auth_data[__ARRAY_EMPTY];
} m_api_prov_auth_data_cfm_t;

/// Read Device public key command
typedef struct m_api_prov_pub_key_read_cmd
{
    /// Command code (@see enum m_cmd_code)
    /// - M_API_PROV_PUB_KEY_READ: Read Device public key
    uint32_t cmd_code;
} m_api_prov_pub_key_read_cmd_t;

/// Read Device public key Command complete event
typedef struct m_api_prov_pub_key_read_cmp_evt
{
    /// Command code (@see enum m_cmd_code)
    /// - M_API_PROV_PUB_KEY_READ: Read Device public key
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;
    /// X Coordinate of public Key (32 bytes LSB)
    uint8_t  pub_key_x[MESH_PUB_KEY_X_LEN];
    /// Y Coordinate of public Key (32 bytes LSB)
    uint8_t  pub_key_y[MESH_PUB_KEY_Y_LEN];
} m_api_prov_pub_key_read_cmp_evt_t;

/// IV Update test mode command
typedef struct m_api_iv_upd_test_mode_cmd
{
    /// Command code (@see enum m_cmd_code)
    /// - M_API_IV_UPD_TEST_MODE: Configure IV Update test mode
    uint32_t cmd_code;
    /// Transit to IV Update in Progress operation (true) or to Normal operation (false)
    bool     update;
} m_api_iv_upd_test_mode_cmd_t;

/// Start Low Power Node feature command structure
typedef struct m_api_lpn_start_cmd
{
    /// Command code (@see enum m_cmd_code)
    /// - M_API_LPN_START: Start Low Power Node feature
    uint32_t cmd_code;

    /// Initial value of PollTimeout timer (in multiple of 100ms)
    uint32_t poll_timeout;
    /// Poll interval (in milliseconds)
    uint32_t poll_intv_ms;
    /// Unicast address of the primary element of the previous friend
    uint16_t prev_addr;
    /// Receive delay
    uint8_t  rx_delay;
    /// RSSI factor
    uint8_t  rssi_factor;
    /// Receive window factor
    uint8_t  rx_window_factor;
    /// Minimum queue size (log value)
    uint8_t  min_queue_size_log;
} m_api_lpn_start_cmd_t;

/// Select Friend Node command structure
typedef struct m_api_lpn_select_friend_cmd
{
    /// Command code (@see enum m_cmd_code)
    /// - M_API_LPN_SELECT_FRIEND: Select Friend node
    uint32_t cmd_code;

    /// Friend node address
    uint16_t friend_addr;
} m_api_lpn_select_friend_cmd_t;

typedef struct m_api_prov_add_new_device_cmd
{
    uint32_t cmd_code;

    uint8_t dev_uuid[MESH_DEV_UUID_LEN];

    uint16_t oob_info;

    uint32_t uri_hash;
} m_api_prov_add_new_device_cmd_t;

typedef struct m_api_prov_start_dev_prov_cmd
{
    uint32_t cmd_code;
    /// The algorithm used for provisioning
    uint8_t  algorithm;
    /// Public Key used
    uint8_t  pub_key;
    /// Authentication Method used
    uint8_t  auth_method;
    /// Selected Output OOB Action or Input OOB Action or 0x00
    uint8_t  auth_action;
    /// Size of the Output OOB used or size of the Input OOB used or 0x00
    uint8_t  auth_size;

    uint8_t * dev_pub_key;
} m_api_prov_start_dev_prov_cmd_t;

typedef struct m_api_confc_dev_compo_data_get_cmd
{
    uint32_t cmd_code;
    /// The algorithm used for provisioning
    uint8_t  dev_lid;
    uint8_t page;
} m_api_confc_dev_compo_data_get_cmd_t;

typedef struct m_api_confc_dev_app_key_add_cmd
{
    uint32_t cmd_code;
    /// The algorithm used for provisioning
    uint8_t  dev_lid;
} m_api_confc_dev_app_key_add_cmd_t;

typedef struct m_api_confc_model_app_bind_cmd
{
    uint32_t cmd_code;
    /// The algorithm used for provisioning
    uint8_t  dev_lid;
    uint8_t  isSIG;
    uint32_t model_id;
} m_api_confc_model_app_bind_cmd_t;

typedef struct
{
   uint32_t cmd_code;
   uint8_t dev_lid;
   uint8_t is_sig;
   m_lid_t model_lid;
   uint16_t addr;
} m_api_confc_model_app_subs_add_cmd_t;

typedef struct
{
    uint32_t cmd_code;
    uint8_t dev_lid;
    uint8_t is_sig;
    m_lid_t model_lid;
    uint16_t addr;
} m_api_confc_model_app_subs_del_cmd_t;

typedef struct
{
    uint32_t cmd_code;
    uint8_t dev_lid;
} m_api_confc_model_app_node_reset_cmd_t;

/// Low Power Node offer reception indication structure
typedef struct m_api_lpn_offer_ind
{
    /// Address of Friend node that sent the Friend Offer message
    uint16_t addr;
    /// Receive window value supported by the Friend node
    uint8_t  rx_window;
    /// Queue size available on the Friend node
    uint8_t  queue_size;
    /// Size of the subscription list that can be supported by the Friend Node
    uint8_t  subs_list_size;
    /// RSSI measured by the Friend node
    int8_t   rssi;
} m_api_lpn_offer_ind_t;

/// Low Power Node status indication structure
typedef struct m_api_lpn_status_ind
{
    /// Status
    uint16_t status;
    /// Friend address
    uint16_t friend_addr;
} m_api_lpn_status_ind_t;

/// Control the Proxy Connectable mode
typedef struct m_api_proxy_ctl_cmd
{
    /// Command code (@see enum m_cmd_code)
    /// - M_API_PROXY_CTL: Control proxy
    uint32_t cmd_code;

    /// 2 - Start connectable advertising with Network ID (duration = 60s)
    /// 1 - Start connectable advertising with None Identity (duration = 60s)
    /// 0 - Stop connectable advertising
    uint8_t enable;
} m_api_proxy_ctl_cmd_t;

/// Indicate that proxy advertising has been started or stopped and the reason
typedef struct m_api_proxy_adv_update_ind
{
    /// State (@see enum mesh_proxy_adv_upd)
    uint8_t state;
    /// Reason (@see enum mesh_proxy_adv_upd_reason)
    uint8_t reason;
} m_api_proxy_adv_update_ind_t;

typedef struct m_api_unprov_beacon_ind
{
    /// Device UUID
    uint8_t dev_uuid[MESH_DEV_UUID_LEN];
    /// OOB information
    uint16_t oob_info;
    uint32_t  uri_hash;
} m_api_unprov_beacon_ind_t;

typedef struct m_api_prov_capabilities_ind
{
    /// Supported algorithms and other capabilities
    uint16_t algorithms;
    /// Supported public key types
    uint8_t  pub_key_type;
    /// Supported static OOB Types
    uint8_t  static_oob_type;
    /// Maximum size of Output OOB supported
    uint8_t  out_oob_size;
    /// Supported Output OOB Actions
    uint16_t out_oob_action;
    /// Maximum size in octets of Input OOB supported
    uint8_t  in_oob_size;
    /// Supported Input OOB Actions
    uint16_t in_oob_action;
} m_api_prov_capabilities_ind_t;

typedef struct m_api_compo_data_status_ind
{
    m_lid_t dev_lid;
    uint8_t data_len;
    uint8_t data[1];
} m_api_compo_data_status_ind_t;

typedef struct m_api_app_key_status_ind
{
    m_lid_t dev_lid;
    uint8_t status;
    uint16_t nwk_key_id;
    uint16_t app_key_id;
} m_api_app_key_status_ind_t;

typedef struct m_api_model_app_status_ind
{
    m_lid_t dev_lid;
    uint8_t status;
    uint16_t address;
    uint16_t app_key_id;
    uint16_t model_id;
} m_api_model_app_status_ind_t;

typedef struct
{
    m_lid_t dev_lid;
    uint8_t status;
    uint16_t elem_addr;
    uint16_t subs_addr;
    uint16_t model_id;
} m_api_model_subs_status_ind_t;

/*
 * MESSAGE DEFINITIONS FOR MESH MODELS
 ****************************************************************************************
 */

/// Command codes for MESH_MDL_API_CMD and MESH_MDL_API_CMP_EVT messages
enum mm_api_cmd_code
{
    /// Register server model configuration
    MM_API_REGISTER_SERVER = 0,
    /// Register client model
    MM_API_REGISTER_CLIENT,
    /// Register Generic User/Admin/Manufacturer/Client Property Server models
    MM_API_REGISTER_SERVER_PROP,

    /// Get state value - for client models
    MM_API_CLI_GET = 10,
    /// Set state value - for client models
    ///     - Generic Default Transition Time
    ///     - Generic Power OnOff
    ///     - Generic Power Range
    ///     - Generic Power Last
    ///     - Generic Power Default
    ///     - Generic Battery
    MM_API_CLI_SET,
    /// Start transition to a new state - for client models
    ///     - Generic OnOff
    ///     - Generic Level
    ///     - Generic Power Actual
    MM_API_CLI_TRANSITION,
    /// Set Generic Location state value (global part) - for Generic Location Client models
    MM_API_CLI_SET_LOCG,
    /// Set Generic Location state value (local part) - for Generic Location Client models
    MM_API_CLI_SET_LOCL,
    /// Get a Generic Property state value or list of Generic Property states
    MM_API_CLI_GET_PROP,
    /// Set a Generic Property state value
    MM_API_CLI_SET_PROP,

    /// Create a group of models that are bound together
    MM_API_GRP_CREATE = 100,
    /// Initiate a new transition
    MM_API_GRP_TRANS_NEW,
    /// Set state of bound models
    MM_API_GRP_SET_STATE,
    /// Start a transition
    MM_API_GRP_TRANS_START,
    /// Get transition information
    MM_API_GRP_GET_INFO,

    /// Set current state
    MM_API_SRV_SET = 200,
};

/// Indication codes for MESH_MDL_API_IND message
enum mm_api_ind_code
{
    /// Model registered indication
    MM_API_REGISTER_IND = 0,

    /// Received state indication for:
    ///    - Generic OnOff Client model
    ///    - Generic Default Transition Time Client model
    ///    - Generic Level Client model
    ///    - Generic Power OnOff Client model
    MM_API_CLI_STATE_IND    = 10,
    /// Received Generic Battery state indication for Generic Battery Client model
    MM_API_CLI_BAT_IND,
    /// Received Generic Location state indication (global part) for Generic Location Client model
    MM_API_CLI_LOCG_IND,
    /// Received Generic Location state indication (local part) for Generic Location Client model
    MM_API_CLI_LOCL_IND,
    /// Get a Generic Property state value or list of Generic Property states
    MM_API_CLI_PROP_IND,
    /// Received list of User or Admin or Manufacturer properties
    MM_API_CLI_PROP_LIST_IND,

    /// State update indication
    MM_API_SRV_STATE_UPD_IND = 100,
    /// Set Generic Location state indication (global part) for Generic Location Server model
    MM_API_SRV_LOCG_UPD_IND,
    /// Set Generic Location state indication (local part) for Generic Location Server model
    MM_API_SRV_LOCL_UPD_IND,
    /// State update indication
    MM_API_SRV_ARRAY_STATE_UPD_IND,

    /// Group event indication
    MM_API_GRP_EVENT_IND = 200,
    /// State set by main model indication
    MM_API_GRP_STATE_IND,
};

/// Indication codes for MESH_MDL_API_REQ_IND message
enum mm_api_req_ind_code
{
    /// Request battery information for a given element
    MM_API_SRV_BAT_REQ_IND = 0,
    /// Request Generic Location state for a given element (global part)
    MM_API_SRV_LOCG_REQ_IND,
    /// Request Generic Location state for a given element (local part)
    MM_API_SRV_LOCL_REQ_IND,
    /// Request to get Generic Property value
    MM_API_SRV_PROP_GET_REQ_IND,
    /// Request to set Generic Property value
    MM_API_SRV_PROP_SET_REQ_IND,

    /// Request start of a new transition to the main model
    MM_API_GRP_TRANS_REQ_IND = 50,
};

/// Minimal content for MESH_MDL_API_CMD message
typedef struct mm_api_cmd
{
    /// Command Code (depends on indicated API)
    uint32_t cmd_code;
} mm_api_cmd_t;

/// Minimal content for MESH_MDL_API_CMP_EVT message
typedef struct mm_api_cmp_evt
{
    /// Command Code (depends on indicated API)
    uint32_t cmd_code;
    /// Status of command execution
    uint16_t status;
} mm_api_cmp_evt_t;

/// Minimal content for MESH_MDL_API_IND message
typedef struct mm_api_ind
{
    /// Indication code (depends on indicated API)
    uint32_t ind_code;
} mm_api_ind_t;

/// Minimal content for MESH_MDL_API_REQ_IND message
typedef struct mm_api_req_ind
{
    /// Request indication code (depends on indicated API)
    uint32_t req_ind_code;
} mm_api_req_ind_t;

/// Minimal content for MESH_MDL_API_CFM message
typedef struct mm_api_cfm
{
    /// Request indication code (depends on indicated API)
    uint32_t req_ind_code;
    /// Status of command execution by application
    uint16_t status;
} mm_api_cfm_t;

/// Register server model configuration command structure
typedef struct mm_api_register_server_cmd
{
    /// Command code (@see enum mm_api_cmd_code)
    /// - MM_API_REGISTER_SERVER: Register server model configuration
    uint32_t cmd_code;

    /// Index of element the model belongs to (zero based)
    uint8_t elmt_idx;
    /// Model configuration index (@see enum mm_cfg_idx)
    uint8_t mdl_cfg_idx;
    /// Informations
    uint8_t info;
} mm_api_register_server_cmd_t;

/// Register client model command structure
typedef struct mm_api_register_client_cmd
{
    /// Command code (@see enum mm_api_cmd_code)
    /// - MM_API_REGISTER_CLIENT: Register client model
    uint32_t cmd_code;

    /// Client Model index (@see enum mm_cmdl_idx)
    uint8_t cmdl_idx;
} mm_api_register_client_cmd_t;

/// Register Generic User/Manufacturer/Admin/Client Property Server model command structure
typedef struct mm_api_register_server_prop_cmd
{
    /// Command code (@see enum mm_api_cmd_code)
    /// - MM_API_REGISTER_SERVER_PROP: Register Generic User/Manufacturer/Admin/Client Server model
    uint32_t cmd_code;

    /// Index of element the model belongs to (zero based)
    uint8_t elmt_idx;
    /// Size of stored message queue
    uint8_t req_queue_len;
    /// Number of Generic User Properties
    uint8_t nb_prop_user;
    /// Number of Generic Admin Properties
    uint8_t nb_prop_admin;
    /// Number of Generic Manufacturer Properties
    uint8_t nb_prop_manuf;
    /// Number of Generic Client Properties
    uint8_t nb_prop_cli;
    /// Property information
    mm_prop_t prop_info[__ARRAY_EMPTY];
} mm_api_register_server_prop_cmd_t;

/// Model registered indication structure
typedef struct mm_api_register_ind
{
    /// Indication code (@see enum mm_api_ind_code)
    /// - MM_API_REGISTER_IND: Model registered
    uint32_t ind_code;

    /// Model ID
    uint32_t model_id;
    /// Model local index
    m_lid_t mdl_lid;
    /// Element index
    uint8_t elmt_idx;
} mm_api_register_ind_t;

/// Get state command structure - for client model
typedef struct mm_api_cli_get_cmd
{
    /// Command code (@see enum mm_api_cmd_code)
    /// - MM_API_CLI_GET: Get state
    uint32_t cmd_code;

    /// Destination
    uint16_t dst;
    /// Get information
    uint16_t get_info;
    /// Client Model local index
    m_lid_t mdl_lid;
} mm_api_cli_get_cmd_t;

/// Set state command structure - for client model
typedef struct mm_api_cli_set_cmd
{
    /// Command code (@see enum mm_api_cmd_code)
    /// - MM_API_CLI_SET: Set state
    uint32_t cmd_code;

    /// State value 1
    uint32_t state_1;
    /// State value 2
    uint32_t state_2;
    /// Destination
    uint16_t dst;
    /// Set information (@see enum mm_set_info)
    uint16_t set_info;
    /// Client Model local index
    m_lid_t mdl_lid;
} mm_api_cli_set_cmd_t;

///  Start transition to a new state command structure - for client model
typedef struct mm_api_cli_transition_cmd
{
    /// Command code (@see enum mm_api_cmd_code)
    /// - MM_API_CLI_TRANSITION: Start transition to a new state
    uint32_t cmd_code;

    /// State value 1
    uint32_t state_1;
    /// State value 2
    uint32_t state_2;
    /// Transition time in milliseconds
    uint32_t trans_time_ms;
    /// Transition information (@see enum mm_trans_info)
    uint16_t trans_info;
    /// Destination
    uint16_t dst;
    /// Delay in milliseconds
    uint16_t delay_ms;
    /// Client Model local index
    m_lid_t mdl_lid;
} mm_api_cli_transition_cmd_t;

/// Set Generic Location state value (global part)
typedef struct mm_api_cli_set_locg_cmd
{
    /// Command code (@see enum mm_api_cmd_code)
    /// - MM_API_CLI_SET_LOCG: Set Generic Location state value (global part)
    uint32_t cmd_code;

    /// Destination
    uint16_t dst;
    /// Client Model local index
    m_lid_t mdl_lid;
    /// Set information (@see enum mm_set_info)
    uint8_t set_info;

    /// Global Latitude
    int32_t latitude;
    /// Global Longitude
    int32_t longitude;
    /// Global Altitude
    int16_t altitude;
} mm_api_cli_set_locg_cmd_t;

/// Set Generic Location state value (global part)
typedef struct mm_api_cli_set_locl_cmd
{
    /// Command code (@see enum mm_api_cmd_code)
    /// - MM_API_CLI_SET_LOCL: Set Generic Location state value (local part)
    uint32_t cmd_code;

    /// Destination
    uint16_t dst;
    /// Client Model local index
    m_lid_t mdl_lid;
    /// Set information (@see enum mm_set_info)
    uint8_t set_info;

    /// Local North
    int16_t north;
    /// Local East
    int16_t east;
    /// Local Altitude
    int16_t altitude;
    /// Uncertainty
    uint16_t uncertainty;
    /// Floor
    uint8_t floor;
} mm_api_cli_set_locl_cmd_t;

/// Get a Generic Property state value or list of Generic Property states
typedef struct mm_api_cli_get_prop_cmd
{
    /// Command code (@see enum mm_api_cmd_code)
    /// - MM_API_CLI_GET_PROP: Get a Generic Property state value or list of Generic Property states
    uint32_t cmd_code;

    /// Destination
    uint16_t dst;
    /// Property ID
    uint16_t prop_id;
    /// Client Model local index
    m_lid_t mdl_lid;
    /// Get type
    uint8_t get_type;
} mm_api_cli_get_prop_cmd_t;

/// Set a Generic Property state value
typedef struct mm_api_cli_set_prop_cmd
{
    /// Command code (@see enum mm_api_cmd_code)
    /// - MM_API_CLI_SET_PROP: Get a Generic Property state value or list of Generic Property states
    uint32_t cmd_code;

    /// Destination
    uint16_t dst;
    /// State value length
    uint16_t length;
    /// Property ID
    uint16_t prop_id;
    /// Client Model local index
    m_lid_t mdl_lid;
    /// Set information
    uint8_t set_info;
    /// User access
    uint8_t user_access;
    /// State value
    uint8_t val[__ARRAY_EMPTY];
} mm_api_cli_set_prop_cmd_t;

/// Set state value
typedef struct mm_api_srv_set_cmd
{
    /// Command code (@see enum mm_api_cmd_code)
    /// - MM_API_SRV_SET: Set state value
    uint32_t cmd_code;

    /// State value
    uint32_t state;
    /// State identifier
    uint16_t state_id;
    /// Model local index
    m_lid_t mdl_lid;
} mm_api_srv_set_cmd_t;

/// Received state indication structure
typedef struct mm_api_cli_state_ind
{
    /// Indication code (@see enum mm_api_ind_code)
    /// - MM_API_CLI_STATE_IND: Received state
    uint32_t ind_code;

    /// Source address
    uint16_t src;
    /// State Identifier
    uint16_t state_id;
    /// State 1
    uint32_t state_1;
    /// State 2
    uint32_t state_2;
    /// Remaining time in milliseconds
    uint32_t rem_time_ms;
} mm_api_cli_state_ind_t;

/// Received Generic Battery state indication structure
typedef struct mm_api_cli_battery_ind
{
    /// Indication code (@see enum mm_api_ind_code)
    /// - MM_API_CLI_BAT_IND: Received Generic Battery State
    uint32_t ind_code;

    /// Time to discharge in minutes
    uint32_t time_discharge;
    /// Time to charge in minutes
    uint32_t time_charge;
    /// Source address
    uint16_t src;
    /// Battery level
    uint8_t bat_lvl;
    /// Flags
    uint8_t flags;
} mm_api_cli_battery_ind_t;

/// Received Generic Location state (global part) indication structure
typedef struct mm_api_cli_locg_ind
{
    /// Indication code (@see enum mm_api_ind_code)
    /// - MM_API_CLI_LOCG_IND: Received Generic Location state indication (global part)
    uint32_t ind_code;

    /// Global Latitude
    int32_t latitude;
    /// Global Longitude
    int32_t longitude;
    /// Global Altitude
    int16_t altitude;

    /// Source address
    uint16_t src;
} mm_api_cli_locg_ind_t;

/// Received Generic Location state (local part) indication structure
typedef struct mm_api_cli_locl_ind
{
    /// Indication code (@see enum mm_api_ind_code)
    /// - MM_API_CLI_LOCL_IND: Received Generic Location state indication (local part)
    uint32_t ind_code;

    /// Source address
    uint16_t src;

    /// Local North
    int16_t north;
    /// Local East
    int16_t east;
    /// Local Altitude
    int16_t altitude;
    /// Uncertainty
    uint16_t uncertainty;
    /// Floor
    uint8_t floor;
} mm_api_cli_locl_ind_t;

/// Received User or Admin or Manufacturer Property value indication structure
typedef struct mm_api_cli_prop_ind
{
    /// Indication code (@see enum mm_api_ind_code)
    /// - MM_API_CLI_PROP_IND: Received User or Admin or Manufacturer Property value
    uint32_t ind_code;

    /// Source address
    uint16_t src;

    /// Property ID
    uint16_t prop_id;
    /// Length
    uint16_t length;
    /// User access
    uint8_t user_access;
    /// Value
    uint8_t val[__ARRAY_EMPTY];
} mm_api_cli_prop_ind_t;

/// Received list of User or Admin or Manufacturer properties indication structure
typedef struct mm_api_cli_prop_list_ind
{
    /// Indication code (@see enum mm_api_ind_code)
    /// - MM_API_CLI_PROP_LIST_IND: Received list of User or Admin or Manufacturer properties
    uint32_t ind_code;

    /// Source address
    uint16_t src;

    /// Number of properties
    uint16_t nb_prop;
    /// Property type
    uint8_t prop_type;
    /// Property IDs
    uint16_t prop_ids[__ARRAY_EMPTY];
} mm_api_cli_prop_list_ind_t;

/// State update indication structure
typedef struct mm_api_srv_state_upd_ind
{
    /// Indication code (@see enum mm_api_ind_code)
    /// - MM_API_SRV_STATE_UPD_IND: Inform application about state update
    uint32_t ind_code;

    /// New state value or targeted state value depending on transition time
    uint32_t state;
    /// Transition time in milliseconds
    uint32_t trans_time_ms;
    /// State identifier (@see mm_state_idx))
    uint16_t state_id;
    /// Element index
    uint8_t elmt_idx;
} mm_api_srv_state_upd_ind_t;

/// State update indication structure
typedef struct mm_api_srv_array_state_upd_ind
{
    /// Indication code (@see enum mm_api_ind_code)
    /// - MM_API_SRV_ARRAY_STATE_UPD_IND: Inform application about state update
    uint32_t ind_code;
    /// data len
    uint8_t len;
    /// Transition time in milliseconds
    uint32_t trans_time_ms;
    /// State identifier (@see mm_state_idx))
    uint16_t state_id;
    /// Element index
    uint8_t elmt_idx;
    /// New state value or targeted state value depending on transition time
    uint8_t val[__ARRAY_EMPTY];
} mm_api_srv_array_state_upd_ind_t;

/// Set Generic Location state (global part) indication structure
typedef struct mm_api_srv_locg_upd_ind
{
    /// Indication code (@see enum mm_api_ind_code)
    /// - MM_API_SRV_LOCG_UPD_IND: Set Generic Location state indication (global part)
    uint32_t ind_code;

    /// Global Latitude
    int32_t latitude;
    /// Global Longitude
    int32_t longitude;
    /// Global Altitude
    int16_t altitude;

    /// Element index
    uint8_t elmt_idx;
} mm_api_srv_locg_upd_ind_t;

/// Set Generic Location state (local part) indication structure
typedef struct mm_api_srv_locl_upd_ind
{
    /// Indication code (@see enum mm_api_ind_code)
    /// - MM_API_SRV_LOCL_UPD_IND: Set Generic Location state indication (local part)
    uint32_t ind_code;

    /// Local North
    int16_t north;
    /// Local East
    int16_t east;
    /// Local Altitude
    int16_t altitude;
    /// Uncertainty
    uint16_t uncertainty;
    /// Floor
    uint8_t floor;

    /// Element index
    uint8_t elmt_idx;
} mm_api_srv_locl_upd_ind_t;

/// Get element state value request indication structure
typedef struct mm_api_srv_state_req_ind
{
    /// Request indication code (@see enum mm_api_req_ind_code)
    uint32_t req_ind_code;

    /// Element index
    uint8_t elmt_idx;
} mm_api_srv_state_req_ind_t;

/// Request to get Generic Property value
typedef struct mm_api_srv_prop_get_req_ind
{
    /// Request indication code
    /// MM_API_SRV_PROP_GET_REQ_IND : Get Generic Property value
    uint32_t req_ind_code;

    /// Element index
    uint8_t elmt_idx;
    /// Property type
    uint8_t prop_type;
    /// Property ID
    uint16_t prop_id;
} mm_api_srv_prop_get_req_ind_t;

/// Request to set Generic Property value
typedef struct mm_api_srv_prop_set_req_ind
{
    /// Request indication code
    /// MM_API_SRV_PROP_SET_REQ_IND : Set Generic Property value
    uint32_t req_ind_code;

    /// Element index
    uint8_t elmt_idx;
    /// Property type
    uint8_t prop_type;
    /// Property ID
    uint16_t prop_id;
    /// Value length
    uint16_t length;
    /// Confirmation expected
    uint8_t ack;
    /// Value
    uint8_t val[__ARRAY_EMPTY];
} mm_api_srv_prop_set_req_ind_t;

/// Message sent by application to return a requested Generic Battery state for an element
typedef struct mm_api_srv_bat_cfm
{
    /// Request indication code (@see enum mm_api_req_ind_code)
    /// MM_API_SRV_BAT_REQ_IND: Generic Battery state
    uint32_t req_ind_code;
    /// Status
    uint16_t status;

    /// Battery level
    uint8_t bat_lvl;
    /// Flags
    uint8_t flags;
    /// Time to discharge
    uint32_t time_discharge;
    /// Time to charge
    uint32_t time_charge;
    /// Element index
    uint8_t elmt_idx;
} mm_api_srv_bat_cfm_t;

/// Message sent by application to return a requested Generic Location state for an element (global part)
typedef struct mm_api_srv_locg_cfm
{
    /// Request indication code (@see enum mm_api_req_ind_code)
    /// MM_API_SRV_LOCG_REQ_IND: Generic Location state (global part)
    uint32_t req_ind_code;
    /// Status
    uint16_t status;

    /// Global Altitude
    uint16_t altitude;
    /// Global Latitude
    int32_t latitude;
    /// Global Longitude
    int32_t longitude;
    /// Element index
    uint8_t elmt_idx;
} mm_api_srv_locg_cfm_t;

/// Message sent by application to return a requested Generic Location state for an element (local part)
typedef struct mm_api_srv_locl_cfm
{
    /// Request indication code (@see enum mm_api_req_ind_code)
    /// MM_API_SRV_LOCL_REQ_IND: Generic Location state (local part)
    uint32_t req_ind_code;
    /// Status
    uint16_t status;

    /// Local North
    int16_t north;
    /// Local East
    int16_t east;
    /// Local Altitude
    int16_t altitude;
    /// Uncertainty
    uint16_t uncertainty;
    /// Floor
    uint8_t floor;
    /// Element index
    uint8_t elmt_idx;
} mm_api_srv_locl_cfm_t;

/// Confirmation for get/set Generic Property value request
typedef struct mm_api_srv_prop_cfm
{
    /// Request indication code
    /// MM_API_SRV_PROP_GET : Get Generic Property value
    /// MM_API_SRV_PROP_SET : Set Generic Property value
    uint32_t req_ind_code;
    /// Status
    uint16_t status;

    /// Element index
    uint8_t elmt_idx;
    /// Property type
    uint8_t prop_type;
    /// Property ID
    uint16_t prop_id;
    /// Value length
    uint16_t length;
    /// Value
    uint8_t val[__ARRAY_EMPTY];
} mm_api_srv_prop_cfm_t;

/// @} MESH_API_MSG_

#endif /* MESH_API_MSG_ */
