#ifndef __VIDEO_DEMO_CONFIG_H__
#define __VIDEO_DEMO_CONFIG_H__

#include "include.h"

#define APP_DEMO_VIDEO_TRANSFER           0

#if (CFG_SOC_NAME == SOC_BK7231N)
#undef  APP_DEMO_VIDEO_TRANSFER
#define APP_DEMO_VIDEO_TRANSFER           0
#endif

#if APP_DEMO_VIDEO_TRANSFER
#include "video_transfer_config.h"

#if (APP_DEMO_CFG_USE_UDP == 0) && (APP_DEMO_CFG_USE_TCP == 0)
#error "must enable one way of network: tcp or udp"
#endif // APP_DEMO_VIDEO_TRANSFER

#define VIDEO_TRANSFER_AP_MODE            1
#define VIDEO_TRANSFER_STA_MODE           2

#define APP_VIDEO_TRANSFER_MODE           (VIDEO_TRANSFER_STA_MODE | VIDEO_TRANSFER_AP_MODE)

#if SUPPORT_TIANZHIHENG_DRONE
#define APP_DEMO_SOFTAP_DEF_SSID          "WIFI_FPV_000000"
#define APP_DEMO_SOFTAP_DEF_NET_IP        "192.168.4.153"
#define APP_DEMO_SOFTAP_DEF_NET_MASK      "255.255.255.0"
#define APP_DEMO_SOFTAP_DEF_NET_GW        "192.168.4.153"
#define APP_DEMO_SOFTAP_DEF_CHANNEL       1
#else  // SUPPORT_TIANZHIHENG_DRONE
// for softap configuration
#define APP_DEMO_SOFTAP_DEF_SSID          "BEKEN_WIFI_000000"
#define APP_DEMO_SOFTAP_DEF_NET_IP        "192.168.1.1"
#define APP_DEMO_SOFTAP_DEF_NET_MASK      "255.255.255.0"
#define APP_DEMO_SOFTAP_DEF_NET_GW        "192.168.1.1"
#define APP_DEMO_SOFTAP_DEF_CHANNEL       1
#endif  // SUPPORT_TIANZHIHENG_DRONE

#endif // APP_DEMO_VIDEO_TRANSFER

#endif // __VIDEO_DEMO_CONFIG_H__