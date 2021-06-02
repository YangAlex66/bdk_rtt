/**
 *******************************************************************************
 *
 * @file user_config.h
 *
 * @brief Application configuration definition
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 *******************************************************************************
 */

#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_


/*******************************************************************************
 *#############################################################################*
 *                              MESH MACRO CTRL                         *
 *#############################################################################*
 *******************************************************************************/

// Open the mesh log
#define CONFIG_MESH_LOG_OUTPUT

/* For the Ali Mesh, don't need to used the gatt proxy and gatt provision */
#define CFG_BLE_MESH_GATT_PROXY
#define CFG_BLE_MESH_GATT_PROV
#define CFG_BLE_MESH_RELAY
#undef CFG_BLE_MESH_FRIEND
#undef CFG_BLE_MESH_LPN
#define CFG_BLE_MESH_STORAGE_NVDS

#define CFG_BLE_MESH_MDL_SERVER


#define CFG_BLE_MESH_MDL_GENS
#define CFG_BLE_MESH_MDL_GENS_OO
#undef CFG_BLE_MESH_MDL_GENS_DTT
#undef CFG_BLE_MESH_MDL_GENS_POO
#undef CFG_BLE_MESH_MDL_GENS_PLVL
#undef CFG_BLE_MESH_MDL_GENS_LVL
#undef CFG_BLE_MESH_MDL_GENS_LOC
#undef CFG_BLE_MESH_MDL_GENS_PROP

#define CFG_BLE_MESH_MDL_LIGHTS
#define CFG_BLE_MESH_MDL_LIGNTS_LN
#define CFG_BLE_MESH_MDL_LIGNTS_CTL
#define CFG_BLE_MESH_MDL_LIGNTS_HSL

#undef CFG_BLE_MESH_MDL_CLIENT
#undef CFG_BLE_MESH_MDL_GENC
#undef CFG_BLE_MESH_MDL_LIGHTC

#undef CFG_MESH_MEM_TB_BUF_DBG

/*******************************************************************************
 *#############################################################################*
 *                              APPLICATION MACRO CTRL                         *
 *#############################################################################*
 *******************************************************************************/

//interval min
#define BLE_UAPDATA_MIN_INTVALUE        20
//interval max
#define BLE_UAPDATA_MAX_INTVALUE        40
//latency
#define BLE_UAPDATA_LATENCY             0
//timeout
#define BLE_UAPDATA_TIMEOUT             600

//scan response data
#define APP_SCNRSP_DATA        "\x0e\x08\x42\x4B\x33\x34\x33\x35\x2D\x47\x41\x54\x54\x30\x31" //BK3435-GATT"
#define APP_SCNRSP_DATA_LEN     (15)

/// Advertising channel map - 37, 38, 39
#define APP_ADV_CHMAP           (0x07)
/// Advertising minimum interval - 100ms (160*0.625ms)
#define APP_ADV_INT_MIN         (80)
/// Advertising maximum interval - 100ms (160*0.625ms)
#define APP_ADV_INT_MAX         (80)
/// Fast advertising interval
#define APP_ADV_FAST_INT        (32)


#define UNPROV_CONN_EN           (1)
#define PROXY_KEEP_CONN_EN       (1)
#define UNPROV_TIMEOUT_ADV       (1)
#define POWER_ONOFF_ONLY         (0)


/*******************************************************************************
 *#############################################################################*
 *                              DRIVER MACRO CTRL                              *
 *#############################################################################*
 ******************************************************************************/

//DRIVER CONFIG
#define UART_DRIVER                 1
#define GPIO_DRIVER                 1
#define AUDIO_DRIVER                    0
#define RTC_DRIVER                      0
#define ADC_DRIVER                      0
#define I2C_DRIVER                      0
#define PWM_DRIVER                      1


#define MAC78da07bcd71b 1   // big
#define MAC78da07bcd71c 0
#define MAC78da07bcd71d 0   //small
#define MACf8a76324a49e  0
#define MACf8a76324a49f  0


#endif /* _USER_CONFIG_H_ */
