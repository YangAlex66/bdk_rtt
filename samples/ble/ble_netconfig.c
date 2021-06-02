#include <rtthread.h>
#ifdef BLE_CONFIG_SAMPLE

#include <finsh.h>
#include "ble_api.h"
#include "ble_pub.h"
#include "param_config.h"
#include "cJSON.h"
#include "ble_netconfig.h"
#include "common.h"
#include "rwapp_config.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "samples_config.h"
#include "../test/direct_connect.h"

#ifdef XIAOYA_OS
#include "parm_cache.h"
#include "player_manager.h"
#endif

//#ifdef BLE_CONFIG_SAMPLE
#define NETCONFIG_TIMEOUT    (90*1000)

#define str_begin_with(s, prefix)       (strstr(s, prefix) == s)
#define str_end_with(buf, len, ending)  (strstr(buf+len-1, ending) == buf+len-1)
static struct ble_session _ble_session = {0}, *ble_session = &_ble_session;
static rt_uint8_t ble_create_prf_ok = 0;
static rt_uint8_t ble_netconfig_state = 0;
static rt_thread_t tid = RT_NULL;


#define BK_ATT_DECL_PRIMARY_SERVICE_128     {0x00,0x28,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_DECL_CHARACTERISTIC_128      {0x03,0x28,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_DESC_CLIENT_CHAR_CFG_128     {0x02,0x29,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define WRITE_REQ_CHARACTERISTIC_128        {0xB0,0xBB,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0}
#define NOTIFY_CHARACTERISTIC_128           {0xB1,0xBB,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0}

static const uint8_t test_svc_uuid[16] = {0xAA,0xAA,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0};

static void station_connect(const char *ssid, const char *passwd)
{
    char argv[64];

    memset(argv, 0, sizeof(argv));
    sprintf(argv, "wifi %s join %s %s", "w0", ssid, passwd);
    msh_exec(argv, strlen(argv));
}

static int result_cb(char *ssid, char *password,char *ble_get_openid, void *user_data, void *userdata_len)
{
 
#ifdef XIAOYA_OS
    xiaoya_player_tips(TIP_FIND_AP_INFO,0);
    parm_set_wechat_openid_str((uint8_t *)ble_get_openid);
    sta_cfg_t sta_cfg; 
    memcpy(sta_cfg.ssid_str,ssid,strlen(ssid)+1);
    memcpy(sta_cfg.pwd_str,password,strlen(password)+1);
    parm_set_sta_cfg(&sta_cfg);
#endif

#ifdef AP_DIERCT_CONNECT_TEST
	extern direct_ap_info_t direct_ap_info;
	memcpy(direct_ap_info.direct_ssid,ssid,strlen(ssid)+1);
	memcpy(direct_ap_info.direct_pwd,password,strlen(password)+1);
#endif

    rt_kprintf("ssid:%s, password:%s,openid:%s\n", ssid, password,ble_get_openid);
    ble_netconfig_state=BLE_NETCONFIG_RECIVE_INFO;
    station_connect(ssid,password);
    return RT_EOK;
}

const bk_attm_desc_t netconfig_att_db[AAAAS_IDX_NB] =
{
    //  Service Declaration
    [AAAAS_IDX_SVC]                     =   {BK_ATT_DECL_PRIMARY_SERVICE_128,   BK_PERM_SET(RD, ENABLE), 0, 0},
    
    //  Level Characteristic Declaration
    [AAAAS_IDX_BBB0_VAL_CHAR]           =   {BK_ATT_DECL_CHARACTERISTIC_128,   BK_PERM_SET(RD, ENABLE), 0, 0},
    //  Level Characteristic Value
    [AAAAS_IDX_BBB0_VAL_VALUE]          =   {WRITE_REQ_CHARACTERISTIC_128,   BK_PERM_SET(WRITE_REQ, ENABLE),BK_PERM_SET(RI, ENABLE)|BK_PERM_SET(UUID_LEN, UUID_16),AAAA_CHAR_DATA_LEN},
    
    [AAAAS_IDX_BBB1_VAL_CHAR]           =   {BK_ATT_DECL_CHARACTERISTIC_128,   BK_PERM_SET(RD, ENABLE), 0, 0},
    //  Level Characteristic Value
    [AAAAS_IDX_BBB1_VAL_VALUE]          =   {NOTIFY_CHARACTERISTIC_128,   BK_PERM_SET(NTF, ENABLE)|BK_PERM_SET(RD, ENABLE),BK_PERM_SET(RI, ENABLE)|BK_PERM_SET(UUID_LEN, UUID_16),AAAA_CHAR_DATA_LEN},
    //  Level Characteristic - Client Characteristic Configuration Descriptor
    [AAAAS_IDX_BBB1_VAL_NTF_CFG]        =   {BK_ATT_DESC_CLIENT_CHAR_CFG_128,  BK_PERM_SET(RD, ENABLE)|BK_PERM_SET(WRITE_REQ, ENABLE),0,0},
};

static ble_err_t ble_create_db(void)
{
    ble_err_t status;
    struct bk_ble_db_cfg ble_db_cfg;

    ble_db_cfg.att_db = netconfig_att_db;
    ble_db_cfg.att_db_nb = AAAAS_IDX_NB;
    ble_db_cfg.prf_task_id = 0;
    ble_db_cfg.start_hdl = 0;
    ble_db_cfg.svc_perm = BK_PERM_SET(SVC_UUID_LEN, UUID_16);
    memcpy(&(ble_db_cfg.uuid[0]), &test_svc_uuid[0], 16);

    status = bk_ble_create_db(&ble_db_cfg);

    return status;
}

static void ble_session_dump(void)
{
    rt_kprintf("ble session len:%d\n", ble_session->len);
    rt_kprintf("ble session buf:%s\n",ble_session->response_buf);
    rt_kprintf("ble status:%d\n", ble_session->status);
}


static rt_err_t ble_push_data(uint8_t* buf, uint8_t len)
{
    //rt_kprintf("len:%d,buf:%s\r\n",len,buf);

    for(int i = 0; i< len; i++)
    {
        rt_kprintf("%c",buf[i]);
    }
    rt_kprintf("\r\n");
    if(ble_session->len + len > BLE_RESPONSE_LEN)
    {
        rt_kprintf("data len is too small\n");
        return -RT_ERROR;
    }
    memcpy(ble_session->response_buf + ble_session->len, buf, len);
    ble_session->len += len;

    return RT_EOK;
}

static void ble_clean_data(void)
{
    memset(ble_session->response_buf, 0x0, BLE_RESPONSE_LEN);
    ble_session->len = 0;
}

static void ble_write_callback(write_req_t *param)
{
    cJSON *root = RT_NULL;
    cJSON *ssid = RT_NULL;
    cJSON *password = RT_NULL;
    cJSON *openid= RT_NULL;
    if(AAAAS_IDX_BBB1_VAL_NTF_CFG==param->att_idx)
    {
        uint16_t ntf_cfg = (param->value[0]) | (param->value[1] << 8);
        if(ble_event_cb != NULL)
            ble_event_cb(BLE_CFG_NOTIFY, (void *)(&(ntf_cfg)));
        return;
    }

    if(ble_session->status == START)
    {
        ble_session->status = RECVING;
        ble_push_data(param->value, param->len);
        ble_session->tick = rt_tick_get();
        return;
    }
    else if(ble_session->status == RECVING)
    {
        /* timeout */
        if((rt_tick_get() - ble_session->tick) > BLE_TIMEOUT)
        {
            rt_kprintf("ble recv timeout\n");
            ble_clean_data();
            if(str_begin_with(param->value, "{") != RT_NULL)
            {
                ble_push_data(param->value, param->len);
                ble_session->tick = rt_tick_get();

            }
            else
            {
                 ble_session->status = START;
                 goto __restart;
            }
        }
        else
        {
            /* not timeout */
            ble_push_data(param->value, param->len);
            ble_session->tick = rt_tick_get();
        }
    }

    if(str_end_with(param->value, param->len, "}"))
    {
        root = cJSON_Parse(ble_session->response_buf);
        if(root == RT_NULL)
        {
            rt_kprintf("ble data parse failed\n");
            ble_session->status = START;
            goto __restart;
        }
        else
        {
            ssid = cJSON_GetObjectItem(root, "ssid");
            password = cJSON_GetObjectItem(root, "password");
            openid=cJSON_GetObjectItem(root, "openid");
            result_cb(ssid->valuestring, password->valuestring,openid->valuestring,RT_NULL, RT_NULL);
            ble_session->status = START;
            goto __restart;
        }
    }
    else
    {
        /* continue receive */
        ble_session->status = RECVING;
        goto __exit;
    }
    
__restart:
    if(root != RT_NULL)
    {
        cJSON_Delete(root);
        root = RT_NULL;
    }
    ble_clean_data();
__exit:
    return;    
}

static uint8_t ble_read_callback(read_req_t *param)
{
    rt_kprintf("prf_id:%d, att_idx:%d\r\n", param->prf_id, param->att_idx);
    param->value[0] = 0x31;
    param->value[1] = 0x32;
    param->value[2] = 0x33;
    return 3;
}
static void ble_event_callback(ble_event_t event, void *param)
{
    switch(event)
    {
       case BLE_STACK_OK:
        {
            rt_kprintf("STACK INIT OK\r\n");
            ble_create_db();
        }
        break;
        case BLE_STACK_FAIL:
            rt_kprintf("STACK INIT FAIL\r\n");
        break;
        case BLE_CREATE_DB_OK:
        {
            rt_kprintf("BLE_CREATE_DB_OK\r\n"); 
            ble_create_prf_ok=1;         
        }
        break;
        case BLE_CREATE_DB_FAIL:
            bk_printf("BLE_CREATE_DB_FAIL\r\n");         
        break;
        case BLE_CONNECT:
            rt_kprintf("BLE CONNECT\r\n");
        break;
        case BLE_DISCONNECT:
        {
            rt_kprintf("BLE DISCONNECT\r\n");
        }
        break;
        case BLE_MTU_CHANGE:
            rt_kprintf("BLE_MTU_CHANGE:%d\r\n", *(uint16_t *)param);
        break;
        case BLE_CFG_NOTIFY:
            rt_kprintf("BLE_CFG_NOTIFY:%d\r\n", *(uint16_t *)param);
        break;
        case BLE_CFG_INDICATE:
            rt_kprintf("BLE_CFG_INDICATE:%d\r\n", *(uint16_t *)param);
        break;
        case BLE_TX_DONE:
            rt_kprintf("BLE_TX_DONE\r\n");
        break;

        default:
            rt_kprintf("UNKNOW EVENT\r\n");
        break;
    }
}

static void ble_start(void)
{
    uint8_t adv_idx, adv_name_len;
    uint8_t mac[6];
    char ble_name[20];

    wifi_get_mac_address((char *)mac, CONFIG_ROLE_STA);
    adv_name_len = rt_snprintf(ble_name, sizeof(ble_name), "bk-%02x%02x", mac[4], mac[5]);

    memset(&adv_info, 0x00, sizeof(adv_info));

    adv_info.channel_map = 7;
    adv_info.interval_min = 160;
    adv_info.interval_max = 160;

    adv_idx = 0;
    adv_info.advData[adv_idx] = 0x02; adv_idx++;
    adv_info.advData[adv_idx] = 0x01; adv_idx++;
    adv_info.advData[adv_idx] = 0x06; adv_idx++;

    adv_info.advData[adv_idx] = adv_name_len + 1; adv_idx +=1;
    adv_info.advData[adv_idx] = 0x09; adv_idx +=1; //name
    memcpy(&adv_info.advData[adv_idx], ble_name, adv_name_len); adv_idx +=adv_name_len;

    adv_info.advDataLen = adv_idx;

    adv_idx = 0;

    adv_info.respData[adv_idx] = adv_name_len + 1; adv_idx +=1;
    adv_info.respData[adv_idx] = 0x08; adv_idx +=1; //name
    memcpy(&adv_info.respData[adv_idx], ble_name, adv_name_len); adv_idx +=adv_name_len;
    adv_info.respDataLen = adv_idx;
    
    if (ERR_SUCCESS != appm_start_advertising())
    {
        rt_kprintf("ERROR\r\n");
    }
}

static rt_err_t ble_netconfig_stop(void)
{
	appm_disconnect();
    rt_thread_delay(500);
    if(ERR_SUCCESS==appm_stop_advertising())
        rt_kprintf("appm_stop_advertising success\r\n");
    if(NULL!=ble_session->response_buf)
    {
        rt_free(ble_session->response_buf);
        ble_session->response_buf=NULL;
    }
    ble_stop();
    
    return RT_EOK;
}

rt_err_t bk_ble_netconfig_thread()
{
	extern rt_sem_t direct_conn_done_sem;
	if(direct_conn_done_sem != RT_NULL)
	{
	 	 rt_kprintf(" rt_sem_release direct_conn_done_sem =%d \n",rt_sem_release(direct_conn_done_sem));
	}
    int time=rt_tick_get();
    if(BLE_NETCONFIG_START==ble_netconfig_state)
    {
        rt_kprintf("ble is already init\n");
        return RT_EOK;
    }
    ble_netconfig_state=BLE_NETCONFIG_START;
    ble_session->response_buf = rt_malloc(1024);
    if(ble_session->response_buf == RT_NULL) 
    {
        rt_kprintf("malloc failed\n");
        return -RT_ENOMEM;
    }
    ble_clean_data();
    ble_session->len = 0;
    ble_session->tick = 0;
    ble_session->status = START;
    
    //ble activate
    
    ble_set_write_cb(ble_write_callback);
    ble_set_read_cb(ble_read_callback);
    ble_set_event_cb(ble_event_callback);
    ble_activate(NULL);
    gapm_set_max_mtu(AAAA_CHAR_DATA_LEN); 
    while(!ble_create_prf_ok)
    {
        rt_thread_delay(10);
    }
    //ble start advertise
    ble_start();
    while(1)
    {
        if(BLE_NETCONFIG_STOP==ble_netconfig_state)
            break;
    #ifdef XIAOYA_OS
        if(rt_tick_get()-time>=NETCONFIG_TIMEOUT)
        {
            rt_kprintf("[PLAYER] netconfig timeout\r\n");
            xiaoya_player_tips(TIP_NET_CONFIG_TIMEOUT,0);
            break;
        }
    #endif
        rt_thread_delay(10);
    }
_exit:
    tid=NULL;
   ble_netconfig_stop();
    return RT_EOK;
}


int bk_ble_netconfig_start()
{
    if (tid)
    {
        rt_kprintf("bk_ble_netconfig_start already init.\n");
        return -1;
    }

    tid = rt_thread_create("ble_netconfig",
                           bk_ble_netconfig_thread,
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
int bk_ble_netconfig_stop()
{
    ble_netconfig_state=BLE_NETCONFIG_STOP;
}
void ble_send_wifi_connected_to_master()
{
    if(BLE_NETCONFIG_RECIVE_INFO==get_ble_netconfig_state())
    {
        uint8_t write_buffer[20]="wifi connected";
        bk_ble_send_ntf_value(strlen(write_buffer), write_buffer, 0, 4);
        rt_thread_delay(500);
    }
}

int get_ble_netconfig_state(void)
{
    return ble_netconfig_state;
}
static void wifi_got_ip_cb()
{
    ble_send_wifi_connected_to_master();
    bk_ble_netconfig_stop();
}

static void ble_netconfig_sample()
{
   // net_set_sta_ipup_callback(wifi_got_ip_cb);
    bk_ble_netconfig_stop();
    bk_ble_netconfig_start();
}
MSH_CMD_EXPORT(ble_netconfig_sample,ble_netconfig_sample);
MSH_CMD_EXPORT(bk_ble_netconfig_start,bk_ble_netconfig_start);
MSH_CMD_EXPORT(bk_ble_netconfig_stop,bk_ble_netconfig_stop);
#endif
