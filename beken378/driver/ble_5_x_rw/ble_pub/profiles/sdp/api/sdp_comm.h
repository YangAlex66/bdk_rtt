#ifndef _SDP_COMM_H_
#define _SDP_COMM_H_

#include "rwip_config.h"             // SW configuration


#if (BLE_APP_PRESENT && (BLE_CENTRAL) && (BLE_SDP_CLIENT))
#include "rwprf_config.h"
#include "rwble_hl_config.h"
#include "prf_types.h"
#include <stdint.h>
#include <stdbool.h>
#include "kernel_task.h"
#include "co_utils.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "sdp_comm_task.h"
#include "gattc_task.h"
#include "ble_api_5_x.h"

///#define BLE_NB_PROFILES_ADD_MAX ( BLE_NB_PROFILES )
#define BLE_NB_PROFILES_ADD_MAX ( 20 )
#define SDP_NB_CHAR_INSTANCES_MAX         (20)
#define SDP_NB_DESC_INSTANCES_MAX         (SDP_NB_CHAR_INSTANCES_MAX)

///Maximum number of SDP Client task instances
#define SDP_IDX_MAX     (BLE_NB_PROFILES_ADD_MAX)

#define SDP_NB_SERVICE_INSTANCES_MAX      (BLE_NB_PROFILES_ADD_MAX)

#define INVALID_STATUS  (0xFFFF)
#define FREE_STATUS     (0xFFFF)
#define USED_STATUS     (0xA5A5)

typedef enum  {
	SDP_NTC_REG_FAILED,
	SDP_NTC_FILTRATION_SVR,
} sdp_notice_t;

struct sdp_chars_descs_inf
{
	struct prf_char_inf *chars_inf;

	//struct prf_char_desc_inf *descs;
	struct prf_char_desc_inf *descs_inf;
};

///Structure containing the characteristics handles, value handles and descriptors
struct sdp_content
{
	/// service info
	struct prf_svc svc;

	/// Characteristic Info:
	unsigned char chars_nb;
	unsigned char char_idx;
	/// Descriptor handles:
	unsigned char descs_nb;

	struct sdp_chars_descs_inf chars_descs_inf;
};

struct prf_sdp_db_env
{
	///index of  instances added
	unsigned char prf_idx;

	struct sdp_content *sdp_cont;
};

/// SDP 'Profile' Client environment variable
struct sdp_env_tag
{
	/// profile environment
	prf_env_t prf_env;

	unsigned char conidx;

	unsigned char rfu;

	uint16_t use_status;
		/// on-going operation
	struct kernel_msg * operation;
	/// Environment variable pointer for db
	struct prf_sdp_db_env  *prf_db_env;
	/// State of different task instances
	kernel_state_t state[SDP_IDX_MAX];
};

struct sdp_env_init_t
{
	uint16_t add_profiles_nums[BLE_CONNECTION_MAX];
	uint16_t used_status[BLE_NB_PROFILES_ADD_MAX];
	uint16_t sdp_need_dis_flag[BLE_NB_PROFILES_ADD_MAX];
	struct sdp_env_tag sdp_env[BLE_NB_PROFILES_ADD_MAX];
};

extern uint8_t sdp_enable_all_server_ntf_ind(uint8_t conidx,uint8_t  reset);
extern void sdp_discover_all_service(uint8_t conidx);
extern struct prf_sdp_db_env* sdp_extract_svc_info(uint8_t conhdl,struct gattc_sdp_svc_ind const *ind);
extern void sdp_add_profiles(uint8_t conidx,struct prf_sdp_db_env *db_env);

extern void sdp_notice_event(sdp_notice_t sdp_evt, void *param);
extern void sdp_service_init(void);

#endif  ////BLE_CENTRAL
#endif  ///_SDP_COMM_H_


