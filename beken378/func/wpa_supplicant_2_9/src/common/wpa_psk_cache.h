#ifndef __WPA_PSK_CACHE_H_
#define __WPA_PSK_CACHE_H_

#include "includes.h"
#include "typedef.h"
#include "list.h"
#include "rtos_pub.h"
#include "common.h"
#include "wpa_common.h"
#include "build_config.h"
#include "../wpa_supplicant/wpa_supplicant_i.h"

#ifdef CONFIG_WPA_PSK_CACHE

#define WPA_PSK_ENTRIES		8

#define WPA_PSK_CACHE_FLAG_PENDING		0x1
#define WPA_PSK_CACHE_FLAG_COMPLETE		0x2

struct wpa_psk_cache_item {
#ifdef CONFIG_WPA_PSK_CACHE_MULTI
	struct dl_list node;
#endif
	char *ssid;
	size_t ssid_len;
	char *passphrase;
	u8 psk[32];
	u8 flags;
};

/**
 * @complete, @pending are protected by @sema
 */
struct wpa_psk_cache {
#ifdef CONFIG_WPA_PSK_CACHE_MULTI
	struct dl_list all;
	struct dl_list pending;		// calcuthread internal use
#else
	struct wpa_psk_cache_item item;
#endif
	beken_semaphore_t sema;
};

void wpa_psk_cache_init();
int wpa_get_psk_from_cache(struct wpa_ssid *ssid);
int __wpa_get_psk_from_cache(u8 *ssid, size_t ssid_len, char *passphrase, u8 *psk, size_t psk_len);
int wpa_psk_request(u8 *ssid, size_t ssid_len, char *passphrase, u8 *psk, size_t psk_len);
void start_wpa_psk_cal_thread();

#else /* !CONFIG_WPA_PSK_CACHE */
static inline void wpa_psk_cache_init(void)
{
}

static inline int wpa_get_psk_from_cache(struct wpa_ssid *ssid)
{
	return -1;
}

static inline int wpa_psk_request(u8 *ssid, size_t ssid_len, char *passphrase, u8 *psk, size_t psk_len)
{
	return -1;
}

#endif // CONFIG_WPA_PSK_CACHE
#endif // __WPA_PSK_CACHE_H_
