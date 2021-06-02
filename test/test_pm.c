#include <rthw.h>
#include <rtthread.h>
#include "wlan_ui_pub.h"
#include <stdint.h>
#include <stdlib.h>
#include "test_config.h"
#include "wlan_dev.h"
#include "rtdevice.h"

#ifdef PM_TEST

int bk_wlan_enter_powersave(struct rt_wlan_device *device, int level)
{
    int result = 0;

    if (device == RT_NULL) return -RT_EIO;

    result = rt_device_control(RT_DEVICE(device), WIFI_ENTER_POWERSAVE, (void *)&level);

    return result;
}

static int pm_level(int argc, char **argv)
{
	uint32_t level;

		if(argc != 2)
		{
			rt_kprintf("input argc is err!\n");
			return -1;
		}
	
		level = atoi(argv[1]);
		if(level > 4)
		{
		    rt_kprintf("nonsupport level %d\n", level);
			return -1;
		}
	
		{
			struct rt_wlan_device *sta_device = (struct rt_wlan_device *)rt_device_find(WIFI_DEVICE_STA_NAME);
			if (NULL != sta_device)
			{
				bk_wlan_enter_powersave(sta_device, level);
				rt_kprintf("bk_wlan_enter_powersave switch to %d\n", level);
			}
		}
	return 0;
}


#ifdef RT_USING_FINSH
#include <finsh.h>

MSH_CMD_EXPORT(pm_level, pm_level 1);

#endif /* RT_USING_FINSH */

#endif

