#ifndef _PWM_H_
#define _PWM_H_

#if (CFG_SOC_NAME == SOC_BK7271)
#include "bk_err.h"
#include "bk_log.h"

#define TAG						"pwm"
#define PWM_LOGE				BK_LOGE
#define PWM_LOGI				BK_LOGI
#define PWM_LOGW				BK_LOGW
#define PWM_LOGD				BK_LOGD

#define PWM_HIGH_LEVEL			1
#define PWM_LOW_LEVEL			0

#define PWM_DEBUG
#ifdef PWM_DEBUG
#define PWM_PRT      os_printf
#define PWM_WARN     warning_prf
#define PWM_FATAL    fatal_prf
#else
#define PWM_PRT      null_prf
#define PWM_WARN     null_prf
#define PWM_FATAL    null_prf
#endif

#define PWM_CHANNEL_NUM								 12
#define PWM_GROUNP_BASE(x)                           (0x00802800 +0x40 *x)

#define PWM_GROUNP_CTL(x)                            (PWM_GROUNP_BASE(x) + 0 * 4)


#define PWM_ENABLE_SET(x)                            (1 << (x*5))
#define PWM_ENABLE_BIT(x)                            (x*5)

#define PWM_INT_ENABLE(x)                            (1 << ( x*5+1))
#define PWM_INT_ENABLE_BIT(x)                        (x*5 +1)

#define PWM_MODE_MASK(x)                             (7 << ( x*5+2))
#define PWM_MODE_SET_BIT(x)                          ( x*5+2)


#define PWM_CLK_DIV									 (16)

#define PWM_GROUNP_INTERRUPT_STATUS(x)               (PWM_GROUNP_BASE(x)+ 1 * 4)

#define PWM_INIT_MASK(x)                             (0x01UL << x)

#define PWM_INIT_CLEAR(x)                            (0x01UL << x)



#define PWM0_END_VALUE(x)                            (PWM_GROUNP_BASE(x) + 2 * 4)
#define PWM0_DUTY_CYCLE(x)                           (PWM_GROUNP_BASE(x) + 3 * 4)
#define PWM0_CAP_VALUE(x)                            (PWM_GROUNP_BASE(x)  + 4 * 4)

#define PWM1_END_VALUE(x)                            (PWM_GROUNP_BASE(x)  + 5 * 4)
#define PWM1_DUTY_CYCLE(x)                           (PWM_GROUNP_BASE(x)  + 6 * 4)
#define PWM1_CAP_VALUE(x)                            (PWM_GROUNP_BASE(x)  + 7 * 4)

#define PWM2_END_VALUE(x)                            (PWM_GROUNP_BASE(x)  + 8* 4)
#define PWM2_DUTY_CYCLE(x)                           (PWM_GROUNP_BASE(x)  + 9 * 4)
#define PWM2_CAP_VALUE(x)                            (PWM_GROUNP_BASE(x)  + 0xa * 4)

#define PWM0_UPDATE(x)							     (PWM_GROUNP_BASE(x)  + 0xb * 4)
#define PWM1_UPDATE(x)							     (PWM_GROUNP_BASE(x)  + 0xc * 4)
#define PWM2_UPDATE(x)								 (PWM_GROUNP_BASE(x)  + 0xd * 4)

#define PWM_UPDATE_ENABLE							 1<<0

#define PWM_GROUNP_READ(x)							 (PWM_GROUNP_BASE(x)  + 0xe * 4)

#define PWM_READ_CNT_ENABLE							(1 << 0)
#define PWM_READ_CNT_ENABLE_MASK						(1 << 0)

#define PWM_READ_CNT_CHANNEL(x)							(x << 1)
#define PWM_READ_CNT_CHANNEL_MASK						(3 << 1)

#define PWM_GROUNP_READ_CNT(x)						 (PWM_GROUNP_BASE(x)  + 0xf * 4)

#define CHANNEL_NO									12
#define CHANNEL_3									3
#define CHANNEL_6									6
#define CHANNEL_9									9

enum
{
	group0	 = 0,
	group1,
	group2,
	group3
};

/*******************************************************************************
* Function Declarations
*******************************************************************************/
UINT32 pwm_ctrl(UINT32 cmd, void *param);
bk_err_t pwm_update_param_enable(UINT8 ucChannel);

#endif
#endif //_PWM_H_

