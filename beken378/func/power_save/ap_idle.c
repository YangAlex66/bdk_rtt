#include "intc_pub.h"
#include "pwm_pub.h"
#include "rw_pub.h"
#include "rtos_pub.h"
#include "arm_arch.h"
#include "sys_ctrl_pub.h"
#include "power_save_pub.h"
#include "ps_debug_pub.h"
#include "target_util_pub.h"
#include "icu_pub.h"
#include "ap_idle_pub.h"


UINT32 ps_fake_global_ap_count = 0;
UINT32 ap_ps_enabled = 0;
UINT32 ap_rf_off = 0;
UINT32 ps_fake_pause_shot = 0;


#if CFG_USE_AP_IDLE
static beken_timer_t ps_ap_bcn_timer = {0};

//switch ap rf on/off
UINT32 ap_bcn_timer_pause_one_shot_get ( void );
void ap_bcn_timer_pause_one_shot_set ( UINT32 value );

void ap_ps_do_rf_up ( void )
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();

    if ( ap_rf_off == 1 ) {
        UINT32 reg = RF_HOLD_BY_AP_BIT;
        sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_SET, &reg);
        wifi_general_mac_state_set_active();
        ap_rf_off = 0;
    }

    GLOBAL_INT_RESTORE();
}

void ap_bcn_timer_real_handler ( void )
{
	UINT32 reg;
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	
    if ( 0 == ap_bcn_timer_pause_one_shot_get() ) {
        if ( ( ps_fake_global_ap_count  == 0 ) ) 
        {
            rtos_change_period ( &ps_ap_bcn_timer, 5 );
            wifi_general_mac_state_set_idle();
            ap_rf_off = 1;
            reg = RF_HOLD_BY_AP_BIT;
            sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_CLR, &reg);
        }
        else if ( ps_fake_global_ap_count == 1 ) 
        {
            rtos_change_period ( &ps_ap_bcn_timer, 4 );
            reg = RF_HOLD_BY_AP_BIT;
            sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_SET, &reg);
            wifi_general_mac_state_set_active();
            ap_rf_off = 0;
        }

        ps_fake_global_ap_count ++;
    }
	else {
		ap_bcn_timer_pause_one_shot_set ( 0 );
	}
	
	if ( ps_fake_global_ap_count == 2 )
		ps_fake_global_ap_count = 0;
		
	GLOBAL_INT_RESTORE();
}

void ap_bcn_timer_handler ( void *data )
{
	bmsg_ps_sender ( PS_BMSG_IOCTL_AP_PS_RUN );
}

void stop_global_ap_bcn_timer ( void )
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    UINT32 reg = RF_HOLD_BY_AP_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_SET, &reg);
    wifi_general_mac_state_set_active();
    ap_rf_off = 0;
#if CFG_USE_MCU_PS
    sctrl_mcu_exit();
#endif
    GLOBAL_INT_RESTORE();

    if ( rtos_is_timer_init ( &ps_ap_bcn_timer ) ) 
    {
        rtos_stop_timer ( &ps_ap_bcn_timer );
        rtos_deinit_timer ( &ps_ap_bcn_timer );
    }
}

void start_global_ap_bcn_timer ( void )
{
	UINT32 err;
	stop_global_ap_bcn_timer();
	err = rtos_init_timer ( &ps_ap_bcn_timer,
	                        5,
	                        ( timer_handler_t ) ap_bcn_timer_handler,
	                        NULL );
	err = rtos_start_timer ( &ps_ap_bcn_timer );
#if CFG_USE_MCU_PS
	sctrl_mcu_init();
#endif
}

void start_ap_idle_timer_msg ( void )
{
	if ( ap_ps_enable_get() && !bk_wlan_has_role ( VIF_STA )
	     && bk_wlan_has_role ( VIF_AP ) ) {
		if ( rwn_mgmt_if_ap_stas_empty() ) {
			bmsg_ps_sender ( PS_BMSG_IOCTL_AP_PS_START );
		}
	}
}

void ap_idle_start ( void )
{
	if ( !bk_wlan_has_role ( VIF_STA ) ) {
		ap_ps_enable_set();
	}
	else {
		os_printf ( "ap_idle_start fail!" );
		return;
	}
	
	start_ap_idle_timer_msg();
}

void ap_idle_stop ( void )
{
	if ( ap_ps_enable_get() && bk_wlan_has_role ( VIF_AP ) ) {
		bmsg_ps_sender ( PS_BMSG_IOCTL_AP_PS_STOP );
	}
}

#endif
void ap_bcn_timer_pause_one_shot_set ( UINT32 value )
{
	ps_fake_pause_shot = value;
}
UINT32 ap_bcn_timer_pause_one_shot_get ( void )
{
	uint32_t value;
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	value =  ps_fake_pause_shot;
	GLOBAL_INT_RESTORE();
	return value;
}
void ap_ps_enable_set ( void )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	ap_ps_enabled = 1;
	GLOBAL_INT_RESTORE();
}

void ap_ps_enable_clear ( void )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	ap_ps_enabled = 0;
	GLOBAL_INT_RESTORE();
}

UINT32 ap_ps_enable_get ( void )
{
	uint32_t value;
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	value =  ap_ps_enabled;
	GLOBAL_INT_RESTORE();
	return value;
}

UINT32 ap_ap_rf_sleep_get ( void )
{
	uint32_t value;
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	value =  ap_rf_off;
	GLOBAL_INT_RESTORE();
	return value;
}


UINT32 ap_if_ap_rf_sleep ( void )
{
	if ( bk_wlan_has_role ( VIF_AP ) ) {
		return ap_ap_rf_sleep_get();
	}
	else {
		return 1;
	}
}

