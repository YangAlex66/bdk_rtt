#ifndef _BLE_API_H_
#define _BLE_API_H_

#include "sys_config.h"

#if CFG_BLE_VERSION == BLE_VERSION_5_x
#include "ble_api_5_x.h"
#else
#define MAX_ADV_DATA_LEN           (0x1F)
#define MAX_SCAN_NUM               (15)

/// BD address length
#define GAP_BD_ADDR_LEN       (6)
/// Maximal length of the Device Name value
#define APP_DEVICE_NAME_MAX_LEN      (18)

//TODO - rename KEY_LEN in future release since it's
//already defined in common_bt.h
#ifdef KEY_LEN
	#if KEY_LEN != 16
	#error "KEY_LEN is redefined with a different value!!!"
	#endif
#else 
	#define KEY_LEN 16
#endif

#define ABIT(n) (1 << n)

#define BK_PERM_SET(access, right) \
    (((BK_PERM_RIGHT_ ## right) << (access ## _POS)) & (access ## _MASK))

#define BK_PERM_GET(perm, access)\
    (((perm) & (access ## _MASK)) >> (access ## _POS))

typedef enum
{
    /// Stop notification/indication
    PRF_STOP_NTFIND = 0x0000,
    /// Start notification
    PRF_START_NTF,
    /// Start indication
    PRF_START_IND,
} prf_conf;


/**
 *   15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 * |EXT | WS | I  | N  | WR | WC | RD | B  |    NP   |    IP   |   WP    |    RP   |
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 *
 * Bit [0-1]  : Read Permission         (0 = NO_AUTH, 1 = UNAUTH, 2 = AUTH, 3 = SEC_CON)
 * Bit [2-3]  : Write Permission        (0 = NO_AUTH, 1 = UNAUTH, 2 = AUTH, 3 = SEC_CON)
 * Bit [4-5]  : Indication Permission   (0 = NO_AUTH, 1 = UNAUTH, 2 = AUTH, 3 = SEC_CON)
 * Bit [6-7]  : Notification Permission (0 = NO_AUTH, 1 = UNAUTH, 2 = AUTH, 3 = SEC_CON)
 *
 * Bit [8]    : Broadcast permission
 * Bit [9]    : Read Command accepted
 * Bit [10]   : Write Command accepted
 * Bit [11]   : Write Request accepted
 * Bit [12]   : Send Notification
 * Bit [13]   : Send Indication
 * Bit [14]   : Write Signed accepted
 * Bit [15]   : Extended properties present
 */

typedef enum
{
    /// Read Permission Mask
    RP_MASK             = 0x0003,
    RP_POS              = 0,
    /// Write Permission Mask
    WP_MASK             = 0x000C,
    WP_POS              = 2,
    /// Indication Access Mask
    IP_MASK            = 0x0030,
    IP_POS             = 4,
    /// Notification Access Mask
    NP_MASK            = 0x00C0,
    NP_POS             = 6,
    /// Broadcast descriptor present
    BROADCAST_MASK     = 0x0100,
    BROADCAST_POS      = 8,
    /// Read Access Mask
    RD_MASK            = 0x0200,
    RD_POS             = 9,
    /// Write Command Enabled attribute Mask
    WRITE_COMMAND_MASK = 0x0400,
    WRITE_COMMAND_POS  = 10,
    /// Write Request Enabled attribute Mask
    WRITE_REQ_MASK     = 0x0800,
    WRITE_REQ_POS      = 11,
    /// Notification Access Mask
    NTF_MASK           = 0x1000,
    NTF_POS            = 12,
    /// Indication Access Mask
    IND_MASK           = 0x2000,
    IND_POS            = 13,
    /// Write Signed Enabled attribute Mask
    WRITE_SIGNED_MASK  = 0x4000,
    WRITE_SIGNED_POS   = 14,
    /// Extended properties descriptor present
    EXT_MASK           = 0x8000,
    EXT_POS            = 15,
} bk_perm_mask;

/**
 * Attribute Extended permissions
 *
 * Extended Value permission bit field
 *
 *   15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 * | RI |UUID_LEN |EKS |                       Reserved                            |
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 *
 * Bit [0-11] : Reserved
 * Bit [12]   : Encryption key Size must be 16 bytes
 * Bit [13-14]: UUID Length             (0 = 16 bits, 1 = 32 bits, 2 = 128 bits, 3 = RFU)
 * Bit [15]   : Trigger Read Indication (0 = Value present in Database, 1 = Value not present in Database)
 */
typedef enum
{
    /// Check Encryption key size Mask
    EKS_MASK         = 0x1000,
    EKS_POS          = 12,
    /// UUID Length
    UUID_LEN_MASK    = 0x6000,
    UUID_LEN_POS     = 13,
    /// Read trigger Indication
    RI_MASK          = 0x8000,
    RI_POS           = 15,
}bk_ext_perm_mask;

/**
 * Service permissions
 *
 *    7    6    5    4    3    2    1    0
 * +----+----+----+----+----+----+----+----+
 * |SEC |UUID_LEN |DIS |  AUTH   |EKS | MI |
 * +----+----+----+----+----+----+----+----+
 *
 * Bit [0]  : Task that manage service is multi-instantiated (Connection index is conveyed)
 * Bit [1]  : Encryption key Size must be 16 bytes
 * Bit [2-3]: Service Permission      (0 = NO_AUTH, 1 = UNAUTH, 2 = AUTH, 3 = Secure Connect)
 * Bit [4]  : Disable the service
 * Bit [5-6]: UUID Length             (0 = 16 bits, 1 = 32 bits, 2 = 128 bits, 3 = RFU)
 * Bit [7]  : Secondary Service       (0 = Primary Service, 1 = Secondary Service)
 */
typedef enum
{
    /// Task that manage service is multi-instantiated
    SVC_MI_MASK        = 0x01,
    SVC_MI_POS         = 0,
    /// Check Encryption key size for service Access
    SVC_EKS_MASK       = 0x02,
    SVC_EKS_POS        = 1,
    /// Service Permission authentication
    SVC_AUTH_MASK      = 0x0C,
    SVC_AUTH_POS       = 2,
    /// Disable the service
    SVC_DIS_MASK       = 0x10,
    SVC_DIS_POS        = 4,
    /// Service UUID Length
    SVC_UUID_LEN_MASK  = 0x60,
    SVC_UUID_LEN_POS   = 5,
    /// Service type Secondary
    SVC_SECONDARY_MASK = 0x80,
    SVC_SECONDARY_POS  = 7,
}bk_svc_perm_mask;

/// Attribute & Service access mode
enum
{
    /// Disable access
    BK_PERM_RIGHT_DISABLE   = 0,
    /// Enable access
    BK_PERM_RIGHT_ENABLE   = 1,
};

/// Attribute & Service access rights
enum
{
    /// No Authentication
    BK_PERM_RIGHT_NO_AUTH  = 0,
    /// Access Requires Unauthenticated link
    BK_PERM_RIGHT_UNAUTH   = 1,
    /// Access Requires Authenticated link
    BK_PERM_RIGHT_AUTH     = 2,
    /// Access Requires Secure Connection link
    BK_PERM_RIGHT_SEC_CON  = 3,
};

/// Attribute & Service UUID Length
enum
{
    /// 16  bits UUID
    BK_PERM_RIGHT_UUID_16         = 0,
    /// 32  bits UUID
    BK_PERM_RIGHT_UUID_32         = 1,
    /// 128 bits UUID
    BK_PERM_RIGHT_UUID_128        = 2,
    /// Invalid
    BK_PERM_RIGHT_UUID_RFU        = 3,
};


typedef enum
{
    ERR_SUCCESS = 0,
    ERR_STACK_FAIL,
    ERR_MEM_FAIL,
    ERR_INVALID_ADV_DATA,
    ERR_ADV_FAIL,
    ERR_STOP_ADV_FAIL,
    ERR_GATT_INDICATE_FAIL,
    ERR_GATT_NOTIFY_FAIL,
    ERR_SCAN_FAIL,
    ERR_STOP_SCAN_FAIL,
    ERR_CONN_FAIL,
    ERR_STOP_CONN_FAIL,
    ERR_DISCONN_FAIL,
    ERR_READ_FAIL,
    ERR_WRITE_FAIL,
    ERR_REQ_RF,
    ERR_PROFILE,
    ERR_CREATE_DB,
    ERR_CMD_NOT_SUPPORT,
    /* Add more BLE error code hereafter */
} ble_err_t;

typedef enum
{
    ADV_NAME_SHORT = 0x8,
    ADV_NAME_FULL
} adv_name_type_t;

typedef enum
{
    AD_LIMITED  = ABIT(0), /* Limited Discoverable */
    AD_GENERAL  = ABIT(1), /* General Discoverable */
    AD_NO_BREDR = ABIT(2)  /* BR/EDR not supported */
} adv_flag_t;

typedef struct
{
    uint8_t  advData[MAX_ADV_DATA_LEN];
    uint8_t  advDataLen;
    uint8_t  respData[MAX_ADV_DATA_LEN];
    uint8_t  respDataLen;
    uint8_t  channel_map;
    uint16_t interval_min;
    uint16_t interval_max;
    /* Subject to add more hereafter in the future */
} adv_info_t;

typedef struct
{
    uint8_t  filter_en;
    uint8_t  channel_map;
    uint16_t interval;
    uint16_t window;
} scan_info_t;

typedef enum
{
    BLE_STACK_OK,
    BLE_STACK_FAIL,
    BLE_CONNECT,
    BLE_DISCONNECT,
    BLE_MTU_CHANGE,
    BLE_CFG_NOTIFY,
    BLE_CFG_INDICATE,
    BLE_TX_DONE,
    BLE_GEN_DH_KEY,
    BLE_GET_KEY,
    BLE_ATT_INFO_REQ,
    BLE_CREATE_DB_OK,
    BLE_CREATE_DB_FAIL,
} ble_event_t;

struct ble_disconnect_ind
{
    /// Connection handle
    uint16_t conhdl;
    /// Reason of disconnection
    uint8_t reason;
};

typedef enum
{
    BLE_MESH_CREATE_DB_OK,
    BLE_MESH_CREATE_DB_FAIL,
    BLE_MESH_INIT_DONE,
    BLE_MESH_APP_KEY_ADD_DONE,
    BLE_MESH_PROV_INVITE,
    BLE_MESH_PROV_START,
    BLE_MESH_PROV_DONE,
} ble_mesh_event_t;

typedef struct
{
    uint16_t prf_id;
    uint16_t att_idx;
    uint16_t length;
    uint8_t status;
} att_info_req_t;

typedef struct
{
    uint16_t prf_id;
    uint16_t att_idx;
    uint8_t *value;
    uint16_t len;
} write_req_t;

typedef struct
{
    uint16_t prf_id;
    uint16_t att_idx;
    uint8_t *value;
    uint16_t size;
} read_req_t;

typedef struct
{
    uint8_t evt_type;
    uint8_t adv_addr_type;
    uint8_t adv_addr[6];
    uint8_t data_len;
    uint8_t *data;
    uint8_t rssi;
} recv_adv_t;

typedef struct
{
	uint8_t addr_type;
	uint8_t rssi;
	uint8_t adv_type;
	uint8_t adv_addr[6];
}device_info_t;

typedef struct
{
	uint8_t scan_count;
	device_info_t info[MAX_SCAN_NUM];
}ble_scan_list_t;

struct ble_get_key_ind
{
    /// X coordinate
    uint8_t *pub_key_x;
    uint8_t pub_x_len;
    /// Y coordinate
    uint8_t *pub_key_y;
    uint8_t pub_y_len;
};

struct ble_gen_dh_key_ind
{
    /// Result (32 bytes)
    uint8_t *result;
    uint8_t len;
};

typedef struct
{
    /// 16 bits UUID LSB First
    uint8_t uuid[16];
    /// Attribute Permissions (@see enum attm_perm_mask)
    uint16_t perm;
    /// Attribute Extended Permissions (@see enum attm_value_perm_mask)
    uint16_t ext_perm;
    /// Attribute Max Size
    /// note: for characteristic declaration contains handle offset
    /// note: for included service, contains target service handle
    uint16_t max_size;
}bk_attm_desc_t;

struct bk_ble_db_cfg
{
    uint16_t prf_task_id;
    ///Service uuid
    uint8_t uuid[16];
    ///Number of db
	uint8_t att_db_nb;
    ///Start handler, 0 means autoalloc
    uint16_t start_hdl;
    ///Attribute database
    bk_attm_desc_t* att_db;
    ///Service config
    uint8_t svc_perm;
};

/// Mesh Provisioning state change indication
struct bk_prov_start_ind
{
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
};

typedef void (*ble_event_cb_t)(ble_event_t event, void *param);
typedef void (*ble_mesh_event_cb_t)(ble_mesh_event_t event, void *param);
typedef void (*ble_recv_adv_cb_t)(recv_adv_t *recv_adv);
typedef uint8_t (*bk_ble_read_cb_t)(read_req_t *read_req);
typedef void (*bk_ble_write_cb_t)(write_req_t *write_req);

extern ble_event_cb_t ble_event_cb;
extern ble_mesh_event_cb_t ble_mesh_event_cb;
extern ble_recv_adv_cb_t ble_recv_adv_cb;
extern bk_ble_read_cb_t bk_ble_read_cb;
extern bk_ble_write_cb_t bk_ble_write_cb;
extern adv_info_t adv_info;
extern scan_info_t scan_info;

ble_err_t bk_ble_create_db (struct bk_ble_db_cfg* ble_db_cfg);
#if CFG_BLE_VERSION == BLE_VERSION_4_2
extern ble_err_t appm_start_advertising(void);
#endif
ble_err_t bk_ble_send_ntf_value(uint32_t len, uint8_t *buf, uint16_t prf_id, uint16_t att_idx);
ble_err_t bk_ble_send_ind_value(uint32_t len, uint8_t *buf, uint16_t prf_id, uint16_t att_idx);
#endif
#endif
