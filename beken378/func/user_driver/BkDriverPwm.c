/**
 ******************************************************************************
 * @file    BkDriverPwm.h
 * @brief   This file provides all the headers of PWM operation functions.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2017 BEKEN Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */
#include "include.h"
#include "rtos_pub.h"
#include "BkDriverPwm.h"
#include "drv_model_pub.h"
#include "error.h"
#include "pwm_pub.h"


#if (CFG_SOC_NAME != SOC_BK7231N) && (CFG_SOC_NAME != SOC_BK7236)
#include "pwm_bk7271.h"

OSStatus bk_pwm_initialize(bk_pwm_t pwm, uint32_t frequency, uint32_t duty_cycle)
{
	UINT32 ret;
	pwm_param_t param;

	/*init pwm*/
	param.channel			= (uint8_t)pwm;
	param.cfg.bits.en		= PWM_INT_DIS;
	param.cfg.bits.int_en	= PWM_INT_DIS;
	param.cfg.bits.mode		= PWM_PWM_MODE;
	param.cfg.bits.clk		= PWM_CLK_26M;
	param.p_Int_Handler		= 0;
	param.duty_cycle		= duty_cycle;
	param.end_value			= frequency;

	ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &param);
	ASSERT(PWM_SUCCESS == ret);

	return ret;

}

OSStatus bk_pwm_capture_initialize(bk_pwm_t pwm, uint8_t cap_mode)
{

	uint8_t ret;
	pwm_param_t param;

	/*init pwm*/
	param.channel			= (uint8_t)pwm;
	param.cfg.bits.en		= PWM_INT_EN;
	param.cfg.bits.int_en	= PWM_INT_EN;
	param.cfg.bits.clk		= PWM_CLK_26M;
	param.p_Int_Handler		= 0;
	param.duty_cycle		= 0;
	param.end_value			= 0;

	if (cap_mode == 0x01)
		param.cfg.bits.mode   = PWM_CAP_POS_MODE;
	else if (cap_mode == 0x02)
		param.cfg.bits.mode   = PWM_CAP_NEG_MODE;
	else
		param.cfg.bits.mode   = PWM_PWM_MODE;

	ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &param);
	ASSERT(PWM_SUCCESS == ret);

	return kNoErr;

}


#if (CFG_SOC_NAME == SOC_BK7271)
bk_err_t bk_pwm_update_param(bk_pwm_t pwm, uint32_t frequency, uint32_t duty_cycle)
{
	pwm_param_t param;
	UINT32 ret;

	param.channel         = (uint8_t)pwm;
	param.duty_cycle	  = duty_cycle;
	param.end_value       = frequency;
	ret = sddev_control(PWM_DEV_NAME, CMD_PWM_UPDATE_PARAM, &param);

	pwm_update_param_enable(pwm);

	return ret;
}
#endif

#else

#include "pwm_bk7231n.h"
OSStatus bk_pwm_initlevl_set_low(bk_pwm_t pwm);
OSStatus bk_pwm_initlevl_set_high(bk_pwm_t pwm);


OSStatus bk_pwm_initialize(bk_pwm_t pwm, uint32_t frequency, uint32_t duty_cycle1,uint32_t duty_cycle2,uint32_t duty_cycle3)
{
    UINT32 ret;
    pwm_param_t param;

    /*init pwm*/
    param.channel         = (uint8_t)pwm;
    param.cfg.bits.int_en = PWM_INT_DIS;
    param.cfg.bits.mode   = PWM_PWM_MODE;
    param.cfg.bits.clk    = PWM_CLK_26M;
    param.p_Int_Handler   = 0;
    param.duty_cycle1     = duty_cycle1;
	param.duty_cycle2     = duty_cycle2;
	param.duty_cycle3     = duty_cycle3;
    param.end_value       = frequency;  

	bk_printf("bk pwm initial:mode = %x\r\n",param.cfg.val);
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &param);
    ASSERT(PWM_SUCCESS == ret);
	
	if(duty_cycle1)
	{
		bk_pwm_initlevl_set_high(param.channel);
	}
	else
	{
		bk_pwm_initlevl_set_low(param.channel);
	}

    return kNoErr;
}


static void pwm_cap_int_callback(UINT8 channel)
{
	PWM_LOGI(TAG, "pwm %d cap value:%d \r\n", channel, bk_pwm_get_capvalue(channel));
}

OSStatus bk_pwm_capture_initialize(bk_pwm_t pwm, UINT8 cap_mode)
{
	UINT32 ret;
	pwm_param_t param;

	/*init pwm*/
	param.channel			= (uint8_t)pwm;
	param.cfg.bits.int_en	= PWM_INT_EN;
	param.cfg.bits.clk		= PWM_CLK_26M;
	param.p_Int_Handler		= pwm_cap_int_callback;
	param.duty_cycle1		= 0;
	param.duty_cycle2		= 0;
	param.duty_cycle3		= 0;
	param.end_value			= 0;
	param.cfg.bits.mode		= cap_mode;

	ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &param);
	ASSERT(PWM_SUCCESS == ret);

	return ret;
}
#endif

OSStatus bk_pwm_start(bk_pwm_t pwm)
{
	UINT32 ret;
	UINT32 param;

	param = pwm;

	ret = sddev_control(PWM_DEV_NAME, CMD_PWM_UNIT_ENABLE, &param);
	ASSERT(PWM_SUCCESS == ret);

	return ret;
}

OSStatus bk_pwm_stop(bk_pwm_t pwm)

{
	UINT32 ret;
	UINT32 param;

	param = pwm;
	ret = sddev_control(PWM_DEV_NAME, CMD_PWM_UNIT_DISABLE, &param);
	ASSERT(PWM_SUCCESS == ret);

	return ret;
}

UINT32 bk_pwm_get_capvalue(bk_pwm_t pwm)
{
	UINT32 ret;

	pwm_capture_t pwm_cap;

	pwm_cap.ucChannel = pwm;

	ret = sddev_control(PWM_DEV_NAME, CMD_PWM_CAP_GET, (VOID *)&pwm_cap);
	ASSERT(PWM_SUCCESS == ret);

	return pwm_cap.value;
}

#if (CFG_SOC_NAME == SOC_BK7231N) || (CFG_SOC_NAME == SOC_BK7236)
static UINT8 group_flag = 0;
static UINT8 pwm1_set_high_flag = 0;
static UINT8 pwm2_set_low_flag = 0;

OSStatus bk_pwm_initlevl_set_low(bk_pwm_t pwm)
{
	UINT32 ret;
	UINT32 param;

	param = pwm;
	ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_LEVL_SET_LOW, &param);
	ASSERT(PWM_SUCCESS == ret);

	return ret;
}

OSStatus bk_pwm_initlevl_set_high(bk_pwm_t pwm)
{
	UINT32 ret;
	UINT32 param;

	param = pwm;
	ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_LEVL_SET_HIGH, &param);
	ASSERT(PWM_SUCCESS == ret);

	return ret;
}


bk_err_t bk_pwm_update_param(bk_pwm_t pwm, uint32_t frequency, uint32_t duty_cycle)
{
	pwm_param_t param;
	UINT32 ret;

	param.channel         = (uint8_t)pwm;
	param.duty_cycle1	  = duty_cycle;
	param.duty_cycle2     = 0;
	param.duty_cycle3     = 0;
	param.end_value       = frequency;

	if (!duty_cycle)
		bk_pwm_initlevl_set_low(pwm);
	else
		bk_pwm_initlevl_set_high(pwm);

	ret = sddev_control(PWM_DEV_NAME, CMD_PWM_UPDATE_PARAM, &param);
	ASSERT(PWM_SUCCESS == ret);

	ret = pwm_update_param_enable(pwm);

	return ret;
}

void bk_pwm_cw_initialize(bk_pwm_t pwm1, bk_pwm_t pwm2, uint32_t frequency, uint32_t duty_cycle1, uint32_t duty_cycle2,  uint32_t dead_band)
{
	pwm_param_t param;

	group_flag = pwm_check_group(pwm1, pwm2);

	/*init pwm1*/
	param.channel = pwm1 ;
	param.cfg.bits.int_en = PWM_INT_DIS;
	param.cfg.bits.mode   = PWM_PWM_MODE;
	param.cfg.bits.clk    = PWM_CLK_26M;
	param.p_Int_Handler   = 0;
	param.duty_cycle1     = frequency - duty_cycle1 - dead_band;
	param.duty_cycle2     = frequency  - dead_band;
	param.duty_cycle3     = 0;
    param.end_value       = frequency;  

	init_pwm_param(&param, 1);
	pwm_init_levl_set_low(pwm1);
	
    /*init pwm2*/

	param.channel = pwm2 ;
	param.cfg.bits.int_en = PWM_INT_DIS;
	param.cfg.bits.mode   = PWM_PWM_MODE;
	param.cfg.bits.clk    = PWM_CLK_26M;
	param.p_Int_Handler   = 0;
	param.duty_cycle1     = duty_cycle2;
	param.duty_cycle2     = frequency;
	param.duty_cycle3     = 0;
	param.end_value       = frequency;

	init_pwm_param(&param, 1);

	if (duty_cycle2 == 0) {
		pwm2_set_low_flag = 1;
		bk_pwm_initlevl_set_low(pwm2);

	} else {
		pwm2_set_low_flag = 0;
		pwm_init_levl_set_high(pwm2);

	}

	if (duty_cycle1 == frequency) {
		pwm1_set_high_flag = 1;
		bk_pwm_initlevl_set_high(pwm1);

	} else {
		pwm1_set_high_flag = 0;
		pwm_init_levl_set_low(pwm1);
	}
}

void bk_pwm_cw_start(bk_pwm_t pwm1, bk_pwm_t pwm2)
{

	group_flag=pwm_check_group(pwm1, pwm2 );
	
	if (group_flag == 1) {
		//grounp enable
		pwm_group_mode_enable(pwm1);
	} else {
		pwm_unit_enable(pwm1);
		pwm_unit_enable(pwm2);
	}
}

void bk_pwm_cw_stop(bk_pwm_t pwm1, bk_pwm_t pwm2)
{
	group_flag=pwm_check_group(pwm1, pwm2 );


	if (group_flag == 1)

	{
		//grounp enable
		pwm_group_mode_disable(pwm1);
	} else
	{
		pwm_unit_disable(pwm1);
		pwm_unit_disable(pwm2);
	}
}


OSStatus bk_pwm_cw_update_param(bk_pwm_t pwm1, bk_pwm_t pwm2, uint32_t frequency, uint32_t duty_cycle1, uint32_t duty_cycle2,  uint32_t dead_band)
{
	pwm_param_t param;

	if (frequency < (duty_cycle1 + duty_cycle2 + 2 * dead_band))
	{
		bk_printf("pwm param set error:freq:%x ,cycle1:%x,cycle2:%x,dead_band:%x\r\n", frequency, duty_cycle1, duty_cycle2, dead_band);
		return BK_FAIL;
	}

	if (duty_cycle2 <= 0)
	{
		duty_cycle2 = 0;
		dead_band = 0;
		//bk_printf("pwm error dead_band > cycle2, cycle2:%x,dead_band:%x\r\n",duty_cycle2, dead_band);
	}

	if (duty_cycle1 <= 0)
	{
		duty_cycle1 = 0;
		dead_band = 0;
		//bk_printf("pwm error dead_band > cycle1, cycle1:%x,dead_band:%x\r\n",duty_cycle1, dead_band);
	}

	//bk_printf("pwm channel :%d :freq:%x ,cycle1:%x,cycle2:%x,dead_band:%x\r\n",pwm1,frequency, duty_cycle1, duty_cycle2, dead_band);

	group_flag = pwm_check_group(pwm1, pwm2);

	/*init pwm1*/
	param.channel		= pwm1 ;
	param.duty_cycle1	= frequency - duty_cycle1 - dead_band;
	param.duty_cycle2	= frequency  - dead_band;
	param.duty_cycle3	= 0;
	param.end_value		= frequency;

	sddev_control(PWM_DEV_NAME, CMD_PWM_UPDATE_PARAM, &param);

	/*init pwm2*/
	param.channel		= pwm2 ;
	param.duty_cycle1	= duty_cycle2;
	param.duty_cycle2	= frequency;
	param.duty_cycle3	= 0;
	param.end_value		= frequency;

	sddev_control(PWM_DEV_NAME, CMD_PWM_UPDATE_PARAM, &param);

	if (pwm1_set_high_flag == 1)
	{
		if (duty_cycle1 == frequency)
			pwm1_set_high_flag = 1;
		else {
			pwm1_set_high_flag = 0;
			bk_pwm_initlevl_set_low(pwm1);
		}
	}
	if (pwm2_set_low_flag == 1)
	{
		if (duty_cycle2 == 0)
			pwm2_set_low_flag = 1;
		else {
			pwm2_set_low_flag = 0;
			bk_pwm_initlevl_set_high(pwm2);
		}
	}

	if (group_flag)
		pwm_group_update_param_enable(pwm1, pwm2);
	else
	{
		pwm_update_param_enable(pwm1);
		pwm_update_param_enable(pwm2);
	}

	if (duty_cycle2 == 0)
	{
		if (pwm_init_levl_get(pwm2) == 1) {
			pwm2_set_low_flag = 1;
			bk_pwm_initlevl_set_low(pwm2);
		}
	}

	if (duty_cycle1 == frequency)
	{
		if (pwm_init_levl_get(pwm1) == 0) {
			pwm1_set_high_flag = 1;
			bk_pwm_initlevl_set_high(pwm1);
		}
	}

	return BK_OK;
}

#endif
// eof
