/**
 ****************************************************************************************
 *
 * @file mm_lightc_int.h
 *
 * @brief Header file for Mesh Model Light Client Module Internal Definitions
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MM_LIGHTC_INT_H_
#define _MM_LIGHTC_INT_H_

/**
 ****************************************************************************************
 * @defgroup MM_LIGHTC Mesh Model Light Client Module
 * @ingroup MESH_MDL
 * @brief Mesh Model Light Client Module Internal Definitions
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mm_lightc.h"      // Mesh Model Generic Client Module Definitions
#include "mm_api_int.h"     // Mesh Model Application Programming Interface Internal Definitions

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Register Light Lightness Client model. Model is registered on first element.
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_lightc_ln_register(void);

/**
 ****************************************************************************************
 * @brief Register Light CTL Client model. Model is registered on first element.
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_lightc_ctl_register(void);

/**
 ****************************************************************************************
 * @brief Register Light HSL Client model. Model is registered on first element.
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_lightc_hsl_register(void);

/**
 ****************************************************************************************
 * @brief Register Light XYL Client model. Model is registered on first element.
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_lightc_xyl_register(void);

/// @} end of group

#endif //_MM_LIGHTC_LN_INT_
