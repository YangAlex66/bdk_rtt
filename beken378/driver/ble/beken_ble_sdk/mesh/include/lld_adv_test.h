/**
 ****************************************************************************************
 *
 * @file lld_adv_test.h
 *
 * @brief Declaration of functions used for advertising test mode driver
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef LLD_ADV_TEST_H_
#define LLD_ADV_TEST_H_

/**
 ****************************************************************************************
 * @addtogroup LLDADVTEST
 * @ingroup LLD
 * @brief Advertising test mode driver
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_EMB_PRESENT && BLE_ADV_TEST_MODE)
#include "ea.h"
#include "common_bt.h"

/*
 * MACROS
 ****************************************************************************************
 */
typedef void (*MAL_ADV_REPORT_CB_T)(const struct adv_report* p_report);
/*
 * DEFINES
 ****************************************************************************************
 */
/// Unique tag that represents ADV Test mode driver data
#define BLE_ADV_TEST_MODE_ELT    (0xADA55ADA)

/// Unique tag that represents SACN Test mode driver data
#define BLE_SCAN_TEST_MODE_ELT    (0xDA5AA5AD)

/// Allows an adv interval of 10ms
#define BLE_ADV_TEST_INTERVAL_MIN (0x10)
/*
 * MACROS
 ****************************************************************************************
 */
#define BLE_ADV_TEST_DUPLICATE_FILTER_MAX  10
/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Start Advertising test mode
 *
 * @param[in] interval         Interval of the advertising (slots unit)
 * @param[in] chmap            Advertising channel map
 * @param[in] nb_tx            Number of transmission per advertising channel to perform
 * @param[in] addr             BD Address to transmit (if zero, use device public address)
 * @param[in] data_len         Length of advertising data.
 * @param[in] data             Advertising data.
 * @param[in] msg_id           Message Identifier to trigger when requested advertising is over
 * @param[in] dst_id           Task Identifier that will handle end message
 * @param[in] non_con_act_en   Used to know if a non-connected activity is on-going
 *
 * @return COMMON_ERROR_NO_ERROR if succeed else see error code.
 ****************************************************************************************
 */
uint8_t lld_adv_test_start(uint16_t interval, uint8_t chmap, uint8_t nb_tx, const uint8_t *addr,
                           uint8_t data_len, const uint8_t *data, uint16_t msg_id, uint16_t dst_id,
                           bool non_con_act_en, uint8_t msg_type);

/**
 ****************************************************************************************
 * @brief Start Scanning test mode
 *
 * @param[in] win_size  Maximum reception window size before changing channel - in slots
 * @param[in] msg_id    Message Identifier to trigger when advertising report is received
 * @param[in] dst_id    Task Identifier that will handle end message
 *
 * @return COMMON_ERROR_NO_ERROR if succeed else see error code.
 ****************************************************************************************
 */
uint8_t lld_adv_test_scan_start(uint16_t win_size, uint16_t msg_id, uint16_t dst_id);

/**
 ****************************************************************************************
 * @brief Stop Scanning test mode
 *
 * @param[in] msg_id    Message Identifier to trigger when scanning driver is stopped
 *
 * @return COMMON_ERROR_NO_ERROR if succeed else see error code.
 ****************************************************************************************
 */
uint8_t lld_adv_test_scan_stop(uint16_t msg_id);

/**
 ****************************************************************************************
 * @brief End of Advertising test mode event (driver internal use)
 *
 * @param[in] elt  Event arbiter information
 *
 ****************************************************************************************
 */
void lld_adv_test_end_cb(struct ea_elt_tag *elt);

/**
 ****************************************************************************************
 * @brief RX ISR Callback - triggered when an RX descriptor is used  (driver internal use)
 *
 * @param[in] elt  Event arbiter information
 *
 ****************************************************************************************
 */
void lld_adv_test_rx_isr_cb(struct ea_elt_tag *elt);

/**
 ****************************************************************************************
 * @brief Initialize advertising test mode driver
 *
 * @param[in] reset  True if reset False if boot init
 *
 ****************************************************************************************
 */
void lld_adv_test_init(bool reset);

bool lld_test_adv_report_check(uint8_t *adv_data, uint8_t adv_len);

void mal_adv_report_register(MAL_ADV_REPORT_CB_T cb);

bool mal_adv_report_is_register(void);

void mal_adv_report_unregister(void);

void lld_adv_test_enable_rand(void);

void lld_adv_test_is_enable_rand(bool is_enable);
#endif // (BLE_EMB_PRESENT && BLE_ADV_TEST_MODE)

#endif // LLD_ADV_TEST_H_

/// @} LLDADVTEST
