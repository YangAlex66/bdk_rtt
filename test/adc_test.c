#include "error.h"
#include "include.h"

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include <stdint.h>
#include <stdlib.h>
#include <finsh.h>
#include <rtdef.h>

#include "saradc_intf.h"
#include "saradc_pub.h"
#include "test_config.h"

#define ADC_TEST
#ifdef  ADC_TEST

int Step_Flag ;
int Adctest_Flag ;
int adc_offfset,  adc_value_2v, adc_voltage;

static ADC_OBJ test_adc;

/****channel 1 - 7***/
static void adc_detect_callback(int new_mv, void *user_data)
{
	static int cnt = 0;
	test_adc.user_data = (void*)new_mv;

	if(cnt++ >= 100)
	{	
		cnt = 0;
    	rt_kprintf("adc channel%d voltage:%d,%x\r\n",test_adc.channel,new_mv,test_adc.user_data);
	}
}

static void adc_detect_callback1(int new_mv, void *user_data)
{
	static int total=0;
	static int cnt = 0;
	int low_adc;
	static int temp=0;
	if(temp++<100)
		return;
	test_adc.user_data = (void*)new_mv;

	total+=new_mv;
	cnt++;
	if(cnt>= 100)
	{	
		low_adc=total/cnt;
		saradc_val.low=low_adc;
		cnt = 0;
		temp=0;
		adc_offfset = low_adc - 2048;
		rt_kprintf("step1: adc channel:%d adc_offfset:%d,new_mv:%d\r\n",test_adc.channel,adc_offfset,low_adc);
		total=0;
		adc_obj_stop(&test_adc);
	}

}


static void adc_detect_callback2(int new_mv, void *user_data)
{
	static int total=0;
	static int cnt = 0;
	static int temp=0;
	if(temp++<100)
		return;
	int high_adc;
	test_adc.user_data = (void*)new_mv;
	total+=new_mv;
	cnt++;
	if(cnt >= 100)
	{
		high_adc=total/cnt;
		saradc_val.high=high_adc;
		cnt = 0;
		temp=0;
		adc_value_2v = high_adc;
		rt_kprintf("step2: adc channel:%d adc_value_2v:%d\r\n",test_adc.channel,high_adc);
		total=0;
		Adctest_Flag =1;
		adc_obj_stop(&test_adc);
	}
}

__maybe_unused static void adc_detect_callback3(int new_mv, void *user_data);
static void adc_detect_callback3(int new_mv, void *user_data)
{

	static int cnt = 0;
	test_adc.user_data = (void*)new_mv;

	if(cnt++ >= 50)
	{
		cnt = 0;
	}
}

void adc_channel_test(int argc,char *argv[])
{
	int channel;
	
	if (strcmp(argv[1], "start") == 0)
	{
		if(argc == 3)
		{
			channel = atoi(argv[2]);
			rt_kprintf("---adc channel:%d---\r\n",channel);
			Adctest_Flag =1;
			Step_Flag = 1;
			saradc_work_create();
			adc_obj_init(&test_adc, adc_detect_callback, channel, &test_adc);
			adc_obj_start(&test_adc);
		}
		else
		{
			rt_kprintf("input param error\r\n");
		}
	}
	if(strcmp(argv[1], "stop") == 0)
	{
		adc_obj_stop(&test_adc);
	}
}

void adc_cal_test(int argc,char *argv[])
{
	int channel;
	
	if (strcmp(argv[1], "start") == 0)
	{
		if(strcmp(argv[3], "offset") == 0)
		{
			channel = atoi(argv[2]);
			rt_kprintf("---adc channel:%d---\r\n",channel);
			Step_Flag = 0;
			Adctest_Flag  = 0;
			saradc_work_create();
			adc_obj_init(&test_adc, adc_detect_callback1, channel, &test_adc);
			adc_obj_start(&test_adc);
		}
		if(strcmp(argv[3], "2v") == 0)
		{
			channel = atoi(argv[2]);
			rt_kprintf("---adc channel:%d---\r\n",channel);
			Step_Flag = 1;
			Adctest_Flag  = 0;
			saradc_work_create();
			adc_obj_init(&test_adc, adc_detect_callback2, channel, &test_adc);
			adc_obj_start(&test_adc);
		}
	    if(strcmp(argv[3], "voltage") == 0)
		{
			channel = atoi(argv[2]);
			rt_kprintf("---adc channel:%d---\r\n",channel);
			Step_Flag = 1;
			Adctest_Flag  = 1;
			saradc_work_create();
			adc_obj_init(&test_adc, adc_detect_callback, channel, &test_adc);
			adc_obj_start(&test_adc);
		}	

	}
	else if(strcmp(argv[1], "stop") == 0)
	{
		adc_obj_stop(&test_adc);
	}
	#if CFG_SARADC_CALIBRATE
	else if(0 == strcmp(argv[1], "read_cali_value"))
    {
    	int status;
        status = manual_cal_load_adc_cali_flash();
        if(status != 0)
        {
            rt_kprintf("Can't read cali value, use default!\r\n");
            rt_kprintf("calibrate low value:[%x]\r\n", saradc_val.low);
            rt_kprintf("calibrate high value:[%x]\r\n", saradc_val.high);
        }
    }
	else if(0 == strcmp(argv[1], "write_cali_value"))
    {
    	int status;
    	if(argc<4)
			return;
		saradc_val.low= atoi(argv[2]);
		saradc_val.high= atoi(argv[3]);
        status = manual_cal_write_adc_cali_flash(&saradc_val);
        if(status != 0)
        {
            rt_kprintf("write cali value fail!\r\n");
            rt_kprintf("default calibrate low value:[%x]\r\n", saradc_val.low);
            rt_kprintf("default calibrate high value:[%x]\r\n", saradc_val.high);
        }
    }
    #endif
}



MSH_CMD_EXPORT(adc_channel_test,adc test);
MSH_CMD_EXPORT(adc_cal_test,adc_cal_test);


#endif
