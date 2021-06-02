/**
 ****************************************************************************************
 *
 * @file mesh_tb_timer.h
 *
 * @brief Header file for Mesh Timer Manager
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef _MESH_TB_TIMER_H_
#define _MESH_TB_TIMER_H_

/**
 ****************************************************************************************
 * @defgroup MESH_TB_TIMER Mesh Timer Manager
 * @ingroup MESH
 * @brief Mesh Timer Manager
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "mesh_inc.h"       // Mesh Stack Includes

/*
 * CALLBACK DEFINITIONS
 ****************************************************************************************
 */

/// Pointer to callback function called when timer expires
typedef void (*mesh_tb_timer_cb_t)(void *);

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// Structure defining the properties of a timer
typedef struct mesh_tb_timer
{
    /// List element for chaining
    common_list_hdr_t hdr;
    /// Function to be called upon timer expiration
    mesh_tb_timer_cb_t cb;
    /// Pointer to be passed to the callback
    void *p_env;
    /// Expiration time in milliseconds
    uint32_t time_ms;
    /// Number of wraps
    uint16_t nb_wrap;
    /// Not used by timer module but by client, this variable is used to save timer period information.
    /// There is no unity to let client use it with any kind of timing step.
    /// It is recommended to use it only to keep period an not other information
    uint32_t period;
} mesh_tb_timer_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Program a timer to be scheduled in the future.
 *
 * @param[in] p_timer     Pointer to the timer structure.
 *                        "time" parameter is filled by the function based on indicated delay value
 * @param[in] delay       Duration before expiration of the timer (in milliseconds)
 ****************************************************************************************
 */
void mesh_tb_timer_set(mesh_tb_timer_t *p_timer, uint32_t delay_ms);

/**
 ****************************************************************************************
 * @brief Clear a programmed timer.
 * This function searches for the timer passed as parameter in the list of programmed
 * timers and extract it from the list.
 *
 * @param[in] p_timer   Pointer to the timer to be cleared
 ****************************************************************************************
 */
void mesh_tb_timer_clear(mesh_tb_timer_t *p_timer);

/**
 ****************************************************************************************
 * @brief Get current time value
 *
 * @param[out] p_time_ms     Pointer to variable that contains current time (ms part)
 * @param[out] p_nb_wrap     Pointer to variable that contains current time (wrap part)
 ****************************************************************************************
 */
void mesh_tb_timer_get_cur_time(uint32_t *p_time_ms, uint16_t *p_nb_wrap);

/**
 ****************************************************************************************
 * @brief Get remaining duration knowing start time and delay
 *
 * @param[in] delay_ms          Delay in milliseconds
 * @param[in] time_start_ms     Start time (ms part)
 * @param[in] nb_wrap_start     Start time (wrap part)
 ****************************************************************************************
 */
uint32_t mesh_tb_timer_get_rem_duration(uint32_t delay_ms, uint32_t time_start_ms, uint16_t nb_wrap_start);

/**
 ****************************************************************************************
 * @brief Get remaining duration before expiration for a given timer
 *
 * @param[in] p_timer       Pointer to the timer
 ****************************************************************************************
 */
uint32_t mesh_tb_timer_get_tmr_rem_duration(mesh_tb_timer_t *p_timer);

/// @} end of group

#endif //_MESH_TB_TIMER_H_
