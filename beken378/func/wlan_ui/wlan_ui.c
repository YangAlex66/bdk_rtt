#include "include.h"
#include "wlan_ui_pub.h"
#include "rw_pub.h"
#include "vif_mgmt.h"
#include "sa_station.h"
#include "param_config.h"
#include "common/ieee802_11_defs.h"
#include "driver_beken.h"
#include "mac_ie.h"
#include "sa_ap.h"
#include "main_none.h"
#include "sm.h"
#include "mac.h"
#include "sm_task.h"
#include "scan_task.h"
#include "hal_machw.h"
#include "error.h"
#include "lwip_netif_address.h"
#include "lwip/inet.h"
#include <stdbool.h>
#include "rw_pub.h"
#include "power_save_pub.h"
#include "rwnx.h"
#include "net.h"
#include "mm_bcn.h"
#include "mm_task.h"
#include "mcu_ps_pub.h"
#include "manual_ps_pub.h"
#include "gpio_pub.h"
#include "phy_trident.h"
#include "rw_msg_rx.h"
#include "app.h"
#include "ate_app.h"
#include "wdt_pub.h"
#include "start_type_pub.h"
#include "wpa_psk_cache.h"
#if CFG_WPA_CTRL_IFACE
#include "wlan_defs_pub.h"
#include "wpa_ctrl.h"
#include "flash_pub.h"
#endif

#if CFG_ROLE_LAUNCH
#include "role_launch.h"
#endif

#if CFG_SUPPORT_OTA_HTTP
#include "utils_httpc.h"
#endif

#include "bk7011_cal_pub.h"
#include "target_util_pub.h"
#include "wlan_ui_pub.h"


monitor_cb_t g_monitor_cb = 0;
unsigned char g_monitor_is_not_filter = 0;
static monitor_cb_t g_bcn_cb = 0;
#if CFG_AP_MONITOR_COEXIST
static int g_ap_monitor_coexist = 0;
#endif

int g_set_channel_postpone_num = 0;
#ifdef CONFIG_AOS_MESH
monitor_cb_t g_mesh_monitor_cb = 0;
uint8_t g_mesh_bssid[6];
#endif
FUNC_1PARAM_PTR connection_status_cb = 0;
static monitor_cb_t g_mgnt_cb = 0;
static uint8_t g_ap_channel = DEFAULT_CHANNEL_AP;

extern void phy_enable_lsig_intr(void);
extern void phy_disable_lsig_intr(void);

#if !CFG_IEEE80211AX
static void rwnx_remove_added_interface(void)
{
    int ret;
    u8 test_mac[6];
    struct mm_add_if_cfm *cfm;
    struct apm_start_cfm *apm_cfm = 0;

	wifi_get_mac_address((char*)test_mac, CONFIG_ROLE_STA);

    cfm = (struct mm_add_if_cfm *)os_malloc(sizeof(struct mm_add_if_cfm));
	ASSERT(cfm);

    ret = rw_msg_send_add_if((const unsigned char *)&test_mac, 3, 0, cfm);
    if(ret || (cfm->status != CO_OK))
    {
        os_printf("[saap]MM_ADD_IF_REQ failed!\r\n");
        goto ERR_RETURN;
    }

    apm_cfm = (struct apm_start_cfm *)os_malloc(sizeof(struct apm_start_cfm));
    ret = rw_msg_send_apm_start_req(cfm->inst_nbr, 1, apm_cfm);

    if(ret || (apm_cfm->status != CO_OK))
    {
        os_printf("[saap]APM_START_REQ failed!\r\n");
        goto ERR_RETURN;
    }

    rw_msg_send_remove_if(cfm->inst_nbr);

ERR_RETURN:
    if(cfm)
    {
        os_free(cfm);
    }

    if(apm_cfm)
    {
        os_free(apm_cfm);
    }
}
#endif

void bk_wlan_connection_loss(void)
{
    struct vif_info_tag *p_vif_entry = vif_mgmt_first_used();

    while (p_vif_entry != NULL)
    {
        if(p_vif_entry->type == VIF_STA)
        {
            os_printf("bk_wlan_connection_loss vif:%d\r\n", p_vif_entry->index);
            sta_ip_down();
            rw_msg_send_connection_loss_ind(p_vif_entry->index);
        }
        p_vif_entry = vif_mgmt_next(p_vif_entry);
    }
}

uint32_t bk_sta_cipher_is_open(void)
{
    ASSERT(g_sta_param_ptr);
    return (BK_SECURITY_TYPE_NONE == g_sta_param_ptr->cipher_suite);
}

uint32_t bk_sta_cipher_is_wep(void)
{
    ASSERT(g_sta_param_ptr);
    return (BK_SECURITY_TYPE_WEP == g_sta_param_ptr->cipher_suite);
}

int bk_sta_cipher_type(void)
{
    if(!sta_ip_is_start())
    {
        return -1;
    }

    return g_sta_param_ptr->cipher_suite;
}

OSStatus bk_wlan_set_country(const wifi_country_t *country)
{
    return rw_ieee80211_set_country(country);
}

OSStatus bk_wlan_get_country(wifi_country_t *country)
{
    return rw_ieee80211_get_country(country);
}

uint32_t bk_wlan_ap_get_frequency(void)
{
    uint8_t channel = bk_wlan_ap_get_channel_config();

    return rw_ieee80211_get_centre_frequency(channel);
}

uint8_t bk_wlan_ap_get_channel_config(void)
{
    return g_ap_param_ptr->chann;
}

void bk_wlan_ap_set_channel_config(uint8_t channel)
{
    g_ap_param_ptr->chann = channel;
}

uint8_t bk_wlan_has_role(uint8_t role)
{
    VIF_INF_PTR vif_entry;
    uint32_t role_count = 0;

    vif_entry = (VIF_INF_PTR)rwm_mgmt_is_vif_first_used();
    while(vif_entry)
    {
        if(vif_entry->type == role)
        {
            role_count ++ ;
        }

        vif_entry = (VIF_INF_PTR)rwm_mgmt_next(vif_entry);
    }

    return role_count;
}

void bk_wlan_set_coexist_at_init_phase(uint8_t current_role)
{
    uint32_t coexit = 0;

    switch(current_role)
    {
        case CONFIG_ROLE_AP:
            if(bk_wlan_has_role(VIF_STA))
            {
                coexit = 1;
            }
            break;

        case CONFIG_ROLE_STA:
            if(bk_wlan_has_role(VIF_AP))
            {
                coexit = 1;
            }
            break;

        case CONFIG_ROLE_NULL:
            if(bk_wlan_has_role(VIF_STA)
                && bk_wlan_has_role(VIF_AP))
            {
                coexit = 1;
            }
            break;

        case CONFIG_ROLE_COEXIST:
            coexit = 1;
            ASSERT(CONFIG_ROLE_COEXIST == g_wlan_general_param->role);
            break;

        default:
            break;
    }

    if(coexit)
    {
        g_wlan_general_param->role = CONFIG_ROLE_COEXIST;
    }
}

uint16_t bk_wlan_sta_get_frequency(void)
{
    uint16_t frequency = 0;
    uint32_t sta_flag = 0;
    VIF_INF_PTR vif_entry;

    vif_entry = (VIF_INF_PTR)rwm_mgmt_is_vif_first_used();
    while(vif_entry)
    {
        if(vif_entry->type == VIF_STA)
        {
            sta_flag = 1;
            break;
        }

        vif_entry = (VIF_INF_PTR)rwm_mgmt_next(vif_entry);
    }

    if(0 == sta_flag)
    {
        goto get_exit;
    }

    frequency = chan_get_vif_frequency(vif_entry);

get_exit:
    return frequency;
}

uint8_t bk_wlan_sta_get_channel(void)
{
    uint8_t channel = 0;
    uint16_t frequency;

    frequency = bk_wlan_sta_get_frequency();
    if(frequency)
    {
        channel = rw_ieee80211_get_chan_id(frequency);
    }

    return channel;
}

uint8_t bk_wlan_ap_get_default_channel(void)
{
    uint8_t channel;

    /* if ap and sta are coexist, ap channel shall match sta channel firstly*/
    channel = bk_wlan_sta_get_channel();
    if(0 == channel)
    {
        channel = g_ap_channel;
    }

    return channel;
}

void bk_wlan_ap_set_default_channel(uint8_t channel)
{
    g_ap_channel = channel;
}

void bk_wlan_ap_csa_coexist_mode(void *arg, uint8_t dummy)
{
    int ret = 0;
    uint16_t frequency;

    if(0 == bk_wlan_has_role(VIF_AP))
    {
        return;
    }

    frequency = bk_wlan_sta_get_frequency();
    if(frequency)
    {
        os_printf("hostapd_channel_switch\n");
#if CFG_ROLE_LAUNCH
        if(!fl_get_pre_sta_cancel_status())
#endif
        {
        	ret = hostapd_channel_switch(frequency);
        }

        if(ret)
        {
            os_printf("csa_failed:%x\r\n", ret);
        }
    }
}

void bk_wlan_reg_csa_cb_coexist_mode(void)
{
    /* the callback routine will be invoked at the timepoint of associating at station mode*/
    mhdr_connect_user_cb(bk_wlan_ap_csa_coexist_mode, 0);
}

void bk_wlan_user_set_rf_wakeup(void)
{
	UINT32 reg = RF_HOLD_BY_USER_BIT;
	sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_SET, &reg);
}

void bk_wlan_user_reset_rf_wakeup(void)
{
	UINT32 reg = RF_HOLD_BY_USER_BIT;
	sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_CLR, &reg);
}

void bk_wlan_phy_open_cca(void)
{
	bk_wlan_user_set_rf_wakeup();
	phy_open_cca();
	bk_wlan_user_reset_rf_wakeup();
	os_printf("bk_wlan cca opened\r\n");
}

void bk_wlan_phy_close_cca(void)
{
	bk_wlan_user_set_rf_wakeup();
	phy_close_cca();
	bk_wlan_user_reset_rf_wakeup();
	os_printf("bk_wlan cca closed\r\n");
}

void bk_wlan_phy_show_cca(void)
{
	phy_show_cca();
}

void bk_reboot(void)
{
    UINT32 wdt_val = 5;

    os_printf("bk_reboot\r\n");

    bk_misc_update_set_type(RESET_SOURCE_REBOOT);

    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();

    sddev_control(WDT_DEV_NAME, WCMD_POWER_DOWN, NULL);
    os_printf("wdt reboot\r\n");
#if (CFG_SOC_NAME == SOC_BK7231N)
    delay_ms(100); //add delay for bk_writer BEKEN_DO_REBOOT cmd
#endif
    sddev_control(WDT_DEV_NAME, WCMD_SET_PERIOD, &wdt_val);
    sddev_control(WDT_DEV_NAME, WCMD_POWER_UP, NULL);
    while(1);
    GLOBAL_INT_RESTORE();
}

void bk_wlan_ap_init(network_InitTypeDef_st *inNetworkInitPara)
{
    os_printf("Soft_AP_start\r\n");
#if CFG_ROLE_LAUNCH
	rl_init_csa_status();
#endif

    if(!g_ap_param_ptr)
    {
        g_ap_param_ptr = (ap_param_t *)os_zalloc(sizeof(ap_param_t));
        ASSERT(g_ap_param_ptr);
    }

    os_memset(g_ap_param_ptr, 0x00, sizeof(*g_ap_param_ptr));

    if(MAC_ADDR_NULL((u8 *)&g_ap_param_ptr->bssid))
    {
        wifi_get_mac_address((char*)(&g_ap_param_ptr->bssid), CONFIG_ROLE_AP);
    }

#if CFG_SUPPORT_RTT
	if(inNetworkInitPara->reserved[0])
	{
		bk_wlan_ap_set_default_channel(inNetworkInitPara->reserved[0]);
	}
#endif

    bk_wlan_ap_set_channel_config(bk_wlan_ap_get_default_channel());
	
    if(!g_wlan_general_param)
    {
        g_wlan_general_param = (general_param_t *)os_zalloc(sizeof(general_param_t));
        ASSERT(g_wlan_general_param);
    }
    g_wlan_general_param->role = CONFIG_ROLE_AP;
    bk_wlan_set_coexist_at_init_phase(CONFIG_ROLE_AP);

    if(inNetworkInitPara)
    {
        g_ap_param_ptr->ssid.length = MIN(SSID_MAX_LEN, os_strlen(inNetworkInitPara->wifi_ssid));
        os_memcpy(g_ap_param_ptr->ssid.array, inNetworkInitPara->wifi_ssid, g_ap_param_ptr->ssid.length);
        g_ap_param_ptr->key_len = os_strlen(inNetworkInitPara->wifi_key);
        if(g_ap_param_ptr->key_len < 8)
        {
            g_ap_param_ptr->cipher_suite = BK_SECURITY_TYPE_NONE;
        }
        else
        {
#if CFG_SOFTAP_WPA3
            g_ap_param_ptr->cipher_suite = BK_SECURITY_TYPE_WPA3_WPA2_MIXED;
#else
            g_ap_param_ptr->cipher_suite = BK_SECURITY_TYPE_WPA2_AES;
#endif
            os_memcpy(g_ap_param_ptr->key, inNetworkInitPara->wifi_key, g_ap_param_ptr->key_len);
        }

        if(inNetworkInitPara->dhcp_mode == DHCP_SERVER)
        {
            g_wlan_general_param->dhcp_enable = 1;
        }
        else
        {
            g_wlan_general_param->dhcp_enable = 0;
        }
        inet_aton(inNetworkInitPara->local_ip_addr, &(g_wlan_general_param->ip_addr));
        inet_aton(inNetworkInitPara->net_mask, &(g_wlan_general_param->ip_mask));
        inet_aton(inNetworkInitPara->gateway_ip_addr, &(g_wlan_general_param->ip_gw));

#if CFG_ROLE_LAUNCH
		if(rl_pre_ap_set_status(RL_STATUS_AP_INITING))
		{
			return;
		}
#endif
    }

	if(inNetworkInitPara)
    {
        UINT32 reg = RF_HOLD_BY_AP_BIT;
        sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_SET, &reg);
    }

    sa_ap_init();
}

OSStatus bk_wlan_start_ap(network_InitTypeDef_st *inNetworkInitParaAP)
{
#if CFG_USE_STA_PS
    bk_wlan_dtim_rf_ps_disable_send_msg();
#endif

#if !CFG_WPA_CTRL_IFACE
    int ret, flag ,empty;
    GLOBAL_INT_DECLARATION();

	while( 1 )
	{
		GLOBAL_INT_DISABLE();
		flag = mm_bcn_get_tx_cfm();
        empty = is_apm_bss_config_empty();
		if ( flag == 0 && empty == 1)
		{
			GLOBAL_INT_RESTORE();
			break;
		}
		else
		{
			GLOBAL_INT_RESTORE();
			rtos_delay_milliseconds(100);
		}
	}

	bk_wlan_stop(BK_SOFT_AP);

    bk_wlan_ap_init(inNetworkInitParaAP);

    ret = hostapd_main_entry(2, 0);
    if(ret)
    {
        os_printf("bk_wlan_start softap failed!!\r\n");
        bk_wlan_stop(BK_SOFT_AP);
        return -1;
    }

    net_wlan_add_netif(&g_ap_param_ptr->bssid);

    ip_address_set(BK_SOFT_AP,
                   inNetworkInitParaAP->dhcp_mode,
                   inNetworkInitParaAP->local_ip_addr,
                   inNetworkInitParaAP->net_mask,
                   inNetworkInitParaAP->gateway_ip_addr,
                   inNetworkInitParaAP->dns_server_ip_addr);
    uap_ip_start();

    sm_build_broadcast_deauthenticate();
#else /* CFG_WPA_CTRL_IFACE */
	/* stop lwip netif */
	uap_ip_down();

	/* set AP parameter, ssid, akm, etc. */
    bk_wlan_ap_init(inNetworkInitParaAP);

	// enable hostapd
	wlan_ap_enable();

	// reload bss configuration
	wlan_ap_reload();

	/* now ap has started, set ip address to this interface */
    ip_address_set(BK_SOFT_AP,
                   inNetworkInitParaAP->dhcp_mode,
                   inNetworkInitParaAP->local_ip_addr,
                   inNetworkInitParaAP->net_mask,
                   inNetworkInitParaAP->gateway_ip_addr,
                   inNetworkInitParaAP->dns_server_ip_addr);

	/* restart lwip network */
    uap_ip_start();
#endif

    return kNoErr;
}

void bk_wlan_sta_init(network_InitTypeDef_st *inNetworkInitPara)
{
#if CFG_ROLE_LAUNCH
	rl_init_csa_status();
#endif

    if(!g_sta_param_ptr)
    {
        g_sta_param_ptr = (sta_param_t *)os_zalloc(sizeof(sta_param_t));
        ASSERT(g_sta_param_ptr);
    }

    wifi_get_mac_address((char*)(&g_sta_param_ptr->own_mac), CONFIG_ROLE_STA);
    if(!g_wlan_general_param)
    {
        g_wlan_general_param = (general_param_t *)os_zalloc(sizeof(general_param_t));
        ASSERT(g_wlan_general_param);
    }
    g_wlan_general_param->role = CONFIG_ROLE_STA;
    bk_wlan_set_coexist_at_init_phase(CONFIG_ROLE_STA);

    if(inNetworkInitPara)
    {
        g_sta_param_ptr->ssid.length = MIN(SSID_MAX_LEN, os_strlen(inNetworkInitPara->wifi_ssid));
        os_memcpy(g_sta_param_ptr->ssid.array,
                  inNetworkInitPara->wifi_ssid,
                  g_sta_param_ptr->ssid.length);

#if CFG_SUPPORT_BSSID_CONNECT
        os_memcpy(g_sta_param_ptr->fast_connect.bssid, inNetworkInitPara->wifi_bssid, sizeof(inNetworkInitPara->wifi_bssid));
#endif

        g_sta_param_ptr->key_len = os_strlen(inNetworkInitPara->wifi_key);
        os_memcpy(g_sta_param_ptr->key, inNetworkInitPara->wifi_key, g_sta_param_ptr->key_len);
		g_sta_param_ptr->key[g_sta_param_ptr->key_len] = 0;		/* append \0 */

        if(inNetworkInitPara->dhcp_mode == DHCP_CLIENT)
        {
            g_wlan_general_param->dhcp_enable = 1;
        }
        else
        {
            g_wlan_general_param->dhcp_enable = 0;
            inet_aton(inNetworkInitPara->local_ip_addr, &(g_wlan_general_param->ip_addr));
            inet_aton(inNetworkInitPara->net_mask, &(g_wlan_general_param->ip_mask));
            inet_aton(inNetworkInitPara->gateway_ip_addr, &(g_wlan_general_param->ip_gw));
        }
    }

#if CFG_ROLE_LAUNCH
	    if(rl_pre_sta_set_status(RL_STATUS_STA_INITING))
	    {
	        return;
	    }
#endif

    if(inNetworkInitPara)
    {
        UINT32 reg = RF_HOLD_BY_STA_BIT;
        sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_SET, &reg);
    }
    else
    {
        UINT32 reg = RF_HOLD_BY_SCAN_BIT;
        sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_SET, &reg);
    }

    bk_wlan_reg_csa_cb_coexist_mode();
    sa_station_init();

    bk_wlan_register_bcn_cb(wlan_ui_bcn_callback);
}

#if CFG_WPA_CTRL_IFACE && CFG_WLAN_FAST_CONNECT

#define BSSID_INFO_ADDR            0x1e2000 /*reserve 4k for bssid info*/

void wlan_read_fast_connect_info(struct wlan_fast_connect_info *fci)
{
	uint32_t status, addr;
	DD_HANDLE flash_hdl;

	flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
	addr = BSSID_INFO_ADDR;
	ddev_read(flash_hdl, (char *)fci, sizeof(*fci), addr);
	ddev_close(flash_hdl);
}

void wlan_write_fast_connect_info(struct wlan_fast_connect_info *fci)
{
	uint32_t status, addr;
	struct wlan_fast_connect_info pre_fci;
	uint8_t protect_flag, protect_param;
	DD_HANDLE flash_hdl;

	/* obtain the previous bssid info*/
	wlan_read_fast_connect_info(&pre_fci);

	/* if different, save the latest information about fast connection*/
	if (!os_memcmp(&pre_fci, fci, sizeof(*fci)))
		goto wr_exit;

	/* write flash and save the information about fast connection*/
	flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);

	ddev_control(flash_hdl, CMD_FLASH_GET_PROTECT, &protect_flag);
	protect_param = FLASH_PROTECT_NONE;
	ddev_control(flash_hdl, CMD_FLASH_SET_PROTECT, (void *)&protect_param);

	addr = BSSID_INFO_ADDR;
	ddev_control(flash_hdl, CMD_FLASH_ERASE_SECTOR, (void *)&addr);

	ddev_write(flash_hdl, fci, sizeof(*fci), addr);

	ddev_control(flash_hdl, CMD_FLASH_SET_PROTECT, (void *)&protect_flag);
	ddev_close(flash_hdl);

	bk_printf("writed fci to flash\n");

wr_exit:
	return;
}
#endif

OSStatus bk_wlan_start_sta(network_InitTypeDef_st *inNetworkInitPara)
{
	size_t psk_len = 0;
	u8 *psk = 0;

#if CFG_WPA_CTRL_IFACE
#if CFG_WLAN_FAST_CONNECT
	struct wlan_fast_connect_info fci;
	int ssid_len, req_ssid_len;
#endif
	int chan = 0;

	/* diconnect previous connection if may */
	sta_ip_down();	// XXX: WLAN_DISCONNECT_EVENT may handle this
	wlan_sta_disconnect();
#else /* !CFG_WPA_CTRL_IFACE */
#if CFG_ROLE_LAUNCH
	rl_status_set_st_state(RL_ST_STATUS_RESTART_ST);
#endif
    bk_wlan_stop(BK_STATION);
#endif
    mhdr_set_station_status(RW_EVT_STA_CONNECTING);
#if CFG_ROLE_LAUNCH
	rl_status_reset_st_state(RL_ST_STATUS_RESTART_HOLD | RL_ST_STATUS_RESTART_ST);
#endif

#if (RF_USE_POLICY == BLE_DEFAULT_WIFI_REQUEST)
    wifi_station_status_event_notice(0,RW_EVT_STA_CONNECTING);
#endif

#if (CFG_SOC_NAME == SOC_BK7231N)
	if (get_ate_mode_state()) {
		// cunliang20210407 set blk_standby_cfg with blk_txen_cfg like txevm, qunshan confirmed
		rwnx_cal_en_extra_txpa();
	}
#endif
    bk_wlan_sta_init(inNetworkInitPara);

#if CFG_WPA_CTRL_IFACE && CFG_WLAN_FAST_CONNECT
	wlan_read_fast_connect_info(&fci);

	ssid_len = os_strlen(fci.ssid);
	if(ssid_len > SSID_MAX_LEN)
		ssid_len = SSID_MAX_LEN;

	req_ssid_len = os_strlen(inNetworkInitPara->wifi_ssid);
	if(req_ssid_len > SSID_MAX_LEN)
		req_ssid_len = SSID_MAX_LEN;

#if 0
	print_hex_dump("fci: ", &fci, sizeof(fci));
	bk_printf("  ssid: |%s|\n", fci.ssid);
	bk_printf("  bssid: %pM\n", fci.bssid);
	bk_printf("  chan: %d\n", fci.channel);
	bk_printf("  desire ssid: |%s|\n", inNetworkInitPara->wifi_ssid);
#endif

	if (ssid_len == req_ssid_len &&
		os_memcmp(inNetworkInitPara->wifi_ssid, fci.ssid, ssid_len) == 0 &&
		os_strcmp(inNetworkInitPara->wifi_key, fci.pwd) == 0) {
		chan = fci.channel;
		psk = fci.psk;
		psk_len = PMK_LEN * 2;

		bk_printf("fast_connect\n");
#if 0
		bk_printf("  chan: %d\n", chan);
		bk_printf("  PMK: %s\n", psk);
#endif
		if (os_strlen(psk) == 0) {
			// no psk info, calcuate pmk
			psk = 0;
			psk_len = 0;
		}
	}
#endif

	/*
	 * let wpa_psk_cal thread to caculate psk.
	 * XXX: If you know psk value, fill last two parameters of `wpa_psk_request()'.
	 */
	wpa_psk_request(g_sta_param_ptr->ssid.array, g_sta_param_ptr->ssid.length,
			(char*)g_sta_param_ptr->key, psk, psk_len);

#if CFG_WPA_CTRL_IFACE
	/* enable wpa_supplicant */
	wlan_sta_enable();

	/* set network parameters: ssid, passphase */
	wlan_sta_set((uint8_t *)inNetworkInitPara->wifi_ssid, os_strlen(inNetworkInitPara->wifi_ssid), (uint8_t *)inNetworkInitPara->wifi_key);

	/* connect to AP */
	wlan_sta_connect(chan);
#else /* !CFG_WPA_CTRL_IFACE */
    supplicant_main_entry(inNetworkInitPara->wifi_ssid);
    net_wlan_add_netif(&g_sta_param_ptr->own_mac);
#endif

	/* set IP mode */
    ip_address_set(inNetworkInitPara->wifi_mode,
                   inNetworkInitPara->dhcp_mode,
                   inNetworkInitPara->local_ip_addr,
                   inNetworkInitPara->net_mask,
                   inNetworkInitPara->gateway_ip_addr,
                   inNetworkInitPara->dns_server_ip_addr);

    return kNoErr;
}

OSStatus bk_wlan_start(network_InitTypeDef_st *inNetworkInitPara)
{
    int ret = 0;
#if CFG_ROLE_LAUNCH
    LAUNCH_REQ lreq;
#endif

    if(bk_wlan_is_monitor_mode())
    {
        os_printf("monitor (ie.airkiss) is not finish yet, stop it or waiting it finish!\r\n");
        return ret;
    }

    if(inNetworkInitPara->wifi_mode == BK_SOFT_AP)
    {
        #if CFG_ROLE_LAUNCH
        lreq.req_type = LAUNCH_REQ_AP;
        lreq.descr = *inNetworkInitPara;

        rl_ap_request_enter(&lreq, 0);
        #else
        bk_wlan_start_ap(inNetworkInitPara);
        #endif
    }
    else if(inNetworkInitPara->wifi_mode == BK_STATION)
    {
        #if CFG_ROLE_LAUNCH
        lreq.req_type = LAUNCH_REQ_STA;
        lreq.descr = *inNetworkInitPara;

		rl_status_reset_private_state(RL_PRIV_STATUS_STA_ADV);
        rl_sta_request_enter(&lreq, 0);
        #else
        bk_wlan_start_sta(inNetworkInitPara);
        #endif
    }

    return 0;
}

void bk_wlan_start_scan(void)
{
#if !CFG_WPA_CTRL_IFACE
    SCAN_PARAM_T scan_param = {0};
#endif

#if CFG_USE_STA_PS
    bk_wlan_dtim_rf_ps_disable_send_msg();
#endif

#if CFG_ROLE_LAUNCH
	rl_status_reset_st_state(RL_ST_STATUS_RESTART_HOLD | RL_ST_STATUS_RESTART_ST);
#endif

    if(bk_wlan_is_monitor_mode())
    {
        os_printf("monitor (ie.airkiss) is not finish yet, stop it or waiting it finish!\r\n");

		#if CFG_ROLE_LAUNCH
		rl_pre_sta_set_status(RL_STATUS_STA_LAUNCH_FAILED);
		#endif

        return;
    }

    bk_wlan_sta_init(0);

#if CFG_WPA_CTRL_IFACE
	wlan_sta_enable();
	wlan_sta_scan_once();
#else /* !CFG_WPA_CTRL_IFACE */
    os_memset(&scan_param.bssid, 0xff, ETH_ALEN);
	scan_param.vif_idx = INVALID_VIF_IDX;
	scan_param.num_ssids = 1;

    rw_msg_send_scanu_req(&scan_param);
#endif
}


void bk_wlan_scan_ap_reg_cb(FUNC_2PARAM_PTR ind_cb)
{
    mhdr_scanu_reg_cb(ind_cb,0);
}

unsigned char bk_wlan_get_scan_ap_result_numbers(void)
{
    return sr_get_scan_number();
}

int bk_wlan_get_scan_ap_result(SCAN_RST_ITEM_PTR scan_rst_table, unsigned char get_scanu_num)
{
	struct scanu_rst_upload *scan_rst;
	unsigned char scanu_num = 0, i;
	GLOBAL_INT_DECLARATION();

	GLOBAL_INT_DISABLE();

	scan_rst = sr_get_scan_results();
	if (scan_rst) {
		scanu_num = (scan_rst->scanu_num) > get_scanu_num ? (get_scanu_num) : (scan_rst->scanu_num);

		for (i = 0; i < scanu_num; i++)
			scan_rst_table[i] = *(scan_rst->res[i]);
	}
	GLOBAL_INT_RESTORE();

	sr_release_scan_results(scan_rst);
	return scanu_num;
}

void bk_wlan_start_assign_scan(UINT8 **ssid_ary, UINT8 ssid_num)
{
#if !CFG_WPA_CTRL_IFACE
	SCAN_PARAM_T scan_param = {0};

	bk_wlan_sta_init(0);

	os_memset(&scan_param.bssid, 0xff, ETH_ALEN);
	scan_param.vif_idx = INVALID_VIF_IDX;
	scan_param.num_ssids = ssid_num;
	for (int i = 0 ; i < ssid_num ; i++) {
		scan_param.ssids[i].length = MIN(SSID_MAX_LEN, os_strlen((char *)ssid_ary[i]));
		os_memcpy(scan_param.ssids[i].array, ssid_ary[i], scan_param.ssids[i].length);
	}

	rw_msg_send_scanu_req(&scan_param);
#else /* CFG_WPA_CTRL_IFACE */
	wlan_sta_scan_param_t scan_param = {0};

	/* init hw */
	bk_wlan_sta_init(0);

	/* enable wpa_supplicant */
	wlan_sta_enable();

	/* set scan ssid list */
	scan_param.num_ssids = MIN(ssid_num, WLAN_SCAN_SSID_MAX);
	for (int i = 0 ; i < scan_param.num_ssids ; i++) {
		scan_param.ssids[i].ssid_len = MIN(WLAN_SSID_MAX_LEN, os_strlen((char *)ssid_ary[i]));
		os_memcpy(scan_param.ssids[i].ssid, ssid_ary[i], scan_param.ssids[i].ssid_len);
	}

	/* start scan */
	wlan_sta_scan(&scan_param);
#endif /* CFG_WPA_CTRL_IFACE */
}

void bk_wlan_sta_init_adv(network_InitTypeDef_adv_st *inNetworkInitParaAdv)
{
    UINT32 reg;

    if(!g_sta_param_ptr)
    {
        g_sta_param_ptr = (sta_param_t *)os_zalloc(sizeof(sta_param_t));
        ASSERT(g_sta_param_ptr);
    }

    if(MAC_ADDR_NULL((u8 *)&g_sta_param_ptr->own_mac))
    {
        wifi_get_mac_address((char *)&g_sta_param_ptr->own_mac, CONFIG_ROLE_STA);
    }

    g_sta_param_ptr->ssid.length = MIN(SSID_MAX_LEN, os_strlen(inNetworkInitParaAdv->ap_info.ssid));
    os_memcpy(g_sta_param_ptr->ssid.array, inNetworkInitParaAdv->ap_info.ssid, g_sta_param_ptr->ssid.length);

	g_sta_param_ptr->cipher_suite = inNetworkInitParaAdv->ap_info.security;
    g_sta_param_ptr->fast_connect_set = 1;
    g_sta_param_ptr->fast_connect.chann = inNetworkInitParaAdv->ap_info.channel;
    os_memcpy(g_sta_param_ptr->fast_connect.bssid, inNetworkInitParaAdv->ap_info.bssid, ETH_ALEN);
    g_sta_param_ptr->key_len = inNetworkInitParaAdv->key_len;
    os_memcpy((uint8_t *)g_sta_param_ptr->key, inNetworkInitParaAdv->key, inNetworkInitParaAdv->key_len);
	g_sta_param_ptr->key[inNetworkInitParaAdv->key_len] = 0;	/* append \0 */

    if(!g_wlan_general_param)
    {
        g_wlan_general_param = (general_param_t *)os_malloc(sizeof(general_param_t));
    }
    g_wlan_general_param->role = CONFIG_ROLE_STA;
    bk_wlan_set_coexist_at_init_phase(CONFIG_ROLE_STA);

    if(inNetworkInitParaAdv->dhcp_mode == DHCP_CLIENT)
    {
        g_wlan_general_param->dhcp_enable = 1;
    }
    else
    {
        g_wlan_general_param->dhcp_enable = 0;
        inet_aton(inNetworkInitParaAdv->local_ip_addr, &(g_wlan_general_param->ip_addr));
        inet_aton(inNetworkInitParaAdv->net_mask, &(g_wlan_general_param->ip_mask));
        inet_aton(inNetworkInitParaAdv->gateway_ip_addr, &(g_wlan_general_param->ip_gw));
    }

    if(inNetworkInitParaAdv)
    {
        reg = RF_HOLD_BY_STA_BIT;
        sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_SET, &reg);
    }
    else
    {
        reg = RF_HOLD_BY_SCAN_BIT;
        sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_SET, &reg);
    }
    bk_wlan_reg_csa_cb_coexist_mode();
    sa_station_init();
}

void bk_wlan_ap_init_adv(network_InitTypeDef_ap_st *inNetworkInitParaAP)
{
    UINT32 reg = RF_HOLD_BY_AP_BIT;

    if(!g_ap_param_ptr)
    {
        g_ap_param_ptr = (ap_param_t *)os_zalloc(sizeof(ap_param_t));
        ASSERT(g_ap_param_ptr);
    }

    if(MAC_ADDR_NULL((u8 *)&g_ap_param_ptr->bssid))
    {
        wifi_get_mac_address((char *)&g_ap_param_ptr->bssid, CONFIG_ROLE_AP);
    }

    if(!g_wlan_general_param)
    {
        g_wlan_general_param = (general_param_t *)os_zalloc(sizeof(general_param_t));
        ASSERT(g_wlan_general_param);
    }
    g_wlan_general_param->role = CONFIG_ROLE_AP;
    bk_wlan_set_coexist_at_init_phase(CONFIG_ROLE_AP);

    if(inNetworkInitParaAP)
    {
        if(0 == inNetworkInitParaAP->channel)
        {
            g_ap_param_ptr->chann = bk_wlan_ap_get_default_channel();
        }
        else
        {
            g_ap_param_ptr->chann = inNetworkInitParaAP->channel;
        }

        g_ap_param_ptr->ssid.length = MIN(SSID_MAX_LEN, os_strlen(inNetworkInitParaAP->wifi_ssid));
        os_memcpy(g_ap_param_ptr->ssid.array, inNetworkInitParaAP->wifi_ssid, g_ap_param_ptr->ssid.length);
        g_ap_param_ptr->key_len = os_strlen(inNetworkInitParaAP->wifi_key);
        os_memcpy(g_ap_param_ptr->key, inNetworkInitParaAP->wifi_key, g_ap_param_ptr->key_len);

        g_ap_param_ptr->cipher_suite = inNetworkInitParaAP->security;

        if(inNetworkInitParaAP->dhcp_mode == DHCP_SERVER)
        {
            g_wlan_general_param->dhcp_enable = 1;
        }
        else
        {
            g_wlan_general_param->dhcp_enable = 0;
        }
        inet_aton(inNetworkInitParaAP->local_ip_addr, &(g_wlan_general_param->ip_addr));
        inet_aton(inNetworkInitParaAP->net_mask, &(g_wlan_general_param->ip_mask));
        inet_aton(inNetworkInitParaAP->gateway_ip_addr, &(g_wlan_general_param->ip_gw));

        sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_SET, &reg);
    }

    sa_ap_init();
}

void bk_wlan_sta_adv_param_2_sta(network_InitTypeDef_adv_st *sta_adv,network_InitTypeDef_st* sta)
{
	memset(sta,0,sizeof(*sta));

	sta->wifi_mode = BK_STATION;
	memcpy(sta->wifi_ssid,sta_adv->ap_info.ssid,32);
	sta->wifi_ssid[32] = '\0';
	memcpy(sta->wifi_key,sta_adv->key,64);
	memcpy(sta->local_ip_addr,sta_adv->local_ip_addr,16);
	memcpy(sta->net_mask,sta_adv->net_mask,16);
	memcpy(sta->gateway_ip_addr,sta_adv->gateway_ip_addr,16);
	memcpy(sta->dns_server_ip_addr,sta_adv->dns_server_ip_addr,16);
	sta->dhcp_mode = sta_adv->dhcp_mode;
	memcpy(sta->wifi_bssid,sta_adv->ap_info.bssid,16);
	sta->wifi_retry_interval = sta_adv->wifi_retry_interval;
}

OSStatus bk_wlan_start_sta_adv(network_InitTypeDef_adv_st *inNetworkInitParaAdv)
{
	if (bk_wlan_is_monitor_mode()) {
		os_printf("airkiss is not finish yet, stop airkiss or waiting it finish!\r\n");
		return 0;
	}

#if !CFG_WPA_CTRL_IFACE
#if CFG_ROLE_LAUNCH
	rl_status_set_private_state(RL_PRIV_STATUS_STA_ADV_RDY);
#endif
#if CFG_ROLE_LAUNCH
	rl_status_set_st_state(RL_ST_STATUS_RESTART_ST);
#endif

	bk_wlan_stop(BK_STATION);
#if CFG_ROLE_LAUNCH
	if (rl_pre_sta_set_status(RL_STATUS_STA_INITING))
		return -1;

	rl_status_reset_st_state(RL_ST_STATUS_RESTART_HOLD | RL_ST_STATUS_RESTART_ST);
	rl_status_set_private_state(RL_PRIV_STATUS_STA_ADV);
	rl_status_reset_private_state(RL_PRIV_STATUS_STA_ADV_RDY);
	LAUNCH_REQ lreq;

	lreq.req_type = LAUNCH_REQ_STA;
	bk_wlan_sta_adv_param_2_sta(inNetworkInitParaAdv, &lreq.descr);

	rl_sta_adv_register_cache_station(&lreq);
#endif

	bk_wlan_sta_init_adv(inNetworkInitParaAdv);

	/*
	 * let wpa_psk_cal thread to caculate psk.
	 * XXX: If you know psk value, fill last two parameters of `wpa_psk_request()'.
	 */
	wpa_psk_request(g_sta_param_ptr->ssid.array, g_sta_param_ptr->ssid.length,
					(char *)(g_sta_param_ptr->key), NULL, 0);

	supplicant_main_entry(inNetworkInitParaAdv->ap_info.ssid);

	net_wlan_add_netif(&g_sta_param_ptr->own_mac);


#else /* CFG_WPA_CTRL_IFACE */
	wlan_sta_config_t config;

#if (CFG_SOC_NAME == SOC_BK7231N)
	if (get_ate_mode_state()) {
		// cunliang20210407 set blk_standby_cfg with blk_txen_cfg like txevm, qunshan confirmed
		rwnx_cal_en_extra_txpa();
	}
#endif
	bk_wlan_sta_init_adv(inNetworkInitParaAdv);

	/*
	 * let wpa_psk_cal thread to caculate psk.
	 * XXX: If you know psk value, fill last two parameters of `wpa_psk_request()'.
	 */
	wpa_psk_request(g_sta_param_ptr->ssid.array, g_sta_param_ptr->ssid.length,
					(char*)g_sta_param_ptr->key, NULL, 0);

	/* disconnect previous connected network */
	wlan_sta_disconnect();

	/* start wpa_supplicant */
	wlan_sta_enable();

	/* set network parameters: ssid, passphase */
	wlan_sta_set((uint8_t*)inNetworkInitParaAdv->ap_info.ssid, os_strlen(inNetworkInitParaAdv->ap_info.ssid),
				 (uint8_t*)inNetworkInitParaAdv->key);

	/* set fast connect bssid */
	os_memset(&config, 0, sizeof(config));
	os_memcpy(config.u.bssid, inNetworkInitParaAdv->ap_info.bssid, ETH_ALEN);
	config.field = WLAN_STA_FIELD_BSSID;
	wpa_ctrl_request(WPA_CTRL_CMD_STA_SET, &config);

	/* set fast connect freq */
	os_memset(&config, 0, sizeof(config));
	config.u.channel = inNetworkInitParaAdv->ap_info.channel;
	config.field = WLAN_STA_FIELD_FREQ;
	wpa_ctrl_request(WPA_CTRL_CMD_STA_SET, &config);

	/* connect to AP */
	wlan_sta_connect(g_sta_param_ptr->fast_connect_set ? g_sta_param_ptr->fast_connect.chann : 0);

#endif
	ip_address_set(BK_STATION, inNetworkInitParaAdv->dhcp_mode,
				   inNetworkInitParaAdv->local_ip_addr,
				   inNetworkInitParaAdv->net_mask,
				   inNetworkInitParaAdv->gateway_ip_addr,
				   inNetworkInitParaAdv->dns_server_ip_addr);

	return 0;
}


OSStatus bk_wlan_start_ap_adv(network_InitTypeDef_ap_st *inNetworkInitParaAP)
{
    int ret = 0;

    if(bk_wlan_is_monitor_mode())
    {
        os_printf("monitor (ie.airkiss) is not finish yet, stop it or waiting it finish!\r\n");
        return ret;
    }

#if CFG_USE_STA_PS
    bk_wlan_dtim_rf_ps_disable_send_msg();
#endif

#if !CFG_WPA_CTRL_IFACE
    bk_wlan_stop(BK_SOFT_AP);

#if CFG_ROLE_LAUNCH
    if(rl_pre_ap_set_status(RL_STATUS_AP_INITING))
    {
        return 0;
    }
#endif

    bk_wlan_ap_init_adv(inNetworkInitParaAP);

    ret = hostapd_main_entry(2, 0);
    if(ret)
    {
        os_printf("bk_wlan_start_ap_adv failed!!\r\n");
        bk_wlan_stop(BK_SOFT_AP);
        return -1;
    }

    net_wlan_add_netif(&g_ap_param_ptr->bssid);

    ip_address_set(BK_SOFT_AP, inNetworkInitParaAP->dhcp_mode,
                   inNetworkInitParaAP->local_ip_addr,
                   inNetworkInitParaAP->net_mask,
                   inNetworkInitParaAP->gateway_ip_addr,
                   inNetworkInitParaAP->dns_server_ip_addr);
    uap_ip_start();

    sm_build_broadcast_deauthenticate();
#else	/* CFG_WPA_CTRL_IFACE */
	/* stop lwip netif */
	uap_ip_down();

	/* set AP parameter, ssid, akm, etc. */
    bk_wlan_ap_init_adv(inNetworkInitParaAP);

	// enable hostapd
	wlan_ap_enable();

	// reload bss configuration
	wlan_ap_reload();

	/* now ap has started, set ip address to this interface */
    ip_address_set(BK_SOFT_AP, inNetworkInitParaAP->dhcp_mode,
                   inNetworkInitParaAP->local_ip_addr,
                   inNetworkInitParaAP->net_mask,
                   inNetworkInitParaAP->gateway_ip_addr,
                   inNetworkInitParaAP->dns_server_ip_addr);

	/* restart lwip network */
    uap_ip_start();
#endif

    return kNoErr;
}

int bk_wlan_stop_scan(void)
{
	struct scan_cancel_cfm cfm = {0};
	int ret = -1;

	os_printf("%s\r\n",__FUNCTION__);
	ret = rw_msg_send_scan_cancel_req(&cfm);
	if(ret == 0){
		/*if(CO_FAIL == cfm.status){
			////is not on scaning
		}else if(CO_OK == cfm.status){
			////is abort scaning
		}
		*/
	}

	return ret;
}

int bk_wlan_stop(char mode)
{
    int ret = kNoErr;
    UINT32 reg;

    #if CFG_USE_AP_IDLE
    if(bk_wlan_has_role(VIF_AP) && ap_ps_enable_get())
    {
    stop_global_ap_bcn_timer();
    }
    #endif

    //bk_wlan_stop_state=1;

    if(mode == BK_STATION)
    {
        mhdr_set_station_status(RW_EVT_STA_IDLE);
    }

    switch(mode)
    {
    case BK_SOFT_AP:
	    sm_build_broadcast_deauthenticate();
        mm_hw_ap_disable();

#if !CFG_WPA_CTRL_IFACE
#if 0
        uap_ip_down();
        net_wlan_remove_netif(&g_ap_param_ptr->bssid);
        hostapd_main_exit();
#else
		wlan_ap_disable();
#endif
#if CFG_ROLE_LAUNCH
        rl_set_csa_switched();
#endif
#else
        wlan_ap_disable();
#endif
        if(bk_wlan_has_role(VIF_STA))
        {
            g_wlan_general_param->role = CONFIG_ROLE_STA;
        }

#if CFG_ROLE_LAUNCH
        rl_pre_ap_set_status(RL_STATUS_AP_LAUNCHED);
#endif
        reg = RF_HOLD_BY_AP_BIT;
        sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_CLR, &reg);
        break;

    case BK_STATION:
#if CFG_ROLE_LAUNCH
        rl_pre_sta_set_status(RL_STATUS_UNKNOWN);
		rl_status_set_st_state(RL_ST_STATUS_RESTART_HOLD);
#endif

#if CFG_USE_STA_PS
        bk_wlan_dtim_rf_ps_disable_send_msg();
#endif

#if (CFG_SOC_NAME == SOC_BK7231N)
		if (get_ate_mode_state()) {
			// cunliang20210407 recover blk_standby_cfg like txevm -e 0, qunshan confirmed
			rwnx_cal_dis_extra_txpa();
		}
#endif
        sta_ip_down();
#if CFG_ROLE_LAUNCH
		rl_pre_sta_clear_cancel();
#endif

#if !CFG_WPA_CTRL_IFACE
#if 0
        net_wlan_remove_netif(&g_sta_param_ptr->own_mac);
        supplicant_main_exit();
        wpa_hostapd_release_scan_rst();
#else
		bk_wlan_stop_scan();
		wlan_sta_disable();
#endif
#else
		wlan_sta_disable();	/* same but call in wpas task */
#endif
        if(bk_wlan_has_role(VIF_AP))
        {
            g_wlan_general_param->role = CONFIG_ROLE_AP;
        }

#if CFG_ROLE_LAUNCH
        rl_pre_sta_set_status(RL_STATUS_STA_LAUNCHED);
#endif
        reg = RF_HOLD_BY_STA_BIT;
        sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_CLR, &reg);
        break;

    default:
        ret = kGeneralErr;
        break;
    }

    return ret;
}

OSStatus bk_wlan_set_ip_status(IPStatusTypedef *inNetpara, WiFi_Interface inInterface)
{
    OSStatus ret = kNoErr;

    switch ( inInterface )
    {
    case BK_SOFT_AP :
        if ( uap_ip_is_start() )
        {
            uap_ip_down();
        }
        else
        {
            ret = kGeneralErr;
        }
        break;

    case BK_STATION :
        if ( sta_ip_is_start() )
        {
            sta_ip_down();
        }
        else
        {
            ret = kGeneralErr;
        }
        break;

    default:
        ret = kGeneralErr;
        break;
    }

    if ( ret == kNoErr )
    {
        ip_address_set(inInterface, inNetpara->dhcp, inNetpara->ip,
                       inNetpara->mask, inNetpara->gate, inNetpara->dns);
        if ( inInterface == BK_SOFT_AP )
        {
            uap_ip_is_start();
        }
        else if ( inInterface == BK_STATION )
        {
            sta_ip_start();
        }
    }

    return ret;
}

OSStatus bk_wlan_get_ip_status(IPStatusTypedef *outNetpara, WiFi_Interface inInterface)
{
    OSStatus ret = kNoErr;
    struct wlan_ip_config addr;

    os_memset(&addr, 0, sizeof(struct wlan_ip_config));

    switch ( inInterface )
    {
    case BK_SOFT_AP :
        net_get_if_addr(&addr, net_get_uap_handle());
        net_get_if_macaddr(outNetpara->mac, net_get_uap_handle());
        break;

    case BK_STATION :
        net_get_if_addr(&addr, net_get_sta_handle());
        net_get_if_macaddr(outNetpara->mac, net_get_sta_handle());
        break;

    default:
        ret = kGeneralErr;
        break;
    }

    if ( ret == kNoErr )
    {
        outNetpara->dhcp = addr.ipv4.addr_type;
        os_strcpy(outNetpara->ip, inet_ntoa(addr.ipv4.address));
        os_strcpy(outNetpara->mask, inet_ntoa(addr.ipv4.netmask));
        os_strcpy(outNetpara->gate, inet_ntoa(addr.ipv4.gw));
        os_strcpy(outNetpara->dns, inet_ntoa(addr.ipv4.dns1));
    }

    return ret;
}

OSStatus bk_wlan_get_link_status(LinkStatusTypeDef *outStatus)
{
	int val, ret = kNoErr;
	u8 vif_idx = 0, ssid_len;
	struct sm_get_bss_info_cfm *cfm = NULL;

	os_null_printf("bk_wlan_get_link_status\r\n");
	if((NULL == outStatus) || (!sta_ip_is_start()))
	{
		ret = kGeneralErr;
		goto get_exit;
	}

	outStatus->conn_state = mhdr_get_station_status();
	vif_idx = rwm_mgmt_vif_mac2idx((void *)&g_sta_param_ptr->own_mac);
	if(INVALID_VIF_IDX == vif_idx)
	{
		ret = kGeneralErr;
		goto get_exit;
	}

	cfm = os_malloc(sizeof(struct sm_get_bss_info_cfm));
	if(!cfm)
	{
		ret = kNoMemoryErr;
		goto get_exit;
	}

	val = rw_msg_get_bss_info(vif_idx, (void *)cfm);
	if(val)
	{
		ret = kGeneralErr;
		goto get_exit;
	}

	outStatus->wifi_strength = cfm->rssi;
	outStatus->channel = rw_ieee80211_get_chan_id(cfm->freq);
	outStatus->security = g_sta_param_ptr->cipher_suite;

	os_memcpy(outStatus->bssid, cfm->bssid, 6);
	ssid_len = MIN(SSID_MAX_LEN, os_strlen((char*)(cfm->ssid)));
	os_memcpy(outStatus->ssid, cfm->ssid, ssid_len);

	get_exit:
	if(cfm)
	{
		os_free(cfm);
	}

    return ret;
}

void bk_wlan_ap_para_info_get(network_InitTypeDef_ap_st *ap_info)
{
    IPStatusTypedef ap_ips;

    if((!ap_info)||(!uap_ip_is_start()))
    {
        return;
    }

    os_memcpy(ap_info->wifi_ssid,g_ap_param_ptr->ssid.array,g_ap_param_ptr->ssid.length);
    os_memcpy(ap_info->wifi_key,g_ap_param_ptr->key,g_ap_param_ptr->key_len);
    ap_info->channel = g_ap_param_ptr->chann;
    ap_info->security = g_ap_param_ptr->cipher_suite;

    bk_wlan_get_ip_status(&ap_ips,BK_SOFT_AP);
    os_memcpy(ap_info->local_ip_addr,ap_ips.ip,16);
    os_memcpy(ap_info->gateway_ip_addr,ap_ips.gate,16);
    os_memcpy(ap_info->net_mask,ap_ips.mask,16);
    os_memcpy(ap_info->dns_server_ip_addr,ap_ips.dns,16);

    ap_info->dhcp_mode = g_wlan_general_param->dhcp_enable;
}

uint32_t bk_wlan_get_INT_status(void)
{
    return platform_is_in_interrupt_context();
}

void bk_wlan_enable_lsig(void)
{
    hal_machw_allow_rx_rts_cts();
    phy_enable_lsig_intr();
}

void bk_wlan_disable_lsig(void)
{
    hal_machw_disallow_rx_rts_cts();
    phy_disable_lsig_intr();
}
/**
 * bit0: 1:mulicast_brdcast is not filte
 * bit1: 1:duplicate frame is not filte
**/
void bk_wlan_set_monitor_filter(unsigned char filter)
{
	g_monitor_is_not_filter = filter;
}

#if CFG_AP_MONITOR_COEXIST
/**
 * set AP+EZ mode
 * @val: 1 if enable AP+EZ, 0: pure EZ
 */
void bk_wlan_set_ap_monitor_coexist(int val)
{
	g_ap_monitor_coexist = !!val;
}

int bk_wlan_get_ap_monitor_coexist()
{
	return g_ap_monitor_coexist;
}
#endif

/** @brief  Start wifi monitor mode
 *
 *  @detail This function disconnect wifi station and softAP.
 *
 */
int bk_wlan_start_monitor(void)
{
#if CFG_AP_MONITOR_COEXIST
	if (bk_wlan_get_ap_monitor_coexist()) {
		/* AP+Monitor Mode, don't need to remove all interfaces */
		hal_machw_enter_monitor_mode();
	} else {
#endif
	uint32_t ret;

	os_printf("bk_wlan_stop-ap\r\n");
	bk_wlan_stop(BK_SOFT_AP);

	os_printf("bk_wlan_stop-sta\r\n");
	bk_wlan_stop(BK_STATION);

#if (SUPPORT_LSIG_MONITOR)
	lsig_init();
#endif

	UINT32 reg = RF_HOLD_BY_MONITOR_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_SET, &reg);

	bk_wlan_ap_init(0);
	rwnx_remove_added_interface();

	if(g_wlan_general_param)
		g_wlan_general_param->role = CONFIG_ROLE_NULL;

	os_printf("mm_enter_monitor_mode\r\n");
	ret = mm_enter_monitor_mode();
	ASSERT(0 == ret);

#if CFG_AP_MONITOR_COEXIST
	}
#endif

    return 0;
}

/** @brief  Stop wifi monitor mode **/
int bk_wlan_stop_monitor(void)
{
    if(g_monitor_cb)
    {
        g_monitor_cb = 0;
        hal_machw_exit_monitor_mode();
    }

	UINT32 reg = RF_HOLD_BY_MONITOR_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_CLR, &reg);

    return 0;
}

int bk_wlan_get_channel(void)
{
    int channel, freq;

    freq = rw_msg_get_channel(NULL);
    channel = rw_ieee80211_get_chan_id(freq);

    return channel;
}

/** @brief  Set the monitor channel
 *
 *  @detail This function change the monitor channel (from 1~13).
 *       it can change the channel dynamically, don't need restart monitor mode.
 */
int bk_wlan_set_channel_sync(int channel)
{
    rwnxl_reset_evt(0);
	UINT32 reg = RF_HOLD_BY_PHY_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_SET, &reg);
    rw_msg_set_channel(channel, PHY_CHNL_BW_20, NULL);

    return 0;
}

int bk_wlan_get_channel_with_band_width(int *channel, int *band_width)
{
    struct phy_channel_info info;

    phy_get_channel(&info, 0);

    *channel = rw_ieee80211_get_chan_id(info.info2);
    *band_width = (info.info1 >> 8) & 0xFF;

    return 0;
}

int bk_wlan_set_channel_with_band_width(int channel, int band_width)
{
    rwnxl_reset_evt(0);
	UINT32 reg = RF_HOLD_BY_PHY_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_SET, &reg);
    rw_msg_set_channel((uint32_t)channel, (uint32_t)band_width, NULL);

    return 0;
}

/** @brief  Set channel at the asynchronous method
 *
 *  @detail This function change the monitor channel (from 1~13).
 *       it can change the channel dynamically, don't need restart monitor mode.
 */
int bk_wlan_set_channel(int channel)
{
	wifi_country_t country;
	GLOBAL_INT_DECLARATION();

    if(0 == channel)
    {
        channel = 1;
    }

	rw_ieee80211_get_country(&country);
	if(channel < country.schan || channel > country.nchan)
	{
		return channel;
	}

	GLOBAL_INT_DISABLE();
	g_set_channel_postpone_num = channel;
	GLOBAL_INT_RESTORE();

	ke_evt_set(KE_EVT_RESET_BIT);

    return 0;
}

/** @brief  Register the monitor callback function
 *        Once received a 802.11 packet call the registered function to return the packet.
 */
void bk_wlan_register_monitor_cb(monitor_cb_t fn)
{
	if(fn)
	{
	    g_monitor_cb = fn;
	}
}

#if (SUPPORT_LSIG_MONITOR)
static int get_cipher_info(uint8_t *frame, int frame_len,
		uint8_t *pairwise_cipher_type)
{
	uint8_t cap = frame[24+10]; // 24 is mac header; 8 timestamp, 2 beacon interval;
	u8 is_privacy = !!(cap & 0x10); // bit 4 = privacy
	const u8 *ie = frame + 36; // 24 + 12
	u16 ielen = frame_len - 36;
	const u8 *tmp;
	int ret = 0;

	tmp = (uint8_t *)mac_ie_find((uint32_t)ie, ielen, WLAN_EID_RSN);
	if (tmp && tmp[1]) {
		*pairwise_cipher_type = WLAN_ENC_CCMP;
	} else {
		tmp = mac_vendor_ie_find(OUI_MICROSOFT, OUI_TYPE_MICROSOFT_WPA, ie, ielen);
		if (tmp) {
			*pairwise_cipher_type = WLAN_ENC_TKIP;
		} else {
			if (is_privacy) {
				*pairwise_cipher_type = WLAN_ENC_WEP;
			} else {
				*pairwise_cipher_type = WLAN_ENC_OPEN;
			}
		}
	}

	return ret;
}

static void bk_monitor_callback(uint8_t *data, int len, wifi_link_info_t *info)
{
    uint8_t enc_type;

    /* check the RTS packet */
    if ((data[0] == 0xB4) && (len == 16)) { // RTS
        rts_update(data, info->rssi, rtos_get_time());
        return;
    }
    /* check beacon/probe reponse packet */
    if ((data[0] == 0x80) || (data[0] == 0x50)) {
        get_cipher_info(data, len, &enc_type);
        beacon_update(&data[16], enc_type);
    }

    if (g_monitor_cb)
    {
        (*g_monitor_cb)(data, len, info);
    }
}
#endif

monitor_cb_t bk_wlan_get_monitor_cb(void)
{
    if (g_monitor_cb)
    {
#if (SUPPORT_LSIG_MONITOR)
        return bk_monitor_callback;
#else
    	return g_monitor_cb;
#endif
    }
    else
	{
        return NULL;
    }
}

int bk_wlan_monitor_filter(unsigned char filter_type)
{
	return ((g_monitor_is_not_filter&filter_type) == filter_type);
}

int bk_wlan_is_monitor_mode(void)
{
    return (0 == g_monitor_cb) ? false : true;
}

void wlan_ui_bcn_callback(uint8_t *data, int len, wifi_link_info_t *info)
{
#if CFG_USE_STA_PS
    if(power_save_if_ps_rf_dtim_enabled())
    {
        power_save_bcn_callback(data,len,info);
    }
#endif
#if CFG_USE_MCU_PS
    mcu_ps_bcn_callback(data,len,info);
#endif
}

void bk_wlan_register_bcn_cb(monitor_cb_t fn)
{
	g_bcn_cb = fn;
}

monitor_cb_t bk_wlan_get_bcn_cb(void)
{
    return g_bcn_cb;
}

extern void bmsg_ps_sender(uint8_t ioctl);

/** @brief  Request deep sleep,and wakeup by gpio.
 *
 *  @param  gpio_index_map:The gpio bitmap which set 1 enable wakeup deep sleep.
 *              gpio_index_map is hex and every bits is map to gpio0-gpio31.
 *          gpio_edge_map:The gpio edge bitmap for wakeup gpios,
 *              gpio_edge_map is hex and every bits is map to gpio0-gpio31.
 *              0:rising,1:falling.
 */
#if CFG_USE_DEEP_PS
void bk_enter_deep_sleep(UINT32 gpio_index_map,
								UINT32 gpio_edge_map,
								UINT32 gpio_last_index_map,
								UINT32 gpio_last_edge_map,
								UINT32 sleep_time,
								UINT32 wake_up_way,
								UINT32 gpio_stay_lo_map,
								UINT32 gpio_stay_hi_map)
{
	PS_DEEP_CTRL_PARAM deep_sleep_param;

	deep_sleep_param.gpio_index_map = gpio_index_map;
	deep_sleep_param.gpio_edge_map = gpio_edge_map;
	deep_sleep_param.gpio_last_index_map = gpio_last_index_map;
	deep_sleep_param.gpio_last_edge_map = gpio_last_edge_map;
	deep_sleep_param.sleep_time = sleep_time;
	deep_sleep_param.gpio_stay_lo_map = gpio_stay_lo_map;
	deep_sleep_param.gpio_stay_hi_map = gpio_stay_hi_map;
	switch(wake_up_way)
	{
	case PS_DEEP_WAKEUP_GPIO:
		deep_sleep_param.wake_up_way = PS_DEEP_WAKEUP_GPIO;
		break;

	case PS_DEEP_WAKEUP_RTC:
		deep_sleep_param.wake_up_way = PS_DEEP_WAKEUP_RTC;
		break;

	case PS_DEEP_WAKEUP_USB:
		deep_sleep_param.wake_up_way = PS_DEEP_WAKEUP_USB;
		break;

	default:
		deep_sleep_param.wake_up_way = PS_DEEP_WAKEUP_GPIO;
		break;
	}

	bk_enter_deep_sleep_mode(&deep_sleep_param);
}
#endif

/** @brief  When in dtim rf off mode,user can manual wakeup before dtim wakeup time.
 *          this function can not be called in "core_thread" context
 */
extern uint8_t ble_switch_mac_sleeped;
int bk_wlan_dtim_rf_ps_mode_do_wakeup()
{
#if CFG_USE_AP_IDLE
    if(bk_wlan_has_role(VIF_AP) && ap_ps_enable_get())
    {
        GLOBAL_INT_DECLARATION();
        GLOBAL_INT_DISABLE();
        UINT32 reg = RF_HOLD_BY_AP_BIT;
        sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_SET, &reg);
        wifi_general_mac_state_set_active();
        GLOBAL_INT_RESTORE();
    }
#endif

    void *sem_list = NULL;
	UINT32 ret = 0;

	sem_list = power_save_rf_ps_wkup_semlist_create();

	if (!sem_list)
    {
        os_printf("err ---- NULL\r\n");
        ASSERT(0);
    }

    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();

    if((power_save_if_ps_rf_dtim_enabled()
            && power_save_if_rf_sleep())
#if CFG_SUPPORT_BLE
            || ble_switch_mac_sleeped
#endif
            )
    {
        power_save_rf_ps_wkup_semlist_wait(sem_list);
    }
    else
    {
        power_save_rf_ps_wkup_semlist_destroy(sem_list);
        os_free(sem_list);
	sem_list = NULL;
    }

    GLOBAL_INT_RESTORE();

    power_save_rf_ps_wkup_semlist_get(sem_list);

    return ret;

}
#if CFG_USE_STA_PS
static uint32_t rf_ps_enabled = 0;
/** @brief  Enable dtim power save,close rf,and wakeup by ieee dtim dynamical
 *
 */
int bk_wlan_dtim_rf_ps_mode_enable(void )
{
        rf_ps_enabled = 1;
        bmsg_ps_sender(PS_BMSG_IOCTL_RF_ENABLE);
        return 0;
}

int bk_wlan_dtim_rf_ps_disable_send_msg(void)
{
#if 0
    if(power_save_if_ps_rf_dtim_enabled()
            && power_save_if_rf_sleep())
    {
        power_save_wkup_event_set(NEED_DISABLE_BIT);
    }
    else
    {
        power_save_dtim_rf_ps_disable_send_msg();
    }
#endif
    power_save_dtim_rf_ps_disable_send_msg();
    return 0;
}

/** @brief  Request exit dtim dynamical ps mode
 */
int bk_wlan_dtim_rf_ps_mode_disable(void)
{
    rf_ps_enabled = 0;
    if (bk_wlan_dtim_rf_ps_disable_send_msg())
		return 1;

    return 0;
}

#if PS_USE_KEEP_TIMER
int bk_wlan_dtim_rf_ps_timer_start(void)
{
    bmsg_ps_sender(PS_BMSG_IOCTL_RF_PS_TIMER_INIT);
    return 0;
}

int bk_wlan_dtim_rf_ps_timer_pause(void)
{
    bmsg_ps_sender(PS_BMSG_IOCTL_RF_PS_TIMER_DEINIT);
    return 0;
}
#endif

UINT32 bk_wlan_dtim_rf_ps_get_sleep_time(void)
{
    return  power_save_get_rf_ps_dtim_time();
}

int auto_check_dtim_rf_ps_mode(void )
{
    if(1 == rf_ps_enabled)
    {
        bmsg_ps_sender(PS_BMSG_IOCTL_RF_ENABLE);
    }

    return 0;
}

int bk_wlan_dtim_rf_ps_get_enable_flag(void)
{
	return rf_ps_enabled;
}
#endif

int bk_wlan_mcu_suppress_and_sleep(UINT32 sleep_ticks )
{
#if CFG_USE_MCU_PS
#if (CFG_OS_FREERTOS)
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	TickType_t missed_ticks = 0;

	missed_ticks = mcu_power_save( sleep_ticks );
	if(missed_ticks >= 0){
		fclk_update_tick( missed_ticks );
	} else{
		GLOBAL_INT_RESTORE();
		return -1;
	}
	GLOBAL_INT_RESTORE();
#endif
#endif

    return 0;
}

#if CFG_USE_MCU_PS
static uint32_t mcu_ps_enabled = 0;
/** @brief  Enable mcu power save,close mcu ,and wakeup by irq
 */
int bk_wlan_mcu_ps_mode_enable(void)
{
	mcu_ps_enabled = 1;
    bmsg_ps_sender(PS_BMSG_IOCTL_MCU_ENABLE);

    return 0;
}

/** @brief  Disable mcu power save mode
 */
int bk_wlan_mcu_ps_mode_disable(void)
{
	mcu_ps_enabled = 0;
    bmsg_ps_sender(PS_BMSG_IOCTL_MCU_DISANABLE);

    return 0;
}

int bk_wlan_mcu_ps_get_enable_flag(void)
{
	return mcu_ps_enabled;
}
#endif

BK_PS_LEVEL global_ps_level = 0;
int bk_wlan_power_save_set_level(BK_PS_LEVEL level)
{
    if(level & PS_DEEP_SLEEP_BIT)
    {
#if CFG_USE_STA_PS
        if(global_ps_level & PS_RF_SLEEP_BIT)
        {
            bk_wlan_dtim_rf_ps_mode_disable();
        }
#endif
#if CFG_USE_MCU_PS
        if(global_ps_level & PS_MCU_SLEEP_BIT)
        {
            bk_wlan_mcu_ps_mode_disable();
        }
#endif
        rtos_delay_milliseconds(100);
#if CFG_USE_DEEP_PS
        bk_enter_deep_sleep(0xc00, 0xc00, 0, 0, 5, 2, 0, 0);
#endif
    }

    if((global_ps_level & PS_RF_SLEEP_BIT) ^ (level & PS_RF_SLEEP_BIT))
    {
#if CFG_USE_STA_PS
        if(global_ps_level & PS_RF_SLEEP_BIT)
        {
            bk_wlan_dtim_rf_ps_mode_disable();
        }
        else
        {
            bk_wlan_dtim_rf_ps_mode_enable();
        }
#endif
    }

    if((global_ps_level & PS_MCU_SLEEP_BIT) ^ (level & PS_MCU_SLEEP_BIT))
    {
#if CFG_USE_MCU_PS
        if(global_ps_level & PS_MCU_SLEEP_BIT)
        {
            bk_wlan_mcu_ps_mode_disable();
        }
        else
        {
            bk_wlan_mcu_ps_mode_enable();
        }
#endif
    }

    global_ps_level = level;

    return 0;
}

void bk_wlan_status_register_cb(FUNC_1PARAM_PTR cb)
{
	connection_status_cb = cb;
}

FUNC_1PARAM_PTR bk_wlan_get_status_cb(void)
{
	return connection_status_cb;
}

OSStatus bk_wlan_ap_is_up(void)
{
#if CFG_ROLE_LAUNCH
    if(RL_STATUS_AP_INITING < rl_pre_ap_get_status())
    {
        return 1;
    }
#else
    ip_addr_t ip_addr;
    int result = 0;
    struct netif * netif = NULL;

    /* TODO: replace hardcoding with flexible way */
    netif = netif_find("ap1");
    if (!netif)
    {
        return -1;
    }

    ip_addr_set_zero(&ip_addr);
    if (ip_addr_cmp(&(netif->ip_addr), &ip_addr))
    {
        result = 0;
    }
    else
    {
        result = 1;
    }

    return result;
#endif

    return 0;
}

OSStatus bk_wlan_sta_is_connected(void)
{
#if CFG_ROLE_LAUNCH
    if(RL_STATUS_STA_LAUNCHED <= rl_pre_sta_get_status())
    {
        return 1;
    }
#else
    ip_addr_t ip_addr;
    int result = 0;
    struct netif * netif = NULL;

    /* TODO: replace hardcoding with flexible way */
    netif = netif_find("w00");
    if (!netif)
    {
        return -1;
    }

    ip_addr_set_zero(&ip_addr);
    if (ip_addr_cmp(&(netif->ip_addr), &ip_addr))
    {
        result = 0;
    }
    else
    {
        result = 1;
    }

    return result;
#endif

    return 0;
}

UINT32 if_other_mode_rf_sleep(void)
{
    if(!bk_wlan_has_role(VIF_MESH_POINT)
        &&!bk_wlan_has_role(VIF_IBSS)
        &&!bk_wlan_is_monitor_mode())
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


uint32_t bk_wlan_reg_rx_mgmt_cb(mgmt_rx_cb_t cb, uint32_t rx_mgmt_flag)
{
	return rxu_reg_mgmt_cb(cb, rx_mgmt_flag);
}

int bk_wlan_sync_send_raw_frame(uint8_t *buffer, int len)
{
	// TODO

	return 0;
}

int bk_wlan_send_raw_frame_with_cb(uint8_t *buffer, int len, void *cb, void *param)
{
	return bmsg_tx_raw_cb_sender(buffer, len, cb, param);
}

extern int bmsg_tx_raw_sender(uint8_t *payload, uint16_t length);
int bk_wlan_send_80211_raw_frame(uint8_t *buffer, int len)
{
	uint8_t *pkt;
	int ret;

	pkt = os_malloc(len);
	if (pkt == NULL)
	{
		return -1;
	}

	os_memcpy(pkt, buffer, len);
	ret = bmsg_tx_raw_sender(pkt, len);
	return ret;
}

void bk_wlan_register_mgnt_monitor_cb(monitor_cb_t fn)
{
	g_mgnt_cb = fn;

	if (fn != NULL) {
		mm_rx_filter_app_set(NXMAC_ACCEPT_PROBE_REQ_BIT);
	} else {
		mm_rx_filter_app_set(0);
	}
}

monitor_cb_t bk_wlan_get_mgnt_monitor_cb(void)
{
    return g_mgnt_cb;
}

#if CFG_SUPPORT_OTA_HTTP
int http_ota_download(const char *uri)
{
    int ret;
    httpclient_t httpclient;
    httpclient_data_t httpclient_data;
    char http_content[HTTP_RESP_CONTENT_LEN];

    os_memset(&httpclient, 0, sizeof(httpclient_t));
    os_memset(&httpclient_data, 0, sizeof(httpclient_data));
    os_memset(&http_content, 0, sizeof(HTTP_RESP_CONTENT_LEN));
    httpclient.header = "Accept: text/xml,text/html,\r\n";
    httpclient_data.response_buf = http_content;
    httpclient_data.response_content_len = HTTP_RESP_CONTENT_LEN;
    ret = httpclient_common(&httpclient,
                            uri,
                            80,/*port*/
                            NULL,
                            HTTPCLIENT_GET,
                            20000,
                            &httpclient_data);

    if (0 != ret) {
        os_printf("request epoch time from remote server failed.");
    } else {
        os_printf("sucess.\r\n");
        bk_reboot();
    }

    return ret;
}
#endif

void bk_wifi_get_station_mac_address(char *mac)
{
	wifi_get_mac_address((char *)mac, CONFIG_ROLE_STA);
}

void bk_wifi_get_softap_mac_address(char *mac)
{
	wifi_get_mac_address((char *)mac, CONFIG_ROLE_AP);
}

#if (CFG_SUPPORT_ALIOS)
/**********************for alios*******************************/

void bk_wifi_get_mac_address(char *mac)
{
	wifi_get_mac_address(mac, CONFIG_ROLE_STA);
}

void bk_wifi_set_mac_address(char *mac)
{
	wifi_set_mac_address(mac);
}

uint32_t bk_wlan_max_power_level_get(void)
{
	return nxmac_max_power_level_get();
}

OSStatus bk_wlan_get_bssid_info(apinfo_adv_t *ap, uint8_t **key, int *key_len)
{
	LinkStatusTypeDef link_stat;

    if( uap_ip_is_start() )
    {
        return kGeneralErr;
    }

	ap->security = g_sta_param_ptr->cipher_suite;
	os_memset(&link_stat, 0, sizeof(link_stat));
	bk_wlan_get_link_status(&link_stat);
	ap->channel = link_stat.channel;
	os_memcpy(ap->bssid, link_stat.bssid, 6);
	os_strcpy((char*)ap->ssid, (char*)link_stat.ssid);

	*key = g_sta_param_ptr->key;
	*key_len = g_sta_param_ptr->key_len;

	return 0;
}

#ifndef CONFIG_AOS_MESH
int wlan_is_mesh_monitor_mode(void)
{
    return false;
}

bool rxu_mesh_monitor(struct rx_swdesc *swdesc)
{
    return false;
}

monitor_cb_t wlan_get_mesh_monitor_cb(void)
{
    return NULL;
}
#else
void wlan_register_mesh_monitor_cb(monitor_cb_t fn)
{
    g_mesh_monitor_cb = fn;
}

monitor_cb_t wlan_get_mesh_monitor_cb(void)
{
    return g_mesh_monitor_cb;
}

int wlan_is_mesh_monitor_mode(void)
{
    if (g_mesh_monitor_cb) {
        return true;
    }
    return false;
}

int wlan_set_mesh_bssid(uint8_t *bssid)
{
    if (bssid == NULL) {
        return -1;
    }
    os_memcpy(g_mesh_bssid, bssid, 6);
    return 0;
}

uint8_t *wlan_get_mesh_bssid(void)
{
    return g_mesh_bssid;
}

bool rxu_mesh_monitor(struct rx_swdesc *swdesc)
{
    struct rx_dmadesc *dma_hdrdesc = swdesc->dma_hdrdesc;
    struct rx_hd *rhd = &dma_hdrdesc->hd;
    struct rx_payloaddesc *payl_d = HW2CPU(rhd->first_pbd_ptr);
    struct rx_cntrl_rx_status *rx_status = &rxu_cntrl_env.rx_status;
    uint32_t *frame = payl_d->buffer;
    struct mac_hdr *hdr = (struct mac_hdr *)frame;
    uint8_t *local_bssid;
    uint8_t *bssid;

    if (wlan_is_mesh_monitor_mode() == false)
	{
        return false;
    }

    if(MAC_FCTRL_DATA_T == (hdr->fctl & MAC_FCTRL_TYPE_MASK))
	{
        local_bssid = wlan_get_mesh_bssid();
        bssid = (uint8_t *)hdr->addr3.array;
        if (os_memcmp(local_bssid, bssid, 6) == 0) {
            return true;
        }
    }
	else if (MAC_FCTRL_ACK == (hdr->fctl & MAC_FCTRL_TYPESUBTYPE_MASK))
	{
        uint16_t local_addr[3];
        uint16_t *addr;
        uint32_t addr_low;
        uint16_t addr_high;

        addr = (uint16_t *)hdr->addr1.array;
        addr_low = nxmac_mac_addr_low_get();
        local_addr[0] = addr_low;
        local_addr[1] = (addr_low & 0xffff0000) >> 16;
        local_addr[2] = nxmac_mac_addr_high_getf();

        if (os_memcmp(local_addr, addr, 6) == 0)
		{
            return true;
        }
    }

    return false;
}

int wlan_get_mesh_condition(void)
{
	int ret = 0;

	if(bk_wlan_has_role(VIF_STA) || bk_wlan_has_role(VIF_AP))
	{
		ret = 1;
	}

	return ret;
}
#endif
#endif
// eof

