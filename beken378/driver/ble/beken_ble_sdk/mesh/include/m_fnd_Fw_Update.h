/**
 ****************************************************************************************
 * @file m_fnd_Fw_Update.h
 *
 * @brief Header file for Mesh Firmware Update Server Model
 *
 * Copyright (C) Beken 2018-2019
 *
 ****************************************************************************************
 */

#ifndef _M_FND_FW_UPDATE_H_
#define _M_FND_FW_UPDATE_H_

/**
 ****************************************************************************************
 * @defgroup M_FND_FW_UPDATE Mesh Foundation Models Internal Defines
 * @ingroup MESH
 * @brief Mesh Foundation Models Internal Defines
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "m_config.h"
#include "m_defines.h"
#include "mal.h"
#include "m_tb.h"


/*
 * DEFINES
 ****************************************************************************************
 */

#define M_FND_FW_UPDATE_2B_OPCODE(opcode)                                                             \
    (((uint16_t)opcode << 8) | 0xB6)


/// Check if opcode is a UPDATE
#define M_IS_UPDATE_OPCODE(opcode)       (((opcode) & 0xFF) == 0xB6)

#define FW_UPDATE_FW_ID_LEN                             4
typedef uint8_t *fw_update_fw_id_t;
/*
 * ENUMERATIONS
 ****************************************************************************************
 */
#pragma pack(1)
// List of 2-byte opcodes for the Firmware Update Client model
enum m_fnd_fw_update_2b_opcodes
{
    // Current Firmware Information Get
    M_FND_FW_UPDATE_OPCODE_INFO_GET              = 0x01,
    // Firmware Information Status
    M_FND_FW_UPDATE_OPCODE_INFO_STATUS,
    // Firmware Update Get
    M_FND_FW_UPDATE_OPCODE_UPDATE_GET,
    // Firmware Update Prepare
    M_FND_FW_UPDATE_OPCODE_UPDATE_PREPARE               =  0x04,
    // Firmware Update Prepare Start
    M_FND_FW_UPDATE_OPCODE_UPDATE_START,
    // Firmware Update Prepare Abort
    M_FND_FW_UPDATE_OPCODE_UPDATE_ABORT,
    // Firmware Update Prepare Apply
    M_FND_FW_UPDATE_OPCODE_UPDATE_APPLY,
    // Firmware Update Prepare Status
    M_FND_FW_UPDATE_OPCODE_UPDATE_STATUS            = 0x08,

};




// Firmware Update Info Status message structure
typedef struct m_fnd_fw_update_info_status
{
    // Company identifier
    uint16_t Company_ID;
    // Unique firmware identifier
    uint8_t Firmware_ID[FW_UPDATE_FW_ID_LEN];
    // URL for update source (optional)
    uint8_t Update_URL[1];
} m_fnd_fw_update_info_status_t;

// Firmware Update Get message structure
typedef struct m_fnd_fw_update_get
{
    // Company identifier
    uint16_t Company_ID;
    // Unique firmware identifier
    uint8_t Firmware_ID[FW_UPDATE_FW_ID_LEN];

} m_fnd_fw_update_get_t;


// Firmware Update Prepare message structure
typedef struct m_fnd_fw_update_prepare
{
    // Company identifier
    uint16_t Company_ID;
    // Unique firmware identifier
    uint8_t Firmware_ID[ FW_UPDATE_FW_ID_LEN];
    // Unique object identifier
    uint8_t Object_ID[8];
    // Vendor specific validation data for update (optional)
    uint8_t Vendor_validation_data[1];

} m_fnd_fw_update_prepare_t;

typedef enum
{
    FW_UPDATE_POLICY_NONE, //!< Do not apply new firmware when Object transfer is completed.
    FW_UPDATE_POLICY_AUTO_UPDATE //!< Apply new firmware when Object transfer is completed.
} m_fnd_fw_update_policy_t;

// Firmware Update Start message structure
typedef struct m_fnd_fw_update_start
{
    // Firmware update policy
    uint8_t Update_Policy;
    // Company identifier
    uint16_t Company_ID;
    // Unique firmware identifier
    uint8_t Firmware_ID[FW_UPDATE_FW_ID_LEN];
} m_fnd_fw_update_start_t;

// Firmware Update Abort message structure
typedef struct m_fnd_fw_update_abort
{
    // Company identifier
    uint16_t Company_ID;
    // Unique firmware identifier
    uint8_t Firmware_ID[FW_UPDATE_FW_ID_LEN];
} m_fnd_fw_update_abort_t;


enum fw_update_phase
{
    //No DFU update in progress
    FW_UPDATE_PHASE_IDLE = 0x00,
    //DFU update is prepared and awaiting start
    FW_UPDATE_PHASE_PREPARED = 0x01,
    //DFU update is in progress
    FW_UPDATE_PHASE_IN_PROGRESS = 0x02,
    //DFU upload is finished and waiting to be apply
    FW_UPDATE_PHASE_WAITING_APPLY = 0x03,

};

enum fw_update_status
{
    //  0x00 - success
    FW_UPDATE_STAT_SUCCESS = 0x00,
    //  0x01 - wrong Company ID and Firmware ID combination
    FW_UPDATE_STAT_WRONG_COMPANY_FIRMWARE_COMBINATION = 0x01,
    //  0x02 - different object transfer already ongoing
    FW_UPDATE_STAT_DIFF_OBJECT_TRANSFER_ALREADY_ONGOING= 0x02,
    //  0x03 - Company ID and Firmware ID combination apply failed
    FW_UPDATE_STAT_COMPANY_FIRMWARE_APPLY_FAILED = 0x03,
    //  0x04 - Company ID and Firmware ID combination permanently rejected, newer firmware version present
    FW_UPDATE_STAT_NEWER_FW_VERSION_PRESENT = 0x04,
    //  0x05 - Company ID and Firmware ID combination temporary rejected, node is not able to accept new firmware now, try again later
    FW_UPDATE_STAT_NOT_AVAILABLE = 0x05,
};
// Firmware Update Apply message structure
typedef struct m_fnd_fw_update_apply
{
    // Company identifier
    uint16_t Company_ID;
    // Unique firmware identifier
    uint8_t Firmware_ID[FW_UPDATE_FW_ID_LEN];
} m_fnd_fw_update_apply_t;

// Firmware Update Status message structure
typedef struct m_fnd_fw_update_status
{
    // Status code of the operation
    uint8_t Status;
    // Phase of the update
    uint8_t Phase : 3;  // 3 bits
    // Additional Information
    uint8_t Additional_Information :5;  // 5 bits
    // Company identifier
    uint16_t Company_ID;
    // Unique firmware identifier
    uint8_t Firmware_ID[FW_UPDATE_FW_ID_LEN];
    // Unique object identifier
    uint8_t Object_ID[8];
} m_fnd_fw_update_status_t;



#pragma pack()
/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Firmware Update Client model initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
//uint16_t m_fnd_fw_update_init(bool reset, void *p_env, const m_cfg_t* p_cfg);
uint16_t m_fnd_fw_update_init(bool reset);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Firmware Update Client model.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_fnd_fw_update_get_env_size(const m_cfg_t* p_cfg);


/// @} end of group

#endif //_M_FND_FIRM_UPD_H_
