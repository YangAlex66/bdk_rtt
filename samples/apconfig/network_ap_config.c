#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include <webnet.h>
#include <wn_module.h>
#include "wlan_cmd.h"

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/netif.h"
#include "lwip/dns.h"
#include "lwip/prot/dns.h"
#include "lwip/udp.h"
#include "netdb.h"
#include "samples_config.h"

#ifdef AP_CONFIG_SAMPLE
struct tcpclient_config{
   char host[32];
   int port;
};

typedef enum
{
    ACT_GET_SSIDLIST,
    ACT_SET_STA,
    ACT_GET_STA,
    ACT_SET_TCPSERVER,
    ACT_GET_TCPSERVER,
    ACT_SET_TCPCLIENT,
    ACT_GET_TCPCLIENT,
    ACT_DISCOVER_DEVICE,
    ACT_GET_IP_CFG,
    ACT_MES_MAX
}HTTP_ACTION_TYPE;

static rt_mq_t mq_httpserver_msg  = NULL;
static const char *recive_data;
static struct webnet_session* web_session;
static char *body=NULL; 
static rt_sem_t create_httpbody_sem;
static int tcpserver_port;
static struct tcpclient_config tcpclient_cfg;
static rt_bool_t tcpserver_stop = RT_TRUE; /* 停止标志 */
static rt_bool_t tcpclient_stop = RT_TRUE; /* 停止标志 */

static int httpserver_msg_send(HTTP_ACTION_TYPE msg)
{
    int ret = -1;
    
    if(mq_httpserver_msg != NULL)
    {   
        ret = rt_mq_send(mq_httpserver_msg, (void *)&msg, sizeof(uint8_t));
        if (ret != RT_EOK)
        {
            rt_kprintf("send httpserver msg failed \n");
        }
    }
    
    return ret;
}

static void action_get_ssidlist()
{
    cJSON *json = RT_NULL,*json_data = RT_NULL,*array=RT_NULL,*ssidinfo=RT_NULL,*ssidinfo1=RT_NULL;
    json = cJSON_CreateObject();
    if (!json)
        goto _exit;
    cJSON_AddStringToObject(json, "action", "getSSIDList");
    cJSON_AddStringToObject(json, "Msg", "success");
    array = cJSON_CreateArray();
    json_data = cJSON_CreateObject();
    ssidinfo = cJSON_CreateObject();
    ssidinfo1 = cJSON_CreateObject();
    if ((!json_data)||(!array)||(!ssidinfo))
        goto _exit;

    cJSON_AddStringToObject(ssidinfo, "ssid", "wifi1");
    cJSON_AddNumberToObject(ssidinfo, "auth",  0);
    cJSON_AddStringToObject(ssidinfo, "bssid", "01:02:03:04:05:06");
    cJSON_AddItemToArray(array,ssidinfo);

/*
    cJSON_UpdateStringToObject(ssidinfo, "ssid", "wifi2");
    cJSON_UpdateNumberToObject(ssidinfo, "auth",  0);
    cJSON_UpdateStringToObject(ssidinfo, "bssid", "01:02:03:04:05:06");
*/
    
    cJSON_AddStringToObject(ssidinfo1, "ssid", "wifi2");
    cJSON_AddNumberToObject(ssidinfo1, "auth",  0);
    cJSON_AddStringToObject(ssidinfo1, "bssid", "01:02:03:04:05:06");
    cJSON_AddItemToArray(array,ssidinfo1);

    cJSON_AddItemToObject(json_data, "data", array);
    cJSON_AddItemToObject(json, "data", json_data);

    body = cJSON_PrintUnformatted(json);
    printf("body:%s\r\n",body);
    rt_sem_release(create_httpbody_sem);

_exit:    
    if (json)
        cJSON_Delete(json);
}


static void action_set_sta()
{
    cJSON *root = RT_NULL,*json=RT_NULL,*data=RT_NULL,*ssid=RT_NULL,*psw=RT_NULL,*deviceName=RT_NULL,*SN=RT_NULL;
    root = cJSON_Parse(recive_data);
    if (!root)
        goto _exit;
    data = cJSON_GetObjectItem(root, "data");
    ssid = cJSON_GetObjectItem(data, "ssid");
    psw  = cJSON_GetObjectItem(data, "psw");
    deviceName= cJSON_GetObjectItem(data, "deviceName");
    SN = cJSON_GetObjectItem(data, "SN");
    rt_kprintf("ssid:%s,psw:%s,deviceName:%s,SN:%s\r\n",ssid->valuestring,psw->valuestring,deviceName->valuestring,SN->valuestring);
    demo_sta_app_init(ssid->valuestring,psw->valuestring);

    json = cJSON_CreateObject();
    if (!json)
        goto _exit;
    cJSON_AddStringToObject(json, "action", "setSTA");
    cJSON_AddStringToObject(json, "Msg", "success"); 
    body = cJSON_PrintUnformatted(json);
    rt_kprintf("body:%s\r\n",body);
    rt_sem_release(create_httpbody_sem);

_exit:    
    if (root)
        cJSON_Delete(root);
    if (json)
        cJSON_Delete(json);
}

static void action_get_sta()
{
    cJSON *json = RT_NULL,*json_data = RT_NULL;
    json = cJSON_CreateObject();
    if (!json)
        goto _exit;
    cJSON_AddStringToObject(json, "action", "getSTA");
    cJSON_AddStringToObject(json, "Msg", "success");

    json_data = cJSON_CreateObject();
    if (!json_data)
    {
         goto _exit;
    }
    cJSON_AddStringToObject(json_data, "ssid", "wifi_test");
    cJSON_AddStringToObject(json_data, "psw",  "12345678");
    cJSON_AddStringToObject(json_data, "deviceName", "XXXX");
    cJSON_AddStringToObject(json_data, "SN", "123456");
    cJSON_AddItemToObject(json, "data", json_data);
    body = cJSON_PrintUnformatted(json);
    rt_kprintf("body:%s\r\n",body);
    rt_sem_release(create_httpbody_sem);

_exit:    
    if (json)
        cJSON_Delete(json);
}


static void action_set_tcpserver()
{
    cJSON *root = RT_NULL,*data=RT_NULL,*port=RT_NULL,*json = RT_NULL;
    root = cJSON_Parse(recive_data);
    if (!root)
        goto _exit;
    data = cJSON_GetObjectItem(root, "data");
    port = cJSON_GetObjectItem(data, "port");
    rt_kprintf("tcp server port:%d\r\n",port->valueint);
    tcpserver_port=port->valueint;
    //crate tcp server threrad
    tcpserver_stop == RT_FALSE;

    json = cJSON_CreateObject();
    if (!json)
        goto _exit;
    cJSON_AddStringToObject(json, "action", "setTCPServer");
    cJSON_AddStringToObject(json, "Msg", "success"); 
    body = cJSON_PrintUnformatted(json);
    rt_kprintf("body:%s\r\n",body);
    rt_sem_release(create_httpbody_sem);

_exit:    
    if (root)
        cJSON_Delete(root);
    if (json)
        cJSON_Delete(json);
}

static void action_get_tcpserver()
{
    cJSON *json = RT_NULL,*json_data = RT_NULL;
    json = cJSON_CreateObject();
    if (!json)
        goto _exit;
    cJSON_AddStringToObject(json, "action", "getTCPServer");
    cJSON_AddStringToObject(json, "Msg", "success");

    json_data = cJSON_CreateObject();
    if (!json_data)
    {
         goto _exit;
    }
    cJSON_AddNumberToObject(json_data, "port", tcpserver_port);
    cJSON_AddItemToObject(json, "data", json_data);
    body = cJSON_PrintUnformatted(json);
    rt_kprintf("body:%s\r\n",body);
    rt_sem_release(create_httpbody_sem);

_exit:    
    if (json)
        cJSON_Delete(json);
}

static void action_set_tcpclient()
{
 
    cJSON *root = RT_NULL,*json = RT_NULL,*data=RT_NULL,*port=RT_NULL,*host=RT_NULL;
    root = cJSON_Parse(recive_data);
    if (!root)
        goto _exit;

    data = cJSON_GetObjectItem(root, "data");
    host = cJSON_GetObjectItem(data, "host");
    port = cJSON_GetObjectItem(data, "port");
    rt_kprintf("tcp server host:%s ,port:%d\r\n",host->valuestring,port->valueint);
    tcpclient_cfg.port=port->valueint;
    memcpy(tcpclient_cfg.host,host->valuestring,strlen(host->valuestring)+1);
    //crate tcp client threrad
    tcpclient_stop == RT_FALSE;

    json = cJSON_CreateObject();
    if (!json)
        goto _exit;
    cJSON_AddStringToObject(json, "action", "setTCPClient");
    cJSON_AddStringToObject(json, "Msg", "success"); 
    body = cJSON_PrintUnformatted(json);
    rt_kprintf("body:%s\r\n",body);
    rt_sem_release(create_httpbody_sem);

_exit:    
    if (root)
        cJSON_Delete(root);
    if (json)
        cJSON_Delete(json);
}

static void action_get_tcpclient()
{

    cJSON *json = RT_NULL,*json_data = RT_NULL;
    json = cJSON_CreateObject();
    if (!json)
        goto _exit;
    cJSON_AddStringToObject(json, "action", "getTCPClient");
    cJSON_AddStringToObject(json, "Msg", "success");

    json_data = cJSON_CreateObject();
    if (!json_data)
    {
         goto _exit;
    }
    cJSON_AddNumberToObject(json_data, "port", tcpclient_cfg.port);
    cJSON_AddStringToObject(json_data, "host", tcpclient_cfg.host);
    cJSON_AddItemToObject(json, "data", json_data);
    body = cJSON_PrintUnformatted(json);
    rt_kprintf("body:%s\r\n",body);
    rt_sem_release(create_httpbody_sem);

_exit:    
    if (json)
        cJSON_Delete(json);
}

static void action_discover_device()
{
    cJSON *json = RT_NULL,*json_data = RT_NULL;
    json = cJSON_CreateObject();
    if (!json)
        goto _exit;
    cJSON_AddStringToObject(json, "action", "discoveryDevice");
    cJSON_AddStringToObject(json, "Msg", "success");

    json_data = cJSON_CreateObject();
    if (!json_data)
    {
         goto _exit;
    }
    cJSON_AddStringToObject(json_data, "staMAC", "30:AE:A4:86:B1:6C");
    cJSON_AddStringToObject(json_data, "staIP", "192.168.234.133");
    cJSON_AddStringToObject(json_data, "devID", "30aea4a4b16c");
    cJSON_AddStringToObject(json_data, "staIP", "192.168.234.133");
    cJSON_AddStringToObject(json_data, "version", "1.0.1");
    cJSON_AddStringToObject(json_data, "deviceName", "XXXX");
    cJSON_AddStringToObject(json_data, "SN", "123456");
    cJSON_AddItemToObject(json, "data", json_data);
    body = cJSON_PrintUnformatted(json);
    rt_kprintf("body:%s\r\n",body);
    rt_sem_release(create_httpbody_sem);

_exit:    
    if (json)
        cJSON_Delete(json);
}


static void action_get_ip_cfg()
{
  
    cJSON *json = RT_NULL;
    struct netif *netif = netif_list;
    json = cJSON_CreateObject();
    if (!json)
        goto _exit;

    while (netif != RT_NULL)
    {
        cJSON *json_netif = 0;
        char str_buf[16]; /* 3*4+3+1 */

        json_netif = cJSON_CreateObject();
        if (!json_netif)
        {
            break;
        }

        rt_enter_critical();
        cJSON_AddStringToObject(json_netif, "ip", inet_ntoa(*((struct in_addr *) & (netif->ip_addr))));
        cJSON_AddStringToObject(json_netif, "netmask", inet_ntoa(*((struct in_addr *) & (netif->netmask))));
        cJSON_AddStringToObject(json_netif, "gateway", inet_ntoa(*((struct in_addr *) & (netif->gw))));
        rt_exit_critical();

        memcpy(str_buf, &netif->name[0], sizeof(netif->name));
        str_buf[sizeof(netif->name)] = '\0';
        cJSON_AddItemToObject(json, str_buf, json_netif);

        if (netif == netif_default)
        {
            cJSON_AddStringToObject(json, "default", str_buf);
        }

        netif = netif->next;
    } /**< while(netif != RT_NULL) */

#ifdef RT_LWIP_DNS
    {
        rt_ubase_t index;
        const ip_addr_t *ip_addr;
        extern const ip_addr_t *dns_getserver(u8_t numdns);

        char str_buf[16]; /* 3*4+3+1 */

        for (index = 0; index < DNS_MAX_SERVERS; index++)
        {
            sprintf(str_buf, "dns%d", index);

            ip_addr = dns_getserver(index);

            rt_enter_critical();
            cJSON_AddStringToObject(json, str_buf, ipaddr_ntoa(ip_addr));
            rt_exit_critical();
        }
    }
#endif /**< #ifdef RT_LWIP_DNS */

    body = cJSON_PrintUnformatted(json);
    rt_kprintf("body:%s\r\n",body);
    rt_sem_release(create_httpbody_sem);

_exit:
    if (json)
        cJSON_Delete(json);
    return;
}


static void httpserver_msg_thread_entry(void *parameter)
{
    rt_kprintf("httpserver_msg_thread_entry start!!!\n");
    uint8_t msg;
    while(1)
    {
        if (rt_mq_recv(mq_httpserver_msg, &msg, sizeof(uint8_t), RT_WAITING_FOREVER) == RT_EOK)
        {
            rt_kprintf("recive httpserver msg:%d\r\n",msg);
            switch (msg)
            {
            case ACT_GET_SSIDLIST:
                action_get_ssidlist();
                break;
            case ACT_SET_STA:
                action_set_sta();
                break;
            case ACT_GET_STA:
                action_get_sta();
                break;
            
            case ACT_SET_TCPSERVER:
                action_set_tcpserver();
                break;

            case ACT_GET_TCPSERVER:
                action_get_tcpserver();
                break;
            
            case ACT_SET_TCPCLIENT:
                action_set_tcpclient();
                break;

            case ACT_GET_TCPCLIENT:
                action_get_tcpclient();
                break;      
            
            case ACT_DISCOVER_DEVICE:
                action_discover_device();
                break;
            case ACT_GET_IP_CFG:
                action_get_ip_cfg();
                break;

            default:
                break;
            }
        }
    }
}

int httpserver_msg_init(void)
{
    mq_httpserver_msg = rt_mq_create("httpserver_msg", sizeof(rt_uint8_t), 6, RT_IPC_FLAG_FIFO);
    create_httpbody_sem = rt_sem_create("create_httpbody_sem", 0, RT_IPC_FLAG_FIFO);

    if (!create_httpbody_sem)
    {
        rt_kprintf("Create  create_httpbody_sem failed! \n");
        return -1;
    }

    if(NULL == mq_httpserver_msg)
    {
        return -1;
    }
    
    rt_thread_t tid = rt_thread_create("httpserver_msg",
                           httpserver_msg_thread_entry,
                           RT_NULL,
                           1024 * 3,
                           27,
                           9);
    if (tid != RT_NULL)
    {   
        rt_thread_startup(tid);
    }

    return 0;
}



static void cgi_web_ap_config_handler(struct webnet_session* session)
{
    char ssid[64]={0};
    char password[64]={0};
    
    const char* mimetype;
    struct webnet_request* request;
    static const char* header = "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\" /><title> ap_config </title></head>";
    static const char* body = "<body><center><form method=\"post\" action=\"/cgi-bin/web_ap_config\"><p>ssid:   <input type=\"text\" name=\"ssid\" value=\"%s\"></p><p>pass:  <input type=\"text\" name=\"password\" value=\"%s\"></p><input type=\"submit\" value=\"start netconfig\"></form></center></body></html>\r\n";

    RT_ASSERT(session != RT_NULL);
    request = session->request;
    RT_ASSERT(request != RT_NULL);

    /* get mimetype */
    mimetype = mime_get_type(".html");

    /* set http header */
    session->request->result_code = 200;
    webnet_session_set_header(session, mimetype, 200, "Ok", -1);

    webnet_session_write(session, (const rt_uint8_t*)header, rt_strlen(header));
    if (request->query_counter)
    {
        const char *ssid_value, *password_value;
        ssid_value = webnet_request_get_query(request, "ssid");
        password_value = webnet_request_get_query(request, "password");
        memcpy(ssid,ssid_value,strlen(ssid_value)+1);
        memcpy(password,password_value,strlen(password_value)+1);
        rt_kprintf("ssid:%s,%s,password:%s,%s\r\n",ssid_value,ssid,password_value,password);
        demo_sta_app_init(ssid,password);
    }

    webnet_session_printf(session, body, ssid, password);
    return;
}

static void cgi_json_ap_config_handler(struct webnet_session* session)
{
    cJSON *root = RT_NULL,*action = RT_NULL,*data=RT_NULL;
    const char* mimetype;
    struct webnet_request* request;
    static const char* error_message ="{\"Msg\": \"parameter error\"}";
    RT_ASSERT(session != RT_NULL);
    request = session->request;
    RT_ASSERT(session != RT_NULL);

    if (request->query_counter)
    { 
        recive_data=request->query;
       // recive_data=request->query_items[0].name;

        root = cJSON_Parse(recive_data);
        if (!root)
        {
            rt_kprintf("create cJSON root fail!\n");
            goto _exit;
        }

        action = cJSON_GetObjectItem(root, "action");
        rt_kprintf("\r\naction:%s\r\n",action->valuestring);
        if(strcmp(action->valuestring, "getSSIDList") == 0)
        {
            rt_kprintf("send getSSIDList event\r\n");
            httpserver_msg_send(ACT_GET_SSIDLIST);
        }
        else if(strcmp(action->valuestring, "setSTA") == 0)
        {  
            rt_kprintf("send setAT event\r\n");
            httpserver_msg_send(ACT_SET_STA);
        }   
        else if(strcmp(action->valuestring, "getSTA") == 0)
        {
            rt_kprintf("send getSTA event\r\n");
            httpserver_msg_send(ACT_GET_STA);
        }
        else if(strcmp(action->valuestring, "setTCPServer") == 0)
        {
            rt_kprintf("send setTCPServer event\r\n");
            httpserver_msg_send(ACT_SET_TCPSERVER);
        }
        else if(strcmp(action->valuestring, "getTCPServer") == 0)
        {
           rt_kprintf("send getTCPServer event\r\n"); 
           httpserver_msg_send(ACT_GET_TCPSERVER);
        }
        else if(strcmp(action->valuestring, "setTCPClient") == 0)
        {
            rt_kprintf("send setTCPClient event\r\n");
            httpserver_msg_send(ACT_SET_TCPCLIENT);
        }
        else if(strcmp(action->valuestring, "getTCPClient") == 0)
        {
            rt_kprintf("send getTCPClient event\r\n");
            httpserver_msg_send(ACT_GET_TCPCLIENT);
        }
        else if(strcmp(action->valuestring, "discoveryDevice") == 0)
        {
            rt_kprintf("send discoveryDevice event\r\n");
            httpserver_msg_send(ACT_DISCOVER_DEVICE);
        }
        else if(strcmp(action->valuestring, "get_ip_cfg") == 0)
        {
            rt_kprintf("send get_ip_cfg event\r\n");
            httpserver_msg_send(ACT_GET_IP_CFG);
        }
        else
        {
            rt_kprintf("Unsupported action\r\n");
            goto _exit;
        }
    }
    else 
    {
        rt_kprintf("no parameter\r\n");
        goto _exit;
    }


    if (rt_sem_take(create_httpbody_sem, rt_tick_from_millisecond(1000)) != RT_EOK)
    {
        rt_kprintf("Wait semaphore timeout \n");
        goto _exit;
    }

_exit:  
    if(body)
    {
        session->request->result_code = 200;
        webnet_session_set_header(session, "application/json", 200, "Ok", -1);
        webnet_session_printf(session, body);
    }
    else
    {
        //session->request->result_code = 400;
        webnet_session_set_header(session, "application/json", 400, "Ok", -1);
        webnet_session_printf(session,error_message);
        rt_kprintf("body null\r\n");
    }
    if (root != RT_NULL)
        cJSON_Delete(root);
    
    if(body)
    {
        free(body);
        body=NULL;
    }
    return;
}



void apconfig_test(void)
{
#ifdef WEBNET_USING_CGI
    webnet_cgi_register("web_ap_config", cgi_web_ap_config_handler);
    webnet_cgi_register("json_ap_config", cgi_json_ap_config_handler);
#endif
    httpserver_msg_init();
    webnet_init();
}


#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(apconfig_test, apconfig_test);
#endif

#endif