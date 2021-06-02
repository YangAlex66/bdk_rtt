#include "error.h"
#include "include.h"

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include <stdint.h>
#include <stdlib.h>
#include <finsh.h>
#include <rtdef.h>

#include "manual_ps_pub.h"
#include "rtos_pub.h"
#include "test_config.h"
#include "gpio_pub.h"
#define DEEP_SLEEP_TEST
#ifdef DEEP_SLEEP_TEST



static int htoi(char s[])
{
    int i;
    int n = 0;
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))
    {
            i = 2;
    }
    else
    {
            i = 0;
    }
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)
    {
            if (tolower(s[i]) > '9')
            {
                        n = 16 * n + (10 + tolower(s[i]) - 'a');
            }
            else
            {
                        n = 16 * n + (tolower(s[i]) - '0');
            }
    }
    return n;
}



static void enter_deep_sleep_test(int argc,char *argv[])
{
	rtos_delay_milliseconds(10);
	PS_DEEP_CTRL_PARAM deep_sleep_param;

	deep_sleep_param.wake_up_way			= 0;
	
	deep_sleep_param.gpio_index_map      	= htoi(argv[1]);
	deep_sleep_param.gpio_edge_map       	= htoi(argv[2]);	
	deep_sleep_param.gpio_last_index_map 	= htoi(argv[3]);
	deep_sleep_param.gpio_last_edge_map  	= htoi(argv[4]);
	deep_sleep_param.sleep_time     		= htoi(argv[5]);
	deep_sleep_param.wake_up_way     		= htoi(argv[6]);
	deep_sleep_param.lpo_32k_src			= htoi(argv[7]);

	if(argc == 8)
	{		
		rt_kprintf("---deep sleep test param : 0x%0X 0x%0X 0x%0X 0x%0X %d %d %d\r\n", 
					deep_sleep_param.gpio_index_map, 
					deep_sleep_param.gpio_edge_map,
					deep_sleep_param.gpio_last_index_map, 
					deep_sleep_param.gpio_last_edge_map,
					deep_sleep_param.sleep_time,
					deep_sleep_param.wake_up_way,
					deep_sleep_param.lpo_32k_src);
		
		bk_enter_deep_sleep_mode(&deep_sleep_param);
	}
	else
	{
		rt_kprintf("---argc error!!! \r\n");
	}
}

FINSH_FUNCTION_EXPORT_ALIAS(enter_deep_sleep_test, __cmd_sleep_mode, test sleep mode);

#endif
