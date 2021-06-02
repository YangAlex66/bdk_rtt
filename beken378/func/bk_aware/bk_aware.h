// Copyright 2020-2021 Beken
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __BK_AWARE_H__
#define __BK_AWARE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "bk_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup WiFi_APIs WiFi Related APIs
  * @brief WiFi APIs
  */

/** @addtogroup WiFi_APIs
  * @{
  */

/** \defgroup BK_AWARE_APIs  BK_AWARE APIs
  * @brief BK_AWARE APIs
  *
  */

/** @addtogroup BK_AWARE_APIs
  * @{
  */
#define BK_ERR_WIFI_BASE             0
#define BK_ERR_BK_AWARE_BASE         (BK_ERR_WIFI_BASE + 100) /*!< BK_AWARE error number base. */
#define BK_ERR_BK_AWARE_NOT_INIT     (BK_ERR_BK_AWARE_BASE + 1) /*!< BK_AWARE is not initialized. */
#define BK_ERR_BK_AWARE_ARG          (BK_ERR_BK_AWARE_BASE + 2) /*!< Invalid argument */
#define BK_ERR_BK_AWARE_NO_MEM       (BK_ERR_BK_AWARE_BASE + 3) /*!< Out of memory */
#define BK_ERR_BK_AWARE_FULL         (BK_ERR_BK_AWARE_BASE + 4) /*!< BK_AWARE peer list is full */
#define BK_ERR_BK_AWARE_NOT_FOUND    (BK_ERR_BK_AWARE_BASE + 5) /*!< BK_AWARE peer is not found */
#define BK_ERR_BK_AWARE_INTERNAL     (BK_ERR_BK_AWARE_BASE + 6) /*!< Internal error */
#define BK_ERR_BK_AWARE_EXIST        (BK_ERR_BK_AWARE_BASE + 7) /*!< BK_AWARE peer has existed */
#define BK_ERR_BK_AWARE_IF           (BK_ERR_BK_AWARE_BASE + 8) /*!< Interface error */

#define BK_AWARE_ETH_ALEN             6         /*!< Length of BK_AWARE peer MAC address */
#define BK_AWARE_KEY_LEN              16        /*!< Length of BK_AWARE peer local master key */

#define BK_AWARE_MAX_TOTAL_PEER_NUM   20        /*!< Maximum number of BK_AWARE total peers */
#define BK_AWARE_MAX_ENCRYPT_PEER_NUM 6         /*!< Maximum number of BK_AWARE encrypted peers */

#define BK_AWARE_MAX_DATA_LEN         250       /*!< Maximum length of BK_AWARE data which is sent very time */

/**
 * @brief Status of sending BK_AWARE data .
 */
typedef enum {
    BK_AWARE_SEND_SUCCESS = 0,       /**< Send BK_AWARE data successfully */
    BK_AWARE_SEND_FAIL,              /**< Send BK_AWARE data fail */
} bk_aware_send_status_t;

/**
 * @brief BK_AWARE peer information parameters.
 */
typedef struct bk_aware_peer_info {
    uint8_t peer_addr[BK_AWARE_ETH_ALEN];    /**< BK_AWARE peer MAC address that is also the MAC address of station or softap */
    uint8_t lmk[BK_AWARE_KEY_LEN];           /**< BK_AWARE peer local master key that is used to encrypt data */
    uint8_t channel;                        /**< Wi-Fi channel that peer uses to send/receive BK_AWARE data. If the value is 0,
                                                 use the current channel which station or softap is on. Otherwise, it must be
                                                 set as the channel that station or softap is on. */
    uint8_t ifidx;                 /**< Wi-Fi interface that peer uses to send/receive BK_AWARE data */
    bool encrypt;                           /**< BK_AWARE data that this peer sends/receives is encrypted or not */
    void *priv;                             /**< BK_AWARE peer private data */
} bk_aware_peer_info_t;

/**
 * @brief Number of BK_AWARE peers which exist currently.
 */
typedef struct bk_aware_peer_num {
    int total_num;                           /**< Total number of BK_AWARE peers, maximum value is BK_AWARE_MAX_TOTAL_PEER_NUM */
    int encrypt_num;                         /**< Number of encrypted BK_AWARE peers, maximum value is BK_AWARE_MAX_ENCRYPT_PEER_NUM */
} bk_aware_peer_num_t;

/**
  * @brief     Callback function of receiving BK_AWARE data
  * @param     mac_addr peer MAC address
  * @param     data received data
  * @param     data_len length of received data
  */
typedef void (*bk_aware_recv_cb_t)(const uint8_t *mac_addr, const uint8_t *data, int data_len);

/**
  * @brief     Callback function of sending BK_AWARE data
  * @param     mac_addr peer MAC address
  * @param     status status of sending BK_AWARE data (succeed or fail)
  */
typedef void (*bk_aware_send_cb_t)(const uint8_t *mac_addr, bk_aware_send_status_t status);

/**
  * @brief     Initialize BK_AWARE function
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_INTERNAL : Internal error
  */
bk_err_t bk_aware_init(void);

/**
  * @brief     De-initialize BK_AWARE function
  *
  * @return
  *          - BK_OK : succeed
  */
bk_err_t bk_aware_deinit(void);

/**
  * @brief     Get the version of BK_AWARE
  *
  * @param     version  BK_AWARE version
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  */
bk_err_t bk_aware_get_version(uint32_t *version);

/**
  * @brief     Register callback function of receiving BK_AWARE data
  *
  * @param     cb  callback function of receiving BK_AWARE data
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BK_AWARE is not initialized
  *          - BK_ERR_BK_AWARE_INTERNAL : internal error
  */
bk_err_t bk_aware_register_recv_cb(bk_aware_recv_cb_t cb);

/**
  * @brief     Unregister callback function of receiving BK_AWARE data
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BK_AWARE is not initialized
  */
bk_err_t bk_aware_unregister_recv_cb(void);

/**
  * @brief     Register callback function of sending BK_AWARE data
  *
  * @param     cb  callback function of sending BK_AWARE data
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BK_AWARE is not initialized
  *          - BK_ERR_BK_AWARE_INTERNAL : internal error
  */
bk_err_t bk_aware_register_send_cb(bk_aware_send_cb_t cb);

/**
  * @brief     Unregister callback function of sending BK_AWARE data
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BK_AWARE is not initialized
  */
bk_err_t bk_aware_unregister_send_cb(void);

/**
  * @brief     Send BK_AWARE data
  *
  * @attention 1. If peer_addr is not NULL, send data to the peer whose MAC address matches peer_addr
  * @attention 2. If peer_addr is NULL, send data to all of the peers that are added to the peer list
  * @attention 3. The maximum length of data must be less than BK_AWARE_MAX_DATA_LEN
  * @attention 4. The buffer pointed to by data argument does not need to be valid after bk_aware_send returns
  *
  * @param     peer_addr  peer MAC address
  * @param     data  data to send
  * @param     len  length of data
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BK_AWARE is not initialized
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  *          - BK_ERR_BK_AWARE_INTERNAL : internal error
  *          - BK_ERR_BK_AWARE_NO_MEM : out of memory
  *          - BK_ERR_BK_AWARE_NOT_FOUND : peer is not found
  *          - BK_ERR_BK_AWARE_IF : current WiFi interface doesn't match that of peer
  */
bk_err_t bk_aware_send(const uint8_t *peer_addr, const uint8_t *data, size_t len);

/**
  * @brief     Add a peer to peer list
  *
  * @param     peer  peer information
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BK_AWARE is not initialized
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  *          - BK_ERR_BK_AWARE_FULL : peer list is full
  *          - BK_ERR_BK_AWARE_NO_MEM : out of memory
  *          - BK_ERR_BK_AWARE_EXIST : peer has existed
  */
bk_err_t bk_aware_add_peer(const bk_aware_peer_info_t *peer);

/**
  * @brief     Delete a peer from peer list
  *
  * @param     peer_addr  peer MAC address
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BK_AWARE is not initialized
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  *          - BK_ERR_BK_AWARE_NOT_FOUND : peer is not found
  */
bk_err_t bk_aware_del_peer(const uint8_t *peer_addr);

/**
  * @brief     Modify a peer
  *
  * @param     peer  peer information
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BK_AWARE is not initialized
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  *          - BK_ERR_BK_AWARE_FULL : peer list is full
  */
bk_err_t bk_aware_mod_peer(const bk_aware_peer_info_t *peer);

/**
  * @brief     Get a peer whose MAC address matches peer_addr from peer list
  *
  * @param     peer_addr  peer MAC address
  * @param     peer  peer information
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BK_AWARE is not initialized
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  *          - BK_ERR_BK_AWARE_NOT_FOUND : peer is not found
  */
bk_err_t bk_aware_get_peer(const uint8_t *peer_addr, bk_aware_peer_info_t *peer);

/**
  * @brief     Fetch a peer from peer list. Only return the peer which address is unicast, for the multicast/broadcast address, the function will ignore and try to find the next in the peer list.
  *
  * @param     from_head  fetch from head of list or not
  * @param     peer  peer information
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BK_AWARE is not initialized
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  *          - BK_ERR_BK_AWARE_NOT_FOUND : peer is not found
  */
bk_err_t bk_aware_fetch_peer(bool from_head, bk_aware_peer_info_t *peer);

/**
  * @brief     Peer exists or not
  *
  * @param     peer_addr  peer MAC address
  *
  * @return
  *          - true : peer exists
  *          - false : peer not exists
  */
bool bk_aware_is_peer_exist(const uint8_t *peer_addr);

/**
  * @brief     Get the number of peers
  *
  * @param     num  number of peers
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BK_AWARE is not initialized
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  */
bk_err_t bk_aware_get_peer_num(bk_aware_peer_num_t *num);

/**
  * @brief     Set the primary master key
  *
  * @param     pmk  primary master key
  *
  * @attention 1. primary master key is used to encrypt local master key
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BK_AWARE is not initialized
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  */
bk_err_t bk_aware_set_pmk(const uint8_t *pmk);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __BK_AWARE_H__ */
