/**
 ****************************************************************************************
 *
 * @file mm_defines.h
 *
 * @brief Header file for Mesh Model Definitions
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef MM_DEFINES_
#define MM_DEFINES_

/**
 ****************************************************************************************
 * @defgroup MM_DEFINES Mesh Model Definitions
 * @ingroup MESH_MDL
 * @brief  Mesh Model Defines
 * @{
 ****************************************************************************************
 */

/*
 * DEFINES (MODEL IDS)
 ****************************************************************************************
 */

/// ************************** Model IDs for Generic Models *****************************

/// Generic Server - OnOff
#define MM_ID_GENS_OO           (0x1000)
/// Generic Server - Level
#define MM_ID_GENS_LVL          (0x1002)
/// Generic Server - Default Transition Time
#define MM_ID_GENS_DTT          (0x1004)
/// Generic Server - Power OnOff
#define MM_ID_GENS_POO          (0x1006)
/// Generic Server - Power OnOff Setup
#define MM_ID_GENS_POOS         (0x1007)
/// Generic Server - Power Level
#define MM_ID_GENS_PLVL         (0x1009)
/// Generic Server - Power Level Setup
#define MM_ID_GENS_PLVLS        (0x100A)
/// Generic Server - Battery
#define MM_ID_GENS_BAT          (0x100C)
/// Generic Server - Location
#define MM_ID_GENS_LOC          (0x100E)
/// Generic Server - Location Setup
#define MM_ID_GENS_LOCS         (0x100F)
/// Generic Server - User Property
#define MM_ID_GENS_UPROP        (0x1013)
/// Generic Server - Admin Property
#define MM_ID_GENS_APROP        (0x1011)
/// Generic Server - Manufacturer Property
#define MM_ID_GENS_MPROP        (0x1012)
/// Generic Server - Client Property
#define MM_ID_GENS_CPROP        (0x1014)

/// Generic Client - OnOff
#define MM_ID_GENC_OO           (0x1001)
/// Generic Client - Level
#define MM_ID_GENC_LVL          (0x1003)
/// Generic Client - Default Transition Time
#define MM_ID_GENC_DTT          (0x1005)
/// Generic Client - Power OnOff
#define MM_ID_GENC_POO          (0x1008)
/// Generic Client - Power Level
#define MM_ID_GENC_PLVL         (0x100B)
/// Generic Client - Location
#define MM_ID_GENC_LOC          (0x1010)
/// Generic Client - Battery
#define MM_ID_GENC_BAT          (0x100D)
/// Generic Client - Property
#define MM_ID_GENC_PROP         (0x1015)

/// ************************** Model IDs for Sensors Models *****************************

/// Sensors Server - Sensor
#define MM_ID_SENS_SEN          (0x1100)
/// Sensors Server - Sensor Setup
#define MM_ID_SENS_SENS         (0x1101)

/// Sensors Client - Sensor
#define MM_ID_SENC_SEN          (0x1102)

/// ********************** Model IDs for Time and Scenes Models *************************

/// Time and Scene Server - Time
#define MM_ID_TSCNS_TIM         (0x1200)
/// Time and Scene Server - Time Setup
#define MM_ID_TSCNS_TIMS        (0x1201)
/// Time and Scene Server - Scene
#define MM_ID_TSCNS_SCN         (0x1203)
/// Time and Scene Server - Scene Setup
#define MM_ID_TSCNS_SCNS        (0x1204)
/// Time and Scene Server - Scheduler
#define MM_ID_TSCNS_SCH         (0x1206)
/// Time and Scene Server - Scheduler Setup
#define MM_ID_TSCNS_SCHS        (0x1207)

/// Time and Scene Client - Time
#define MM_ID_TSCNC_TIM         (0x1202)
/// Time and Scene Client - Scene
#define MM_ID_TSCNC_SCN         (0x1205)
/// Time and Scene Client - Scheduler
#define MM_ID_TSCNC_SCH         (0x1208)

/// ************************** Model IDs for Lighting Models ****************************

/// Lighting Server - Light Lightness
#define MM_ID_LIGHTS_LN         (0x1300)
/// Lighting Server - Light Lightness Setup
#define MM_ID_LIGHTS_LNS        (0x1301)
/// Lighting Server - Light CTL
#define MM_ID_LIGHTS_CTL        (0x1303)
/// Lighting Server - Light CTL Temperature
#define MM_ID_LIGHTS_CTLT       (0x1306)
/// Lighting Server - Light CTL Setup
#define MM_ID_LIGHTS_CTLS       (0x1304)
/// Lighting Server - Light HSL
#define MM_ID_LIGHTS_HSL        (0x1307)
/// Lighting Server - Light HSL Hue
#define MM_ID_LIGHTS_HSLH       (0x130A)
/// Lighting Server - Light HSL Saturation
#define MM_ID_LIGHTS_HSLSAT     (0x130B)
/// Lighting Server - Light HSL Setup
#define MM_ID_LIGHTS_HSLS       (0x1308)
/// Lighting Server - Light xyL
#define MM_ID_LIGHTS_XYL        (0x130C)
/// Lighting Server - Light xyL Setup
#define MM_ID_LIGHTS_XYLS       (0x130D)
/// Lighting Server - Light LC
#define MM_ID_LIGHTS_LC         (0x130F)
/// Lighting Server - Light LC Setup
#define MM_ID_LIGHTS_LCS        (0x1310)

/// Lighting Client - Light Lightness
#define MM_ID_LIGHTC_LN         (0x1302)
/// Lighting Client - Light CTL
#define MM_ID_LIGHTC_CTL        (0x1305)
/// Lighting Client - Light HSL
#define MM_ID_LIGHTC_HSL        (0x1309)
/// Lighting Client - Light xyL
#define MM_ID_LIGHTC_XYL        (0x130E)
/// Lighting Client - Light LC
#define MM_ID_LIGHTC_LC         (0x1311)

/*
 * DEFINES (MESSAGE IDS)
 ****************************************************************************************
 */

/// ********************* Message IDs for Generic OnOff Model **************************

/// Generic OnOff Get
#define MM_MSG_GEN_OO_GET               (0x0182)
/// Generic OnOff Set
#define MM_MSG_GEN_OO_SET               (0x0282)
/// Generic OnOff Set Unacknowledged
#define MM_MSG_GEN_OO_SET_UNACK         (0x0382)
/// Generic OnOff Status
#define MM_MSG_GEN_OO_STATUS            (0x0482)

/// ********************* Message IDs for Generic Level Model ***************************

/// Generic Level Get
#define MM_MSG_GEN_LVL_GET              (0x0582)
/// Generic Level Set
#define MM_MSG_GEN_LVL_SET              (0x0682)
/// Generic Level Set Unacknowledged
#define MM_MSG_GEN_LVL_SET_UNACK        (0x0782)
/// Generic Level Status
#define MM_MSG_GEN_LVL_STATUS           (0x0882)
/// Generic Delta Set
#define MM_MSG_GEN_DELTA_SET            (0x0982)
/// Generic Delta Set Unacknowledged
#define MM_MSG_GEN_DELTA_SET_UNACK      (0x0A82)
/// Generic Move Set
#define MM_MSG_GEN_MOVE_SET             (0x0B82)
/// Generic Move Set Unacknowledged
#define MM_MSG_GEN_MOVE_SET_UNACK       (0x0C82)

/// ************** Message IDs for Generic Default Transition Time Model ****************

/// Generic Default Transition Time Get
#define MM_MSG_GEN_DTT_GET              (0x0D82)
/// Generic Default Transition Time Set
#define MM_MSG_GEN_DTT_SET              (0x0E82)
/// Generic Default Transition Time Set Unacknowledged
#define MM_MSG_GEN_DTT_SET_UNACK        (0x0F82)
/// Generic Default Transition Time Status
#define MM_MSG_GEN_DTT_STATUS           (0x1082)

/// ******************** Message IDs for Generic Power OnOff Model *********************

/// Generic OnPowerUp Get
#define MM_MSG_GEN_ONPUP_GET            (0x1182)
/// Generic OnPowerUp Status
#define MM_MSG_GEN_ONPUP_STATUS         (0x1282)

/// ******************** Message IDs for Generic Power OnOff Model *********************

/// Generic OnPowerUp Set
#define MM_MSG_GEN_ONPUP_SET            (0x1382)
/// Generic OnPowerUp Set Unacknowledged
#define MM_MSG_GEN_ONPUP_SET_UNACK      (0x1482)

/// ******************** Message IDs for Generic Power Level Model **********************

/// Generic Power Level Get
#define MM_MSG_GEN_PLVL_GET             (0x1582)
/// Generic Power Level Set
#define MM_MSG_GEN_PLVL_SET             (0x1682)
/// Generic Power Level Set Unacknowledged
#define MM_MSG_GEN_PLVL_SET_UNACK       (0x1782)
/// Generic Power Level Status
#define MM_MSG_GEN_PLVL_STATUS          (0x1882)
/// Generic Power Last Get
#define MM_MSG_GEN_PLAST_GET            (0x1982)
/// Generic Power Last Status
#define MM_MSG_GEN_PLAST_STATUS         (0x1A82)
/// Generic Power Default Get
#define MM_MSG_GEN_PDFLT_GET            (0x1B82)
/// Generic Power Default Status
#define MM_MSG_GEN_PDFLT_STATUS         (0x1C82)
/// Generic Power Range Get
#define MM_MSG_GEN_PRANGE_GET           (0x1D82)
/// Generic Power Range Status
#define MM_MSG_GEN_PRANGE_STATUS        (0x1E82)

/// ***************** Message IDs for Generic Power Level Setup Model *******************

/// Generic Power Default Set
#define MM_MSG_GEN_PDFLT_SET            (0x1F82)
/// Generic Power Default Set Unacknowledged
#define MM_MSG_GEN_PDFLT_SET_UNACK      (0x2082)
/// Generic Power Range Set
#define MM_MSG_GEN_PRANGE_SET           (0x2182)
/// Generic Power Range Set Unacknowledged
#define MM_MSG_GEN_PRANGE_SET_UNACK     (0x2282)

/// ***************** Message IDs for Generic Battery Model *******************

/// Generic Battery Get
#define MM_MSG_GEN_BAT_GET              (0x2382)
/// Generic Battery Status
#define MM_MSG_GEN_BAT_STATUS           (0x2482)

/// ***************** Message IDs for Generic Location Model *******************

/// Generic Location Global Get
#define MM_MSG_GEN_LOCG_GET             (0x2582)
/// Generic Location Global Status
#define MM_MSG_GEN_LOCG_STATUS          (0x40)
/// Generic Location Local Get
#define MM_MSG_GEN_LOCL_GET             (0x2682)
/// Generic Location Local Status
#define MM_MSG_GEN_LOCL_STATUS          (0x2782)

/// ***************** Message IDs for Generic Location Setup Model *******************

/// Generic Location Global Set
#define MM_MSG_GEN_LOCG_SET             (0x41)
/// Generic Location Global Set Unacknowledged
#define MM_MSG_GEN_LOCG_SET_UNACK       (0x42)
/// Generic Location Local Set
#define MM_MSG_GEN_LOCL_SET             (0x2882)
/// Generic Location Local Set Unacknowledged
#define MM_MSG_GEN_LOCL_SET_UNACK       (0x2982)

/// ***************** Message IDs for Generic Manufacturer Property Model *******************

/// Generic Manufacturer Properties Get
#define MM_MSG_GEN_MPROPS_GET           (0x2A82)
/// Generic Manufacturer Properties Status
#define MM_MSG_GEN_MPROPS_STATUS        (0x43)
/// Generic Manufacturer Property Get
#define MM_MSG_GEN_MPROP_GET            (0x2B82)
/// Generic Manufacturer Property Set
#define MM_MSG_GEN_MPROP_SET            (0x44)
/// Generic Manufacturer Property Set Unacknowledged
#define MM_MSG_GEN_MPROP_SET_UNACK      (0x45)
/// Generic Manufacturer Property Status
#define MM_MSG_GEN_MPROP_STATUS         (0x46)

/// ***************** Message IDs for Generic Admin Property Model *******************

/// Generic Admin Properties Get
#define MM_MSG_GEN_APROPS_GET           (0x2C82)
/// Generic Admin Properties Status
#define MM_MSG_GEN_APROPS_STATUS        (0x47)
/// Generic Admin Property Get
#define MM_MSG_GEN_APROP_GET            (0x2D82)
/// Generic Admin Property Set
#define MM_MSG_GEN_APROP_SET            (0x48)
/// Generic Admin Property Set Unacknowledged
#define MM_MSG_GEN_APROP_SET_UNACK      (0x49)
/// Generic Admin Property Status
#define MM_MSG_GEN_APROP_STATUS         (0x4A)

/// ***************** Message IDs for Generic User Property Model *******************

/// Generic User Properties Get
#define MM_MSG_GEN_UPROPS_GET           (0x2E82)
/// Generic User Properties Status
#define MM_MSG_GEN_UPROPS_STATUS        (0x4B)
/// Generic User Property Get
#define MM_MSG_GEN_UPROP_GET            (0x2F82)
/// Generic User Property Set
#define MM_MSG_GEN_UPROP_SET            (0x4C)
/// Generic User Property Set Unacknowledged
#define MM_MSG_GEN_UPROP_SET_UNACK      (0x4D)
/// Generic User Property Status
#define MM_MSG_GEN_UPROP_STATUS         (0x4E)

/// ***************** Message IDs for Generic Client Property Model *******************

/// Generic Client Properties Get
#define MM_MSG_GEN_CPROPS_GET           (0x4F)
/// Generic Client Properties Status
#define MM_MSG_GEN_CPROPS_STATUS        (0x50)

/// ********************** Message IDs for Light Lightness Model **********************

/// Light Lightness Get
#define MM_MSG_LIGHT_LN_GET                 (0x4B82)
/// Light Lightness Set
#define MM_MSG_LIGHT_LN_SET                 (0x4C82)
/// Light Lightness Set Unacknowledged
#define MM_MSG_LIGHT_LN_SET_UNACK           (0x4D82)
/// Light Lightness Status
#define MM_MSG_LIGHT_LN_STATUS              (0x4E82)
/// Light Lightness Linear Get
#define MM_MSG_LIGHT_LN_LINEAR_GET          (0x4F82)
/// Light Lightness Linear Set
#define MM_MSG_LIGHT_LN_LINEAR_SET          (0x5082)
/// Light Lightness Linear Set Unacknowledged
#define MM_MSG_LIGHT_LN_LINEAR_SET_UNACK    (0x5182)
/// Light Lightness Linear Status
#define MM_MSG_LIGHT_LN_LINEAR_STATUS       (0x5282)
/// Light Lightness Last Get
#define MM_MSG_LIGHT_LN_LAST_GET            (0x5382)
/// Light Lightness Last Status
#define MM_MSG_LIGHT_LN_LAST_STATUS         (0x5482)
/// Light Lightness Default Get
#define MM_MSG_LIGHT_LN_DFLT_GET            (0x5582)
/// Light Lightness Default Status
#define MM_MSG_LIGHT_LN_DFLT_STATUS         (0x5682)
/// Light Lightness Range Get
#define MM_MSG_LIGHT_LN_RANGE_GET           (0x5782)
/// Light Lightness Range Status
#define MM_MSG_LIGHT_LN_RANGE_STATUS        (0x5882)

/// ***************** Message IDs for Light Lightness Setup Model *********************

/// Light Lightness Default Set
#define MM_MSG_LIGHT_LN_DFLT_SET            (0x5982)
/// Light Lightness Default Set Unacknowledged
#define MM_MSG_LIGHT_LN_DFLT_SET_UNACK      (0x5A82)
/// Light Lightness Range Set
#define MM_MSG_LIGHT_LN_RANGE_SET           (0x5B82)
/// Light Lightness Range Set Unacknowledged
#define MM_MSG_LIGHT_LN_RANGE_SET_UNACK     (0x5C82)

/// ************************* Message IDs for Light CTL Model *************************

/// Light CTL Get
#define MM_MSG_LIGHT_CTL_GET                (0x5D82)
/// Light CTL Set
#define MM_MSG_LIGHT_CTL_SET                (0x5E82)
/// Light CTL Set Unacknowledged
#define MM_MSG_LIGHT_CTL_SET_UNACK          (0x5F82)
/// Light CTL Status
#define MM_MSG_LIGHT_CTL_STATUS             (0x6082)
/// Light CTL Temperature Get
#define MM_MSG_LIGHT_CTL_TEMP_GET           (0x6182)
/// Light CTL Temperature Set
#define MM_MSG_LIGHT_CTL_TEMP_SET           (0x6482)
/// Light CTL Temperature Set Unacknowledged
#define MM_MSG_LIGHT_CTL_TEMP_SET_UNACK     (0x6582)
/// Light CTL Temperature Status
#define MM_MSG_LIGHT_CTL_TEMP_STATUS        (0x6682)
/// Light CTL Temperature Range Get
#define MM_MSG_LIGHT_CTL_TEMP_RANGE_GET     (0x6282)
/// Light CTL Temperature Range Status
#define MM_MSG_LIGHT_CTL_TEMP_RANGE_STATUS  (0x6382)
/// Light CTL Default Get
#define MM_MSG_LIGHT_CTL_DFLT_GET           (0x6782)
/// Light CTL Default Status
#define MM_MSG_LIGHT_CTL_DFLT_STATUS        (0x6882)

/// ********************* Message IDs for Light CTL Setup Model ***********************

/// Light CTL Default Set
#define MM_MSG_LIGHT_CTL_DFLT_SET             (0x6982)
/// Light CTL Default Set Unacknowledged
#define MM_MSG_LIGHT_CTL_DFLT_SET_UNACK       (0x6A82)
/// Light CTL Temperature Range Set
#define MM_MSG_LIGHT_CTL_TEMP_RANGE_SET       (0x6B82)
/// Light CTL Temperature Range Set Unacknowledged
#define MM_MSG_LIGHT_CTL_TEMP_RANGE_SET_UNACK (0x6C82)

/// ************************* Message IDs for Light HSL Model *************************

/// Light HSL Get
#define MM_MSG_LIGHT_HSL_GET                (0x6D82)
/// Light HSL Hue Get
#define MM_MSG_LIGHT_HSL_HUE_GET            (0x6E82)
/// Light HSL Hue Set
#define MM_MSG_LIGHT_HSL_HUE_SET            (0x6F82)
/// Light HSL Hue Set Unacknowledged
#define MM_MSG_LIGHT_HSL_HUE_SET_UNACK      (0x7082)
/// Light HSL Hue Status
#define MM_MSG_LIGHT_HSL_HUE_STATUS         (0x7182)
/// Light HSL Saturation Get
#define MM_MSG_LIGHT_HSL_SAT_GET            (0x7282)
/// Light HSL Saturation Set
#define MM_MSG_LIGHT_HSL_SAT_SET            (0x7382)
/// Light HSL Saturation Set Unacknowledged
#define MM_MSG_LIGHT_HSL_SAT_SET_UNACK      (0x7482)
/// Light HSL Saturation Status
#define MM_MSG_LIGHT_HSL_SAT_STATUS         (0x7582)
/// Light HSL Set
#define MM_MSG_LIGHT_HSL_SET                (0x7682)
/// Light HSL Set Unacknowledged
#define MM_MSG_LIGHT_HSL_SET_UNACK          (0x7782)
/// Light HSL Status
#define MM_MSG_LIGHT_HSL_STATUS             (0x7882)
/// Light HSL Target Get
#define MM_MSG_LIGHT_HSL_TGT_GET            (0x7982)
/// Light HSL Target Status
#define MM_MSG_LIGHT_HSL_TGT_STATUS         (0x7A82)
/// Light HSL Default Get
#define MM_MSG_LIGHT_HSL_DFLT_GET           (0x7B82)
/// Light HSL Default Status
#define MM_MSG_LIGHT_HSL_DFLT_STATUS        (0x7C82)
/// Light HSL Range Get
#define MM_MSG_LIGHT_HSL_RANGE_GET          (0x7D82)
/// Light HSL Range Status
#define MM_MSG_LIGHT_HSL_RANGE_STATUS       (0x7E82)

/// ********************* Message IDs for Light HSL Setup Model ***********************

/// Light HSL Default Set
#define MM_MSG_LIGHT_HSL_DFLT_SET           (0x7F82)
/// Light HSL Default Set Unacknowledged
#define MM_MSG_LIGHT_HSL_DFLT_SET_UNACK     (0x8082)
/// Light HSL Range Set
#define MM_MSG_LIGHT_HSL_RANGE_SET          (0x8182)
/// Light HSL Range Set Unacknowledged
#define MM_MSG_LIGHT_HSL_RANGE_SET_UNACK    (0x82)

/// ************************* Message IDs for Light xyL Model *************************

/// Light xyL Get
#define MM_MSG_LIGHT_XYL_GET                (0x8382)
/// Light xyL Set
#define MM_MSG_LIGHT_XYL_SET                (0x8482)
/// Light xyL Set Unacknowledged
#define MM_MSG_LIGHT_XYL_SET_UNACK          (0x8582)
/// Light xyL Status
#define MM_MSG_LIGHT_XYL_STATUS             (0x8682)
/// Light xyL Target Get
#define MM_MSG_LIGHT_XYL_TGT_GET            (0x8782)
/// Light xyL Target Status
#define MM_MSG_LIGHT_XYL_TGT_STATUS         (0x8882)
/// Light HSL Default Get
#define MM_MSG_LIGHT_XYL_DFLT_GET           (0x8982)
/// Light xyL Default Status
#define MM_MSG_LIGHT_XYL_DFLT_STATUS        (0x8A82)
/// Light xyL Range Get
#define MM_MSG_LIGHT_XYL_RANGE_GET          (0x8B82)
/// Light xyL Range Status
#define MM_MSG_LIGHT_XYL_RANGE_STATUS       (0x8C82)

/// ********************* Message IDs for Light xyL Setup Model ***********************

/// Light xyL Default Set
#define MM_MSG_LIGHT_XYL_DFLT_SET           (0x8D82)
/// Light xyL Default Set Unacknowledged
#define MM_MSG_LIGHT_XYL_DFLT_SET_UNACK     (0x8E82)
/// Light xyL Range Set
#define MM_MSG_LIGHT_XYL_RANGE_SET          (0x8F82)
/// Light xyL Range Set Unacknowledged
#define MM_MSG_LIGHT_XYL_RANGE_SET_UNACK    (0x9082)

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of steps for transition time value
#define MM_TRANS_TIME_NB_STEPS_MAX      (0x3E)
/// Transition time not provided
#define MM_TRANS_TIME_UNKNOWN           (0x3F)

/// Location Global Latitude Not Configured
#define MM_LOC_GLOBAL_LAT_NOT_CONFIG    (0x80000000)
/// Location Global Longitude Not Configured
#define MM_LOC_GLOBAL_LONG_NOT_CONFIG   (0x80000000)
/// Location Global Altitude Not Configured
#define MM_LOC_GLOBAL_ALT_NOT_CONFIG    (0x7FFF)
/// Location Local North Not Configured
#define MM_LOC_LOCAL_NORTH_NOT_CONFIG   (0x8000)
/// Location Local East Not Configured
#define MM_LOC_LOCAL_EAST_NOT_CONFIG    (0x8000)
/// Location Local Altitude Not Configured
#define MM_LOC_LOCAL_ALT_NOT_CONFIG     (0x7FFF)
/// Floor Number Not Configured
#define MM_LOC_FLOOR_NOT_CONFIG         (0xFF)

/// Battery Level Max value
#define MM_BAT_LVL_MAX                  (0x64)
/// Battery Level Unknown
#define MM_BAT_LVL_UNKNOWN              (0xFF)
/// Battery Flags (all unknown)
#define MM_BAT_FLAGS_UNKNOWN            (0xFF)
/// Battery Time to discharge unknown
#define MM_BAT_TIME_DISCHARGE_UNKNOWN   (0x00FFFFFF)
/// Battery Time to charge unknown
#define MM_BAT_TIME_CHARGE_UNKNOWN      (0x00FFFFFF)

/*
 * DEFINES (MESSAGE LENGTH)
 ****************************************************************************************
 */

/// Minimal length of Generic OnOff Set/Set Unacknowledged message
#define MM_GEN_OO_SET_MIN_LEN              (2)
/// Length of Generic OnOff Set/Set Unacknowledged message when all field are present
#define MM_GEN_OO_SET_LEN                  (4)
/// Minimal length of Generic OnOff Status message
#define MM_GEN_OO_STATUS_MIN_LEN           (1)
/// Length of Generic OnOff Status message when all fields are present
#define MM_GEN_OO_STATUS_LEN               (3)

/// Minimal length of Generic Level Set/Set Unacknowledged message
#define MM_GEN_LVL_SET_MIN_LEN             (3)
/// Length of Generic Level Set/Set Unacknowledged message when all field are present
#define MM_GEN_LVL_SET_LEN                 (5)
/// Minimal length of Generic Delta Set/Set Unacknowledged message
#define MM_GEN_LVL_SET_DELTA_MIN_LEN       (5)
/// Length of Generic Delta Set/Set Unacknowledged message when all field are present
#define MM_GEN_LVL_SET_DELTA_LEN           (7)
/// Minimal length of Generic Move Set/Set Unacknowledged message
#define MM_GEN_LVL_SET_MOVE_MIN_LEN        (3)
/// Length of Generic Move Set/Set Unacknowledged message when all field are present
#define MM_GEN_LVL_SET_MOVE_LEN            (5)
/// Minimal length of Generic Level Status message
#define MM_GEN_LVL_STATUS_MIN_LEN          (2)
/// Length of Generic Level Status message when all fields are present
#define MM_GEN_LVL_STATUS_LEN              (5)

/// Length of Generic Default Transition Time Set/Set Unacknowledged message
#define MM_GEN_DTT_SET_LEN                 (1)
/// Length of Generic Default Transition Time Status message
#define MM_GEN_DTT_STATUS_LEN              (1)

/// Length of Generic OnPowerUp Set/Set Unacknowledged message
#define MM_GEN_POO_SET_LEN                 (1)
/// Length of Generic OnPowerUp Status message
#define MM_GEN_POO_STATUS_LEN              (1)

/// Length of Generic Battery Status message
#define MM_GEN_BAT_STATUS_LEN              (8)

/// Length of Generic Location Global Set/Set Unacknowledged message
#define MM_GEN_LOC_SET_GLOB_LEN            (10)
/// Length of Generic Location Local Set/Set Unacknowledged message
#define MM_GEN_LOC_SET_LOC_LEN             (9)
/// Length of Generic Location Global Status message
#define MM_GEN_LOC_STATUS_GLOB_LEN         (10)
/// Length of Generic Location Local Status message
#define MM_GEN_LOC_STATUS_LOC_LEN          (9)

/// Minimal length of Generic Power Set/Set Unacknowledged message
#define MM_GEN_PLVL_SET_MIN_LEN            (3)
/// Length of Generic Power Set/Set Unacknowledged message when all field are present
#define MM_GEN_PLVL_SET_LEN                (5)
/// Minimal length of Generic Power Level Status message
#define MM_GEN_PLVL_STATUS_MIN_LEN         (2)
/// Length of Generic Power Level Status message when all fields are present
#define MM_GEN_PLVL_STATUS_LEN             (5)
/// Length of Generic Power Last Status
#define MM_GEN_PLVL_LAST_STATUS_LEN        (2)
/// Length of Generic Power Default Set/Set Unacknowledged
#define MM_GEN_PLVL_DFLT_SET_LEN           (2)
/// Length of Generic Power Default Status
#define MM_GEN_PLVL_DFLT_STATUS_LEN        (2)
/// Length of Generic Power Range Set/Set Unacknowledged
#define MM_GEN_PLVL_RANGE_SET_LEN          (4)
/// Length of Generic Power Range Status
#define MM_GEN_PLVL_RANGE_STATUS_LEN       (5)

/// Minimal length of Generic User Property Set/Set Unacknowledged message
#define MM_GEN_PROP_USER_SET_MIN_LEN       (2)
/// Minimal length of Generic Admin Property Set/Set Unacknowledged message
#define MM_GEN_PROP_ADMIN_SET_MIN_LEN      (3)
/// Length of Generic Manufacturer Property Set/Set Unacknowledged message
#define MM_GEN_PROP_MANUF_SET_LEN          (3)
/// Minimal length of Generic User/Admin/Manufacturer Property Status message
#define MM_GEN_PROP_STATUS_MIN_LEN         (3)

/// Minimal length of Light Lightness Set/Set Unacknowledged message
#define MM_LIGHT_LN_SET_MIN_LEN            (3)
/// Length of Light Lightness Set/Set Unacknowledged message
#define MM_LIGHT_LN_SET_LEN                (5)
/// Minimal length of Light Lightness Status message
#define MM_LIGHT_LN_STATUS_MIN_LEN         (2)
/// Length of Light Lightness Status message
#define MM_LIGHT_LN_STATUS_LEN             (5)
/// Minimal length of Light Lightness Linear Set/Set Unacknowledged message
#define MM_LIGHT_LN_LIN_SET_MIN_LEN        (3)
/// Length of Light Lightness Linear Set/Set Unacknowledged message
#define MM_LIGHT_LN_LIN_SET_LEN            (5)
/// Minimal length of Light Lightness Linear Status message
#define MM_LIGHT_LN_LIN_STATUS_MIN_LEN     (2)
/// Length of Light Lightness Linear Status message
#define MM_LIGHT_LN_LIN_STATUS_LEN         (5)
/// Length of Light Lightness Last Status message
#define MM_LIGHT_LN_LAST_STATUS_LEN        (2)
/// Length of Light Lightness Default Set/Set Unacknowledged message
#define MM_LIGHT_LN_DFLT_SET_LEN           (2)
/// Length of Light Lightness Default Status message
#define MM_LIGHT_LN_DFLT_STATUS_LEN        (2)
/// Length of Light Lightness Range Set/Set Unacknowledged message
#define MM_LIGHT_LN_RANGE_SET_LEN          (4)
/// Length of Light Lightness Range Status message
#define MM_LIGHT_LN_RANGE_STATUS_LEN       (5)

/// Minimal length of Light CTL Set/Set Unacknowledged message
#define MM_LIGHT_CTL_SET_MIN_LEN           (7)
/// Length of Light CTL Set//Set Unacknowledged message
#define MM_LIGHT_CTL_SET_LEN               (9)
/// Minimal length of Light CTL Status message
#define MM_LIGHT_CTL_STATUS_MIN_LEN        (4)
/// Length of Light CTL Status message
#define MM_LIGHT_CTL_STATUS_LEN            (9)
/// Minimal length of Light CTL Temperature Set/Set Unacknowledged message
#define MM_LIGHT_CTL_TEMP_SET_MIN_LEN      (5)
/// Length of Light CTL Temperature Set//Set Unacknowledged message
#define MM_LIGHT_CTL_TEMP_SET_LEN          (7)
/// Minimal length of Light CTL Temperature Status message
#define MM_LIGHT_CTL_TEMP_STATUS_MIN_LEN   (4)
/// Length of Light CTL Temperature Status message
#define MM_LIGHT_CTL_TEMP_STATUS_LEN       (9)
/// Length of Light CTL Temperature Range Set/Set Unacknowledged
#define MM_LIGHT_CTL_TEMP_RANGE_SET_LEN    (4)
/// Length of Light CTL Temperature Range Status
#define MM_LIGHT_CTL_TEMP_RANGE_STATUS_LEN (5)
/// Length of Light CTL Default Set/Unacknowledged message
#define MM_LIGHT_CTL_DFLT_SET_LEN          (6)
/// Length of Light CTL Default Status message
#define MM_LIGHT_CTL_DFLT_STATUS_LEN       (6)

/// Minimal length of Light HSL Set/Set Unacknowledged message
#define MM_LIGHT_HSL_SET_MIN_LEN           (7)
/// Length of Light HSL Set//Set Unacknowledged message
#define MM_LIGHT_HSL_SET_LEN               (9)
/// Minimal length of Light HSL Status message
#define MM_LIGHT_HSL_STATUS_MIN_LEN        (6)
/// Length of Light HSL Status message
#define MM_LIGHT_HSL_STATUS_LEN            (7)
/// Minimal length of Light HSL Target Status message
#define MM_LIGHT_HSL_TGT_STATUS_MIN_LEN    (6)
/// Length of Light HSL Target Status message
#define MM_LIGHT_HSL_TGT_STATUS_LEN        (7)
/// Minimal length of Light HSL Hue Set/Set Unacknowledged message
#define MM_LIGHT_HSL_HUE_SET_MIN_LEN       (3)
/// Length of Light HSL Hue Set//Set Unacknowledged message
#define MM_LIGHT_HSL_HUE_SET_LEN           (5)
/// Minimal length of Light HSL Hue Status message
#define MM_LIGHT_HSL_HUE_STATUS_MIN_LEN    (2)
/// Length of Light HSL Hue Status message
#define MM_LIGHT_HSL_HUE_STATUS_LEN        (5)
/// Minimal length of Light HSL Saturation Set/Set Unacknowledged message
#define MM_LIGHT_HSL_SAT_SET_MIN_LEN       (3)
/// Length of Light HSL Saturation Set//Set Unacknowledged message
#define MM_LIGHT_HSL_SAT_SET_LEN           (5)
/// Minimal length of Light HSL Saturation Status message
#define MM_LIGHT_HSL_SAT_STATUS_MIN_LEN    (2)
/// Length of Light HSL Saturation Status message
#define MM_LIGHT_HSL_SAT_STATUS_LEN        (5)
/// Length of Light HSL Default Set/Set Unacknowledged message
#define MM_LIGHT_HSL_DFLT_SET_LEN          (6)
/// Length of Light HSL Default Status message
#define MM_LIGHT_HSL_DFLT_STATUS_LEN       (6)
/// Length of Light HSL Range Set/Set Unacknowledged message
#define MM_LIGHT_HSL_RANGE_SET_LEN         (8)
/// Length of Light HSL Range Status message
#define MM_LIGHT_HSL_RANGE_STATUS_LEN      (9)

/// Minimal length of Light xyL Set/Set Unacknowledged message
#define MM_LIGHT_XYL_SET_MIN_LEN           (7)
/// Length of Light xyL Set//Set Unacknowledged message
#define MM_LIGHT_XYL_SET_LEN               (9)
/// Minimal length of Light xyL Status message
#define MM_LIGHT_XYL_STATUS_MIN_LEN        (6)
/// Length of Light xyL Status message
#define MM_LIGHT_XYL_STATUS_LEN            (7)
/// Minimal length of Light xyL Target Status message
#define MM_LIGHT_XYL_TGT_STATUS_MIN_LEN    (6)
/// Length of Light xyL Target Status message
#define MM_LIGHT_XYL_TGT_STATUS_LEN        (7)
/// Length of Light xyL Default Set/Set Unacknowledged message
#define MM_LIGHT_XYL_DFLT_SET_LEN          (6)
/// Length of Light xyL Default Status message
#define MM_LIGHT_XYL_DFLT_STATUS_LEN       (6)
/// Length of Light xyL Range Set/Set Unacknowledged message
#define MM_LIGHT_XYL_RANGE_SET_LEN         (8)
/// Length of Light xyL Range Status message
#define MM_LIGHT_XYL_RANGE_STATUS_LEN      (9)

/*
 * ENUMERATIONS (MESSAGE CONTENT)
 ****************************************************************************************
 */

/// Positions in Generic OnOff Set/Set Unacknowledged message
enum mm_gen_oo_set_pos
{
    /// OnOff state value
    MM_GEN_OO_SET_OO_POS = 0,
    /// TID value
    MM_GEN_OO_SET_TID_POS,
    /// Transition time value
    MM_GEN_OO_SET_TRANS_TIME_POS,
    /// Delay value
    MM_GEN_OO_SET_DELAY_POS,
};

/// Positions in Generic OnOff Status message
enum mm_gen_oo_status_pos
{
    /// Present OnOff state value
    MM_GEN_OO_STATUS_OO_POS = 0,
    /// Target OnOff state value
    MM_GEN_OO_STATUS_TGT_OO_POS,
    /// Remaining time value
    MM_GEN_OO_STATUS_REM_TIME_POS,
};

/// Positions in Generic Level Set/Set Unacknowledged message
enum mm_gen_lvl_set_pos
{
    /// Level value
    MM_GEN_LVL_SET_LVL_POS = 0,
    /// TID value
    MM_GEN_LVL_SET_TID_POS = 2,
    /// Transition time value
    MM_GEN_LVL_SET_TRANS_TIME_POS,
    /// Delay value
    MM_GEN_LVL_SET_DELAY_POS,
};

/// Positions in Generic Delta Set/Set Unacknowledged message
enum mm_gen_lvl_set_delta_pos
{
    /// Delta level value
    MM_GEN_LVL_SET_DELTA_LVL_POS = 0,
    /// TID value
    MM_GEN_LVL_SET_DELTA_TID_POS = 4,
    /// Transition time value
    MM_GEN_LVL_SET_DELTA_TRANS_TIME_POS,
    /// Delay value
    MM_GEN_LVL_SET_DELTA_DELAY_POS,
};

/// Positions in Generic Move Set/Set Unacknowledged message
enum mm_gen_lvl_set_move_pos
{
    /// Delta level value
    MM_GEN_LVL_SET_MOVE_DELTA_LVL_POS = 0,
    /// TID value
    MM_GEN_LVL_SET_MOVE_TID_POS = 2,
    /// Transition time value
    MM_GEN_LVL_SET_MOVE_TRANS_TIME_POS,
    /// Delay value
    MM_GEN_LVL_SET_MOVE_DELAY_POS,
};

/// Positions in Generic Level Status message
enum mm_gen_lvl_status_pos
{
    /// Present Level value
    MM_GEN_LVL_STATUS_LVL_POS = 0,
    /// Target Level value
    MM_GEN_LVL_STATUS_TGT_LVL_POS = 2,
    /// Remaining time value
    MM_GEN_LVL_STATUS_REM_TIME_POS = 4,
};

/// Positions in Generic Power Level Set/Set Unacknowledged message
enum mm_gen_plvl_set_pos
{
    /// Power value
    MM_GEN_PLVL_SET_POWER_POS = 0,
    /// TID value
    MM_GEN_PLVL_SET_TID_POS = 2,
    /// Transition time value
    MM_GEN_PLVL_SET_TRANS_TIME_POS,
    /// Delay value
    MM_GEN_PLVL_SET_DELAY_POS,
};

/// Positions in Generic Power Level Status message
enum mm_gen_plvl_status_pos
{
    /// Present Power value
    MM_GEN_PLVL_STATUS_PRES_POWER_POS = 0,
    /// Target Power value
    MM_GEN_PLVL_STATUS_TGT_POWER_POS = 2,
    /// Remaining time value
    MM_GEN_PLVL_STATUS_REM_TIME_POS = 4,
};

/// Positions in Generic Power Last Status message
enum mm_gen_plvl_last_status_pos
{
    /// Power value
    MM_GEN_PLVL_LAST_STATUS_POWER_POS = 0,
};

/// Positions in Generic Power Default Set/Set Unacknowledged message
enum mm_gen_plvl_dflt_set_pos
{
    /// Power value
    MM_GEN_PLVL_DFLT_SET_POWER_POS = 0,
};

/// Positions in Generic Power Default Status message
enum mm_gen_plvl_dflt_status_pos
{
    /// Power value
    MM_GEN_PLVL_DFLT_STATUS_POWER_POS = 0,
};

/// Positions in Generic Power Range Set/Set Unacknowledged message
enum mm_gen_plvl_range_set_pos
{
    /// Range Min
    MM_GEN_PLVL_RANGE_SET_MIN_POS = 0,
    /// Range Max
    MM_GEN_PLVL_RANGE_SET_MAX_POS = 2,
};

/// Positions in Generic Power Range Status message
enum mm_gen_plvl_range_status_pos
{
    /// Status code
    MM_GEN_PLVL_RANGE_STATUS_CODE_POS = 0,
    /// Range Min
    MM_GEN_PLVL_RANGE_STATUS_MIN_POS = 1,
    /// Range Max
    MM_GEN_PLVL_RANGE_STATUS_MAX_POS = 3,
};

/// Positions in Generic Default Transition Time Set/Set Unacknowledged message
enum mm_gen_ddt_set_pos
{
    /// Default transition time state value
    MM_GEN_DDT_SET_DDT_POS = 0,
};

/// Positions in Generic Default Transition Time Status message
enum mm_gen_ddt_status_pos
{
    /// Default transition time state value
    MM_GEN_DDT_STATUS_DDT_POS = 0,
};

/// Positions in Generic OnPowerUp Set/Set Unacknowledged message
enum mm_gen_poo_set_pos
{
    /// OnPowerUp state value
    MM_GEN_POO_SET_OPU_POS = 0,
};

/// Positions in Generic OnPowerUp Status message
enum mm_gen_poo_status_pos
{
    /// OnPowerUp state value
    MM_GEN_POO_STATUS_OPU_POS = 0,
};

/// Positions in Generic Battery Status message
enum mm_gen_bat_status_pos
{
    /// Battery Level
    MM_GEN_BAT_STATUS_LEVEL_POS = 0,
    /// Time to discharge
    MM_GEN_BAT_STATUS_TIME_DISCHARGE_POS = 1,
    /// Time to charge
    MM_GEN_BAT_STATUS_TIME_CHARGE_POS = 4,
    /// Flags
    MM_GEN_BAT_STATUS_FLAGS_POS = 7,
};

/// Positions in Generic Location Global Set/Set Unacknowledged message
enum mm_gen_loc_set_glob_pos
{
    /// Global Latitude
    MM_GEN_LOC_SET_GLOB_LAT_POS = 0,
    /// Global Longitude
    MM_GEN_LOC_SET_GLOB_LONG_POS = 4,
    /// Global Altitude
    MM_GEN_LOC_SET_GLOB_ALT_POS = 8,
};

/// Positions in Generic Location Global Status message
enum mm_gen_loc_status_glob_pos
{
    /// Global Latitude
    MM_GEN_LOC_STATUS_GLOB_LAT_POS = 0,
    /// Global Longitude
    MM_GEN_LOC_STATUS_GLOB_LONG_POS = 4,
    /// Global Altitude
    MM_GEN_LOC_STATUS_GLOB_ALT_POS = 8,
};

/// Positions in Generic Location Local Set/Set Unacknowledged message
enum mm_gen_loc_set_loc_pos
{
    /// Local North
    MM_GEN_LOC_SET_LOC_NORTH_POS = 0,
    /// Local East
    MM_GEN_LOC_SET_LOC_EAST_POS = 2,
    /// Local Altitude
    MM_GEN_LOC_SET_LOC_ALT_POS = 4,
    /// Floor Number
    MM_GEN_LOC_SET_LOC_FLOOR_POS = 6,
    /// Uncertainty
    MM_GEN_LOC_SET_LOC_UNCERT_POS = 7,
};

/// Positions in Generic Location Local Status message
enum mm_gen_loc_status_loc_pos
{
    /// Local North
    MM_GEN_LOC_STATUS_LOC_NORTH_POS = 0,
    /// Local East
    MM_GEN_LOC_STATUS_LOC_EAST_POS = 2,
    /// Local Altitude
    MM_GEN_LOC_STATUS_LOC_ALT_POS = 4,
    /// Floor Number
    MM_GEN_LOC_STATUS_LOC_FLOOR_POS = 6,
    /// Uncertainty
    MM_GEN_LOC_STATUS_LOC_UNCERT_POS = 7,
};

/// Positions in Generic User Property Set/Set Unacknowledged message
enum mm_gen_prop_set_user_pos
{
    /// User Property ID
    MM_GEN_PROP_SET_USER_ID_POS = 0,
    /// User Property value
    MM_GEN_PROP_SET_USER_VAL_POS = 2,
};

/// Positions in Generic Admin Property Set/Set Unacknowledged message
enum mm_gen_prop_set_admin_pos
{
    /// Admin Property ID
    MM_GEN_PROP_SET_ADMIN_ID_POS = 0,
    /// Admin User Access
    MM_GEN_PROP_SET_ADMIN_ACCESS_POS = 2,
    /// Admin Property Value
    MM_GEN_PROP_SET_ADMIN_VAL_POS = 3,
};

/// Positions in Generic Manufacturer Property Set/Set Unacknowledged message
enum mm_gen_prop_set_manuf_pos
{
    /// Manufacturer Property ID
    MM_GEN_PROP_SET_MANUF_ID_POS = 0,
    /// Manufacturer User Access
    MM_GEN_PROP_SET_MANUF_ACCESS_POS = 2,
};

/// Positions in Generic User/Admin/Manufacturer Property Status message
enum mm_gen_prop_status_pos
{
    /// Property ID
    MM_GEN_PROP_STATUS_ID_POS = 0,
    /// User Access
    MM_GEN_PROP_STATUS_ACCESS_POS = 2,
    /// Property value
    MM_GEN_PROP_STATUS_VALUE_POS = 3,
};

/// Positions in Light Lightness Set/Set Unacknowledged message
enum mm_light_ln_set_pos
{
    /// Lightness
    MM_LIGHT_LN_SET_LIGHTNESS_POS = 0,
    /// TID
    MM_LIGHT_LN_SET_TID_POS = 2,
    /// Transition Time
    MM_LIGHT_LN_SET_TRANS_TIME_POS = 3,
    /// Delay
    MM_LIGHT_LN_SET_DELAY_POS = 4
};

/// Positions in Light Lightness Status message
enum mm_light_ln_status_pos
{
    /// Present Lightness
    MM_LIGHT_LN_STATUS_LIGHTNESS_POS = 0,
    /// Target Lightness
    MM_LIGHT_LN_STATUS_TGT_LIGHTNESS_POS = 2,
    /// Remaining Time
    MM_LIGHT_LN_STATUS_REM_TIME_POS = 4,
};

/// Positions in Light Lightness Linear Set/Set Unacknowledged message
enum mm_light_ln_lin_set_pos
{
    /// Lightness
    MM_LIGHT_LN_LIN_SET_LIGHTNESS_POS = 0,
    /// TID
    MM_LIGHT_LN_LIN_SET_TID_POS = 2,
    /// Transition Time
    MM_LIGHT_LN_LIN_SET_TRANS_TIME_POS = 3,
    /// Delay
    MM_LIGHT_LN_LIN_SET_DELAY_POS = 4
};

/// Positions in Light Lightness Linear Status message
enum mm_light_ln_lin_status_pos
{
    /// Present Lightness
    MM_LIGHT_LN_LIN_STATUS_LIGHTNESS_POS = 0,
    /// Target Lightness
    MM_LIGHT_LN_LIN_STATUS_TGT_LIGHTNESS_POS = 2,
    /// Remaining Time
    MM_LIGHT_LN_LIN_STATUS_REM_TIME_POS = 4,
};

/// Positions in Light Lightness Last Status message
enum mm_light_ln_last_status_pos
{
    /// Lightness
    MM_LIGHT_LN_LAST_STATUS_LIGHTNESS_POS = 0,
};

/// Positions in Light Lightness Default Set/Set Unacknowledged message
enum mm_light_ln_dflt_set_pos
{
    /// Lightness
    MM_LIGHT_LN_DFLT_SET_LIGHTNESS_POS = 0,
};

/// Positions in Light Lightness Linear Status message
enum mm_light_ln_dflt_status_pos
{
    /// Lightness
    MM_LIGHT_LN_DFLT_STATUS_LIGHTNESS_POS = 0,
};

/// Positions in Light Lightness Range Set/Set Unacknowledged message
enum mm_light_ln_range_set_pos
{
    /// Range Min
    MM_LIGHT_LN_RANGE_SET_MIN_POS = 0,
    /// Range Max
    MM_LIGHT_LN_RANGE_SET_MAX_POS = 2,
};

/// Positions in Light Lightness Range Status message
enum mm_light_ln_range_status_pos
{
    /// Status Code
    MM_LIGHT_LN_RANGE_STATUS_CODE_POS = 0,
    /// Range Min
    MM_LIGHT_LN_RANGE_STATUS_MIN_POS = 1,
    /// Range Max
    MM_LIGHT_LN_RANGE_STATUS_MAX_POS = 3,
};

/// Positions in Light CTL Set/Set Unacknowledged message
enum mm_light_ctl_set_pos
{
    /// CTL Lightness
    MM_LIGHT_CTL_SET_LIGHTNESS_POS = 0,
    /// CTL Temperature
    MM_LIGHT_CTL_SET_TEMP_POS = 2,
    /// CTL Delta UV
    MM_LIGHT_CTL_SET_DELTA_UV_POS = 4,
    /// TID
    MM_LIGHT_CTL_SET_TID_POS = 6,
    /// Transition Time
    MM_LIGHT_CTL_SET_TRANS_TIME_POS = 7,
    /// Delay
    MM_LIGHT_CTL_SET_DELAY_POS = 8
};

/// Positions in Light CTL Status message
enum mm_light_ctl_status_pos
{
    /// Present CTL Lightness
    MM_LIGHT_CTL_STATUS_LIGHTNESS_POS = 0,
    /// Present CTL Temperature
    MM_LIGHT_CTL_STATUS_TEMP_POS = 2,
    /// Target CTL Lightness
    MM_LIGHT_CTL_STATUS_TGT_LIGHTNESS_POS = 4,
    /// Target CTL Temperature
    MM_LIGHT_CTL_STATUS_TGT_TEMP_POS = 6,
    /// Remaining Time
    MM_LIGHT_CTL_STATUS_REM_TIME_POS = 8,
};

/// Positions in Light CTL Temperature Set/Set Unacknowledged message
enum mm_light_ctl_temp_set_pos
{
    /// CTL Temperature
    MM_LIGHT_CTL_TEMP_SET_TEMP_POS = 0,
    /// CTL Delta UV
    MM_LIGHT_CTL_TEMP_SET_DELTA_UV_POS = 2,
    /// TID
    MM_LIGHT_CTL_TEMP_SET_TID_POS = 4,
    /// Transition Time
    MM_LIGHT_CTL_TEMP_SET_TRANS_TIME_POS = 5,
    /// Delay
    MM_LIGHT_CTL_TEMP_SET_DELAY_POS = 6
};

/// Positions in Light CTL Temperature Status message
enum mm_light_ctl_temp_status_pos
{
    /// Present CTL Temperature
    MM_LIGHT_CTL_TEMP_STATUS_TEMP_POS = 0,
    /// Present CTL Delta UV
    MM_LIGHT_CTL_TEMP_STATUS_DELTA_UV_POS = 2,
    /// Target CTL Temperature
    MM_LIGHT_CTL_TEMP_STATUS_TGT_TEMP_POS = 4,
    /// Target CTL Delta UV
    MM_LIGHT_CTL_TEMP_STATUS_TGT_DELTA_UV_POS = 6,
    /// Remaining Time
    MM_LIGHT_CTL_TEMP_STATUS_REM_TIME_POS = 8,
};

/// Positions in Light CTL Temperature Range Set/Set Unacknowledged message
enum mm_light_ctl_temp_range_set_pos
{
    /// Range Min
    MM_LIGHT_CTL_TEMP_RANGE_SET_MIN_POS = 0,
    /// Range Max
    MM_LIGHT_CTL_TEMP_RANGE_SET_MAX_POS = 2,
};

/// Positions in Light CTL Temperature Range Status message
enum mm_light_ctl_temp_range_status_pos
{
    /// Status Code
    MM_LIGHT_CTL_TEMP_RANGE_STATUS_CODE_POS = 0,
    /// Range Min
    MM_LIGHT_CTL_TEMP_RANGE_STATUS_MIN_POS = 1,
    /// Range Max
    MM_LIGHT_CTL_TEMP_RANGE_STATUS_MAX_POS = 3,
};

/// Positions in Light CTL Default Set/Set Unacknowledged message
enum mm_light_ctl_dflt_set_pos
{
    /// Lightness
    MM_LIGHT_CTL_DFLT_SET_LIGHTNESS_POS = 0,
    /// Temperature
    MM_LIGHT_CTL_DFLT_SET_TEMP_POS = 2,
    /// Delta UV
    MM_LIGHT_CTL_DFLT_SET_DELTA_UV_POS = 4,
};

/// Positions in Light CTL Default Status message
enum mm_light_ctl_dflt_status_pos
{
    /// Lightness
    MM_LIGHT_CTL_DFLT_STATUS_LIGHTNESS_POS = 0,
    /// Temperature
    MM_LIGHT_CTL_DFLT_STATUS_TEMP_POS = 2,
    /// Delta UV
    MM_LIGHT_CTL_DFLT_STATUS_DELTA_UV_POS = 4,
};

/// Positions in Light HSL Set/Set Unacknowledged message
enum mm_light_hsl_set_pos
{
    /// HSL Lightness
    MM_LIGHT_HSL_SET_LIGHTNESS_POS = 0,
    /// HSL Hue
    MM_LIGHT_HSL_SET_HUE_POS = 2,
    /// HSL Saturation
    MM_LIGHT_HSL_SET_SAT_POS = 4,
    /// TID
    MM_LIGHT_HSL_SET_TID_POS = 6,
    /// Transition Time
    MM_LIGHT_HSL_SET_TRANS_TIME_POS = 7,
    /// Delay
    MM_LIGHT_HSL_SET_DELAY_POS = 8
};

/// Positions in Light HSL Status message
enum mm_light_hsl_status_pos
{
    /// HSL Lightness
    MM_LIGHT_HSL_STATUS_LIGHTNESS_POS = 0,
    /// HSL Hue
    MM_LIGHT_HSL_STATUS_HUE_POS = 2,
    /// HSL Saturation
    MM_LIGHT_HSL_STATUS_SAT_POS = 4,
    /// Remaining Time
    MM_LIGHT_HSL_STATUS_REM_TIME_POS = 6,
};

/// Positions in Light HSL Target Status message
enum mm_light_hsl_tgt_status_pos
{
    /// Target HSL Lightness
    MM_LIGHT_HSL_TGT_STATUS_LIGHTNESS_POS = 0,
    /// Target HSL Hue
    MM_LIGHT_HSL_TGT_STATUS_HUE_POS = 2,
    /// Target HSL Saturation
    MM_LIGHT_HSL_TGT_STATUS_SAT_POS = 4,
    /// Remaining Time
    MM_LIGHT_HSL_TGT_STATUS_REM_TIME_POS = 6,
};

/// Positions in Light HSL Hue Set/Set Unacknowledged message
enum mm_light_hsl_hue_set_pos
{
    /// Hue
    MM_LIGHT_HSL_HUE_SET_HUE_POS = 0,
    /// TID
    MM_LIGHT_HSL_HUE_SET_TID_POS = 2,
    /// Transition Time
    MM_LIGHT_HSL_HUE_SET_TRANS_TIME_POS = 3,
    /// Delay
    MM_LIGHT_HSL_HUE_SET_DELAY_POS = 4
};

/// Positions in Light HSL Hue Status message
enum mm_light_hsl_hue_status_pos
{
    /// Present Hue
    MM_LIGHT_HSL_HUE_STATUS_HUE_POS = 0,
    /// Target Hue
    MM_LIGHT_HSL_HUE_STATUS_TGT_HUE_POS = 2,
    /// Remaining Time
    MM_LIGHT_HSL_HUE_STATUS_REM_TIME_POS = 4,
};

/// Positions in Light HSL Saturation Set/Set Unacknowledged message
enum mm_light_hsl_sat_set_pos
{
    /// Saturation
    MM_LIGHT_HSL_SAT_SET_SAT_POS = 0,
    /// TID
    MM_LIGHT_HSL_SAT_SET_TID_POS = 2,
    /// Transition Time
    MM_LIGHT_HSL_SAT_SET_TRANS_TIME_POS = 3,
    /// Delay
    MM_LIGHT_HSL_SAT_SET_DELAY_POS = 4
};

/// Positions in Light HSL Saturation Status message
enum mm_light_hsl_sat_status_pos
{
    /// Present Saturation
    MM_LIGHT_HSL_SAT_STATUS_SAT_POS = 0,
    /// Target Saturation
    MM_LIGHT_HSL_SAT_STATUS_TGT_SAT_POS = 2,
    /// Remaining Time
    MM_LIGHT_HSL_SAT_STATUS_REM_TIME_POS = 4,
};

/// Positions in Light HSL Default Set/Set Unacknowledged message
enum mm_light_hsl_dflt_set_pos
{
    /// Lightness
    MM_LIGHT_HSL_DFLT_SET_LIGHTNESS_POS = 0,
    /// Hue
    MM_LIGHT_HSL_DFLT_SET_HUE_POS = 2,
    /// Saturation
    MM_LIGHT_HSL_DFLT_SET_SAT_POS = 4,
};

/// Positions in Light HSL Default Status message
enum mm_light_hsl_dflt_status_pos
{
    /// Lightness
    MM_LIGHT_HSL_DFLT_STATUS_LIGHTNESS_POS = 0,
    /// Hue
    MM_LIGHT_HSL_DFLT_STATUS_HUE_POS = 2,
    /// Saturation
    MM_LIGHT_HSL_DFLT_STATUS_SAT_POS = 4,
};

/// Positions in Light HSL Range Set/Set Unacknowledged message
enum mm_light_hsl_range_set_pos
{
    /// Hue Range Min
    MM_LIGHT_HSL_RANGE_SET_HUE_MIN_POS = 0,
    /// Hue Range Max
    MM_LIGHT_HSL_RANGE_SET_HUE_MAX_POS = 2,
    /// Saturation Range Min
    MM_LIGHT_HSL_RANGE_SET_SAT_MIN_POS = 4,
    /// Saturation Range Max
    MM_LIGHT_HSL_RANGE_SET_SAT_MAX_POS = 6,
};

/// Positions in Light HSL Range Status message
enum mm_light_hsl_range_status_pos
{
    /// Status
    MM_LIGHT_HSL_RANGE_STATUS_CODE_POS = 0,
    /// Hue Range Min
    MM_LIGHT_HSL_RANGE_STATUS_HUE_MIN_POS = 1,
    /// Hue Range Max
    MM_LIGHT_HSL_RANGE_STATUS_HUE_MAX_POS = 3,
    /// Saturation Range Min
    MM_LIGHT_HSL_RANGE_STATUS_SAT_MIN_POS = 5,
    /// Saturation Range Max
    MM_LIGHT_HSL_RANGE_STATUS_SAT_MAX_POS = 7,
};

/// Positions in Light xyL Set/Set Unacknowledged message
enum mm_light_xyl_set_pos
{
    /// xyL Lightness
    MM_LIGHT_XYL_SET_LIGHTNESS_POS = 0,
    /// xyL x
    MM_LIGHT_XYL_SET_X_POS = 2,
    /// xyL y
    MM_LIGHT_XYL_SET_Y_POS = 4,
    /// TID
    MM_LIGHT_XYL_SET_TID_POS = 6,
    /// Transition Time
    MM_LIGHT_XYL_SET_TRANS_TIME_POS = 7,
    /// Delay
    MM_LIGHT_XYL_SET_DELAY_POS = 8
};

/// Positions in Light xyL Status message
enum mm_light_xyl_status_pos
{
    /// xyL Lightness
    MM_LIGHT_XYL_STATUS_LIGHTNESS_POS = 0,
    /// xyL x
    MM_LIGHT_XYL_STATUS_X_POS = 2,
    /// xyL y
    MM_LIGHT_XYL_STATUS_Y_POS = 4,
    /// Remaining Time
    MM_LIGHT_XYL_STATUS_REM_TIME_POS = 6,
};

/// Positions in Light xyL Target Status message
enum mm_light_xyl_tgt_status_pos
{
    /// Target xyL Lightness
    MM_LIGHT_XYL_TGT_STATUS_LIGHTNESS_POS = 0,
    /// Target xyL x
    MM_LIGHT_XYL_TGT_STATUS_X_POS = 2,
    /// Target xyL y
    MM_LIGHT_XYL_TGT_STATUS_Y_POS = 4,
    /// Remaining Time
    MM_LIGHT_XYL_TGT_STATUS_REM_TIME_POS = 6,
};

/// Positions in Light xyL Default Set/Set Unacknowledged message
enum mm_light_xyl_dflt_set_pos
{
    /// Lightness
    MM_LIGHT_XYL_DFLT_SET_LIGHTNESS_POS = 0,
    /// xyL x
    MM_LIGHT_XYL_DFLT_SET_X_POS = 2,
    /// xyL y
    MM_LIGHT_XYL_DFLT_SET_Y_POS = 4,
};

/// Positions in Light xyL Default Status message
enum mm_light_xyl_dflt_status_pos
{
    /// Lightness
    MM_LIGHT_XYL_DFLT_STATUS_LIGHTNESS_POS = 0,
    /// xyL x
    MM_LIGHT_XYL_DFLT_STATUS_X_POS = 2,
    /// xyL y
    MM_LIGHT_XYL_DFLT_STATUS_Y_POS = 4,
};

/// Positions in Light xyL Range Set/Set Unacknowledged message
enum mm_light_xyl_range_set_pos
{
    /// xyL x Range Min
    MM_LIGHT_XYL_RANGE_SET_X_MIN_POS = 0,
    /// xyL x Range Max
    MM_LIGHT_XYL_RANGE_SET_X_MAX_POS = 2,
    /// xyL y Range Min
    MM_LIGHT_XYL_RANGE_SET_Y_MIN_POS = 4,
    /// xyL y Range Max
    MM_LIGHT_XYL_RANGE_SET_Y_MAX_POS = 6,
};

/// Positions in Light xyL Range Status message
enum mm_light_xyl_range_status_pos
{
    /// xyL x Range Min
    MM_LIGHT_XYL_RANGE_STATUS_X_MIN_POS = 0,
    /// xyL x Range Max
    MM_LIGHT_XYL_RANGE_STATUS_X_MAX_POS = 2,
    /// xyL y Range Min
    MM_LIGHT_XYL_RANGE_STATUS_Y_MIN_POS = 4,
    /// xyL y Range Max
    MM_LIGHT_XYL_RANGE_STATUS_Y_MAX_POS = 6,
};

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Transition time step resolution values
enum mm_trans_time_step
{
    /// Resolution is 100 milliseconds
    MM_TRANS_TIME_STEP_100MS = 0,
    /// Resolution is 1 second
    MM_TRANS_TIME_STEP_1S,
    /// Resolution is 10 seconds
    MM_TRANS_TIME_STEP_10S,
    /// Resolution is 10 minutes
    MM_TRANS_TIME_STEP_10M
};

/// Transition time format bitfield
/// 7            6                   0
/// +------------+-------------------+
/// | Resolution |  Number of steps  |
/// +------------+-------------------+
enum mm_trans_time_pos
{
    /// Number of steps
    MM_TRANS_TIME_STEP_NB_LSB = 0,
    MM_TRANS_TIME_STEP_NB_MASK = 0x3F,

    /// Resolution (@see enum mm_trans_time_step)
    MM_TRANS_TIME_RESOL_LSB = 6,
    MM_TRANS_TIME_RESOL_MASK = 0xC0,
};

/// Generic OnPowerUp state values
enum mm_on_power_up
{
    /// Off. After being powered up, the element is in an off state
    MM_ON_POWER_UP_OFF = 0,
    /// Default. After being powered up, the element is in an On state and uses default
    /// state values
    MM_ON_POWER_UP_DEFAULT,
    /// Restore. If a transition was in progress when powered down, the element restores
    /// the target state when powered up. Otherwise the element restores the state it
    /// was in when powered down.
    MM_ON_POWER_UP_RESTORE,
};

enum mm_status
{
    /// Command successfully processed
    MM_STATUS_SUCCESS = 0,
    /// The provided value for Range Min cannot be set
    MM_STATUS_CANNOT_SET_RANGE_MIN,
    /// The provided value for Range Max cannot be set
    MM_STATUS_CANNOT_SET_RANGE_MAX,
};

/// @} MM_DEFINES

#endif /* MM_DEFINES_ */
