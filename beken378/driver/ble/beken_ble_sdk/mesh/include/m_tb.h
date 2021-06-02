/**
 ****************************************************************************************
 *
 * @file m_tb.h
 *
 * @brief Header file for Mesh Toolboxes
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_TB_H_
#define _M_TB_H_

/**
 ****************************************************************************************
 * @defgroup M_TB Mesh Toolboxes
 * @ingroup MESH
 * @brief Mesh Toolboxes
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh_int.h"
#include "m_tb_friend.h"
#include "m_tb_key.h"
#include "m_tb_lpn.h"
#include "m_tb_mio.h"
#include "m_tb_state.h"
#include "m_tb_store.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh toolbox module initialization
 *
 * @note Called when Mesh stack is initialized or when a SW reset is requested.
 *
 * @param[in] reset True means SW reset, False means task initialization
 * @param[in] p_env Pointer to the environment structure
 * @param[in] p_cfg Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_tb_init(bool reset, void *p_env, const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Mesh toolbox module memory requirement
 *
 * @note This function is called before init function in order to know memory requirements of the module
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this mesh module
 ****************************************************************************************
 */
uint16_t m_tb_get_env_size(const m_cfg_t *p_cfg);

/// @} end of group

#endif //_M_TB_H_
