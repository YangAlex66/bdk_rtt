#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>
#include <wlan_dev.h>
#include <wlan_mgnt.h>
#include "airkiss.h"
#include <stdio.h>
#include <sys/socket.h>
#include "lwip/sockets.h"
#include "samples_config.h"
#include "arch.h"
#include "phy.h"
#include "mac_frame.h"
#include "mac_ie.h"
#include "wlan_ui_pub.h"

#ifdef RT_USING_AIRKISS
#define AIRKISS_PRINTF rt_kprintf
#define AIRKISS_SWITCH_TIMER    rt_tick_from_millisecond(50)    // 50ms
#define AIRKISS_DOING_TIMER     rt_tick_from_millisecond(30000)  // 20s
#define MAX_CHANNEL_NUM         14
static airkiss_context_t *ak_contex = RT_NULL;
const airkiss_config_t ak_conf =
{
    (airkiss_memset_fn) &rt_memset,
    (airkiss_memcpy_fn) &rt_memcpy,
    (airkiss_memcmp_fn) &rt_memcmp,
    (airkiss_printf_fn) &rt_kprintf,
};

static rt_timer_t g_switch_timer;
static rt_timer_t g_doing_timer;
static volatile uint8_t g_current_channel;
static int airkiss_recv_ret;
static rt_sem_t g_cfg_done_sem;
static uint8_t lock_channel_err_flag=0;
static int channel_idx;
static rt_thread_t airkiss_tid = RT_NULL;
static airkiss_result_t result;
typedef struct  switch_channel_t
{
    unsigned char switch_channel[14];
    unsigned char channel_bit[14];
    unsigned char channel_num;
}SWITCH_CHANNEL_ST;


typedef struct  mac_channel_t
{
    unsigned char ap_bssid[6];
    unsigned char ap_real_channel;
}MAX_CHANNEL_ST;

typedef struct  airkiss_ap_t
{   
    unsigned char ap_num;
    MAX_CHANNEL_ST *airkiss_ap_table;  
}AIRKISS_AP_LIST;

typedef struct  airkiss_result_info
{   
    unsigned char ssid[32];
    unsigned char passwd[32];
}AIRKISS_RESULT_T;

static AIRKISS_RESULT_T airkiss_result_ap;
static SWITCH_CHANNEL_ST channel_tab;
static AIRKISS_AP_LIST *airkiss_info=NULL;

static uint8_t get_channel_with_bssid(uint8_t *bssid)
{
    uint8_t channel=0;
    for(int index=0;index<airkiss_info->ap_num;index++)
    {
        if(memcmp(bssid,airkiss_info->airkiss_ap_table[index].ap_bssid,6)==0)
        {
            rt_kprintf("get channel:%d\r\n",airkiss_info->airkiss_ap_table[index].ap_real_channel);
            channel=airkiss_info->airkiss_ap_table[index].ap_real_channel;
            return channel;
        }
    }
    return channel;
}

static void  bk_wifi_scan(void)
{   
    struct rt_wlan_device *wlan;
    struct rt_wlan_scan_result *scan_result = RT_NULL;
    wlan = (struct rt_wlan_device *)rt_device_find("w0");
    if (!wlan)
    {
        AIRKISS_PRINTF("no wlan device\n");
        return;
    }
    rt_wlan_scan(wlan,RT_NULL, &scan_result);
    airkiss_info->ap_num=0;
    if (scan_result)
    {
        int index, num;
        num = scan_result->ap_num;
        airkiss_info->ap_num=num;
        airkiss_info->airkiss_ap_table = rt_malloc(sizeof(MAX_CHANNEL_ST) * num);
        rt_memset(airkiss_info->airkiss_ap_table, 0, sizeof(MAX_CHANNEL_ST) * num);
        AIRKISS_PRINTF("             SSID                      MAC            rssi   chn    Mbps\n");
        AIRKISS_PRINTF("------------------------------- -----------------     ----   ---    ----\n");
        for (index = 0; index < num; index ++)
        {
            AIRKISS_PRINTF("%-32.32s", scan_result->ap_table[index].ssid);
            AIRKISS_PRINTF("%02x:%02x:%02x:%02x:%02x:%02x     ", 
                scan_result->ap_table[index].bssid[0],
                scan_result->ap_table[index].bssid[1],
                scan_result->ap_table[index].bssid[2],
                scan_result->ap_table[index].bssid[3],
                scan_result->ap_table[index].bssid[4],
                scan_result->ap_table[index].bssid[5]
            );
            channel_tab.channel_bit[scan_result->ap_table[index].channel]=1;
            airkiss_info->airkiss_ap_table[index].ap_real_channel=scan_result->ap_table[index].channel;
            rt_memcpy(airkiss_info->airkiss_ap_table[index].ap_bssid, scan_result->ap_table[index].bssid,6);
            AIRKISS_PRINTF("%4d    ", scan_result->ap_table[index].rssi);
            AIRKISS_PRINTF("%2d    ", scan_result->ap_table[index].channel);
            AIRKISS_PRINTF("%d\n", scan_result->ap_table[index].datarate / 1000000);
            }
        }
        rt_wlan_release_scan_result(&scan_result);
}

static int bk_airkiss_check_channel(uint8_t * frame)
{
    int channel=0;
    uint8_t *bssi=NULL;

    struct mac_hdr *fmac_hdr = (struct mac_hdr *)frame;
    if((fmac_hdr->fctl & MAC_FCTRL_TODS_FROMDS) == MAC_FCTRL_FROMDS)
    {
        AIRKISS_PRINTF("=======from ds===========\r\n");
        bssi=(uint8_t*)&fmac_hdr->addr2;
    }
    else if((fmac_hdr->fctl & MAC_FCTRL_TODS_FROMDS) == MAC_FCTRL_TODS)
    {
        AIRKISS_PRINTF("=======to ds===========\r\n");
        bssi= (uint8_t*)&fmac_hdr->addr1;
    }
    AIRKISS_PRINTF("bssi %02x:%02x:%02x:%02x:%02x:%02x\n",bssi[0],bssi[1],bssi[2],bssi[3],bssi[4],bssi[5]);
    channel=get_channel_with_bssid(bssi);

    if(0==channel)
    {
        AIRKISS_PRINTF("wifi scan null,goto normal airkiss\r\n");
    }
    else if(g_current_channel!=channel)
    {
        AIRKISS_PRINTF("channel Lock error,try to switch right channel\r\n");
        g_current_channel=channel;
    }
    else
    {
        AIRKISS_PRINTF("channel Lock right\r\n");
        channel=0;
    }
    return channel;
}

static void get_channel_list_to_switch()
{
    uint8_t index=0;
    channel_tab.channel_num=0;
    for(uint8_t i=1;i<=13;i++)
    {
        if(channel_tab.channel_bit[i]==1)
        {
            channel_tab.switch_channel[index++]=i;
            channel_tab.channel_num++;
        }
    }

    if(0==index)
    {
        for(uint8_t i=0;i<13;i++)
        {
            channel_tab.switch_channel[i]=i+1;
        }
        channel_tab.channel_num=13;
    }

    AIRKISS_PRINTF("switch channel: ");   
    for(uint8_t i=0;i<channel_tab.channel_num;i++)
        AIRKISS_PRINTF("%d  ",channel_tab.switch_channel[i]);
    AIRKISS_PRINTF("\r\n");   

}

static void airkiss_switch_channel(void *parameter)
{

    if(1==lock_channel_err_flag)
    {
        AIRKISS_PRINTF("Lock err before,Lock channel %d \n", g_current_channel);
        bk_wlan_set_channel_with_band_width(g_current_channel,PHY_CHNL_BW_20);  
        lock_channel_err_flag=0;
        rt_timer_stop(g_switch_timer);
        rt_timer_start(g_doing_timer);
        return;
    }

    g_current_channel= channel_tab.switch_channel[channel_idx++];
    if(channel_idx>=channel_tab.channel_num)
        channel_idx=0;
    AIRKISS_PRINTF("Switch channel %d \n", g_current_channel);
    bk_wlan_set_channel_with_band_width(g_current_channel,PHY_CHNL_BW_20);  
    airkiss_change_channel(ak_contex);
}

static void airkiss_doing_timeout(void *parameter)
{
    int result;
    AIRKISS_PRINTF("airkiss_doing_timeout, restart channel switch timer\r\n");
    result = airkiss_init(ak_contex, &ak_conf);
    if (RT_EOK != result)
    {
        AIRKISS_PRINTF("Airkiss init failed!!\r\n");
    } 

    rt_timer_start(g_switch_timer);
}

static void airkiss_monitor_callback(uint8_t *data, int len, void *user_data)
{
    if(airkiss_recv_ret==AIRKISS_STATUS_COMPLETE)
        return;
    airkiss_recv_ret = airkiss_recv(ak_contex, data, len);
    if (airkiss_recv_ret == AIRKISS_STATUS_CHANNEL_LOCKED)
    {
        if(bk_airkiss_check_channel(data))
        {
            lock_channel_err_flag=1;
            AIRKISS_PRINTF("Lock channel error,lock again\r\n");
        }
        else
        {
            AIRKISS_PRINTF("Lock channel:%d\r\n",g_current_channel);
            rt_timer_stop(g_switch_timer);
            rt_timer_start(g_doing_timer);
        }
    }
    else if (airkiss_recv_ret == AIRKISS_STATUS_COMPLETE)
    {
        rt_sem_release(g_cfg_done_sem);
        rt_timer_stop(g_doing_timer);
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
        AIRKISS_PRINTF("Got IP address : %s\n", ipaddr_ntoa(&(netif->ip_addr)));
    }

    return result;
}

static int station_connect(char *ssid, char *passwd)
{
    rt_err_t result = RT_EOK;
    struct rt_wlan_info info;
    struct rt_wlan_device *wlan;

    wlan = (struct rt_wlan_device *)rt_device_find(WIFI_DEVICE_STA_NAME);
    if (!wlan)
    {
        AIRKISS_PRINTF("no wlan:%s device\n", WIFI_DEVICE_STA_NAME);
        return -1;
    }

    result = rt_wlan_init(wlan, WIFI_STATION);
    rt_wlan_info_init(&info, WIFI_STATION, SECURITY_WPA2_AES_PSK, ssid);
    result = rt_wlan_connect(wlan, &info, passwd);
    rt_wlan_info_deinit(&info);

    return result;
}

static void airkiss_send_notification_thread(void *parameter)
{
    int sock = -1;
    int udpbufsize = 2;
    uint8_t random = (uint32_t)parameter;
    struct sockaddr_in g_stUDPBCAddr, g_stUDPBCServerAddr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        AIRKISS_PRINTF("notify create socket error!\n");
        goto _exit;
    }

    g_stUDPBCAddr.sin_family = AF_INET;
    g_stUDPBCAddr.sin_port = htons(10000);
    g_stUDPBCAddr.sin_addr.s_addr = htonl(0xffffffff);
    rt_memset(&(g_stUDPBCAddr.sin_zero), 0, sizeof(g_stUDPBCAddr.sin_zero));

    g_stUDPBCServerAddr.sin_family = AF_INET;
    g_stUDPBCServerAddr.sin_port = htons(10000);
    g_stUDPBCServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    rt_memset(&(g_stUDPBCServerAddr.sin_zero), 0, sizeof(g_stUDPBCServerAddr.sin_zero));

    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &udpbufsize, sizeof(int)) != 0)
    {
        AIRKISS_PRINTF("notify socket setsockopt error\n");
        goto _exit;
    }

    if (bind(sock, (struct sockaddr *)&g_stUDPBCServerAddr, sizeof(g_stUDPBCServerAddr)) != 0)
    {
        AIRKISS_PRINTF("notify socket bind error\n");
        goto _exit;
    }

    for (int i = 0; i <= 20; i++)
    {
        sendto(sock, (char *)&random, 1, 0, (struct sockaddr *)&g_stUDPBCAddr, sizeof(g_stUDPBCAddr));
        rt_thread_delay(10);
    }

    AIRKISS_PRINTF("airkiss notification thread exit!\n");

_exit:
    if (sock >= 0)
    {
        closesocket(sock);
    }
}

uint32_t airkiss_get_status(void)
{
    return airkiss_recv_ret;
}

airkiss_result_t *airkiss_result_get(void)
{
    return &result;
}

static void airkiss_thread_entry(void *parameter)
{
    int ret;
    static struct rt_wlan_device *g_wlan_device = RT_NULL;
    airkiss_recv_ret=AIRKISS_STATUS_CONTINUE;
    airkiss_info = (AIRKISS_AP_LIST *)rt_malloc(sizeof(AIRKISS_AP_LIST));
    airkiss_info->airkiss_ap_table=NULL;
    if (!airkiss_info)
    {
        rt_kprintf("Malloc memory for airkiss context \n");
        goto _exit;
    }
    memset(&channel_tab,0,sizeof(SWITCH_CHANNEL_ST));
    bk_wifi_scan();
    get_channel_list_to_switch();

    g_switch_timer = rt_timer_create("switch_channel",
                                     airkiss_switch_channel,
                                     RT_NULL,
                                     AIRKISS_SWITCH_TIMER,
                                     RT_TIMER_FLAG_SOFT_TIMER | RT_TIMER_FLAG_PERIODIC);
    if (!g_switch_timer)
    {
        rt_kprintf("Create airkiss swtich channel timer failed \n");
        goto _exit;
    }

    g_doing_timer = rt_timer_create("doing_timeout",
                                    airkiss_doing_timeout,
                                    RT_NULL,
                                    AIRKISS_DOING_TIMER,
                                    RT_TIMER_FLAG_SOFT_TIMER | RT_TIMER_FLAG_ONE_SHOT);
    if (!g_doing_timer)
    {
        rt_kprintf("Create airkiss doing timeout timer failed \n");
        goto _exit;
    }

    g_cfg_done_sem = rt_sem_create("tlink", 0, RT_IPC_FLAG_FIFO);
    if (!g_cfg_done_sem)
    {
        rt_kprintf("Create  airkiss config done sem failed! \n");
        goto _exit;
    }

    ak_contex = (airkiss_context_t *)rt_malloc(sizeof(airkiss_context_t));
    if (!ak_contex)
    {
        rt_kprintf("Malloc memory for airkiss context \n");
        goto _exit;
    }

    ret = airkiss_init(ak_contex, &ak_conf);
    if (ret != RT_EOK)
    {
        rt_kprintf("Airkiss init failed!!\r\n");
        goto _exit;
    }
    AIRKISS_PRINTF("Airkiss version: %s\r\n", airkiss_version());

    g_wlan_device = (struct rt_wlan_device *)rt_device_find(WIFI_DEVICE_STA_NAME);
    if (g_wlan_device == RT_NULL)
    {
        rt_kprintf("Device not found\n");
        return;
    }
    channel_idx=1;
    g_current_channel = 1;
    bk_wlan_set_channel_with_band_width(g_current_channel,PHY_CHNL_BW_20);  
    rt_wlan_set_monitor_callback(g_wlan_device, airkiss_monitor_callback);
    rt_wlan_cfg_monitor(g_wlan_device, WIFI_MONITOR_START);

    rt_timer_start(g_switch_timer);

    if (rt_sem_take(g_cfg_done_sem, rt_tick_from_millisecond(1000 * 90)) != RT_EOK)
    {
        AIRKISS_PRINTF("Wait semaphore timeout \n");
    }
    rt_wlan_cfg_monitor(g_wlan_device, WIFI_MONITOR_STOP);
    rt_wlan_set_monitor_callback(g_wlan_device, RT_NULL);
    
    if (airkiss_recv_ret == AIRKISS_STATUS_COMPLETE)
    {
        AIRKISS_PRINTF("airkiss complte,start connect ap\r\n");
        int8_t err;
        int8_t tick = 0;
        err = airkiss_get_result(ak_contex, &result);
        rt_memcpy(airkiss_result_ap.passwd,result.pwd,result.pwd_length);
        if (err == 0)
        {

            AIRKISS_PRINTF("\r\nairkiss_get_result() ok!\n");
            AIRKISS_PRINTF(" ssid = %s \n pwd = %s \n,random = 0x%02x\r\n",
                           result.ssid, airkiss_result_ap.passwd,result.random);
            
        }

        station_connect(result.ssid, result.pwd);
        do
        {
            tick ++;
            rt_thread_delay(rt_tick_from_millisecond(1000));
            if (tick >= 30)
            {
                rt_kprintf("GET IP Time Out!!! \n");
                goto _exit;
            }

        }while (!get_wifi_status(g_wlan_device->parent.netif));

        {
            rt_thread_t tid;

            tid = rt_thread_create("air_echo",
                                   airkiss_send_notification_thread, (void *)((uint32_t)result.random),
                                   1536, RT_THREAD_PRIORITY_MAX - 3, 20);
            if (tid != RT_NULL)
            {
                rt_thread_startup(tid);
            }
        }
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
    if (ak_contex != RT_NULL)
    {
        rt_free(ak_contex);
        ak_contex = RT_NULL;
    }
    if(airkiss_info->airkiss_ap_table)
    {
        rt_free(airkiss_info->airkiss_ap_table);
        airkiss_info->airkiss_ap_table = RT_NULL;
    }
    if(airkiss_info)
    {
        rt_free(airkiss_info);
        airkiss_info = RT_NULL;
    }
    if (g_cfg_done_sem)
    {
        rt_sem_delete(g_cfg_done_sem);
        g_cfg_done_sem = RT_NULL;
    }
    if(airkiss_tid)
    {
        airkiss_tid=NULL;
    }
    AIRKISS_PRINTF("airkiss exit\r\n");
}

int start_airkiss(void)
{
    
    int result = -1;

    if(airkiss_tid)
    {
        AIRKISS_PRINTF("airkiss have been init\r\n");
        return -1;
    }

    airkiss_tid = rt_thread_create("airkiss",
                           airkiss_thread_entry,
                           RT_NULL,
                           1024 * 4,
                           20,
                           10);

    if (airkiss_tid != NULL)
    {
        rt_thread_startup(airkiss_tid);
        result = 0;
    }
    return result;
}

int stop_airkiss(void)
{
	if(!airkiss_tid) {
		return -1;
	}

	rt_sem_release(g_cfg_done_sem);
	return 0;
}

MSH_CMD_EXPORT(start_airkiss, start_ariksss);
MSH_CMD_EXPORT(stop_airkiss, stop_airkiss);
#endif
