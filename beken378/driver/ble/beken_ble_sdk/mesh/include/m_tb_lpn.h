/**
 ****************************************************************************************
 * @file m_tb_lpn.h
 *
 * @brief Header file for Friendship Manager (Low Power Node)
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _M_TB_LPN_H_
#define _M_TB_LPN_H_

/**
 ****************************************************************************************
 * @defgroup M_TB_LPN Friendship Manager (Low Power Node)
 * @ingroup MESH
 * @brief Friendship Manager (Low Power Node)
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "m_api.h"      // Mesh Application Programming Interface
#include "m_config.h"       // Mesh Profile Configuration

#if (BLE_MESH_LPN)

/*
 * TYPE DEFINTIONS
 ****************************************************************************************
 */

/// Structure containing information about a friendship established as Low Power Node
typedef struct m_tb_lpn_info
{
    /// Poll interval in milliseconds
    uint32_t poll_intv_ms;
    /// LPN Counter
    uint16_t lpn_cnt;
    /// Address of Friend node
    uint16_t friend_addr;
    /// Friend Counter
    uint16_t friend_cnt;
    /// Receive delay in milliseconds
    uint8_t  rx_delay_ms;
    /// Receive window in milliseconds
    uint8_t  rx_window_ms;
    /// Friend subscription list size
    uint8_t  subs_list_size;
    /// Friend Sequence Number
    uint8_t  fsn;
} m_tb_lpn_info_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Return structure containing information about the friendship established
 * as a Low Power Node.
 ****************************************************************************************
 */
m_tb_lpn_info_t *m_tb_lpn_get_info(void);

/**
 ****************************************************************************************
 * @brief Indicate that a friendship with a Friend Node has been added
 *
 * @param[in] p_info        Pointer to structure containing information about friendship
 *
 * @return Error status
 ****************************************************************************************
 */
uint16_t m_tb_lpn_add(m_tb_lpn_info_t *p_info);

/**
 ****************************************************************************************
 * @brief Request to remove information about current friendship with a Friend Node
 ****************************************************************************************
 */
void m_tb_lpn_rem(void);

/**
 ****************************************************************************************
 * @brief Get information about current friendship with a Friend Node
 ****************************************************************************************
 */
m_tb_lpn_info_t *m_tb_lpn_get_info(void);

#if (!BLE_MESH_STORAGE_NONE)
/**
 ****************************************************************************************
 * @brief Load information about friendship with a Friend Node
 *
 * @param[in] p_info            Pointer to friendship information
 * @param[in] end_cb            Callback function called once friendship has been added
 *
 * @return Error status
 ****************************************************************************************
 */
uint16_t m_tb_lpn_load(m_tb_lpn_info_t *p_info, m_api_end_cb end_cb);
#endif //(!BLE_MESH_STORAGE_NONE)

#endif //(BLE_MESH_LPN)

/// @} end of group

#endif //_M_TB_LPN_H_
