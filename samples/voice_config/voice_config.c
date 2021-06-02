#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>
#include "samples_config.h"
#include "voice_config.h"
#include "../test/direct_connect.h"


#ifdef XIAOYA_OS
#include "parm_cache.h"
#include "player_manager.h"
#endif

#ifdef VOICE_CONFIG_TEST
#define NETCONFIG_TIMEOUT    (90*1000)

#define DEBUG_PRINTF     rt_kprintf("[voice] ");rt_kprintf
#define SAMPLE_RATE         (16000)

#define malloc  rt_malloc
#define realloc rt_realloc
#define free    rt_free

#define codec_device_lock(...)
#define codec_device_unlock(...)

/************************ voice config start ***************************/
static unsigned char voice_config_ssid[32 + 1] = {0};
static unsigned char voice_config_password[64 + 1] = {0};


void *voice_malloc(int size)
{
    return rt_malloc(size);
}

void voice_free(void *mem)
{
    rt_free(mem);
}

int voice_read(void *device, void *buffer, int size)
{
    struct rt_device *dev = (struct rt_device *)device;
    rt_size_t read_bytes = 0;

    while (read_bytes < size)
    {
        rt_size_t rb = rt_device_read(dev, 0, (void *)((char *)buffer + read_bytes), size - read_bytes);

        if (rb == 0)
            break;

        read_bytes += rb;
    }

    return read_bytes;
}

#include <finsh.h>
#include <msh.h>
static void station_connect(const char *ssid, const char *passwd)
{
    char argv[64];

    memset(argv, 0, sizeof(argv));
    sprintf(argv, "wifi %s join %s %s", "w0", ssid, passwd);
    msh_exec(argv, strlen(argv));
}

static rt_thread_t tid = RT_NULL;
static void cmd_voice_config_thread(void *parameter)
{
	extern rt_sem_t direct_conn_done_sem;

	if(direct_conn_done_sem != RT_NULL)
	{
	 	 rt_kprintf(" rt_sem_release direct_conn_done_sem =%d \n",rt_sem_release(direct_conn_done_sem));
	}

    rt_device_t device = 0;
    struct voice_config_result result={0};
    int res;
    rt_kprintf("cmd_voice_config_thread start!\n");
    DEBUG_PRINTF("voice config version: %s\r\n", voice_config_version());

    /* open audio device and set tx done call back */
    device = rt_device_find("mic");
    if (device == RT_NULL)
    {
        DEBUG_PRINTF("audio device not found!\r\n");
        goto _err;
    }

    codec_device_lock();
    if(device->flag & RT_DEVICE_FLAG_ACTIVATED)
    {
        rt_device_close(device);
    }
    res = rt_device_open(device, RT_DEVICE_OFLAG_RDWR);
    /* set samplerate */
    if (RT_EOK == res)
    {
        int SamplesPerSec = SAMPLE_RATE;
        if (rt_device_control(device, CODEC_CMD_SAMPLERATE, &SamplesPerSec)
                != RT_EOK)
        {
            rt_kprintf("[record] audio device doesn't support this sample rate: %d\r\n",
                       SamplesPerSec);
            goto _err;
        }
    }
    else
    {
        DEBUG_PRINTF("open audio device fail!\r\n");
        goto _err;
    }

    rt_device_write(device, 0, 0, 100); // start to record
    DEBUG_PRINTF("voice_config_work-----\r\n");
    res = voice_config_work(device, SAMPLE_RATE, NETCONFIG_TIMEOUT, &result);
    if(res == 0)
    {

    #ifdef XIAOYA_OS
        xiaoya_player_tips(TIP_FIND_AP_INFO,0);        
        /*not real save,just cache*/
        parm_set_wechat_openid_str((uint8_t *)result.custom);
        sta_cfg_t sta_cfg; 
        memcpy(sta_cfg.ssid_str,result.ssid,strlen(result.ssid)+1);
        memcpy(sta_cfg.pwd_str,result.passwd,strlen(result.passwd)+1);
        parm_set_sta_cfg(&sta_cfg);
    #endif
		/**    wifi–≈œ¢¥Ê¥¢     ***/
	
#ifdef AP_DIERCT_CONNECT_TEST
		extern direct_ap_info_t direct_ap_info;
	    memcpy(direct_ap_info.direct_ssid,result.ssid,strlen(result.ssid)+1);
        memcpy(direct_ap_info.direct_pwd,result.passwd,strlen(result.passwd)+1);
#endif

		
        rt_kprintf("ssid len=%d, [%s]\n", result.ssid_len, result.ssid);
        rt_kprintf("passwd L=%d, [%s]\n", result.passwd_len, result.passwd);
        rt_kprintf("custom L=%d, [%s]\n", result.custom_len, result.custom);
		
		station_connect(result.ssid,result.passwd);
    }
    else
    {
        rt_kprintf("voice_config res:%d\n", res);
    }

_err:
    if (device)
    {
        rt_device_close(device);
        codec_device_unlock();
    }

    tid = RT_NULL;

    return;
}

int voice_netconfig_start()
{
    rt_kprintf("voice_config start!\n");
    if (tid)
    {
        rt_kprintf("voice config already init.\n");
        return -1;
    }

    tid = rt_thread_create("voice_config",
                           cmd_voice_config_thread,
                           RT_NULL,
                           1024 * 6,
                           20,
                           10);

    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }

    return 0;
}


void voice_netconfig_stop(void)
{
    if (tid != RT_NULL)
    {
        
        rt_kprintf("voice config cancel .\n");
        voice_config_stop();
        tid = NULL;
    }
}

#ifdef FINSH_USING_MSH
#include "finsh.h"

MSH_CMD_EXPORT(voice_netconfig_start, start voice config);
MSH_CMD_EXPORT(voice_netconfig_stop, stop voice config);

#endif /* FINSH_USING_MSH */

#endif
