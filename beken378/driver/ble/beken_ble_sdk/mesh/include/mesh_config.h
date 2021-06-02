/**
 ****************************************************************************************
 *
 * @file mesh_config.h
 *
 * @brief Header file for Mesh Stack Configuration
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef MESH_CONFIG_
#define MESH_CONFIG_

/**
 ****************************************************************************************
 * @defgroup MESH_CONFIG Mesh Stack Configuration
 * @ingroup MESH
 * @brief  Mesh Stack Configuration
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "rwip_task.h"
#include "mesh_defines.h"
#include "user_config.h"
/*
 * COMPILATION FLAGS
 ****************************************************************************************
 */


/// Flag indicating that Mesh Debug is enabled
#if (defined(CFG_BLE_MESH_ROLE_PROVISIONER))
#define BLE_MESH_PROVISIONER                         (1)
#else
#define BLE_MESH_PROVISIONER                         (0)
#endif //(defined(CFG_BLE_MESH_DBG))

#if (defined(CONFIG_MESH_LOG_OUTPUT))
#define BLE_MESH_LOG_OUTPUT                  (1)
#else
#define BLE_MESH_LOG_OUTPUT                  (0)
#endif /* (defined(CONFIG_MESH_LOG_OUTPUT)) */

#if (defined(CONFIG_MESH_SCH_GPIO_TRIGER_TEST))
#define MESH_SCH_GPIO_TRIGER_TEST            (1)
#else
#define MESH_SCH_GPIO_TRIGER_TEST            (0)
#endif /* defined(CONFIG_MESH_SCH_GPIO_TRIGER_TEST) */

/// Flag indicating that Mesh Debug is enabled
#if (defined(CFG_BLE_MESH_DBG))
#define BLE_MESH_DBG                         (1)
#else
#define BLE_MESH_DBG                         (0)
#endif //(defined(CFG_BLE_MESH_DBG))

/// Flag indicating that Mesh Message API is enabled
#if (defined(CFG_BLE_MESH_MSG_API))
#define BLE_MESH_MSG_API                     (1)
#else
#define BLE_MESH_MSG_API                     (0)
#endif //(defined(CFG_BLE_MESH_MSG_API))

#if (defined(CFG_BLE_MESH_STORAGE_NONE))
#define BLE_MESH_STORAGE_NONE                (1)
#else
#define BLE_MESH_STORAGE_NONE                (0)
#endif //(BLE_MESH_STORAGE_NONE)

#if (defined(CFG_BLE_MESH_STORAGE_WVT))
#define BLE_MESH_STORAGE_WVT                 (1)
#else
#define BLE_MESH_STORAGE_WVT                 (0)
#endif //(BLE_MESH_STORAGE_WVT)

#if (defined(CFG_BLE_MESH_STORAGE_NVDS))
#define BLE_MESH_STORAGE_NVDS                (1)
#else
#define BLE_MESH_STORAGE_NVDS                (0)
#endif //(CFG_BLE_MESH_STORAGE_NVDS)

/// Support of GATT Proxy feature
#if defined(CFG_BLE_MESH_GATT_PROXY)
#define BLE_MESH_GATT_PROXY                  (1)
#else
#define BLE_MESH_GATT_PROXY                  (0)
#endif //defined(CFG_BLE_MESH_GATT_PROXY)

/// Support of GATT Provisioning feature
#if defined(CFG_BLE_MESH_GATT_PROV)
#define BLE_MESH_GATT_PROV                   (1)
#else
#define BLE_MESH_GATT_PROV                   (0)
#endif //defined(CFG_BLE_MESH_GATT_PROV)

/// Used to know if GATT Bearer is present
#define BLE_MESH_GATT_BEARER                 (BLE_MESH_GATT_PROXY || BLE_MESH_GATT_PROV)

/// Support of Relay feature
#if defined(CFG_BLE_MESH_RELAY)
#define BLE_MESH_RELAY                       (1)
#else
#define BLE_MESH_RELAY                       (0)
#endif //defined(CFG_BLE_MESH_RELAY)

/// Support of Friend feature
#if defined(CFG_BLE_MESH_FRIEND)
#define BLE_MESH_FRIEND                      (1)
#else
#define BLE_MESH_FRIEND                      (0)
#endif //defined(CFG_BLE_MESH_FRIEND)

/// Support of Low Power Node feature
#if defined(CFG_BLE_MESH_LPN)
#define BLE_MESH_LPN                         (1)
#else
#define BLE_MESH_LPN                         (0)
#endif //defined(CFG_BLE_MESH_LPN)

#if defined(CFG_MESH_MEM_TB_BUF_DBG)
#define MESH_MEM_TB_BUF_DBG                  (1)
#else /*  undef CFG_MESH_MEM_TB_BUF_DBG */
#define MESH_MEM_TB_BUF_DBG                  (0)
#endif /* CFG_MESH_MEM_TB_BUF_DBG */

/// Supported feature mask
#define BLE_MESH_FEAT_MASK   (  (BLE_MESH_MSG_API    * M_FEAT_MSG_API_SUP        )\
                              | (BLE_MESH_RELAY      * M_FEAT_RELAY_NODE_SUP     )\
                              | (BLE_MESH_GATT_PROXY * M_FEAT_PROXY_NODE_SUP     )\
                              | (BLE_MESH_GATT_PROV  * M_FEAT_PB_GATT_SUP        )\
                              | (BLE_MESH_FRIEND     * M_FEAT_FRIEND_NODE_SUP    )\
                              | (BLE_MESH_LPN        * M_FEAT_LOW_POWER_NODE_SUP ))

/*
 * COMPILATION FLAGS FOR MODELS
 ****************************************************************************************
 */

/// Support of server role for models
#if (defined(CFG_BLE_MESH_MDL_SERVER))
#define BLE_MESH_MDL_SERVER                  (1)
#else
#define BLE_MESH_MDL_SERVER                  (0)
#endif //(BLE_MESH_MDL_SERVER)

/// Support of client role for models
#if (defined(CFG_BLE_MESH_MDL_CLIENT))
#define BLE_MESH_MDL_CLIENT                  (1)
#else
#define BLE_MESH_MDL_CLIENT                  (0)
#endif //(BLE_MESH_MDL_CLIENT)

/// Support of models
#define BLE_MESH_MDL                         (BLE_MESH_MDL_SERVER || BLE_MESH_MDL_CLIENT)

/// Support of Generic Server Models
#if (defined(CFG_BLE_MESH_MDL_GENS))
#define BLE_MESH_MDL_GENS                    (1)
#else
#define BLE_MESH_MDL_GENS                    (0)
#endif //(BLE_MESH_MDL_GENS)


/// Support of Generic Battery Server Models
#if (defined(CFG_BLE_MESH_MDL_GENS_OO))
#define BLE_MESH_MDL_GENS_OO                    (1)
#else
#define BLE_MESH_MDL_GENS_OO                    (0)
#endif //(BLE_MESH_MDL_GENS_OO)

/// Support of Generic Battery Server Models
#if (defined(CFG_BLE_MESH_MDL_GENS_BAT))
#define BLE_MESH_MDL_GENS_BAT                    (1)
#else
#define BLE_MESH_MDL_GENS_BAT                    (0)
#endif //(BLE_MESH_MDL_GENS_BAT)


/// Support of Generic Level Server Models
#if (defined(CFG_BLE_MESH_MDL_GENS_LVL))
#define BLE_MESH_MDL_GENS_LVL                    (1)
#else
#define BLE_MESH_MDL_GENS_LVL                    (0)
#endif //(BLE_MESH_MDL_GENS_LVL)


/// Support of Generic Location Server Models
#if (defined(CFG_BLE_MESH_MDL_GENS_LOC))
#define BLE_MESH_MDL_GENS_LOC                    (1)
#else
#define BLE_MESH_MDL_GENS_LOC                    (0)
#endif //(BLE_MESH_MDL_GENS_LOC)


/// Support of Generic Power Level Server Models
#if (defined(CFG_BLE_MESH_MDL_GENS_PLVL))
#define BLE_MESH_MDL_GENS_PLVL                    (1)
#else
#define BLE_MESH_MDL_GENS_PLVL                    (0)
#endif //(BLE_MESH_MDL_GENS_PLVL)

/// Support of Generic Power OnOff  Server Models
#if (defined(CFG_BLE_MESH_MDL_GENS_POO))
#define BLE_MESH_MDL_GENS_POO                    (1)
#else
#define BLE_MESH_MDL_GENS_POO                    (0)
#endif //(BLE_MESH_MDL_GENS_POO)

/// Support of Generic User/Admin/Manufacturer/Client Property  Server Models
#if (defined(CFG_BLE_MESH_MDL_GENS_PROP))
#define BLE_MESH_MDL_GENS_PROP                    (1)
#else
#define BLE_MESH_MDL_GENS_PROP                    (0)
#endif //(BLE_MESH_MDL_GENS_PROP)

/// Support of Generic Default Transition Time Server Models
#if (defined(CFG_BLE_MESH_MDL_GENS_DTT))
#define BLE_MESH_MDL_GENS_DTT                       (1)
#else
#define BLE_MESH_MDL_GENS_DTT                       (0)
#endif //(BLE_MESH_MDL_GENS_PROP)



/// Support of Generic Client Models
#if (defined(CFG_BLE_MESH_MDL_GENC))
#define BLE_MESH_MDL_GENC                    (1)
#else
#define BLE_MESH_MDL_GENC                    (0)
#endif //(BLE_MESH_MDL_GENC)

/// Support of Generic Battery Client Models
#if (defined(CFG_BLE_MESH_MDL_GENC_OO))
#define BLE_MESH_MDL_GENC_OO                    (1)
#else
#define BLE_MESH_MDL_GENC_OO                    (0)
#endif //(BLE_MESH_MDL_GENS_OO)


/// Support of Generic Battery Client Models
#if (defined(CFG_BLE_MESH_MDL_GENC_BAT))
#define BLE_MESH_MDL_GENC_BAT                    (1)
#else
#define BLE_MESH_MDL_GENC_BAT                    (0)
#endif //(BLE_MESH_MDL_GENC_BAT)


/// Support of Generic Level Client Models
#if (defined(CFG_BLE_MESH_MDL_GENC_LVL))
#define BLE_MESH_MDL_GENC_LVL                    (1)
#else
#define BLE_MESH_MDL_GENC_LVL                    (0)
#endif //(BLE_MESH_MDL_GENC_LVL)


/// Support of Generic Location Client Models
#if (defined(CFG_BLE_MESH_MDL_GENC_LOC))
#define BLE_MESH_MDL_GENC_LOC                    (1)
#else
#define BLE_MESH_MDL_GENC_LOC                    (0)
#endif //(BLE_MESH_MDL_GENC_LOC)


/// Support of Generic Power Level Client Models
#if (defined(CFG_BLE_MESH_MDL_GENC_PLVL))
#define BLE_MESH_MDL_GENC_PLVL                    (1)
#else
#define BLE_MESH_MDL_GENC_PLVL                    (0)
#endif //(BLE_MESH_MDL_GENC_PLVL)

/// Support of Generic Power OnOff  Client Models
#if (defined(CFG_BLE_MESH_MDL_GENC_POO))
#define BLE_MESH_MDL_GENC_POO                    (1)
#else
#define BLE_MESH_MDL_GENC_POO                    (0)
#endif //(BLE_MESH_MDL_GENC_POO)

/// Support of Generic User/Admin/Manufacturer/Client Property  Client Models
#if (defined(CFG_BLE_MESH_MDL_GENC_PROP))
#define BLE_MESH_MDL_GENC_PROP                    (1)
#else
#define BLE_MESH_MDL_GENC_PROP                    (0)
#endif //(BLE_MESH_MDL_GENC_PROP)

/// Support of Generic Default Transition Time Client Models
#if (defined(CFG_BLE_MESH_MDL_GENC_DTT))
#define BLE_MESH_MDL_GENC_DTT                       (1)
#else
#define BLE_MESH_MDL_GENC_DTT                       (0)
#endif //(BLE_MESH_MDL_GENC_PROP)



/// Support of Sensor Server Models
#if (defined(CFG_BLE_MESH_MDL_SENSS))
#define BLE_MESH_MDL_SENSS                   (1)
#else
#define BLE_MESH_MDL_SENSS                   (0)
#endif //(BLE_MESH_MDL_SENSS)

/// Support of Sensor Client Models
#if (defined(CFG_BLE_MESH_MDL_SENSC))
#define BLE_MESH_MDL_SENSC                   (1)
#else
#define BLE_MESH_MDL_SENSC                   (0)
#endif //(BLE_MESH_MDL_SENSC)

/// Support of Time and Scenes Server Models
#if (defined(CFG_BLE_MESH_MDL_TSCNS))
#define BLE_MESH_MDL_TSCNS                   (1)
#else
#define BLE_MESH_MDL_TSCNS                   (0)
#endif //(BLE_MESH_MDL_TSCNS)

/// Support of Time and Scenes Client Models
#if (defined(CFG_BLE_MESH_MDL_TSCNC))
#define BLE_MESH_MDL_TSCNC                   (1)
#else
#define BLE_MESH_MDL_TSCNC                   (0)
#endif //(BLE_MESH_MDL_TSCNC)

/// Support of Light Server Models
#if (defined(CFG_BLE_MESH_MDL_LIGHTS))
#define BLE_MESH_MDL_LIGHTS                  (1)
#else
#define BLE_MESH_MDL_LIGHTS                  (0)
#endif //(BLE_MESH_MDL_LIGHTS)

/// Support of Light Ln Server Models
#if (defined(CFG_BLE_MESH_MDL_LIGNTS_LN))
#define BLE_MESH_MDL_LIGHTS_LN                    (1)
#else
#define BLE_MESH_MDL_LIGHTS_LN                    (0)
#endif //(BLE_MESH_MDL_LIGHTS_LN)

/// Support of Light CTL Server Models
#if (defined(CFG_BLE_MESH_MDL_LIGNTS_CTL))
#define BLE_MESH_MDL_LIGHTS_CTL                    (1)
#else
#define BLE_MESH_MDL_LIGHTS_CTL                    (0)
#endif //(BLE_MESH_MDL_LIGHTS_CTL)

/// Support of Light HSL Server Models
#if (defined(CFG_BLE_MESH_MDL_LIGNTS_HSL))
#define BLE_MESH_MDL_LIGHTS_HSL                    (1)
#else
#define BLE_MESH_MDL_LIGHTS_HSL                   (0)
#endif //(BLE_MESH_MDL_LIGHTS_HSL)



/// Support of Light Client Models
#if (defined(CFG_BLE_MESH_MDL_LIGHTC))
#define BLE_MESH_MDL_LIGHTC                  (1)
#else
#define BLE_MESH_MDL_LIGHTC                  (0)
#endif //(BLE_MESH_MDL_LIGHTS)


/// Support of Light Ln Client Models
#if (defined(CFG_BLE_MESH_MDL_LIGNTC_LN))
#define BLE_MESH_MDL_LIGHTC_LN                    (1)
#else
#define BLE_MESH_MDL_LIGHTC_LN                    (0)
#endif //(BLE_MESH_MDL_LIGHTC_LN)

/// Support of Light CTL Client Models
#if (defined(CFG_BLE_MESH_MDL_LIGNTC_CTL))
#define BLE_MESH_MDL_LIGHTC_CTL                    (1)
#else
#define BLE_MESH_MDL_LIGHTC_CTL                    (0)
#endif //(BLE_MESH_MDL_LIGHTC_CTL)

/// Support of Light HSL Client Models
#if (defined(CFG_BLE_MESH_MDL_LIGNTC_HSL))
#define BLE_MESH_MDL_LIGHTC_HSL                    (1)
#else
#define BLE_MESH_MDL_LIGHTC_HSL                   (0)
#endif //(BLE_MESH_MDL_LIGHTC_HSL)

/// Support of Light XYL Client Models
#if (defined(CFG_BLE_MESH_MDL_LIGNTC_XYL))
#define BLE_MESH_MDL_LIGHTC_XYL                    (1)
#else
#define BLE_MESH_MDL_LIGHTC_XYL                   (0)
#endif //(BLE_MESH_MDL_LIGHTC_XYL)

/*
 * MESH PROFILE BLOCK CONFIGURATION
 ****************************************************************************************
 */

/// Maximum number of subnets the node can belong to
#define MESH_SUBNET_NB_MAX                   (5)
/// Maximum number of models that can be registered on the node
/// Shall be at least 2 (for Configuration Server Model and Health Server Model)
#define MESH_MODEL_NB_MAX                    (30)

/// Maximum number of buffer block that can be allocated by the Buffer Manager
#define MESH_BUF_BLOCK_NB_MAX                (2)
/// Size of data part of "long" buffers
/// Value must be a multiple of 4
#define MESH_BUF_LONG_SIZE                   (152)
/// Size of data part of "small" buffers
/// Value must be a multiple of 4
#define MESH_BUF_SMALL_SIZE                  (32)
#if (BLE_MESH_FRIEND)
/// Number of "small" buffer to allocate during mesh stack initialization
#define MESH_BUF_INIT_LONG_NB                (8)
/// Number of "long" buffer to allocate during mesh stack initialization
#define MESH_BUF_INIT_SMALL_NB               (20)
#else
/// Number of "small" buffer to allocate during mesh stack initialization
#define MESH_BUF_INIT_LONG_NB                (4)
/// Number of "long" buffer to allocate during mesh stack initialization
#define MESH_BUF_INIT_SMALL_NB               (20)
// The remaining threshold value
#define MESH_BUF_INIT_SMALL_REMAIN_THRESHOLD (MESH_BUF_INIT_SMALL_NB / 3)
#endif //(BLE_MESH_FRIEND)

/// Maximum number of buffers that can be dynamically allocated
#define MESH_BUF_DYN_NB_MAX                  (100)

/*
 * MESH MODEL BLOCK CONFIGURATION
 ****************************************************************************************
 */

#define MESH_MDL_DFLT_NB_REPLAY_ELMT         (5)

/// @} MESH_CONFIG

#endif /* MESH_CONFIG_ */
