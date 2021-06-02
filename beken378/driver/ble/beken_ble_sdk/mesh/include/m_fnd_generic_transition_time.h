/**
 ****************************************************************************************
 * @file m_fnd_generic_transition_time.h
 *
 * @brief Header file for Mesh Generic_transition Server Model
 *
 * Copyright (C) Beken 2018-2019
 *
 ****************************************************************************************
 */

#ifndef _M_FND_GENERIC_TRANSITION_TIME_H_
#define _M_FND_GENERIC_TRANSITION_TIME_H_


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


#define M_FND_GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE      0x00
#define M_FND_GENERIC_TRANSITION_NUM_STEPS_MAX            0x3E
#define M_FND_GENERIC_TRANSITION_NUM_STEPS_UNKNOWN        0x3F
#define M_IS_GENERIC_TRANSITION_STEPS_VALID(step)     (step < M_FND_GENERIC_TRANSITION_NUM_STEPS_UNKNOWN)


typedef enum
{
    GENERIC_TRANSITION_STEP_RESOLUTION_100MILLISECONDS,
    GENERIC_TRANSITION_STEP_RESOLUTION_1SECOND,
    GENERIC_TRANSITION_STEP_RESOLUTION_10SECONDS,
    GENERIC_TRANSITION_STEP_RESOLUTION_10MINUTS
} generic_transition_step_resolution_t;


#pragma pack(1)
typedef struct
{
    union
    {
        uint8_t num_steps: 6; //!< @ref generic_transition_num_steps_t
        uint8_t step_resolution: 2; //!< @ref generic_transition_step_resolution_t
        /// Vendor model identifier
        uint8_t rem_time;
    };
} generic_transition_time_t, *generic_transition_time_p;
#pragma pack()

#define IS_MAX_GENERIC_TRANSITION_TIME(trans_time) \
    ((trans_time.num_steps == M_FND_GENERIC_TRANSITION_NUM_STEPS_MAX) && \
     (trans_time.step_resolution == GENERIC_TRANSITION_STEP_RESOLUTION_10MINUTS))

typedef void (*generic_transition_step_change_cb)(mm_tb_state_mdl_env_t * pmodel_info,
        uint32_t trans_type,
        generic_transition_time_t total_time,
        generic_transition_time_t remaining_time);


/** @} */

/** @defgroup GENERIC_LEVEL_SERVER_API   generic level server api
  * @brief Functions declaration
  * @{
  */

/**
 * @brief initialize generic transition
 * @retval TRUE: initialize success
 * @retval FALSE: initialize failed
 */
bool generic_transition_time_init(void);

/**
 * @brief start mode state transition timer
 * @param[in] pmodel_info: pointer to model information context that need to transition
 * @param[in] trans_type: transition type
 * @param[in] trans_time: transition total time
 * @retval TRUE: start transition success
 * @retval FALSE: start transition failed
 */
bool generic_transition_timer_start(mm_tb_state_mdl_env_t * pmodel_info,
                                    uint32_t trans_type,
                                    generic_transition_time_t trans_time,
                                    generic_transition_step_change_cb step_change_cb);

/**
 * @brief stop model state transition timer
 * @param[in] pmodel_inf: pointer to model information context that need to stop transition
 * @param[in] trans_type: transition type
 */
void generic_transition_timer_stop(mm_tb_state_mdl_env_t * pmodel_info,
                                   uint32_t trans_type);

/**
 * @brief get model transition remaining time
 * @param[in] pmodel_info: pointer to model information context that in transition
 * @param[in] trans_type: transition type
 * @return remaining transition time
 */
generic_transition_time_t generic_transition_time_get(mm_tb_state_mdl_env_t * pmodel_info,
        uint32_t trans_type);
/** @} */


#endif // 

