#include <rthw.h>
#include <rtthread.h>
#include "wlan_ui_pub.h"
#include <stdint.h>
#include <stdlib.h>
#include "test_config.h"
#include "wlan_dev.h"
#include "rtdef.h"
#include "direct_connect.h"
#include "easyflash.h"
#include "msh.h"
#include "net.h"

#define AP_DIERCT_CONNECT_TEST
#ifdef AP_DIERCT_CONNECT_TEST

direct_ap_info_t direct_ap_info;
rt_sem_t direct_conn_done_sem;



static int save_ap_info(void)
{
	int replace_times;
	replace_times=atoi(ef_get_env("replace_times"));
	dir_link_prf("\r\n-------------@@@@@@@@@replace_times=%d @@@@@@@@@@@-----------\r\n",replace_times);
	if(strcmp((char*)direct_ap_info.direct_ssid,"")==0)
	{
		dir_link_prf("\r\n--------@@@@@@@@ direct_ap_info.direct_ssid is null@@@@@@@@@-------\r\n");
		return 0;
	}

	if(strcmp((char*)direct_ap_info.direct_ssid,ef_get_env(DIERCT_CONNECT_SSID_1))==0) {
		ef_set_env(DIERCT_CONNECT_PWD_1, (char*)direct_ap_info.direct_pwd);
		dir_link_prf("-------------@@@@@@@@@direct_pwd_1    direct_ssid_1  @@@@@@@@@@@-----------\r\n");
	} else if(strcmp((char*)direct_ap_info.direct_ssid,ef_get_env(DIERCT_CONNECT_SSID_2))==0) {
		ef_set_env(DIERCT_CONNECT_PWD_2, (char*)direct_ap_info.direct_pwd);
		dir_link_prf("-------------@@@@@@@@@direct_pwd_2    direct_ssid_2 @@@@@@@@@@@-----------\r\n");
	} else if(strcmp((char*)direct_ap_info.direct_ssid,ef_get_env(DIERCT_CONNECT_SSID_3))==0) {
		ef_set_env(DIERCT_CONNECT_PWD_3, (char*)direct_ap_info.direct_pwd);
		dir_link_prf("-------------@@@@@@@@@direct_pwd_3    direct_ssid_3  @@@@@@@@@@@-----------\r\n");
	} else if(strcmp((char*)direct_ap_info.direct_ssid,ef_get_env(DIERCT_CONNECT_SSID_4))==0) {
		ef_set_env(DIERCT_CONNECT_PWD_4, (char*)direct_ap_info.direct_pwd);
		dir_link_prf("-------------@@@@@@@@@direct_pwd_4    direct_ssid_4  @@@@@@@@@@@-----------\r\n");
	} else if(strcmp((char*)direct_ap_info.direct_ssid,ef_get_env(DIERCT_CONNECT_SSID_5))==0) {
		ef_set_env(DIERCT_CONNECT_PWD_5, (char*)direct_ap_info.direct_pwd);
		dir_link_prf("-------------@@@@@@@@@direct_pwd_5    direct_ssid_5  @@@@@@@@@@@-----------\r\n");
	} else {
		switch (replace_times){
		case 0:
		{
			ef_set_env(DIERCT_CONNECT_SSID_1, (char*)direct_ap_info.direct_ssid);
			ef_set_env(DIERCT_CONNECT_PWD_1, (char*)direct_ap_info.direct_pwd);
			ef_set_env("replace_times", "1");
			break;
		}
		case 1:
		{
			ef_set_env(DIERCT_CONNECT_SSID_2, (char*)direct_ap_info.direct_ssid);
			ef_set_env(DIERCT_CONNECT_PWD_2, (char*)direct_ap_info.direct_pwd);
			ef_set_env("replace_times", "2");
			break;
		}
		case 2:
		{
			ef_set_env(DIERCT_CONNECT_SSID_3, (char*)direct_ap_info.direct_ssid);
			ef_set_env(DIERCT_CONNECT_PWD_3, (char*)direct_ap_info.direct_pwd);
			ef_set_env("replace_times", "3");
			break;
		}
		case 3:
		{
			ef_set_env(DIERCT_CONNECT_SSID_4, (char*)direct_ap_info.direct_ssid);
			ef_set_env(DIERCT_CONNECT_PWD_4, (char*)direct_ap_info.direct_pwd);
			ef_set_env("replace_times", "4");
			break;
		}
		case 4:
		{
			ef_set_env(DIERCT_CONNECT_SSID_5, (char*)direct_ap_info.direct_ssid);
			ef_set_env(DIERCT_CONNECT_PWD_5, (char*)direct_ap_info.direct_pwd);
			ef_set_env("replace_times", "0");
			break;
		}
		}
	}

	ef_save_env();
	return 0;
}

static int wifi_got_ip_cb(void)
{
	rt_sem_release(direct_conn_done_sem);
	dir_link_prf("-------------@@@@@@@@@wifi_got_ip_cb@@@@@@@@@@@-----------\r\n");
	save_ap_info();	
	return 0;
}

static void station_connect(char *ssid, char *passwd)
{
    char argv[64];
    memset(argv, 0, sizeof(argv));
	bk_printf("wifi join   %s     %s\r\n",  ssid, passwd);
    sprintf(argv, "wifi %s join %s %s", "w0", ssid, passwd);
    msh_exec(argv, strlen(argv));
}

static void direct_connect_thread(void *arg)
{
	int poll_times=0;

	__maybe_unused_var(arg);
	memset(&direct_ap_info, 0, sizeof(direct_ap_info_t));
	dir_link_prf("\r\n-------direct_ap_info.direct_ssid=%s------\r\n",direct_ap_info.direct_ssid);

	if (!direct_conn_done_sem) {
		dir_link_prf("Create direct_connect done sem failed! \n");
		goto _exit;
	}

	if(ef_get_env(DIERCT_CONNECT_SSID_1)==NULL) {//默认第一个信息为空时不连接,注意手动删除后要进行添加
		dir_link_prf("ap_info is null! \n");
		goto _exit;
	}

	do {
		switch (poll_times){
		case 0:
		{
			dir_link_prf("@@@@@@@@@@@@@@ poll_times=%d \r\n",poll_times);
			station_connect(ef_get_env(DIERCT_CONNECT_SSID_1),ef_get_env(DIERCT_CONNECT_PWD_1));
			break;
		}
		case 1:
		{
			dir_link_prf("@@@@@@@@@@@@@@ poll_times=%d \r\n",poll_times);
			station_connect(ef_get_env(DIERCT_CONNECT_SSID_2),ef_get_env(DIERCT_CONNECT_PWD_2));
			break;
		}
		case 2:
		{
			dir_link_prf("@@@@@@@@@@@@@@ poll_times=%d \r\n",poll_times);
			station_connect(ef_get_env(DIERCT_CONNECT_SSID_3),ef_get_env(DIERCT_CONNECT_PWD_3));
			break;
		}
		case 3:
		{
			dir_link_prf("@@@@@@@@@@@@@@ poll_times=%d \r\n",poll_times);
			station_connect(ef_get_env(DIERCT_CONNECT_SSID_4),ef_get_env(DIERCT_CONNECT_PWD_4));
			break;
		}
		case 4:
		{
			dir_link_prf("@@@@@@@@@@@@@@ poll_times=%d \r\n",poll_times);
			station_connect(ef_get_env(DIERCT_CONNECT_SSID_5),ef_get_env(DIERCT_CONNECT_PWD_5));
			break;
		}
		}
		poll_times++;
		if(poll_times>4)poll_times=0;
	} while (rt_sem_take(direct_conn_done_sem, rt_tick_from_millisecond(POLLING_TIMEOUT))!=RT_EOK);

_exit:
	if (direct_conn_done_sem) {
		rt_sem_delete(direct_conn_done_sem);
		direct_conn_done_sem = RT_NULL;
	}
	return;
}

int direct_connect_init(void)
{  
	rt_thread_t tid;
	easyflash_init();
	net_set_sta_ipup_callback(wifi_got_ip_cb);
	
    direct_conn_done_sem = rt_sem_create("directlink", 0, RT_IPC_FLAG_FIFO);
	
    tid = rt_thread_create("direct_connect", direct_connect_thread, RT_NULL, 1024 * 4, 22, 10);
    if (RT_NULL != tid)
    {
        rt_thread_startup(tid);
    }
	 return RT_EOK;
}


MSH_CMD_EXPORT(direct_connect_init, direct_connect_init);
#endif

