
/*key button test*/
//#define BUTTON_TEST 

/*voice config*/
//#define VOICE_CONFIG_TEST

/*mixer demo*/
//#define MIXER_DEMO

/*flipping check*/
//#define FLIP_CHECK_TEST

/*wantong test*/
//#define WANTONG_TEST

/*ap network configure sample*/
//#define AP_CONFIG_SAMPLE

/* using BLE */
//#define USING_BLE_TEST
//#define BLE_CONFIG_SAMPLE

/*using ezconfig sample*/
//#define  EZ_CONFIG_SAMPLE

/*using airkiss */
//#define RT_USING_AIRKISS

/*http ota sample */
//#define SAMPLE_USING_OTA_HTTP 

/* web net sample */
//#define WEBNET_SAMPLE

#ifdef XIAOYA_OS
#define VOICE_CONFIG_TEST
#define WANTONG_TEST
#define BLE_CONFIG_SAMPLE
#define KEY_MODE_SELECT    0       //0:开发板普通按键1:故事机ad按键
#endif
