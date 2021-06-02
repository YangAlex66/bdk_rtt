/**
 ****************************************************************************************
 *
 * @file m_version.h
 *
 * @brief Header file for Mesh Version
 *
 * Copyright (C) RivieraWaves 2017-2018
 *
 ****************************************************************************************
 */

#ifndef M_VERSION_
#define M_VERSION_

/**
 ****************************************************************************************
 * @defgroup MESH Mesh stack
 * @ingroup ROOT
 * @brief  Mesh Stack
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "common_version.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Mesh Specification version
#define MESH_VERSION_MAJOR                   (1)

/// Mesh Integration version (BLE core version)
#define MESH_VERSION_MINOR                   (RWBT_SW_VERSION_MAJOR)

/// Mesh SW version
#define MESH_VERSION_BUILD                   (0)

/// Mesh Sub SW version - if needed
#define MESH_VERSION_SUB_BUILD               (0)

/// @} MESH

#endif /* M_VERSION_ */
