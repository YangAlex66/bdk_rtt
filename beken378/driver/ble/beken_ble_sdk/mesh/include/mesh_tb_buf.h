/**
 ****************************************************************************************
 *
 * @file mesh_tb_buf.h
 *
 * @brief Header file for Mesh Buffer Manager
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MESH_TB_BUF_H_
#define _MESH_TB_BUF_H_

/**
 ****************************************************************************************
 * @defgroup MESH_TB_BUF Mesh Buffer Manager
 * @ingroup MESH
 * @brief Mesh Buffer Manager
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh_inc.h"       // Mesh Stack Includes

/*
 * DEFINES
 ****************************************************************************************
 */

/// Debug Pattern
#define MESH_TB_BUF_DBG_PATTERN         (0xA55A5AA5)
/// Size of environment part in buffer provided by the Buffer Manager
#define MESH_TB_BUF_ENV_SIZE            (32)

/*
 * MACROS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Get pointer to data part of a provided buffer.
 *
 * @param[in] p_buf     Pointer to buffer structure.
 *
 * @return Address of data part in the provided buffer.
 ****************************************************************************************
 */
#define MESH_TB_BUF_DATA(p_buf)        (&(p_buf)->buf[0] + (p_buf)->head_len)

/**
 ****************************************************************************************
 * @brief Get pointer to tail part of a provided buffer.
 *
 * @param[in] p_buf     Pointer to buffer structure.
 *
 * @return Address to tail part in the provided buffer.
 ****************************************************************************************
 */
#define MESH_TB_BUF_TAIL(p_buf)        (&(p_buf)->buf[0] + (p_buf)->head_len + (p_buf)->data_len)

/**
 ****************************************************************************************
 * @brief Get pointer to head part of a provided buffer.
 *
 * @param[in] p_buf     Pointer to buffer structure.
 *
 * @return Address of head part in the provided buffer.
 ****************************************************************************************
 */
#define MESH_TB_BUF_HEAD(p_buf)        (&(p_buf)->buf[0])

/*
 * CALLBACK DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Callback executed when a buffer block has been properly released.
 *
 * @param[in] block_id  Buffer Block identifier
 ****************************************************************************************
 */
typedef void (*mesh_tb_buf_block_released_cb)(uint8_t block_id);



typedef void (*mm_route_sent_cb) (void);
/*
 * STRUCTURES
 ****************************************************************************************
 */

/// Buffer information structure
typedef struct mesh_tb_buf
{
    /// List header for chaining
    common_list_hdr_t hdr;
    /// Length of the head part
    uint16_t head_len;
    /// Length of the tail part
    uint16_t tail_len;
    /// Length of the data part
    uint16_t data_len;
    /// Total size of the buffer array
    uint16_t buf_len;
    /// Index of block the buffer belongs to
    /// Set to @see MESH_TB_BUF_DYN_ALLOC if dynamically allocated
    uint8_t block_id;
    /// Index of the buffer in the block if buffer belongs to a block
    uint8_t buf_id;
    /// Acquire counter
    uint8_t acq_cnt;
    /// Dummy Tag that can be used to inform a layer about type of buffer.
    /// Value should not be kept along several layers
    uint8_t dummy_tag;
    uint32_t re_tx_flag;

    mm_route_sent_cb  sent_cb;
    /// Environment variable that can be used for multiple purposes
    uint8_t env[MESH_TB_BUF_ENV_SIZE];
#if (BLE_MESH_DBG)
    /// Debug pattern used to verify that content of environment part did not overwritten the
    /// buffer part. Also used to verify that pointed buffer is well a buffer
    uint32_t pattern;
#endif //(BLE_MESH_DBG)
    /// Data buffer containing the PDU
    /// Length is head_len + tail_len + data_len
    uint8_t buf[__ARRAY_EMPTY];
} mesh_tb_buf_t;

#if (MESH_MEM_TB_BUF_DBG)

#define M_MEM_DBG_INFO_MAX    100

typedef struct
{
    void *p;
    uint16_t size;
    uint16_t line;
    const char *func;
} m_mem_dbg_info_t;

#define mesh_tb_buf_alloc(pp_buf, head_len, data_len, tail_len)                          \
({                                                                                       \
    uint16_t status = mesh_tb_buf_alloc_dbg(pp_buf, head_len, data_len, tail_len);       \
    mesh_mem_dbg_record(*pp_buf, sizeof(mesh_tb_buf_t) + head_len + data_len + tail_len, \
                        __func__, __LINE__);                                             \
    (status);                                                                            \
})

#define mesh_tb_buf_release(p_buf)                                            \
({                                                                            \
    uint16_t status;                                                          \
    void *tmp_point = (void *)(p_buf);                                      \
    if ((status = mesh_tb_buf_release_dbg(p_buf)) == MESH_ERR_NO_ERROR)     \
    {                                                                        \
        mesh_mem_dbg_clean(tmp_point, __func__, __LINE__);                  \
    }                                                                        \
    (status);                                                               \
})

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

 void mesh_mem_dbg_init(void);

 void mesh_mem_dbg_record(void *p, uint16_t size, const char *func, uint16_t line);

 void mesh_mem_dbg_clean(void *p, const char *func, int line);

 void mesh_mem_dbg_show(void);

 #endif /* MESH_MEM_TB_BUF_DBG */

/**
 ****************************************************************************************
 * @brief Allocate a block of small or long buffers.
 * Size of small buffers is @see MESH_BUF_SMALL_SIZE.
 * Size of long buffers is @see MESH_BUF_LONG_SIZE.
 *
 * @param[out] p_block_id        Pointer to the variable that will contain the index of the allocated
 *                               block.
 * @param[in] nb_bufs            Number of buffers contained in the block.
 * @param[in] small              Indicate if block contains small or long buffers.
 *
 * @return MESH_ERR_NO_ERROR if block has been successfully allocated.
 *         MESH_ERR_INVALID_PARAM if provided number of blocks is invalid.
 *         MESH_ERR_NO_RESOURCES if no more block can be allocated.
 ****************************************************************************************
 */
uint16_t mesh_tb_buf_block_alloc(uint8_t *p_block_id, uint8_t nb_bufs, bool small);

/**
 ****************************************************************************************
 * @brief Free an existing block. As some of the buffers belonging to the block can be
 * in use, it might happen that the block cannot be freed upon reception of the request.
 * Once all the buffer are released, the block is freed and @see m_api_buf_free_blockd function
 * is called.
 *
 * @param[in] block_id     Index of the block to be freed.
 * @param[in] cb_released  Callback executed when a buffer is released
 * once block has been freed.
 *
 * @return MESH_ERR_NO_ERROR if block exists and can be freed.
 *         MESH_ERR_INVALID_PARAM if indicated buffer does not exists.
 ****************************************************************************************
 */
uint16_t mesh_tb_buf_block_free(uint8_t block_id, mesh_tb_buf_block_released_cb cb_released);

/**
 ****************************************************************************************
 * @brief Allocate a buffer and specify initial length of head, data and tail parts.
 * If total length of the buffer is higher than @see MESH_BUF_LONG_SIZE, the buffer will be
 * dynamically allocated.
 * If total length is lower than @see MESH_BUF_SMALL_SIZE, a small buffer will be allocated.
 * Else a long buffer is used.
 *
 * @param[out] p_bu   f    Pointer to a variable that will contain the address of the allocated
 *                         buffer.
 * @param[in] head_len     Initial Head Length.
 * @param[in] data_len     Initial Data Length.
 * @param[in] tail_len     Initial Tail Length.
 *
 * @return MESH_ERR_NO_ERROR if buffer can be allocated.
 *         MESH_ERR_NO_RESOURCES if no more buffers are available.
 ****************************************************************************************
 */
#if (MESH_MEM_TB_BUF_DBG)
uint16_t mesh_tb_buf_alloc_dbg(mesh_tb_buf_t **pp_buf, uint16_t head_len, 
                                           uint16_t data_len, uint16_t tail_len);
#else /* !MESH_MEM_TB_BUF_DBG */
uint16_t mesh_tb_buf_alloc(mesh_tb_buf_t **pp_buf, uint16_t head_len, 
                                     uint16_t data_len, uint16_t tail_len);
#endif /* MESH_MEM_TB_BUF_DBG */

/**
 ****************************************************************************************
 * @brief Reserve bytes in the head part in order to increase length of data part.
 *
 * @param[in] p_buf        Pointer to buffer in which length of data part must be increased.
 * @param[in] len          Length to be reserved.
 *
 * @return MESH_ERR_NO_ERROR if needed length has been reserved.
 *         MESH_ERR_INVALID_PARAM if provided length is higher than current length of head part.
 ****************************************************************************************
 */
uint16_t mesh_tb_buf_head_reserve(mesh_tb_buf_t *p_buf, uint16_t len);

/**
 ****************************************************************************************
 * @brief Release bytes in the data part in order to increase length of head part.
 *
 * @param[in] p_buf        Pointer to buffer in which length of head part must be increased.
 * @param[in] len          Length to be released.
 *
 * @return MESH_ERR_NO_ERROR if needed length has been released.
 *         MESH_ERR_INVALID_PARAM if provided length is higher than current length of data part.
 ****************************************************************************************
 */
uint16_t mesh_tb_buf_head_release(mesh_tb_buf_t *p_buf, uint16_t len);

/**
 ****************************************************************************************
 * @brief Reserve bytes in the tail part in order to increase length of data part.
 *
 * @param[in] p_buf        Pointer to buffer in which length of data part must be increased.
 * @param[in] len          Length to be reserved.
 *
 * @return MESH_ERR_NO_ERROR if needed length has been reserved.
 *         MESH_ERR_INVALID_PARAM if provided length is higher than current length of tail part.
 ****************************************************************************************
 */
uint16_t mesh_tb_buf_tail_reserve(mesh_tb_buf_t *p_buf, uint16_t len);

/**
 ****************************************************************************************
 * @brief Release bytes in the data part in order to increase length of tail part.
 *
 * @param[in] p_buf        Pointer to buffer in which length of tail part must be increased.
 * @param[in] len          Length to be released.
 *
 * @return MESH_ERR_NO_ERROR if needed length has been released.
 *         MESH_ERR_INVALID_PARAM if provided length is higher than current length of data part.
 ****************************************************************************************
 */
uint16_t mesh_tb_buf_tail_release(mesh_tb_buf_t *p_buf, uint16_t len);

/**
 ****************************************************************************************
 * @brief Request to increment value of acquire counter of a buffer during processing of buffer content.
 *
 * @param[in] p_buf        Pointer to acquired buffer
 ****************************************************************************************
 */
void mesh_tb_buf_acquire(mesh_tb_buf_t *p_buf);

/**
 ****************************************************************************************
 * @brief Request to release previously acquired buffer. The acquire counter for this buffer
 * is decremented. If the acquire counter value becomes zero, the buffer is freed as no more
 * entity is using the buffer anymore.
 *
 * @param[in] p_buf        Pointer to acquired buffer.
 *
 * @return MESH_ERR_NO_ERROR if buffer has been released.
 *         MESH_ERR_COMMAND_DISALLOWED if buffer was free.
 ****************************************************************************************
 */
#if (MESH_MEM_TB_BUF_DBG)
uint16_t mesh_tb_buf_release_dbg(mesh_tb_buf_t *p_buf);
#else /* !MESH_MEM_TB_BUF_DBG */
uint16_t mesh_tb_buf_release(mesh_tb_buf_t *p_buf);
#endif /* MESH_MEM_TB_BUF_DBG */

/**
 ****************************************************************************************
 * @brief Copy content of a buffer to another buffer.
 *
 * @param[in] p_buf_in        Pointer to the buffer to copy.
 * @param[in] p_buf_out       Pointer to the buffer that will contain content of input buffer.
 * @param[in] length          Length of data to copy
 * @param[in] copy_env        Indicate if environment has to be copied.
 *
 * @return MESH_ERR_NO_ERROR if copy has been properly performed.
 *         MESH_ERR_COMMAND_DISALLOWED if the two provided buffers have different sizes.
 ****************************************************************************************
 */
uint16_t mesh_tb_buf_copy(mesh_tb_buf_t *p_buf_in, mesh_tb_buf_t *p_buf_out, uint16_t length, bool copy_env);

/**
 ****************************************************************************************
 * @brief Copy content of a data buffer to the data part of a buffer.
 *
 * @param[in] p_buf       Pointer to the buffer in which indicated data will be copied.
 * @param[in] p_mem       Pointer to data buffer containing the data to copy
 * @param[in] len         Length of data to copy
 *
 * @return MESH_ERR_NO_ERROR if copy has been properly performed.
 *         MESH_ERR_COMMAND_DISALLOWED if length of data part in the buffer in different than
 * provided length.
 ****************************************************************************************
 */
uint16_t mesh_tb_buf_copy_data_from_mem(mesh_tb_buf_t *p_buf, const uint8_t *p_mem, uint16_t len);

/**
 ****************************************************************************************
 * @brief Copy content of buffer data part in a data buffer.
 *
 * @param[in] p_buf       Pointer to the buffer in which indicated data will be read.
 * @param[in] p_mem       Pointer to data buffer that will contain the read data.
 ****************************************************************************************
 */
void mesh_tb_buf_copy_data_to_mem(mesh_tb_buf_t *p_buf, uint8_t *p_mem);

/**
 ****************************************************************************************
 * @brief Initialize a buffer with initial length of head, data and tail parts.
 * Idea is to reuse an already allocated buffer for a different purpose
 *
 * @param[in] p_buf        Pointer to the buffer in which indicated data will be read.
 * @param[in] head_len     Initial Head Length.
 * @param[in] data_len     Initial Data Length.
 * @param[in] tail_len     Initial Tail Length.
 *
 * @return MESH_ERR_NO_ERROR if copy has been properly performed.
 *         MESH_ERR_INVALID_PARAM if length fields exceed length of initial buffer
 ****************************************************************************************
 */
uint16_t mesh_tb_buf_reuse(mesh_tb_buf_t *p_buf, uint16_t head_len, uint16_t data_len, uint16_t tail_len);



void mesh_tb_buf_get_free_num(uint8_t *small_cnt, uint8_t *long_cnt);


/// @} end of group

#endif //_MESH_TB_BUF_H_
