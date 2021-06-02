/**
 ****************************************************************************************
 *
 * @file m_api.h
 *
 * @brief Header file for Mesh Profile Application Programming Interface
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_API_H_
#define _M_API_H_

/**
 ****************************************************************************************
 * @defgroup M_API Mesh Profile Application Programming Interface
 * @ingroup MESH
 * @brief Mesh Profile Application Programming Interface
 * @{
 ****************************************************************************************
 */

/*
 * VARIABLES
 ****************************************************************************************
 */

#include "mesh_defines.h"
#include "mesh_api.h"

/*
 * VARIABLES
 ****************************************************************************************
 */

/// Callback used to discuss with a native application
extern const m_api_cb_t *p_m_api_cb;
/// Callback used to discuss with a native application
extern const m_api_fault_cb_t *p_m_api_fault_cb;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh API Module initialization function
 *
 * @param[in] reset     True means SW reset, False means task initialization
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module
 ****************************************************************************************
 */
uint16_t m_api_init(bool reset, void *p_env, const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Return memory size needed for environment allocation of Mesh API Module
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module
 ****************************************************************************************
 */
uint16_t m_api_get_env_size(const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief General handler for message targeting the Mesh Profile block
 *
 * @param[in] msg_id        Message Identifier
 * @param[in] src_id        Source ID
 * @param[in] p_param       Pointer to message parameters
 *
 * @return Message status
 ****************************************************************************************
 */
uint8_t m_api_handler(uint16_t msg_id, uint16_t src_id, const void *p_param);

/**
 ****************************************************************************************
 * @brief Indicate an update of attention state to the application. It can be sent either
 * because attention timer has expired, or because attention state has been modified by
 * the provisioner.
 *
 * @param[in] attention_state       New attention state value
 ****************************************************************************************
 */
void m_api_send_attention_update_ind(uint8_t attention_state);

/**
 ****************************************************************************************
 * @brief Request content of a page of composition data to the application.
 *
 * @param[in] page       Page number
 ****************************************************************************************
 */
void m_api_send_compo_data_req_ind(uint8_t page);

/**
 ****************************************************************************************
 * @brief Request stored information to the application.
 ****************************************************************************************
 */
void m_api_send_stored_info_req_ind(void);

/**
 ****************************************************************************************
 * @brief Inform the application about a configuration update.
 *
 * @param upd_type      Update type
 * @param length        Entry length
 * @param p_data        Pointer to entry data
 ****************************************************************************************
 */
void m_api_send_update_ind(uint8_t upd_type, uint16_t length, uint8_t *p_data);


/**
 ****************************************************************************************
 * @brief Inform the application about a configuration model app.
 *
 * @param status        status
 * @param length        Entry length
 * @param p_data        Pointer to entry data
 ****************************************************************************************
 */
void m_api_send_model_app_bind_ind(uint8_t status, uint32_t model_id, uint16_t element_addr, m_lid_t model_lid, m_lid_t app_key_lid);


void m_api_send_model_subs_ind(uint8_t status, uint32_t model_id, uint16_t element_addr, uint16_t subs_addr, m_lid_t model_lid);

void m_api_send_key_ind(uint8_t key_type, const void *key);


/**
 ****************************************************************************************
 * @brief Inform the application about received node reset request from the provisioner
 ****************************************************************************************
 */
void m_api_send_node_reset_ind(void);

/**
 ****************************************************************************************
 * @brief Request to get the current Registered Fault state identified by Company ID for primary
 * element.
 *
 * @param[in] comp_id       Company ID
 ****************************************************************************************
 */
void m_api_send_fault_get_req_ind(uint16_t comp_id);

/**
 ****************************************************************************************
 * @brief Request to start a test procedure of primary element.
 *
 * @param[in] comp_id       Company ID
 * @param[in] test_id       Test ID
 * @param[in] cfm_needed    Indicate if MESH_FAULT_CFM message is expected from the application.
 ****************************************************************************************
 */
void m_api_send_fault_test_req_ind(uint16_t comp_id, uint8_t test_id, bool cfm_needed);

/**
 ****************************************************************************************
 * @brief Inform that clear of Registered Fault state identified by Company ID has been received for
 * primary element
 *
 * @param[in] comp_id       Company ID
 ****************************************************************************************
 */
void m_api_send_fault_clear_ind(uint16_t comp_id);

/**
 ****************************************************************************************
 * @brief Send MESH_FAULT_PERIOD_IND message to the application
 *
 * @param[in] period_ms       Publication period in milliseconds
 * @param[in] period_ms       Publication period in milliseconds when one or several fault are known
 ****************************************************************************************
 */
void m_api_send_fault_period_ind(uint32_t period_ms, uint32_t period_fault_ms);

#if (BLE_MESH_LPN)
/**
 ****************************************************************************************
 * @brief Request to send a MESH_API_LPN_STATUS_IND message to the upper application.
 *
 * @param[in] status        Friendship status
 * @param[in] friend_addr   Address of Friend node
 ****************************************************************************************
 */
void m_api_send_lpn_status_ind(uint16_t status, uint16_t friend_addr);

/**
 ****************************************************************************************
 * @brief Request to send a MESH_API_LPN_OFFER_IND message to the upper application.
 *
 * @param[in] friend_addr       Address of node that sent the Friend Offer message
 * @param[in] rx_window         RX window supported by the Friend node
 * @param[in] queue_size        Queue size supported by the Friend node
 * @param[in] subs_list_size    Size of subscription list supported by the Friend node
 * @param[in] rssi              RSSI measured upon reception of Friend Offer message
 ****************************************************************************************
 */
void m_api_send_lpn_offer_ind(uint16_t friend_addr, uint8_t rx_window,
                              uint8_t queue_size, uint8_t subs_list_size, int8_t rssi);
#endif //(BLE_MESH_LPN)

#if (BLE_MESH_GATT_PROXY)
/**
 ****************************************************************************************
 * @brief Inform upper application about a state change of proxy advertising
 ****************************************************************************************
 */
void m_api_send_proxy_adv_update_ind(uint8_t state, uint8_t reason);
#endif //(BLE_MESH_GATT_PROXY)

#if (BLE_MESH_MSG_API)
/**
 ****************************************************************************************
 * @brief Set the task ID to be used as destination task id for message sent for a given
 * model.
 *
 * @param[in] model_lid       Model Local Index
 * @param[in] task_id         Task ID of Task handling the model
 ****************************************************************************************
 */
void m_api_model_set_task_id(m_lid_t model_lid, uint16_t task_id);

/**
 ****************************************************************************************
 * @brief Get the task ID to be used as destination task id for message sent for a given
 * model.
 *
 * @param[in] model_lid       Model Local Index
 ****************************************************************************************
 */
uint16_t m_api_model_get_task_id(m_lid_t model_lid);
#endif //(BLE_MESH_MSG_API)

#if (BLE_MESH_PROVISIONER)
uint16_t m_api_add_network_key(uint16_t net_key_id, uint8_t *netkey);

uint16_t m_api_add_app_key(uint16_t app_key_id, uint8_t *app_key, uint16_t net_key_id);
#endif //BLE_MESH_PROVISIONER

/// @} end of group

#endif //_M_API_
