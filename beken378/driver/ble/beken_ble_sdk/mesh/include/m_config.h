/**
 ****************************************************************************************
 *
 * @file m_config.h
 *
 * @brief Header file for Mesh configuration defines
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef M_CONFIG_
#define M_CONFIG_

/**
 ****************************************************************************************
 * @defgroup M_CONFIG Mesh configuration defines
 * @ingroup MESH
 * @brief  Mesh configuration defines
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh_config.h"        // Mesh Stack Configuration
#include "mesh_param_int.h"
#include "ble_uart.h"
#include "mesh_log.h"
/*
 * COMPILATION FLAGS
 ****************************************************************************************
 */


/*
 * DEFINES
 ****************************************************************************************
 */
 
/// Default TTL to use for transmission  TODO Need to be managed by Foundation layer
#define M_TTL_DEFAULT                        (5) // 11

/// Number of advertising transmission to perform
#define M_ADV_NB_TX                          (5)     /* For improve the BLE  Mesh sending performance, change the number of advertising from 3 to 5. */
/// Advertising interval - 100ms - 160 slots
#define M_ADV_INTERVAL                       (32 * 10) // 80
/// Network Advertising interval - 20ms - 32 slotss
#define M_ADV_NET_INTERVAL                   (48)
/// Advertising interval - 20ms - 32 slotss
#define M_ADV_PROVISIONING_INTERVAL          (32)
/// Advertising connectable interval - 500ms - 800 slots
#define M_ADV_CON_INTERVAL                   (48)
/// Scanning interval - 30ms - 48 slots
#define M_ADV_SCAN_INTERVAL                  (24)

//#define M_ADV_NET_NB_TX_MAX                (10)
//#define M_ADV_NET_NB_TX_MIN                (3)

#define M_ADV_NET_NB_TX_MAX                  (6)
#define M_ADV_NET_NB_TX_MIN                  (3)

/// Maximum number of subnets the node can belong to
#define M_SUBNET_NB_MAX                      (5)

/// Maximum number of connectable bearer
#define M_BEARER_CON_MAX                     (BLE_CONNECTION_MAX)

/// Maximum number of bearers that can be added
#define M_BEARER_MAX                         (M_BEARER_CON_MAX + 1)
/// Maximum number of network interfaces (+ 1 for Local Interface)
#define M_LAY_NET_INTF_MAX                   (M_BEARER_MAX + 1)
/// Number of entries in the Network Message Cache
#define M_LAY_NET_MSG_CACHE_SIZE             (50)
/// Number of entries in the Network Pre-filtering list
#define M_LAY_NET_PRE_FILT_SIZE              (50)

/// Number of buffer that can be used for transmission of generated lower transmit packet
#define M_LAY_LTRANS_NB_TX_BUFFER            (6 * 2)
/// Number of segmentation that can be performed in parallel
#define M_LAY_LTRANS_NB_SEGMENTATION         (6 * 2)
/// Number of reassembly that can be performed in parallel
#define M_LAY_LTRANS_NB_REASSEMBLY           (6 * 2)
/// Number of segment info in filtering list
#define M_LAY_LTRANS_NB_SEGMENT_FILTER       (20)	//(100)//6 set 0 for ota bug sean 
/// Number of time to send a segmented message which not target a unicast address
#define M_LAY_LTRANS_NB_SEG_PACKET_RETRANS   (2)

/// Transmission Pool size
#define M_LAY_ACCESS_TX_POOL_SIZE            (6)


/// Number of model instances used by the device
/// Shall be at least 2 (Configuration Server model and Health Server model)
#define M_TB_MIO_MODEL_NB                    (MESH_MODEL_NB_MAX)
/// Number of addresses that can be put in the subscription list for a model instance
#define M_TB_MIO_SUBS_LIST_SIZE              (10)
/// Number of virtual addresses that can be stored
#define M_TB_MIO_VIRT_ADDR_LIST_SIZE         (5)
#if (BLE_MESH_PROVISIONER == 1)
#define M_TB_KEY_MAX_NB_DEV                  (100)
#else
#define M_TB_KEY_MAX_NB_DEV                  (1)
#endif

/// Maximum number of network keys supported by Mesh stack
#define M_TB_KEY_MAX_NB_NET                  (M_SUBNET_NB_MAX)
/// Maximum number of Application keys supported by Mesh stack
#define M_TB_KEY_MAX_NB_APP                 (5)
/// Total number of keys
#define M_TB_KEY_MAX_NB                     (M_TB_KEY_MAX_NB_DEV + M_TB_KEY_MAX_NB_NET + M_TB_KEY_MAX_NB_APP)
/// Maximum number of Bound between Application and Model supported by Mesh stack
#define M_TB_KEY_MAX_NB_APP_MODEL_BIND      (40)

/// Maximum number of connection Low Power Nodes when Friend Feature is used
#define M_FRIEND_LPN_MAX                    (5)
/// Maximum number of messages stored for a Low Power Node - Should be a power of 2
#define M_FRIEND_NB_STORED_MSG_MAX          (16)
/// Maximum number of address in Subscription List for a Low Power Node (max 255)
#define M_FRIEND_SUBS_LIST_MAX_LEN          (10)
/// Receive window in milliseconds
#define M_FRIEND_RECEIVE_WINDOW_MS          (3 * ((m_stack_param.m_adv_scan_interval > 0) ? m_stack_param.m_adv_scan_interval : M_ADV_SCAN_INTERVAL) * SLOT_SIZE / 1000)

/// Duration between two update of Connectable advertising data (100ms)
#define M_PROXY_CON_ADV_UPDATE_DUR          (50000)//(100)

#define M_PROXY_CON_ADV_ALWAYS_ON           1
#define M_PROXY_CON_ADV_ALWAYS_OFF          0

/// Size of the Proxy filter list per active connections
#define M_PROXY_FILT_LIST_SIZE              (30)
/// @} M_CONFIG

#endif /* M_CONFIG_ */
