#include "sys_rtos.h"
#include "fake_clock_pub.h"
#include "pwm_pub.h"
#include "bk_timer_pub.h"
#include "icu_pub.h"
#include "drv_model_pub.h"
#include "uart_pub.h"
#include "rtos_pub.h"

#if (CFG_SUPPORT_ALIOS)
#include "k_api.h"
#endif
#include "power_save_pub.h"
#include "mcu_ps_pub.h"
#include "BkDriverWdg.h"

static volatile UINT64 current_clock = 0;
static volatile UINT32 current_seconds = 0;
#if (CFG_OS_FREERTOS)
static UINT32 second_countdown = FCLK_SECOND;
#endif
static BK_HW_TIMER_INDEX fclk_id = BK_PWM_TIMER_ID0;
static CAL_TICK_T cal_tick_save;
UINT32 use_cal_net = 0;

extern void mcu_ps_increase_clr(void);
extern uint32_t preempt_delayed_schedule_get_flag(void);
extern void preempt_delayed_schedule_clear_flag(void);
static inline UINT64 fclk_freertos_get_tick64(void);

#if (CFG_OS_FREERTOS)

#define INT_WDG_FEED_PERIOD_TICK ((BK_MS_TO_TICKS(CFG_INT_WDG_PERIOD_MS)) >> 4)
#define TASK_WDG_PERIOD_TICK (BK_MS_TO_TICKS(CFG_TASK_WDG_PERIOD_MS))

#if (CFG_INT_WDG_ENABLED)

void int_watchdog_feed(void)
{
        static UINT64 s_last_int_wdg_feed = 0;
        UINT64 current_tick = fclk_freertos_get_tick64();

        if ( (current_tick - s_last_int_wdg_feed) >= INT_WDG_FEED_PERIOD_TICK) {
                bk_wdg_reload();
                s_last_int_wdg_feed = current_tick;
                //os_printf("feed interrupt watchdog\n");
        }
}
#endif

#if (CFG_TASK_WDG_ENABLED)

static UINT64 s_last_task_wdg_feed_tick = 0;
static UINT64 s_last_task_wdg_log_tick = 0;

void task_watchdog_feed(void)
{
        s_last_task_wdg_feed_tick = fclk_freertos_get_tick64();
}

static inline void task_watchdog_timeout_check(void)
{
	if (s_last_task_wdg_feed_tick) {
		UINT64 current_tick = fclk_freertos_get_tick64();
		if ( (current_tick - s_last_task_wdg_feed_tick) > TASK_WDG_PERIOD_TICK ) {
			if ((current_tick - s_last_task_wdg_log_tick) > TASK_WDG_PERIOD_TICK) {
				os_printf("warning: task watchdog tiggered\n");
				s_last_task_wdg_log_tick = current_tick;
			}
		}
	}
}
#endif
#endif //CFG_OS_FREERTOS

void fclk_hdl(UINT8 param)
{
#if CFG_USE_TICK_CAL
	if(!mcu_ps_need_pstick()) {
		return;
	}
#endif

#if (CFG_OS_FREERTOS)
        GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	current_clock ++;

    if (--second_countdown == 0)
    {
        current_seconds ++;
        second_countdown = FCLK_SECOND;
    }

#if (CFG_INT_WDG_ENABLED)
        int_watchdog_feed();
#endif

#if (CFG_TASK_WDG_ENABLED)
        task_watchdog_timeout_check();
#endif
    
    if((xTaskIncrementTick() != pdFALSE) 
			|| preempt_delayed_schedule_get_flag())
    {
    	preempt_delayed_schedule_clear_flag();
		
        /* Select a new task to run. */
        vTaskSwitchContext();
    }
    GLOBAL_INT_RESTORE();
#endif

#if (CFG_SUPPORT_ALIOS)
	krhino_tick_proc();
#endif
}

#if !(CFG_SUPPORT_RTT || CFG_SUPPORT_ALIOS)
static UINT32 fclk_freertos_update_tick(UINT32 tick)
{
    current_clock += tick;

    while(tick >= FCLK_SECOND)
    {
        current_seconds ++;
        tick -= FCLK_SECOND;
    }

    if(second_countdown <= tick)
    {
        current_seconds ++;
        second_countdown = FCLK_SECOND - (tick - second_countdown);
    }
    else
    {
        second_countdown -= tick;
    }

    return 0;
}
#endif

UINT32 fclk_update_tick(UINT32 tick)
{
#if (CFG_SUPPORT_RTT)
    rtt_update_tick(tick);
#elif (CFG_SUPPORT_ALIOS)
    krhino_update_sys_tick((UINT64)tick);
#else
    GLOBAL_INT_DECLARATION();

    if(tick == 0)
        return 0;

    GLOBAL_INT_DISABLE();
    mcu_ps_increase_clr();
    fclk_freertos_update_tick(tick);
    vTaskStepTick( tick );
    GLOBAL_INT_RESTORE();
    
#endif
    return 0;
}

static inline UINT32 fclk_freertos_get_tick32(void)
{
    return (UINT32)current_clock;
}

static inline UINT64 fclk_freertos_get_tick64(void)
{
	return current_clock;
}

UINT64 fclk_get_tick(void)
{
    UINT64 fclk;
#if (CFG_SUPPORT_RTT)
    fclk = (UINT64)rt_tick_get();
#elif (CFG_SUPPORT_ALIOS)
    fclk = krhino_sys_tick_get();
#else
    fclk = fclk_freertos_get_tick32();
#endif
    return fclk;
}

UINT32 fclk_get_second(void)
{
#if (CFG_SUPPORT_RTT)
    return (rt_tick_get()/FCLK_SECOND);
#elif (CFG_SUPPORT_ALIOS)
    return (krhino_sys_tick_get()/FCLK_SECOND);
#else
    return fclk_freertos_get_tick32()/FCLK_SECOND;
#endif
}

UINT32 fclk_from_sec_to_tick(UINT32 sec)
{
    return sec * FCLK_SECOND;
}

void fclk_reset_count(void)
{
    current_clock = 0;
    current_seconds = 0;
}

#if CFG_USE_TICK_CAL
UINT32 timer_cal_init(void)
{
    UINT32 fclk;

    fclk = BK_TICKS_TO_MS(fclk_get_tick());

    cal_tick_save.fclk_tick = fclk;
    cal_tick_save.tmp1 = 0;
    return 0;
}

extern int increase_tick;
UINT32 timer_cal_tick(void)
{
    UINT32 fclk, tmp2;
    UINT32 machw = 0;
    INT32 lost;
    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    
    fclk = BK_TICKS_TO_MS(fclk_get_tick());
    cal_tick_save.tmp1 += ONE_CAL_TIME;

    tmp2 = fclk;

    lost = (INT32)(cal_tick_save.tmp1  - (UINT32)tmp2);

    if((lost >= (2*FCLK_DURATION_MS)))
    {
        lost -= FCLK_DURATION_MS;
        fclk_update_tick(BK_MS_TO_TICKS(lost));
        increase_tick = 0;
    }
    else
    {
        if(lost <= (-(2*FCLK_DURATION_MS)))
        {
            if(lost < (-50000))
            {
                os_printf("m reset:%x %x\r\n", lost, machw);
            }
            increase_tick = lost + FCLK_DURATION_MS;
        }
    }
    
    mcu_ps_machw_init();
    GLOBAL_INT_RESTORE();
    return 0 ;
}


void cal_timer_hdl(UINT8 param)
{
    timer_cal_tick();
}

void cal_timer_set(void)
{
    timer_param_t param;
    UINT32 ret;
    UINT32 timer_channel;

    timer_cal_init();

    param.channel = CAL_TIMER_ID;
    param.div = 1;
    param.period = ONE_CAL_TIME;
    param.t_Int_Handler= cal_timer_hdl;

    ret = sddev_control(TIMER_DEV_NAME, CMD_TIMER_INIT_PARAM, &param);
    ASSERT(BK_TIMER_SUCCESS == ret);
    timer_channel = param.channel;
    ret = sddev_control(TIMER_DEV_NAME, CMD_TIMER_UNIT_ENABLE, &timer_channel);
    ASSERT(BK_TIMER_SUCCESS == ret);
}

void cal_timer_deset(void)
{
    UINT32 ret;
    UINT32 timer_channel;

    timer_channel = CAL_TIMER_ID;
    ret = sddev_control(TIMER_DEV_NAME, CMD_TIMER_UNIT_DISABLE, &timer_channel);
    ASSERT(BK_TIMER_SUCCESS == ret);
    timer_cal_init();
}

UINT32 bk_cal_init(UINT32 setting)
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    
    if(1 == setting)
    {
        cal_timer_deset();
        use_cal_net = 1;
        mcu_ps_machw_init();
#if (CFG_OS_FREERTOS)
        os_printf("decset:%d %d %d %d\r\n",use_cal_net, fclk_freertos_get_tick32(), fclk_get_second(), xTaskGetTickCount());
#endif
    }
    else
    {
        mcu_ps_machw_cal();
        cal_timer_set();
        use_cal_net = 0;
        mcu_ps_machw_reset();
#if (CFG_OS_FREERTOS)
        os_printf("cset:%d %d %d %d\r\n",use_cal_net, fclk_freertos_get_tick32(), fclk_get_second(), xTaskGetTickCount());
#endif
    }
    GLOBAL_INT_RESTORE();

	return 0;
}
#endif

UINT32 fclk_cal_endvalue(UINT32 mode)
{
    UINT32 value = 1;

    if(PWM_CLK_32K == mode)
    {
        /*32k clock*/
        value = FCLK_DURATION_MS * 32;
    }
    else if(PWM_CLK_26M == mode)
    {
        /*26m clock*/
        value = FCLK_DURATION_MS * 26000;
    }

    return value;
}

BK_HW_TIMER_INDEX fclk_get_tick_id(void)
{
    return fclk_id;
}

/*timer_id:BK_PWM_TIMER_ID0 or BK_TIMER_ID3*/
void fclk_timer_hw_init(BK_HW_TIMER_INDEX timer_id)
{
    UINT32 ret;

#if (CFG_SOC_NAME == SOC_BK7231)
    ASSERT(timer_id>= BK_PWM_TIMER_ID0);
#endif

    fclk_id = timer_id;

    if(fclk_id >= BK_PWM_TIMER_ID0)
    {   //pwm timer
        pwm_param_t param;

        /*init pwm*/
        param.channel         = (fclk_id - PWM0);
        param.cfg.bits.en     = PWM_ENABLE;
        param.cfg.bits.int_en = PWM_INT_EN;
        param.cfg.bits.mode   = PWM_TIMER_MODE;

#if(CFG_RUNNING_PLATFORM == FPGA_PLATFORM)  // FPGA:PWM0-2-32kCLK, pwm3-5-24CLK
        param.cfg.bits.clk    = PWM_CLK_32K;
#else
        param.cfg.bits.clk    = PWM_CLK_26M;
#endif

        param.p_Int_Handler   = fclk_hdl;
#if (CFG_SOC_NAME == SOC_BK7231N)
        param.duty_cycle1     = 0;
#else
        param.duty_cycle      = 0;
#endif
        param.end_value       = fclk_cal_endvalue((UINT32)param.cfg.bits.clk);

        sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &param);
        
    }
    else
    {   //timer
        timer_param_t param;
        param.channel = fclk_id;
        param.div = 1;
        param.period = FCLK_DURATION_MS;
        param.t_Int_Handler= fclk_hdl;

        ret = sddev_control(TIMER_DEV_NAME, CMD_TIMER_INIT_PARAM, &param);
        ASSERT(BK_TIMER_SUCCESS == ret);
        UINT32 timer_channel;
        timer_channel = param.channel;
        sddev_control(TIMER_DEV_NAME, CMD_TIMER_UNIT_ENABLE, &timer_channel);
        
    }
}

void fclk_init(void)
{
    #if (CFG_SOC_NAME == SOC_BK7231)
    fclk_timer_hw_init(BK_PWM_TIMER_ID0);
    #else
    fclk_timer_hw_init(BK_TIMER_ID3);
    #endif

    #if CFG_USE_TICK_CAL
    bk_cal_init(0);
    #endif
}

// eof

