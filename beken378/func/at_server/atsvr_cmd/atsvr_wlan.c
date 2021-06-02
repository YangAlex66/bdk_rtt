#include "atsvr_wlan.h"
#include "atsvr_core.h"
#include "at_server.h"
#include "atsvr_cmd.h"
#include "at_server.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "include.h"
#include "wlan_ui_pub.h"
#include "rw_msg_pub.h"
#include "mem_pub.h"
#include "rtos_pub.h"
#include "generic.h"
#include "param_config.h"

typedef struct{
	unsigned dhcp : 1;
	unsigned station_status : 1;
#define STATION_UP     1
#define STATION_DOWN   0
	unsigned softap_status : 1;
#define SOFTAP_UP     1
#define SOFTAP_DOWN   0

	unsigned char static_ip[16];
	unsigned char static_maks[16];
	unsigned char static_gate[16];
	unsigned char static_dns1[16];
	unsigned char static_dns2[16];

	unsigned char softap_ip[16];
	unsigned char softap_mask[16];
	unsigned char softap_gate[16];
	unsigned char softap_dns[16];
}atsvr_wlan_t;

atsvr_wlan_t atsvr_wlan = {
	.dhcp = ATSVR_WLAN_STA_DHCP,
	.station_status = STATION_DOWN,
	.softap_status = SOFTAP_DOWN,
	.static_ip = ATSVR_WLAN_STA_DEFAULT_IP,
	.static_maks = ATSVR_WLAN_STA_DEFAULT_MASK,
	.static_gate = ATSVR_WLAN_STA_DEFAULT_GW,
	.static_dns1 = ATSVR_WLAN_STA_DEFAULT_DNS,
	.static_dns2 = ATSVR_WLAN_STA_DEFAULT_DNS,

	.softap_ip = ATSVR_WLAN_DEFAULT_IP,
	.softap_mask = ATSVR_WLAN_DEFAULT_MASK,
	.softap_gate = ATSVR_WLAN_DEFAULT_GW,
	.softap_dns = ATSVR_WLAN_DEFAULT_DNS,
};



void wlan_get_station_mac_address(char *mac)
{
	wifi_get_mac_address((char *)mac, CONFIG_ROLE_STA);
}

void wlan_get_softap_mac_address(char *mac)
{
	wifi_get_mac_address((char *)mac, CONFIG_ROLE_AP);
}

int wlan_start_station_connect(char *my_ssid,char* connect_key)
{
	network_InitTypeDef_st wNetConfig;
	int dhcp_sta;
	unsigned char *bssid = NULL;

	memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));
	dhcp_sta = wlan_get_station_dhcp();

	memcpy((char *)wNetConfig.wifi_ssid, my_ssid,32);
	wNetConfig.wifi_ssid[32] = '\0';
	memcpy((char *)wNetConfig.wifi_key, connect_key,64);
	wNetConfig.wifi_mode = BK_STATION;
	if(dhcp_sta){
		wNetConfig.dhcp_mode = DHCP_CLIENT;
	}else{
		ATSVRLOG("DHCP Static ip:%s,maks:%s,gate:%s,dns1:%s,dns2:%s\r\n",
			atsvr_wlan.static_ip,atsvr_wlan.static_maks, atsvr_wlan.static_gate, 
			atsvr_wlan.static_dns1, atsvr_wlan.static_dns2);
		wNetConfig.dhcp_mode = DHCP_DISABLE;
		strncpy((char*)wNetConfig.local_ip_addr, (char *)atsvr_wlan.static_ip,16);
		strncpy((char*)wNetConfig.net_mask, (char *)atsvr_wlan.static_maks,16);
		strncpy((char*)wNetConfig.gateway_ip_addr, (char *)atsvr_wlan.static_gate,16);
		strncpy((char*)wNetConfig.dns_server_ip_addr, (char *)atsvr_wlan.static_dns1,16);
		///strncpy((char*)wNetConfig.dns2_server_ip_addr, (char *)atsvr_wlan.static_dns2,16);
	}
	wNetConfig.wifi_retry_interval = 100;
	if ( bssid != NULL){
		memcpy(wNetConfig.wifi_bssid, bssid, 6);
	}

	ATSVRLOG("atsvr_ssid:%s atsvr_key:%s\r\n", wNetConfig.wifi_ssid, wNetConfig.wifi_key);
	if(bk_wlan_start(&wNetConfig) != 0){
		return -1;
	}

	return 0;
}

int wlan_stop_station(void)
{
	bk_wlan_stop(BK_STATION);
	return 0;
}

void wlan_set_station_dhcp(int en)
{
	atsvr_wlan.dhcp = ( en ) ? 1 : 0;
}

int wlan_get_station_dhcp(void)
{
	return ( atsvr_wlan.dhcp != 0 ) ? 1 : 0;
}

int wlan_set_station_static_ip(char *ip,char *mask,char *gate,char *dns,char *dns2)
{
	strncpy((char *)atsvr_wlan.static_ip,ip,16);
    strncpy((char *)atsvr_wlan.static_maks,mask,16);
    strncpy((char *)atsvr_wlan.static_gate,gate,16);
	if( dns != NULL){
		strncpy((char *)atsvr_wlan.static_dns1,dns,16);
	}
	if( dns != NULL){
		strncpy((char *)atsvr_wlan.static_dns2,dns2,16);
	}
	return 0;
}

int wlan_set_station_dns(char *dns_ip,char *dns2_ip)
{
	if( dns_ip != NULL ) {
		strncpy((char *)atsvr_wlan.static_dns1,dns_ip,16);
	}
	if( dns2_ip != NULL ) {
		strncpy((char *)atsvr_wlan.static_dns2,dns2_ip,16);
	}
	return 0;
}

int wlan_get_station_cur_status(void)
{
	return (atsvr_wlan.station_status == STATION_UP) ? 1 : 0;
}

int wlan_softap_start(char *ap_ssid, char *ap_key)
{
    network_InitTypeDef_st wNetConfig;
    int len;

    len = strlen(ap_ssid);
    if(ATSVR_MAX_SSID_LEN < len) {
        ATSVRLOG("ssid name more than 32 Bytes\r\n");
        return -1;
    }

	memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));
    strncpy((char *)wNetConfig.wifi_ssid, ap_ssid,33);
    strncpy((char *)wNetConfig.wifi_key, ap_key,64);

    wNetConfig.wifi_mode = BK_SOFT_AP;
    wNetConfig.dhcp_mode = DHCP_SERVER;
    wNetConfig.wifi_retry_interval = 100;
    strncpy((char *)wNetConfig.local_ip_addr, (char *)atsvr_wlan.softap_ip,16);
    strncpy((char *)wNetConfig.net_mask, (char *)atsvr_wlan.softap_mask,16);
    strncpy((char *)wNetConfig.gateway_ip_addr, (char *)atsvr_wlan.softap_gate,16);
    strncpy((char *)wNetConfig.dns_server_ip_addr, (char *)atsvr_wlan.softap_dns,16);

    ATSVRLOG("softap-ssid:%s  |  key:%s\r\n", wNetConfig.wifi_ssid, wNetConfig.wifi_key);
	bk_wlan_start(&wNetConfig);
	return 0;
}

int wlan_stop_softap(void)
{
	bk_wlan_stop(BK_SOFT_AP);
	return 0;
}

int wlan_set_softap_static_ip(char *ip,char *mask,char *gate,char *dns_ip)
{
	strncpy((char *)atsvr_wlan.softap_ip,ip,16);
    strncpy((char *)atsvr_wlan.softap_mask,mask,16);
    strncpy((char *)atsvr_wlan.softap_gate,gate,16);
	if(dns_ip  != NULL){
		strncpy((char *)atsvr_wlan.softap_dns,dns_ip,16);
	}
	return 0;
}

int wlan_set_softap_dns(char *dns_ip)
{
	strncpy((char *)atsvr_wlan.softap_dns,dns_ip,16);
	return 0;
}

int wlan_get_softap_cur_status(void)
{
	return (atsvr_wlan.softap_status == SOFTAP_UP) ? 1 : 0;
}

atsvr_wlan_sec_type wlan2atsvr_sec_type(int sec_type)
{
	switch(sec_type){
	case BK_SECURITY_TYPE_NONE:
		return ATSVR_SEC_TYPE_NONE;
	case BK_SECURITY_TYPE_WEP:
		return ATSVR_SEC_TYPE_WEP;
	case BK_SECURITY_TYPE_WPA_TKIP:
		return ATSVR_SEC_TYPE_WPA_TKIP;
	case BK_SECURITY_TYPE_WPA_AES:
		return ATSVR_SEC_TYPE_WPA_AES;
	case BK_SECURITY_TYPE_WPA2_TKIP:
		return ATSVR_SEC_TYPE_WPA2_TKIP;
	case BK_SECURITY_TYPE_WPA2_AES:
		return ATSVR_SEC_TYPE_WPA2_AES;
	case BK_SECURITY_TYPE_WPA2_MIXED:
		return ATSVR_SEC_TYPE_WPA2_MIXED;
	case BK_SECURITY_TYPE_WPA3_SAE:
		return ATSVR_SEC_TYPE_WPA3_SAE;
	case BK_SECURITY_TYPE_WPA3_WPA2_MIXED:
		return ATSVR_SEC_TYPE_WPA3_WPA2_MIXED;
	case BK_SECURITY_TYPE_AUTO:
		return ATSVR_SEC_TYPE_AUTO;
	}
	return ATSVR_SEC_TYPE_AUTO;
}

int atsvr2wlan_sec_type(atsvr_wlan_sec_type sec_type)
{
	switch(sec_type){
	case ATSVR_SEC_TYPE_NONE:
		return BK_SECURITY_TYPE_NONE;
	case ATSVR_SEC_TYPE_WEP:
		return BK_SECURITY_TYPE_WEP;
	case ATSVR_SEC_TYPE_WPA_TKIP:
		return BK_SECURITY_TYPE_WPA_TKIP;
	case ATSVR_SEC_TYPE_WPA_AES:
		return BK_SECURITY_TYPE_WPA_AES;
	case ATSVR_SEC_TYPE_WPA2_TKIP:
		return BK_SECURITY_TYPE_WPA2_TKIP;
	case ATSVR_SEC_TYPE_WPA2_AES:
		return BK_SECURITY_TYPE_WPA2_AES;
	case ATSVR_SEC_TYPE_WPA2_MIXED:
		return BK_SECURITY_TYPE_WPA2_MIXED;
	case ATSVR_SEC_TYPE_WPA3_SAE:
		return BK_SECURITY_TYPE_WPA3_SAE;
	case ATSVR_SEC_TYPE_WPA3_WPA2_MIXED:
		return BK_SECURITY_TYPE_WPA3_WPA2_MIXED;
	case ATSVR_SEC_TYPE_AUTO:
		return BK_SECURITY_TYPE_AUTO;
	default:
		break;
	}
	return BK_SECURITY_TYPE_AUTO;
}


static beken_semaphore_t atsvr_scan_sema = NULL;

static void atsvr_wlan_scan_callback(void *ctxt, unsigned char param)
{
	if (atsvr_scan_sema) {
		rtos_set_semaphore( &atsvr_scan_sema );
	}
}

int atsvr_wlan_scan_ap_sorting(int ap_num,struct sta_scan_res *table,char **out)
{
	char *array = NULL;
	char t_rssi[64][2];
	char temp[2];
	unsigned char i,j;

	if(out == NULL){
		return -1;
	}
	*out = NULL;
	array = at_malloc(ap_num);
	if(array == NULL){
		return -1;
	}
	for(i=0;i<ap_num;i++) {
		t_rssi[i][0] = i;
		t_rssi[i][1] = table[i].level;
	}

	for(i=0;i<ap_num;i++){
		for(j=i;j<ap_num;j++){
			if((char)t_rssi[j][1] > (char)t_rssi[i][1]){
				temp[0] = t_rssi[i][0];
				temp[1] = t_rssi[i][1];
				t_rssi[i][0] = t_rssi[j][0];
				t_rssi[i][1] = t_rssi[j][1];
				t_rssi[j][0] = temp[0];
				t_rssi[j][1] = temp[1];
			}
		}
	}

	for(i = 0; i < ap_num; i++){
		array[i] = t_rssi[i][0];
	}
	*out = array;
	return 0;
}

static int atsvr_wlan_scan_find_ssid_len(unsigned char *ssid)
{
	int i;

	for(i = 0;i < 32; i++){
		if(ssid[i] == '\0'){
			break;
		}
	}
	return i;
}

int atsvr_wlan_scan_ap_result(void)
{
	char *resultbuf  = NULL;
	int i;
	int n = 0;
#define ATSVR_WLAN_SCAN_BUF_SIZE     (5 * 1024)
	resultbuf = at_malloc(ATSVR_WLAN_SCAN_BUF_SIZE);
	if(resultbuf == NULL) {
		ATSVRLOG("resultbuf malloc failed!\r\n");
		return 1;
	}
#if (defined(CFG_WPA_CTRL_IFACE) && !CFG_WPA_CTRL_IFACE)
	struct sta_scan_res *at_scan_rst_table = NULL;
	char scan_rst_ap_num = 0;       /**< The number of access points found in scanning. */
	char filte_scan_rst_ap_num = 0;
	unsigned char index;
	char *sorting_array = NULL;
	char temp[150];

	scan_rst_ap_num = bk_wlan_get_scan_ap_result_numbers();
	if(scan_rst_ap_num == 0) {
		ATSVRLOG("atsvr_wlan_scan no ap\r\n");
		return 0;
	}

	at_scan_rst_table = (struct sta_scan_res *)at_malloc(sizeof(struct sta_scan_res) * scan_rst_ap_num);
	if(at_scan_rst_table == NULL) {
		ATSVRLOG("at_scan_rst_table malloc failed!\r\n");
		goto exit;
	}

	bk_wlan_get_scan_ap_result(at_scan_rst_table,scan_rst_ap_num);

	filte_scan_rst_ap_num = atsvr_wlan_scan_ap_sorting(scan_rst_ap_num,at_scan_rst_table,&sorting_array);
	if(filte_scan_rst_ap_num < 0) {
		ATSVRLOG("sorting_array malloc failed!\r\n");
		goto exit;
	}

	if (!scan_rst_ap_num){
		ATSVRLOG("%s ap num = 0\r\n", __func__);
	}
	for( i = 0; i < scan_rst_ap_num; i++ ) {
		index = sorting_array[i];
		if(at_scan_rst_table[index].ssid[0] == 0x00) {
			ATSVRLOG("filter ssid:%s,mac %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				at_scan_rst_table[index].ssid,at_scan_rst_table[index].bssid[0],at_scan_rst_table[index].bssid[1],
				at_scan_rst_table[index].bssid[2],at_scan_rst_table[index].bssid[3],
				at_scan_rst_table[index].bssid[4],at_scan_rst_table[index].bssid[5]);
			continue;
		}
		if(atsvr_wlan_scan_find_ssid_len((unsigned char*)at_scan_rst_table[index].ssid) >= 32){
			memcpy(temp,at_scan_rst_table[index].ssid,32);
			temp[32] = '\0';
			n += sprintf(resultbuf+n,ATSVR_CMDRSP_HEAD"+WSCAN:%s,",temp);
		}else{
			n += sprintf(resultbuf+n,"+WSCAN:%s,",at_scan_rst_table[index].ssid);
		}
		switch(at_scan_rst_table[index].security) {
		case BK_SECURITY_TYPE_NONE:
			n += sprintf(resultbuf+n,"%s","OPEN");
			break;
		case BK_SECURITY_TYPE_WEP:
			n += sprintf(resultbuf+n,"%s","WEP_SHARED");
			break;
		case BK_SECURITY_TYPE_WPA_TKIP:
			n += sprintf(resultbuf+n,"%s","WPA_TKIP_PSK");
			break;
		case BK_SECURITY_TYPE_WPA_AES:
			n += sprintf(resultbuf+n,"%s","WPA_AES_PSK");
			break;
		case BK_SECURITY_TYPE_WPA2_TKIP:
			n += sprintf(resultbuf+n,"%s","WPA2_TKIP_PSK");
			break;
		case BK_SECURITY_TYPE_WPA2_AES:
			n += sprintf(resultbuf+n,"%s","WPA2_AES_PSK");
			break;
		case BK_SECURITY_TYPE_WPA2_MIXED:
			n += sprintf(resultbuf+n,"%s","WPA2_MIXED_PSK");
			break;
		case BK_SECURITY_TYPE_WPA3_SAE:
			n += sprintf(resultbuf+n,"%s","WPA3_SAE");
			break;
		case BK_SECURITY_TYPE_WPA3_WPA2_MIXED:
			n += sprintf(resultbuf+n,"%s","WPA3_WPA2_MIXED");
			break;
		case BK_SECURITY_TYPE_AUTO:
			n += sprintf(resultbuf+n,"%s","AUTO_PSK");
			break;
		default:
			n += sprintf(resultbuf+n,"%s","UNKNOWN");
			break;
		}
        n += sprintf(resultbuf+n,",%d",ATSVR_RSSI2APPOWER(at_scan_rst_table[index].level)); ///RSSI
        n += sprintf(resultbuf+n,","ATSVR_MACSTR,at_scan_rst_table[index].bssid[0],
			        at_scan_rst_table[index].bssid[1],at_scan_rst_table[index].bssid[2],
			        at_scan_rst_table[index].bssid[3],at_scan_rst_table[index].bssid[4],
			        at_scan_rst_table[index].bssid[5]);
        n += sprintf(resultbuf+n,",%d\r\n",at_scan_rst_table[index].channel);
    }

exit:
	atsvr_output_msg(resultbuf,strlen(resultbuf));
	if( at_scan_rst_table != NULL ) {
		at_free(at_scan_rst_table);
		at_scan_rst_table = NULL;
	}

	if(sorting_array) {
		at_free(sorting_array);
		sorting_array = NULL;
	}
    return 0;
#elif (defined(CFG_WPA_CTRL_IFACE) && CFG_WPA_CTRL_IFACE)	/* CFG_WPA_CTRL_IFACE */
		ScanResult_adv apList;
		if (wlan_sta_scan_result(&apList) == 0) {
			int ap_num = apList.ApNum;

			bk_printf("Got ap count: %d\r\n", apList.ApNum);
			resultbuf[0] = '\0';
			for (i = 0; i < ap_num; i++) {
				if(atsvr_wlan_scan_find_ssid_len((unsigned char*)apList.ApList[i].ssid) >= 32){
					char temp[33];
					memcpy(temp,apList.ApList[i].ssid,32);
					temp[32] = '\0';
					n += sprintf(resultbuf+n,ATSVR_CMDRSP_HEAD"+WSCAN:%s,",temp);
				}else{
					n += sprintf(resultbuf+n,ATSVR_CMDRSP_HEAD"+WSCAN:%s,",apList.ApList[i].ssid);
				}
				switch(apList.ApList[i].security) {
				case BK_SECURITY_TYPE_NONE:
					n += sprintf(resultbuf+n,"%s","OPEN");
					break;
				case BK_SECURITY_TYPE_WEP:
					n += sprintf(resultbuf+n,"%s","WEP_SHARED");
					break;
				case BK_SECURITY_TYPE_WPA_TKIP:
					n += sprintf(resultbuf+n,"%s","WPA_TKIP_PSK");
					break;
				case BK_SECURITY_TYPE_WPA_AES:
					n += sprintf(resultbuf+n,"%s","WPA_AES_PSK");
					break;
				case BK_SECURITY_TYPE_WPA2_TKIP:
					n += sprintf(resultbuf+n,"%s","WPA2_TKIP_PSK");
					break;
				case BK_SECURITY_TYPE_WPA2_AES:
					n += sprintf(resultbuf+n,"%s","WPA2_AES_PSK");
					break;
				case BK_SECURITY_TYPE_WPA2_MIXED:
					n += sprintf(resultbuf+n,"%s","WPA2_MIXED_PSK");
					break;
				case BK_SECURITY_TYPE_WPA3_SAE:
					n += sprintf(resultbuf+n,"%s","WPA3_SAE");
					break;
				case BK_SECURITY_TYPE_WPA3_WPA2_MIXED:
					n += sprintf(resultbuf+n,"%s","WPA3_WPA2_MIXED");
					break;
				case BK_SECURITY_TYPE_AUTO:
					n += sprintf(resultbuf+n,"%s","AUTO_PSK");
					break;
				default:
					n += sprintf(resultbuf+n,"%s","UNKNOWN");
					break;
				}
				n += sprintf(resultbuf+n,",%d",ATSVR_RSSI2APPOWER(apList.ApList[i].ApPower)); ///RSSI
				n += sprintf(resultbuf+n,","ATSVR_MACSTR,apList.ApList[i].bssid[0],
								apList.ApList[i].bssid[1],apList.ApList[i].bssid[2],
								apList.ApList[i].bssid[3],apList.ApList[i].bssid[4],
								apList.ApList[i].bssid[5]);
				n += sprintf(resultbuf+n,",%d\r\n",apList.ApList[i].channel);
				if(n > (ATSVR_WLAN_SCAN_BUF_SIZE - 512)){
					atsvr_output_msg(resultbuf,strlen(resultbuf));
					resultbuf[0] = '\0';
					n = 0;
				}
			}
			os_free(apList.ApList);
			atsvr_output_msg(resultbuf,strlen(resultbuf));
		}
#endif /* CFG_WPA_CTRL_IFACE */

	if(resultbuf) {
		at_free(resultbuf);
		resultbuf = NULL;
	}
	return 0;
}

int wlan_scan_start(char *ssid)
{
	int err;

	if( atsvr_scan_sema == NULL ) {
        err = rtos_init_semaphore( &atsvr_scan_sema, 1 );
		if(err != kNoErr){
			return -1;
		}
    }
	extern void mhdr_scanu_reg_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt);
	mhdr_scanu_reg_cb(atsvr_wlan_scan_callback, 0);
	bk_wlan_start_scan();
	if(atsvr_scan_sema != NULL) {
		err = rtos_get_semaphore(&atsvr_scan_sema, ATSVR_SCAN_TIMEOUT_MS);
		if(err != kNoErr) {
			ATSVRLOG("atsvr scan wait semaphore error!\r\n");
		} else {
			ATSVRLOG("atsvr scan wait semaphore error!\r\n");
		}
	} else {
		ATSVRLOG("atsvr waited semaphore!\r\n");
		return -1;
	}
	if(atsvr_wlan_scan_ap_result() != 0){
		return -1;
	}


	return 0;
}


int wlan_event_handler(int event)
{
	int ret = 0;
	switch(event){
	case ATSVR_WLAN_DISCONNECT:
		atsvr_wlan.station_status = STATION_DOWN;
		ATSVR_SIZEOF_OUTPUT_STRRING(ATSVR_EVT_WLAN_DISCONNECTED);
		break;
	case ATSVR_WLAN_CONNECTTED:
		atsvr_wlan.station_status = STATION_UP;
		ATSVR_SIZEOF_OUTPUT_STRRING(ATSVR_EVT_WLAN_CONNECTED);
		break;
	case ATSVR_WLAN_GOT_IP:
		ATSVR_SIZEOF_OUTPUT_STRRING(ATSVR_EVT_GOT_IP);
		break;
	default:
		ret = -1;
		break;
	}
	return ret;
}


static void wlan_status_callback(void *ctxt)
{
	int notice_event = *(unsigned int*)ctxt;

	ATSVRLOG("[WLAN]event(%x)\r\n",notice_event);
	switch(notice_event){
	case RW_EVT_STA_GOT_IP:
		atsvr_event_sender(ATSVR_WLAN_GOT_IP);
		break;
	case RW_EVT_STA_CONNECTED:
		atsvr_event_sender(ATSVR_WLAN_CONNECTTED);
		break;
	case RW_EVT_STA_DISCONNECTED:
		atsvr_event_sender(ATSVR_WLAN_DISCONNECT);
		break;
	default:
		break;
	}
}

int judge_the_string_is_ipv4_string(char *is_ip_string)
{
	int len = strlen(is_ip_string);

	if((len > 16 ) || (len < 7 )){
		return -1;
	}
	if(is_ip_string[(len - 1)] == '.'){
		return -1;
	}
	int num;
	int point_num = 0;
	char *p,*p_prev = is_ip_string;
	char ip_num[4];
	int i,j;

	while(1){
		p = strchr(p_prev, '.');
		if((p == NULL) && (point_num != 3)) {
			return -1;
		}
		point_num++;
		i = p - p_prev;
		if((i <= 3) && (i > 0)){
			for(j = 0; j < i; j++) {
				ip_num[j] = p_prev[j];
				if((ip_num[j] < '0') || (ip_num[j] > '9')){
					return -1;
				}
			}
			ip_num[j] = '\0';
			num = atoi( ip_num );
			if(num < 0 || num > 255){
				return -1;
			}else {
				p_prev = p + 1;
				if(point_num == 3) {
					p = strchr(p_prev, '.');
					if(p != NULL){
						return -1;
					}
					i = strlen(p_prev);
					if(i > 3 || i <= 0){
						return -1;
					}
					for(j = 0; j < i; j++) {
						ip_num[j] = p_prev[j];
						if((ip_num[j] < '0') || (ip_num[j] > '9')){
							return -1;
						}
					}
					ip_num[j] = '\0';
					num = atoi( ip_num );
					if(num < 0 || num > 255){
						return -1;
					}
					return 0;
				}
			}
		}else{
			return -1;
		}
	}

	return 0;
}

void atsvr_wlan_init(void)
{
	extern void bk_wlan_status_register_cb(FUNC_1PARAM_PTR cb);
	bk_wlan_status_register_cb(wlan_status_callback);
}

