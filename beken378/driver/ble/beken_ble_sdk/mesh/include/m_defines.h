/**
 ****************************************************************************************
 *
 * @file m_defines.h
 *
 * @brief Header file for Mesh Define
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef M_DEFINES_
#define M_DEFINES_

/**
 ****************************************************************************************
 * @defgroup M_DEFINES Mesh Defines
 * @ingroup MESH
 * @brief  Mesh Defines
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mal_lib.h"       // Mesh Abstraction Layer Library

/*
 * DEFINES
 ****************************************************************************************
 */

/// Unassigned address
#define M_UNASSIGNED_ADDR                   (0x0000)

/// Network identifier length
#define M_NET_ID_LEN                        (8)
/// Packet sequence number length
#define M_SEQ_LEN                           (3)
/// Address length
#define M_ADDR_LEN                          (2)
/// Key IDs length
#define M_KEY_ID_LEN                        (2)
/// Size of Label UUID
#define M_LABEL_UUID_LEN                    (16)
/// Authentication data OOB length
#define M_OOB_AUTH_DATA_LEN                 (16)
/// 64 bit random value
#define M_PROXY_RANDOM_LEN                  (8)
/// 64 bit HASH value
#define M_PROXY_HASH_LEN                    (8)

/// Maximum Publish TTL value
#define M_TTL_MAX                           (0x7F)
/// Publish TTL value indicating that Default TTL  value must be used
#define M_TTL_USE_DEFAULT                   (0xFF)

/// Fixed group address - All proxies
#define M_ALL_PROXIES_ADDR                  (0xFFFC)
/// Fixed group address - All friends
#define M_ALL_FRIENDS_ADDR                  (0xFFFD)
/// Fixed group address - All relays
#define M_ALL_RELAYS_ADDR                   (0xFFFE)
/// Fixed group address - All nodes
#define M_ALL_NODES_ADDR                    (0xFFFF)

/// Length of Network MIC for an Access Message
#define M_NMIC_ACC_LEN                      (4)
/// Length of Network MIC for a Control Message
#define M_NMIC_CTL_LEN                      (8)

/// Perform Connectable Advertising during 60s
#define M_PROXY_CON_ADV_DUR                 (60000)

// -----------------------------------------------------------------------------
//                         Network Layer defines
// -----------------------------------------------------------------------------

/// Advertising Data type header
#define M_BEARER_ADV_HEAD_LEN               (2)

// -----------------------------------------------------------------------------
//                    Lower Transport Layer defines
// -----------------------------------------------------------------------------

/// Size of the lower layer required buffer header length
#define M_LAY_TRANS_BUF_HEADER_LEN          (9)
/// Size of the lower layer required buffer tail length for access message
#define M_LAY_TRANS_BUF_TAIL_ACCESS_LEN     (4)
/// Size of the lower layer required buffer tail length for control message
#define M_LAY_TRANS_BUF_TAIL_CTL_LEN        (8)
/// Maximum size of unsegmented control packet
#define M_LAY_TRANS_CTL_UNSEG_SIZE          (11)
/// Maximum size of unsegmented access packet
#define M_LAY_TRANS_ACCESS_UNSEG_SIZE       (15)
/// Size of a Control packet segment
#define M_LAY_TRANS_CTL_SEG_SIZE            (8)
/// Size of an Access packet segment
#define M_LAY_TRANS_ACCESS_SEG_SIZE         (12)
/// Size of a Block ACK packet
#define M_LAY_TRANS_BLOCK_ACK_LEN           (7)
#define M_LAY_TRANS_BLOCK_HEADER_LEN        (3)
/// Segment packet header length
#define M_LAY_TRANS_SEG_HEADER_LEN          (4)
/// Unsegmented packet header length
#define M_LAY_TRANS_UNSEG_HEADER_LEN        (1)
/// Incomplete Timer for reassembly default duration (10 seconds in milliseconds)
#define M_LAY_TRANS_INCOMPLETE_TIMER_DUR    (1000)//(10000)
/// Acknowledgment timer minimum duration (150 milliseconds)
#define M_LAY_TRANS_ACK_TIMER_MIN_DUR       (200)
/// Acknowledgment timer TTL duration (50 milliseconds * TTL)
#define M_LAY_TRANS_ACK_TIMER_TTL_DUR       (50)
/// Segment transmission timer minimum duration (200 milliseconds)
#define M_LAY_TRANS_SEG_TX_TIMER_MIN_DUR    (150)
/// Segment transmission timer TTL duration (50 milliseconds * TTL)
#define M_LAY_TRANS_SEG_TX_TIMER_TTL_DUR    (50)
/// SEQ Zero of Upper Transport PDU is a 13 bits number
#define M_LAY_TRANS_SEQ_ZERO_MASK           (0x00001FFF)
#define M_LAY_TRANS_SEQ_ZERO_WRAP           (0x00002000)

// -----------------------------------------------------------------------------
//                    Access Layer defines
// -----------------------------------------------------------------------------

/// Size of the access layer required buffer header length
#define M_LAY_ACCESS_BUF_HEADER_LEN         (3)
/// Size of the access layer required buffer tail length
#define M_LAY_ACCESS_BUF_TAIL_LEN           (0)
/// Maximum Access PDU Length
#define M_ACCESS_PDU_LENGTH_MAX             (384)
/// Maximum Access Payload Parameter Length
#define M_ACCESS_PAYLOAD_PARAM_LENGTH_MAX   (379)

// -----------------------------------------------------------------------------
//                    Provisioning defines
// -----------------------------------------------------------------------------

/// Generic Provisioning PDU maximum size.
#define M_PROV_GEN_PLD_MAX_SIZE             (64)
/// Minimum size of a provisioning packet on PB-ADV bearer
#define M_PROV_PB_ADV_PKT_MIN_LEN           (6)
/// Maximum transmit size of PB-ADV bearer
#define M_PROV_PB_ADV_MTU                   (24)
/// Unprovisioned device UUID size
#define M_PROV_DEV_UUID_LEN                 (16)
/// Link Close reason field length
#define M_PROV_REASON_LEN                   (1)
/// Transaction start PDU minimum length
#define M_PROV_TRANS_START_PDU_MIN_LEN      (4)
/// Maximum size of transaction start PDU payload size
#define M_PROV_TRANS_START_PLD_MAX_LEN      (19)
/// Maximum size of transaction continue PDU payload size
#define M_PROV_TRANS_CONT_PLD_MAX_LEN       (23)
/// 30s of transaction timeout
#define M_PROV_TRANS_TIMEOUT                (30000)
/// 60s of Link timeout
#define M_PROV_LINK_TIMEOUT                 (60000)
/// 1s of Wait Close Link timeout
#define M_PROV_LINK_FAILED_TIMEOUT          (1000)
/// 50ms Semd Close Link timeout (since no ACK are transmitted)
#define M_PROV_LINK_CLOSE_TIMEOUT           (500)
/// Static OOB size
#define M_PROV_STATIC_OOB_SIZE              (16)
/// 700ms of Wait regenerate the ECDH key.
#define M_PROV_ECDH_REGEN_TIMEOUT           (1200)

/// Minimum receive delay value (in units of 1ms)
#define M_RECEIVE_DELAY_MS_MIN              (0x0A)
/// Minimum receive window value (in units of 1ms)
#define M_RECEIVE_WINDOW_MS_MIN             (0x01)
/// Minimum poll timeout value (in units of 100ms)
#define M_POLL_TIMEOUT_100MS_MIN            (0x00000A)
/// Maximum poll timeout value (in units of 100ms)
#define M_POLL_TIMEOUT_100MS_MAX            (0x34BBFF)

/*
 * MACROS
 ****************************************************************************************
 */

/// Macro returning if an address is a unicast address.
#define M_IS_UNICAST_ADDR(addr)         ((addr & COMMON_BIT(15)) == 0x0000)
/// Macro returning if an address is a group address.
#define M_IS_GROUP_ADDR(addr)           ((addr & (COMMON_BIT(15) | COMMON_BIT(14))) == (COMMON_BIT(15) | COMMON_BIT(14)))
/// Macro returning if an address is a fixed group address
#define M_IS_FIXED_GROUP_ADDR(addr)     ((addr & 0xFF00) == 0xFF00)
/// Macro returning if an address is a virtual address
#define M_IS_VIRTUAL_ADDR(addr)         ((addr & (COMMON_BIT(15) | COMMON_BIT(14))) == COMMON_BIT(15))

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Advertising packet type
enum m_bearer_adv_type
{
    /// PB-ADV Advertising data type
    M_BEARER_ADV_TYPE_PB_ADV   = (0x29),
    /// Mesh Message Advertising data type
    M_BEARER_ADV_TYPE_MSG      = (0x2A),
    /// Mesh Beacon Advertising data type
    M_BEARER_ADV_TYPE_BEACON   = (0x2B),
};

/// State of the provisioning
enum m_prov_state
{
    /// Provisioning started - procedure started by a provisioner
    M_PROV_STARTED,
    /// Provisioning succeed
    M_PROV_SUCCEED,
    //Provsioning adv ctr
    M_PROV_ADVCTL,
    /// Provisioning failed
    M_PROV_FAILED,
};

/// Provisioning PDU Type values
enum m_prov_pdu_types
{
    /// Invites a device to join a mesh network
    M_PROV_PDU_INVITE             = 0x00,
    M_PROV_PDU_INVITE_LEN         = 1,
    /// Indicates the capabilities of the device
    M_PROV_PDU_CAPABILITIES       = 0x01,
    M_PROV_PDU_CAPABILITIES_LEN   = 11,
    /// Indicates the provisioning method selected by the Provisioner based on the capabilities of the device
    M_PROV_PDU_START              = 0x02,
    M_PROV_PDU_START_LEN          = 5,
    /// Contains the Public Key of the device or the Provisioner
    M_PROV_PDU_PUBLIC_KEY         = 0x03,
    M_PROV_PDU_PUBLIC_KEY_LEN     = 64,
    /// Indicates that the user has completed inputting a value
    M_PROV_PDU_INPUT_COMPLETE     = 0x04,
    M_PROV_PDU_INPUT_COMPLETE_LEN = 0,
    /// Contains the provisioning confirmation value of the device or the Provisioner
    M_PROV_PDU_CONFIRMATION       = 0x05,
    M_PROV_PDU_CONFIRMATION_LEN   = 16,
    /// Contains the provisioning random value of the device or the Provisioner
    M_PROV_PDU_RANDOM             = 0x06,
    M_PROV_PDU_RANDOM_LEN         = 16,
    /// Includes the assigned unicast address of the primary element, a network key, NetKey Index, Flags and the IV Index
    M_PROV_PDU_DATA               = 0x07,
    M_PROV_PDU_DATA_LEN           = 33,
    /// Indicates that provisioning is complete
    M_PROV_PDU_COMPLETE           = 0X08,
    M_PROV_PDU_COMPLETE_LEN       = 0,
    /// Indicates that provisioning was unsuccessful
    M_PROV_PDU_FAILED             = 0x09,
    M_PROV_PDU_FAILED_LEN         = 1,

    M_PROV_PDU_MAX                = 0x0A,
};

/// Algorithms field values
enum m_prov_algo
{
    /// Bit[0]: FIPS P-256 Elliptic Curve
    M_PROV_ALGO_FIPS_P256_ELLIPTIC_CURVE = 0x0001,
    /// Bit[1-15]: Reserved for Future Use
};

/// Public Key field values
enum m_prov_pub_key_type
{
    /// Bit[0]: Public Key OOB information available
    M_PROV_PUB_KEY_OOB_AVAILABLE = 0x01,
    /// Bit[1-7]: Prohibited
};


/// Public Key Type field values
enum m_prov_pub_key_val
{
    /// No OOB Public Key is used
    M_PROV_PUB_KEY_OOB_NOT_USED = 0x00,
    /// OOB Public Key is used
    M_PROV_PUB_KEY_OOB_USED     = 0x01,

    M_PROV_PUB_KEY_OOB_MAX,
};


/// Authentication Method field values
enum m_prov_auth_method
{
    /// No OOB authentication is used
    M_PROV_AUTH_NO_OOB,
    /// Static OOB authentication is used
    M_PROV_AUTH_STATIC_OOB,
    /// Output OOB authentication is used
    M_PROV_AUTH_OUTPUT_OOB,
    /// Input OOB authentication is used
    M_PROV_AUTH_INPUT_OOB,
};


/// Static OOB Type field values
enum m_prov_static_oob
{
    /// Bit[0]: Static OOB information available
    M_PROV_STATIC_OOB_AVAILABLE = 0x01,
    /// Bit[1-7]: Prohibited
};

/// Static OOB size
#define M_PROV_STATIC_OOB_SIZE  (16)


/// Output OOB Action field values
enum m_prov_out_oob
{
    /// Bit[0]: Blink
    M_PROV_OUT_OOB_BLINK        = 0x0001,
    /// Bit[1]: Beep
    M_PROV_OUT_OOB_BEEP         = 0x0002,
    /// Bit[2]: Vibrate
    M_PROV_OUT_OOB_VIBRATE      = 0x0004,
    /// Bit[3]: Output Numeric
    M_PROV_OUT_OOB_NUMERIC      = 0x0008,
    /// Bit[4]: Output Alphanumeric
    M_PROV_OUT_OOB_ALPHANUMERIC = 0x0010,
    /// Bit[5-15]: Reserved for Future Use
};


/// Input OOB Action field values
enum m_prov_in_oob
{
    /// Bit[0]: Push
    M_PROV_IN_OOB_PUSH         = 0x0001,
    /// Bit[1]: Twist
    M_PROV_IN_OOB_TWIST        = 0x0002,
    /// Bit[2]: Input Numeric
    M_PROV_IN_OOB_NUMERIC      = 0x0004,
    /// Bit[3]: Input Alphanumeric
    M_PROV_IN_OOB_ALPHANUMERIC = 0x0008,
    /// Bit[4-15]: Reserved for Future Use
};

enum m_prov_info
{
    /// URI Hash present or not in the unprovisioned device beacon
    M_PROV_INFO_URI_HASH_PRESENT = (1 << 0),
};

/// Generic Provisioning Control Format field values
enum m_prov_gpcf
{
    /// Transaction Start
    M_PROV_GPCF_TRANS_START = 0x00,
    /// Transaction Acknowledgment
    M_PROV_GPCF_TRANS_ACK   = 0x01,
    /// Transaction Continuation
    M_PROV_GPCF_TRANS_CONT  = 0x02,
    /// Provisioning Bearer Control
    M_PROV_GPCF_BEARER_CTL  = 0x03,
};


/// Provisioning Bearer operation code
enum m_prov_bearer_opcode
{
    /// Open a session on a bearer with a device
    M_PROV_LINK_OPEN = 0x00,
    /// Acknowledge a session on a bearer
    M_PROV_LINK_ACK   = 0x01,
    /// Close a session on a bearer
    M_PROV_LINK_CLOSE  = 0x02,
};


/// Bit field of Generic provisioning PDU
///   7   6   5   4   3   2   1   0
/// +---+---+---+---+---+---+---+---+
/// |         SEG_N         |  GPCF |
/// +---+---+---+---+---+---+---+---+
/// or
/// +---+---+---+---+---+---+---+---+
/// |         OPCODE        |  GPCF |
/// +---+---+---+---+---+---+---+---+
enum m_prov_header
{
    /// Generic Provisioning control format (@see enum m_prov_gpcf)
    M_PROV_GPCF_LSB     = 0,
    M_PROV_GPCF_MASK    = 0x03,
    /// Segment Number (last for transaction start)
    M_PROV_SEG_N_LSB    = 2,
    M_PROV_SEG_N_MASK   = 0xFC,
    /// Bearer operation code
    M_PROV_OPCODE_LSB   = 2,
    M_PROV_OPCODE_MASK  = 0xFC,
};

// -----------------------------------------------------------------------------
//                    Configuration Model Defines
// -----------------------------------------------------------------------------

/// Secure network beacon state values
enum m_conf_bcn_state
{
    /// The node is not broadcasting a Secure Network beacon
    M_CONF_BCN_STATE_NOT_BROAD = 0x00,
    /// The node is broadcasting a Secure Network beacon
    M_CONF_BCN_STATE_BROAD
};

/// Relay state values
enum m_conf_relay_state
{
    /// Relay feature supported and disabled
    M_CONF_RELAY_STATE_DISABLED = 0x00,
    /// Relay feature supported and enabled
    M_CONF_RELAY_STATE_ENABLED,
    /// Relay feature not supported
    M_CONF_RELAY_STATE_NOT_SUPPORTED,
};

/// GATT proxy state values
enum m_conf_gatt_proxy_state
{
    /// GATT proxy feature supported and disabled
    M_CONF_GATT_PROXY_STATE_DISABLED = 0x00,
    /// GATT proxy feature supported and enabled
    M_CONF_GATT_PROXY_STATE_ENABLED,
    /// GATT proxy feature not supported
    M_CONF_GATT_PROXY_STATE_NOT_SUPPORTED,
};

/// Friend state values
enum m_conf_friend_state
{
    /// Friend feature supported and disabled
    M_CONF_FRIEND_STATE_DISABLED = 0x00,
    /// Friend feature supported and enabled
    M_CONF_FRIEND_STATE_ENABLED,
    /// Friend feature not supported
    M_CONF_FRIEND_STATE_NOT_SUPPORTED,
};

/// Node Identity state values
enum m_conf_node_id_state
{
    /// Node Identity for a subnet is stopped
    M_CONF_NODE_ID_STOPPED = 0x00,
    /// Node Identity for a subnet is running
    M_CONF_NODE_ID_RUNNING,
    /// Node identity is not supported
    M_CONF_NODE_ID_NOT_SUPPORTED,
};

// -----------------------------------------------------------------------------
//                    Friend / Low Power Node Defines
// -----------------------------------------------------------------------------

/// Receive window factor values
enum m_rx_window_factor
{
    /// 1
    M_RX_WINDOW_FACTOR_1   = 0,
    /// 1.5
    M_RX_WINDOW_FACTOR_1_5,
    /// 2
    M_RX_WINDOW_FACTOR_2,
    /// 2.5
    M_RX_WINDOW_FACTOR_2_5,
};

/// RSSI factor values
enum m_rssi_factor
{
    /// 1
    M_RSSI_FACTOR_1   = 0,
    /// 1.5
    M_RSSI_FACTOR_1_5,
    /// 2
    M_RSSI_FACTOR_2,
    /// 2.5
    M_RSSI_FACTOR_2_5,
};

/// Minimum cache size log values
enum m_min_cache_size_log
{
    /// Prohibited
    M_MIN_CACHE_SIZE_LOG_PROHIB = 0,
    /// N = 2
    M_MIN_CACHE_SIZE_LOG_2,
    /// N = 4
    M_MIN_CACHE_SIZE_LOG_4,
    /// N = 8
    M_MIN_CACHE_SIZE_LOG_8,
    /// N = 16
    M_MIN_CACHE_SIZE_LOG_16,
    /// N = 32
    M_MIN_CACHE_SIZE_LOG_32,
    /// N = 64
    M_MIN_CACHE_SIZE_LOG_64,
    /// N = 128
    M_MIN_CACHE_SIZE_LOG_128,
};

/// Criteria field format
///     7     6      5         4           3        2     1       0
/// +-----+------+------+-----------+-----------+------+------+------+
/// | RFU | RSSI Factor | Receive Window Factor | Min Cache Size Log |
/// +-----+------+------+-----------+-----------+------+------+------+
enum m_friend_criteria
{
    /// Minimum cache size log (@see enum m_min_cache_size_log)
    M_FRIEND_CRIT_MIN_CACHE_SIZE_LOG_LSB  = 0,
    M_FRIEND_CRIT_MIN_CACHE_SIZE_LOG_MASK = 0x07,

    /// Receive window factor (@see enum m_rx_window_factor)
    M_FRIEND_CRIT_RX_WINDOW_FACTOR_LSB    = 3,
    M_FRIEND_CRIT_RX_WINDOW_FACTOR_MASK   = 0x18,

    /// RSSI factor (@see enum m_rssi_factor))
    M_FRIEND_CRIT_RSSI_FACTOR_LSB         = 5,
    M_FRIEND_CRIT_RSSI_FACTOR_MASK        = 0x60,
};

/// FSN field format
///    7    6  5  4  3  2  1  0
/// +-----+--+--+--+--+--+--+---+
/// | FSN | Padding (0b0000000) |
/// +-----+--+--+--+--+--+--+---+
enum m_friend_fsn
{
    /// Friend Sequence Number
    M_FRIEND_FSN_FSN_POS      = 0,
    M_FRIEND_FSN_FSN_BIT      = 0x01,

    /// Padding (shall be 0b0000000)
    M_FRIEND_FSN_PADDING_LSB  = 1,
    M_FRIEND_FSN_PADDING_MASK = 0xFE,
};

/// MD values
enum m_friend_md
{
    /// Friend queue is empty
    M_FRIEND_MD_QUEUE_EMPTY      = 0,
    /// Friend queue is not empty
    M_FRIEND_MD_QUEUE_NOT_EMPTY,
};

/// Transport Control Message operation codes
enum m_lay_utrans_opcode
{
    /// Friend Poll
    M_LAY_UTRANS_OPCODE_FRIEND_POLL = 1,
    /// Friend Update
    M_LAY_UTRANS_OPCODE_FRIEND_UPDATE,
    /// Friend Request
    M_LAY_UTRANS_OPCODE_FRIEND_REQUEST,
    /// Friend Offer
    M_LAY_UTRANS_OPCODE_FRIEND_OFFER,
    /// Friend Clear
    M_LAY_UTRANS_OPCODE_FRIEND_CLEAR,
    /// Friend Clear Confirm
    M_LAY_UTRANS_OPCODE_FRIEND_CLEAR_CFM,
    /// Friend Subscription List Add
    M_LAY_UTRANS_OPCODE_FRIEND_SUBS_LIST_ADD,
    /// Friend Subscription List Remove
    M_LAY_UTRANS_OPCODE_FRIEND_SUBS_LIST_REM,
    /// Friend Subscription List Confirm
    M_LAY_UTRANS_OPCODE_FRIEND_SUBS_LIST_CFM,
    /// Heartbeat
    M_LAY_UTRANS_OPCODE_HEARTBEAT,
};

/// Retransmission parameters
///   7   6   5   4   3   2   1   0
/// +---+---+---+---+---+---+---+---+
/// |     RETX_STEP     |  RETX_NB  |
/// +---+---+---+---+---+---+---+---+
enum m_retx_params
{
    /// Bit 0-2: Number of retransmissions for each message
    M_RETX_NB_LSB    = 0,
    M_RETX_NB_MASK   = 0x07,
    /// Bit 3-7: Number of 50-ms steps between retransmissions
    M_RETX_STEP_LSB  = 3,
    M_RETX_STEP_MASK = 0xF8,
};


/// Control message operation code.
enum m_ctl_opcode
{
    // Block Ack Operation code
    M_OPCODE_BLOCK_ACK = 0,
};

/// Bit field of Time To live field
///   7   6   5   4   3   2   1   0
/// +---+---+---+---+---+---+---+---+
/// |CTL|            TTL            |
/// +---+---+---+---+---+---+---+---+
enum m_msg_ttl_format
{
    /// Time To Live
    M_MSG_TTL_LSB     = 0,
    M_MSG_TTL_MASK    = 0x7F,

    /// Control Message
    M_MSG_CTL_POS     = 7,
    M_MSG_CTL_BIT     = 0x80,
};

/// Bit field of Flags field
///   7   6   5   4   3   2       1             0
/// +---+---+---+---+---+---+-----------+-------------+
/// |                       | IV UPDATE | KEY REFRESH |
/// +---+---+---+---+---+---+-----------+-------------+
enum m_msg_flags
{
    /// Key refresh flag
    M_MSG_FLAGS_KEY_REFRESH_POS = 0,
    M_MSG_FLAGS_KEY_REFRESH_BIT = 0x01,

    /// IV update flag
    M_MSG_FLAGS_IV_UPDATE_POS   = 1,
    M_MSG_FLAGS_IV_UPDATE_BIT   = 0x02,
};


/// Bit field of Access Message segment header
///   7   6   5   4   3   2   1   0
/// +---+---+---+---+---+---+---+---+
/// |SEG|AFK|          AID          |
/// +---+---+---+---+---+---+---+---+
/// or packet Control Message segment header
/// +---+---+---+---+---+---+---+---+
/// |SEG|          OPCODE           |
/// +---+---+---+---+---+---+---+---+
enum m_seg_head_format
{
    /// Application Key Identifier - Access Message
    M_MSG_AID_LSB     = 0,
    M_MSG_AID_MASK    = 0x3F,
    /// Application Key Flag
    M_MSG_AFK_POS     = 6,
    M_MSG_AFK_BIT     = 0x40,

    /// Control Message Operation Code
    M_MSG_OPCODE_LSB  = 0,
    M_MSG_OPCODE_MASK = 0x7F,

    /// Segmented Message
    M_MSG_SEG_POS     = 7,
    M_MSG_SEG_BIT     = 0x80,
};

/// Heartbeat Publication Feature bitfield
enum m_hbeat_pub_feat
{
    /// Relay feature change triggers a Heartbeat message
    M_HBEAT_PUB_FEAT_RELAY_POS      = 0,
    M_HBEAT_PUB_FEAT_RELAY_BIT      = 0x01,

    /// GATT proxy feature change triggers a Heartbeat message
    M_HBEAT_PUB_FEAT_GATT_PROXY_POS = 1,
    M_HBEAT_PUB_FEAT_GATT_PROXY_BIT = 0x02,

    /// Friend feature change triggers a Heartbeat message
    M_HBEAT_PUB_FEAT_FRIEND_POS     = 2,
    M_HBEAT_PUB_FEAT_FRIEND_BIT     = 0x04,

    /// Low Power Node feature change triggers a Heartbeat message
    M_HBEAT_PUB_FEAT_LPN_POS        = 3,
    M_HBEAT_PUB_FEAT_LPN_BIT        = 0x08,
};

/// Bit field of Lower Transport Segmented header (MSB Format)
///
/// +-------------------------------+-------------------------------+-------------------------------+-------------------------------+
/// |           OCTET 0             |           OCTET 1             |           OCTET 2             |           OCTET 3             |
/// +-------------------------------+-------------------------------+-------------------------------+-------------------------------+
/// | 7   6   5   4   3   2   1   0 | 7   6   5   4   3   2   1   0 | 7   6   5   4   3   2   1   0 | 7   6   5   4   3   2   1   0 |
/// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
/// |SEG|   OP_CODE/AID_AFK         |SZM|                      SEQ_ZERO                     |       SEG_O       |       SEG_N       |
/// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
/// or (block ack)
/// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
/// | 0 |         0x00              |OBO|                      SEQ_ZERO                     |  RFU  |         BLOCK_ACK[31:24]      |
/// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
///  31  30  29  28  27  26  25  24  23  22  21  20  19  18  17  16  15  14  13  12  11  10   9   8   7   6   5   4   3   2   1   0
enum m_seg_header_format
{
    /// Segmented Message
    M_HEAD_SEG_POS          = 31,
    M_HEAD_SEG_BIT          = (int)0x80000000,
    /// Application Key Flag
    M_HEAD_AFK_POS          = 30,
    M_HEAD_AFK_BIT          = 0x40000000,
    /// Application Key Identifier - Access Message
    M_HEAD_AID_LSB          = 24,
    M_HEAD_AID_MASK         = 0x3F000000,
    /// Control Message Operation Code
    M_HEAD_OPCODE_LSB       = 24,
    M_HEAD_OPCODE_MASK      = 0x7F000000,
    /// Size of transport MIC
    M_HEAD_SZMIC_POS        = 23,
    M_HEAD_SZMIC_BIT        = 0x00800000,
    /// Friend On Behalf of Low Power node
    M_HEAD_OBO_POS          = 23,
    M_HEAD_OBO_BIT          = 0x00800000,
    /// SEQ Zero of Upper Transport PDU
    M_HEAD_SEQ_ZERO_LSB     = 10,
    M_HEAD_SEQ_ZERO_MASK    = 0x007FFC00,
    /// Segment Offset
    M_HEAD_SEG_O_LSB        = 5,
    M_HEAD_SEG_O_MASK       = 0x000003E0,
    /// Last segment number
    M_HEAD_SEG_N_LSB        = 0,
    M_HEAD_SEG_N_MASK       = 0x0000001F,
};

/// @} M_DEFINES

#endif /* M_DEFINES_ */
