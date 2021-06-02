/**
 ****************************************************************************************
 *
 * @file m_lay.h
 *
 * @brief Header file for Mesh Layers
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_LAY_H_
#define _M_LAY_H_

/**
 ****************************************************************************************
 * @defgroup M_LAY Mesh Layers
 * @ingroup MESH
 * @brief Mesh Layers
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh_defines.h"
#include "m_tb.h"       // Toolbox defines

/*
 * TYPE DEFINITIONS (BUFFER ENVIRONMENT)
 ****************************************************************************************
 */

/// Buffer environment (from Access Layer to Network Layer)
typedef struct m_lay_buf_env
{
    /// Flags (position must be aligned with flags value in @see m_lay_bearer_buf_env)
    uint8_t  flags;
    /// Network key local index
    m_lid_t  net_lid;
    /// Application key local index
    /// Note: For a Proxy Protocol Message, This identifier contains the bearer local identifier
    m_lid_t  app_lid;
    union
    {
        /// Model local index (valid if buffer does not contain a control message
        m_lid_t model_lid;
        /// Source layer of a control message to be transmitted (@see enum m_lay_ctl)
        uint8_t layer_ctl_src;
    } u3;
    /// Sequence number or Sequence Authentication fields (Seq_Auth)
    uint32_t seq;
    union
    {
        /// IV value used for encryption/decryption by Upper Transport Layer and Network Layer
        uint32_t iv;
        /// Opcode (used by Access Layer to store opcode of a received message)
        uint32_t opcode;
    } u5;
    /// Source address of the message
    uint16_t src;
    /// Destination address of the message
    uint16_t dst;
    /// bit[7]   CTL: 1 = Control message | 0 = Access Message
    /// bit[6:0] TTL: Time to leave value; range [0-127]
    uint8_t  ttl_ctl;
    union
    {
        /// TX Handle (used by Access Layer, valid only for TX)
        uint8_t tx_hdl;
        /// RSSI (valid only for RX)
        int8_t  rssi;
    } u9;
    /// Virtual Address Local Index
    m_lid_t  va_lid;
    /// Friendship Local Index
    m_lid_t  friend_lid;
    union
    {
        /// First buffer pointer (Upper Transport Layer)
        mesh_tb_buf_t *p_buf_1;
        /// Seq Zero (used when Block Ack is stored for a Low Power Node)
        uint16_t   seq_zero;
    } u12;
    /// Second buffer pointer (Network Layer)
    mesh_tb_buf_t *p_buf_2;
    union
    {
        /// Opcode (Control) or AID/AFK (Access)
        uint8_t opcode_aid_afk;
        /// Interface index for Network Layer (used by Network Layer, valid only for TX)
        uint8_t intf_idx_net;
    } u14;
    union
    {
        /// Interface index for Upper Transport Layer (valid only for TX)
        uint8_t intf_idx_utrans;
        /// Number of decryption attempts (used by Upper Transport Layer upon reception of buffer)
        uint8_t nb_dec_attempts;
        /// Reassembly structure index (used by Lower Transport Layer upon reception of segments)
        uint8_t reas_idx;
        /// Used internally by Friend Layer in order to known how many segments have to be sent for the buffer
        /// Valid only if RX_SEG bit is set in flags field
        uint8_t nb_seg;
    } u15;
    union
    {
        /// Number of decryption attempts due to identical Label UUIDs (used by Upper Transport Layer)
        uint8_t nb_dec_attemps_va;
        /// Segmentation structure index (used by Lower Transport Layer during transmission of segments)
        uint8_t seg_idx;
    } u16;
} m_lay_buf_env_t;

/// Buffer environment (from Bearer Layer to Network Layer)
typedef struct m_lay_bearer_buf_env
{
    /// Flags (position must be aligned with flags value in @see m_lay_buf_env_t)
    uint8_t    flags;
    /// Bearer local index
    m_lid_t    bearer_lid;
    /// Msg Type (@see enum m_bearer_msg)
    uint8_t    msg_type;
    /// Client of the message (@see enum m_bearer_msg)
    uint8_t    client;
    /// Pointer to a buffer (basically buffer containing decrypted network PDU
    mesh_tb_buf_t *p_buf;
    /// IV (used by Network Layer)
    uint32_t   iv;
    /// Bitfield indicating bearers on which PDU has to be sent (used by Proxy Layer)
    uint32_t   tx_bearer_bf;
    /// RSSI (valid only for reception)
    int8_t     rssi;
    /// Number of decryption attempts
    uint8_t    nb_dec_attempts;
    union
    {
        /// Network Key local index
        m_lid_t net_key_lid;
        /// Subnet local index
        m_lid_t subnet_lid;
    } u9;
    /// MIC Length (used by Network Layer)
    uint8_t    mic_len;
    /// Friendship local index
    uint8_t    friend_lid;
} m_lay_bearer_buf_env_t;

/*
 * CALLBACK DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Definition of function to call for transmission of a PDU through a layer.
 *
 * @param[in] p_buf           Pointer to the buffer containing the received PDU.
 ****************************************************************************************
 */
typedef uint16_t (*m_lay_send_api) (mesh_tb_buf_t *p_buf);


/**
 ****************************************************************************************
 * @brief Definition of function to call for transmission of a PDU through upper transport layer.
 *
 * @param[in] p_buf           Pointer to the buffer containing the received PDU.
 * @param[in] intf_type       Interface type
 ****************************************************************************************
 */
typedef uint16_t (*m_lay_trans_send_api) (mesh_tb_buf_t *p_buf, uint8_t intf_type);

/**
 ****************************************************************************************
 * @brief Definition of callback function to call upon reception of a PDU in order to notify
 * upper layer about the received PDU.
 *
 * @param[in] p_buf             Pointer to the buffer containing the received PDU.
 ****************************************************************************************
 */
typedef void (*m_lay_rx_cb) (mesh_tb_buf_t *p_buf);

/**
 ****************************************************************************************
 * @brief Definition of callback function to call once PDU provided by an upper layer has
 * properly been sent.
 *
 * @param[in] p_buf             Pointer to the buffer containing the sent PDU.
 * @param[in] status            Transmission status.
 ****************************************************************************************
 */
typedef void (*m_lay_sent_cb) (mesh_tb_buf_t *p_buf, uint16_t status);

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh layers module memory requirement
 *
 * @note this function is called before init function in order to know memory requirements of the module
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this mesh module
 ****************************************************************************************
 */
uint16_t m_lay_get_env_size(const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Mesh layers initialization
 *
 * @note called when Mesh stack is initialized or when a SW reset is requested.
 *
 * @param[in] reset True means SW reset, False means task initialization
 * @param[in] p_env Pointer to the environment structure
 * @param[in] p_cfg Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_lay_init(bool reset, void *p_env, const m_cfg_t *p_cfg);

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
 * @param[in] trans_mic_64 For a segmented PDU force transport mic to 64 bits
 *
 * @return Execution status code
 ****************************************************************************************
 */
uint16_t m_lay_access_publish(m_lid_t model_lid, uint32_t opcode, uint8_t tx_hdl, mesh_tb_buf_t *p_buf, bool trans_mic_64);

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
uint16_t m_lay_access_rsp_send(m_lid_t model_lid, uint32_t opcode, uint8_t tx_hdl, mesh_tb_buf_t *p_buf,
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
void m_lay_access_opcode_status(m_lid_t model_lid, uint32_t opcode, uint16_t status);

#if (BLE_MESH_LPN)
/**
 ****************************************************************************************
 * @brief Start Low Power Node feature
 *
 * @param[in] poll_timeout          Poll Timeout
 * @param[in] poll_intv_ms          Poll Interval in milliseconds
 * @param[in] prev_addr             Address of previous Friend Node
 * @param[in] rx_delay              Receive delay
 * @param[in] rssi_factor           RSSI factor for friendship establishment
 * @param[in] rx_window_factor      Receive Window factor for friendship establishment
 * @param[in] min_queue_size_log    Minimum required queue size (log value)
 *
 * @return Error status
 ****************************************************************************************
 */
uint16_t m_lay_lpn_start(uint32_t poll_timeout, uint32_t poll_intv_ms, uint16_t prev_addr, uint8_t rx_delay,
                         uint8_t rssi_factor, uint8_t rx_window_factor, uint8_t min_queue_size_log);

/**
 ****************************************************************************************
 * @brief Stop Low Power Node feature
 *
 * @return Error status
 ****************************************************************************************
 */
uint16_t m_lay_lpn_stop(void);

/**
 ****************************************************************************************
 * @brief Select Friend Node with which friendship must be established
 *
 * @param[in] friend_addr   Friend Node address
 *
 * @return Error status
 ****************************************************************************************
 */
uint16_t m_lay_lpn_select_friend(uint16_t friend_addr);

#if (!BLE_MESH_STORAGE_NONE)
/**
 ****************************************************************************************
 * @brief Load information about friendship with a Friend Node
 *
 * @param[in] p_info            Pointer to friendship information
 * @param[in] end_cb            Callback function called once friendship has been added
 *
 * @return Error status
 ****************************************************************************************
 */
uint16_t m_lay_lpn_load(m_api_end_cb end_cb);
#endif //(!BLE_MESH_STORAGE_NONE)
#endif //(BLE_MESH_LPN)

#if (BLE_MESH_FRIEND)
#if (!BLE_MESH_STORAGE_NONE)
/**
 ****************************************************************************************
 * @brief Load information about friendship with a Low Power Node
 *
 * @param[in] p_lpn_info        Pointer to friendship information
 * @param[out] p_lpn_lid        Pointer to variable that will contain the allocated friendship
 * local index
 * @param[in] end_cb            Callback function called once friendship has been added
 *
 * @return Error status
 ****************************************************************************************
 */
uint16_t m_lay_friend_load_lpn(m_tb_friend_lpn_info_t *p_lpn_info, m_lid_t *p_lpn_lid,
                               m_api_end_cb end_cb);
#endif //(!BLE_MESH_STORAGE_NONE)
#endif //(BLE_MESH_FRIEND)

#if (BLE_MESH_GATT_PROXY)
/**
 ****************************************************************************************
 * @brief Change state of Mesh GATT Proxy
 *
 * @param[in] enable True to enable GATT Proxy, False to stop GATT Proxy and all on-going connections
 *
 * @return Execution status, MESH_ERR_NO_ERROR if succeed
 ****************************************************************************************
 */
uint16_t m_lay_proxy_state_set(bool enable);

/**
 ****************************************************************************************
 * @brief Control state of the GATT Proxy Service Advertising state
 *
 * @param[in] enable        True to enable advertising for 60s (@see enum mesh_proxy_adv_ctl)
 * @param[in] net_key_lid   Network key local identifier used to target a specific Node Identity
 *                          If Invalid LID, interlace Network ID of all available network keys.
 * @param[in] reason        Update reason (@see enum mesh_proxy_adv_upd_reason)
 *
 * @return Execution status, MESH_ERR_NO_ERROR if succeed
 ****************************************************************************************
 */
uint16_t m_lay_proxy_bearer_adv_ctrl(uint8_t enable, m_lid_t net_key_lid, uint8_t reason);
#endif //(BLE_MESH_GATT_PROXY)

/// @} end of group

#endif //_M_LAY_INT_H_
