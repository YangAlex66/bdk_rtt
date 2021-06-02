/**
 ****************************************************************************************
 *
 * @file mesh_api.h
 *
 * @brief Header file for Mesh Stack Application Programming Interface
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MESH_API_
#define _MESH_API_

/**
 ****************************************************************************************
 * @defgroup MESH_API_MSG_ Mesh Stack Application Programming Interface
 * @ingroup MESH
 * @brief  Mesh Stack Application Programming Interface
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh_defines.h"
#include "mesh_config.h"
#include "gap.h"
/*
 * ENUMERATIONS FOR MESH PROFILE
 ****************************************************************************************
 */

/// Mesh Supported Features
enum m_api_feat
{
    /// Relay Node
    M_FEAT_RELAY_NODE_SUP       = (1 << 0),
    /// Proxy Node
    M_FEAT_PROXY_NODE_SUP       = (1 << 1),
    /// Friend Node
    M_FEAT_FRIEND_NODE_SUP      = (1 << 2),
    /// Low Power Node
    M_FEAT_LOW_POWER_NODE_SUP   = (1 << 3),

    /// Message API supported
    M_FEAT_MSG_API_SUP          = (1 << 16),
    /// Provisioning over GATT
    M_FEAT_PB_GATT_SUP          = (1 << 17),
    /// Dynamic beacon interval supported
    M_FEAT_DYN_BCN_INTV_SUP     = (1 << 18),
};

/// Mesh gatt adv use custom configuration

enum m_gatt_adv_config
{
    M_GATT_ADV_CONFIG_DEFAULT = 0x0,
    M_GATT_ADV_CONFIG_CUSTOM = 0x01,
};


/// Model configuration
/// 7     1            0
/// +-----+------------+
/// | RFU | Publi Auth |
/// +-----+------------+
enum m_mdl_config
{
    /// Indicate if sending of publications is authorized or not
    M_MDL_CONFIG_PUBLI_AUTH_POS = 0,
    M_MDL_CONFIG_PUBLI_AUTH_BIT = 0x01,
};

/*
 * TYPES DEFINITION FOR MESH PROFILE
 ****************************************************************************************
 */

/// Mesh Profile Configuration Structure
typedef struct m_cfg
{
    /// Mask of supported features (@see enum m_api_feat)
    uint32_t features;
    /// 16-bit company identifier assigned by the Bluetooth SIG
    uint16_t cid;
    /// 16-bit vendor-assigned product identifier
    uint16_t pid;
    /// 16-bit vendor-assigned product version identifier
    uint16_t vid;
    /// Localization descriptor
    uint16_t loc;
    /// Number of pages in the Composition Data
    uint8_t  nb_cdata_page;

    /// Receive window in milliseconds when Friend feature is supported
    uint8_t frd_rx_window_ms;
    /// Queue size when Friend feature is supported
    uint8_t frd_queue_size;
    /// Subscription list size when Friend feature is supported
    uint8_t frd_subs_list_size;

    /// mesh gatt adv data use custom define
    uint8_t gatt_adv_custom;
    uint8_t gatt_adv_len;
    uint8_t gatt_adv_data[GAP_ADV_DATA_LEN];
    uint8_t gatt_scan_len;
    uint8_t gatt_scan_data[GAP_SCAN_RSP_DATA_LEN];

} m_cfg_t ;

typedef struct
{
   uint8_t number_of_elements; /**< The number of elements that are supported by the device. */
   uint8_t public_key_type;    /**< Supported public key types. */
   uint8_t static_oob_type;    /**< Supported static OOB types. */
   uint8_t output_oob_size;    /**< The maximum size of the supported output OOB. */
   uint16_t output_oob_action; /**< Supported output OOB actions. */
   uint8_t input_oob_size;     /**< The maximum size in octets of the supported input OOB. */
   uint16_t input_oob_action;  /**< Supported input OOB actions. */
} ble_mesh_capabilities_t;

typedef struct
{
   uint8_t device_uuid[16];  /**< Device UUID uniquely identifying this device . */
   uint16_t oob_information; /**< Supported public key types. */
   uint32_t uri_hash;        /**< Hash of the associated URI advertised with the URI AD Type (optional field) . */
} ble_mesh_unprovisioned_device_beacon_t;

typedef struct
{
   ble_mesh_unprovisioned_device_beacon_t config; /**< The Unprovisioned Device beacon parameters. */
   ble_mesh_capabilities_t cap;                   /**< The capabilities parameters. */
   uint8_t auth_value[16];                             /**< The Confirmation parameters. */
} ble_mesh_unprovisioned_data_t;

typedef struct
{
   uint16_t cid;     //!< company identifier
   uint16_t pid;     //!< product identifier
   uint16_t vid;     //!< product version identifier
   uint16_t crpl;    //!< representing the minimum number of replay protection list entries in a device
   uint32_t feature; //!< indicating the device feature
} ble_mesh_composition_data_t;

/// Indicates the provisioning method selected by the Provisioner based on the capabilities of the device
typedef struct m_prov_start
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
} m_prov_start_t;

/**
 ****************************************************************************************
 * @brief Mesh Buffer, the value of the pointer must not be changed.
 *
 * It must be:
 * - Allocated through m_api_buf_alloc()
 * - Released with m_api_buf_release()
 ****************************************************************************************
 */
typedef void m_api_buf_t;

/*
 * CALLBACKS DEFINITION FOR MESH PROFILE
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Definition of callback function to call upon reception of a PDU for specific model identifier
 *
 * @param[in] model_lid    Model Local Identifier
 * @param[in] opcode       Operation code
 * @param[in] p_buf        Pointer to the buffer containing the message PDU. - No need to release buffer.
 * @param[in] app_key_lid  Application Key Local identifier (Required for a response)
 * @param[in] src          Source address of the message (Required for a response)
 * @param[in] rssi         Measured RSSI level for the received PDU.
 * @param[in] not_relayed  True if message have been received by an immediate peer; False, it can have been relayed
 ****************************************************************************************
 */
typedef void (*m_api_model_rx_cb)(m_lid_t model_lid, uint32_t opcode, m_api_buf_t *p_buf, m_lid_t app_key_lid,
                                  uint16_t src, int8_t rssi, bool not_relayed);

/**
 ****************************************************************************************
 * @brief Definition of callback function to call upon reception of a PDU to check that model can handle it
 *
 * @note m_api_model_opcode_status function must be used to provide information about opcode support
 *
 * @param[in] model_lid Model Local Identifier
 * @param[in] opcode    Operation code to check
 ****************************************************************************************
 */
typedef void (*m_api_model_opcode_check_cb)(m_lid_t model_lid, uint32_t opcode);

/**
 ****************************************************************************************
 * @brief Definition of callback function to call when publication parameters are updated
 *
 * @param[in] model_lid    Model Local Identifier
 * @param[in] addr         Publication Address
 * @param[in] period_ms    Publish period in milliseconds
 ****************************************************************************************
 */
typedef void (*m_api_model_publish_param_cb)(m_lid_t model_lid, uint16_t addr, uint32_t period_ms);

/**
 ****************************************************************************************
 * @brief Definition of callback function to call once PDU has been sent.
 *
 * @param[in] model_lid Model Local Identifier
 * @param[in] tx_hdl    Handle value configured by model when message has been requested to be sent
 * @param[in] p_buf     Pointer to the buffer containing the transmitted PDU. - Buffer must be released by model.
 * @param[in] status    Transmission status.
 ****************************************************************************************
 */
typedef void (*m_api_model_sent_cb)(m_lid_t model_lid, uint8_t tx_hdl, m_api_buf_t *p_buf, uint16_t status);

/**
 ****************************************************************************************
 * @brief Callback executed when simple execution performed
 *
 * @param[in] status    Execution status.
 ****************************************************************************************
 */
typedef void (*m_api_end_cb)(uint16_t status);

/**
 ****************************************************************************************
 * Callback used to inform about a modification of the provisioning module state
 *
 * @param state    State of the provisioner   (@see enum m_prov_state)
 * @param status   Relevant only for provisioning failed (failed reason)
 ****************************************************************************************
 */
typedef void (*m_api_prov_state_cb)(uint8_t state, uint16_t status);

/**
 ****************************************************************************************
 * Callback used to inform that provisioning parameters are required
 ****************************************************************************************
 */
typedef void (*m_api_prov_param_req_cb)(void);

/**
 ****************************************************************************************
 * Callback used to inform that Out Of Band Authentication Data is required for provisioning
 *
 * @note Authentication data must be provided using @see m_api_prov_oob_auth_rsp() function
 *
 *
 * @param auth_method  Authentication method (@see enum m_prov_auth_method)
 * @param auth_action  Authentication Action:
 *                     - M_PROV_AUTH_NO_OOB     = Prohibited
 *                     - M_PROV_AUTH_STATIC_OOB = 16 bytes LSB static out of band data required
 *                     - M_PROV_AUTH_OUTPUT_OOB = @see enum m_prov_out_oob, 1 bit set.
 *                     - M_PROV_AUTH_INPUT_OOB  = @see enum m_prov_in_oob, 1 bit set.
 * @param auth_size    expected authentication maximum data size
 ****************************************************************************************
 */
typedef void (*m_api_prov_oob_auth_req_cb)(uint8_t auth_method, uint16_t auth_action, uint8_t auth_size);

/**
 ****************************************************************************************
 * Callback used to inform that stored information have been loaded
 ****************************************************************************************
 */
typedef void (*m_api_storage_loaded_cb)(uint16_t status);

/**
 ****************************************************************************************
 * Callback used to inform application about start/end of attention timer
 ****************************************************************************************
 */
typedef void (*m_api_attention_cb)(uint8_t attention_state);

/**
 ****************************************************************************************
 * Callback used to request a page of composition data to the application
 ****************************************************************************************
 */
typedef void (*m_api_compo_data_cb)(uint8_t page);

/**
 ****************************************************************************************
 * Callback used to inform application about received node reset request
 ****************************************************************************************
 */
typedef void (*m_api_node_reset_cb)(void);

/**
 ****************************************************************************************
 * Callback used to inform application of friendship update as low power node
 ****************************************************************************************
 */
typedef void (*m_api_lpn_status_cb)(uint16_t status, uint16_t friend_addr);

/**
 ****************************************************************************************
 * Callback used to inform application about reception of a Friend Offer message
 ****************************************************************************************
 */
typedef void (*m_api_lpn_offer_cb)(uint16_t friend_addr, uint8_t rx_window,
                                   uint8_t queue_size, uint8_t subs_list_size, int8_t rssi);

/**
 ****************************************************************************************
 * Callback used to inform application about reception of a Friend Offer message
 ****************************************************************************************
 */
typedef void (*m_api_proxy_adv_update_cb)(uint8_t state, uint8_t reason);

/**
 ****************************************************************************************
 * Callback used to inform application about unprovisioned beacon
 ****************************************************************************************
 */
typedef void (*m_api_unprov_beacon_cb)(uint8_t* dev_uuid, uint16_t oob_info);

typedef void (*m_api_prov_capability_cb)(uint16_t algorithms, uint8_t  pub_key_type,
        uint8_t  static_oob_type, uint8_t  out_oob_size, uint16_t out_oob_action,
        uint8_t  in_oob_size, uint16_t in_oob_action);

typedef void (*m_api_dev_added_cb)(m_lid_t dev_lid);

typedef void (*m_api_confc_dev_compo_data_status_cb)(m_lid_t dev_lid, uint8_t* data, uint8_t len);

typedef void (*m_api_confc_app_key_status_cb)(m_lid_t dev_lid, uint8_t status, uint16_t netkey_id, uint16_t appkey_id);

typedef void (*m_api_confc_model_app_status_cb)(m_lid_t dev_lid, uint8_t status, uint16_t address, uint16_t appkey_id, uint16_t model_id);

typedef void (*m_api_confc_model_app_subs_status_cb)(m_lid_t dev_lid, uint8_t status, uint16_t elem_addr, uint16_t subs_addr, uint16_t model_id);

typedef void (*m_api_prov_start_cb)(m_prov_start_t *p_prov_start);

/**
 ****************************************************************************************
 * Callback used to request list of fault for primary element
 ****************************************************************************************
 */
typedef void (*m_api_fault_get_cb)(uint16_t comp_id);

/**
 ****************************************************************************************
 * Callback used to request test of faults for primary element
 ****************************************************************************************
 */
typedef void (*m_api_fault_test_cb)(uint16_t comp_id, uint8_t test_id, bool cfm_needed);

/**
 ****************************************************************************************
 * Callback used to inform application that fault status for primary element must be cleared
 ****************************************************************************************
 */
typedef void (*m_api_fault_clear_cb)(uint16_t comp_id);

/**
 ****************************************************************************************
 * Callback used to inform application that fault period for primary element has been updated
 ****************************************************************************************
 */
typedef void (*m_api_fault_period_cb)(uint32_t period_ms, uint32_t period_fault_ms);

/*
 * CALLBACK STRUCTURES FOR MESH PROFILE
 ****************************************************************************************
 */

/// Mesh Profile Callback Structure
typedef struct m_api_cb
{
    /// Callback executed at end of mesh enable request
    m_api_end_cb                cb_enabled;
    /// Callback executed at end of mesh disable request
    m_api_end_cb                cb_disabled;
    /// Callback used to inform application about provisioning state
    m_api_prov_state_cb         cb_prov_state;
    /// Callback used to request provisioning parameters to the application
    m_api_prov_param_req_cb     cb_prov_param_req;
    /// Callback used to request out of band authentication data
    m_api_prov_oob_auth_req_cb  cb_prov_auth_req;
    /// Callback used to inform that stored information have been loaded
    m_api_storage_loaded_cb     cb_loaded;
    /// Callback used to inform application about start/end of attention timer
    m_api_attention_cb          cb_attention;
    /// Callback used to request a page of composition data to the application
    m_api_compo_data_cb         cb_compo_data;
    /// Callback used to inform application about received node reset request
    m_api_node_reset_cb         cb_node_reset;
#if (BLE_MESH_LPN)
    /// Callback used to inform application of friendship update as low power node
    m_api_lpn_status_cb         cb_lpn_status;
    /// Callback used to inform application about reception of a Friend Offer message
    m_api_lpn_offer_cb          cb_lpn_offer;
#endif //(BLE_MESH_LPN)
#if (BLE_MESH_GATT_PROXY)
    /// Callback used to inform application about state update of proxy advertising
    m_api_proxy_adv_update_cb   cb_proxy_adv_update;
#endif //(BLE_MESH_GATT_PROXY)
#if (BLE_MESH_PROVISIONER)
    m_api_unprov_beacon_cb cb_unprov_beacon;
    m_api_prov_capability_cb cb_m_api_prov_capability;
    m_api_dev_added_cb cb_m_api_dev_added;

    m_api_confc_dev_compo_data_status_cb cb_m_api_confc_dev_compo_data_status;
    m_api_confc_app_key_status_cb cb_m_api_confc_app_key_status;
    m_api_confc_model_app_status_cb cb_m_api_confc_model_app_status;
    m_api_confc_model_app_subs_status_cb subs_status_cb;
#endif //(BLE_MESH_PROVISIONER) 
	m_api_prov_start_cb prov_start_cb;
} m_api_cb_t;

/// Callback Structure for Health Model for primary element
typedef struct m_api_fault_cb
{
    /// Callback used to request list of fault for primary element
    m_api_fault_get_cb          cb_fault_get;
    /// Callback used to request test of faults for primary element
    m_api_fault_test_cb         cb_fault_test;
    /// Callback used to inform application that fault status for primary element must be cleared
    m_api_fault_clear_cb        cb_fault_clear;
    /// Callback used to inform application that fault period for primary element has been updated
    m_api_fault_period_cb       cb_fault_period;
} m_api_fault_cb_t;

/// Callback Structure for registered models
typedef struct m_api_model_cb
{
    /// Reception of a buffer for model
    m_api_model_rx_cb             cb_rx;
    /// Callback executed when a PDU is properly sent
    m_api_model_sent_cb           cb_sent;
    /// Check if model can handle operation code
    m_api_model_opcode_check_cb   cb_opcode_check;
    /// Callback function called when publication parameters are updated
    m_api_model_publish_param_cb cb_publish_param;
} m_api_model_cb_t;

/*
 * FUNCTIONS DEFINITION FOR MESH PROFILE
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Define the set of callback to communicate with mesh native API
 *
 * @param[in] p_cb_api Native application callback set use to communicate with a native API
 *
 * @return Execution Status code
 ****************************************************************************************
 */
uint16_t m_api_set(const m_api_cb_t *p_cb_api, const m_api_fault_cb_t *p_fault_cb_api);

/**
 ****************************************************************************************
 * @brief Enable Mesh profile
 *
 * @note cb_enabled() of m_api_cb_t called at end of enable execution
 *
 * @return Execution Status code
 ****************************************************************************************
 */
uint16_t m_api_enable(void);

/**
 ****************************************************************************************
 * @brief Disable Mesh profile
 *
 * @note cb_disabled() of m_api_cb_t called at end of disable execution
 *
 * @return Execution Status code
 ****************************************************************************************
 */
uint16_t m_api_disable(void);

/**
 ****************************************************************************************
 * @brief Allocate buffers command
 *
 * @param[out] pp_buf    Pointer to the buffer structure allocated
 * @param[in]  size      size of data required for the buffer
 *
 * @return Execution Status code
 ****************************************************************************************
 */
uint16_t m_api_buf_alloc(m_api_buf_t **pp_buf, uint16_t size);

/**
 ****************************************************************************************
 * @brief Release allocate buffers. The buffer is free as soon as all reference to buffer are released.
 *
 * @param[in] p_buf    Pointer to the buffer structure
 *
 * @return Execution Status code
 ****************************************************************************************
 */
uint16_t m_api_buf_release(m_api_buf_t *p_buf);

/**
 ****************************************************************************************
 * @brief Register a model
 *
 * @param[in] model_id          Model ID.
 * @param[in] elmt_idx          Index of element the model belongs to
 * @param[in] config            Configuration (@see enum m_api_mdl_config)
 * @param[in] p_cb              Pointer to callback functions defined by the model
 * @param[out] p_model_lid      Pointer to the variable that will contain the allocated Model LID.
 *
 * @return Execution status code
 ****************************************************************************************
 */
uint16_t m_api_register_model(uint32_t model_id, uint8_t elmt_idx, uint8_t config,
                              const m_api_model_cb_t *p_cb, m_lid_t *p_model_lid);

/**
 ****************************************************************************************
 * @brief Load stored information for storage manager
 *
 * @param[in] length    Length of stored information report
 * @param[in] p_data    Pointer to stored information report
 ****************************************************************************************
 */
uint16_t m_api_storage_load(uint16_t length, uint8_t *p_data);

/**
 ****************************************************************************************
 * @brief Store information about mesh network
 ****************************************************************************************
 */
uint16_t m_api_storage_save(void);

/**
 ****************************************************************************************
 * @brief Configure storage module
 ****************************************************************************************
 */
uint16_t m_api_storage_config(uint32_t config);

/**
 ****************************************************************************************
 * @brief Set IV update mode and ignore 96-hour limit
 *
 * @param[in] update    True if transition to IV Update in Progress state is required, False if
 * require to transit to Normal state
 ****************************************************************************************
 */
void m_api_iv_upd_test_mode(bool update);

/**
 ****************************************************************************************
 * @brief Let the model publish a message over mesh network
 *
 * @note Message status will be reported with model callback (@see m_api_model_sent_cb)
 *
 * @param[in] model_id     Model ID.
 * @param[in] opcode       Operation code of the message
 * @param[in] tx_hdl       Handle value used by model to retrieve which message has been sent
 * @param[in] p_buf        Pointer to the buffer structure that contains message to publish
 * @param[in] trans_mic_64 1 = Segmented PDU force transport MIC to 64 bits ; 0 = 32 bits transport MIC
 *
 * @return Execution status code
 ****************************************************************************************
 */
uint16_t m_api_model_publish(m_lid_t model_lid, uint32_t opcode, uint8_t tx_hdl, m_api_buf_t *p_buf, bool trans_mic_64);

/**
 ****************************************************************************************
 * @brief Let the model publish a message over mesh network
 *
 * @note Message status will be reported with model callback (@see m_api_model_sent_cb)
 *
 * @param[in] model_id     Model ID.
 * @param[in] opcode       Operation code of the message
 * @param[in] tx_hdl       Handle value used by model to retrieve which message has been sent
 * @param[in] p_buf        Pointer to the buffer structure that contains message to publish
 * @param[in] key_lid      Key information.
 *                         If key_lid & 0x80 != 0, key_lid & 0x7F = network key local index
 *                         else key_lid & 0x7F = application key local index.
 * @param[in] dst          Unicast destination address of the message (source address parameter of received request message)
 * @param[in] trans_mic_64 For a segmented PDU force transport mic to 64 bits
 * @param[in] not_relay    True, send message to an immediate peer; False, accept message to be relayed
 *
 * @return Execution status code
 ****************************************************************************************
 */
uint16_t m_api_model_rsp_send(m_lid_t model_lid, uint32_t opcode, uint8_t tx_hdl, m_api_buf_t *p_buf,
                              m_lid_t key_lid, uint16_t dst, bool trans_mic_64, bool not_relay);

/**
 ****************************************************************************************
 * @brief Reply to the Model operation code support (@see m_api_model_opcode_check_cb)
 *
 * @param[in] model_id  Model ID.
 * @param[in] opcode    Operation code checked
 * @param[in] status    MESH_ERR_NO_ERROR if operation supported by model, other error code to reject
 *
 * @return Execution status code
 ****************************************************************************************
 */
void m_api_model_opcode_status(m_lid_t model_lid, uint32_t opcode, uint16_t status);

/**
 ****************************************************************************************
 * @brief Provide composition data
 *
 * @param[in] page      Page of composition data
 * @param[in] length    Page length
 * @param[in] p_data    Pointer to page content
 ****************************************************************************************
 */
void m_api_compo_data_cfm(uint8_t page, uint8_t length, uint8_t *p_data);

/**
 ****************************************************************************************
 * @brief Provide fault status for primary element
 *
 * @param[in] comp_id           Company ID
 * @param[in] test_id           Test ID
 * @param[in] length            Length of fault array
 * @param[in] p_fault_array     Pointer to the fault array
 ****************************************************************************************
 */
void m_api_health_status_send(uint16_t comp_id, uint8_t test_id, uint8_t length,
                              uint8_t *p_fault_array);

/**
 ****************************************************************************************
 * @brief Provide fault status for primary element
 *
 * @param[in] comp_id           Company ID
 * @param[in] test_id           Test ID
 * @param[in] length            Length of fault array
 * @param[in] p_fault_array     Pointer to the fault array
 ****************************************************************************************
 */
void m_api_health_cfm(bool accept, uint16_t comp_id, uint8_t test_id, uint8_t length,
                      uint8_t *p_fault_array);

/**
 ****************************************************************************************
 * @brief Provide provisioning parameters to the provisioning module
 *
 * @param[in] p_param      Provisioning parameters
 ****************************************************************************************
 */
void m_api_prov_param_rsp(void *p_param);

/**
 ****************************************************************************************
 * @brief Provide authentication data to the provisioning module
 *
 * @param[in] accept      True, Accept pairing request, False reject
 * @param[in] auth_size   Authentication data size (<= requested size else pairing automatically rejected)
 * @param[in] p_auth_data Authentication data (LSB for a number or array of bytes)
 ****************************************************************************************
 */
void m_api_prov_oob_auth_rsp(bool accept, uint8_t auth_size, const uint8_t *p_auth_data);

/**
 ****************************************************************************************
 * @brief Get the local public key for out of band transmission of local public key
 *
 * @param[out] p_pub_key_x   X Coordinate of public Key (32 bytes LSB)
 * @param[out] p_pub_key_y   Y Coordinate of public Key (32 bytes LSB)
 *
 * @return Execution status code
 ****************************************************************************************
 */
uint16_t m_api_prov_pub_key_read(uint8_t *p_pub_key_x, uint8_t *p_pub_key_y);

#if (BLE_MESH_LPN)
/**
 ****************************************************************************************
 * @brief Enable Low Power Node feature and start looking for an available Friend node in
 * the neighborhood.
 *
 * @param[in] poll_timeout          Initial value of PollTimeout timer
 * @param[in] poll_intv_ms          Poll interval in milliseconds
 * @param[in] rx_delay              Requested receive delay
 * @param[in] rssi_factor           RSSI factor
 * @param[in] rx_window_factor      Receive window factor
 * @param[in] min_queue_size_log    Requested minimum number of messages that the Friend node
 * can store in its Friend Queue.
 ****************************************************************************************
 */
uint16_t m_api_lpn_start(uint32_t poll_timeout, uint32_t poll_intv_ms, uint16_t prev_addr, uint8_t rx_delay,
                         uint8_t rssi_factor, uint8_t rx_window_factor, uint8_t min_queue_size_log);

/**
 ****************************************************************************************
 * @brief Disable Low Power Node feature.
 ****************************************************************************************
 */
uint16_t m_api_lpn_stop(void);

/**
 ****************************************************************************************
 * @brief Select a friend after reception of one or several Friend Offer messages.
 *
 * @param[in] friend_addr       Address of the selected Friend node.
 ****************************************************************************************
 */
uint16_t m_api_lpn_select_friend(uint16_t friend_addr);
#endif //(BLE_MESH_LPN)

#if (BLE_MESH_GATT_PROXY)
/**
 ****************************************************************************************
 * @brief Control if Proxy service should start / stop advertising it's capabilities
 *
 * @param[in] enable  True to enable advertising for 60s, False to stop advertising
 *
 * @return Execution status code
 ****************************************************************************************
 */
uint16_t m_api_proxy_ctrl(uint8_t enable);
#endif // (BLE_MESH_GATT_PROXY)


/*
 * ENUMERATIONS FOR MESH MODELS
 ****************************************************************************************
 */

/// Model Configuration Index
enum mm_cfg_idx
{
    /// ************ Generic Server ************

    MM_CFG_IDX_GENS_MIN                 = 0,
    /// Generic OnOff Server
    MM_CFG_IDX_GENS_ONOFF               = MM_CFG_IDX_GENS_MIN,
    /// Generic Level Server
    MM_CFG_IDX_GENS_LEVEL,
    /// Generic Default Transition Time
    MM_CFG_IDX_GENS_DFT_TRANS_TIME,
    /// Generic Power OnOff
    MM_CFG_IDX_GENS_POWER_ONOFF,
    /// Generic Power Level
    MM_CFG_IDX_GENS_POWER_LEVEL,
    /// Generic Battery
    MM_CFG_IDX_GENS_BATTERY,
    /// Generic Location
    MM_CFG_IDX_GENS_LOCATION,
    MM_CFG_IDX_GENS_MAX                 = MM_CFG_IDX_GENS_LOCATION,

    /// ************ Lighting Server ***********

    MM_CFG_IDX_LIGHTS_MIN               = 50,
    /// Light Lightness Server
    MM_CFG_IDX_LIGHTS_LN                = MM_CFG_IDX_LIGHTS_MIN,
    /// Light CTL Server
    MM_CFG_IDX_LIGHTS_CTL,
    /// Light HSL Server
    MM_CFG_IDX_LIGHTS_HSL,

    MM_CFG_IDX_LIGHTS_CTL_HSL,

    /// Light xyL Server
    MM_CFG_IDX_LIGHTS_XYL,
    MM_CFG_IDX_LIGHTS_MAX               = MM_CFG_IDX_LIGHTS_XYL,

    /// Number of known configuration
    MM_CFG_IDX_NB,
};

/// Client Model Index
enum mm_cmdl_idx
{
    /// ************ Generic ************

    MM_CMDL_IDX_GENC_MIN                 = 0,
    /// Generic OnOff Client
    MM_CMDL_IDX_GENC_ONOFF               = MM_CMDL_IDX_GENC_MIN,
    /// Generic Level Client
    MM_CMDL_IDX_GENC_LEVEL,
    /// Generic Default Transition Time Client
    MM_CMDL_IDX_GENC_DFT_TRANS_TIME,
    /// Generic Power OnOff Client
    MM_CMDL_IDX_GENC_POWER_ONOFF,
    /// Generic Power Level Client
    MM_CMDL_IDX_GENC_POWER_LEVEL,
    /// Generic Battery Client
    MM_CMDL_IDX_GENC_BATTERY,
    /// Generic Location Client
    MM_CMDL_IDX_GENC_LOCATION,
    /// Generic Property Client
    MM_CMDL_IDX_GENC_PROPERTY,
    MM_CMDL_IDX_GENC_MAX                 = MM_CMDL_IDX_GENC_PROPERTY,

    /// ************ Lighting ***********

    MM_CMDL_IDX_LIGHTC_MIN               = 50,
    /// Light Lightness Client
    MM_CMDL_IDX_LIGHTC_LN                = MM_CMDL_IDX_LIGHTC_MIN,
    /// Light CTL Client
    MM_CMDL_IDX_LIGHTC_CTL,
    /// Light HSL Client
    MM_CMDL_IDX_LIGHTC_HSL,
    /// Light xyL Client
    MM_CMDL_IDX_LIGHTC_XYL,
    MM_CMDL_IDX_LIGHTC_MAX               = MM_CMDL_IDX_LIGHTC_XYL,

    /// Number of known client models
    MM_CMDL_IDX_NB,
};

/// Transition type
enum mm_trans_type
{
    /// Classic Set
    MM_TRANS_TYPE_CLASSIC = 0,
    /// Delta Set
    MM_TRANS_TYPE_DELTA,
    /// Move Set
    MM_TRANS_TYPE_MOVE,
    /// No transition
    MM_TRANS_TYPE_NONE,
};

/// Set information bit field)
/// 8     7     0
/// +-----+------+
/// | Ack | Type |
/// +-----+------+
enum mm_set_info
{
    /// Type (value depends on model for which operation is required)
    MM_SET_INFO_TYPE_LSB = 0,
    MM_SET_INFO_TYPE_MASK = 0x7F,

    /// Set or Set Unacknowledged
    MM_SET_INFO_ACK_POS = 7,
    MM_SET_INFO_ACK_BIT = 0x80,
};

/// Transition information bit field)
/// 16    8     7      6     2      0
/// +-----+-----+------+-----+------+
/// | TID | Ack | Long | RFU | Type |
/// +-----+-----+------+-----+------+
enum mm_trans_info
{
    /// Type (value depends on model for which transition is required)
    /// @see enum mm_trans_type
    MM_TRANS_INFO_TYPE_LSB = 0,
    MM_TRANS_INFO_TYPE_MASK = 0x0003,

    /// Include transition time and delay
    MM_TRANS_INFO_LONG_POS = 6,
    MM_TRANS_INFO_LONG_BIT = 0x0040,

    /// Set or Set Unacknowledged
    MM_TRANS_INFO_ACK_POS = 7,
    MM_TRANS_INFO_ACK_BIT = 0x0080,

    /// TID
    MM_TRANS_INFO_TID_LSB = 8,
    MM_TRANS_INFO_TID_MASK = 0xFF00,
};

/// Get type values for Generic Power Level Client model
enum mm_get_type_plvl
{
    /// Get Generic Power Actual state value
    MM_GET_TYPE_PLVL_ACTUAL = 0,
    /// Get Generic Power Last state value
    MM_GET_TYPE_PLVL_LAST,
    /// Get Generic Power Default state value
    MM_GET_TYPE_PLVL_DFLT,
    /// Get Generic Power Range state value
    MM_GET_TYPE_PLVL_RANGE,

    /// Last option value
    MM_GET_TYPE_PLVL_MAX = MM_GET_TYPE_PLVL_RANGE,
};

/// Get type values for Generic Location Client model
enum mm_get_type_loc
{
    /// Get Generic Location Global state value
    MM_GET_TYPE_LOC_GLOBAL = 0,
    /// Get Generic Power Last state value
    MM_GET_TYPE_LOC_LOCAL,

    /// Last option value
    MM_GET_TYPE_LOC_MAX = MM_GET_TYPE_LOC_LOCAL,
};

/// Get type values for Generic Property Client model
enum mm_get_type_prop
{
    /// Send Generic User Properties Get message
    MM_GET_TYPE_PROP_UPROPS = 0,
    /// Send Generic User Property Get message
    MM_GET_TYPE_PROP_UPROP,
    /// Send Generic Admin Properties Get message
    MM_GET_TYPE_PROP_APROPS,
    /// Send Generic Admin Property Get message
    MM_GET_TYPE_PROP_APROP,
    /// Send Generic Manufacturer Properties Get message
    MM_GET_TYPE_PROP_MPROPS,
    /// Send Generic Manufacturer Property Get message
    MM_GET_TYPE_PROP_MPROP,
    /// Send Generic Client Properties Get message
    MM_GET_TYPE_PROP_CPROPS,

    /// Last option value
    MM_GET_TYPE_PROP_MAX = MM_GET_TYPE_PROP_CPROPS,
};

/// Get type values for Light Lightness Client model
enum mm_get_type_light_ln
{
    /// Get Light Lightness state value
    MM_GET_TYPE_LIGHT_LN_ACTUAL = 0,
    /// Get Light Lightness Linear state value
    MM_GET_TYPE_LIGHT_LN_LINEAR,
    /// Get Light Lightness Default state value
    MM_GET_TYPE_LIGHT_LN_DFLT,
    /// Get Light Lightness Last state value
    MM_GET_TYPE_LIGHT_LN_LAST,
    /// Get Light Lightness Range state value
    MM_GET_TYPE_LIGHT_LN_RANGE,

    /// Last option value
    MM_GET_TYPE_LIGHT_LN_MAX = MM_GET_TYPE_LIGHT_LN_RANGE,
};

/// Get type values for Light CTL Client model
enum mm_get_type_light_ctl
{
    /// Get Light CTL Lightness and Light CTL Temperature state value
    MM_GET_TYPE_LIGHT_CTL = 0,
    /// Get Light CTL Temperature and Light CTL Delta UV state value
    MM_GET_TYPE_LIGHT_CTL_TEMP,
    /// Get Light CTL Temperature Range state value
    MM_GET_TYPE_LIGHT_CTL_TEMP_RANGE,
    /// Get Light Lightness Default and Light CTL Temperature Default and Light CTL
    /// Delta UV Default state values
    MM_GET_TYPE_LIGHT_CTL_DFLT,

    /// Last option value
    MM_GET_TYPE_LIGHT_CTL_MAX = MM_GET_TYPE_LIGHT_CTL_DFLT,
};

/// Get type values for Light HSL Client model
enum mm_get_type_light_hsl
{
    /// Get Light HSL Lightness and Light HSL Hue and Light HSL Saturation state values
    MM_GET_TYPE_LIGHT_HSL = 0,
    /// Get Light HSL Hue state value
    MM_GET_TYPE_LIGHT_HSL_HUE,
    /// Get Light HSL Saturation state value
    MM_GET_TYPE_LIGHT_HSL_SAT,
    /// Get Light HSL Lightness and Light HSL Hue and Light HSL Saturation target state values
    MM_GET_TYPE_LIGHT_HSL_TGT,
    /// Get Light Lightness and Light HSL Hue and Light HSL Saturation default state values
    MM_GET_TYPE_LIGHT_HSL_DFLT,
    /// Get Light HSL Hue and Light HSL Saturation state range values
    MM_GET_TYPE_LIGHT_HSL_RANGE,

    /// Last option value
    MM_GET_TYPE_LIGHT_HSL_MAX = MM_GET_TYPE_LIGHT_HSL_RANGE,
};

/// Get type values for Light xyL Client model
enum mm_get_type_light_xyl
{
    /// Get Light xyL Lightness and Light xyL x and Light xyL y state values
    MM_GET_TYPE_LIGHT_XYL = 0,
    /// Get Light xyL Lightness and Light xyL x and Light xyL y state target values
    MM_GET_TYPE_LIGHT_XYL_TGT,
    /// Get Light Lightness and Light xyL x and Light xyL y state default values
    MM_GET_TYPE_LIGHT_XYL_DFLT,
    /// Get Light xyL x and Light xyL y state range values
    MM_GET_TYPE_LIGHT_XYL_RANGE,

    /// Last option value
    MM_GET_TYPE_LIGHT_XYL_MAX = MM_GET_TYPE_LIGHT_XYL_RANGE,
};

/// Set type values for the Generic Power Level Client model
enum mm_set_type_plvl
{
    /// Set Generic Power Default state value
    MM_SET_TYPE_PLVL_DFLT = 0,
    /// Set Generic Power Range state value
    MM_SET_TYPE_PLVL_RANGE,

    /// Last option value
    MM_SET_TYPE_PLVL_MAX = MM_SET_TYPE_PLVL_RANGE,
};

/// Set type values for the Light Lightness Client model
enum mm_set_type_light_ln
{
    /// Set Light Lightness Default state value
    MM_SET_TYPE_LIGHT_LN_DFLT = 0,
    /// Set Light Lightness Range state value
    MM_SET_TYPE_LIGHT_LN_RANGE,

    /// Last option value
    MM_SET_TYPE_LIGHT_LN_MAX = MM_SET_TYPE_LIGHT_LN_RANGE,
};

/// Set type values for the Light CTL Client model
enum mm_set_type_light_ctl
{
    /// Set Light CTL Temperature Range state value
    MM_SET_TYPE_LIGHT_CTL_TEMP_RANGE = 0,
    /// Set Light CTL Default state value
    MM_SET_TYPE_LIGHT_CTL_DFLT,

    /// Last option value
    MM_SET_TYPE_LIGHT_CTL_MAX = MM_SET_TYPE_LIGHT_CTL_DFLT,
};

/// Set type values for the Light HSL Client model
enum mm_set_type_light_hsl
{
    /// Set Light HSL Hue and Light HSL Saturation state range values
    MM_SET_TYPE_LIGHT_HSL_RANGE = 0,
    /// Set Light Lightness and Light HSL Hue and Light HSL Saturation default state values
    MM_SET_TYPE_LIGHT_HSL_DFLT,

    /// Last option value
    MM_SET_TYPE_LIGHT_HSL_MAX = MM_SET_TYPE_LIGHT_HSL_DFLT,
};

/// Set type values for the Light xyL Client model
enum mm_set_type_light_xyl
{
    /// Set Light xyL x and Light xyL y state range values
    MM_SET_TYPE_LIGHT_XYL_RANGE = 0,
    /// Set Light Lightness and Light xyL x and Light xyL y state default values
    MM_SET_TYPE_LIGHT_XYL_DFLT,

    /// Last option value
    MM_SET_TYPE_LIGHT_XYL_MAX = MM_SET_TYPE_LIGHT_XYL_DFLT,
};

/// Transition type values for the Light Lightness Client model
enum mm_trans_type_light_ln
{
    /// Set Light Lightness state value
    MM_TRANS_TYPE_LIGHT_LN = 0,
    /// Set Light Lightness Linear state value
    MM_TRANS_TYPE_LIGHT_LN_LIN,

    /// Last option value
    MM_TRANS_TYPE_LIGHT_LN_MAX = MM_TRANS_TYPE_LIGHT_LN_LIN,
};

/// Transition type values for the Light CTL Client model
enum mm_trans_type_light_ctl
{
    /// Set Light CTL Lightness and Light CTL Temperature and Light CTL Delta UV state values
    MM_TRANS_TYPE_LIGHT_CTL = 0,
    /// Set Light CTL Temperature and Light CTL Delta UV state values
    MM_TRANS_TYPE_LIGHT_CTL_TEMP,

    /// Last option value
    MM_TRANS_TYPE_LIGHT_CTL_MAX = MM_TRANS_TYPE_LIGHT_CTL_TEMP,
};

/// Transition type values for the Light HSL Client model
enum mm_trans_type_light_hsl
{
    /// Set Light HSL Lightness and Light HSL Hue and Light HSL Saturation state values
    MM_TRANS_TYPE_LIGHT_HSL = 0,
    /// Set Light HSL Hue state value
    MM_TRANS_TYPE_LIGHT_HSL_HUE,
    /// Set Light HSL Saturation state value
    MM_TRANS_TYPE_LIGHT_HSL_SAT,

    /// Last option value
    MM_TRANS_TYPE_LIGHT_HSL_MAX = MM_TRANS_TYPE_LIGHT_HSL_SAT,
};

/// State identifier values
enum mm_state_idx
{
    /// Generic OnOff state
    MM_STATE_GEN_ONOFF = 0,
    /// Generic Level state
    MM_STATE_GEN_LVL,
    /// Generic Default Transition Time state
    MM_STATE_GEN_DTT,
    /// Generic Power Actual state
    MM_STATE_GEN_POWER_ACTUAL,
    /// Generic Power Last state
    MM_STATE_GEN_POWER_LAST,
    /// Generic Power Default state
    MM_STATE_GEN_POWER_DFLT,
    /// Generic Power Range state
    MM_STATE_GEN_POWER_RANGE,
    /// Generic OnPowerUp state
    MM_STATE_GEN_ONPOWERUP,

    /// Light Lightness
    MM_STATE_LIGHT_LN = 50,
    /// Light Lightness Linear
    MM_STATE_LIGHT_LN_LIN,
    /// Light Lightness Last
    MM_STATE_LIGHT_LN_LAST,
    /// Light Lightness Default
    MM_STATE_LIGHT_LN_DFLT,
    /// Light Lightness Range
    MM_STATE_LIGHT_LN_RANGE,
    /// Light Lightness Range Min
    MM_STATE_LIGHT_LN_RANGE_MIN,
    /// Light Lightness Range Max
    MM_STATE_LIGHT_LN_RANGE_MAX,

    /// Light CTL Lightness
    MM_STATE_LIGHT_CTL_LN = 100,
    /// Light CTL Temperature
    MM_STATE_LIGHT_CTL_TEMP,
    /// Light CTL Delta UV
    MM_STATE_LIGHT_CTL_DELTA_UV,
    /// Light CTL Temperature Default
    MM_STATE_LIGHT_CTL_TEMP_DFLT,
    /// Light CTL Temperature Range
    MM_STATE_LIGHT_CTL_TEMP_RANGE,
    /// Light CTL Delta UV Default
    MM_STATE_LIGHT_CTL_DELTA_UV_DFLT,

    /// Light HSL Lightness
    MM_STATE_LIGHT_HSL_LN = 150,
    /// Light HSL Hue
    MM_STATE_LIGHT_HSL_HUE,
    /// Light HSL Saturation
    MM_STATE_LIGHT_HSL_SAT,
    /// Light HSL Target
    MM_STATE_LIGHT_HSL_TGT,
    /// Light HSL Default (Lightness + Hue + Saturation)
    MM_STATE_LIGHT_HSL_DFLT,
    /// Light HSL Lightness Default
    MM_STATE_LIGHT_HSL_DFLT_LN,
    /// Light HSL Hue Default
    MM_STATE_LIGHT_HSL_DFLT_HUE,
    /// Light HSL Saturation Default
    MM_STATE_LIGHT_HSL_DFLT_SAT,
    /// Light HSL Hue Range
    MM_STATE_LIGHT_HSL_RANGE_HUE,
    /// Light HSL Saturation Range
    MM_STATE_LIGHT_HSL_RANGE_SAT,

    /// Light xyL Lightness
    MM_STATE_LIGHT_XYL_LN = 200,
    /// Light xyL x and y
    MM_STATE_LIGHT_XYL_XY,
    /// Light xyL Lightness Target
    MM_STATE_LIGHT_XYL_LN_TGT,
    /// Light xyL x and y Target
    MM_STATE_LIGHT_XYL_XY_TGT,
    /// Light xyL Lightness Default
    MM_STATE_LIGHT_XYL_LN_DFLT,
    /// Light xyL x and y Default
    MM_STATE_LIGHT_XYL_XY_DFLT,
    /// Light xyL x and y Range
    MM_STATE_LIGHT_XYL_XY_RANGE,

    MM_STATE_VENDORS_ATTR  = 250,

    MM_STATE_VENDORS_HSL  = 251
};

/// Type of state that can be set
enum mm_state_type
{
    /// Current state value
    MM_STATE_TYPE_CURRENT = 0,
    /// Targeted state value
    MM_STATE_TYPE_TARGET,
    /// Targeted state value during move transition
    MM_STATE_TYPE_TARGET_MOVE,
};

/// Generic Property type
enum mm_prop_type
{
    /// User Property
    MM_PROP_TYPE_USER = 0,
    /// Admin Property
    MM_PROP_TYPE_ADMIN,
    /// Manufacturer Property
    MM_PROP_TYPE_MANUF,
    /// Client Property
    MM_PROP_TYPE_CLI,
};

enum mm_adv_state
{
    PB_ADV_RUNNING,
    CUS_ADV_RUNNING,
    PB_CUS_RUNNING,
    ADV_IDLE,
    NEED_WAIT,
};

/*
 * TYPE DEFINITIONS FOR MESH MODELS
 ****************************************************************************************
 */

/// Mesh Model Configuration Structure
typedef struct mm_cfg
{
    /// Number of replay elements to allocate as part of the Replay Manager
    uint8_t nb_replay;
} mm_cfg_t;


/// Structure for Generic Property state information
typedef struct mm_prop
{
    /// Property ID
    uint16_t prop_id;
    /// User Access
    uint8_t user_access;
} mm_prop_t;

#if (BLE_MESH_MDL)

/*
 * CALLBACKS DEFINITION FOR MESH MODELS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Callback function called when a model has been registered.
 *
 * @param[in] model_id      Model Identifier
 * @param[in] elmt_idx      Element Index
 * @param[in] mdl_lid       Allocated model local index
 ****************************************************************************************
 */
typedef void (*mm_api_register_ind_cb)(uint32_t model_id, uint8_t elmt_idx, m_lid_t mdl_lid);

/**
 ****************************************************************************************
 * @brief Callback function called to inform application about a state update
 *
 * @param[in] state_id          State identifier (@see mm_state_idx)
 * @param[in] elmt_idx          Element Index
 * @param[in] state             New state value or targeted state value depending on
 * transition time
 * @param[in] trans_time_ms     Transition time in milliseconds
 ****************************************************************************************
 */
typedef void (*mm_api_srv_state_upd_ind_cb)(uint16_t state_id, uint8_t elmt_idx, uint32_t state,
        uint32_t trans_time_ms);

typedef void (*mm_api_srv_array_state_upd_ind_cb)(uint16_t state_id, uint8_t elmt_idx, uint8_t len, uint8_t *state,
        uint32_t trans_time_ms);
/**
 ****************************************************************************************
 * @brief Callback function called when a state value not stored locally is required.
 *
 * @param[in] req_ind_code      Request indication code
 * @param[in] elmt_idx          Element Index
 ****************************************************************************************
 */
typedef void (*mm_api_srv_state_req_ind_cb)(uint32_t req_ind_code, uint8_t elmt_idx);

/**
 ****************************************************************************************
 * @brief Callback function called when global part of Generic Location state has been
 * set.
 *
 * @param[in] elmt_idx      Element Index
 * @param[in] latitude      Global Latitude
 * @param[in] longitude     Global Longitude
 * @param[in] altitude      Global Altitude
 ****************************************************************************************
 */
typedef void (*mm_api_srv_locg_upd_ind_cb)(uint8_t elmt_idx, int32_t latitude, int32_t longitude,
        int16_t altitude);

/**
 ****************************************************************************************
 * @brief Callback function called when local part of Generic Location state has been
 * set.
 *
 * @param[in] elmt_idx      Element Index
 * @param[in] north         Local North
 * @param[in] east          Local East
 * @param[in] altitude      Local Altitude
 * @param[in] floor         Floor Number
 * @param[in] uncertainty   Uncertainty
 ****************************************************************************************
 */
typedef void (*mm_api_srv_locl_upd_ind_cb)(uint8_t elmt_idx, int16_t north, int16_t east,
        int16_t altitude, uint8_t floor, uint16_t uncertainty);

/**
 ****************************************************************************************
 * @brief Inform application about received get request for a Generic User/Admin/Manufacturer
 * Property. A confirmation is expected from the application.
 *
 * @param[in] elmt_idx      Index of element for which get request has been received
 * @param[in] prop_type     Property type (@see enum mm_prop_type)
 * @param[in] prop_id       Property ID
 ****************************************************************************************
 */
typedef void (*mm_api_srv_prop_get_req_ind_cb)(uint8_t elmt_idx, uint8_t prop_type, uint16_t prop_id);

/**
 ****************************************************************************************
 * @brief Inform application about received set request for a Generic User/Admin/Manufacturer
 * Property. A confirmation is expected from the application.
 *
 * @param[in] elmt_idx      Index of element for which set request has been received
 * @param[in] prop_type     Property type (@see enum mm_prop_type)
 * @param[in] prop_id       Property ID
 * @param[in] ack           True if confirmation is expected, else False
 * @param[in] length        Property value length
 * @param[in] p_val         Pointer to the received property value
 ****************************************************************************************
 */
typedef void (*mm_api_srv_prop_set_req_ind_cb)(uint8_t elmt_idx, uint8_t prop_type, uint16_t prop_id,
        bool ack, uint16_t length, uint8_t *p_val);

/**
 ****************************************************************************************
 * @brief Callback function called upon reception of a status message in order to inform
 * upper application about received state value.
 *
 * @param[in] src           Address of node's element for which state value is reported
 * @param[in] state_id      State Identifier (@see mm_state_idx)
 * @param[in] state_1       State value 1
 * @param[in] state_2       State value 2
 * @param[in] rem_time_ms   Remaining time in milliseconds before end of transition
 ****************************************************************************************
 */
typedef void (*mm_api_cli_state_ind_cb)(uint16_t src, uint16_t state_id, uint32_t state_1,
                                        uint32_t state_2, uint32_t rem_time_ms);

/**
 ****************************************************************************************
 * @brief Callback function called upon reception of Generic Battery Status message in order
 * to inform application about received Generic Battery state value.
 *
 * @param[in] src               Address of node's element for which state value is reported
 * @param[in] bat_lvl           Battery Level value
 * @param[in] time_discharge    Time to discharge in minutes
 * @param[in] time_charge       Time to charge in minutes
 * @param[in] flags             Flags
 ****************************************************************************************
 */
typedef void (*mm_api_cli_bat_ind_cb)(uint16_t src, uint8_t bat_lvl, uint32_t time_discharge,
                                      uint32_t time_charge, uint8_t flags);

/**
 ****************************************************************************************
 * @brief Callback function called upon reception of Generic Location Global Status message
 * in order to inform application about received global part of Generic Location state value.
 *
 * @param[in] src               Address of node's element for which state value is reported
 * @param[in] latitude          Global Latitude
 * @param[in] longitude         Global Longitude
 * @param[in] altitude          Global Altitude
 ****************************************************************************************
 */
typedef void (*mm_api_cli_locg_ind_cb)(uint16_t src, int32_t latitude, int32_t longitude,
                                       int16_t altitude);

/**
 ****************************************************************************************
 * @brief Callback function called upon reception of Generic Location Global Status message
 * in order to inform application about received local part of Generic Location state value.
 *
 * @param[in] src               Address of node's element for which state value is reported
 * @param[in] north             Local North
 * @param[in] east              Local East
 * @param[in] altitude          Local Altitude
 * @param[in] floor             Floor Number
 * @param[in] uncertainty       Uncertainty
 ****************************************************************************************
 */
typedef void (*mm_api_cli_locl_ind_cb)(uint16_t src, int16_t north, int16_t east, int16_t altitude,
                                       uint8_t floor, uint16_t uncertainty);

/**
 ****************************************************************************************
 * @brief Inform application about reception of a Generic User/Admin/Manufacturer Property
 * value
 *
 * @param[in] src           Address of element for which property value has been received
 * @param[in] prop_id       Property ID
 * @param[in] user_access   User access
 * @param[in] length        Property value length
 * @param[in] p_val         Pointer to the property value
 ****************************************************************************************
 */
typedef void (*mm_api_cli_prop_ind_cb)(uint16_t src, uint16_t prop_id, uint8_t user_access,
                                       uint16_t length, uint8_t *p_val);

/**
 ****************************************************************************************
 * @brief Inform application about reception of list of Generic User/Admin/Manufacturer/Client
 * Properties supported by an element
 *
 * @param[in] src           Address of element for which list of properties has been received
 * @param[in] prop_type     Property type (@see enum mm_prop_type)
 * @param[in] p_prop_ids    Pointer to received list of Property IDs
 ****************************************************************************************
 */
typedef void (*mm_api_cli_prop_list_ind_cb)(uint16_t src, uint8_t prop_type, uint16_t nb_prop,
        uint16_t *p_prop_ids);

/*
 * CALLBACK STRUCTURES FOR MESH MODELS
 ****************************************************************************************
 */

/// Mesh Models Callback Structure
typedef struct mm_api_cb
{
    /// Callback used to inform application about registered model
    mm_api_register_ind_cb cb_register_ind;
#if (BLE_MESH_MDL_SERVER)
    /// Callback used inform application about a state update
    mm_api_srv_state_upd_ind_cb cb_srv_state_upd_ind;

    mm_api_srv_array_state_upd_ind_cb cb_srv_array_state_upd_ind;
    /// Callback used to request state information from an application
    mm_api_srv_state_req_ind_cb cb_srv_state_req_ind;
#if (BLE_MESH_MDL_GENS)
    /// Callback used to inform application about set Generic Location state value
    /// (global part)
    mm_api_srv_locg_upd_ind_cb cb_srv_locg_upd_ind;
    /// Callback used to inform application about set Generic Location state value
    /// (local part)
    mm_api_srv_locl_upd_ind_cb cb_srv_locl_upd_ind;
    /// Callback used to request Generic Property state value
    mm_api_srv_prop_get_req_ind_cb cb_srv_prop_get_req_ind;
    /// Callback used to inform application about set Generic Property state value
    mm_api_srv_prop_set_req_ind_cb cb_srv_prop_set_req_ind;
#endif //(BLE_MESH_MDL_GENS)
#endif //(BLE_MESH_MDL_SERVER)
#if (BLE_MESH_MDL_CLIENT)
    /// Callback used to inform application about received state
    mm_api_cli_state_ind_cb cb_cli_state_ind;
#if (BLE_MESH_MDL_GENC)
    /// Callback used to inform application about received Generic Battery state value
    mm_api_cli_bat_ind_cb cb_cli_bat_ind;
    /// Callback used to inform application about received Generic Location state value
    /// (global part)
    mm_api_cli_locg_ind_cb cb_cli_locg_ind;
    /// Callback used to inform application about received Generic Location state value
    /// (local part)
    mm_api_cli_locl_ind_cb cb_cli_locl_ind;
    /// Callback used to inform application about a received Generic Property state value
    mm_api_cli_prop_ind_cb cb_cli_prop_ind;
    /// Callback used to inform application about received list of Generic User/Admin/Manufacturer
    /// Properties
    mm_api_cli_prop_list_ind_cb cb_cli_prop_list_ind;
#endif //(BLE_MESH_MDL_GENC)
#endif //(BLE_MESH_MDL_CLIENT)
} mm_api_cb_t;

/*
 * FUNCTION DECLARATIONS FOR MESH MODELS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Define the set of callback functions to communicate with Mesh Model block
 *
 * @param[in] p_cb_api      Callback functions for communication with Mesh Model block
 *
 * @return An error code
 ****************************************************************************************
 */
uint16_t mm_api_set(const mm_api_cb_t *p_cb_api);

#if (BLE_MESH_MDL_SERVER)
/**
 ****************************************************************************************
 * @brief Register Server model configuration for a local element
 *
 * @param[in] elmt_idx         Element Index
 * @param[in] mdl_cfg_idx      Server model configuration index (@see enum mm_cfg_idx)
 * @param[in] info             Information
 *
 * @return An error status (@see mesh_err)
 ****************************************************************************************
 */
uint16_t mm_api_register_server(uint8_t elmt_idx, uint8_t mdl_cfg_idx, uint8_t info);

#if (BLE_MESH_MDL_GENS)
/**
 ****************************************************************************************
 * @brief Register Generic Property models on an element
 *
 * @param[in] elmt_idx          Index of element on which models must be registered
 * @param[in] req_queue_len     Number of received messages that can be queued when model
 * is waiting for application confirmation
 * @param[in] nb_prop_user      Number of Generic User Properties
 * @param[in] nb_prop_admin     Number of Generic Admin Properties
 * @param[in] nb_prop_manuf     Number of Generic Manufacturer Properties
 * @param[in] nb_prop_cli       Number of Generic Client Properties
 * @param[in] p_props           Pointer to list of Generic Property information
 *
 * @return An handling error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_api_register_server_prop(uint8_t elmt_idx, uint8_t req_queue_len,
                                     uint8_t nb_prop_user, uint8_t nb_prop_admin,
                                     uint8_t nb_prop_manuf, uint8_t nb_prop_cli,
                                     const mm_prop_t *p_props);
#endif //(BLE_MESH_MDL_GENS)

/**
 ****************************************************************************************
 * @brief Set state value
 *
 * @param[in] p_env         Pointer to the environment allocated for the model
 * @param[in] state_id      State identifier
 * @param[in] state         State value
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t mm_api_srv_set(m_lid_t mdl_lid, uint16_t state_id, uint32_t state);

#if (BLE_MESH_MDL_GENS)
/**
 ****************************************************************************************
 * @brief Confirm value of Generic Battery state for a given local element after reception
 * of a request indication
 *
 * @param[in] status            Confirmation status
 * @param[in] elmt_idx          Element Index
 * @param[in] bat_lvl           Battery Level
 * @param[in] time_charge       Time to charge in minutes
 * @param[in] time_discharge    Time to discharge in minutes
 * @param[in] flags             Flags
 ****************************************************************************************
 */
void mm_api_srv_bat_cfm(uint16_t status, uint8_t elmt_idx, uint8_t bat_lvl, uint32_t time_charge,
                        uint32_t time_discharge, uint8_t flags);

/**
 ****************************************************************************************
 * @brief Confirm value global part of Generic Location state for a given local element
 * after reception of a request indication
 *
 * @param[in] status            Confirmation status
 * @param[in] elmt_idx          Element Index
 * @param[in] latitude          Global Latitude
 * @param[in] longitude         Global Longitude
 * @param[in] altitude          Global Altitude
 ****************************************************************************************
 */
void mm_api_srv_locg_cfm(uint16_t status, uint8_t elmt_idx, int32_t latitude, int32_t longitude,
                         int16_t altitude);

/**
 ****************************************************************************************
 * @brief Confirm value local part of Generic Location state for a given local element
 * after reception of a request indication
 *
 * @param[in] status            Confirmation status
 * @param[in] elmt_idx          Element Index
 * @param[in] north             Local North
 * @param[in] east              Local East
 * @param[in] altitude          Local Altitude
 * @param[in] floor             Floor number
 * @param[in] uncertainty       Uncertainty
 ****************************************************************************************
 */
void mm_api_srv_locl_cfm(uint16_t status, uint8_t elmt_idx, int16_t north, int16_t east,
                         int16_t altitude, uint8_t floor, uint16_t uncertainty);

/**
 ****************************************************************************************
 * @brief Confirmation functions for get and set request indication received by the application
 *
 * @param[in] status        Confirmation status
 * @param[in] elmt_idx      Element index
 * @param[in] prop_type     Property Type (@see enum mm_prop_type)
 * @param[in] prop_id       Property ID
 * @param[in] length        Property Length
 * @param[in] p_val         Pointer to the property value
 ****************************************************************************************
 */
void mm_api_srv_prop_cfm(uint16_t status, uint8_t elmt_idx, uint8_t prop_type, uint16_t prop_id,
                         uint16_t length, const uint8_t *p_val);
#endif //(BLE_MESH_MDL_GENS)
#endif //(BLE_MESH_MDL_SERVER)

#if (BLE_MESH_MDL_CLIENT)
/**
 ****************************************************************************************
 * @brief Register use of a client model
 *
 * @param[in] elmt_idx         Element Index
 * @param[in] cmdl_idx         Client model index (@see enum mm_cmdl_idx)
 *
 * @return Handling status
 ****************************************************************************************
 */
uint16_t mm_api_register_client(uint16_t cmdl_idx);

/**
 ****************************************************************************************
 * @brief Get value of a state
 *
 * @param[in] mdl_lid       Local index for the client model used to get the needed state value
 * @param[in] dst           Element address to which the get message must be sent
 * @param[in] get_info      Get information
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_api_cli_get(m_lid_t mdl_lid, uint16_t dst, uint16_t get_info);

/**
 ****************************************************************************************
 * @brief Set value of a state
 *
 * @param[in] mdl_lid       Local index for the client model used to set the needed state value
 * @param[in] dst           Element address to which the set message must be sent
 * @param[in] state_1       State value 1
 * @param[in] state_2       State value 2
 * @param[in] set_info      Set information
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_api_cli_set(m_lid_t mdl_lid, uint16_t dst, uint32_t state_1,
                        uint32_t state_2, uint16_t set_info);

/**
 ****************************************************************************************
 * @brief Set value of a state using a transition
 *
 * @param[in] mdl_lid           Local index for the client model used to set the needed state value
 * @param[in] dst               Element address to which the set message must be sent
 * @param[in] state_1           State value 1
 * @param[in] state_2           State value 2
 * @param[in] trans_time_ms     Transition Time in milliseconds
 * @param[in] delay_ms          Delay in milliseconds
 * @param[in] trans_info        Transition Information
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_api_cli_transition(m_lid_t mdl_lid, uint16_t dst, uint32_t state_1, uint32_t state_2,
                               uint32_t trans_time_ms, uint16_t delay_ms, uint16_t trans_info);

#if (BLE_MESH_MDL_GENC)
/**
 ****************************************************************************************
 * @brief Set global part of Generic Location state
 *
 * @param[in] mdl_lid       Local index for the client model used to set the needed state value
 * @param[in] dst           Element address to which the set message must be sent
 * @param[in] set_info      Set information
 * @param[in] latitude      Global Latitude
 * @param[in] longitude     Global Longitude
 * @param[in] altitude      Global Altitude
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_api_cli_set_locg(m_lid_t mdl_lid, uint16_t dst, uint8_t set_info,
                             int32_t latitude, int32_t longitude, int16_t altitude);

/**
 ****************************************************************************************
 * @brief Set local part of Generic Location state
 *
 * @param[in] mdl_lid       Local index for the client model used to set the needed state value
 * @param[in] dst           Element address to which the set message must be sent
 * @param[in] set_info      Set information
 * @param[in] north         Local North
 * @param[in] east          Local East
 * @param[in] altitude      Local Altitude
 * @param[in] floor         Floor Number
 * @param[in] uncertainty   Uncertainty
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_api_cli_set_locl(m_lid_t mdl_lid, uint16_t dst, uint8_t set_info,
                             int16_t north, int16_t east, int16_t altitude, uint8_t floor,
                             uint16_t uncertainty);

/**
 ****************************************************************************************
 * @brief Request to send a Generic User/Admin/Manufacturer/Client Property(ies) Get message
 * to an element
 *
 * @param[in] mdl_lid       Model Local Index of Generic Property Client model
 * @param[in] dst           Address of element to which the message must be sent
 * @param[in] prop_id       Property ID
 *
 * @return An handling status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_api_cli_get_prop(m_lid_t mdl_lid, uint16_t dst, uint8_t get_type, uint16_t prop_id);

/**
 ****************************************************************************************
 * @brief Request to send a Generic User/Admin/Manufacturer Property Set message to an
 * element
 *
 * @param[in] mdl_lid       Model local Index of Generic Property Client model
 * @param[in] dst           Address of element to which the message must be sent
 * @param[in] set_info      Set information
 * @param[in] prop_id       Property ID
 * @param[in] user_access   User access
 * @param[in] length        Property value length
 * @param[in] p_val         Pointer to the property value
 *
 * @return An handling status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_api_cli_set_prop(m_lid_t mdl_lid, uint16_t dst, uint8_t set_info, uint16_t prop_id,
                             uint8_t user_access, uint16_t length, const uint8_t *p_val);
#endif //(BLE_MESH_MDL_GENC)
#endif //(BLE_MESH_MDL_CLIENT)
#endif //(BLE_MESH_MDL)

/*
 * TYPE DEFINITIONS FOR MESH STACK
 ****************************************************************************************
 */

/// Mesh Stack Configuration Structure
typedef struct mesh_cfg
{
    /// Mesh Profile Configuration
    m_cfg_t prf_cfg;
#if (BLE_MESH_MDL)
    /// Mesh Model Configuration
    mm_cfg_t model_cfg;
#endif //(BLE_MESH_MDL)
} mesh_cfg_t;

/// Mesh Stack Version Structure
typedef struct mesh_version
{
    /// Mesh Specification version (X.Y.Z)
    uint8_t mesh_version_x;
    uint8_t mesh_version_y;
    uint8_t mesh_version_z;

    /// Mesh Software version (X.Y)
    uint8_t mesh_sw_version_x;
    uint8_t mesh_sw_version_y;
} mesh_version_t;

/*
 * CALLBACK DEFINITIONS FOR MESH STACK
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Callback executed when a buffer block has been properly released.
 *
 * @param[in] block_id  Buffer Block identifier
 ****************************************************************************************
 */
typedef void (*mesh_api_buf_block_released_cb)(uint8_t block_id);

/*
 * CALLBACK STRUCTURES FOR MESH STACK
 ****************************************************************************************
 */

/// Callbacks structure for Mesh Stack
typedef struct mesh_api_cb
{
    /// Callback used to inform that a buffer block has been properly released
    mesh_api_buf_block_released_cb cb_buf_block_freed;
} mesh_api_cb_t;

/*
 * FUNCTION DECLARATIONS FOR MESH STACK
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief TODO [LT]
 ****************************************************************************************
 */
uint8_t mesh_handler(uint16_t msg_id, uint16_t src_id, const void *p_param);

/**
 ****************************************************************************************
 * @brief Define the set of callback to communicate with mesh native API
 *
 * @param[in] p_cb_api Native application callback set use to communicate with a native API
 *
 * @return Execution Status code
 ****************************************************************************************
 */
uint16_t mesh_api_set(const mesh_api_cb_t *p_cb_api);

/**
 ****************************************************************************************
 * @brief Get device run time
 *
 * @param[out] p_clock_ms       Pointer to variable that will contain the current clock in milliseconds.
 * @param[out] p_nb_wrap        Pointer to variable that will contain the number of wrap.
 ****************************************************************************************
 */
void mesh_api_get_run_time(uint32_t *p_clock_ms, uint16_t *p_nb_wrap);

/**
 ****************************************************************************************
 * @brief Set device run time
 *
 * @param[out] clock_ms       Current clock in milliseconds.
 * @param[out] nb_wrap        Number of wraps.
 ****************************************************************************************
 */
uint16_t mesh_api_set_run_time(uint32_t clock_ms, uint16_t nb_wrap);

/**
 ****************************************************************************************
 * @brief Get Mesh Stack version
 *
 * @param[out] p_version       Pointer to structure in which version information will be provided
 ****************************************************************************************
 */
void mesh_api_get_version(mesh_version_t *p_version);

/**
 ****************************************************************************************
 * @brief Allocate block of buffers
 *
 * @param[out] p_block_id  Pointer to the variable that will contain the index of the allocated block.
 * @param[in]  nb_bufs     Number of buffers contained in the block.
 * @param[in]  small       Indicate if block contains small or long buffers.
 *
 * @return Execution Status code
 ****************************************************************************************
 */
uint16_t mesh_api_buf_alloc_block(uint8_t *p_block_id, uint8_t nb_bufs, bool small);

/**
 ****************************************************************************************
 * @brief Free block of buffers
 *
 * @note cb_release() of m_api_cb_t called at end of disable execution
 *
 * @param[in] block_id   Index of the allocated block.
 *
 * @return Execution Status code
 ****************************************************************************************
 */
uint16_t mesh_api_buf_free_block(uint8_t block_id);

/// @} MESH_API_

#endif /* MESH_API_ */
