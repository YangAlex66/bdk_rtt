/**
 ****************************************************************************************
 * @file m_tb_int.h
 *
 * @brief Header file for Mesh Toolbox Internal Defines
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_TB_INT_H_
#define _M_TB_INT_H_

/**
 ****************************************************************************************
 * @defgroup M_TB_INT Mesh Layer Internal Defines
 * @ingroup M_TB
 * @brief Mesh Layer Internal Defines
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "m_api.h"

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh key manager Toolbox initialization
 *
 * @note called when Mesh stack is initialized or when a SW reset is requested.
 *
 * @param[in] reset True means SW reset, False means task initialization
 * @param[in] p_env Pointer to the environment structure
 * @param[in] p_cfg Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_tb_key_init(bool reset, void *p_env, const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Mesh  key managerToolbox memory requirement
 *
 * @note this function is called before init function in order to know memory requirements of the module
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this mesh module
 ****************************************************************************************
 */
uint16_t m_tb_key_get_env_size(const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Model IO Manager Toolbox Initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_tb_mio_init(bool reset, void *p_env, const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Model IO Manager Toolbox.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_tb_mio_get_env_size(const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief State Manager Toolbox Initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_tb_state_init(bool reset, void *p_env, const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Get size of environment used by State Manager Toolbox.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_tb_state_get_env_size(const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Storage Manager Toolbox Initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_tb_store_init(bool reset, void *p_env, const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Storage Manager Toolbox.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_tb_store_get_env_size(const m_cfg_t *p_cfg);

#if (BLE_MESH_FRIEND)
/**
 ****************************************************************************************
 * @brief Friendship Manager Initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_tb_friend_init(bool reset, void *p_env, const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Friendship Manager.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_tb_friend_get_env_size(const m_cfg_t *p_cfg);
#endif //(BLE_MESH_FRIEND)

#if (BLE_MESH_LPN)
/**
 ****************************************************************************************
 * @brief Friendship Manager (Low Power Node) Initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_tb_lpn_init(bool reset, void *p_env, const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Friendship Manager (Low Power Node).
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_tb_lpn_get_env_size(const m_cfg_t *p_cfg);
#endif //(BLE_MESH_LPN)

/// @} end of group

#endif //_M_TB_INT_H_
