/**
 ****************************************************************************************
 * @file m_fnd_int.h
 *
 * @brief Header file for Mesh Foundation Models Internal Defines
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_FND_INT_H_
#define _M_FND_INT_H_

/**
 ****************************************************************************************
 * @defgroup M_FND_INT Mesh Foundation Models Internal Defines
 * @ingroup MESH
 * @brief Mesh Foundation Models Internal Defines
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "m_api.h"
#include "mesh_defines.h"
#include "m_defines.h"
#include "m_config.h"
#include "mal.h"
#include "m_tb.h"
#include "mm_tb_state.h"
/*
 * DEFINES
 ****************************************************************************************
 */

typedef struct m_fnd_model_env *m_fnd_model_env_p;

typedef int32_t (*m_fnd_data_cb_p)(m_fnd_model_env_p p_env, uint32_t type,
                                   void *pargs);

typedef struct m_fnd_model_env
{
    /// Basic model environment - Must be first element in the structure - DO NOT MOVE
    mm_tb_state_mdl_env_t env;

    /// Timer for sending of publications
    mesh_tb_timer_t tmr_publi;

    uint32_t publi_period_ms;

    /// List of buffers containing messages to process
    common_list_t   process_queue;
    /// Delayed job structure
    mal_djob_t djob;
    /// Opcode actually handled
    uint8_t     opcode;

    uint32_t model_id;

    m_lid_t model_lid;

    m_fnd_data_cb_p model_data_cb;

    void *pargs;

} m_fnd_model_env_t;


/// Head length for buffers allocated by the foundation model layer
#define M_FND_BUF_HEAD_LEN                    (2)

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Configuration Client model initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_fnd_confs_init(bool reset, void *p_env, const m_cfg_t *p_cfg);

uint16_t m_fnd_confc_init(bool reset, void *p_env, const m_cfg_t* p_cfg);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Configuration Client model.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_fnd_confs_get_env_size(const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Health Client model initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_fnd_hlths_init(bool reset, void *p_env, const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Health Client model.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_fnd_hlths_get_env_size(const m_cfg_t *p_cfg);

/**
 ****************************************************************************************
 * @return Model local index allocated for the Health Client model of the primary element.
 ****************************************************************************************
 */
m_lid_t m_fnd_hlths_get_model_lid(void);

/// @} end of group

#endif //_M_FND_INT_H_
