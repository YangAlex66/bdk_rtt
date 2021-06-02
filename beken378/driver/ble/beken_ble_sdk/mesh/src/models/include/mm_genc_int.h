/**
 ****************************************************************************************
 *
 * @file mm_genc_int.h
 *
 * @brief Header file for Mesh Model Generic Client Module Internal Definitions
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MM_GENC_INT_H_
#define _MM_GENC_INT_H_

/**
 ****************************************************************************************
 * @defgroup MM_GENC Mesh Model Generic Client Module
 * @ingroup MESH_MDL
 * @brief Mesh Model Generic Client Module Internal Definitions
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mm_genc.h"        // Mesh Model Generic Client Module Definitions
#include "mm_api_int.h"     // Mesh Model Application Programming Interface Internal Definitions

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Register Generic OnOff Client model. Model is registered on first element.
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_genc_oo_register(void);

/**
 ****************************************************************************************
 * @brief Register Generic Level Client model. Model is registered on first element.
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_genc_lvl_register(void);

/**
 ****************************************************************************************
 * @brief Register Generic Default Transition Time Client model. Model is registered on first element.
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_genc_dtt_register(void);

/**
 ****************************************************************************************
 * @brief Register Generic Power OnOff Client model. Model is registered on first element.
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_genc_poo_register(void);

/**
 ****************************************************************************************
 * @brief Register Generic Power Loevel Client model. Model is registered on first element.
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_genc_plvl_register(void);

/**
 ****************************************************************************************
 * @brief Register Generic Battery Client model. Model is registered on first element.
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_genc_bat_register(void);

/**
 ****************************************************************************************
 * @brief Register Generic Location Client model. Model is registered on first element.
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_genc_loc_register(void);

/**
 ****************************************************************************************
 * @brief Register Generic Property Client model. Model is registered on first element.
 *
 * @return An error status (@see enum mesh_err)
 ****************************************************************************************
 */
uint16_t mm_genc_prop_register(void);

/// @} end of group

#endif //_MM_GENC_INT_
