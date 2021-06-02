/**
 ****************************************************************************************
 * @file m_fnd_Scenes.h
 *
 * @brief Header file for Mesh Scenes Server Model
 *
 * Copyright (C) Beken 2018-2019
 *
 ****************************************************************************************
 */

#ifndef _M_FND_SCENES_H_
#define _M_FND_SCENES_H_

/**
 ****************************************************************************************
 * @defgroup M_FND_SCENES Mesh Foundation Models Internal Defines
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
#include "m_fnd_generic_transition_time.h"
#include "m_fnd_int.h"

/*
 * DEFINES
 ****************************************************************************************
 */


/// Length of Scene Model Status message
#define M_FND_SCENES_MODEL_STATUS_LEN              (6)

/// Length of Scene Model Status message
#define M_FND_SCENES_MODEL_REGISTER_STATUS_MIN_LEN              (3)//MIN LEN

/// Length of Scene Model Store message
#define M_FND_SCENES_MODEL_STORE_LEN                        (2)
/// Length of Scene Model Store message
#define M_FND_SCENES_MODEL_STORE_UNACK_LEN                        (2)
/// Length of Scene Model Store message
#define M_FND_SCENES_MODEL_DELETE_LEN                        (2)
/// Length of Scene Model Store message
#define M_FND_SCENES_MODEL_DELETE_UNACK_LEN                        (2)


#define M_FND_SCENES_STORE_MAX                      (14)
/*
* ENUMERATIONS
****************************************************************************************
*/

typedef struct
{
    uint16_t scene_number;
} m_fnd_scene_server_get_present_t;

typedef struct
{
    uint16_t scenes[M_FND_SCENES_STORE_MAX];
} m_fnd_scene_server_get_register_t;


typedef struct
{
    generic_transition_time_t trans_time;
} m_fnd_scene_server_get_default_transition_time_t;

typedef struct
{
    uint16_t scene_number;
    generic_transition_time_t total_time;
    generic_transition_time_t remaining_time;
} m_fnd_scene_server_set_t;


/*
 * MACROS
 ****************************************************************************************
 */
#define M_FND_SCENES_2B_OPCODE(opcode)                                                             \
    (((uint16_t)opcode << 8) | 0x82)



/// Check if opcode is a SCENES
#define M_IS_SCENES_OPCODE(opcode)       (((opcode) & 0xFF) == 0x82)

#define M_IS_SCENES_VALID(val)   (val > 0)


/** @defgroup SCENE_TRANSITION_TYPE generic on off transition type
  * @brief
  * @{
  */
#define GENERIC_TRANSITION_TYPE_SCENE   0




/** @defgroup GENERIC_SCENE_SERVER_DATA generic on off server data
  * @brief Data types and structure used by data process callback
  * @{
  */
#define M_FND_SCENE_SERVER_GET_PRESENT                                  0 //!<
#define M_FND_SCENE_SERVER_GET_REGISTER                                       1 //!<
#define M_FND_SCENE_SERVER_GET_DEFAULT_TRANSITION_TIME                2 //!< 
#define M_FND_SCENE_SERVER_REGISTER                                   3 //!<
#define M_FND_SCENE_SERVER_STORE                                      4 //!< 
#define M_FND_SCENE_SERVER_RECALL                                      5 //!< 
#define M_FND_SCENE_SERVER_DELETE                                     6 //!< 

/// List of 1-byte opcodes for the Scene Client model
enum m_fnd_scenes_1b_opcodes
{
    // Current Scene status message
    M_FND_SCENES_OPCODE_STATUS                      = 0x5E,
};

/// List of 2-byte opcodes for the Scene Client model
enum m_fnd_scenes_2b_opcodes
{
    // Current Scene get message
    M_FND_SCENES_OPCODE_GET                     = 0x41,
    // Scene Recall message
    M_FND_SCENES_OPCODE_RECALL                      = 0x42,
    // Scene Recall Unack message
    M_FND_SCENES_OPCODE_RECALL_UNACK                    = 0x43,
    // Current Scene register get message
    M_FND_SCENES_OPCODE_REGISTER_GET                    = 0x44,
    // Current Scene register status message
    M_FND_SCENES_OPCODE_REGISTER_STATUS                     = 0x45,
    // Scene Store message
    M_FND_SCENES_OPCODE_STORE                   = 0x46,
    // Scene Store Unack message
    M_FND_SCENES_OPCODE_STORE_UNACK                     = 0x47,
    // Scene Store message
    M_FND_SCENES_OPCODE_DELETE                      = 0x9E,
    // Scene delete Unack message
    M_FND_SCENES_OPCODE_DELETE_UNACK                    = 0x9F,

};

enum scenes_status
{
    SCENES_SUCCESS,
    SCENES_REGISTER_FULL,
    SCENES_NOT_FOUND,
};



/*
 * MESSAGE STRUCTURES
 ****************************************************************************************
 */

#pragma pack(1)


/// Scene get message structure
typedef struct m_fnd_scenes_get
{
    /// The number of the scene to be recalled.
    uint16_t scene_number;
} m_fnd_scenes_get_t;
/// Scene recall message structure
typedef struct m_fnd_scenes_recall
{
    /// The number of the scene to be recalled.
    uint16_t scene_number;
    // Transaction Identifier
    uint8_t tid;
    // 0b00 The Default Transition Step Resolution is 100 milliseconds
    // 0b01 The Default Transition Step Resolution is 1 second
    // 0b10 The Default Transition Step Resolution is 10 seconds
    // 0b11 The Default Transition Step Resolution is 10 minutes
    generic_transition_time_t trans_time;
    // Message execution delay in 5 millisecond steps
    // If the Transition Time field is present, the Delay field shall also be present; otherwise these fields shall not be present.
    uint8_t delay;
} m_fnd_scenes_recall_t;


/// Scene recall message Unacknowledged structure
typedef struct m_fnd_scenes_recall_unack
{
    /// The number of the scene to be recalled.
    uint16_t scene_number;
    // Transaction Identifier
    uint8_t tid;
    // 0b00 The Default Transition Step Resolution is 100 milliseconds
    // 0b01 The Default Transition Step Resolution is 1 second
    // 0b10 The Default Transition Step Resolution is 10 seconds
    // 0b11 The Default Transition Step Resolution is 10 minutes

    generic_transition_time_t trans_time;
    // Message execution delay in 5 millisecond steps
    // If the Transition Time field is present, the Delay field shall also be present; otherwise these fields shall not be present.
    uint8_t delay;
} m_fnd_scenes_recall_unack_t;


/// Scene status message structure
typedef struct m_fnd_scenes_status
{
    // The present value of the scene state.
    uint8_t status_code;
    // Scene Number of a current scene.
    uint16_t current_scene;
    // Scene Number of a target scene. (Optional)
    uint16_t target_scene;
    // Format as defined in Section 3.1.3. (C.1)
    // If the Target OnOff field is present, the Remaining Time field shall also be present; otherwisethese fields shall not be present.
    generic_transition_time_t rema_time;
} m_fnd_scenes_status_t;

/// Scene register status message structure
typedef struct m_fnd_scenes_register_status
{
    // The present value of the scene state.
    uint8_t status_code;
    // Scene Number of a current scene
    uint16_t current_scene;
    // A list of scenes stored within an element
    uint16_t scenes[M_FND_SCENES_STORE_MAX];
} m_fnd_scenes_register_status_t;


/// Scene store message structure
typedef struct m_fnd_scenes_store
{
    /// The number of the scene to be recalled.
    uint16_t scene_number;

} m_fnd_scenes_store_t;

/// Scene store unack message structure
typedef struct m_fnd_scenes_store_unack
{
    /// The number of the scene to be recalled.
    uint16_t scene_number;

} m_fnd_scenes_store_unack_t;

/// Scene delete message structure
typedef struct m_fnd_scenes_delete
{
    /// The number of the scene to be recalled.
    uint16_t scene_number;

} m_fnd_scenes_delete_t;

/// Scene store unack message structure
typedef struct m_fnd_scenes_delete_unack
{
    /// The number of the scene to be recalled.
    uint16_t scene_number;

} m_fnd_scenes_delete_unack_t;



#pragma pack()
/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Scenes Client model initialization function.
 *
 * @param[in] reset     Indicate if function is called during initialization or during reset.
 * @param[in] p_env     Pointer to the environment structure
 * @param[in] p_cfg     Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_fnd_scenes_init(m_fnd_data_cb_p cb);

/**
 ****************************************************************************************
 * @brief Get size of environment used by Scenes Client model.
 *
 * @note This function is called before init function in order to know memory requirements of the module.
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_fnd_scenes_get_env_size(const m_cfg_t* p_cfg);





/// @} end of group

#endif //_M_FND_GEN_ONOFF_H_
