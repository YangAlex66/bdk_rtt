#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>
#include <wlan_dev.h>
#include <wlan_mgnt.h>
#include <stdio.h>
#include <sys/socket.h>
#include "lwip/sockets.h"
#include "ezconfig.h"
#include "phy.h"
#include "samples_config.h"
#ifdef EZ_CONFIG_SAMPLE

#define EZCONFIG_PRINTF  rt_kprintf
#define EZCONFIG_SWITCH_TIMER    rt_tick_from_millisecond(50)    // ms
#define EZCONFIG_DOING_TIMER     rt_tick_from_millisecond(30000)  // 20s
#define MAX_CHANNEL_NUM          14
 
static rt_timer_t g_switch_timer;
static rt_timer_t g_doing_timer;
static struct rt_wlan_device *g_wlan_device = RT_NULL;
static volatile uint8_t g_current_channel;
static int ezconfig_recv_ret;
static rt_sem_t g_cfg_done_sem;
static int channel_model;
EZconfig_result_t *ez_result;
static void bk_ezconfig_switch_channel(void *parameter)
{
       
    g_current_channel++;
    if (g_current_channel >= MAX_CHANNEL_NUM)
    {
        g_current_channel = 1;
        channel_model=~channel_model;
    }
    if(0==channel_model)
    {
        bk_wlan_set_channel_with_band_width(g_current_channel,PHY_CHNL_BW_20);  
    }
    else
    {
        bk_wlan_set_channel_with_band_width(g_current_channel,PHY_CHNL_BW_40);  
    }
   
    EZCONFIG_PRINTF("Switch channel %d \n", g_current_channel);

}

static void bk_ezconfig_doing_timeout(void *parameter)
{
    EZCONFIG_PRINTF("bk_ezconfig_doing_timeout, restart channel switch timer\r\n");
    bk_ezconfig_reset_recive();
    rt_timer_start(g_switch_timer);
}

static void bk_ezconfig_monitor_callback(uint8_t *data, int len, void *user_data)
{

    
    ezconfig_recv_ret=bk_ezconfig_recv(data);

    if (ezconfig_recv_ret == EZCONFIG_STATUS_CHANNEL_LOCKED)
    {
        rt_timer_stop(g_switch_timer);
        EZCONFIG_PRINTF("Lock channel in %d \n", g_current_channel);
        rt_timer_start(g_doing_timer);
    }
    else if (ezconfig_recv_ret == EZCONFIG_STATUS_COMPLETE)
    {
        rt_wlan_cfg_monitor(g_wlan_device, WIFI_MONITOR_STOP);
        rt_wlan_set_monitor_callback(g_wlan_device, RT_NULL);
        rt_timer_stop(g_doing_timer);
        rt_sem_release(g_cfg_done_sem);
    }

}

static int get_wifi_status(struct netif *netif)
{
    ip_addr_t ip_addr;
    int result = 0;

    ip_addr_set_zero(&ip_addr);
    if (ip_addr_cmp(&(netif->ip_addr), &ip_addr))
        result = 0;
    else
    {
        result = 1;
        rt_kprintf("Got IP address : %s\n", ipaddr_ntoa(&(netif->ip_addr)));
    }

    return result;
}


static int station_connect(char *ssid, char *passwd)
{
    rt_err_t result = RT_EOK;
    struct rt_wlan_info info;
    struct rt_wlan_device *wlan;
    rt_tick_t tick = 0;

    wlan = (struct rt_wlan_device *)rt_device_find(WIFI_DEVICE_STA_NAME);
    if (!wlan)
    {
        rt_kprintf("no wlan:%s device\n", WIFI_DEVICE_STA_NAME);
        return -1;
    }

    result = rt_wlan_init(wlan, WIFI_STATION);
    rt_wlan_info_init(&info, WIFI_STATION, SECURITY_WPA2_AES_PSK, ssid);
    result = rt_wlan_connect(wlan, &info, passwd);
    rt_wlan_info_deinit(&info);

    return result;
}

static uint32_t hexstr_ip_to_hex_ip(uint8_t  *addr_test)
{
    uint32_t  ip_t;
    ip_t=(addr_test[0]<<24)|(addr_test[1]<<16)|(addr_test[2]<<8)|addr_test[3];
    return ip_t;
}


static void ezconfig_send_notification_thread(void *parameter)
{
    int sock = -1;
    uint8_t ret=0;
    uint8_t *ip_hexstr = parameter;
    struct sockaddr_in g_stUDPBCAddr;
    const char *net_state="OK";

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        EZCONFIG_PRINTF("notify create socket error!\n");
        goto _exit;
    }
    //recv_data = rt_malloc(BUFSZ);
    uint32_t ip_addr=hexstr_ip_to_hex_ip(ip_hexstr);
    rt_kprintf("======clinet ip_addr:%d.%d.%d.%d\r\n",ip_hexstr[0],ip_hexstr[1],ip_hexstr[2],ip_hexstr[3]);
    
    g_stUDPBCAddr.sin_family = AF_INET;
    g_stUDPBCAddr.sin_port = htons(10000);
    //g_stUDPBCAddr.sin_addr.s_addr = htonl(ip_addr);
    g_stUDPBCAddr.sin_addr.s_addr = htonl(ip_addr);
    rt_memset(&(g_stUDPBCAddr.sin_zero), 0, sizeof(g_stUDPBCAddr.sin_zero));

    if (connect(sock, (struct sockaddr *)&g_stUDPBCAddr, sizeof(struct sockaddr)) == -1)
    {
        /* 连接失败 */
        rt_kprintf("Connect fail!\n");
        goto _exit; 
    }
    /* 发送数据到sock连接 */
    ret = send(sock, net_state, strlen(net_state), 0);
    if (ret < 0)
    {
        /* 接收失败，关闭这个连接 */
        EZCONFIG_PRINTF("\nsend error,close the socket.\r\n");

    }
    else if (ret == 0)
    {
        /* 打印send函数返回值为0的警告信息 */
        EZCONFIG_PRINTF("\n Send warning,send function return 0.\r\n");
    }
    else
    {
        EZCONFIG_PRINTF("\nsend state success.\r\n");
    }


_exit:
    if (sock >= 0)
    {
        closesocket(sock);
    }
    EZCONFIG_PRINTF("ezconfig notification thread exit!\n");
}



uint32_t bk_ezconfig_get_status(void)
{
    return ezconfig_recv_ret;
}


static void ezconfig_thread_entry(void *parameter)
{
    int ret;

    g_switch_timer = rt_timer_create("switch_channel",
                                     bk_ezconfig_switch_channel,
                                     RT_NULL,
                                     EZCONFIG_SWITCH_TIMER,
                                     RT_TIMER_FLAG_SOFT_TIMER | RT_TIMER_FLAG_PERIODIC);
    if (!g_switch_timer)
    {
        rt_kprintf("Create ezconfig swtich channel timer failed \n");
        goto _exit;
    }

    g_doing_timer = rt_timer_create("doing_timeout",
                                    bk_ezconfig_doing_timeout,
                                    RT_NULL,
                                    EZCONFIG_DOING_TIMER,
                                    RT_TIMER_FLAG_SOFT_TIMER | RT_TIMER_FLAG_ONE_SHOT);
    if (!g_doing_timer)
    {
        rt_kprintf("Create ezconfig doing timeout timer failed \n");
        goto _exit;
    }

    g_cfg_done_sem = rt_sem_create("ezlink", 0, RT_IPC_FLAG_FIFO);
    if (!g_cfg_done_sem)
    {
        rt_kprintf("Create  ezconfig config done sem failed! \n");
        goto _exit;
    }

    bk_ezconfig_init();

    g_wlan_device = (struct rt_wlan_device *)rt_device_find(WIFI_DEVICE_STA_NAME);
    if (g_wlan_device == RT_NULL)
    {
        rt_kprintf("Device not found\n");
        return;
    }
    channel_model=0;
    g_current_channel =1;
   
    rt_wlan_set_monitor_callback(g_wlan_device, bk_ezconfig_monitor_callback);
    rt_wlan_cfg_monitor(g_wlan_device, WIFI_MONITOR_START);
    // rt_wlan_set_channel(g_wlan_device, g_current_channel);
    bk_wlan_set_channel_with_band_width(g_current_channel,PHY_CHNL_BW_20);
    rt_timer_start(g_switch_timer);

    if (rt_sem_take(g_cfg_done_sem, rt_tick_from_millisecond(1000 * 120)) != RT_EOK)
    {
        EZCONFIG_PRINTF("Wait semaphore timeout \n");
    }

    if (ezconfig_recv_ret == EZCONFIG_STATUS_COMPLETE)
    {
        int8_t err;
        int8_t tick = 0;
        ez_result=ezconfig_get_result();
        EZCONFIG_PRINTF("ssid = %s, passwd = %s\r\n",ez_result->ssid, ez_result->passwd);
        EZCONFIG_PRINTF("ip:%d.%d.%d.%d\r\n",ez_result->ip[0],ez_result->ip[1],ez_result->ip[2],ez_result->ip[3]);
        station_connect(ez_result->ssid, ez_result->passwd);

        do
        {
            tick ++;
            rt_thread_delay(rt_tick_from_millisecond(1000));
            if (tick >= 60)
            {
                rt_kprintf("GET IP Time Out!!! \n");
                goto _exit;
            }

        }while (!get_wifi_status(g_wlan_device->parent.netif));

        {
            rt_thread_t tid;

            tid = rt_thread_create("ez_echo",
                                   ezconfig_send_notification_thread, (void *)((uint8_t *)ez_result->ip),
                                   1024, RT_THREAD_PRIORITY_MAX - 3, 20);
            if (tid != RT_NULL)
            {
                rt_thread_startup(tid);
            }
        }
 
    }
    else
    {
        rt_wlan_cfg_monitor(g_wlan_device, WIFI_MONITOR_STOP);
        rt_wlan_set_monitor_callback(g_wlan_device, RT_NULL);
    }

_exit:
    if (g_switch_timer)
    {
        rt_timer_stop(g_switch_timer);
        rt_timer_delete(g_switch_timer);
        g_switch_timer = RT_NULL;
    }
    if (g_doing_timer)
    {
        rt_timer_stop(g_doing_timer);
        rt_timer_delete(g_doing_timer);
        g_doing_timer = RT_NULL;
    }

    if (g_cfg_done_sem)
    {
        rt_sem_delete(g_cfg_done_sem);
        g_cfg_done_sem = RT_NULL;
    }
}

int ezconfig(void)
{
    
    int result = 0;
    rt_thread_t tid = RT_NULL;

    tid = rt_thread_create("ezconfig",
                           ezconfig_thread_entry,
                           RT_NULL,
                           1024 * 4,
                           20,
                           10);

    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
        result = 1;
    }
    return result;
}


int start_ezconfig(int argc, char *argv[])
{
    if(g_cfg_done_sem == RT_NULL)
    {
        if(1 == ezconfig())
        {
            rt_kprintf("ezconfig start\r\n");

            rt_thread_delay(rt_tick_from_millisecond(1000));    
            
            while(g_cfg_done_sem)
            {
                uint32_t res;
                res = bk_ezconfig_get_status();
                if(res == EZCONFIG_STATUS_COMPLETE)
                {
                    EZconfig_result_t *ez_result;
                    ez_result=ezconfig_get_result();
                    rt_kprintf("---ssid:%s , key:%s---\r\n", ez_result->ssid,ez_result->passwd);
                    break;
                }
                
                rt_thread_delay(rt_tick_from_millisecond(100)); 
            }
        }
        else
            rt_kprintf("ezconfig fail\r\n");
    }
}
#ifdef FINSH_USING_MSH
#include "finsh.h"

MSH_CMD_EXPORT(start_ezconfig, start_ezconfig);

#endif
#endif