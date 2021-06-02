#include "intc_pub.h"
#include "pwm_pub.h"
#include "rw_pub.h"
#include "rtos_pub.h"
#include "arm_arch.h"
#include "sys_ctrl_pub.h"
#include "mcu_ps.h"
#include "mcu_ps_pub.h"
#include "power_save_pub.h"
#include "ps_debug_pub.h"
#include "target_util_pub.h"
#include "icu_pub.h"
#include "fake_clock_pub.h"
#include "bk_timer_pub.h"
#include "drv_model_pub.h"

static MCU_PS_INFO mcu_ps_info = {
	.mcu_ps_on = 0,
	.peri_busy_count = 0,
	.mcu_prevent = 0,

};

#if CFG_USE_TICK_CAL
MCU_PS_TSF mcu_ps_tsf_save;
MCU_PS_MACHW_TM mcu_ps_machw_save;
int increase_tick = 0;
#endif

#if CFG_USE_MCU_PS
#if (CFG_SUPPORT_ALIOS)
static UINT32 sleep_pwm_t, wkup_type;
#endif

void mcu_ps_cal_increase_tick ( UINT32 *lost_p );

void peri_busy_count_add ( void )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	mcu_ps_info.peri_busy_count ++;
	GLOBAL_INT_RESTORE();
}

void peri_busy_count_dec ( void )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	mcu_ps_info.peri_busy_count --;
	GLOBAL_INT_RESTORE();
}

UINT32 peri_busy_count_get ( void )
{
	return mcu_ps_info.peri_busy_count;
}

void mcu_prevent_set ( UINT32 prevent )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	mcu_ps_info.mcu_prevent |= prevent;
	GLOBAL_INT_RESTORE();
}

void mcu_prevent_clear ( UINT32 prevent )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	mcu_ps_info.mcu_prevent &= ~ prevent;
	GLOBAL_INT_RESTORE();
}

UINT32 mcu_prevent_get ( void )
{
	return mcu_ps_info.mcu_prevent;
}

void mcu_ps_enable ( void )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	mcu_ps_info.mcu_ps_on = 1;
	GLOBAL_INT_RESTORE();
}

void mcu_ps_disable ( void )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	mcu_ps_info.mcu_ps_on = 0;
	GLOBAL_INT_RESTORE();
}

UINT32 mcu_power_save ( UINT32 sleep_tick )
{
	UINT32 sleep_ms, sleep_pwm_t, param, uart_miss_us = 0, miss_ticks = 0;
	UINT32 wkup_type, wastage = 0;
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();

	if ( ( mcu_ps_info.mcu_ps_on == 1 )
	     && ( peri_busy_count_get() == 0 )
	     && ( mcu_prevent_get() == 0 )) {
		do {
			sleep_ms = BK_TICKS_TO_MS ( sleep_tick );

			if ( sleep_ms <= 2 ) {
				break;
			}

			sleep_ms = sleep_ms - FCLK_DURATION_MS;//early wkup
			sleep_pwm_t = ( sleep_ms * 32 );

			if ( ( int32 ) sleep_pwm_t <= 64 ) {
				break;
			}

#if (CFG_SOC_NAME == SOC_BK7231)

			if ( sleep_pwm_t > 65535 )
				sleep_pwm_t = 65535;
			else
#endif
				if ( sleep_pwm_t < 64 )
					sleep_pwm_t = 64;
					
			if ( sctrl_if_mcu_can_sleep() ) {
#if (CHIP_U_MCU_WKUP_USE_TIMER && ((CFG_SOC_NAME == SOC_BK7231U) || (SOC_BK7231N == CFG_SOC_NAME)))
			
				if ( fclk_get_tick_id() >= BK_PWM_TIMER_ID0 ) {
					ps_pwm_disable();
				}

				ps_timer3_enable ( sleep_pwm_t );
#else
				ps_pwm_suspend_tick ( sleep_pwm_t );
#endif
			}
			
#if (CHIP_U_MCU_WKUP_USE_TIMER && ((CFG_SOC_NAME == SOC_BK7231U) || (SOC_BK7231N == CFG_SOC_NAME)))
			param = ( 0xfffff  & ( ~PWD_TIMER_26M_CLK_BIT ) & ( ~PWD_TIMER_32K_CLK_BIT ) & ( ~PWD_UART2_CLK_BIT )
			          & ( ~PWD_UART1_CLK_BIT )
			        );
#else
			param = ( 0xfffff & ( ~PWD_MCU_WAKE_PWM_BIT ) & ( ~PWD_UART2_CLK_BIT )
			          & ( ~PWD_UART1_CLK_BIT )
			        );
#endif
			sctrl_mcu_sleep ( param );
#if (CHIP_U_MCU_WKUP_USE_TIMER && ((CFG_SOC_NAME == SOC_BK7231U) || (SOC_BK7231N == CFG_SOC_NAME)))
			ps_timer3_measure_prepare();
#endif
			wkup_type = sctrl_mcu_wakeup();
#if (CHIP_U_MCU_WKUP_USE_TIMER && ((CFG_SOC_NAME == SOC_BK7231U) || (SOC_BK7231N == CFG_SOC_NAME)))
			
			if ( 1 == wkup_type ) {
				wastage = 768;

				if ( ! ( sctrl_if_rf_sleep() || power_save_if_rf_sleep()
#if CFG_SUPPORT_BLE
					|| ble_switch_mac_sleeped
#endif
					) ) {
					ps_timer3_disable();
					mcu_ps_machw_cal();
				}
				else {
					miss_ticks =  ( ps_timer3_disable() + ( uart_miss_us + wastage ) / 1000 ) / FCLK_DURATION_MS;

					if ( fclk_get_tick_id() >= BK_PWM_TIMER_ID0 ) {
						ps_pwm_enable();
					}
				}
			}

#else
			{
				if ( 1 == wkup_type ) {
					wastage = 24;
				}

				if ( ps_pwm_int_status() ) {
					miss_ticks = ( sleep_pwm_t + ( uart_miss_us >> 5 ) + wastage ) / ( FCLK_DURATION_MS * 32 );
				}
				else {
					if ( ! ( sctrl_if_rf_sleep() || power_save_if_rf_sleep() ) ) {
						mcu_ps_machw_cal();
					}
					else {
						miss_ticks = ( ( uart_miss_us >> 5 ) + wastage ) / ( FCLK_DURATION_MS * 32 );
					}
				}

				miss_ticks += FCLK_DURATION_MS;//for early wkup
			}
			ps_pwm_resume_tick();
#endif
		}
		while ( 0 );
	}
	else {
	}

	mcu_ps_cal_increase_tick ( & miss_ticks );
	GLOBAL_INT_RESTORE();
	if(miss_ticks <0){
		os_printf("miss_ticks error\r\n");
	}
	return miss_ticks;
}

#if (CFG_SUPPORT_ALIOS)
int aos_mcu_ps_timer_start ( UINT32 tm_us )
{
	UINT32 sleep_ms;

	if ( mcu_ps_info.mcu_ps_on == 1
	     && ( peri_busy_count_get() == 0 )
	     && ( mcu_prevent_get() == 0 )
	     && ( txl_sleep_check() )
	   ) {
		sleep_ms = tm_us / 1000;

		if ( sleep_ms <= 2 ) {
			return -1;
		}

		sleep_ms = sleep_ms - FCLK_DURATION_MS;//early wkup
		sleep_pwm_t = ( sleep_ms * 32 );

		if ( ( int32 ) sleep_pwm_t <= 64 ) {
			return -1;
		}

#if (CFG_SOC_NAME == SOC_BK7231)

		if ( sleep_pwm_t > 65535 )
			sleep_pwm_t = 65535;
		else
#endif
			if ( sleep_pwm_t < 64 )
				sleep_pwm_t = 64;
				
#if (CHIP_U_MCU_WKUP_USE_TIMER && ((CFG_SOC_NAME == SOC_BK7231U) || (SOC_BK7231N == CFG_SOC_NAME)))
				
		if ( fclk_get_tick_id() >= BK_PWM_TIMER_ID0 ) {
			ps_pwm_disable();
		}

		ps_timer3_enable ( sleep_pwm_t );
#else
		ps_pwm_suspend_tick ( sleep_pwm_t );
#endif
		return 0;
	}
	else {
		return -1;
	}
}

void aos_mcu_ps_sleep()
{
	UINT32 param;
	GLOBAL_INT_DECLARATION();
#if (CHIP_U_MCU_WKUP_USE_TIMER && ((CFG_SOC_NAME == SOC_BK7231U) || (SOC_BK7231N == CFG_SOC_NAME)))
	param = ( 0xfffff & ( ~PWD_TIMER_32K_CLK_BIT ) & ( ~PWD_UART2_CLK_BIT )
	          & ( ~PWD_UART1_CLK_BIT )
	        );
#else
	param = ( 0xfffff & ( ~PWD_MCU_WAKE_PWM_BIT ) & ( ~PWD_UART2_CLK_BIT )
	          & ( ~PWD_UART1_CLK_BIT )
	        );
#endif
	GLOBAL_INT_DISABLE();
	sctrl_mcu_sleep ( param );
#if (CHIP_U_MCU_WKUP_USE_TIMER && ((CFG_SOC_NAME == SOC_BK7231U) || (SOC_BK7231N == CFG_SOC_NAME)))
	ps_timer3_measure_prepare();
#endif
	wkup_type = sctrl_mcu_wakeup();
	GLOBAL_INT_RESTORE();
}

int aos_mcu_ps_timer_stop ( UINT64 *tm_us )
{
	UINT32 miss_ticks = 0, wastage = 0;
#if (CHIP_U_MCU_WKUP_USE_TIMER && ((CFG_SOC_NAME == SOC_BK7231U) || (SOC_BK7231N == CFG_SOC_NAME)))
	
	if ( 1 == wkup_type ) {
		wastage = 768;
	}

	miss_ticks =  ( ps_timer3_disable() * 1000 + ( wastage ) ) ;
	*tm_us = miss_ticks;

	if ( fclk_get_tick_id() >= BK_PWM_TIMER_ID0 ) {
		ps_pwm_enable();
	}

#else
	{
		if ( 1 == wkup_type ) {
			wastage = 24;
		}

		if ( ps_pwm_int_status() ) {
			miss_ticks = ( sleep_pwm_t + wastage ) * 32;
		}
		else {
			if ( ! ( sctrl_if_rf_sleep() || power_save_if_rf_sleep() ) ) {
				mcu_ps_machw_cal();
			}
			else {
				miss_ticks = ( wastage ) * 32;
			}
		}

		miss_ticks += FCLK_DURATION_MS * 1000;//for early wkup
	}
	*tm_us = miss_ticks;
	ps_pwm_resume_tick();
#endif
	return 0;
}
#endif

void mcu_ps_dump ( void )
{
	os_printf ( "mcu:%x\r\n", mcu_ps_info.mcu_ps_on );
}

#if (CHIP_U_MCU_WKUP_USE_TIMER)
void timer3_isr ( UINT8 param )
{
	//os_printf("t3\r\n");
}

void mcu_init_timer3 ( void )
{
	timer_param_t param;
	param.channel = 3;
	param.div = 1;
	param.period = 50 * 32;
	param.t_Int_Handler = timer3_isr;
	sddev_control ( TIMER_DEV_NAME, CMD_TIMER_INIT_PARAM, &param );
}
#endif

void mcu_ps_init ( void )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();

	if ( fclk_get_tick_id() >= BK_PWM_TIMER_ID0 ) {
		mcu_init_timer3();
	}

	if ( 0 == mcu_ps_info.mcu_ps_on ) {
		sctrl_mcu_init();
		mcu_ps_info.mcu_ps_on = 1;
		mcu_ps_info.peri_busy_count = 0;
		os_printf ( "%s %d\r\n", __FUNCTION__, mcu_ps_info.mcu_prevent );
	}

	mcu_ps_machw_init();
	GLOBAL_INT_RESTORE();
}

void mcu_ps_exit ( void )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();

	if ( 1 == mcu_ps_info.mcu_ps_on ) {
		mcu_ps_info.mcu_ps_on = 0;
		sctrl_mcu_exit();
		mcu_ps_info.peri_busy_count = 0;
		os_printf ( "%s %d\r\n", __FUNCTION__, mcu_ps_info.mcu_prevent );
	}

	mcu_ps_machw_reset();
	GLOBAL_INT_RESTORE();
}
#endif

#if CFG_USE_TICK_CAL
static struct mac_addr bssid;
static UINT64 last_tsf = 0;
extern UINT32 use_cal_net;
void mcu_ps_bcn_callback ( uint8_t *data, int len, wifi_link_info_t *info )
{
	struct bcn_frame *bcn = ( struct bcn_frame * ) data;
	UINT64 tsf_start_peer = bcn->tsf;
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();

	if ( use_cal_net == 0 ) {
		last_tsf = tsf_start_peer;
		GLOBAL_INT_RESTORE();
		return  ;
	}

	if ( memcmp ( & ( bcn->h.addr3 ), &bssid, 6 ) || ( last_tsf >= tsf_start_peer ) ) {
		mcu_ps_tsf_cal ( ( UINT64 ) 0 );
		memcpy ( &bssid, & ( bcn->h.addr3 ), 6 );
	}
	else {
		mcu_ps_tsf_cal ( ( UINT64 ) tsf_start_peer );
	}

	last_tsf = tsf_start_peer;
	GLOBAL_INT_RESTORE();
}

void mcu_ps_increase_clr ( void )
{
	increase_tick = 0;
}
void mcu_ps_cal_increase_tick ( UINT32 *lost_p )
{
	int32 lost = * lost_p;
	GLOBAL_INT_DECLARATION();

	if ( ( lost <= 0 ) || ( 0 == increase_tick ) )
		return;

#if 0

	if ( ( increase_tick < ( -5000 ) ) || ( increase_tick > 5000 ) ) {
		os_printf ( "--itick--:%x\r\n", increase_tick );
		increase_tick = 0;
	}

#endif
	GLOBAL_INT_DISABLE();
	lost += increase_tick;

	if ( lost < 0 ) {
		increase_tick = lost;
		lost = 0;
	}
	else {
		increase_tick = 0;
	}

	*lost_p = lost;
	GLOBAL_INT_RESTORE();
}


uint32 mcu_ps_need_pstick ( void )
{
	static uint32 need_pass = 0;
	uint32 ret;
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();

	if ( ( mcu_ps_is_on() && power_save_if_ps_rf_dtim_enabled() ) ) {
		ret = FCLK_DURATION_MS;
	}

	if ( ( ( need_pass ++ ) % 5 ) == 0 ) {
		uint32 lost = FCLK_DURATION_MS;
		mcu_ps_cal_increase_tick ( &lost );

		if ( !lost ) {
			//os_printf("p_t\r\n");
		}

		ret = lost;
	}
	else {
		ret = FCLK_DURATION_MS;
	}

	GLOBAL_INT_RESTORE();
	return ret;
}


UINT32 mcu_ps_tsf_cal ( UINT64 tsf )
{
#if (CFG_SUPPORT_ALIOS)
	UINT64 fclk, tmp2, tmp4;
#else
	UINT32 fclk, tmp2, tmp4;
#endif
	UINT64 machw, tmp1, tmp3;
	INT32 loss;
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();

	if ( ( 0 == tsf ) || ( 0 == mcu_ps_tsf_save.first_tsf ) ) {
		goto TFS_RESET;
	}

	fclk = BK_TICKS_TO_MS ( fclk_get_tick() );
	machw = tsf;
	tmp3 = mcu_ps_tsf_save.first_tsf;
	tmp4 = mcu_ps_tsf_save.first_tick;

	if ( machw < mcu_ps_tsf_save.first_tsf ) {
		goto TFS_RESET;
	}
	else {
		tmp1 = machw - mcu_ps_tsf_save.first_tsf;
	}

	tmp2 = ( fclk - mcu_ps_tsf_save.first_tick );
	tmp1 /= 1000;
	loss = ( INT32 ) ( tmp1  - ( UINT64 ) tmp2 );

	if ( loss > 0 ) {
		if ( loss > 5000 ) {
			os_printf ( "tsf cal_:%x \r\n", loss );
#if (CFG_SUPPORT_ALIOS)
			os_printf ( "%x %x\r\n", ( UINT32 ) ( fclk >> 32 ), ( UINT32 ) ( fclk ) );
			os_printf ( "%x %x\r\n", ( UINT32 ) ( tmp2 >> 32 ), ( UINT32 ) ( tmp2 ) );
			os_printf ( "%x %x\r\n", ( UINT32 ) ( tmp4 >> 32 ), ( UINT32 ) ( tmp4 ) );
#else
			os_printf ( "%x %x %x\r\n", fclk, tmp2, tmp4 );
#endif
			os_printf ( "tsf:%x %x\r\n", ( UINT32 ) ( machw >> 32 ), ( UINT32 ) machw );
			os_printf ( "tmp3:%x %x\r\n", ( UINT32 ) ( tmp3 >> 32 ), ( UINT32 ) tmp3 );
			os_printf ( "tmp1:%x %x\r\n", ( UINT32 ) ( tmp1 >> 32 ), ( UINT32 ) tmp1 );

			if ( loss > 50000 ) {
				goto TFS_RESET;
			}
		}

		fclk_update_tick ( BK_MS_TO_TICKS ( loss ) );
		mcu_ps_machw_init();
		increase_tick = 0;
	}
	else {
		if ( loss < 0 ) {
			if ( loss < ( -50000 ) ) {
				goto TFS_RESET;
			}

			increase_tick = loss;
		}
	}

	GLOBAL_INT_RESTORE();
	return 0 ;
TFS_RESET:
	mcu_ps_tsf_save.first_tsf = tsf;
	fclk = BK_TICKS_TO_MS ( fclk_get_tick() );
	mcu_ps_tsf_save.first_tick = fclk;
	os_printf ( "mcu_ps_tsf_cal init\r\n" );
	GLOBAL_INT_RESTORE();
	return 0 ;
}

UINT32 mcu_ps_machw_reset ( void )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	mcu_ps_machw_save.fclk_tick = 0;
	mcu_ps_machw_save.machw_tm = 0;
	GLOBAL_INT_RESTORE();
	return 0;
}

UINT32 mcu_ps_machw_init ( void )
{
#if (CFG_SUPPORT_ALIOS)
	UINT64 fclk;
#else
	UINT32 fclk;
#endif
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	fclk = BK_TICKS_TO_MS ( fclk_get_tick() );
	mcu_ps_machw_save.fclk_tick = fclk;
	mcu_ps_machw_save.machw_tm = hal_machw_time();
	GLOBAL_INT_RESTORE();
	return 0;
}

UINT32 mcu_ps_machw_cal ( void )
{
#if (CFG_SUPPORT_ALIOS)
	UINT64 fclk, tmp2;
#else
	UINT32 fclk, tmp2;
#endif
	UINT32 machw, tmp1;
	UINT32 lost;
	GLOBAL_INT_DECLARATION();

	if ( ! ( ( 1 == bk_unconditional_sleep_mode_get() ) ||
	         ( mcu_ps_is_on() && power_save_if_ps_rf_dtim_enabled() ) ) ) {
		return 0;
	}
	if ( ( 0 == mcu_ps_machw_save.machw_tm ) || ( 0xdead5555 == mcu_ps_machw_save.machw_tm )
	     || ( 0xdead5555 == hal_machw_time() ) ) {
		mcu_ps_machw_init();
		return 0 ;
	}

	GLOBAL_INT_DISABLE();
	fclk = BK_TICKS_TO_MS ( fclk_get_tick() );
	machw = hal_machw_time();
	tmp1 = ( machw - mcu_ps_machw_save.machw_tm );
	tmp2 = ( fclk - mcu_ps_machw_save.fclk_tick );
	tmp1 /= 1000;
	lost = ( INT32 ) ( tmp1  - tmp2 );

	if ( ( lost < ( 0xFFFFFFFF >> 1 ) ) && ( lost > 0 ) ) {
		if ( lost > 5000 ) {
			os_printf ( "hw cal_:%x %x %x\r\n", lost, machw, mcu_ps_machw_save.machw_tm );

			if ( lost > 50000 ) {
				goto HWCAL_RESET;
			}
		}

		//os_printf("mc:%d\r\n",lost);
		mcu_ps_cal_increase_tick ( &lost );
		fclk_update_tick ( BK_MS_TO_TICKS ( lost ) );
		mcu_ps_machw_init();
		//os_printf("rmc:%d\r\n",lost);
	}
	else {
	}

	GLOBAL_INT_RESTORE();
	return 0 ;
HWCAL_RESET:
	mcu_ps_machw_init();
	GLOBAL_INT_RESTORE();
	return 0 ;
}
#endif

UINT32 mcu_ps_is_on ( void )
{
	return ( 1 == mcu_ps_info.mcu_ps_on );
}


#if (0 == CFG_USE_MCU_PS)
void peri_busy_count_add ( void )
{
}

void peri_busy_count_dec ( void )
{
}
void mcu_prevent_set ( UINT32 prevent )
{
}

void mcu_prevent_clear ( UINT32 prevent )
{
}

#endif

