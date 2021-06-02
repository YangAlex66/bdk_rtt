/**
 ****************************************************************************************
 * @file m_lay_friend.h
 *
 * @brief Header file for Mesh Friend Features Defines
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_LAY_FRIEND_H_
#define _M_LAY_FRIEND_H_

/**
 ****************************************************************************************
 * @defgroup M_FRIEND Mesh Friend Features Defines
 * @ingroup MESH
 * @brief Mesh Friend Features Defines
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "m_lay_int.h"      // Mesh layer internals

/*
 * DEFINES
 ****************************************************************************************
 */

/// Length of Friend Request message
#define M_FRIEND_MSG_REQ_LEN                (10)
/// Length of Friend Offer message
#define M_FRIEND_MSG_OFFER_LEN              (6)
/// Length of Friend Poll message
#define M_FRIEND_MSG_POLL_LEN               (1)
/// Length of Friend Update message
#define M_FRIEND_MSG_UPDATE_LEN             (6)
/// Length of Friend Clear message
#define M_FRIEND_MSG_CLEAR_LEN              (4)
/// Length of Friend Clear Confirm message
#define M_FRIEND_MSG_CLEAR_CFM_LEN          (4)
/// Minimal length of Friend Subscription List Add message
#define M_FRIEND_MSG_SUBS_LIST_ADD_LEN      (3)
/// Minimal length of Friend Subscription List Remove message
#define M_FRIEND_MSG_SUBS_LIST_REM_LEN      (3)
/// Length of Friend Request message
#define M_FRIEND_MSG_SUBS_LIST_CFM_LEN      (1)

/// Format of Friend Request message
#define M_FRIEND_MSG_REQ_FORMAT             (">BBDHBH")
/// Length of Friend Offer message
#define M_FRIEND_MSG_OFFER_FORMAT           (">BBBBH")
/// Length of Friend Poll message
#define M_FRIEND_MSG_POLL_FORMAT            (">B")
/// Length of Friend Update message
#define M_FRIEND_MSG_UPDATE_FORMAT          (">BLB")
/// Length of Friend Clear message
#define M_FRIEND_MSG_CLEAR_FORMAT           (">HH")
/// Length of Friend Clear Confirm message
#define M_FRIEND_MSG_CLEAR_CFM_FORMAT       (">HH")
/// Length of Friend Request message
#define M_FRIEND_MSG_SUBS_LIST_CFM_FORMAT   (">B")

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Friend Request message content
typedef struct m_friend_req
{
    /// Criteria (@see enum m_friend_criteria)
    uint8_t  criteria;
    /// Receive Delay
    uint8_t  rx_delay;
    /// Poll Timeout
    uint32_t poll_timeout;
    /// Previous Address
    uint16_t prev_addr;
    /// Number of elements
    uint8_t  nb_elem;
    /// LPN Counter
    uint16_t lpn_cnt;
} m_friend_req_t;

/// Friend Offer message content
typedef struct m_friend_offer
{
    /// Receive Window
    uint8_t  rx_window;
    /// Queue size
    uint8_t  queue_size;
    /// Subscription list size
    uint8_t  subs_list_size;
    /// RSSI
    int8_t   rssi;
    /// Friend Counter
    uint16_t friend_cnt;
} m_friend_offer_t;

/// Friend Poll message content
typedef struct m_friend_poll
{
    /// FSN (@see enum m_friend_fsn)
    uint8_t  fsn;
} m_friend_poll_t;

/// Friend Update message content
typedef struct m_friend_upd
{
    /// Flags
    uint8_t  flags;
    /// IV index
    uint32_t iv_index;
    /// More Data (@see enum m_friend_md)
    uint8_t  md;
} m_friend_upd_t;

/// Friend Clear message content
typedef struct m_friend_clear
{
    /// LPN Address
    uint16_t lpn_addr;
    /// LPN Counter
    uint16_t lpn_cnt;
} m_friend_clear_t;

/// Friend Clear Confirm message content
typedef struct m_friend_clear_cfm
{
    /// LPN Address
    uint16_t lpn_addr;
    /// LPN Counter
    uint16_t lpn_cnt;
} m_friend_clear_cfm_t;

/// Friend Subscription List Add message content
typedef struct m_friend_subs_list_add
{
    /// Transaction Number
    uint8_t  transac_nb;
    ///  List of group addresses and virtual addresses
    uint16_t list[__ARRAY_EMPTY];
} m_friend_subs_list_add_t;

/// Friend Subscription List Remove message content
typedef struct m_friend_subs_list_rem
{
    /// Transaction Number
    uint8_t  transac_nb;
    ///  List of group addresses and virtual addresses
    uint16_t list[__ARRAY_EMPTY];
} m_friend_subs_list_rem_t;

/// Friend Subscription List Confirm message content
typedef struct m_friend_subs_list_cfm
{
    /// Transaction Number
    uint8_t  transac_nb;
} m_friend_subs_list_cfm_t;

/// Friend message definition
union m_friend_msg
{
    /// Friend Request
    m_friend_req_t           req;
    /// Friend Offer
    m_friend_offer_t         offer;
    /// Friend Poll
    m_friend_poll_t          poll;
    /// Friend Update
    m_friend_upd_t           upd;
    /// Friend Clear
    m_friend_clear_t         clear;
    /// Friend Clear Confirm
    m_friend_clear_cfm_t     clear_cfm;
    /// Friend Subscription List Add
    m_friend_subs_list_add_t subs_list_add;
    /// Friend Subscription List Remove
    m_friend_subs_list_rem_t subs_list_rem;
    /// Friend Subscription List Confirm
    m_friend_subs_list_cfm_t subs_list_cfm;
};

/// @} end of group

#endif //_M_LAY_FRIEND_H_
