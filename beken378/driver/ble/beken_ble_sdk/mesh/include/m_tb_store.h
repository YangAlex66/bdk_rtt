/**
 ****************************************************************************************
 * @file m_tb_store.h
 *
 * @brief Header file for Storage Manager Toolbox
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_TB_STORE_H_
#define _M_TB_STORE_H_

/**
 ****************************************************************************************
 * @defgroup M_TB_STORE Store Manager
 * @ingroup MESH
 * @brief Storage Manager Toolbox
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "m_api.h"

/*
 * MACROS
 ****************************************************************************************
 */

/// Get first entry in a section
#define M_TB_STORE_GET_NVDS_TAG(tag)  (NVDS_TAG_MESH_FIRST + tag)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// List of NVDS tag identifiers
enum m_tb_store_nvds_tag
{
    /// First tag available for mesh entries
    NVDS_TAG_MESH_FIRST                 = 0xB0,

    /// Unicast Address
    NVDS_TAG_MESH_UNICAST_ADDR          = 0,
    NVDS_LEN_MESH_UNICAST_ADDR          = 2,

    /// Default TTL
    NVDS_TAG_MESH_DEFAULT_TTL           = NVDS_TAG_MESH_UNICAST_ADDR + 1,////1
    NVDS_LEN_MESH_DEFAULT_TTL           = 1,

    /// Secure Network Beacon
    NVDS_TAG_MESH_SECURE_BCN            = NVDS_TAG_MESH_DEFAULT_TTL + 1,///2
    NVDS_LEN_MESH_SECURE_BCN            = 1,

    /// Network Transmit
    NVDS_TAG_MESH_NETWORK_TX            = NVDS_TAG_MESH_SECURE_BCN + 1,//3
    NVDS_LEN_MESH_NETWORK_TX            = 1,

    /// Relay
    NVDS_TAG_MESH_RELAY                 = NVDS_TAG_MESH_NETWORK_TX + 1,//4
    NVDS_LEN_MESH_RELAY                 = 2,

    /// GATT Proxy
    NVDS_TAG_MESH_GATT_PROXY            = NVDS_TAG_MESH_RELAY + 1,//5
    NVDS_LEN_MESH_GATT_PROXY            = 1,

    /// Friend
    NVDS_TAG_MESH_FRIEND                = NVDS_TAG_MESH_GATT_PROXY + 1,//6
    NVDS_LEN_MESH_FRIEND                = 1,

   
    NVDS_TAG_MESH_CTLSET_VALUE	= NVDS_TAG_MESH_FRIEND+1,   ///add 0305
    NVDS_LEN_MESH_CTLSET_VALUE	= 1,
    

#if (BLE_MESH_PROVISIONER)
    /// Device Key
    NVDS_TAG_MESH_DEV_KEY_FIRST         = NVDS_TAG_MESH_CTLSET_VALUE + 1, //7   ///add change 0305
    NVDS_TAG_MESH_DEV_KEY_LAST          = NVDS_TAG_MESH_DEV_KEY_FIRST + M_TB_KEY_MAX_NB_DEV - 1,
    NVDS_LEN_MESH_DEV_KEY               = 32,
#else /* !BLE_MESH_PROVISIONER */
    /// Device Key
    NVDS_TAG_MESH_DEV_KEY               = NVDS_TAG_MESH_CTLSET_VALUE + 1, //7
    NVDS_LEN_MESH_DEV_KEY               = 16,
#endif /* BLE_MESH_PROVISIONER */

#if (BLE_MESH_PROVISIONER)
    NVDS_TAG_MESH_IV_SEQ                = NVDS_TAG_MESH_DEV_KEY_LAST + 1, //8
#else /* !BLE_MESH_PROVISIONER */
    /// IV - Sequence Number
    NVDS_TAG_MESH_IV_SEQ                = NVDS_TAG_MESH_DEV_KEY + 1, //8
#endif /* BLE_MESH_PROVISIONER */
    NVDS_LEN_MESH_IV_SEQ                = 14,

    /// Network Key Information
    NVDS_TAG_MESH_NET_KEY_FIRST         = NVDS_TAG_MESH_IV_SEQ + 1,//9
    NVDS_TAG_MESH_NET_KEY_LAST          = NVDS_TAG_MESH_NET_KEY_FIRST + M_TB_KEY_MAX_NB_NET - 1, //M_TB_KEY_MAX_NB_NET 5
    NVDS_LEN_MESH_NET_KEY               = 36,

    /// Application Key Information
    NVDS_TAG_MESH_APP_KEY_FIRST         = NVDS_TAG_MESH_NET_KEY_LAST + 1,//14
    NVDS_TAG_MESH_APP_KEY_LAST          = NVDS_TAG_MESH_APP_KEY_FIRST + M_TB_KEY_MAX_NB_APP - 1,// M_TB_KEY_MAX_NB_APP 8
    NVDS_LEN_MESH_APP_KEY               = 38,

    /// Publication Parameters
    NVDS_TAG_MESH_PUBLI_FIRST           = NVDS_TAG_MESH_APP_KEY_LAST + 1,//22
    NVDS_TAG_MESH_PUBLI_LAST            = NVDS_TAG_MESH_PUBLI_FIRST + M_TB_MIO_MODEL_NB - 1,//M_TB_MIO_MODEL_NB 20
    NVDS_LEN_MESH_PUBLI                 = 32,

    /// Subscription Lists
    NVDS_TAG_MESH_SUBS_FIRST            = NVDS_TAG_MESH_PUBLI_LAST + 1,//44
    NVDS_TAG_MESH_SUBS_LAST             = NVDS_TAG_MESH_SUBS_FIRST + M_TB_MIO_MODEL_NB - 1,//
    NVDS_LEN_MESH_SUBS                  = 255,

    /// Bindings section
    NVDS_TAG_MESH_BINDINGS_FIRST        = NVDS_TAG_MESH_SUBS_LAST + 1,//64
    NVDS_TAG_MESH_BINDINGS_LAST         = NVDS_TAG_MESH_BINDINGS_FIRST + M_TB_MIO_MODEL_NB - 1,//
    NVDS_LEN_MESH_BINDINGS              = 255,

    /// Friend Information
    NVDS_TAG_MESH_FRIEND_INFO_FIRST     = NVDS_TAG_MESH_BINDINGS_LAST + 1,
    NVDS_TAG_MESH_FRIEND_INFO_LAST      = NVDS_TAG_MESH_FRIEND_INFO_FIRST + M_FRIEND_LPN_MAX - 1,
    NVDS_LEN_MESH_FRIEND_INFO           = 20 + M_FRIEND_SUBS_LIST_MAX_LEN * 2,

    /// LPN Information
    NVDS_TAG_MESH_LPN_INFO              = NVDS_TAG_MESH_FRIEND_INFO_LAST + 1,
    NVDS_LEN_MESH_LPN_INFO              = 16,

#if (BLE_MESH_LPN)
    NVDS_TAG_MESH_NB                    = NVDS_TAG_MESH_LPN_INFO + 1,
#elif (BLE_MESH_FRIEND)
    NVDS_TAG_MESH_NB                    = NVDS_TAG_MESH_FRIEND_INFO_LAST + 1,
#else
    NVDS_TAG_MESH_NB                    = NVDS_TAG_MESH_BINDINGS_LAST + 1,
#endif //(BLE_MESH_FRIEND || BLE_MESH_LPN)
};

/// Update types
enum m_tb_store_upd_type
{
    /// Network key updated
    M_TB_STORE_UPD_TYPE_NET_KEY_UPDATED,
    /// Network key deleted
    M_TB_STORE_UPD_TYPE_NET_KEY_DELETED,

    /// Application key updated
    M_TB_STORE_UPD_TYPE_APP_KEY_UPDATED,
    /// Application key deleted
    M_TB_STORE_UPD_TYPE_APP_KEY_DELETED,

    /// Model publication parameters updated
    M_TB_STORE_UPD_TYPE_PUBLI_PARAM,
    /// Model subscription list updated
    M_TB_STORE_UPD_TYPE_SUBS_LIST,
    /// Model/application key binding updated
    M_TB_STORE_UPD_TYPE_BINDING,

#if (BLE_MESH_PROVISIONER)
    /// Device key updated
    M_TB_STORE_UPD_TYPE_DEV_KEY_UPDATED,
#endif /* BLE_MESH_PROVISIONER */
    /// State updated
    M_TB_STORE_UPD_TYPE_STATE,

    /// Friendship with LPN updated
    M_TB_STORE_UPD_LPN_UPDATED,
    /// Friendship with LPN lost
    M_TB_STORE_UPD_LPN_LOST,

    /// Friendship with Friend updated
    M_TB_STORE_UPD_FRIEND_UPDATED,
    /// Friendship with Friend lost
    M_TB_STORE_UPD_FRIEND_LOST,
};

/// State types
enum m_tb_store_state_type
{
    /// Primary element unicast address
    M_TB_STORE_TYPE_UNICAST_ADDR         = 0,
    /// Default TTL state
    M_TB_STORE_TYPE_DEFAULT_TTL_STATE,
    /// Secure network beacon state
    M_TB_STORE_TYPE_SEC_BCN_STATE,
    /// Network transmit state
    M_TB_STORE_TYPE_NET_TX_STATE,
    /// Relay state
    M_TB_STORE_TYPE_RELAY_STATE,
    /// GATT proxy state
    M_TB_STORE_TYPE_GATT_PROXY_STATE,
    /// Friend state
    M_TB_STORE_TYPE_FRIEND_STATE,
    /// Device key
    M_TB_STORE_TYPE_DEV_KEY,
    /// IV and SEQ values
    M_TB_STORE_TYPE_IV_SEQ,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Definition of callback function to call upon reception of a page of composition data
 *
 * @param[in] page      Page number of the composition data
 * @param[in] length    Page length
 * @param[in] p_data    Pointer to page content
 ****************************************************************************************
 */
typedef void (*m_tb_store_cb_compo_data_t)(uint8_t page, uint8_t length, uint8_t *p_data);

/**
 ****************************************************************************************
 * @brief Definition of callback function to call at end of load procedure
 ****************************************************************************************
 */
typedef void (*m_tb_store_cb_load_t)(uint16_t status);

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Configure storage manager behavior
 ****************************************************************************************
 */
uint16_t m_tb_store_config(uint32_t config);

/**
 ****************************************************************************************
 * @brief Request the storage manager to retrieve and load stored data.
 ****************************************************************************************
 */
uint16_t m_tb_store_load(uint16_t length, uint8_t *p_data, m_tb_store_cb_load_t cb_load);

/**
 ****************************************************************************************
 * @brief Request the storage manager to store all data.
 ****************************************************************************************
 */
uint16_t m_tb_store_save(void);

#if (BLE_MESH_STORAGE_NONE)
#define m_tb_store_update_ind(upd_type, dummy)
#else
/**
 ****************************************************************************************
 * @brief Inform the storage manager about an update of a parameter that need to be stored
 *
 * @param[in] upd_type      Update type (@see enum m_tb_store_upd_type)
 * @param[in] dummy         Dummy parameter whose meaning depends on update type value
 ****************************************************************************************
 */
void m_tb_store_update_ind(uint8_t upd_type, uint32_t dummy);
#endif //(BLE_MESH_STORAGE_NONE)

/**
 ****************************************************************************************
 * @brief Get a page of composition data
 *
 * @param[in] page      Page number of the composition data
 * @param[in] cb        Callback function called one the requested page is available
 ****************************************************************************************
 */
void m_tb_store_get_compo_data(uint8_t page, m_tb_store_cb_compo_data_t cb);

/**
 ****************************************************************************************
 * @brief Function called upon reception of a page of composition data
 *
 * @param[in] page      Page number of the composition data
 * @param[in] length    Page length
 * @param[in] p_data    Pointer to page content
 ****************************************************************************************
 */
void m_tb_store_rx_compo_data(uint8_t page, uint8_t length, uint8_t *p_data);

bool m_tb_store_nvs_check(void);

void m_tb_stop_scan_before_store_nvs(void);
void m_tb_store_nvs_after_stop_scan(void);

/// @} end of group

#endif //_M_TB_STORE_H_
