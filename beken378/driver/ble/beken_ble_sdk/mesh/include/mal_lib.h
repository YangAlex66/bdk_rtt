/**
 ****************************************************************************************
 *
 * @file mal_lib.h
 *
 * @brief Header file for Mesh Abstraction Layer Library
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef MAL_LIB_
#define MAL_LIB_

/**
 ****************************************************************************************
 * @defgroup MAL Mesh Abstraction Layer
 * @ingroup MESH
 * @brief  Mesh Abstraction Layer Library
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <string.h>         // For byte array handling (memcpy, memset, ...)
#include <stdint.h>         // For variable type definitions (uint8_t, uint16_t, ...)
#include <stdbool.h>        // For use of boolean variables
#include "architect.h"           // Platform Definitions (for use of BLE_ASSERT_XXX functions)
#include "common_list.h"        // List Management
#include "common_math.h"        // For use of COMMON_BIT macro
#include "common_utils.h"       // For use of GETB, SETB, GETF, SETF macros
#include "common_endian.h"      // For endianness management

/*
 * FUNCTION WRAPPERS
 ****************************************************************************************
 */

/// @} MAL

#endif /* MAL_LIB_ */
