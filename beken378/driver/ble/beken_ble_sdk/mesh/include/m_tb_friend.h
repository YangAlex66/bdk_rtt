/**
 ****************************************************************************************
 * @file m_tb_friend.h
 *
 * @brief Header file for Friendship Manager
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_TB_FRIEND_H_
#define _M_TB_FRIEND_H_

/**
 ****************************************************************************************
 * @defgroup M_TB_FRIEND Friendship Manager
 * @ingroup MESH
 * @brief Friendship Manager
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "m_api.h"      // Mesh application program interface
#include "m_config.h"

#if (BLE_MESH_FRIEND)

/*
 * ENUMERATION
 ****************************************************************************************
 */

/// List if friendship events
enum m_tb_friend_event
{
    /// New Low Power Node has been discovered
    M_TB_FRIEND_EVENT_DISC = 0,
    /// Friendship established with Low Power Node
    M_TB_FRIEND_EVENT_ESTAB,
    /// Friendship with Low Power Node has been lost
    M_TB_FRIEND_EVENT_LOST,
};

/*
 * TYPE DEFINTIONS
 ****************************************************************************************
 */

/// Structure containing information about a Low Power Node
typedef struct m_tb_friend_lpn_info
{
    /// Poll Timeout in milliseconds
    uint32_t poll_timeout_ms;
    /// Last poll reception time
    uint32_t poll_rx_time_ms;
    uint16_t poll_rx_nb_wrap;
    /// Low Power Node address
    uint16_t lpn_addr;
    /// Low Power Node counter
    uint16_t lpn_cnt;
    /// Friend counter for transmission of Friend Offer message
    uint16_t friend_cnt;
    /// Low Power Node receive delay in milliseconds
    uint8_t  rx_delay_ms;
    /// Friend Sequence Number
    uint8_t  fsn;
    /// Number of elements in the Low Power Node
    uint8_t  nb_elements;
    /// Current number of addresses in the subscription list (valid once friendship has
    /// been established
    uint8_t  nb_addr;
    /// Subscription list
    uint16_t subs_list[M_FRIEND_SUBS_LIST_MAX_LEN];
} m_tb_friend_lpn_info_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Indicate that a new Low Power Node has been found and that friendship establishment
 * has been started.
 *
 * @param[in] lpn_lid       Friendship local index
 * @param[in] p_lpn_info    Pointer to friendship information structure
 ****************************************************************************************
 */
void m_tb_friend_lpn_add(uint8_t lpn_lid, m_tb_friend_lpn_info_t *p_lpn_info);

/**
 ****************************************************************************************
 * @brief Indicate that friendship has been successfully established with a Low Power Node.
 *
 * @param[in] lpn_lid       Friendship local index
 ****************************************************************************************
 */
void m_tb_friend_lpn_estab(uint8_t lpn_lid);

/**
 ****************************************************************************************
 * @brief Indicate that friendship with a low power node has been lost.
 *
 * @param[in] lpn_lid       Friendship local index
 * @param[in] estab         Indicate if friendship was established or not
 ****************************************************************************************
 */
void m_tb_friend_lpn_rem(uint8_t lpn_lid, bool estab);

/**
 ****************************************************************************************
 * @brief Return if a node is a Low Power Node with which a friendship is established and
 * provided the friendship local index if it is the case.
 *
 * @param[in] addr          Node address
 * @param[out] p_lpn_lid    Pointer used to return the friendship local index
 *
 * return True if provided address is well the address of a Low Power Node with which friendship
 * is established, else False
 ****************************************************************************************
 */
bool m_tb_friend_is_lpn(uint16_t addr, uint8_t *p_lpn_lid);

/**
 ****************************************************************************************
 * @brief Check if a provided address in present in the subscription list of a Low Power Node.
 *
 * @param[in] lpn_lid       Friendship local index
 * @param[in] addr          Address to seek for in the subscription list
 *
 * @return True if the provided address has been found in the subscription list of the indicated
 * Low Power Node, else False
 ****************************************************************************************
 */
bool m_tb_friend_is_subs_dst(uint8_t lpn_lid, uint16_t addr);

/**
 ****************************************************************************************
 * @brief Get Poll Timeout value in milliseconds for a given friendship
 *
 * @param[in] lpn_lid       Friendship local index
 *
 * @return Requestede Poll Timeout value in milliseconds
 ****************************************************************************************
 */
uint32_t m_tb_friend_get_polltimeout_ms(uint8_t lpn_lid);

/**
 ****************************************************************************************
 * @brief Return number of known Low Power Nodes (friendship established or not)
 ****************************************************************************************
 */
uint8_t m_tb_friend_get_nb_lpn_known(void);

/**
 ****************************************************************************************
 * @brief Return number of Low Power Nodes with which a friendship has been successfully
 * established.
 ****************************************************************************************
 */
uint8_t m_tb_friend_get_nb_lpn_estab(void);

/**
 ****************************************************************************************
 * @brief Add (or remove) an address in (from) the subscription list of a Low Power Node.
 *
 * @param[in] lpn_lid       Low power node loca lindex
 * @param[in] addr          Address
 * @param[in] add           True if provided address must be added, else False
 ****************************************************************************************
 */
void m_tb_friend_subs_list_update(uint8_t lpn_lid, uint16_t addr, bool add);

/**
 ****************************************************************************************
 * @brief Return current Friend Counter value. This value is incremented each time a Friend
 * Request message is transmitted.
 ****************************************************************************************
 */
uint16_t m_tb_friend_get_friend_cnt(void);

#if (BLE_MESH_DBG || !BLE_MESH_STORAGE_NONE)
/**
 ****************************************************************************************
 * @brief Retrieve information about a given Low Power Node
 *
 * @param[in] lpn_lid       Friendship local index
 ****************************************************************************************
 */
m_tb_friend_lpn_info_t *m_tb_friend_get_lpn_info(uint8_t lpn_lid);
#endif //(BLE_MESH_DBG || !BLE_MESH_STORAGE_NONE)

#if (BLE_MESH_DBG)
/**
 ****************************************************************************************
 * @brief DEBUG PURPOSE ONLY - Set the value of the Friend Counter.
 *
 * @param[in] friend_cnt        New Friend Counter value
 ****************************************************************************************
 */
void m_tb_friend_set_friend_cnt(uint16_t friend_cnt);
#endif //(BLE_MESH_DBG)

/**
 ****************************************************************************************
 * @brief Get receive window value in milliseconds
 ****************************************************************************************
 */
uint8_t m_tb_friend_get_rx_window(void);

/**
 ****************************************************************************************
 * @brief Get friend queue size
 ****************************************************************************************
 */
uint8_t m_tb_friend_get_queue_size(void);

/**
 ****************************************************************************************
 * @brief Get friend subscription list size
 ****************************************************************************************
 */
uint8_t m_tb_friend_get_subs_list_size(void);

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
uint16_t m_tb_friend_load(m_tb_friend_lpn_info_t *p_lpn_info, m_lid_t *p_lpn_lid, m_api_end_cb end_cb);
#endif //(!BLE_MESH_STORAGE_NONE)

#endif //(BLE_MESH_FRIEND)

/// @} end of group

#endif //_M_TB_FRIEND_H_
