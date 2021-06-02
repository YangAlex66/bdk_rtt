/**
 ****************************************************************************************
 *
 * @file app_light_ali_server.h
 *
 * @brief mesh Application Module entry point
 *
 * @auth  gang.cheng
 *
 * @date  2018.11.13
 *
 * Copyright (C) Beken 2009-2016
 *
 *
 ****************************************************************************************
 */

#ifndef _APP_LIGHT_LIGHT_ALI_SERVER_H
#define _APP_LIGHT_LIGHT_ALI_SERVER_H

#include "mal.h"
#include "m_fnd_int.h"

extern m_lid_t g_vdr_lid, g_ln_mdl_lid, g_ctl_ln_mdl_lid;
extern m_lid_t g_hsl_ln_mdl_lid, g_ctl_mdl_lid;
extern m_lid_t g_ctlt_mdl_lid, g_oo_mdl_lid;
extern m_lid_t g_hsl_mdl_lid, g_hslh_mdl_lid, g_hslsat_mdl_lid;

void light_ali_app_init(void);

void light_unBind_complete(void);

void light_prov_start(void);

void app_ai_lights_models_init(uint8_t elmt_idx);

int32_t light_scene_server_data(const m_fnd_model_env_p pmodel_info, uint32_t type, void *pargs);

void ali_light_status_report(uint16_t attr, uint8_t len, uint8_t status);

#endif // _APP_LIGHT_LIGHT_ALI_SERVER_H

