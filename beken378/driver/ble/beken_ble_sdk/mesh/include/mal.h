/**
 ****************************************************************************************
 *
 * @file mal.h
 *
 * @brief Header file for Mesh Abstraction Layer
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef MAL_
#define MAL_

/**
 ****************************************************************************************
 * @defgroup MAL Mesh Abstraction Layer
 * @ingroup MESH
 * @brief  Mesh Abstraction Layer
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mal_lib.h"           // Mesh Abstraction Layer Library
#include "mesh_config.h"       // Mesh Stack Configuration
#include "mesh_defines.h"      // Mesh Stack Defines
#include "kernel_msg.h"            // Kernel Messages Definitions

/*
 * DEFINES
 ****************************************************************************************
 */

/// Minimum BLE interval in slots
#define MAL_BLE_INTERVAL_MIN       (6)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Mesh message status after execution of a message handler
enum mal_msg_status
{
    /// Message can be freed
    MAL_MSG_FREE = 0,
    /// Message cannot be freed
    MAL_MSG_NO_FREE,
};

/// type of supported connections
enum mal_con_type
{
    /// No connection type specified
    MAL_CON_NONE = 0,
    /// Proxy connection type
    MAL_CON_PROXY,
    /// Provisioning connection type
    MAL_CON_PROV,
    // Customer connection type
    MAL_CON_CUSTOM,
};
/*
 * MACROS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * Allocate a message to send to application with a fixed length
 *
 * @param[in] msg_id Output message identifier
 * @param[in] type   Message parameter type
 *
 * @return Pointer to the message parameter to fill
 ****************************************************************************************
 */
#define MAL_MSG_ALLOC(msg_id, type)            ((type *)mal_msg_alloc((msg_id), sizeof(type)))

/**
 ****************************************************************************************
 * Allocate a message with no parameter
 *
 * @param[in] msg_id Output message identifier
 *
 * @return Pointer to the message parameter to fill
 ****************************************************************************************
 */
#define MAL_MSG_ALLOC_DEFAULT(msg_id)          (mal_msg_alloc((msg_id), 0))

/**
 ****************************************************************************************
 * Allocate a message to send to application with a dynamic length
 *
 * @param[in] msg_id Output message identifier
 * @param[in] type   Message parameter type
 * @param[in] len    Size to append to default type size
 *
 * @return Pointer to the message parameter to fill
 ****************************************************************************************
 */
#define MAL_MSG_ALLOC_DYN(msg_id, type, len)  ((type *)mal_msg_alloc((msg_id), sizeof(type) + len))

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Mesh start-up configuration - @see m_api.h
typedef struct m_cfg m_cfg_t;

/// Advertising callback set
typedef struct mal_adv_cb mal_adv_cb_t;
/// Connection callback set
typedef struct mal_con_cb mal_con_cb_t;

/**
 ****************************************************************************************
 * @brief Prototype of a function used for Mesh API message handler
 *
 * @param[in] msg_id  Message identifier
 * @param[in] src_id  Identifier of the task that issue message
 * @param[in] p_param Parameters of the message received
 *
 * @return status of message after execution of the handler (@see enum mal_msg_status)
 ****************************************************************************************
 */
typedef uint8_t (*mal_msg_handler) (uint16_t msg_id, uint16_t src_id, const void *p_param);


/**
 ****************************************************************************************
 * @brief Call back definition of the function that can handle result of an AES based algorithm
 *
 * @param[in] p_aes_res  16 bytes block result
 ****************************************************************************************
 */
typedef void (*mal_sec_aes_res_cb) (const uint8_t *p_aes_res);

/**
 ****************************************************************************************
 * @brief Call back definition of the function that can handle result of ECDH Secret computation algorithm
 *
 * @param[in] status         Status of the ECDH Secret computation
 * @param[in] p_ecdh_secret  32 bytes (LSB) ECDH Secret value
 ****************************************************************************************
 */
typedef void (*mal_sec_ecdh_secret_res_cb) (uint16_t status, const uint8_t *p_ecdh_secret);

/**
 ****************************************************************************************
 * @brief Call back definition of the function that can handle read public key value
 *
 * @param[in] status         Status of the public key reading
 * @param[in] p_pub_key_x    32 bytes (LSB) X coordinate of public key
 * @param[in] p_pub_key_y    32 bytes (LSB) Y coordinate of public key
 ****************************************************************************************
 */
typedef void (*mal_sec_pub_key_res_cb) (uint16_t status, const uint8_t *p_pub_key_x, const uint8_t *p_pub_key_y);

/**
 ****************************************************************************************
 * @brief Call back definition of the function that can handle result of AES-CCM Cipher/Decipher
 *
 * @param[in] mic_error  True if a MIC error detected when Decipher, False else
 *                       In case of MIC error output message is considered invalid
 ****************************************************************************************
 */
typedef void (*mal_sec_aes_ccm_cb) (bool mic_error);

/**
 ****************************************************************************************
 * @brief Definition of function called when advertising packet has been properly sent
 *
 * @param[in] bearer_lid    Bearer LID
 * @param[in] status        Execution status code (@see enum m_err)
 ****************************************************************************************
 */
typedef void (*mal_adv_sent_cb) (m_lid_t bearer_lid, uint16_t status);


/**
 ****************************************************************************************
 * @brief Definition of function called when an advertising packet has been received
 *
 * @param[in] bearer_lid    Bearer LID
 * @param[in] status        Execution status code (@see enum m_err)
 * @param[in] rssi          RSSI value of the received ADV report
 * @param[in] adv_len       Length of ADV report packet (without source address)
 * @param[in] p_data        Advertising data payload.
 *
 ****************************************************************************************
 */
typedef void (*mal_adv_rx_cb) (m_lid_t bearer_lid, uint16_t status, int8_t rssi, uint8_t adv_len, const uint8_t *p_data);

/**
 ****************************************************************************************
 * @brief Definition of function called when scanning is either started or stopped
 *
 * @param[in] bearer_lid    Bearer LID
 * @param[in] status        Execution status code (@see enum m_err)
 ****************************************************************************************
 */
typedef void (*mal_adv_scan_op_cb) (m_lid_t bearer_lid, uint16_t status);


/**
 ****************************************************************************************
 * @brief Start Scanning operation, Scanning is performed with a duty cycle of 100%
 *
 * @note: start_cb executed at end of scan start procedure
 * @note: rx_cb executed if an advertising report is received
 * @note: end_cb executed if scan is aborted
 *
 * @param[in] bearer_lid    Bearer LID
 * @param[in] interval      Scan window interval (in slots)
 * @param[in] p_cb_evts     Set of callback executed when an event occurs on advertising bearer
 *
 * @return Error status
 ****************************************************************************************
 */
typedef uint16_t (*mal_adv_scan_start_api) (m_lid_t bearer_lid, uint16_t interval, const mal_adv_cb_t *p_cb_evts);
/**
 ****************************************************************************************
 * @brief Stop Scanning operation
 *
 * @note: end_cb executed if scan is properly stopped
 *
 * @param[in] bearer_lid    Bearer LID
 *
 * @return Error status
 ****************************************************************************************
 */
typedef uint16_t (*mal_adv_scan_stop_api) (m_lid_t bearer_lid);

/**
 ****************************************************************************************
 * @brief Definition of function to be used to send an advertising packet
 *
 * @note: sent_cb executed when send operation is performed or if an error occurs
 *
 * @param[in] bearer_lid    Bearer LID
 * @param[in] interval      Packet transmission interval (in slots)
 * @param[in] nb_tx         Number of packet transmission to perform
 * @param[in] adv_len       Length of data to transmit
 * @param[in] p_data        Pointer to the data to transmit
 *
 * @return Error status
 ****************************************************************************************
 */
typedef uint16_t (*mal_adv_send_api) (m_lid_t bearer_lid, uint16_t interval, uint8_t nb_tx,
                                      uint16_t adv_len, const uint8_t *p_data, uint8_t msg_type);


#if (BLE_MESH_GATT_BEARER)
/**
 ****************************************************************************************
 * @brief Start being connectable (using advertising)
 *
 * @note: start_cb   executed at end connection start operation
 * @note: created_cb executed when a connection is established
 * @note: closed_cb  executed when a connection is closed
 * @note: end_cb     executed when advertisement is stopped
 * @note: rx_cb      executed when valid proxy or provisionning data is received
 *
 * @param[in] con_type      Type of connection (@see enum mal_con_type)
 * @param[in] interval      Advertising interval
 * @param[in] data_len      Advertising data length
 * @param[in] p_data        Advertising data used in connectable advertising
 * @param[in] p_cb_evts     Set of callback executed when an event occurs on connection bearer
 *
 * @return Error status
 ****************************************************************************************
 */
typedef uint16_t (*mal_con_start_api) (uint8_t con_type, uint16_t interval, uint16_t data_len, const uint8_t *p_data,
                                       const mal_con_cb_t *p_cb_evts);

/**
 ****************************************************************************************
 * @brief Stop connectable advertising
 *
 * @note: enc_cb executed at end connection stop operation
 *
 * @return Error status
 ****************************************************************************************
 */
typedef uint16_t (*mal_con_stop_api) (void);

/**
 ****************************************************************************************
 * @brief Update advertising data used for connectable mode
 *
 * @param[in] data_len      Advertising data length
 * @param[in] p_data        Advertising data used in connectable advertising
 *
 * @return Error status
 ****************************************************************************************
 */
typedef uint16_t (*mal_con_update_api) (uint16_t data_len, const uint8_t *p_data);

/**
 ****************************************************************************************
 * @brief Close a specific bearer identifier
 *
 * @note: closed_cb executed when a connection is closed
 *
 * @param[in] bearer_lid    Bearer LID
 *
 * @return Error status
 ****************************************************************************************
 */
typedef uint16_t (*mal_con_close_api) (m_lid_t bearer_lid);

/**
 ****************************************************************************************
 * @brief Definition of function to be used to send provisioning or proxy PDUs
 *
 * @note: sent_cb executed when send operation is performed or if an error occurs
 *
 * @param[in] bearer_lid    Bearer LID
 * @param[in] type          Type of GATT packet received (proxy or provisioning data)
 * @param[in] data_len      Length of data to transmit
 * @param[in] p_data        Pointer to the data to transmit
 *
 * @return Error status
 ****************************************************************************************
 */
typedef uint16_t (*mal_con_send_api) (m_lid_t bearer_lid, uint8_t type, uint16_t data_len, const uint8_t *p_data);
#endif // (BLE_MESH_GATT_BEARER)

/**
 ****************************************************************************************
 * @brief Definition of function called when connection is either started or stopped
 *
 * @param[in] status        Execution status code (@see enum m_err)
 ****************************************************************************************
 */
typedef void (*mal_con_op_cb) (uint16_t status);

/**
 ****************************************************************************************
 * @brief Callback of connection creation.
 *
 * @param[in] bearer_lid    Bearer LID
 * @return Bearer local identifier associated with connection
 ****************************************************************************************
 */
typedef m_lid_t (*mal_con_created_cb)(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Callback of connection closed.
 *
 * @param[in] bearer_lid    Bearer LID
 * @param[in] reason        Disconnection reason.
 ****************************************************************************************
 */
typedef void (*mal_con_closed_cb)(m_lid_t bearer_lid, uint8_t reason);

/**
 ****************************************************************************************
 * @brief Callback of connection message reception
 *
 * @param[in] bearer_lid    Bearer LID
 * @param[in] type          Type of GATT packet received (proxy or provisioning data)
 * @param[in] data_len      Length of the data received
 * @param[in] p_data        Pointer to the data received
 ****************************************************************************************
 */
typedef void (*mal_con_msg_rx_cb)(m_lid_t bearer_lid, uint8_t type, uint8_t data_len, const uint8_t *p_data);

/**
 ****************************************************************************************
 * @brief Callback of GATT (proxy or provisioning data) message sent
 *
 * @param[in] bearer_lid    Bearer LID
 * @param[in] status        Status of the transmission performed
 ****************************************************************************************
 */
typedef void (*mal_con_msg_sent_cb)(m_lid_t bearer_lid, uint16_t status);

/**
 ****************************************************************************************
 * @brief Job function to called into a background context
 *
 * @param[in] p_env   Pointer to environment that will be used as callback parameter.
 ****************************************************************************************
 */
typedef void (*mal_djob_cb)(void *p_env);

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// Advertising callback set
typedef struct mal_adv_cb
{
    /// Scan started status
    mal_adv_scan_op_cb start_cb;
    /// Scan stopped status
    mal_adv_scan_op_cb stop_cb;
    /// Advertising report reception
    mal_adv_rx_cb      rx_cb;
    /// Advertising packet properly sent
    mal_adv_sent_cb    sent_cb;
} mal_adv_cb_t;


/// Structure for Advertising Abstraction Layer API
typedef struct mal_adv_api_
{
    /// Start scanning operation
    mal_adv_scan_start_api  scan_start;
    /// Stop scanning operation
    mal_adv_scan_stop_api   scan_stop;
    /// Send Advertising packet function
    mal_adv_send_api        send;
} mal_adv_api_t;


#if (BLE_MESH_GATT_BEARER)
/// Structure for Connection Abstraction Layer API
typedef struct mal_con_api_
{
    /// Start connection bearer
    mal_con_start_api  con_start;
    /// Stop all connectable bearer
    mal_con_stop_api   con_stop;
    /// Close a specific connection
    mal_con_close_api  con_close;
    /// Send packet over connection
    mal_con_send_api   send;
} mal_con_api_t;
#endif // (BLE_MESH_GATT_BEARER)

/// Connection callback set
typedef struct mal_con_cb
{
    /// Connection operation started status
    mal_con_op_cb        start_cb;
    /// Connection operation stopped status
    mal_con_op_cb        stop_cb;
    /// Connection Creation
    mal_con_created_cb   created_cb;
    /// Connection Deleted
    mal_con_closed_cb    closed_cb;
    /// Message received over proxy or provisioning service
    mal_con_msg_rx_cb    rx_cb;
    /// Message sent over proxy or provisioning service
    mal_con_msg_sent_cb  sent_cb;
} mal_con_cb_t;


/// Job element structure
typedef struct mal_djob
{
    /// List element header
    common_list_hdr_t hdr;
    /// Pointer to environment that will be used as callback parameter.
    void         *p_env;
    /// Callback to execute in background context
    mal_djob_cb  cb;
} mal_djob_t;

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

/// Advertising API
extern const mal_adv_api_t mal_adv_api;

#if (BLE_MESH_GATT_BEARER)
/// Connection API
extern const mal_con_api_t mal_con_api;
#endif //(BLE_MESH_GATT_BEARER)

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/* ************************************************************************************ *
 *                FUNCTION FROM ABSTRACTION LAYER CALLED BY MESH STACK                  *
 * ************************************************************************************ */

/**
 ****************************************************************************************
 * @brief Allocate a message to send to application
 *
 * @param[in] msg_id    Output message identifier
 * @param[in] param_len Message parameter length
 *
 * @return Pointer to the message parameter to fill
 ****************************************************************************************
 */
void *mal_msg_alloc(uint16_t msg_id, uint16_t param_len);

/**
 ****************************************************************************************
 * @brief Free a message received from application
 *
 * @param[in] p_msg Message parameter
 ****************************************************************************************
 */
void mal_msg_free(const void *p_msg);

/**
 ****************************************************************************************
 * @brief Send a message to destination application
 *
 * @param[in] dst_id    Destination task identifier
 * @param[in] p_msg     Message parameter
 ****************************************************************************************
 */
void mal_msg_send(uint16_t dst_id, const void *p_msg);

/**
 ****************************************************************************************
 * @brief Return application task ID for communications with application
 ****************************************************************************************
 */
kernel_task_id_t mal_app_id_get(void);

/**
 ****************************************************************************************
 * @brief Send a message to destination application
 *
 * @param[in] p_msg Message parameter
 *
 * @return source identifier of task that issue message
 ****************************************************************************************
 */
uint16_t mal_msg_src_id_get(const void *p_msg);

/**
 ****************************************************************************************
 * @brief Allocation of a block of memory.
 *
 * Allocates a memory block whose size is size; if no memory is available return NULL
 *
 * @param[in] size Size of the memory area that need to be allocated.
 *
 * @return A pointer to the allocated memory area.
 ****************************************************************************************
 */
void *mal_malloc(uint16_t size);

/**
 ****************************************************************************************
 * @brief Freeing of a block of memory.
 *
 * Free the memory area pointed by mem_ptr
 *
 * @param[in] mem_ptr Pointer to the memory area that need to be freed.
 *
 ****************************************************************************************
 */
void mal_free(void *p_ptr);

/*
 ****************************************************************************************
 * Security functions
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Perform an AES encryption
 *
 * @note Pointer provided to the function must stay valid till result call-back is called.
 *       Security Block does not copy pointer value into a cache
 *
 * @param[in] p_key    Key used for the encryption
 * @param[in] p_val    Value to encrypt using AES
 * @param[in] res_cb   Function that will handle the AES based result (16 bytes)
 ****************************************************************************************
 */
void mal_sec_aes(const uint8_t *p_key, const uint8_t *p_val, mal_sec_aes_res_cb res_cb);

/**
 ****************************************************************************************
 * @brief Generate a 128bits random number using AES encryption
 *
 * @param[in] res_cb   Function that will handle the AES based result (16 bytes)
 ****************************************************************************************
 */
void mal_sec_rand(mal_sec_aes_res_cb res_cb);

/**
 ****************************************************************************************
 * @brief Execute the AES CMAC crypto function.
 *
 * @note Pointer provided to the function must stay valid till result call-back is called.
 *       Security Block does not copy pointer value into a cache
 *
 * @param[in] p_key        Pointer to the Key to be used
 * @param[in] p_message    Pointer to the block of data the data on which the CMAC is performed
 * @param[in] message_len  Length (in bytes) of the block of data M
 * @param[in] res_cb       Function that will handle the AES based result (16 bytes)
 ****************************************************************************************
 */
void mal_sec_aes_cmac(const uint8_t *p_key, const uint8_t *p_message, uint16_t message_len, mal_sec_aes_res_cb res_cb);

/**
 ****************************************************************************************
 * @brief Execute the AES-S1 crypto function.
 *
 * @param[in] p_message    Message used to generate Salted key
 * @param[in] message_len  Length (in bytes) of the block of data M
 * @param[in] res_cb       Function that will handle the AES based result (16 bytes)
 ****************************************************************************************
 */
void mal_sec_aes_s1(const uint8_t *p_message, uint8_t message_len, mal_sec_aes_res_cb res_cb);

/**
 ****************************************************************************************
 * @brief Start the AES-K1 crypto function.
 *
 * @param[in] p_salt       Pointer to Salted Key to use
 * @param[in] p_n          Pointer to value of N
 * @param[in] n_len        Length of N
 * @param[in] p_p          Pointer to value of P
 * @param[in] p_len        Length of P
 * @param[in] res_cb       Function that will handle the AES based result (16 bytes)
 ****************************************************************************************
 */
void mal_sec_aes_k1(const uint8_t *p_salt, const uint8_t *p_n, uint8_t n_len, const uint8_t *p_p, uint8_t p_len,
                    mal_sec_aes_res_cb res_cb);

/**
 ****************************************************************************************
 * @brief Start the AES K2 crypto function.
 *
 * @param[in] p_n          Pointer to value of N
 * @param[in] p_p          Pointer to value of P
 * @param[in] p_len        Length of P
 * @param[in] res_cb       Function that will handle the AES based result (33 bytes)
 ****************************************************************************************
 */
void mal_sec_aes_k2(const uint8_t *p_n, const uint8_t *p_p, uint8_t p_len, mal_sec_aes_res_cb res_cb);


/**
 ****************************************************************************************
 * @brief Start the AES K3 crypto function.
 *
 * @param[in] p_n          Pointer to value of N - 128 bits
 * @param[in] res_cb       Function that will handle the AES based result (8 bytes)
 ****************************************************************************************
 */
void mal_sec_aes_k3(const uint8_t *p_n, mal_sec_aes_res_cb res_cb);

/**
 ****************************************************************************************
 * @brief Start the AES K4 crypto function.
 *
 * @param[in] p_n          Pointer to value of N- 128 bits
 * @param[in] res_cb       Function that will handle the AES based result (1 byte)
 ****************************************************************************************
 */
void mal_sec_aes_k4(const uint8_t *p_n, mal_sec_aes_res_cb res_cb);


/**
 ****************************************************************************************
 * @brief Start the AES CCM crypto function. Allocate memory for the CCM and start processing it
 *        Execute result callback at end of function execution
 *
 * @note Pointer provided to the function must stay valid till result call-back is called.
 *       Security Block does not copy pointer value into a cache
 *
 * @param[in]  p_key             Pointer to the Key to be used
 * @param[in]  p_nonce           13 Bytes Nonce to use for cipher/decipher
 * @param[in]  p_in_message      Input message for AES-CCM exectuion
 * @param[out] p_out_message     Output message that will contain cipher+mic or decipher data
 * @param[in]  message_len       Length of Input/Output message without mic
 * @param[in]  mic_len           Length of the mic to use (2, 4, 6, 8, 10, 12, 14, 16 valid)
 * @param[in]  cipher            True to encrypt message, False to decrypt it.
 * @param[in]  p_add_auth_data   Additional Authentication data used for computation of MIC
 * @param[in]  add_auth_data_len Length of Additional Authentication data
 * @param[in]  res_cb            Function that will handle the AES CCM result
 ****************************************************************************************
 */
void mal_sec_aes_ccm(const uint8_t *p_key, const uint8_t *p_nonce, const uint8_t *p_in_message,
                     uint8_t *p_out_message, uint16_t message_len, uint8_t mic_len, bool cipher,
                     const uint8_t *p_add_auth_data, uint8_t add_auth_data_len, mal_sec_aes_ccm_cb res_cb);


/**
 ****************************************************************************************
 * @brief Read the Public key used for ECDH algorithm
 *
 * @param[in] res_cb  Function that will provide result of Public Key Value
 ****************************************************************************************
 */
void mal_sec_pub_key_read(mal_sec_pub_key_res_cb res_cb);

/**
 ****************************************************************************************
 * @brief Perform computation of the ECDH Secret computation using peer device key
 *
 * @param[in] p_pub_key_x    32 bytes (LSB) X coordinate of public key
 * @param[in] p_pub_key_y    32 bytes (LSB) Y coordinate of public key
 * @param[in] res_cb         Function that will provide result of ECDH Secret computation
 ****************************************************************************************
 */
void mal_sec_ecdh_secret(const uint8_t *p_pub_key_x, const uint8_t *p_pub_key_y, mal_sec_ecdh_secret_res_cb res_cb);

/*
 ****************************************************************************************
 * Timer functions
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve absolute expiration time for a timer to be programmed with a given delay.
 *
 * @param[out] p_time_ms     Pointer to variable where the computed time value will be written (in milliseconds)
 * @param[out] p_nb_wrap     Pointer to variable where the computed wrap number value will be written
 * @param[in] delay_ms       Delay (in milliseconds)
 *
 * @return True if delay value is long enough to compute the requested time in the future,
 * else False
 ****************************************************************************************
 */
bool mal_timer_get_time_ms(uint32_t *p_time_ms, uint16_t *p_nb_wrap, uint32_t delay_ms);

/**
 ****************************************************************************************
 * @brief Get delay value (in milliseconds) to provide to mal_timer_set function in order
 * to program a timer.
 *
 * @param[in] time_ms           Expiration time value (in milliseconds).
 * @param[in] nb_wrap           Wrap value.
 * @param[out] p_delay_ms       Pointer to variable that will contain the computed delay value
 *
 * @return MESH_ERR_NO_ERROR if delay has been properly computed,
 *         MESH_ERR_TIME_PAST if provided expiration time is in the past
 *         MESH_ERR_INVALID_PARAM if provided expiration time is too far in the future to compute
 * a delay value.
 ****************************************************************************************
 */
uint16_t mal_timer_get_delay(uint32_t time_ms, uint16_t nb_wrap, uint32_t *p_delay_ms);

/**
 ****************************************************************************************
 * @brief Set timer expiration time.
 *
 * @param[in] time_ms       Timer expiration time (in milliseconds).
 ****************************************************************************************
 */
void mal_timer_set(uint32_t delay_ms);

/**
 ****************************************************************************************
 * @brief Clear timer.
 ****************************************************************************************
 */
void mal_timer_clear(void);

/**
 ****************************************************************************************
 * @brief Set duration elapsed since device up time in milliseconds.
 *
 * @param[in] clock_ms       Current clock.
 * @param[in] nb_wrap        Number of wrap.
 ****************************************************************************************
 */
void mal_timer_set_clock(uint32_t clock_ms, uint16_t nb_wrap);

/**
 ****************************************************************************************
 * @brief Compute duration elapsed since device up time in milliseconds.
 *
 * @param[out] p_clock_ms       Pointer to variable that will contain the current clock.
 * @param[out] p_nb_wrap        Pointer to variable that will contain the number of wrap.
 ****************************************************************************************
 */
void mal_timer_get_clock(uint32_t *p_clock_ms, uint16_t *p_nb_wrap);

/*
 ****************************************************************************************
 * Delayed Job functions
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Register to execute a job delayed in background
 *
 * @param[in] p_djob   Pointer to the delayed job structure
 ****************************************************************************************
 */
void mal_djob_reg(mal_djob_t *p_djob);

/* ************************************************************************************ *
 *               FUNCTION FROM MESH STACK CALLED BY ABSTRACTION LAYER                   *
 * ************************************************************************************ */

/**
 ****************************************************************************************
 * @brief Schedule the next timer(s).
 * This function pops the first timer from the timer queue and calls the attached
 * callback function. The function checks also the next timers and process them if they
 * have expired or are about to expire.
 ****************************************************************************************
 */
void mesh_tb_timer_expired(void);

/// @} MAL

#endif /* MAL_ */
