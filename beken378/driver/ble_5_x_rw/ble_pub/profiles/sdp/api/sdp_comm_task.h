#ifndef _SDP_COMM_TASK_H_
#define _SDP_COMM_TASK_H_

#include "rwip_config.h"             // SW configuration


#if (BLE_APP_PRESENT && (BLE_CENTRAL) && (BLE_SDP_CLIENT))
#include "sdp_comm_pub.h"
#include <stdint.h>         // Standard Integer
#include "rwip_task.h"      // Task definitions
#include "kernel_task.h"        // Kernel Task


/// Possible states of the SDP task
enum sdp_state
{
	/// Disconnected state
	SDP_FREE,
	/// IDLE state
	SDP_IDLE,
	/// Busy State

	SDP_DISCOVING,
	SDP_BUSY,

	/// Number of defined states.
	SDP_STATE_MAX
};

enum
{
	SDP_SERVICE_NEED_DISCOVER,
	SDP_SERVICE_NO_NEED_DISCOVER,
};


////SDP : Service discovery procedure


enum sdp_msg_id
{
	/// Start the Battery Service Client Role - at connection
	SDP_ENABLE_REQ = MSG_ID(SDP, 0x00),////TASK_BLE_FIRST_MSG(TASK_BLE_ID_SDP),
	///Confirm that cfg connection has finished with discovery results, or that normal cnx started
	SDP_ENABLE_RSP = MSG_ID(SDP, 0x01),

	/// Read Characteristic xxxx Request
	SDP_READ_INFO_REQ  = MSG_ID(SDP, 0x02),
	/// Read Characteristic xxxx Response
	SDP_READ_INFO_RSP = MSG_ID(SDP, 0x03),

	/// Write Characteristic Value Request
	SDP_WRITE_VALUE_INFO_REQ = MSG_ID(SDP, 0x04),
	///  Write Characteristic Value response
	SDP_WRITE_VALUE_INFO_RSP = MSG_ID(SDP, 0x05),

	/// Write  Notification Configuration Value request
	SDP_WRITE_NTF_CFG_REQ = MSG_ID(SDP, 0x06),
	/// Write  Notification Configuration Value response
	SDP_WRITE_NTF_CFG_RSP = MSG_ID(SDP, 0x07),

	/// Indicate to APP that the  ntf value has been received
	SDP_NTF_IND = MSG_ID(SDP, 0x08),
};


////////////Internal function
extern void sdp_task_init(struct kernel_task_desc *task_desc,void* sdp_env);

#endif  ////BLE_CENTRAL
#endif  ///_SDP_COMM_TASK_H_


