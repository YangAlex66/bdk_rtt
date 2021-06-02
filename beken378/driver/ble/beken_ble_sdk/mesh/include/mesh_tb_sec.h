/**
 ****************************************************************************************
 *
 * @file mesh_tb_sec.h
 *
 * @brief Header file for Mesh Security Manager
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef MESH_TB_SEC_
#define MESH_TB_SEC_

/**
 ****************************************************************************************
 * @defgroup MESH_TB_SEC Mesh Security Manager
 * @ingroup MESH
 * @brief Mesh Security Manager
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh_inc.h"       // Mesh Stack Includes

/*
 * ENUMERATIONS
 ****************************************************************************************
 */
/// Invalid token identifier
#define M_TB_SEC_INVALID_TOKEN_ID  (0xFF)

/// List of security token that can be used
enum mesh_tb_sec_token_id
{
    /// Security token for foundation model layer
    MESH_TB_SEC_TOKEN_FOUND,
    /// Security token for upper transport layer (encryption)
    MESH_TB_SEC_TOKEN_UTRANS_ENC,
    /// Security token for upper transport layer (decryption)
    MESH_TB_SEC_TOKEN_UTRANS_DEC,
    /// Security token for network layer
    MESH_TB_SEC_TOKEN_NETWORK,
    /// Security token for key manager
    MESH_TB_SEC_TOKEN_KEY_MANAGER,
    /// Security token for provisioning layer
    MESH_TB_SEC_TOKEN_PROVISIONING,
    /// Security token for beaconing layer
    MESH_TB_SEC_TOKEN_BEACONING,

#if (BLE_MESH_GATT_PROXY)
    /// Security token for GATT Proxy
    MESH_TB_SEC_TOKEN_PROXY,
#endif //(BLE_MESH_GATT_PROXY)
#if (BLE_MESH_DBG)
    /// Specific token for debug purpose
    MESH_TB_SEC_TOKEN_DBG,
#endif //(BLE_MESH_DBG)

    /// Number of token in use
    MESH_TB_SEC_TOKEN_MAX,
};

/*
 * CALLBACK DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * Callback executed when security request has been processed
 *
 * @param[in] token_id  Token identifier of security function which is over
 * @param[in] status    Execution status (@see enum m_error)
 * @param[in] p_enc_val Pointer to value where result of the AES based function is returned
 *                      (@note length is variable and depends on security function used)
 *                      (@note equals NULL for AES-CCM encrypt and decrypt functions)
 ****************************************************************************************
 */
typedef void (*mesh_tb_sec_finished_cb)(uint8_t token_id, uint16_t status, const uint8_t *p_auth_val);

/**
 ****************************************************************************************
 * Callback executed when AES function is ready to be executed
 *
 * @param[in]  token_id     Token identifier of security function
 * @param[out] pp_key       Pointer to key to use for authenticating message
 * @param[out] pp_val       Pointer to Value to encrypt (128 bits)
 *
 * @return Status error code (@see enum m_error)
 ****************************************************************************************
 */
typedef uint16_t (*mesh_tb_sec_aes_ready_cb)(uint8_t token_id, const uint8_t **pp_key, const uint8_t **pp_val);

/**
 ****************************************************************************************
 * Callback executed when AES-CMAC function is ready to be executed
 *
 * @param[in]  token_id     Token identifier of security function
 * @param[out] pp_key       Pointer to key to use for authenticating message
 * @param[out] pp_msg       Pointer to Message to authenticate
 * @param[out] p_msg_len    Pointer to Length of the message
 *
 * @return Status error code (@see enum m_error)
 ****************************************************************************************
 */
typedef uint16_t (*mesh_tb_sec_cmac_ready_cb)(uint8_t token_id, const uint8_t **pp_key, const uint8_t **pp_msg,
        uint16_t *p_msg_len);

/**
 ****************************************************************************************
 * Callback executed when S1 function is ready to be executed
 *
 * @param[in]  token_id     Token identifier of security function
 * @param[out] pp_msg       Pointer to Message to authenticate
 * @param[out] p_msg_len    Pointer to Length of the message
 *
 * @return Status error code (@see enum m_error)
 ****************************************************************************************
 */
typedef uint16_t (*mesh_tb_sec_s1_ready_cb)(uint8_t token_id, const uint8_t **pp_msg, uint8_t *p_msg_len);


/**
 ****************************************************************************************
 * Callback executed when K1 function is ready to be executed
 *
 * @param[in]  token_id     Token identifier of security function
 * @param[out] pp_salt      Pointer to Salt key
 * @param[out] pp_n         Pointer to N value
 * @param[out] p_n_len      Pointer to N length
 * @param[out] pp_p         Pointer to P value
 * @param[out] p_p_len      Pointer to P length
 *
 * @return Status error code (@see enum m_error)
 ****************************************************************************************
 */
typedef uint16_t (*mesh_tb_sec_k1_ready_cb)(uint8_t token_id, const uint8_t **pp_salt,
        const uint8_t **pp_n, uint8_t *p_n_len,
        const uint8_t **pp_p, uint8_t *p_p_len);

/**
 ****************************************************************************************
 * Callback executed when K2 function is ready to be executed
 *
 * @param[in]  token_id     Token identifier of security function
 * @param[out] pp_n         Pointer to N value - (128 bits)
 * @param[out] pp_p         Pointer to P value
 * @param[out] p_p_len      Pointer to P length
 *
 * @return Status error code (@see enum m_error)
 ****************************************************************************************
 */
typedef uint16_t (*mesh_tb_sec_k2_ready_cb)(uint8_t token_id, const uint8_t **pp_n,
        const uint8_t **pp_p, uint8_t *p_p_len);

/**
 ****************************************************************************************
 * Callback executed when K3 function is ready to be executed
 *
 * @param[in]  token_id     Token identifier of security function
 * @param[out] pp_n         Pointer to N value - (128 bits)
 *
 * @return Status error code (@see enum m_error)
 ****************************************************************************************
 */
typedef uint16_t (*mesh_tb_sec_k3_ready_cb)(uint8_t token_id, const uint8_t **pp_n);

/**
 ****************************************************************************************
 * Callback executed when K4 function is ready to be executed
 *
 * @param[in]  token_id     Token identifier of security function
 * @param[out] pp_n         Pointer to N value - (128 bits)
 *
 * @return Status error code (@see enum m_error)
 ****************************************************************************************
 */
typedef uint16_t (*mesh_tb_sec_k4_ready_cb)(uint8_t token_id, const uint8_t **pp_n);

/**
 ****************************************************************************************
 * Callback executed when AES CCM Encryption function is ready to be executed
 *
 * @param[in]  token_id          Token identifier of security function
 * @param[out] pp_key            Pointer to the Key to be used (128 bits)
 * @param[out] nonce             Pointer to 13 Bytes Nonce to use
 * @param[out] pp_msg            Pointer to message to encrypt
 * @param[out] pp_cipher         Pointer to message where encrypted data and mic will be pushed
 * @param[out] p_msg_len         Pointer where length of Input message have to be set
 * @param[out] mic_len           Pointer where length of the mic have to be set (2, 4, 6, 8, 10, 12, 14, 16 valid)
 * @param[out] pp_auth_data      Pointer to Additional Authentication data used for computation of MIC
 * @param[out] p_auth_data_len   Pointer where Additional Authentication data length have to be set
 *
 * @return Status error code (@see enum m_error)
 ****************************************************************************************
 */
typedef uint16_t (*mesh_tb_sec_ccm_enc_ready_cb)(uint8_t token_id, const uint8_t **pp_key,
        const uint8_t **pp_nonce, const uint8_t **pp_msg,
        uint8_t **pp_cipher, uint16_t *p_msg_len,
        uint8_t *p_mic_len,  const uint8_t **pp_auth_data,
        uint8_t *p_auth_data_len);

/**
 ****************************************************************************************
 * Callback executed when AES CCM Decryption function is ready to be executed
 *
 * @param[in]  token_id          Token identifier of security function
 * @param[out] pp_key            Pointer to the Key to be used (128 bits)
 * @param[out] nonce             Pointer to 13 Bytes Nonce to use
 * @param[out] pp_cipher         Pointer to input encrypted data with mic
 * @param[out] pp_msg            Pointer to message where decrypted data will be pushed
 * @param[out] p_msg_len         Pointer where length of Encrypted message have to be set (without mic length)
 * @param[out] mic_len           Pointer where length of the mic have to be set (2, 4, 6, 8, 10, 12, 14, 16 valid)
 * @param[out] pp_auth_data      Pointer to Additional Authentication data used for computation of MIC
 * @param[out] p_auth_data_len   Pointer where Additional Authentication data length have to be set
 *
 * @return Status error code (@see enum m_error)
 ****************************************************************************************
 */
typedef uint16_t (*mesh_tb_sec_ccm_dec_ready_cb)(uint8_t token_id, const uint8_t **pp_key, const uint8_t **pp_nonce,
        const uint8_t **pp_cipher,  uint8_t **pp_msg, uint16_t *p_msg_len,
        uint8_t *p_mic_len,  const uint8_t **pp_auth_data,
        uint8_t *p_auth_data_len);
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Start Execution of an AES function
 *
 * @param[in] token_id    Token identifier of security function
 * @param[in] cb_ready    Function to call when security manager is ready to execute function
 * @param[in] cb_finished Function to call when security function execution is done
 *
 * @return Status of function execution request (@see enum m_error)
 ****************************************************************************************
 */
uint16_t mesh_tb_sec_aes_start(uint8_t token_id, mesh_tb_sec_aes_ready_cb cb_ready, mesh_tb_sec_finished_cb cb_finished);

/**
 ****************************************************************************************
 * @brief Start Execution of an AES-CMAC function
 *
 * @param[in] token_id    Token identifier of security function
 * @param[in] cb_ready    Function to call when security manager is ready to execute function
 * @param[in] cb_finished Function to call when security function execution is done
 *
 * @return Status of function execution request (@see enum m_error)
 ****************************************************************************************
 */
uint16_t mesh_tb_sec_cmac_start(uint8_t token_id, mesh_tb_sec_cmac_ready_cb cb_ready, mesh_tb_sec_finished_cb cb_finished);

/**
 ****************************************************************************************
 * @brief Start Execution of an S1 function
 *
 * @param[in] token_id    Token identifier of security function
 * @param[in] cb_ready    Function to call when security manager is ready to execute function
 * @param[in] cb_finished Function to call when security function execution is done
 *
 * @return Status of function execution request (@see enum m_error)
 ****************************************************************************************
 */
uint16_t mesh_tb_sec_s1_start(uint8_t token_id, mesh_tb_sec_s1_ready_cb cb_ready, mesh_tb_sec_finished_cb cb_finished);

/**
 ****************************************************************************************
 * @brief Start Execution of an K1 function
 *
 * @param[in] token_id    Token identifier of security function
 * @param[in] cb_ready    Function to call when security manager is ready to execute function
 * @param[in] cb_finished Function to call when security function execution is done
 *
 * @return Status of function execution request (@see enum m_error)
 ****************************************************************************************
 */
uint16_t mesh_tb_sec_k1_start(uint8_t token_id, mesh_tb_sec_k1_ready_cb cb_ready, mesh_tb_sec_finished_cb cb_finished);

/**
 ****************************************************************************************
 * @brief Start Execution of an K2 function
 *
 * @param[in] token_id    Token identifier of security function
 * @param[in] cb_ready    Function to call when security manager is ready to execute function
 * @param[in] cb_finished Function to call when security function execution is done
 *
 * @return Status of function execution request (@see enum m_error)
 ****************************************************************************************
 */
uint16_t mesh_tb_sec_k2_start(uint8_t token_id, mesh_tb_sec_k2_ready_cb cb_ready, mesh_tb_sec_finished_cb cb_finished);

/**
 ****************************************************************************************
 * @brief Start Execution of an K3 function
 *
 * @param[in] token_id    Token identifier of security function
 * @param[in] cb_ready    Function to call when security manager is ready to execute function
 * @param[in] cb_finished Function to call when security function execution is done
 *
 * @return Status of function execution request (@see enum m_error)
 ****************************************************************************************
 */
uint16_t mesh_tb_sec_k3_start(uint8_t token_id, mesh_tb_sec_k3_ready_cb cb_ready, mesh_tb_sec_finished_cb cb_finished);

/**
 ****************************************************************************************
 * @brief Start Execution of an K4 function
 *
 * @param[in] token_id    Token identifier of security function
 * @param[in] cb_ready    Function to call when security manager is ready to execute function
 * @param[in] cb_finished Function to call when security function execution is done
 *
 * @return Status of function execution request (@see enum m_error)
 ****************************************************************************************
 */
uint16_t mesh_tb_sec_k4_start(uint8_t token_id, mesh_tb_sec_k4_ready_cb cb_ready, mesh_tb_sec_finished_cb cb_finished);

/**
 ****************************************************************************************
 * @brief Start Execution of a Random function using AES
 *
 * @param[in] token_id    Token identifier of security function
 * @param[in] cb_finished Function to call when security function execution is done
 *
 * @return Status of function execution request (@see enum m_error)
 ****************************************************************************************
 */
uint16_t mesh_tb_sec_rand_start(uint8_t token_id, mesh_tb_sec_finished_cb cb_finished);

/**
 ****************************************************************************************
 * @brief Start Execution of an AES-CCM Encryption function
 *
 * @param[in] token_id    Token identifier of security function
 * @param[in] cb_ready    Function to call when security manager is ready to execute function
 * @param[in] cb_finished Function to call when security function execution is done
 *
 * @return Status of function execution request (@see enum m_error)
 ****************************************************************************************
 */
uint16_t mesh_tb_sec_ccm_enc_start(uint8_t token_id, mesh_tb_sec_ccm_enc_ready_cb cb_ready,
                                   mesh_tb_sec_finished_cb cb_finished);

/**
 ****************************************************************************************
 * @brief Start Execution of an AES-CCM Decryption function
 *
 * @param[in] token_id    Token identifier of security function
 * @param[in] cb_ready    Function to call when security manager is ready to execute function
 * @param[in] cb_finished Function to call when security function execution is done
 *
 * @return Status of function execution request (@see enum m_error)
 ****************************************************************************************
 */
uint16_t mesh_tb_sec_ccm_dec_start(uint8_t token_id, mesh_tb_sec_ccm_dec_ready_cb cb_ready,
                                   mesh_tb_sec_finished_cb cb_finished);

#if (BLE_MESH_DBG)
/**
 ****************************************************************************************
 * @brief Inform Security Manager that a Public Key has been generated by the BLE stack
 *
 * @param[in] p_pub_key_x       Pointer to X coordinates of the Public Key
 * @param[in] p_pub_key_y       Pointer to Y coordinates of the Public Key
 ****************************************************************************************
 */
void mesh_tb_sec_pub_key_ind(const uint8_t *p_pub_key_x, const uint8_t *p_pub_key_y);

/**
 ****************************************************************************************
 * @brief Request Security Manager the return the currently known Public Key
 *
 * @param[in] p_pub_key_x       Address at which X coordinates of the Public Key must be returned
 * @param[in] p_pub_key_y       Address at which Y coordinates of the Public Key must be returned
 ****************************************************************************************
 */
void mesh_tb_sec_get_pub_key(uint8_t *p_pub_key_x, uint8_t *p_pub_key_y);

/**
 ****************************************************************************************
 * @brief Set the random number to be used for provisioning and during proxy bearer
 * establishment
 *
 * @param[in] p_rand        Pointer to the Random value to be used
 ****************************************************************************************
 */
void mesh_tb_sec_set_rand(uint8_t *p_rand);
#endif //(BLE_MESH_DBG)

/// @} MESH_TB_SEC

#endif /* MESH_TB_SEC_ */
