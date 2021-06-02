#include "pwm_mutex.h"
#include "include.h"
#include "icu_pub.h"
#include "pwm_pub.h"
#include "uart_pub.h"
#include "drv_model_pub.h"


#if (CFG_SOC_NAME == SOC_BK7221U)
int bk_cw_pwm_reset_duty_cycle(uint8 channel_num_1, uint8 channel_num_2,
										uint32 duty_cycle_1, uint32 duty_cycle_2,
										uint32 end_value, uint32 dead_band_1)
{
	GLOBAL_INT_DECLARATION();
	UINT32 status;

	TPWM_PRT("bk_cw_pwm_reset_duty_cycle: %d %d %ld %ld %ld %ld\r\n",
	channel_num_1, channel_num_2, duty_cycle_1, duty_cycle_2, end_value, dead_band_1);

	if ((channel_num_1 >= PWM_COUNT) || (channel_num_2 >= PWM_COUNT)) {
		TPWM_FATAL("bk_cw_pwm_reset_duty_cycle param 1 error!\r\n");
		return -1;
	}

	if (end_value < duty_cycle_1 + duty_cycle_2 + dead_band_1*2) {
		TPWM_FATAL("bk_cw_pwm_reset_duty_cycle param 2 error!\r\n");
		return -2;
	}

	// set PWM 1/2 end value
	*(volatile unsigned long *) (0x00802A80 + 0x08 + (channel_num_1 * 3 * 4)) = end_value;
	*(volatile unsigned long *) (0x00802A80 + 0x08 + (channel_num_2 * 3 * 4)) = end_value;

	// set PWM 1/2 duty cycle
	*(volatile unsigned long *) (0x00802A80 + 0x0C + (channel_num_1 * 3 * 4)) = duty_cycle_1;
	*(volatile unsigned long *) (0x00802A80 + 0x0C + (channel_num_2 * 3 * 4)) = duty_cycle_2;


	//enable pwm1 and pwm1 int
	*(volatile unsigned long *) (0x00802040) |= (1 << 9);
	*(volatile unsigned long *) (0x00802A80) |= (1 << (channel_num_1 * 4));
	*(volatile unsigned long *) (0x00802A80) |= (1 << (channel_num_1 * 4+1));

	GLOBAL_INT_DISABLE();

	while (1) {
		//wait pwm int
		status = *(volatile unsigned long *) (0x0080204C);
		if (status & (1 << 9)) {
			break;
		}
	}

	// disable PWM 2 channel
	*(volatile unsigned long *) (0x00802A80) &= (~((1 << (channel_num_2 * 4))));

	// cache bk_cw_pwm_reset_duty_cycle function
	do {
		char *text, temp;
		int i;

		text = (char *)bk_cw_pwm_reset_duty_cycle;
		for (i = 0; i < 0x400; i++) {
			temp = text[i];
		}
		(void)temp;
	} while(0);

	// delay use timer0
	*(volatile unsigned long *) (0x00802008) &= ~(1 << 20);
	*(volatile unsigned long *) (0x00802A00) = (duty_cycle_1 + dead_band_1);
	*(volatile unsigned long *) (0x00802A0C) |= (1 << 0);
	*(volatile unsigned long *) (0x00802040) |= (1 << 8);
	while (1) {
		//wait timer int
		status = *(volatile unsigned long *) (0x0080204C);
		if (status & (1 << 8)) {
			status = *(volatile unsigned long *) (0x00802A0C);
			if (status & (1 << 7)) {
				break;
			}
		}
	}
	*(volatile unsigned long *) (0x00802A0C) &= ~(1 << 0);

	// enable PWM channel 1/2
	*(volatile unsigned long *) (0x00802A80) |= (1 << (channel_num_1 * 4))
												| (1 << (channel_num_2 * 4));
	GLOBAL_INT_RESTORE();
	return 0;
}
#else
int  bk_cw_pwm_reset_duty_cycle(uint8 channel_num_1, uint8 channel_num_2,
									uint32 duty_cycle_1, uint32 duty_cycle_2,
									uint32 end_value, uint32 dead_band_1)
{
	return -1;
}
#endif

//Eof

