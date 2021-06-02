#ifndef _PWM_MUTEX_H_
#define _PWM_MUTEX_H_


#include "include.h"

#define TPWM_DEBUG
#undef TPWM_DEBUG

#ifdef TPWM_DEBUG
#define TPWM_PRT      os_printf
#define TPWM_WARN     warning_prf
#define TPWM_FATAL    fatal_prf
#else
#define TPWM_PRT      null_prf
#define TPWM_WARN     null_prf
#define TPWM_FATAL    null_prf
#endif

extern int bk_cw_pwm_reset_duty_cycle(uint8 channel_num_1, uint8 channel_num_2,
                                           uint32 duty_cycle_1, uint32 duty_cycle_2,
                                           uint32 end_value, uint32 dead_band_1);

#endif

///Eof

