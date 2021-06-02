#ifndef _IP_PORT_H_
#define _IP_PORT_H_

#include "rwnx_config.h"
#include "reg_mac_core.h"
#include "reg_mac_pl.h"
#include "mm.h"

typedef enum{
	WN_TX_CFM_ISR_EVT,
	WN_TX_RESET_EVT,
	WN_NEXT_TBTT_CALC_EVT,
	WN_NX_MULTI_ROLE_CAL_BCN_DTIM_PER,
	WN_STA_BEACON_IND_MISS,
	WN_HALMAC_INIT,
	WN_MM_HW_INFO_SET,
	WN_MM_SET_BEACON_INT_EVT,
	WN_IND_MOREDATA_EVT,
	
}wifi_notice_ble_event;


extern int wifi_notice_ble_status(int evt,unsigned int param,unsigned int param2);
extern int wn_txl_hd_pending_is_allow(uint8_t flag,void *first_thd,
	                          void *last_thd,
	                          uint8_t access_category);
extern UINT32 sddev_control(char *dev_name, UINT32 cmd, VOID *param);
extern void delay05us(INT32 num);
extern int bmsg_ps_handler_rf_ps_mode_real_wakeup(void);

#endif

