#include "include.h"
#include <stdio.h>
#include <string.h>
#include "typedef.h"
#include "arm_arch.h"
#include "gpio_pub.h"
#include "gpio_pub.h"
#include "uart_pub.h"
#include "multi_button.h"
#include "rtos_pub.h"
#include "error.h"
#include "sys_ctrl_pub.h"
#include "samples_config.h"

#ifdef  BUTTON_TEST

#define TEST_BUTTON 4

static beken_timer_t g_key_timer;

static void button_short_press(void *param)
{
	rt_kprintf("button_short_press\r\n");
}

static void button_double_press(void *param)
{
	rt_kprintf("button_double_press\r\n");
}

static void button_long_press_hold(void *param)
{
	rt_kprintf("button_long_press_hold\r\n");
}

static uint8_t key_get_gpio_level(BUTTON_S*handle)
{
	return bk_gpio_input((uint32_t)handle->user_data);
}


BUTTON_S gpio_button_test[2];

void button_test(int argc,char *argv[])
{
	OSStatus result;
	
	int gpio ;
	if(argc != 2)
	{	
		rt_kprintf("---! ! !param error---\r\n");
	}
	else
	{			
		gpio = atoi(argv[1]);
		
		rt_kprintf("---gpio%d as button : test start---\r\n",gpio);
			
		if((gpio >=40)||(gpio >= 40))
		{
			rt_kprintf("---! ! !gpio error---\r\n");
			return;
		}
		
		/*gpio key config:input && pull up*/
		gpio_config(gpio,GMODE_INPUT_PULLUP);
		
		button_init(&gpio_button_test[0], key_get_gpio_level, 0,(void*)gpio);			/*初始化按键*/
		
		button_attach(&gpio_button_test[0], SINGLE_CLICK,     button_short_press);      /*配置按键事件的回调函数*/
		button_attach(&gpio_button_test[0], DOUBLE_CLICK,     button_double_press);
		button_attach(&gpio_button_test[0], LONG_PRESS_HOLD,  button_long_press_hold);
		
		button_start(&gpio_button_test[0]);    											/*开始按键检测*/
	
		result = rtos_init_timer(&g_key_timer,										/*初始化按键状态检测时钟*/					
                            	TICKS_INTERVAL,
                            	button_ticks,
                            	(void *)0);
    	ASSERT(kNoErr == result);
	
    	result = rtos_start_timer(&g_key_timer);										/*开启时钟*/	
    	ASSERT(kNoErr == result);	
	}	
}

MSH_CMD_EXPORT(button_test,button test);

// eof
#endif
