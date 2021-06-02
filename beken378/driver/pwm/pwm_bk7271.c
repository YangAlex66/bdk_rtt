#include "include.h"
#include "arm_arch.h"

#if (CFG_SOC_NAME == SOC_BK7271)
#include "pwm_bk7271.h"
#include "pwm_pub.h"
#include "drv_model_pub.h"
#include "intc_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"
#include "gpio_bk7271.h"
#include "icu_bk7271.h"
#include "uart_pub.h"

static const SDD_OPERATIONS pwm_op = {
	pwm_ctrl
};

void (*p_PWM_Int_Handler[PWM_CHANNEL_NUM])(UINT8);

static int volatile pwm_group = 0;
static int volatile current_pwm_channel = 0;

static void pwm_gpio_configuration(UINT8 chan, UINT8 enable)
{
	UINT32 ret;
	UINT32 param;

	switch (chan) {
	case PWM0:
		param = GFUNC_MODE_PWM0;
		break;

	case PWM1:
		param = GFUNC_MODE_PWM1;
		break;

	case PWM2:
		param = GFUNC_MODE_PWM2;
		break;

	case PWM3:
		param = GFUNC_MODE_PWM3;
		break;

	case PWM4:
		param = GFUNC_MODE_PWM4;
		break;

	case PWM5:
		param = GFUNC_MODE_PWM5;
		break;

	case PWM6:
		param = GFUNC_MODE_PWM6_9_MODE1;
		break;

	case PWM7:
		param = GFUNC_MODE_PWM6_9_MODE1;
		break;

	case PWM8:
		param = GFUNC_MODE_PWM6_9_MODE1;
		break;

	case PWM9:
		param = GFUNC_MODE_PWM6_9_MODE1;
		break;

	case PWM10:
		param = GFUNC_MODE_PWM10;
		break;

	case PWM11:
		param = GFUNC_MODE_PWM11;
		break;

	default:
		break;
	}

	if (enable)
		ret = sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
	else {
		param = GPIO_CFG_PARAM(param, GMODE_INPUT);
		ret = sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
	}
	ASSERT(GPIO_SUCCESS == ret);
}

static void pwm_icu_configuration(pwm_param_t *pwm_param, UINT8 enable)
{
	UINT32 ret;
	UINT32 param;

	/* set clock power down of icu module*/
	switch (pwm_group) {
	case 0:
		param = PWD_PWM0_CLK_BIT;
		break;

	case 1:
		param = PWD_PWM1_CLK_BIT;
		break;

	case 2:
		param= PWD_PWM2_CLK_BIT;
		break;

	case 3:
		param= PWD_PWM3_CLK_BIT;
		break;

	default:
		PWM_WARN("pwm_iconfig_fail\r\n");
		goto exit_icu;
	}

	if (enable) {
		ret = sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, (void *)&param);
		ASSERT(ICU_SUCCESS == ret);

		if (PWM_CLK_32K == pwm_param->cfg.bits.clk) {
			param = PCLK_POSI_PWMS;
			ret = sddev_control(ICU_DEV_NAME, CMD_CONF_PWM_LPOCLK, (void *)&param);
		} else if (PWM_CLK_26M == pwm_param->cfg.bits.clk) {
			param = PCLK_POSI_PWMS;
			ret = sddev_control(ICU_DEV_NAME, CMD_CONF_PCLK_26M, (void *)&param);

			param = pwm_param->channel;
			ret = sddev_control(ICU_DEV_NAME, CMD_CONF_PWM_PCLK, (void *)&param);
		} else {
			param = PCLK_POSI_PWMS;
			ret = sddev_control(ICU_DEV_NAME, CMD_CONF_PCLK_DCO, (void *)&param);

			param = pwm_param->channel;
			ret = sddev_control(ICU_DEV_NAME, CMD_CONF_PWM_PCLK, (void *)&param);

		}
		ASSERT(ICU_SUCCESS == ret);

	} else {
		ret = sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, (void *)&param);
		ASSERT(ICU_SUCCESS == ret);
	}

	if (PWM_INT_EN == pwm_param->cfg.bits.int_en) {
		param = IRQ_PWM_BIT;
		ret = sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, (void *)&param);

	}

exit_icu:

	return;
}

static bk_err_t get_pwm_current_channel(UINT8 channel)
{
	if (channel >= PWM_CHANNEL_NUM)
		return BK_ERR_PARAM;

	if (channel < CHANNEL_3)
	{
		pwm_group = group0;
		current_pwm_channel = channel ;
	} else if (channel < CHANNEL_6)
	{
		pwm_group = group1;
		current_pwm_channel = channel - CHANNEL_3;
	} else if (channel < CHANNEL_9)
	{
		pwm_group = group2;
		current_pwm_channel = channel - CHANNEL_6;
	} else
	{
		pwm_group = group3;
		current_pwm_channel = channel - CHANNEL_9;
	}

	return pwm_group;
}

static void init_pwm_param(pwm_param_t *pwm_param, UINT8 enable)
{
	UINT32 value;

	pwm_group =  get_pwm_current_channel(pwm_param->channel);

	if (pwm_param->cfg.bits.mode != PWM_TIMER_MODE)
		pwm_gpio_configuration(pwm_param->channel, enable);

	value = REG_READ(PWM_GROUNP_CTL(pwm_group));
	value = (value & (~(PWM_MODE_MASK(current_pwm_channel))))
			| ((pwm_param->cfg.bits.mode) << PWM_MODE_SET_BIT(current_pwm_channel));
	REG_WRITE(PWM_GROUNP_CTL(pwm_group), value);

	if (current_pwm_channel == 0) {
		value = (UINT32) pwm_param->end_value;
		REG_WRITE(PWM0_END_VALUE(pwm_group), value);
		value = (UINT32) pwm_param->duty_cycle;
		REG_WRITE(PWM0_DUTY_CYCLE(pwm_group), value);
	} else if (current_pwm_channel == 1) {
		value = (UINT32) pwm_param->end_value;
		REG_WRITE(PWM1_END_VALUE(pwm_group), value);
		value = (UINT32) pwm_param->duty_cycle;
		REG_WRITE(PWM1_DUTY_CYCLE(pwm_group), value);
	} else {
		value = (UINT32) pwm_param->end_value;
		REG_WRITE(PWM2_END_VALUE(pwm_group), value);
		value = (UINT32) pwm_param->duty_cycle;
		REG_WRITE(PWM2_DUTY_CYCLE(pwm_group), value);
	}

	//pwm int clear
	value = REG_READ(PWM_GROUNP_INTERRUPT_STATUS(pwm_group));
	value = value | PWM_INIT_CLEAR(current_pwm_channel);
	REG_WRITE(PWM_GROUNP_INTERRUPT_STATUS(pwm_group), value);

	p_PWM_Int_Handler[pwm_param->channel] = pwm_param->p_Int_Handler;

	//pwm int enable set
	value = REG_READ(PWM_GROUNP_CTL(pwm_group));
	value = (value & (~(PWM_INT_ENABLE(current_pwm_channel))))
			| (pwm_param->cfg.bits.int_en << PWM_INT_ENABLE_BIT(current_pwm_channel));
	REG_WRITE(PWM_GROUNP_CTL(pwm_group), value);

	PWM_LOGD(TAG,"group:%d, current_pwm:%d,mode: %x, REG_PWM_GROUP_CTRL= 0x%lx\r\n", pwm_group, current_pwm_channel, pwm_param->cfg.val, REG_READ(PWM_GROUNP_CTL(pwm_group)));

	pwm_icu_configuration(pwm_param, enable);
}

static void pwm_unit_enable(UINT8 ucChannel)
{

	UINT32 value;

	pwm_group =  get_pwm_current_channel(ucChannel);

	value = REG_READ(PWM_GROUNP_CTL(pwm_group));
	value |= PWM_ENABLE_SET(current_pwm_channel);
	REG_WRITE(PWM_GROUNP_CTL(pwm_group), value);
}

void pwm_unit_disable(UINT8 ucChannel)
{

	UINT32 value;

	pwm_group =  get_pwm_current_channel(ucChannel);

	value = REG_READ(PWM_GROUNP_CTL(pwm_group));
	value &= ~(PWM_ENABLE_SET(current_pwm_channel));
	REG_WRITE(PWM_GROUNP_CTL(pwm_group), value);

}
static void pwm_param_clear(UINT8 ucChannel)
{
	pwm_group =  get_pwm_current_channel(ucChannel);

	if (current_pwm_channel == 0)
		REG_WRITE(PWM0_END_VALUE(pwm_group), 0);
	else if (current_pwm_channel == 1)
		REG_WRITE(PWM1_END_VALUE(pwm_group), 0);
	else
		REG_WRITE(PWM2_END_VALUE(pwm_group), 0);
}

static UINT32 pwm_capture_value_get(UINT8 ucChannel)
{
	//return REG_READ(REG_APB_BK_PWMn_CAP_ADDR(ucChannel));
	//TODO need to implemented
	return -1;
}

static void pwm_set_duty_cycle(UINT8 ucChannel, UINT32 u32DutyCycle)
{

	UINT32 value;

	pwm_group =  get_pwm_current_channel(ucChannel);


	if (current_pwm_channel == 0) {
		//enable update bit
		value = REG_READ(PWM0_UPDATE(pwm_group));
		value |= PWM_UPDATE_ENABLE;
		REG_WRITE(PWM0_UPDATE(pwm_group), value);

		REG_WRITE(PWM0_DUTY_CYCLE(pwm_group), u32DutyCycle);
	} else if (current_pwm_channel == 1) {
		//enable update bit
		value = REG_READ(PWM1_UPDATE(pwm_group));
		value |= PWM_UPDATE_ENABLE;
		REG_WRITE(PWM1_UPDATE(pwm_group), value);

		REG_WRITE(PWM1_DUTY_CYCLE(pwm_group), u32DutyCycle);
	} else {
		//enable update bit
		value = REG_READ(PWM2_UPDATE(pwm_group));
		value |= PWM_UPDATE_ENABLE;
		REG_WRITE(PWM2_UPDATE(pwm_group), value);

		REG_WRITE(PWM2_DUTY_CYCLE(pwm_group), u32DutyCycle);
	}
}

static void pwm_set_end_value(UINT8 ucChannel, UINT32 u32EndValue)
{
	UINT32 value;

	pwm_group =  get_pwm_current_channel(ucChannel);

	if (current_pwm_channel == 0) {
		//enable update bit
		value = REG_READ(PWM0_UPDATE(pwm_group));
		value |= PWM_UPDATE_ENABLE;
		REG_WRITE(PWM0_UPDATE(pwm_group), value);

		REG_WRITE(PWM0_END_VALUE(pwm_group), u32EndValue);
	} else if (current_pwm_channel == 1) {
		//enable update bit
		value = REG_READ(PWM1_UPDATE(pwm_group));
		value |= PWM_UPDATE_ENABLE;
		REG_WRITE(PWM1_UPDATE(pwm_group), value);

		REG_WRITE(PWM1_END_VALUE(pwm_group), u32EndValue);
	} else {
		//enable update bit
		value = REG_READ(PWM2_UPDATE(pwm_group));
		value |= PWM_UPDATE_ENABLE;
		REG_WRITE(PWM2_UPDATE(pwm_group), value);

		REG_WRITE(PWM2_END_VALUE(pwm_group), u32EndValue);
	}

}

UINT32 pwm_get_counter(UINT8 ucChannel)
{
	UINT32 value;

	pwm_group =  get_pwm_current_channel(ucChannel);

	//wait read bit is 0
	value = REG_READ(PWM_GROUNP_READ(pwm_group));
	if (value & PWM_READ_CNT_ENABLE_MASK)
	{
		PWM_LOGE(TAG,"pwm read error\r\n");
		return 0;
	}

	value = PWM_READ_CNT_ENABLE | PWM_READ_CNT_CHANNEL(current_pwm_channel);
	REG_WRITE(PWM_GROUNP_READ(pwm_group), value);

	value = PWM_GROUNP_READ_CNT(pwm_group);

	return value;
}


void pwm_update_param(pwm_param_t *pwm_param)
{
	UINT32 value;

	pwm_group =  get_pwm_current_channel(pwm_param->channel);

	if (current_pwm_channel == 0)
	{
		//enable update bit
		value = REG_READ(PWM0_UPDATE(pwm_group));
		value |= PWM_UPDATE_ENABLE;
		REG_WRITE(PWM0_UPDATE(pwm_group), value);

		REG_WRITE(PWM0_DUTY_CYCLE(pwm_group), pwm_param->duty_cycle);
		REG_WRITE(PWM0_END_VALUE(pwm_group), pwm_param->end_value);
	} else if (current_pwm_channel == 1)
	{
		//enable update bit
		value = REG_READ(PWM1_UPDATE(pwm_group));
		value |= PWM_UPDATE_ENABLE;
		REG_WRITE(PWM1_UPDATE(pwm_group), value);

		REG_WRITE(PWM1_DUTY_CYCLE(pwm_group), pwm_param->duty_cycle);
		REG_WRITE(PWM1_END_VALUE(pwm_group), pwm_param->end_value);
	} else
	{
		//enable update bit
		value = REG_READ(PWM2_UPDATE(pwm_group));
		value |= PWM_UPDATE_ENABLE;

		REG_WRITE(PWM2_UPDATE(pwm_group), value);
		REG_WRITE(PWM2_DUTY_CYCLE(pwm_group), pwm_param->duty_cycle);
		REG_WRITE(PWM2_END_VALUE(pwm_group), pwm_param->end_value);
	}
}

bk_err_t pwm_update_param_enable(UINT8 ucChannel)
{
	UINT32 value;

	pwm_group =  get_pwm_current_channel(ucChannel);

	if (current_pwm_channel == 0)
	{
		//enable update bit
		value = REG_READ(PWM0_UPDATE(pwm_group));
		value |= PWM_UPDATE_ENABLE;

		REG_WRITE(PWM0_UPDATE(pwm_group), value);
	} else if (current_pwm_channel == 1)
	{
		//enable update bit
		value = REG_READ(PWM1_UPDATE(pwm_group));
		value |= PWM_UPDATE_ENABLE;

		REG_WRITE(PWM1_UPDATE(pwm_group), value);
	} else
	{
		//enable update bit
		value = REG_READ(PWM2_UPDATE(pwm_group));
		value |= PWM_UPDATE_ENABLE;

		REG_WRITE(PWM2_UPDATE(pwm_group), value);
	}

	return BK_OK;
}

static void pwm_int_handler_clear(UINT8 ucChannel)
{
	p_PWM_Int_Handler[ucChannel] = NULL;
}

void pwm_init(void)
{
	REG_WRITE(PWM_GROUNP_CTL(0), 0x0);
	REG_WRITE(PWM_GROUNP_CTL(1), 0x0);
	REG_WRITE(PWM_GROUNP_CTL(2), 0x0);
	REG_WRITE(PWM_GROUNP_CTL(3), 0x0);

	intc_service_register(IRQ_PWM, PRI_IRQ_PWM, pwm_isr);
	intc_service_register(IRQ_PWM2, PRI_IRQ_PWM, pwm_isr);

	sddev_register_dev(PWM_DEV_NAME, (SDD_OPERATIONS*)&pwm_op);
}

void pwm_exit(void)
{
	sddev_unregister_dev(PWM_DEV_NAME);
}

UINT32 pwm_ctrl(UINT32 cmd, void *param)
{
	UINT32 ret = PWM_SUCCESS;
	UINT32 ucChannel;
	pwm_param_t *p_param;
	pwm_capture_t *p_capture;

	switch (cmd) {
	case CMD_PWM_UNIT_ENABLE:
		ucChannel = (*(UINT32 *)param);
		if (ucChannel > 11) {
			ret = PWM_FAILURE;
			break;
		}
		pwm_unit_enable(ucChannel);
		break;
	case CMD_PWM_UNIT_DISABLE:
		ucChannel = (*(UINT32 *)param);
		if (ucChannel > 11) {
			ret = PWM_FAILURE;
			break;
		}
		pwm_unit_disable(ucChannel);
		pwm_param_clear(ucChannel);
		break;
	case CMD_PWM_IR_ENABLE:

		break;
	case CMD_PWM_IR_DISABLE:

		break;
	case CMD_PWM_IR_CLEAR:
		ucChannel = (*(UINT32 *)param);
		if (ucChannel > PWM_CHANNEL_NUM) {
			ret = PWM_FAILURE;
			break;
		}
		pwm_int_handler_clear(ucChannel);
		break;
	case CMD_PWM_INIT_PARAM:
		p_param = (pwm_param_t *)param;
		init_pwm_param(p_param, 1);
		break;
	case CMD_PWM_SET_DUTY_CYCLE:
		p_param = (pwm_param_t *)param;
		if (p_param->channel >= PWM_CHANNEL_NUM) {
			ret = PWM_FAILURE;
			break;
		}
		pwm_set_duty_cycle(p_param->channel, p_param->duty_cycle);
		break;
	case CMD_PWM_SET_END_VALUE:
		p_param = (pwm_param_t *)param;
		if (p_param->channel >= PWM_CHANNEL_NUM) {
			ret = PWM_FAILURE;
			break;
		}
		pwm_set_end_value(p_param->channel, p_param->end_value);
		break;
	case CMD_PWM_CAP_GET:
		p_capture = (pwm_capture_t *)param;
		if (p_capture->ucChannel >= PWM_CHANNEL_NUM) {
			ret = PWM_FAILURE;
			break;
		}
		p_capture->value = pwm_capture_value_get(p_capture->ucChannel);
		break;
	case CMD_PWM_UPDATE_PARAM:
		p_param = (pwm_param_t *)param;
		pwm_update_param(p_param);
		break;
	case CMD_PWM_DEINIT_PARAM:
		p_param = (pwm_param_t *)param;
		init_pwm_param(p_param, 0);
		break;
	default:
		ret = PWM_FAILURE;
		break;
	}

	return ret;
}

void pwm_isr(void)
{
	int i;
	UINT32 status;

	status = REG_READ(PWM_GROUNP_INTERRUPT_STATUS(pwm_group));

	for (i = 0; i < PWM_CHANNEL_NUM; i++) {
		if (status & (1 << i)) {
			if (p_PWM_Int_Handler[i]) {
				p_PWM_Int_Handler[i]((UINT8)i);
				do {
					REG_WRITE(PWM_GROUNP_INTERRUPT_STATUS(pwm_group), (1 << i));
				} while (REG_READ(PWM_GROUNP_INTERRUPT_STATUS(pwm_group)) & (1 << i));
			}
		}
	}

	do {
		REG_WRITE(PWM_GROUNP_INTERRUPT_STATUS(pwm_group), status);
	} while (REG_READ(PWM_GROUNP_INTERRUPT_STATUS(pwm_group)) & status & 0x3F);
}

#endif
