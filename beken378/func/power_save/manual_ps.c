#include "manual_ps.h"
#include "manual_ps_pub.h"
#include "gpio_pub.h"
#include "power_save_pub.h"
#include "sys_ctrl_pub.h"
#include "target_util_pub.h"

#include "mcu_ps_pub.h"
#include "ps_debug_pub.h"
#include "icu_pub.h"
#include "sys_ctrl.h"
#include "gpio.h"
#include "fake_clock_pub.h"
#include "icu.h"
#if PS_SUPPORT_MANUAL_SLEEP

#include "BkDriverGpio.h"
#include "rwnx.h"

extern void phy_init_after_wakeup ( void );


void general_sleep_wakeup_with_peri ( UINT8 uart2_wk, UINT32 gpio_index_map, UINT32 gpio_edge_map )
{
	UINT32 reg, ret;
	UINT32 param = 0;
	UINT32 i;
	UINT32    gpio_stat_cfg[32] = {0};
	
	if ( power_save_ps_mode_get() != PS_NO_PS_MODE ) {
		os_printf ( "can't peri ps,ps in mode %d!\r\n", power_save_ps_mode_get() );
		return ;
	}
	
	for ( i = 0; i < GPIONUM; i++ ) {
		if ( gpio_index_map & ( 0x01UL << i ) ) {
			gpio_stat_cfg[i] = REG_READ ( 0x00802800 + i * 4 );
			
			if ( gpio_index_map & gpio_edge_map & ( 0x01UL << i ) ) {
				BkGpioEnableIRQ ( i, 0x3, NULL, NULL );
			}
			else {
				BkGpioEnableIRQ ( i, 0x2, NULL, NULL );
			}
			
			os_printf ( "set peri wkup gpio %d\r\n", i );
		}
	}
	
#if(PS_XTAL26M_EN == 1)
	param = LPO_SELECT_32K_DIV;
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_SET_LOW_PWR_CLK, &param );
#endif
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	ps_pwm0_disable();
	param = 0;
	
	if ( uart2_wk == 1 ) {
		param |= ( UART2_ARM_WAKEUP_EN_BIT );
		os_printf ( "set peri wkup uart2\r\n" );
	}
	
	if ( gpio_index_map )
		param |= ( GPIO_ARM_WAKEUP_EN_BIT );
		
	os_printf ( "enter peri ps\r\n" );
	sddev_control ( ICU_DEV_NAME, CMD_ARM_WAKEUP, &param );
	param = ( 0xfffff & ~PWD_UART2_CLK_BIT );
	wifi_mac_state_set_idle();
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_NORMAL_SLEEP, &param );
	delay ( 2 );
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_NORMAL_WAKEUP, 0 );
	wifi_mac_state_set_prev();
	
	for ( i = 0; i < GPIONUM; i++ ) {
		if ( gpio_index_map & ( 0x01UL << i ) ) {
			BkGpioDisableIRQ ( i );
			REG_WRITE ( ( 0x00802800 + i * 4 ), gpio_stat_cfg[i] );
		}
	}
	
	ps_pwm0_enable();
	GLOBAL_INT_RESTORE();
	os_printf ( "exit peri ps\r\n" );
}

void general_sleep_wakeup_with_timer ( UINT32 sleep_time )
{
	UINT32 reg;
	UINT32 param;
	UINT32 wakeup_timer;
	
	if ( power_save_ps_mode_get() != PS_NO_PS_MODE ) {
		os_printf ( "can't pwm ps,ps in mode %d!\r\n", power_save_ps_mode_get() );
		return ;
	}
	
	if ( sleep_time != 0xffffffff ) {
		os_printf ( "sleep with pwm,%d ms\r\n", sleep_time );
		param = LPO_SELECT_ROSC;
		sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_SET_LOW_PWR_CLK, &param );
		wakeup_timer = ( sleep_time * 32 );
		
		if ( wakeup_timer > 65535 )
			wakeup_timer = 65535;
		else if ( wakeup_timer < 160 )
			wakeup_timer = 160;
			
		delay ( 5 );
		ps_pwm0_suspend_tick ( wakeup_timer );
	}
	else {
		ps_pwm0_disable();
		os_printf ( "sleep forever\r\n" );
	}
	
	os_printf ( "enter pwm ps\r\n" );
	param = PWM_ARM_WAKEUP_EN_BIT;
	sddev_control ( ICU_DEV_NAME, CMD_ARM_WAKEUP, &param );
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	param = ( 0xfffff & ( ~PWD_PWM0_CLK_BIT ) & ( ~PWD_UART2_CLK_BIT ) );
	wifi_mac_state_set_idle();
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_NORMAL_SLEEP, &param );
	delay ( 5 );
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_NORMAL_WAKEUP, 0 );
	wifi_mac_state_set_prev();
	phy_init_after_wakeup();
	GLOBAL_INT_RESTORE();
	ps_pwm0_resume_tick();
	os_printf ( "exit pwm ps\r\n" );
}


void general_sleep_wakeup_with_gpio ( UINT32 gpio_index )
{
	UINT32 reg;
	UINT32 param;
	
	if ( power_save_ps_mode_get() != PS_NO_PS_MODE ) {
		os_printf ( "can't gpio ps,ps in mode %d!\r\n", power_save_ps_mode_get() );
		return ;
	}
	
	os_printf ( "enter gpio ps\r\n" );
	bk_gpio_config_input_pdwn ( gpio_index );
	BkGpioEnableIRQ ( gpio_index, 0x2, NULL, NULL );
	param = LPO_SELECT_ROSC;
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_SET_LOW_PWR_CLK, &param );
	param = ( GPIO_ARM_WAKEUP_EN_BIT );
	sddev_control ( ICU_DEV_NAME, CMD_ARM_WAKEUP, &param );
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	param = ( 0xfffff & ( ~PWD_UART2_CLK_BIT ) );
	wifi_mac_state_set_idle();
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_NORMAL_SLEEP, &param );
	delay ( 5 );
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_NORMAL_WAKEUP, 0 );
	wifi_mac_state_set_prev();
	phy_init_after_wakeup();
	GLOBAL_INT_RESTORE();
	os_printf ( "exit gpio ps\r\n" );
}

/*
 *  idle sleep
 *
*/
void idle_sleep_wakeup_with_timer ( UINT32 sleep_time )
{
	UINT32 reg, param;
	PS_DEEP_CTRL_PARAM deep_param;
	
	if ( power_save_ps_mode_get() != PS_NO_PS_MODE ) {
		os_printf ( "can't pwm ps,ps in mode %d!\r\n", power_save_ps_mode_get() );
		return ;
	}
	
	if ( sleep_time != 0xffffffff ) {
		os_printf ( "idle sleep with rtc,%d ms\r\n", sleep_time );
		deep_param.param = ( ( sleep_time * 102400 ) / 3125 ) ;
		
		if ( deep_param.param > 0xffff ) //only 16 bit
			deep_param.param = 0xffff;
		else if ( deep_param.param < 32 )
			deep_param.param = 32;
	}
	else {
		os_printf ( "idle sleep forever\r\n" );
		deep_param.param = 0xffffffff;
	}
	
	param = LPO_SELECT_ROSC;
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_SET_LOW_PWR_CLK, &param );
	deep_param.deep_wkway = PS_DEEP_WAKEUP_RTC;
	os_printf ( "enter rtc ps\r\n" );
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	ps_pwm0_disable();
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_RTOS_IDLE_SLEEP, &deep_param );
	delay ( 5 );
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_RTOS_IDLE_WAKEUP, 0 );
	phy_init_after_wakeup();
	ps_pwm0_enable();
	ke_evt_set ( KE_EVT_RESET_BIT );
	GLOBAL_INT_RESTORE();
	os_printf ( "exit rtc ps\r\n" );
}


/** @brief  Request idle sleep,and wakeup by gpio.
 */
void idle_sleep_wakeup_with_gpio ( UINT32 gpio_index_map, UINT32 gpio_edge_map )
{
	UINT32 reg, param;
	PS_DEEP_CTRL_PARAM deep_param;
	UINT32 i;
	
	//for(i = 0; i < 32; i ++)
	//	*((uint32_t *)(0x00802800 + i*4)) = 0x2c;
	
	for ( i = 0; i < GPIONUM; i++ ) {
		if ( gpio_index_map & ( 0x01UL << i ) ) {
			if ( gpio_index_map & gpio_edge_map & ( 0x01UL << i ) ) {
				param = GPIO_CFG_PARAM ( i, GMODE_INPUT_PULLUP );
				sddev_control ( GPIO_DEV_NAME, CMD_GPIO_CFG, &param );
			}
			else {
				param = GPIO_CFG_PARAM ( i, GMODE_INPUT_PULLDOWN );
				sddev_control ( GPIO_DEV_NAME, CMD_GPIO_CFG, &param );
			}
		}
	}
	
	os_printf ( "enter idle ps %x %x\r\n", gpio_index_map, gpio_edge_map );
	param = LPO_SELECT_ROSC;
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_SET_LOW_PWR_CLK, &param );
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	ps_pwm0_disable();
	uart_wait_tx_over();
	wifi_mac_state_set_idle();
	deep_param.deep_wkway = PS_DEEP_WAKEUP_GPIO;
	deep_param.param = gpio_index_map;
	deep_param.gpio_lv = gpio_edge_map;
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_RTOS_IDLE_SLEEP, &deep_param );
	delay ( 5 );
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_RTOS_IDLE_WAKEUP, 0 );
	wifi_mac_state_set_prev();
	phy_init_after_wakeup();
	ps_pwm0_enable();
	ke_evt_set ( KE_EVT_RESET_BIT );
	GLOBAL_INT_RESTORE();
	os_printf ( "exit gpio ps\r\n" );
}


/** @brief  Request power save,and wakeup some time later
 *  @mode   0:normal sleep,600uA,connect no influence
 *          1:idle sleep,20uA,but wifi need reconnect
 *  @param  sleep_time: Sleep time with milliseconds.
 *              if 0xffffffff not wakeup
 */
void bk_wlan_ps_wakeup_with_timer ( MANUAL_MODE mode, UINT32 sleep_time )
{
	if ( mode == MANUAL_MODE_NORMAL )
		general_sleep_wakeup_with_timer ( sleep_time );
	else if ( mode == MANUAL_MODE_IDLE )
		idle_sleep_wakeup_with_timer ( sleep_time );
}

/** @brief  Request power save,and wakeup by uart if uart2_wk=1,nd wakeup by gpio from bitmap of gpio_index_map.
 */
void bk_wlan_ps_wakeup_with_peri ( UINT8 uart2_wk, UINT32 gpio_index_map, UINT32 gpio_edge_map )
{
	general_sleep_wakeup_with_peri ( uart2_wk, gpio_index_map, gpio_edge_map );
}

/** @brief  Request power save,and wakeup by gpio
 *  @mode   0:normal sleep,600uA,connect no influence
 *          1:idle sleep,20uA,but wifi need reconnect
 *  @param  gpio_index_map:The gpio bitmap which set 1 enable wakeup deep sleep.
 *              gpio_index_map is hex and every bits is map to gpio0-gpio31.
 *          gpio_edge_map:The gpio edge bitmap for wakeup gpios,
 *              gpio_edge_map is hex and every bits is map to gpio0-gpio31.
 *              0:rising,1:falling.
 */
void bk_wlan_ps_wakeup_with_gpio ( MANUAL_MODE mode, UINT32 gpio_index_map, UINT32 gpio_edge_map )
{
	//if(mode == MANUAL_MODE_NORMAL)
	//    general_sleep_wakeup_with_peri(0, gpio_index_map, gpio_edge_map);
	//else if(mode == MANUAL_MODE_IDLE)
	idle_sleep_wakeup_with_gpio ( gpio_index_map, gpio_edge_map );
}


#endif

UINT32 use_unconditional_sleep = 0;
UINT32 bk_unconditional_sleep_mode_get ( void )
{
	return use_unconditional_sleep;
}
#if CFG_USE_FAKERTC_PS
UINT32 unconditional_sleep_inited = 0;
void unconditional_ps_init ( void )
{
	UINT32 reg;
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	
	if ( 0 == unconditional_sleep_inited ) {
		os_printf ( "%s\r\n", __FUNCTION__ );
		sctrl_mcu_init();
		unconditional_sleep_inited = 1;
	}
	
	GLOBAL_INT_RESTORE();
}


void unconditional_ps_exit ( void )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	
	if ( 1 == unconditional_sleep_inited ) {
		os_printf ( "%s\r\n", __FUNCTION__ );
		unconditional_sleep_inited = 0;
		sctrl_mcu_exit();
	}
	
	GLOBAL_INT_RESTORE();
}

void bk_enable_unconditional_sleep ( void )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	use_unconditional_sleep = 1;
	os_printf ( "%s\r\n", __FUNCTION__ );
	unconditional_ps_init();
	GLOBAL_INT_RESTORE();
}

void bk_disable_unconditional_sleep ( void )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	use_unconditional_sleep = 0;
	os_printf ( "%s\r\n", __FUNCTION__ );
	unconditional_ps_exit();
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_UNCONDITIONAL_RF_UP, 0 );
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_UNCONDITIONAL_MAC_UP, 0 );
	GLOBAL_INT_RESTORE();
}


int bk_unconditional_normal_sleep ( UINT32 sleep_ms, char flag )
{
	//flag:1:only gpio wake,0:and other wakeup
	UINT32  sleep_pwm_t, param, uart_miss_us = 0, miss_ticks = 0;
	UINT32 wkup_type, wastage = 0;
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	{
		do {
			if ( sleep_ms != 0xFFFFFFFF ) {
				if ( ps_timer2_get() <= 2U ) {
					break;
				}
				
				if ( sleep_ms <= 5U ) {
					break;
				}
				
				sleep_ms = sleep_ms - FCLK_DURATION_MS;
				sleep_pwm_t = ( sleep_ms * 32U );
				
				if ( ( int32 ) sleep_pwm_t <= 64U ) {
					break;
				}
				
#if (CFG_SOC_NAME == SOC_BK7231)
				
				if ( sleep_pwm_t > 65535U )
					sleep_pwm_t = 65535;
				else
#endif
					if ( sleep_pwm_t < 64U )
						sleep_pwm_t = 64;
			}
			
			sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_UNCONDITIONAL_RF_UP, 0 );
			sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_UNCONDITIONAL_MAC_UP, 0 );
			mcu_ps_machw_init();
			mcu_ps_machw_cal();
			//os_printf("s:%d %d %x\r\n",fclk_freertos_get_tick(),sleep_pwm_t,REG_READ(ICU_ARM_WAKEUP_EN));
			{
				if ( 0 == REG_READ ( ICU_GLOBAL_INT_EN ) ) {
					bk_printf ( "ICU_GLOBAL_INT_EN err\r\n" );
				}
				
				if ( ( 0x3 ) != ( REG_READ ( ICU_INTERRUPT_ENABLE ) & ( 0x3 ) ) ) {
					bk_printf ( "ICU_INTERRUPT_ENABLE err %x\r\n", REG_READ ( ICU_INTERRUPT_ENABLE ) );
				}
				
#if 1
				
				if ( fclk_get_tick_id() == BK_PWM_TIMER_ID0 ) {
					ps_pwm_disable();
				}
				
				if ( sleep_ms != 0xFFFFFFFF ) {
					ps_timer3_enable ( sleep_pwm_t );
				}
				else {
					os_printf ( "set tim 0xFFFFFFFF\r\n" );
					ps_timer3_disable();
					
					if ( flag == 1 ) {
						ps_timer02_disable();
					}
				}
				
#else
				ps_pwm0_suspend_tick ( sleep_pwm_t );
#endif
			}
#if 1
			
			if ( sleep_ms != 0xFFFFFFFF ) {
				if ( flag == 1 ) {
					param = ( 0xfffff );
				}
				else {
					param = ( 0xfffff  & ( ~PWD_TIMER_26M_CLK_BIT )
					          & ( ~PWD_TIMER_32K_CLK_BIT ) & ( ~PWD_UART2_CLK_BIT )
					          & ( ~PWD_UART1_CLK_BIT )
					        );
				}
			}
			else {
				if ( flag == 1 ) {
					param = ( 0xfffff );
				}
				else {
					param = ( 0xfffff & ( ~PWD_UART2_CLK_BIT )
					          & ( ~PWD_UART1_CLK_BIT )
					        );
				}
			}
			
#else
#endif
			
			if ( sctrl_unconditional_normal_sleep ( param ) != 0 ) {
				//os_printf("c\r\n");
				ps_timer3_disable();
				
				if ( fclk_get_tick_id() == BK_PWM_TIMER_ID0 ) {
					ps_pwm_enable();
				}
				
				if ( flag == 1 ) {
					ps_timer02_restore();
				}
				
				GLOBAL_INT_RESTORE();
				return 1;
			}
			
#if (CHIP_U_MCU_WKUP_USE_TIMER && (CFG_SOC_NAME != SOC_BK7231))
			ps_timer3_measure_prepare();
#endif
			wkup_type = sctrl_unconditional_normal_wakeup();
#if 1
			ps_timer3_disable();
			
			if ( fclk_get_tick_id() == BK_PWM_TIMER_ID0 ) {
				ps_pwm_enable();
			}
			
			if ( flag == 1 ) {
				ps_timer02_restore();
			}
			
			mcu_ps_machw_cal();
#endif
			//os_printf("t:%d is:%x\r\n",fclk_freertos_get_tick(),REG_READ(ICU_INT_STATUS));
		}
		while ( 0 );
	}
	GLOBAL_INT_RESTORE();
	return 0;
}
#endif

#if CFG_USE_DEEP_PS
void bk_enter_deep_sleep_mode ( PS_DEEP_CTRL_PARAM *deep_param )
{
	ASSERT ( deep_param != NULL );

	if ( ( deep_param->wake_up_way & PS_DEEP_WAKEUP_GPIO ) ) {
		if ( deep_param->gpio_index_map ) {
			os_printf ( "---enter deep sleep :wake up with gpio 0~31 ps: 0x%x 0x%x \r\n",
			            deep_param->gpio_index_map, deep_param->gpio_edge_map );
		}
		
		if ( deep_param->gpio_last_index_map ) {
			os_printf ( "---enter deep sleep :wake up with gpio32~39 ps: 0x%x 0x%x \r\n",
			            deep_param->gpio_last_index_map, deep_param->gpio_last_edge_map );
		}
	}
	
	if ( ( deep_param->wake_up_way & PS_DEEP_WAKEUP_RTC ) ) {
		os_printf ( "---enter deep sleep :wake up with " );
		
		if ( deep_param->lpo_32k_src == LPO_SELECT_32K_XTAL ) {
			os_printf ( " xtal 32k " );
		}
		else {
			os_printf ( "  rosc " );
		}
		
		os_printf ( " ps :%d s\r\n", deep_param->sleep_time );
		
		if ( deep_param->sleep_time > 0x1ffff ) {
			deep_param->sleep_time = 0x1ffff;
		}
		
		deep_param->sleep_time = 32768 * deep_param->sleep_time;
	}
	
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_RTOS_DEEP_SLEEP, deep_param );
	delay ( 5 );
	GLOBAL_INT_RESTORE();
}
#endif
