/**
 ****************************************************************************************
 *
 * @file mal_int.h
 *
 * @brief Header file for Mesh Abstraction Layer Internal Definitions
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef MAL_INT_
#define MAL_INT_

/**
 ****************************************************************************************
 * @defgroup MAL Mesh Abstraction Layer
 * @ingroup MESH
 * @brief  Mesh Abstraction Layer
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mal.h"         // Mesh Abstraction Layer Definitions
#include "mal_lib.h"     // Mesh Abstraction Layer Libraries
#include "prf.h"         // Profile defines
#include "common_bt.h"
#include "mesh_tb_timer.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define MAL_IDX_MAX        (1)
/// Advertising channel map - 37, 38, 39
#define MAL_ADV_CHMAP      (0x07)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Message IDs for internal mesh messages
enum mesh_int_msg_id
{
    MESH_FIRST_INT_MSG = TASK_BLE_FIRST_MSG(TASK_BLE_ID_MESH) + 200,
    /// Kernel message sent when timer set by mesh stack expires
    MESH_TIMER_IND = MESH_FIRST_INT_MSG,
    /// Kernel message sent when advertising has been properly performed
    MESH_ADV_TX_DONE,
    /// Kernel message sent when Networking message has been properly performed
    MESH_NET_TX_DONE,
    /// Timer used to renew the mesh transmission address
    MESH_ADV_RENEW_IND,
    /// Kernel message used to handle advertising report from scanning driver
    MESH_ADV_REPORT_IND,
    /// Kernel message used to handle advertising report from scanning driver
    MESH_API_USER_ADV_REPORT_IND,
    /// Kernel message sent when User advertising has been properly performed
    MESH_API_USER_ADV_RELAY_IND,
    /// Kernel message sent by abstraction layer to itself in order to simulate feedback
    /// from BLE stack for start scan operation
    MESH_ADV_SCAN_STARTED_IND,
    /// Kernel message used to handle scan stopped event
    MESH_ADV_SCAN_STOPPED_IND,
#if (BLE_MESH_GATT_BEARER)
    /// Kernel message sent when PDU reassembly expires
    MESH_CON_TIMER_FIRST_IND,
    MESH_CON_TIMER_LAST_IND = (MESH_CON_TIMER_FIRST_IND + BLE_CONNECTION_MAX),
    /// Mesh used to simulate complete event when connectable advertising is started
    MESH_CON_ADV_STARTED_IND,
#endif //(BLE_MESH_GATT_BEARER)
};

enum mesh_adv_instance
{
    MESH_ADV_TYPE_PROV,
    MESH_ADV_TYPE_CUSTOM,
    MESH_ADV_TYPE_MAX,
};

typedef enum
{
    MESH_PROVISIONING_ADV,
    MESH_PROXY_ADV,
    MESH_CUSTOM_ADV,
    MESH_ADV_ALL,
} mesh_mal_adv_type_t;

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/// Structure for Timer Abstraction Layer environment
typedef struct mal_timer_env
{
    /// Last read local time value
    uint32_t last_time_loc;
    /// Clock (in milliseconds)
    uint32_t clock_ms;
    /// Number of time the clock has wrapped since wake-up time
    uint16_t nb_wrap;
} mal_timer_env_t;

/// Advertising environment definition
typedef struct mal_adv_env
{
    /// Set of callback executed when an event occurs on advertising bearer
    const mal_adv_cb_t     *p_cb_evts;
    //  Status of the user send adv finish
    uint8_t                  user_adv_finish;
    /// Status of the scanning
    uint8_t                  scan_state;
    /// Identifier of bearer that handle packet transmission / reception
    m_lid_t                  bearer_lid;

    /// Non resolvable random address used for advertising packet transmission
    uint8_t                  tx_bd_addr[BD_ADDR_LEN];
} mal_adv_env_t;

/// Structure Delayed job execution
typedef struct mal_djob_env
{
    /// Job queue to execute
    common_list_t job_queue;
} mal_djob_env_t;


/// Structure for security environment
typedef struct mal_sec_env
{
    /// Call-back to read Public key
    mal_sec_pub_key_res_cb     cb_pub_key;
    /// Call-back for DH-Key generation
    mal_sec_ecdh_secret_res_cb cb_dh_key;
} mal_sec_env_t;

/// Structure that handle connection
typedef struct mal_con
{
    /// Pointer of data to transmit
    const uint8_t *p_tx_data;
    /// Queue of received message
    common_list_t      rx_queue;
    /// Reception message type
    uint8_t        rx_type;
    /// Length of received PDU
    uint8_t        rx_length;
    /// Transmission message type
    uint8_t        tx_type;
    /// Transmission data offset
    uint8_t        tx_offset;
    /// Transmission length
    uint8_t        tx_length;
    /// Notification configuration
    uint8_t        ntf_cfg;
    /// Identifier of bearer that handle connection
    m_lid_t        bearer_lid;
} mal_con_t;

#if (BLE_MESH_GATT_BEARER)
typedef struct mal_adv
{
    uint16_t adv_data_len;
    uint8_t  adv_data[31];
    uint16_t scan_rsp_len;
    uint8_t  scan_rsp_data[31];
} mal_adv_t;
/// Structure for Connection module
typedef struct mal_con_env
{
    /// Connection specific parameters
    mal_con_t          *p_con[BLE_CONNECTION_MAX];
    /// Callback structure
    const mal_con_cb_t *p_cb;
    /// Pointer to the advertising data
    const uint8_t       *p_adv_data;
    /// Length of advertising data
    uint16_t             adv_data_len;
    /// Provisioning service start handle
    uint16_t             prov_start_hdl;
    /// Proxy service start handle
    uint16_t             proxy_start_hdl;
    /// Connectable advertising interval (unit 1 slot)
    uint16_t             interval;
    /// Supported connection type
    uint8_t              con_type;
    /// Advertising state machine
    uint8_t              adv_state;
    uint8_t              adv_type;
    /// Connection index of created connection
    uint8_t              conidx;
    /// Number of Mesh GATT Active connections
    uint8_t              nb_con;
    uint8_t              adv_status;
    mal_adv_t            mal_adv[MESH_ADV_TYPE_MAX];
    uint16_t             duration;
    mesh_tb_timer_t      timer_switch_adv;
    bool                 adv_update;
    bool                 adv_restart;
    bool                 prov_adv_update;
    bool                 customer_adv_noupdate;
} mal_con_env_t;
#endif //(BLE_MESH_GATT_BEARER)

/// Mesh Abstraction layer Environment Variable
typedef struct mal_env
{
    /// Profile environment
    prf_env_t         prf_env;
    /// Mesh task state
    kernel_state_t        state[MAL_IDX_MAX];
    /// Security module environment
    mal_sec_env_t    sec;
    /// Timer Environment
    mal_timer_env_t  timer;
    /// Advertising environment
    mal_adv_env_t    adv;
#if (BLE_MESH_GATT_BEARER)
    /// Connection oriented environment
    mal_con_env_t    con;
#endif //(BLE_MESH_GATT_BEARER)
    /// Delayed Job environment
    mal_djob_env_t   djob;
#if (BLE_MESH_GATT_BEARER)
    /// Boolean indicating if a connectable advertising is enabled
    bool con_adv_en;
#endif //(BLE_MESH_GATT_BEARER)
} mal_env_t;

/*
 * GLOBAL VARIABLE
 ****************************************************************************************
 */

extern mal_env_t *p_mal_env;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * Initialize encryption block for Mesh abstraction layer
 *
 * @param[in] reset     Initialization due to a reset
 ****************************************************************************************
 */
void mal_sec_init(bool reset);

/**
 ****************************************************************************************
 * Request HL to execute an AES function
 *
 * @param[in] key       128 bits key
 * @param[in] val       128 bits value to encrypt with AES
 ****************************************************************************************
 */
void mal_sec_aes_req(const uint8_t *key, const uint8_t *val);

/**
 ****************************************************************************************
 * Initialize timer block for Mesh abstraction layer
 *
 * @param[in] reset Initialization due to a reset
 ****************************************************************************************
 */
void mal_timer_init(bool reset);

/**
 ****************************************************************************************
 * Initialize Advertising block for Mesh abstraction layer
 *
 * @param[in] reset Initialization due to a reset
 ****************************************************************************************
 */
void mal_adv_init(bool reset);

/**
 ****************************************************************************************
 * @brief TODO [LT]
 ****************************************************************************************
 */
void mal_adv_scan_started_handler(void);

/**
 ****************************************************************************************
 * Initialize delayed job block for Mesh abstraction layer
 *
 * @param[in] reset Initialization due to a reset
 ****************************************************************************************
 */
void mal_djob_init(bool reset);

#if (BLE_MESH_GATT_BEARER)
/**
 ****************************************************************************************
 * Initialize delayed job block for Mesh abstraction layer
 *
 * @param[in] reset                 Initialization due to a reset
 * @param[in|out] p_start_hdl       Start handle of the GATT database
 * @param[in] mesh_task             Task number of Mesh profile
 * @param[in] sec_lvl               Database security level requirements
 * @param[in] features              Database supported features (@see enum m_api_feat)
 ****************************************************************************************
 */
uint8_t mal_con_init(bool reset, uint16_t *p_start_hdl, uint16_t mesh_task,
                     uint8_t sec_lvl, uint32_t features);

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
void mal_con_create(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Handles Disconnection
 *
 * @param[in|out]    env        Allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 ****************************************************************************************
 */
void mal_con_cleanup(uint8_t conidx, uint8_t reason);
#endif //(BLE_MESH_GATT_BEARER)

/**
 ****************************************************************************************
 * @brief Initialize Mesh Abstraction Layer Task Module
 *
 * @param[in] reset         Initialization due to a reset or not
 * @param[out] p_env        Allocated environment data
 * @param[in] app_task      Application task number
 * @param[in] sec_lvl       Security level (AUTH, EKS and MI field of @see enum attm_value_perm_mask)
 ****************************************************************************************
 */
void mal_task_init(bool reset, struct prf_task_env *p_env, uint16_t app_task,
                   uint8_t sec_lvl);


uint16_t mal_set_scanrsp_data(char const *scanrsp_data, uint8_t len);

uint8_t *mal_get_scanrsp_data(uint8_t *scanrsp_data_len);

uint16_t mal_custom_adv_start(uint16_t interval, uint16_t adv_data_len,
                                          const uint8_t *adv_data, 
                                          uint16_t scan_rsp_len, 
                                          const uint8_t *p_scan_rsp_data, 
                                          uint16_t duration);

uint16_t mal_adv_stop(mesh_mal_adv_type_t type);

void mal_custom_adv_stop(void);

void mal_set_prov_adv_update(bool value);

void clear_all_adv_state(void);

/// @} MAL

#endif /* MAL_INT_ */
