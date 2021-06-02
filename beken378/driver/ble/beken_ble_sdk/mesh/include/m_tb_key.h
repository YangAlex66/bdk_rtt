/**
 ****************************************************************************************
 *
 * @file m_tb_key.h
 *
 * @brief Mesh Key Manager Toolbox
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef M_TB_KEY_
#define M_TB_KEY_

/**
 ****************************************************************************************
 * @defgroup M_TB_KEY Mesh Security Manager Toolbox
 * @ingroup MESH
 * @brief  Mesh Security Manager Toolbox
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "m_defines.h"   // Mesh Profile Definitions
#include "m_config.h"    // Mesh Configuration

/*
 * DEFINES
 ****************************************************************************************
 */

/// Bit field of KEY LID
///   7   6   5   4   3   2   1   0
/// +---+---+---+---+---+---+---+---+
/// | N |            LID            |
/// +---+---+---+---+---+---+---+---+
///
#define M_TB_KEY_NEW_POS    ((sizeof(m_lid_t)<<3)-1)
#define M_TB_KEY_NEW_BIT    (1<<(M_TB_KEY_NEW_POS))
#define M_TB_KEY_LID_LSB    (0)
#define M_TB_KEY_LID_MASK   (M_TB_KEY_NEW_BIT-1)

/// Invalid Local Key Identifier
#define M_TB_KEY_INVALID_LID        (0xFF)
/// Invalid Network Key Identifier
#define M_TB_KEY_INVALID_NID        (0xFF)
/// Invalid Application Key Identifier
#define M_TB_KEY_INVALID_AID        (0xFF)
/// Device Key local identifier equals to zero
#if (BLE_MESH_PROVISIONER == 1)
/// Provisioner's Device Key local identifier equals to zero
#define M_TB_KEY_DEVICE_LID         (0x00)

#define M_TB_KEY_DEV_INVALID_LID (0xFF)
#define M_TB_KEY_DEV_SELF_LID (0xFFFE)

/// Device Key local identifier equals to zero
#define M_TB_KEY_DEVICE_START_LID         (0x00)
/// Device Key local identifier equals to zero
#define M_TB_KEY_DEVICE_END_LID         (M_TB_KEY_MAX_NB_DEV - 1)
#else
/// Device Key local identifier equals to zero
#define M_TB_KEY_DEVICE_LID         (0x00)
#endif

/*
 * MACROS
 ****************************************************************************************
 */

#if (BLE_MESH_PROVISIONER == 1)
/// Get local index of a subnet from local index of a network key
#define M_TB_KEY_GET_SUBNET_LID(net_key_lid)                    \
    (net_key_lid - M_TB_KEY_MAX_NB_DEV)

/// Get local index of a network key from local index of a subnet
#define M_TB_KEY_GET_NET_KEY_LID(subnet_lid)                    \
    (subnet_lid + M_TB_KEY_MAX_NB_DEV)

#else
/// Get local index of a subnet from local index of a network key
#define M_TB_KEY_GET_SUBNET_LID(net_key_lid)                    \
    (net_key_lid - (M_TB_KEY_DEVICE_LID + 1))

/// Get local index of a network key from local index of a subnet
#define M_TB_KEY_GET_NET_KEY_LID(subnet_lid)                    \
    (subnet_lid + M_TB_KEY_DEVICE_LID + 1)
#endif

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Type of supported keys
enum m_tb_key_type
{
    /// Device Key
    M_TB_KEY_DEVICE,
    /// Network Key
    M_TB_KEY_NETWORK,
    /// Application Key
    M_TB_KEY_APPLICATION,
};


/// Key state
enum m_tb_key_state
{
    /// Current key is in ready state (Phase 0)
    M_TB_KEY_STATE_NORMAL = 0,
    /// A key refresh procedure is on-going, current key is old key (Phase 1)
    M_TB_KEY_STATE_P1,
    /// A key refresh procedure is on-going, current key is new key (Phase 2)
    M_TB_KEY_STATE_P2,

    // INTERNAL STATES - USED ONLY IN KEY MANAGER
    // -------------- Key Generation state, key cannot have this state into key table --------------

    /// Generate encryption key
    M_TB_KEY_STATE_NET_GEN_ENC,
#if(BLE_MESH_GATT_PROXY)
    /// Generate identity key
    M_TB_KEY_STATE_NET_GEN_ID,
#endif // (BLE_MESH_GATT_PROXY)
    /// Generate beacon key
    M_TB_KEY_STATE_NET_GEN_BEACON,
    /// Generate Network ID
    M_TB_KEY_STATE_NET_GEN_N_ID,

    /// Generate application identifier
    M_TB_KEY_STATE_APP_GEN_AID,
};

/// Network key type
enum m_tb_key_net_type
{
    /// Privacy key
    M_TB_KEY_TYPE_PRIV      = 0,
    /// Encryption key
    M_TB_KEY_TYPE_ENC,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

#if (BLE_MESH_LPN || BLE_MESH_FRIEND)
/// Definition of callback function called once Friend security credentials have been generated
typedef void (*m_tb_key_friendship_cb_finished)(uint8_t idx, uint16_t status);
#endif //(BLE_MESH_LPN || BLE_MESH_FRIEND)

/// minimum structure information available on all keys
typedef struct m_tb_key
{
    /// Type of the key (@see enum m_tb_key_type)
    uint8_t        key_type;
    /// Key state (@see enum m_tb_key_state)
    uint8_t        state;
    /// key value
    uint8_t        key[MESH_KEY_LEN];
} m_tb_key_t;

/// Definition of a Device key
typedef struct m_tb_key_dev
{
    /// Type of the key (@see enum m_tb_key_type)
    ///  - M_TB_KEY_DEVICE
    uint8_t key_type;
    /// Key state (@see enum m_tb_key_state)
    uint8_t state;
    /// Device key value
    uint8_t key[MESH_KEY_LEN];
    /// Number of models instances currently bound with this application key
    uint8_t nb_models;
} m_tb_key_dev_t;

/// Definition of a Network key
typedef struct m_tb_key_net
{
    /// Type of the key (@see enum m_tb_key_type)
    ///  - M_TB_KEY_NETWORK
    uint8_t  key_type;
    /// Key state (@see enum m_tb_key_state)
    uint8_t  state;
    /// Network key value
    uint8_t  key[MESH_KEY_LEN];
    /// Network Encryption key value
    uint8_t  enc_key[MESH_KEY_LEN];
    /// Network privacy key value
    uint8_t  priv_key[MESH_KEY_LEN];
    /// Network beacon key value
    uint8_t  beacon_key[MESH_KEY_LEN];
    /// NID (Network Identifier)
    uint8_t  nid;
    /// Flags
    uint8_t  flags;
#if (BLE_MESH_GATT_PROXY)
    /// Node identity
    uint8_t  node_identity;
    /// Identity key value
    uint8_t  identity[MESH_KEY_LEN];
#endif //(BLE_MESH_GATT_PROXY)
    /// Network key index in network database (unique and manage by configuration)
    uint16_t net_key_id;
    /// Network ID
    uint8_t  net_id[M_NET_ID_LEN];
    /// Number of application keys currently bound with this network key
    uint8_t  nb_app_keys;
} m_tb_key_net_t;

/// Definition of an Application key
typedef struct m_tb_key_app
{
    /// Type of the key (@see enum m_tb_key_type)
    ///  - M_TB_KEY_APPLICATION
    uint8_t  key_type;
    /// Key state (@see enum m_tb_key_state)
    uint8_t  state;
    /// Application key value
    uint8_t  key[MESH_KEY_LEN];
    /// Number of models instances currently bound with this application key
    uint8_t  nb_models;
    /// Application identifier
    uint8_t  aid;
    /// Application key identifier
    uint16_t app_key_id;
    /// Local index of Network key this application key is bound with
    m_lid_t  net_key_lid;
} m_tb_key_app_t;

/// Element of network key mapping table used to retrieve a network key from its network identifier.
/// Network identifier are sorted in ascending order in order to speed-up search.
typedef struct m_tb_key_map_net
{
    /// Network identifier (NID) which is not unique
    uint8_t  nid;
    /// Network Key Local identifier (unique)
    m_lid_t  net_key_lid;
} m_tb_key_map_net_t;

/// Element of bound key mapping between a Network key and an application key
///
/// Network keys are sorted in ascending order and for each network key, corresponding Application key identifier are
/// sorted in ascending order. This would speed-up key search.
typedef struct m_tb_key_bound_net_app
{
    /// Network Key Local identifier (not unique in this table)
    m_lid_t  net_key_lid;
    /// Application key Identifier (not unique)
    uint8_t  aid;
    /// Application Key Local identifier (unique)
    m_lid_t  app_key_lid;
} m_tb_key_bound_net_app_t;

#if(BLE_MESH_PROVISIONER == 1)
typedef struct m_tb_dev_info
{
    uint16_t                   nwk_key_lid;
    uint16_t                   nwk_key_id;
    /*only have one app key for a device*/
    uint16_t                   app_key_lid;
    uint16_t                   app_key_id;

    uint16_t                   nwk_addr;
    uint8_t                    nb_elt;
} m_tb_key_dev_info_t;
#endif

/**
 ****************************************************************************************
 * @brief Callback executed when a key is added to key manager (with all subkey generated)
 *
 * @param[in] status  Key Generation status (@see enum m_error)
 * @param[in] key_lid Added Key local identifier
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
typedef void (*m_tb_key_added_cb)(uint16_t status, m_lid_t key_lid);

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/* ************************************************************************************* *
 *                                  KEY CONFIGURATION                                    *
 * ************************************************************************************* */

/**
 ****************************************************************************************
 * @brief Add device key into the key manager
 *
 * @param[in] p_dev_key Pointer to the 128bits key
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_tb_key_dev_add(const uint8_t *p_dev_key, uint8_t dev_lid);

#if (BLE_MESH_DBG)
/**
 ****************************************************************************************
 * @brief Delete device key from key manager
 ****************************************************************************************
 */
void m_tb_key_dev_delete(m_lid_t dev_lid);
#endif //(BLE_MESH_DBG)

/**
 ****************************************************************************************
 * @brief Add a network key into the key manager
 *
 * This function will generate all network sub-keys derived from network key and save
 * them into the key manager.
 *
 * @param[in] net_key_id          Network key index
 * @param[in] p_net_key           Pointer to the 128bits key
 * @param[in] flags               Flags value
 * @param[in] res_cb              Function called when all sub-keys has been generated
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_tb_key_net_add(uint16_t net_key_id, const uint8_t *p_net_key, uint8_t flags, m_tb_key_added_cb res_cb);

/**
 ****************************************************************************************
 * @brief Delete a network key from the key manager
 *
 * If the key in under an updating state, the key to update is also deleted.
 * All application keys binded to the network key are deleted
 * (and so the bounds between application key and model identifier)
 *
 * @param[in] net_key_lid   Network key local identifier
 * @param[in] force         Indicate if key must be deleted even if only one key is remaining
 *                          (for debug purpose)
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_tb_key_net_delete(m_lid_t net_key_lid, bool force);

/**
 ****************************************************************************************
 * @brief Add an application key into the key manager
 *
 * This function will generate application sub-key derived from application key and save
 * them into the key manager.
 *
 * @param[in] app_key_id  Application key index to add
 * @param[in] p_app_key   Pointer to the 128bits key
 * @param[in] net_key_lid Network key local index bind to the new application key
 * @param[in] res_cb      Function called when all sub-keys has been generated
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_tb_key_app_add(uint16_t app_key_id, const uint8_t *p_app_key, m_lid_t net_key_lid,
                          m_tb_key_added_cb res_cb);

/**
 ****************************************************************************************
 * @brief Delete an application key.
 * Existing bindings between application key and models are also removed.
 *
 * @param[in] app_key_id    AppKey Index
 * @param[in] net_key_lid   Network key local identifier
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_tb_key_app_delete(m_lid_t app_key_lid, m_lid_t net_key_lid);

/**
 ****************************************************************************************
 * @brief Create a bind between a specific model and an application key
 *
 * @param[in] app_key_lid Application key local identifier
 * @param[in] model_lid   Model local identifier
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_tb_key_model_bind(m_lid_t app_key_lid, m_lid_t model_lid);

/**
 ****************************************************************************************
 * @brief Remove a bind between a specific model and an application key
 *
 * @param[in] app_key_lid Application key local identifier
 * @param[in] model_lid   Model local identifier
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_tb_key_model_unbind(m_lid_t app_key_lid, m_lid_t model_lid);


/* ************************************************************************************* *
 *                                     KEY UPDATE                                        *
 * ************************************************************************************* */

/**
 ****************************************************************************************
 * @brief As part of Key update procedure, Update the network key with a new
 * network key value.
 *
 * First this new key will be used for packet reception till the key state is transitioned
 * using m_tb_key_net_use_new function.
 * *
 * During the transition, a second key will be allocated to support reception of packet
 * using two different keys
 *
 * It is not possible to update a key already in update state.
 *
 * @param[in] net_key_lid Network key local index
 * @param[in] p_net_key   Pointer to the 128bits key
 * @param[in] res_cb      Function called when all sub-keys has been generated
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_tb_key_net_update(m_lid_t net_key_id, const uint8_t *p_net_key, m_tb_key_added_cb res_cb);

/**
 ****************************************************************************************
 * @brief Update the network key transition state to use it.
 *
 * Calling this function to use new key for network transmission
 * (also the new application keys)
 *
 * @param[in] net_key_lid Network key local index
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_tb_key_net_use_new(m_lid_t net_key_lid);

/**
 ****************************************************************************************
 * @brief Update the network key to revoke the old key
 *
 * Calling this function to remove old network key
 * (and corresponding old application key that has been refreshed)
 *
 * @param[in] net_key_lid Network key local index
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_tb_key_net_revoke_old(m_lid_t net_key_lid);

/**
 ****************************************************************************************
 * @brief As part of Key update procedure, Update the application key with a new
 * key value.
 *
 * Updating the key does not mean that it will be used for packet transmission immediately
 * The key state must be transitioned using  m_tb_key_net_transition function.
 *
 * During the transition, a second key will be allocated to support reception of packet
 * using two different keys
 *
 * If the key is in incorrect transition state, update is rejected.
 *
 * If the function call succeed, the new network key is used only for reception.
 *
 * @param[in] app_key_id  AppKey Index
 * @param[in] p_app_key   Pointer to the 128bits key
 * @param[in] res_cb      Function called when all sub-keys has been generated
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_tb_key_app_update(uint16_t app_key_id, const uint8_t *p_app_key, m_lid_t net_key_lid,
                             m_tb_key_added_cb res_cb);

/* ************************************************************************************* *
 *                                     KEY SEARCH                                        *
 * ************************************************************************************* */

/**
 ****************************************************************************************
 * @brief Retrieve device key information
 *
 * @param[out] pp_dev_key Device key parameters
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_tb_key_dev_get(m_lid_t dev_key_lid, const m_tb_key_dev_t** pp_dev_key);

m_lid_t m_tb_key_dev_get_idx_from_addr(uint16_t addr);

uint16_t m_tb_gen_prim_addr(void);

/**
 ****************************************************************************************
 * @brief Retrieve network key information
 *
 * If the key is under transition state,
 *      - MSB = 1: new key information provided
 *      - MSB = 0: old key information provided
 *
 * @param[in]  net_key_lid  Network key local identifier
 * @param[out] pp_net_key   Network key parameters
 * @param[in]  tx           Indicate if key for TX (true) or for RX (false) must be provided.
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_tb_key_net_get(m_lid_t net_key_lid, const m_tb_key_net_t **pp_net_key, bool tx);

/**
 ****************************************************************************************
 * @brief Find a network key from network key index
 *
 * @param[in]  net_key_id     Network key index
 * @param[out] p_net_key_lid  Network key local identifier
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_tb_key_net_find(uint16_t net_key_id, m_lid_t *p_net_key_lid);

/**
 ****************************************************************************************
 * @brief Get Next network key available
 *
 * @param[in|out]  p_net_key_lid  Pointer to next valid network key local index
 * @param[out]     pp_net_key     Pointer to the network key data
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_tb_key_net_next(m_lid_t *p_net_key_lid, const m_tb_key_net_t **pp_net_key);

/**
 ****************************************************************************************
 * @brief Retrieve application key information
 *
 * MSB bit of local identifier is used to select key
 *      - MSB = 1: new key information provided
 *      - MSB = 0: old key information provided
 *
 * @param[in]  app_key_lid  Application key local identifier
 * @param[out] pp_app_key   Application key parameters
 * @param[in]  tx           Indicate if key for TX (true) or for RX (false) must be provided.
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_tb_key_app_get(m_lid_t app_key_lid, const m_tb_key_app_t **pp_app_key, bool tx);

/**
 ****************************************************************************************
 * @brief Find an application key from application key index
 *
 * @param[in]  app_key_id      Application key index
 * @param[out] p_app_key_lid   Pointer used to return application key local identifier
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t m_tb_key_app_find(uint16_t app_key_id, m_lid_t *p_app_key_lid);

/**
 ****************************************************************************************
 * @brief Find a network key identifier from a NID
 *
 * @param[in]  nid     Network identifier (NID)
 * @param[in]  offset  Offset in the table which have the proposed NID
 *
 * @return Network key local identifier if found else MESH_INVALID_LID.
 ****************************************************************************************
 */
m_lid_t m_tb_key_net_lid_from_nid(uint8_t nid, uint8_t offset);

/**
 ****************************************************************************************
 * @brief Find an application key local identifier from an AID and a network key locak index.
 *
 * @param[in] net_lid   Network key local index
 * @param[in] aid       Application identifier (AID)
 * @param[in] offset    Offset in the table
 *
 * @return Network key local identifier if found else MESH_INVALID_LID.
 ****************************************************************************************
 */
m_lid_t m_tb_key_app_lid_from_net_aid(m_lid_t net_lid, uint8_t aid, uint8_t offset);

/**
 ****************************************************************************************
 * @brief Set IV for a given network key.
 *
 * @param[in]  net_lid     Network key local identifier.
 * @param[in]  iv          New IV value.
 * @param[in]  seq         New Sequence number value.
 *
 * @return Error status.
 ****************************************************************************************
 */
void m_tb_key_set_iv_seq(uint32_t iv, uint32_t seq);

/**
 ****************************************************************************************
 * @brief Get new sequence number for a packet to transmit
 *
 * @note The key sequence number is then automatically incremented
 *
 * @param[in]  net_lid     Network key local identifier.
 * @param[out] p_seq       Sequence number to use for packet transmission
 * @param[out] p_iv        IV value
 *
 * @return Error status.
 ****************************************************************************************
 */
void m_tb_key_get_iv_seq(uint32_t *p_seq, uint32_t *p_iv);


void m_tb_key_get_cur_iv_seq(uint32_t *p_seq, uint32_t *p_iv);


/**
 ****************************************************************************************
 * @brief Get IV to be used for received packet decryption based on received IVI value.
 *
 * @param[in] ivi       IVI value received as part of network pdu header
 *
 * @return IV value to use
 ****************************************************************************************
 */
uint32_t m_tb_key_get_iv_rx(uint8_t ivi);

/**
 ****************************************************************************************
 * @brief Check if a binding exist between application and model
 *
 * @param[in] app_key_lid Application Key Local Identifier
 * @param[in] model_lid   Model local identifier
 *
 * @return Error status.
 ****************************************************************************************
 */
uint16_t m_tb_key_model_bind_check(m_lid_t app_key_lid, m_lid_t model_lid);

m_lid_t  m_tb_model_bind_get_app_key_lid(m_lid_t model_lid);

/**
 ****************************************************************************************
 * @brief Get number of known network keys.
 *
 * @return Number of network keys.
 ****************************************************************************************
 */
uint8_t m_tb_key_get_nb_net_keys(void);

/**
 ****************************************************************************************
 * @brief Get list of known NetKey indexes.
 *
 * @param[in] p_netkey_ids   Pointer to the buffer in which found NetKey indexes have to be
 * copied. Found values are packed as described in Mesh Profile specification, section 4.3.1.1.
 ****************************************************************************************
 */
void m_tb_key_net_get_ids(uint8_t *p_netkey_ids);

/**
 ****************************************************************************************
 * @brief Get number of application keys bound with a network key.
 *
 * @param[in] net_key_lid       Network key local index
 *
 * @return Number of application keys bound with a network key.
 ****************************************************************************************
 */
uint8_t m_tb_key_get_nb_app_keys(m_lid_t net_key_lid);

/**
 ****************************************************************************************
 * @brief Get list of AppKey indexes for all application keys bound with a given network key.
 *
 * @param[in] net_key_lid    Network key local identifier.
 * @param[in] p_appkey_ids   Pointer to the buffer in which found AppKey indexes have to be
 * copied. Found values are packed as described in Mesh Profile specification, section 4.3.1.1.
 ****************************************************************************************
 */
void m_tb_key_app_get_ids(m_lid_t net_key_lid, uint8_t *p_appkey_ids);

/**
 ****************************************************************************************
 * @brief Get list of Application Keys (identified by the key local index) bound with a given
 * Network Key
 *
 * @param[in] net_key_lid       Network Key local index
 * @param[out] p_appkey_lids    Pointer to array that will contain the list of local indexes
 ****************************************************************************************
 */
void m_tb_key_app_get_lids(m_lid_t net_key_lid, m_lid_t *p_appkey_lids);

/**
 ****************************************************************************************
 * @brief Get list of AppKey indexes for all application keys bound with a given model.
 *
 * @param[in] model_lid      Model local identifier
 * @param[in] exp_nb_ids     Expected number of AppKey indexes
 * @param[in] p_appkey_ids   Pointer to the buffer in which found AppKey indexes have to be
 * copied. Found values are packed as described in Mesh Profile specification, section 4.3.1.1.
 ****************************************************************************************
 */
void m_tb_key_get_model_appkey_ids(m_lid_t model_lid, uint8_t exp_nb_ids, uint8_t *p_appkey_ids, bool pack);

/**
 ****************************************************************************************
 * @brief Return the NetKey index for a given network key local index.
 * It is supposed here that the provided key exists.
 *
 * @param[in] net_key_lid   Network key local index
 *
 * @return Request NetKey Index value
 ****************************************************************************************
 */
uint16_t m_tb_key_get_netkey_id(m_lid_t net_key_lid);

/**
 ****************************************************************************************
 * @brief Get node identity state for a given network key
 *
 * @param[in] net_key_lid   Network key local index
 *
 * @return Node identity state for the key provided as parameter
 ****************************************************************************************
 */
uint8_t m_tb_key_get_node_identity(m_lid_t net_key_lid);

#if (BLE_MESH_PROVISIONER)
m_tb_key_dev_info_t* m_tb_key_dev_info_get(m_lid_t dev_lid);
m_tb_key_dev_info_t *m_tb_key_dev_info_get_env(m_lid_t dev_lid);
#endif /* BLE_MESH_PROVISIONER */

m_lid_t m_tb_key_dev_id_get_by_addr(uint16_t addr);

#if (BLE_MESH_GATT_PROXY)
/**
 ****************************************************************************************
 * @brief Set node identity state for a given network key
 *
 * @param[in] net_key_lid   Network key local index
 * @param[in] identity      New node identity state
 *
 * @return Error status
 ****************************************************************************************
 */
uint16_t m_tb_key_set_node_identity(m_lid_t net_key_lid, uint8_t identity);
#endif //(BLE_MESH_GATT_PROXY)

/**
 ****************************************************************************************
 * @brief Look for a network key for which provided network ID value has been computed.
 * Several network keys might have the same network ID value.
 *
 * @param[in]     p_net_id          Pointer to the network ID value to be looked for
 * @param[in]     p_net_key_lid     Pointer to found network key local
 *
 * @return MESH_ERR_NO_ERROR if a key has been found, else error status.
 ****************************************************************************************
 */
uint16_t m_tb_key_get_net_from_net_id(uint8_t *p_net_id, m_lid_t *p_net_key_lid);

/**
 ****************************************************************************************
 * @brief Get current phase for a given network key.
 *
 * @param[in] net_key_lid   Network key local index
 *
 * @return Key phase
 ****************************************************************************************
 */
uint8_t m_tb_key_get_phase(m_lid_t net_key_lid);

/**
 ****************************************************************************************
 * @brief Inform the key manager about reception of a secure network beacon for management of
 * key refresh and IV update procedures.
 *
 * @param[in] net_key_lid   Network key local index
 * @param[in] flags         Received flags value
 * @param[in] iv            Received IV value
 ****************************************************************************************
 */
bool m_tb_key_secure_bcn_rx_ind(m_lid_t net_key_lid, uint8_t flags, uint32_t iv);

/**
 ****************************************************************************************
 * @brief Inform the key manager about reception of key refresh phase transition request from the
 * provisioner.
 *
 * @param[in] net_key_lid   Network key local index
 * @param[in] transition    Required transition
 ****************************************************************************************
 */
uint16_t m_tb_key_phase_transition_ind(m_lid_t net_key_lid, uint8_t transition);

/**
 ****************************************************************************************
 * @brief Get information needed to fill a secure network beacon to be sent on a given subnet.
 *
 * @param[in] net_key_lid       Network key local index
 * @param[out] p_iv             Pointer to address at which IV value must be written
 * @param[out] p_net_id         Pointer to address at which Network ID value must be written
 * @param[out] p_flags          Pointer to address at which flags value must be written
 ****************************************************************************************
 */
void m_tb_key_get_net_info(uint8_t net_key_lid, uint32_t *p_iv, uint8_t **p_net_id, uint8_t *p_flags);

/**
 ****************************************************************************************
 * @brief Inform the key manager that it must either transit to IV Update in Progress state
 * or in Normal state for the first network key. This function is called as part of the
 * IV Update test mode mechanism.
 *
 * @param[in] update    True if next state is IV Update in Progress, else False
 ****************************************************************************************
 */
void m_tb_key_iv_upd_test_mode_ind(bool update);

m_lid_t m_tb_key_dev_info_add(uint16_t nwk_key_lid, uint16_t nwk_key_id, uint16_t nwk_addr, uint8_t nb_elt);

void m_tb_key_dev_info_reset(void);

#if (BLE_MESH_DBG)
/**
 ****************************************************************************************
 * @brief Limit number of keys that can be stored to 1
 ****************************************************************************************
 */
void m_tb_key_limit_key_nb(void);
#endif //(BLE_MESH_DBG)

#if (BLE_MESH_LPN || BLE_MESH_FRIEND)
/**
 ****************************************************************************************
 * @brief Inform the key manager about a friendship being established so that it can generate
 * security credential for this friendship.
 *
 * @param[in] idx           Friendship local index
 * @param[in] lpn_addr      Low Power Node address
 * @param[in] friend_addr   Friend address
 * @param[in] lpn_cnt       Low Power Node counter
 * @param[in] friend_cnt    Friend counter
 * @param[in] cb_finished   Callback function called once friendship security credential have
 * been generated for the friendship
 ****************************************************************************************
 */
uint16_t m_tb_key_friend_new_ind(uint8_t idx, uint16_t lpn_addr, uint16_t friend_addr, uint16_t lpn_cnt,
                                 uint16_t friend_cnt, m_tb_key_friendship_cb_finished cb_finished);

/**
 ****************************************************************************************
 * @brief Inform the key manager about a lost friendship.
 *
 * @param[in] idx           Friendship local index
 ****************************************************************************************
 */
void m_tb_key_friend_end_ind(uint8_t idx);

/**
 ****************************************************************************************
 * @brief Find a network key identifier from a NID
 *
 * @param[in]  nid     Network identifier (NID)
 * @param[in]  offset  Offset in the table which have the proposed NID
 *
 * @return Network key local identifier if found else MESH_INVALID_LID.
 ****************************************************************************************
 */
m_lid_t m_tb_key_friend_net_lid_from_nid(uint8_t nid, uint8_t offset, uint8_t *p_friend_lid);

/**
 ****************************************************************************************
 * @brief Get either encryption key or privacy for a given friendship and a given subnet.
 *
 * @param[in] net_key_lid       Network key local index
 * @param[in] friend_lid        Friendship local index
 * @param[in] key_type          Key type
 * @param[out] pp_key           Pointer used to return pointer to the requested key
 ****************************************************************************************
 */
uint16_t m_tb_key_friend_get_key(uint8_t net_key_lid, uint8_t friend_lid,
                                 uint8_t key_type, uint8_t **pp_key, bool tx);

/**
 ****************************************************************************************
 * @brief Get NID for a given friendship and a given subnet
 *
 * @param[in] net_key_lid       Network key local index
 * @param[in] friend_lid        Friendship local index
 * @param[out] p_nid            Pointer used to return the NID
 ****************************************************************************************
 */
uint16_t m_tb_key_friend_get_nid(uint8_t net_key_lid, uint8_t friend_lid, uint8_t *p_nid, bool tx);

#if (BLE_MESH_DBG)
/**
 ****************************************************************************************
 * @brief Get number of known Network IDs for friendship security credentials.
 ****************************************************************************************
 */
uint16_t m_tb_key_friend_get_nb_nids(void);

/**
 ****************************************************************************************
 * @brief Dump content of NID table for friendship security credentials
 ****************************************************************************************
 */
void m_tb_key_friend_dump_nid(m_lid_t *p_buf);
#endif //(BLE_MESH_DBG)
#endif //(BLE_MESH_LPN || BLE_MESH_FRIEND)

/// @} M_TB_KEY

#endif /* M_TB_KEY_ */
