/**
 ****************************************************************************************
 * @file m_tb_mio.h
 *
 * @brief Header file for Model IO Manager Toolbox
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_TB_MIO_H_
#define _M_TB_MIO_H_

/**
 ****************************************************************************************
 * @defgroup M_TB_MIO Model IO Manager Toolbox
 * @ingroup MESH
 * @brief Model IO Manager Toolbox
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "m_api.h"      // Mesh application program interface
#include "m_config.h"

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// Callback function to call when an unknown address has been added in one of the subscription lists
typedef void (*m_tb_mio_addr_list_cb_add)(uint16_t addr);
/// Callback function to call when a address has been removed and is no more present in a subscription list
typedef void (*m_tb_mio_addr_list_cb_rem)(uint16_t addr);

typedef struct
{
	uint16_t loc;
	uint8_t nb_sig_models;
	uint8_t nb_vendor_models;
	uint8_t start;
	uint8_t end;
}m_tb_mio_element_list;


typedef struct
{
	uint16_t cid;
	uint16_t pid;
	uint16_t vid;
	uint16_t crpl;
	uint16_t feature;
	uint8_t element_num;
	uint8_t modle_num;
	m_tb_mio_element_list element_list[5];
	uint32_t modle_id_list[30];
}m_tb_mio_comp_data;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Register a model instance.
 *
 * @param[in] model_id          Model ID.
 * @param[in] elmt_idx          Index of the element the model belongs to
 * @param[in] config            Configuration
 * @param[in] p_cb              Pointer to callbacks defined by the model
 * @param[out] p_model_lid      Pointer to the variable that will contain the allocated Model LID.
 *
 * @return MESH_ERR_NO_ERROR if model instance has been registered
 *         MESH_ERR_COMMAND_DISALLOWED if it was not possible to register the model instance.
 ****************************************************************************************
 */
uint16_t m_tb_mio_register_model(uint32_t model_id, uint8_t elmt_idx, uint8_t config,
                                 const m_api_model_cb_t *p_cb, m_lid_t *p_model_lid);

/**
 ****************************************************************************************
 * @brief Get number of registered models.
 *
 * @return Number of registered models.
 ****************************************************************************************
 */
uint8_t m_tb_mio_get_nb_model(void);

/**
 ****************************************************************************************
 * @brief Get local index for a model instance identified by its element address and its
 * model identifier.
 *
 * @param[in] element_addr      Element address.
 * @param[in] model_id          Model ID.
 * @param[out] p_model_lid      Pointer to the variable that will contain the found Model LID.
 *
 * @return MESH_ERR_NO_ERROR if requested model instance has been found.
 *         MESH_ERR_INVALID_PARAM if requested model instance is not registered.
 ****************************************************************************************
 */
uint16_t m_tb_mio_get_local_id(uint16_t element_addr, uint32_t model_id, m_lid_t *p_model_lid);

/**
 ****************************************************************************************
 * @brief Get model identifier for a model identified by its model local index.
 *
 * @param[in] model_lid         Model local index.
 * @param[in] p_vendor          Vendor or SIG model identifier.
 *
 * @return Model identifier
 ****************************************************************************************
 */
uint32_t m_tb_mio_get_model_id(m_lid_t model_lid);

/**
 ****************************************************************************************
 * @brief Get unicast element address of a provided model
 *
 * @param[in]  model_lid      Model LID.
 * @param[out] p_elt_addr     Model Element address
 *
 * @return MESH_ERR_NO_ERROR if publication parameters have been set.
 *         MESH_ERR_INVALID_MODEL if model identifier is unknown
 ****************************************************************************************
 */
uint16_t m_tb_mio_get_element_addr(m_lid_t model_lid, uint16_t *p_elt_addr);

/**
 ****************************************************************************************
 * @brief Get set of callback for a provided model
 *
 * @param[in]  model_lid      Model LID.
 * @param[out] pp_model_cb    Callback exposed by the model instance
 *
 * @return MESH_ERR_NO_ERROR if publication parameters have been set.
 *         MESH_ERR_INVALID_MODEL if model identifier is unknown
 ****************************************************************************************
 */
uint16_t m_tb_mio_cb_get(m_lid_t model_lid, const m_api_model_cb_t **pp_model_cb);

/**
 ****************************************************************************************
 * @brief Set publication parameters for a given model instance.
 *
 * @param[in] model_lid        Model LID.
 * @param[in] p_params         Pointer to a buffer containing the publication parameters.
 *
 * @return MESH_ERR_NO_ERROR if publication parameters have been set.
 *         MESH_ERR_INVALID_PARAM if publication parameters are not valid.
 ****************************************************************************************
 */
uint16_t m_tb_mio_set_publi_param(m_lid_t model_lid, uint16_t addr, uint8_t *p_label_uuid,
                                  uint8_t app_key_lid, uint8_t ttl, uint8_t period, uint8_t retx_params,
                                  uint8_t friend_cred);

/**
 ****************************************************************************************
 * @brief Get publication parameters to be used for a given model instance.
 *
 * @param[in] model_lid        Model LID.
 * @param[in] p_params         Pointer to a buffer that will contain the publication parameters.
 *
 * @return MESH_ERR_NO_ERROR if publication parameters have been written.
 *         MESH_ERR_COMMAND_DISALLOWED if publication parameters have not been set.
 ****************************************************************************************
 */
uint16_t m_tb_mio_get_publi_param(m_lid_t model_lid, uint16_t *p_addr, uint8_t *p_va_lid,
                                  uint8_t *p_app_key_lid, uint8_t *p_ttl, uint8_t *p_period,
                                  uint8_t *p_retx_params, uint8_t *p_friend_cred);

/**
 ****************************************************************************************
 * @brief Add an address in the subscription list of a given model instance.
 *
 * @param[in] model_lid        Model LID.
 * @param[in] addr             Address to insert.
 * @param[in] p_label_uuid     Pointer to Label UUID used to generate the address in case it is a
 * virtual address.
 *
 * @return MESH_ERR_NO_ERROR if address has been inserted.
 *         MESH_ERR_COMMAND_DISALLOWED if address cannot be inserted.
 ****************************************************************************************
 */
uint16_t m_tb_mio_add_subscription(m_lid_t model_lid, uint16_t addr);

/**
 ****************************************************************************************
 * @brief Add a virtual address to the subscription list of a model.
 *
 * @param[in] model_lid         Model local index.
 * @param[in] addr              Generated virtual address.
 * @param[in] p_label_uuid      Pointer to the Label UUID value used to generate the provided
 * virtual address
 *
 * @return MESH_ERR_NO_ERROR if address has been inserted, else MESH_ERR_INSUFFICIENT_RESOURCES.
 ****************************************************************************************
 */
uint16_t m_tb_mio_add_subscription_virt(m_lid_t model_lid, uint16_t addr, uint8_t *p_label_uuid);

/**
 ****************************************************************************************
 * @brief Delete an address in the subscription list of a given model instance.
 *
 * @param[in] model_lid        Model LID.
 * @param[in] addr             Address to remove.
 ****************************************************************************************
 */
void m_tb_mio_delete_subscription(m_lid_t model_lid, uint16_t addr);

/**
 ****************************************************************************************
 * @brief Delete a virtual address from the subscription list of a model.
 *
 * @param[in] model_lid         Model local index
 * @param[in] p_label_uuid      Pointer to the Label UUID value allowing to find the address
 * to delete.
 * @param[out] p_addr           Pointer to variable in which deleted virtual address will be written.
 ****************************************************************************************
 */
void m_tb_mio_delete_subscription_virt(m_lid_t model_lid, uint8_t *p_label_uuid, uint16_t *p_addr);

/**
 ****************************************************************************************
 * @brief Delete content of the subscription list of a given model instance.
 *
 * @param[in] model_lid        Model LID.
 ****************************************************************************************
 */
void m_tb_mio_delete_all_subscription(m_lid_t model_lid);

/**
 ****************************************************************************************
 * @brief Get number of addresses registered in the subscription list of a given model
 * instance.
 *
 * @param[in] model_lid        Model LID.
 *
 * @return Number of address in the subscription list.
 ****************************************************************************************
 */
uint16_t m_tb_mio_get_subscription_list_size(m_lid_t model_lid);

/**
 ****************************************************************************************
 * @brief Get number of virtual addresses registered in the subscription list of a given model
 * instance.
 *
 * @param[in] model_lid        Model LID.
 *
 * @return Number of virtual addresses in the subscription list.
 ****************************************************************************************
 */
uint16_t m_tb_mio_get_subscription_list_size_vaddr(m_lid_t model_lid);

/**
 ****************************************************************************************
 * @brief Get content of the subscription list of a given model instance and copy it in
 * a buffer.
 *
 * @param[in] model_lid        Model LID.
 * @param[in] p_addr           Pointer to the buffer in which content of the subscription
 * list must be copied.
 * @param[in] label_uuid       Boolean indicating if label uuid value must be provided with a
 * virtual address.
 ****************************************************************************************
 */
void m_tb_mio_get_subscription_list(m_lid_t model_lid, uint8_t *p_addr, bool label_uuid);

/**
 ****************************************************************************************
 * @brief Check if an address is targetting current model
 *        (element address or configured subscription list)
 *
 * @param[in] model_lid        Model LID.
 * @param[in] addr             Address to find in the subscription list.
 * @param[in] va_lid           Valid only for a virtual address, the virtual address
 *                             local identifier
 *
 * @return True if address is targeting current model; false else
 ****************************************************************************************
 */
bool m_tb_mio_is_dest_addr(m_lid_t model_lid, uint16_t addr, m_lid_t va_lid);

/**
 ****************************************************************************************
 * @brief Get Label UUID value used to generate a provided virtual address. As a given virtual
 * address can have been generated using different Label UUID, several tries might be needed
 * in order to retrieve the current Label UUID value.
 * In order to parse the Label UUID values one by one, a skip parameter is provided in order
 * in indicate number tries already performed.
 *
 * @param[in] virt_addr         Virtual address.
 * @param[in] skip              Number of valid Label UUID values to skip
 * @param[in] pp_label_uuid     Pointer to the Label UUID.
 ****************************************************************************************
 */
m_lid_t m_tb_mio_get_label_uuid(uint16_t virt_addr, m_lid_t skip, uint8_t **pp_label_uuid);

/**
 ****************************************************************************************
 * @brief Indicate that an application key has been bound with a given model.
 *
 * @param[in] model_lid        Model local index.
 ****************************************************************************************
 */
void m_tb_mio_bind(m_lid_t model_lid);

/**
 ****************************************************************************************
 * @brief Indicate that an application key has been unbound from a given model.
 *
 * @param[in] model_lid        Model local index.
 ****************************************************************************************
 */
void m_tb_mio_unbind(m_lid_t model_lid, m_lid_t app_key_lid);

/**
 ****************************************************************************************
 * @brief Get number of application keys currently bound with a given model.
 *
 * @param[in] model_lid        Model local index.
 *
 * @return Number of bound application keys.
 ****************************************************************************************
 */
uint8_t m_tb_mio_get_nb_bound_app(m_lid_t model_lid);

/**
 ****************************************************************************************
 * @brief Get publish period in milliseconds for a given model.
 *
 * @param[in] model_lid        Model local index.
 *
 * @return Publish period in milliseconds.
 ****************************************************************************************
 */
uint32_t m_tb_mio_get_period(m_lid_t model_lid);

/**
 ****************************************************************************************
 * @brief Set address of primary element.
 ****************************************************************************************
 */
void m_tb_mio_set_prim_addr(uint16_t prim_addr);

/**
 ****************************************************************************************
 * @return Address of the primary element
 ****************************************************************************************
 */
uint16_t m_tb_mio_get_prim_addr(void);

/**
 ****************************************************************************************
 * @brief Return if a provided element address is address of a local element.
 ****************************************************************************************
 */
bool m_tb_mio_is_local_addr(uint16_t addr);

/**
 ****************************************************************************************
 * @brief Get Label UUID for a virtual address.
 *
 * @param[in] va_lid    Virtual address local index
 ****************************************************************************************
 */
uint8_t *m_tb_mio_get_vaddr(m_lid_t va_lid);

/**
 ****************************************************************************************
 * @brief Return local number of elements
 ****************************************************************************************
 */
uint8_t m_tb_mio_get_nb_elements(void);

/**
 ****************************************************************************************
 * @brief Get number of registered models
 *
 * @param[in] elmt_idx              Element Index
 * @param[out] p_nb_sig_models      Pointer to variable that will contain number of registered
 * SIG models
 * @param[out] p_nb_vendor_models   Pointer to variable that will contain number of registered
 * vendor models
 ****************************************************************************************
 */
void m_tb_mio_get_nb_models(uint8_t elmt_idx, uint8_t *p_nb_sig_models, uint8_t *p_nb_vendor_models);

/**
 ****************************************************************************************
 * @brief Get list of vendor or SIG Model IDs
 *
 * @param[in] elmt_idx  Element Index
 * @param[out] p_data   Pointer to array that will contain list of Model IDs
 * @param[in]  vendor   True if list of vendor Model IDs is requested, false for SIG Model IDs
 ****************************************************************************************
 */
void m_tb_mio_get_model_ids(uint8_t elmt_idx, uint8_t *p_data, bool vendor);

#if (BLE_MESH_LPN)
/**
 ****************************************************************************************
 * @brief Inform the Model IO manager that it must maintain a list of all addresses present
 * in the subscription lists of the registered model so that content of this list can be provided
 * to the Friend Node after establishment of a friendship
 *
 * @param[in] nb_addr_max       Maximum number of address that can be stored on Friend Node
 * @param[in] cb_addr           Callback function to call when an unknown address is added
 * in a subscription list
 * @param[in] cb_rem            Callback function to call when an address is no more present
 * in any of the subscription lists.
 *
 * @return Error status
 ****************************************************************************************
 */
uint16_t m_tb_mio_addr_list_start(uint8_t nb_addr_max,
                                  m_tb_mio_addr_list_cb_add cb_addr,
                                  m_tb_mio_addr_list_cb_rem cb_rem);

/**
 ****************************************************************************************
 * @brief Inform the Model IO manager that it can stop maintaining list of all known addresses
 ****************************************************************************************
 */
void m_tb_mio_addr_list_stop(void);

/**
 ****************************************************************************************
 * @brief Get number of address present in the list of known addresses
 *
 * @param[out] p_nb_addr        Pointer used to return the number of known addresses
 *
 * @return Error status
 ****************************************************************************************
 */
uint16_t m_tb_mio_addr_list_get_size(uint8_t *p_nb_addr);

/**
 ****************************************************************************************
 * @brief Get list of known addresses
 *
 * @param[out] p_addr           Pointer used to return the list of known addresses
 *
 * @return Error status
 ****************************************************************************************
 */
uint16_t m_tb_mio_addr_list_get(uint16_t *p_addr);
#endif //(BLE_MESH_LPN)

/**
 ****************************************************************************************
 * @brief Inform Model IO manager that an application key has been removed. Publication for models
 * that are using this application key must be disabled.
 *
 * @param[in] app_key_lid       Application Key local index
 ****************************************************************************************
 */
void m_tb_mio_app_key_rem_ind(m_lid_t app_key_lid);

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
void m_tb_mio_set_task_id(m_lid_t model_lid, uint16_t task_id);

/**
 ****************************************************************************************
 * @brief Get the task ID to be used as destination task id for message sent for a given
 * model.
 *
 * @param[in] model_lid       Model Local Index
 ****************************************************************************************
 */
uint16_t m_tb_mio_get_task_id(m_lid_t model_lid);

void m_tb_mio_get_comp(m_tb_mio_comp_data *comp_data);
void m_tb_mio_del_elmt_subscr(uint16_t element_addr, uint32_t model_id, uint16_t group_addr);
void m_tb_mio_add_elmt_subscr(uint16_t element_addr, uint32_t model_id, uint16_t group_addr);

#endif //(BLE_MESH_MSG_API)

/// @} end of group

#endif //_M_TB_MIO_H_
