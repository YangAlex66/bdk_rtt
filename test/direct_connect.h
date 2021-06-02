#ifndef _DIRECT_CONNECT_H_
#define _DIRECT_CONNECT_H_

#ifdef __cplusplus
extern "C" {
#endif

#define  direct_connect_debug  1

#if(direct_connect_debug)
	#define dir_link_prf  bk_printf
#else 
	#define dir_link_prf  os_null_printf
#endif


#define DIERCT_CONNECT_SSID_1 		"direct_ssid_1"
#define DIERCT_CONNECT_PWD_1 		"direct_pwd_1"
#define DIERCT_CONNECT_SSID_2 		"direct_ssid_2"
#define DIERCT_CONNECT_PWD_2 		"direct_pwd_2"
#define DIERCT_CONNECT_SSID_3 		"direct_ssid_3"
#define DIERCT_CONNECT_PWD_3		"direct_pwd_3"
#define DIERCT_CONNECT_SSID_4 		"direct_ssid_4"
#define DIERCT_CONNECT_PWD_4 		"direct_pwd_4"
#define DIERCT_CONNECT_SSID_5 		"direct_ssid_5"
#define DIERCT_CONNECT_PWD_5 		"direct_pwd_5"

#define POLLING_TIMEOUT				 1000 * 15


typedef struct
{
	uint8_t direct_ssid[33];
	uint8_t direct_pwd[65];
}direct_ap_info_t;

int direct_connect_init(void);


#endif 

