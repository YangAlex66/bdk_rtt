/**
 ****************************************************************************************
 *
 * @file mm_tb_state.h
 *
 * @brief Header file for Mesh Model State Manager Module
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MM_TB_STATE_H_
#define _MM_TB_STATE_H_

/**
 ****************************************************************************************
 * @defgroup MM_API Mesh Model State Manager Module
 * @ingroup MESH_MDL
 * @brief Mesh Model State Manager Module
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mal_lib.h"            // Mesh Abstraction Layer Library
#include "mesh_defines.h"       // Mesh Stack Definitions
#include "mesh_tb_buf.h"        // Buffer Manager Definitions
#include "mesh_tb_timer.h"      // Timer Manager Definitions
#include "mm_route.h"           // Mesh Model Routing Module Definitions

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Model role
enum mm_tb_state_role
{
    /// Client role
    MM_TB_STATE_ROLE_CLI = 0,
    /// Server role
    MM_TB_STATE_ROLE_SRV
};

/// Model Configuration
enum mm_tb_state_cfg
{
    /// Role
    MM_TB_STATE_CFG_ROLE_LSB = 0,
    MM_TB_STATE_CFG_ROLE_MASK = 0x01,

    /// Allocate role-specific callback structure
    MM_TB_STATE_CFG_CB_POS = 1,
    MM_TB_STATE_CFG_CB_BIT = COMMON_BIT(MM_TB_STATE_CFG_CB_POS),
};

/// Information bit field for model environment
/// 7     3      2       1      0
/// +-----+------+-------+------+
/// | RFU | Role | Publi | Main |
/// +-----+------+-------+------+
enum mm_tb_state_mdl_info
{
    /// Model is main model of its group - set by Binding Manager
    MM_TB_STATE_MDL_INFO_MAIN_POS = 0,
    MM_TB_STATE_MDL_INFO_MAIN_BIT = COMMON_BIT(MM_TB_STATE_MDL_INFO_MAIN_POS),

    /// Publication are enabled for the model
    MM_TB_STATE_MDL_INFO_PUBLI_POS = 1,
    MM_TB_STATE_MDL_INFO_PUBLI_BIT = COMMON_BIT(MM_TB_STATE_MDL_INFO_PUBLI_POS),

    /// Role
    MM_TB_STATE_MDL_INFO_ROLE_LSB = 2,
    MM_TB_STATE_MDL_INFO_ROLE_MASK = 0x04,
};

/*
 * TYPE DEFINTIONS
 ****************************************************************************************
 */

typedef struct mm_tb_state_mdl_env mm_tb_state_mdl_env_t;

/*
 * INTERNAL CALLBACK FUNCTIONS PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief
 ****************************************************************************************
 */
typedef void (*mm_rx_cb)(mm_tb_state_mdl_env_t *p_env, mesh_tb_buf_t *p_buf,
                         mm_route_buf_env_t *p_route_env);

/**
 ****************************************************************************************
 * @brief
 ****************************************************************************************
 */
typedef uint16_t (*mm_opcode_check_cb)(mm_tb_state_mdl_env_t *p_env, uint32_t opcode);

/**
 ****************************************************************************************
 * @brief
 ****************************************************************************************
 */
typedef void (*mm_publish_period_cb)(mm_tb_state_mdl_env_t *p_env, uint16_t addr, uint32_t period_ms);

/**
 ****************************************************************************************
 * @brief
 ****************************************************************************************
 */
typedef uint16_t (*mm_srv_set_cb)(mm_tb_state_mdl_env_t *p_env, uint16_t state_id, uint32_t state);

/**
 ****************************************************************************************
 * @brief
 ****************************************************************************************
 */
typedef uint16_t (*mm_cli_get_cb)(mm_tb_state_mdl_env_t *p_env, uint16_t dst, uint16_t get_info);

/**
 ****************************************************************************************
 * @brief
 ****************************************************************************************
 */
typedef uint16_t (*mm_cli_set_cb)(mm_tb_state_mdl_env_t *p_env, uint16_t dst,
                                  uint32_t state_1, uint32_t state_2, uint16_t set_info);

/**
 ****************************************************************************************
 * @brief
 ****************************************************************************************
 */
typedef uint16_t (*mm_cli_transition_cb)(mm_tb_state_mdl_env_t *p_env, uint16_t dst,
        uint32_t state_1, uint32_t state_2,
        uint32_t trans_time_ms, uint16_t delay_ms,
        uint16_t trans_info);

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Structure containing specific callback functions for a Server Model
typedef struct mm_srv_cb
{
    /// Set state value of a local Server Model
    mm_srv_set_cb cb_set;
} mm_srv_cb_t;

/// Structure containing specific callback functions for a Client Model
typedef struct mm_cli_cb
{
    /// Get state value using a Client Model
    mm_cli_get_cb cb_get;
    /// Set state value using a Client Model
    mm_cli_set_cb cb_set;
    /// Initiate transition to a new state value using a Client Model
    mm_cli_transition_cb cb_transition;
} mm_cli_cb_t;

/// Internal Callback Structure
typedef struct mm_cb
{
    /// Inform model about reception of a mesh message with a supported opcode
    mm_rx_cb cb_rx;
    /// Check if an opcode is handled by the model
    mm_opcode_check_cb cb_opcode_check;
    /// Inform model about new publication period
    mm_publish_period_cb cb_publish_param;
    /// Pointer to either callback functions for Server Model or callback functions for Client Model
    union
    {
        /// Pointer to callback functions for Server Model
        mm_srv_cb_t *p_cb_srv;
        /// Pointer to callback functions for Client Model
        mm_cli_cb_t *p_cb_cli;
    } u;
} mm_cb_t;

/// Basic structure for model information
struct mm_tb_state_mdl_env
{
    /// Model ID
    uint32_t mdl_id;
    /// Internal callback functions
    mm_cb_t cb;
    /// Information bit field (@see enum mm_tb_state_mdl_info)
    uint8_t info;
    /// Model Local Index
    m_lid_t mdl_lid;
    /// Group Local Index - filled by Binding Manager
    m_lid_t grp_lid;
    /// Element Index
    uint8_t elmt_idx;
};

/// Basic structure for model information when sending of publications is supported
typedef struct mm_tb_state_mdl_publi_env
{
    /// Basic model environment - Must be first element in the structure - DO NOT MOVE
    mm_tb_state_mdl_env_t env;
    /// Timer for sending of publications
    mesh_tb_timer_t tmr_publi;
    /// Publication period in milliseconds
    uint32_t publi_period_ms;
} mm_tb_state_mdl_publi_env_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Inform the Model State Manager about registration of a new model
 *
 * @param[in] mdl_id        Model Identifier
 * @param[in] elmt_idx      Index of element the model belongs to
 * @param[in] mdl_lid       Local index allocated by the mesh profile for the model
 * @param[in] cfg           Internal configuration
 * @param[in] env_size      Size of environment to allocate for this model
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t mm_tb_state_register(uint8_t elmt_idx, uint32_t mdl_id, m_lid_t mdl_lid,
                              uint8_t cfg, uint16_t env_size);

/**
 ****************************************************************************************
 * @brief Enable the Mesh Model block
 *
 * @return An error status (@see mesh_err)
 ****************************************************************************************
 */
uint16_t mm_tb_state_enable(void);

/**
 ****************************************************************************************
 * @brief Get pointer to environment allocated for a given model
 *
 * @param[in] mdl_lid       Model local index
 *
 * @return Pointer to the environment
 ****************************************************************************************
 */
mm_tb_state_mdl_env_t *mm_tb_state_get_env(m_lid_t mdl_lid);

/**
 ****************************************************************************************
 * @brief Get local index allocated for a given model on a given element
 *
 * @param[in] elmt_idx      Element Index
 * @param[in] mdl_id        Model Index
 ****************************************************************************************
 */
m_lid_t mm_tb_state_get_lid(uint8_t elmt_idx, uint32_t mdl_id);

/**
 ****************************************************************************************
 * @brief Inform State Manager about received publication parameters for a model
 *
 * @param[in] p_env         Pointer to environment of model for which parameters have been
 * received
 * @param[in] addr          Publication address
 * @param[in] period_ms     Publication period in milliseconds
 ****************************************************************************************
 */
void mm_tb_state_publish_param_ind(mm_tb_state_mdl_publi_env_t *p_env,
                                   uint16_t addr, uint32_t period_ms);

/// @} end of group

#endif //_MM_TB_STATE_
