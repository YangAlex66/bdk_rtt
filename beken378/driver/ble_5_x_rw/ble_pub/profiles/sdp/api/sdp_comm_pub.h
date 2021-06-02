#ifndef _SDP_COMM_PUB_H_
#define _SDP_COMM_PUB_H_

#include "rwip_config.h"             // SW configuration

#define BLE_APP_SDP_LOG                1

#define BLE_APP_SDP_ALL                0x00
#define BLE_APP_SDP_WARN               0x01
#define BLE_APP_SDP_IMPO               0x02   ///important
#define BLE_APP_SDP_NONE               0xFF

#define BLE_APP_SDP_DBG_LEVEL          BLE_APP_SDP_IMPO

#define BLE_APP_SDP_DBG_CHECK(LEVEL)   (BLE_APP_SDP_LOG && ((LEVEL) >= BLE_APP_SDP_DBG_LEVEL))

#if (BLE_APP_PRESENT && (BLE_CENTRAL) && (BLE_SDP_CLIENT))

#include "sdp_comm.h"
#include "att.h"


enum sdpc_type
{
	/// SERVICE Char  value
	SDPC_OPERATE_HANDLE,
	SDPC_OPERATE_UUID,
};

/// Parameters of the @ref SDP_ENABLE_REQ message
struct sdp_enable_req
{
	uint8_t conidx;
	///Connection type
	uint8_t con_type;
};

/// Peer service info that can be read
enum sdpc_info
{
	/// SERVICE Char  value
	SDPC_CHAR_VAL,
	///  Client Characteristic Configuration
	SDPC_CHAR_NTF_CFG,
	  ///  Client Characteristic User Description
	SDPC_CHAR_USER_DESC_VAL,
	/// Characteristic Presentation Format
	SDPC_CHAR_PRES_FORMAT,

	SDPC_INFO_MAX,
};

///Parameters of the @ref SDP_READ_INFO_REQ message
struct sdp_read_info_req
{
	uint8_t conidx;

	///Characteristic info @see enum sdpc_info
	enum sdpc_info info;

	enum sdpc_type type;

	uint16_t handle;
	uint8_t uuid_len;
	uint8_t uuid[ATT_UUID_128_LEN];
};

///Parameters of the @ref SDP_W_NTF_CFG_REQ message
struct sdp_write_ntf_cfg_req
{
	uint8_t conidx;
	uint8_t info;
	uint16_t handle;
	///Notification Configuration
	uint16_t ntf_cfg;
	uint16_t seq_num;
};

///Parameters of the @ref  SDP_WRITE_INFO_REQ message
struct sdp_write_info_req
{
	uint8_t conidx;
	uint8_t info;
	uint8_t operation;

	uint16_t handle;
	uint16_t length;
	uint16_t seq_num;
	uint8_t data[__ARRAY_EMPTY];
};


/// Parameters of the @ref SDP_ENABLE_RSP message
struct sdp_enable_rsp
{
	uint8_t conidx;
	/// Status
	uint8_t status;
};


/////////extern function
extern uint16_t sdp_add_profiles_num_get(uint8_t conidx);
extern struct prf_sdp_db_env * sdp_profiles_sdp_db_env(unsigned char conhdl,unsigned short handle);
#endif  ////BLE_CENTRAL
#endif  ///_SDP_COMM_PUB_H_

