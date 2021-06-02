/**
 ****************************************************************************************
 * @file m_fnd_BLOB_Transfer.h
 *
 * @brief Header file for Mesh BLOB Transfer Server Model
 *
 * Copyright (C) Beken 2018-2019
 *
 ****************************************************************************************
 */

#ifndef _M_FND_BLOB_TRANSFER_H_
#define _M_FND_BLOB_TRANSFER_H_

/**
 ****************************************************************************************
 * @defgroup M_FND_BLOB_TRANSFER Mesh Foundation Models Internal Defines
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
//#include "oad_common.h"


#define M_FND_BLOB_TRANSFER_2B_OPCODE(opcode)                                                             \
    (((uint16_t)opcode << 8) | 0xb7)


/// Check if opcode is a TRANSFER
#define M_IS_TRANSFER_OPCODE(opcode)       (((opcode) & 0xFF) == 0xB7)


/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// List of 2-byte opcodes for the  BLOB Transfer Client model
enum m_fnd_blob_transfer_1b_opcodes
{
    // Current  object transfer message
    M_FND_BLOB_OPCODE_OBJ_CHUNK_TRANS              = 0x7D,
    // START object transfer message
    M_FND_BLOB_OPCODE_OBJ_BLK_GET                    = 0x7E,

};

/// List of 2-byte opcodes for the  BLOB Transfer Client model
enum m_fnd_blob_transfer_2b_opcodes
{
    // Current  object transfer message
    M_FND_BLOB_OPCODE_OBJ_TRANS_GET              = 0x01,
    // START object transfer message
    M_FND_BLOB_OPCODE_OBJ_TRANS_START                    = 0x02,
    // Abort object transfer message
    M_FND_BLOB_OPCODE_OBJ_TRANS_ABORT                    = 0x03,
    //Status object transfer message
    M_FND_BLOB_OPCODE_OBJ_TRANS_STATUS                   = 0x04,
    // START object transfer message
    M_FND_BLOB_OPCODE_OBJ_BLK_TRANS_START                    = 0x05,
    //Status object transfer message
    M_FND_BLOB_OPCODE_OBJ_BLK_TRANS_STATUS                   = 0x06,
    //Status object block transfer message
    M_FND_BLOB_OPCODE_OBJ_BLK_STATUS                     = 0x09,
    // object info transfer message get
    M_FND_BLOB_OPCODE_OBJ_INFO_GET                   = 0x0A,
    //Status object block transfer message
    M_FND_BLOB_OPCODE_OBJ_INFO_STATUS                    = 0x0B,

};



enum blob_object_transfer_phase_state
{

    BLOB_IDLE = 0x00,
    BLOB_WAITING_FOR_NEXT_BLOCK = 0x01,
    BLOB_WAITING_FOR_NEXT_CHUNK = 0x02,

};


enum blob_object_transfer_status
{
    //Object transfer is not active.
    BLOB_TRANS_STATUS_READY = 0x00,
    //Object transfer is currently active.
    BLOB_TRANS_STATUS_BUSY = 0x01,
    //Requested object size cannot be supported.
    BLOB_TRANS_STATUS_NOT_SUPPORTED = 0x02,
    //Cannot abort.
    BLOB_TRANS_STATUS_CANNOT_ABORT  = 0x03,
    //Invalid Parameter.
    BLOB_TRANS_STATUS_INVALID_PARAM = 0x04,

};

enum blob_object_blk_transfer_status
{
    //Block transfer accepted.
    BLOB_BLK_TRANS_STATUS_ACCEPTED = 0x00,
    //Block already transferred.
    BLOB_BLK_TRANS_STATUS_DUPLICATIN_BLOCK = 0x01,
    //Requested object size cannot be supported.
    BLOB_BLK_TRANS_STATUS_INVALID_BLOCK_NUMBLE = 0x02,
    //Bigger than the Block Size Log.
    BLOB_BLK_TRANS_STATUS_WRONG_BLOCK_SIZE  = 0x03,
    //Wrong chunk size. Bigger then Block Size divided by Max Chunks Number.
    BLOB_BLK_TRANS_STATUS_WRONG_CHUNK_SIZE = 0x04,
    //Unknown checksum algorithm.
    BLOB_BLK_TRANS_STATUS_UNKNOWN_CHECK_ALGORITHM = 0x05,
    //Model is in the invalid state
    BLOB_BLK_TRANS_STATUS_INVALID_STATE = 0x06,
    //Parameter value cannot be accepted
    BLOB_BLK_TRANS_STATUS_INVALID_PARAM = 0x07,
    //Not All Chunks Received, and checksum is not computed.
    BLOB_BLK_TRANS_STATUS_NOT_ALL_CHUNKS_RECEIVED = 0x08,
    //All chunks received, computed checksum value is not equal to expected value
    BLOB_BLK_TRANS_STATUS_WRONG_CHECKSUM = 0x09,
    //Requested Object ID not active.
    BLOB_BLK_TRANS_STATUS_WRONG_OBJ_ID = 0x0A,
    //Requested block was never received.
    BLOB_BLK_TRANS_STATUS_FRESH_BLOCK = 0x0B,
};


enum blob_object_blk_status
{
    //Block transfer Success.
    BLOB_BLK_STATUS_ALL_CHUNK_RECEVICED = 0x00,
    //Block transfer not all recevice
    BLOB_BLK_STATUS_N0T_ALL_CHUNK_RECEVICED = 0x01,

    BLOB_BLK_STATUS_WRONG_CHECKSUM = 0x02,

    BLOB_BLK_STATUS_WRONG_OBJ_ID  = 0x03,

    BLOB_BLK_STATUS_WRONG_BLK = 0x04,

};



#pragma pack(1)

/// BLOB Transfer phase status message  structure
typedef struct m_fnd_blob_obj_trans_phase_status
{
    //Phase of the object transfer process
    uint8_t Object_Transfer_Phase;
    //Unique object identifier
    uint8_t Object_ID[8];

} m_fnd_blob_obj_trans_phase_status_t;


/// BLOB Transfer phase status message  structure
typedef struct m_fnd_blob_obj_trans_get
{
    //Unique object identifier
    uint8_t Object_ID[8];

} m_fnd_blob_obj_trans_get_t;


/// BLOB Transfer Start message  structure
typedef struct m_fnd_blob_obj_trans_start
{
    //Unique object identifier
    uint8_t Object_ID[8];
    //Object size in bytes
    uint32_t Object_Size;
    //Size of the block during current transfer
    uint8_t Block_Size_Log;

} m_fnd_blob_obj_trans_start_t;

/// BLOB Transfer Abort message  structure
typedef struct m_fnd_blob_obj_trans_abort
{
    //Unique object identifier
    uint8_t Object_ID[8];

} m_fnd_blob_obj_trans_abort_t;


/// BLOB Transfer phase status message  structure
typedef struct m_fnd_blob_obj_trans_status
{
    //Status of operation
    uint8_t Status;
    //Unique object identifier
    uint8_t Object_ID[8];
    //Object size in bytes
    uint32_t Object_Size;
    //Size of the block during current transfer
    uint8_t Block_Size_Log;

} m_fnd_blob_obj_trans_status_t;


/// BLOB Transfer Start message  structure
typedef struct m_fnd_blob_obj_blk_trans_start
{
    //Unique object identifier
    uint8_t Object_ID[8];
    //Block number
    uint16_t Block_Number;
    //Chunk size bytes for this block
    uint16_t Chunk_Size;
    //Checksum type
    uint8_t Block_Checksum_Algorithm;
    //Checksum for image block
    uint32_t Block_Checksum_Value;
    //Block size in bytes
    uint16_t Current_Block_Size;

} m_fnd_blob_obj_blk_trans_start_t;


/// BLOB Object Block Transfer Status message  structure
typedef struct m_fnd_blob_obj_blk_trans_status
{
    //Status of operation
    uint8_t Status;

} m_fnd_blob_obj_blk_trans_status_t;

/// BLOB Chunk Transfer Start message  structure
typedef struct m_fnd_blob_obj_chunk_trans
{
    //Chunk number
    uint16_t Chunk_Number;
    //Part of the object data
    uint8_t Chunk_Data[256];

} m_fnd_blob_obj_chunk_trans_t;


/// BLOB Object Block Get message  structure
typedef struct m_fnd_blob_obj_blk_get
{
    //Unique object identifier
    uint8_t Object_ID[8];
    //Block number
    uint16_t Block_Number;

} m_fnd_blob_obj_blk_get_t;

/// BLOB Object Block Status message  structure
typedef struct m_fnd_blob_obj_blk_status
{
    //Status of operation
    uint8_t Status;
    //Missing chunks list
    uint16_t Missing_Chunks_List[16];

} m_fnd_blob_obj_blk_status_t;


/// BLOB Object info Status message  structure
typedef struct m_fnd_blob_obj_info_status
{
    //Minimum block size:  2 ^ Max Block Size Log
    uint8_t Min_Block_Size_Log;
    //Maximum block size:  2 ^ Max Block Size Log
    uint8_t Max_Block_Size_Log;
    //Supported maximum number of chunks in block
    uint16_t Max_Chunks_Number;

} m_fnd_blob_obj_info_status_t;




typedef struct nvds_mesh_ota_tag
{
    uint8_t Object_ID[8];
    uint16_t Block_Num; //  正常进行trans 的Block Num

    uint16_t company_id;
    uint8_t firmware_id[4];
    uint16_t Current_Block_Size; //  正常进行trans 的 Block size

    uint16_t Receive_Chunk_Mask;

    uint16_t Current_Chunk_Mask;

    uint32_t Block_Checksum_Value;// Block Checksum_Value

    uint8_t apply_flag;
} nvds_mesh_ota_tag_t;

#pragma pack()
/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief BLOB Transfer Client model initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
//uint16_t m_fnd_blob_init(bool reset, void *p_env, const m_cfg_t* p_cfg);
uint16_t m_fnd_blob_init(bool reset);

/**
 ****************************************************************************************
 * @brief Get size of environment used by blob_transfer Client model.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_fnd_blob_get_env_size(const m_cfg_t* p_cfg);



/// @} end of group

#endif //_M_FND_BLOB_TRANSFER_H_
