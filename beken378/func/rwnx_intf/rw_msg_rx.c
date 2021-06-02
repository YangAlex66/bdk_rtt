#include "include.h"
#include "rw_msg_rx.h"
#include "rw_pub.h"
#include "ke_msg.h"
#include "mem_pub.h"
#include "mac_common.h"
#include "scanu_task.h"
#include "sa_station.h"
#include "apm_task.h"
#include "me_task.h"
#include "sm_task.h"
#include "hostapd_intf_pub.h"
#include "mac_ie.h"
#include "ieee802_11_defs.h"
#include "wlan_ui_pub.h"
#include "mcu_ps_pub.h"
#include "driver.h"
#include "drv_model_pub.h"
#if CFG_WPA_CTRL_IFACE
#include "signal.h"
#include "ctrl_iface.h"
#endif

#if CFG_ROLE_LAUNCH
#include "role_launch.h"
#endif
#if CFG_WPA_CTRL_IFACE
#include "wpa_ctrl.h"
#include "notifier_pub.h"
#endif
#include "rxu_task.h"
#include "main_none.h"
#include "sys_ctrl_pub.h"

uint32_t resultful_scan_cfm = 0;
uint8_t *ind_buf_ptr = 0;
struct co_list rw_msg_rx_head;
struct co_list rw_msg_tx_head;
rw_evt_type connect_flag = RW_EVT_STA_IDLE;
SCAN_RST_UPLOAD_T *scan_rst_set_ptr = 0;

#if CFG_WPA_CTRL_IFACE
IND_CALLBACK_T scan_cfm_cb_user = {0};
#endif
#if !CFG_WPA_CTRL_IFACE
IND_CALLBACK_T scan_cfm_cb[2] = {0};
#else
IND_CALLBACK_T scan_cfm_cb = {0};
#endif
IND_CALLBACK_T assoc_cfm_cb = {0};
IND_CALLBACK_T deassoc_evt_cb = {0};
IND_CALLBACK_T deauth_evt_cb = {0};
IND_CALLBACK_T wlan_connect_user_cb = {0};

extern FUNC_1PARAM_PTR bk_wlan_get_status_cb(void);
extern void app_set_sema(void);
extern int get_security_type_from_ie(u8 *, int, u16);
extern void rwnx_cal_set_txpwr(UINT32 pwr_gain, UINT32 grate);
extern void bk7011_default_rxsens_setting(void);

/* scan result malloc item */
UINT8 *sr_malloc_result_item(UINT32 vies_len)
{
    return os_zalloc(vies_len + sizeof(struct sta_scan_res));
}

/* free scan result item */
void sr_free_result_item(UINT8 *item_ptr)
{
    os_free(item_ptr);
}

UINT8 *sr_malloc_shell(void)
{
    UINT8 *ptr;
    UINT32 layer1_space_len;
    UINT32 layer2_space_len;

    layer1_space_len = sizeof(SCAN_RST_UPLOAD_T);
    layer2_space_len = MAX_BSS_LIST * sizeof(struct sta_scan_res *);
    ptr = os_zalloc(layer1_space_len + layer2_space_len);

	if(ptr)
	{
		return ptr;
	}
	else
	{
    	os_printf("sr_malloc fail \r\n");
		return 0;
	}
}

void sr_free_shell(UINT8 *shell_ptr)
{
    os_free(shell_ptr);
}

void sr_free_all(SCAN_RST_UPLOAD_T *scan_rst)
{
    UINT32 i;

    for(i = 0; i < scan_rst->scanu_num; i ++)
    {
        sr_free_result_item((UINT8 *)scan_rst->res[i]);
        scan_rst->res[i] = 0;
    }
    scan_rst->scanu_num = 0;
	scan_rst->ref = 0;

    sr_free_shell((UINT8 *)scan_rst);
}

uint32_t sr_get_scan_number(void)
{
	uint32_t count = 0;
	GLOBAL_INT_DECLARATION();

	GLOBAL_INT_DISABLE();
	if(scan_rst_set_ptr)
	{
		count = scan_rst_set_ptr->scanu_num;
	}
	GLOBAL_INT_RESTORE();

	return count;
}

/* Attention: sr_get_scan_results and sr_release_scan_results have to come in pairs*/
void *sr_get_scan_results(void)
{
	void *ptr;
	GLOBAL_INT_DECLARATION();

	GLOBAL_INT_DISABLE();
	ptr = scan_rst_set_ptr;
	scan_rst_set_ptr->ref += 1;
	GLOBAL_INT_RESTORE();

    return ptr;
}

void sr_flush_scan_results(SCAN_RST_UPLOAD_PTR ptr)
{
	GLOBAL_INT_DECLARATION();

	GLOBAL_INT_DISABLE();
	ptr->ref = 1;
	sr_release_scan_results(ptr);
	GLOBAL_INT_RESTORE();
}

/* Attention: sr_get_scan_results and sr_release_scan_results have to come in pairs*/
void sr_release_scan_results(SCAN_RST_UPLOAD_PTR ptr)
{
	GLOBAL_INT_DECLARATION();

	GLOBAL_INT_DISABLE();
	if((0 == ptr) || (0 == ptr->ref))
	{
		os_printf("released_scan_results\r\n");
		goto release_exit;
	}

	ptr->ref -= 1;

	if(ptr->ref)
	{
		os_printf("release_scan_results later\r\n");
		goto release_exit;
	}

    if(ptr)
    {
        sr_free_all(ptr);
    }

    scan_rst_set_ptr = 0;
	resultful_scan_cfm = 0;

	wpa_clear_scan_results();

release_exit:
	GLOBAL_INT_RESTORE();
	return;
}

void mr_kmsg_init(void)
{
    co_list_init(&rw_msg_tx_head);
    co_list_init(&rw_msg_rx_head);
}

UINT32 mr_kmsg_fwd(struct ke_msg *msg)
{
    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    co_list_push_back(&rw_msg_rx_head, &msg->hdr);
    GLOBAL_INT_RESTORE();

    app_set_sema();

    return 0;
}

void mr_kmsg_flush(void)
{
    while(mr_kmsg_fuzzy_handle());
}

UINT32 mr_kmsg_fuzzy_handle(void)
{
    UINT32 ret = 0;
    struct ke_msg *msg;
    struct co_list_hdr *node;

    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    node = co_list_pop_front(&rw_msg_rx_head);
    GLOBAL_INT_RESTORE();

    if(node)
    {
        msg = (struct ke_msg *)node;
        ke_msg_free(msg);

        ret = 1;
    }

    return ret;
}

UINT32 mr_kmsg_exact_handle(UINT16 rsp)
{
    UINT32 ret = 0;
    struct ke_msg *msg;
    struct co_list_hdr *node;

    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    node = co_list_pop_front(&rw_msg_rx_head);
    GLOBAL_INT_RESTORE();

    if(node)
    {
        msg = (struct ke_msg *)node;
        if(rsp == msg->id)
        {
            ret = 1;
        }
        ke_msg_free(msg);
    }

    return ret;
}

void mhdr_connect_user_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
    wlan_connect_user_cb.cb = ind_cb;
    wlan_connect_user_cb.ctxt_arg = ctxt;
}

void mhdr_assoc_cfm_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
    assoc_cfm_cb.cb = ind_cb;
    assoc_cfm_cb.ctxt_arg = ctxt;
}

#if CFG_WPA_CTRL_IFACE
void scanu_notifier_func(void *cxt, int type, int value)
{
	//os_printf("%s: type %d, cb %p, value %d\r\n", __func__, type, scan_cfm_cb_user.cb, value);
	if (type != WLAN_EVENT_SCAN_RESULTS || !scan_cfm_cb_user.cb || !value)
		return;
	scan_cfm_cb_user.cb(cxt, (uint8_t)value);
}

void mhdr_scanu_reg_cb_for_wpa(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
    scan_cfm_cb.cb = ind_cb;
    scan_cfm_cb.ctxt_arg = ctxt;
}

void mhdr_scanu_reg_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
    scan_cfm_cb_user.cb = ind_cb;
    scan_cfm_cb_user.ctxt_arg = ctxt;
	wlan_register_notifier(scanu_notifier_func, ctxt);
}

void mhdr_scanu_reg_cb_handle(struct scanu_start_cfm *cfm)
{
	if(scan_cfm_cb.cb)
	{
		(*scan_cfm_cb.cb)(scan_cfm_cb.ctxt_arg, cfm->vif_idx);
	}
}
#else	/* !CFG_WPA_CTRL_IFACE */

void mhdr_scanu_reg_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
	int i;

	for(i=0;i<(sizeof(scan_cfm_cb)/sizeof(IND_CALLBACK_T));i++)
	{
		if((scan_cfm_cb[i].cb == ind_cb)
			&&(scan_cfm_cb[i].ctxt_arg == ctxt))
		{
			return;
		}
	}

	for(i=0;i<(sizeof(scan_cfm_cb)/sizeof(IND_CALLBACK_T));i++)
	{
		if(scan_cfm_cb[i].cb == NULL)
		{
			scan_cfm_cb[i].cb = ind_cb;
			scan_cfm_cb[i].ctxt_arg = ctxt;
			return;
		}
	}
}

void mhdr_scanu_reg_cb_clean(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
	int i;

	for(i = 0; i < (sizeof(scan_cfm_cb)/sizeof(IND_CALLBACK_T)); i++){
		if((scan_cfm_cb[i].cb == ind_cb)
			||(scan_cfm_cb[i].ctxt_arg == ctxt)){
			scan_cfm_cb[i].ctxt_arg = NULL;
			scan_cfm_cb[i].cb = NULL;
			return;
		}
	}
}


void mhdr_scanu_reg_cb_handle(struct scanu_start_cfm *cfm)
{
	IND_CALLBACK_T _scan_cfm_cb[2];

	_scan_cfm_cb[0] = scan_cfm_cb[0];
	_scan_cfm_cb[1] = scan_cfm_cb[1];
	scan_cfm_cb[0].cb = NULL;
	scan_cfm_cb[0].ctxt_arg = NULL;
	scan_cfm_cb[1].cb = NULL;
	scan_cfm_cb[1].ctxt_arg = NULL;

	if(_scan_cfm_cb[0].cb)
	{
		(*_scan_cfm_cb[0].cb)(_scan_cfm_cb[0].ctxt_arg, cfm->vif_idx);
	}

	if(_scan_cfm_cb[1].cb)
	{
		(*_scan_cfm_cb[1].cb)(_scan_cfm_cb[1].ctxt_arg, cfm->vif_idx);
	}
}
#endif



void mhdr_deauth_evt_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
    deauth_evt_cb.cb = ind_cb;
    deauth_evt_cb.ctxt_arg = ctxt;
}

void mhdr_deassoc_evt_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
    deassoc_evt_cb.cb = ind_cb;
    deassoc_evt_cb.ctxt_arg = ctxt;
}

void mhdr_disconnect_ind(void *msg)
{
    struct ke_msg *msg_ptr;
    struct sm_disconnect_ind *disc;

    msg_ptr = (struct ke_msg *)msg;
    disc = (struct sm_disconnect_ind *)msg_ptr->param;

    os_printf("%s reason_code=%d\n", __FUNCTION__, disc->reason_code);
    switch (disc->reason_code)
    {
        case WLAN_REASON_PREV_AUTH_NOT_VALID:
        case WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT:
            mhdr_set_station_status(RW_EVT_STA_PASSWORD_WRONG);
            break;

        case WLAN_REASON_DEAUTH_LEAVING:
        default:
            mhdr_set_station_status(RW_EVT_STA_CONNECT_FAILED);
            break;
    }

#if CFG_ROLE_LAUNCH
	if(rl_sta_req_is_null())
	{
		rl_sta_cache_request_enter();
	}
	else if(msg && disc
				&& (VENDOR_CONNECTION_LOSS == disc->reason_code))
	{
		os_printf("VENDOR_CONNECTION_LOSS\r\n");
		rl_sta_cache_request_enter();
	}
	else if(rl_pre_sta_get_status() >= RL_STATUS_STA_SCANNING)
	{
		rl_sta_cache_request_enter();
	}
	else if(deassoc_evt_cb.cb)
#elif !CFG_WPA_CTRL_IFACE
    sa_reconnect_init();
    nxmac_pwr_mgt_setf(0);

    if(deassoc_evt_cb.cb)
#endif
	{
		os_printf("deassoc_evt_cb\r\n");
        (*deassoc_evt_cb.cb)(deassoc_evt_cb.ctxt_arg, disc->vif_idx);
    }
}

#ifdef CONFIG_SME
/* SM_ASSOCIATE_IND handler */
void mhdr_assoc_ind(void *msg, UINT32 len)
{
	struct ke_msg *msg_ptr;
	struct sm_assoc_indication *ind;
	void *p;

	msg_ptr = (struct ke_msg *)msg;
	ind = (struct sm_assoc_indication *)msg_ptr->param;

	/* send to wpa_s via ctrl iface */
	wpa_ctrl_event_copy(WPA_CTRL_EVENT_ASSOC_IND, ind, sizeof(*ind));

	if (0 == ind->status_code) {
		os_printf("---------SM_ASSOC_IND_ok\r\n");

		bk7011_default_rxsens_setting();

		if (wlan_connect_user_cb.cb)
			(*wlan_connect_user_cb.cb)(wlan_connect_user_cb.ctxt_arg, 0);
	}

	mcu_prevent_clear(MCU_PS_CONNECT);

    UINT32 reg = RF_HOLD_BY_CONNECT_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_CLR, &reg);
}

/* SM_AUTH_IND handler, send it to wpa_s */
void mhdr_auth_ind(void *msg, UINT32 len)
{
    struct ke_msg *msg_ptr;
    struct sm_auth_indication *ind;

    msg_ptr = (struct ke_msg *)msg;
    ind = (struct sm_auth_indication *)msg_ptr->param;

	wpa_ctrl_event_copy(WPA_CTRL_EVENT_AUTH_IND, ind, sizeof(*ind));
}


#else /* !CONFIG_SME */
void mhdr_connect_ind(void *msg, UINT32 len)
{
	struct ke_msg *msg_ptr;
	struct sm_connect_ind *conn_ind_ptr;

	msg_ptr = (struct ke_msg *)msg;
	conn_ind_ptr = (struct sm_connect_ind *)msg_ptr->param;

#if !CFG_WPA_CTRL_IFACE
	if (0 == conn_ind_ptr->status_code) {
		os_printf("---------SM_CONNECT_IND_ok\r\n");

		bk7011_default_rxsens_setting();
		if (assoc_cfm_cb.cb)
			(*assoc_cfm_cb.cb)(assoc_cfm_cb.ctxt_arg, conn_ind_ptr->vif_idx);

		if (wlan_connect_user_cb.cb)
			(*wlan_connect_user_cb.cb)(wlan_connect_user_cb.ctxt_arg, 0);
	} else {
		os_printf("---------SM_CONNECT_IND_fail\r\n");
		mhdr_disconnect_ind(msg);
	}
#else
	if (0 == conn_ind_ptr->status_code) {
		os_printf("---------SM_CONNECT_IND_ok\n");

		bk7011_default_rxsens_setting();

		if (wlan_connect_user_cb.cb)
			(*wlan_connect_user_cb.cb)(wlan_connect_user_cb.ctxt_arg, 0);
	} else {
		os_printf("---------SM_CONNECT_IND_fail\n");
	}

	/* Send to wpa_supplicant */
	wpa_ctrl_event_copy(WPA_CTRL_EVENT_CONNECT_IND, conn_ind_ptr, sizeof(*conn_ind_ptr));
#endif

	mcu_prevent_clear(MCU_PS_CONNECT);
	UINT32 reg = RF_HOLD_BY_CONNECT_BIT;
	sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_CLR, &reg);
}

#endif

/* RXU_MGT_IND handler, send it to wpa_s */
void mhdr_mgmt_ind(void *msg, UINT32 len)
{
    struct ke_msg *msg_ptr = (struct ke_msg *)msg;
    struct rxu_mgt_ind *ind = (struct rxu_mgt_ind *)msg_ptr->param;

#if CFG_WPA_CTRL_IFACE
	wpa_ctrl_event_copy(WPA_CTRL_EVENT_MGMT_IND, ind, sizeof(*ind) + ind->length);
#else
	/* FIXME: DON'T CALL IN RWNX_MSG THREAD */
	union wpa_event_data data;
	struct wpa_supplicant *wpa_s = wpa_suppliant_ctrl_get_wpas();

	os_memset(&data, 0, sizeof(data));
	data.rx_mgmt.ssi_signal = ind->rssi;
	data.rx_mgmt.frame = (u8 *)ind->payload;
	data.rx_mgmt.frame_len = ind->length;
	data.rx_mgmt.freq = ind->center_freq;

	//print_hex_dump("MGMT: ", ind->payload, ind->length);
	if (wpa_s)
		wpa_supplicant_event_sta(wpa_s, EVENT_RX_MGMT, &data);
#endif
}

void mhdr_set_station_status(rw_evt_type val)
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    connect_flag = val;
#if (CFG_SUPPORT_ALIOS)
	if(val > RW_EVT_STA_CONNECTING && val < RW_EVT_STA_CONNECTED)
	{
		connect_fail_cb(val);
	}
#endif
    GLOBAL_INT_RESTORE();
}

rw_evt_type mhdr_get_station_status(void)
{
    return connect_flag;
}

static void sort_scan_result(SCAN_RST_UPLOAD_T *ap_list)
{
    int i, j;
    struct sta_scan_res *tmp;

    if (ap_list->scanu_num == 0)
        return;

    for(i = 0; i < (ap_list->scanu_num - 1); i++)
    {
        for(j = i + 1; j < ap_list->scanu_num; j++)
        {
            if (ap_list->res[j]->level > ap_list->res[i]->level)
            {
                tmp = ap_list->res[j];
                ap_list->res[j] = ap_list->res[i];
                ap_list->res[i] = tmp;
            }
        }
    }
}

UINT32 mhdr_scanu_start_cfm(void *msg, SCAN_RST_UPLOAD_T *ap_list)
{
    struct scanu_start_cfm *cfm;
    struct ke_msg *msg_ptr;

    msg_ptr = (struct ke_msg *)msg;
    cfm = (struct scanu_start_cfm *)msg_ptr->param;

    if(ap_list)
    {
        sort_scan_result(ap_list);
    }

	mhdr_scanu_reg_cb_handle(cfm);

    return RW_SUCCESS;
}


UINT32 mhdr_scanu_result_ind(SCAN_RST_UPLOAD_T *scan_rst, void *msg, UINT32 len)
{
    UINT32 ret, chann;
    UINT8 *elmt_addr;
    UINT32 vies_len, i;
    UINT8 *var_part_addr;
    struct ke_msg *msg_ptr;
    SCAN_RST_ITEM_PTR item;
    SCAN_RST_UPLOAD_PTR result_ptr;
    SCAN_IND_PTR scanu_ret_ptr;
    IEEE802_11_PROBE_RSP_PTR probe_rsp_ieee80211_ptr;
    char on_channel;
    int replace_index = -1;

    ret = RW_SUCCESS;
    result_ptr = scan_rst;

    if (result_ptr->scanu_num >= MAX_BSS_LIST)
        goto scan_rst_exit;

    msg_ptr = (struct ke_msg *)msg;
    scanu_ret_ptr = (SCAN_IND_PTR)msg_ptr->param;
    probe_rsp_ieee80211_ptr =  (IEEE802_11_PROBE_RSP_PTR)scanu_ret_ptr->payload;
    vies_len = scanu_ret_ptr->length - MAC_BEACON_VARIABLE_PART_OFT;
    var_part_addr = probe_rsp_ieee80211_ptr->rsp.variable;

    elmt_addr = (UINT8 *)mac_ie_find((UINT32)var_part_addr, (UINT16)vies_len, MAC_ELTID_DS);
    if(elmt_addr) // adjust channel
    {
        chann = *(elmt_addr + MAC_DS_CHANNEL_OFT);
        if(rw_ieee80211_is_scan_rst_in_countrycode(chann) == 0)
        {
            elmt_addr = (UINT8 *)mac_ie_find((UINT32)var_part_addr,
                                             (UINT16)vies_len,
                                             MAC_ELTID_SSID);
            if(elmt_addr)
            {
                UINT8 ssid_b[MAC_SSID_LEN];
                UINT8 ssid_len = *(elmt_addr + MAC_SSID_LEN_OFT);

                if (ssid_len > MAC_SSID_LEN)
                    ssid_len = MAC_SSID_LEN;


                os_memcpy(ssid_b, elmt_addr + MAC_SSID_SSID_OFT, ssid_len);
                os_printf("drop: %s, chan:%d\r\n", ssid_b, chann);
            }

            goto scan_rst_exit;
        }

        if (chann == rw_ieee80211_get_chan_id(scanu_ret_ptr->center_freq))
            on_channel = 1;
        else
            on_channel = 0;
    }
    else
    {
        chann = rw_ieee80211_get_chan_id(scanu_ret_ptr->center_freq);
        on_channel = 0;
        os_printf("scan rst no ds param, drop it?\r\n");
    }

    /* check the duplicate bssid*/
    do
    {
        for(i = 0; i < result_ptr->scanu_num; i ++)
        {
            if(!os_memcmp(probe_rsp_ieee80211_ptr->bssid, result_ptr->res[i]->bssid, ETH_ALEN))
            {
                if ((result_ptr->res[i]->on_channel == 1) || (on_channel == 0))
                {
                    goto scan_rst_exit;
                }
                else
                {
                    replace_index = i; // should replace it.
                }
            }
        }
    }
    while(0);

    item = (SCAN_RST_ITEM_PTR)sr_malloc_result_item(vies_len);
    if (item == NULL)
        goto scan_rst_exit;

    elmt_addr = (UINT8 *)mac_ie_find((UINT32)var_part_addr,
                                     (UINT16)vies_len,
                                     MAC_ELTID_SSID);
    if(elmt_addr)
    {
        UINT8 ssid_len = *(elmt_addr + MAC_SSID_LEN_OFT);

        if (ssid_len > MAC_SSID_LEN)
            ssid_len = MAC_SSID_LEN;

        os_memcpy(item->ssid, elmt_addr + MAC_SSID_SSID_OFT, ssid_len);
    }
    else
    {
        os_printf("NoSSid\r\n");
    }

    os_memcpy(item->bssid, probe_rsp_ieee80211_ptr->bssid, ETH_ALEN);
    item->channel = chann;
    item->beacon_int = probe_rsp_ieee80211_ptr->rsp.beacon_int;
    item->caps = probe_rsp_ieee80211_ptr->rsp.capab_info;
    item->level = scanu_ret_ptr->rssi;
    item->on_channel = on_channel;

    os_memcpy(item->tsf, probe_rsp_ieee80211_ptr->rsp.timestamp, 8);

    item->ie_len = vies_len;
    os_memcpy(item + 1, var_part_addr, vies_len);

    item->security = get_security_type_from_ie((u8 *)var_part_addr, vies_len, item->caps);

    if (replace_index >= 0)
    {
        sr_free_result_item((UINT8 *)result_ptr->res[replace_index]);
        result_ptr->res[replace_index] = item;
    }
    else
    {
        result_ptr->res[result_ptr->scanu_num] = item;
        result_ptr->scanu_num ++;
    }

scan_rst_exit:
    return ret;
}

void rwnx_handle_recv_msg(struct ke_msg *rx_msg)
{
	uint32_t param;
	FUNC_1PARAM_PTR fn = bk_wlan_get_status_cb();

	switch (rx_msg->id) {

	/**************************************************************************/
	/*                          scan_hdlrs                                    */
	/**************************************************************************/
	case SCANU_START_CFM:
		/* scan complete */
#if CFG_ROLE_LAUNCH
		rl_pre_sta_set_status(RL_STATUS_STA_SCAN_OVER);
#endif
		if (scan_rst_set_ptr)
		{
			/* scan activity has valid result*/
			resultful_scan_cfm = 1;
		}

		mhdr_scanu_start_cfm(rx_msg, scan_rst_set_ptr);
        UINT32 reg = RF_HOLD_BY_SCAN_BIT;
        sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_CLR, &reg);
		break;

	case SCANU_RESULT_IND:
		/* scan result indication */
		if (resultful_scan_cfm && scan_rst_set_ptr) {
			sr_flush_scan_results(scan_rst_set_ptr);

			scan_rst_set_ptr = 0;
			resultful_scan_cfm = 0;
		}

		if (0 == scan_rst_set_ptr) {
            scan_rst_set_ptr = (SCAN_RST_UPLOAD_T *)sr_malloc_shell();
			if(scan_rst_set_ptr){
			    scan_rst_set_ptr->scanu_num = 0;
            	scan_rst_set_ptr->res = (SCAN_RST_ITEM_PTR*)&scan_rst_set_ptr[1];
				mhdr_scanu_result_ind(scan_rst_set_ptr, rx_msg, rx_msg->param_len);
			}
			else{
				os_printf("scan_rst_set_ptr malloc fail\r\n");
			}
		}
		else{
				mhdr_scanu_result_ind(scan_rst_set_ptr, rx_msg, rx_msg->param_len);
		}

		break;

		/**************************************************************************/
		/*							sm_hdlrs								      */
		/**************************************************************************/
#ifdef CONFIG_SME
	case SM_AUTH_IND:
		/* authentication indication */
		mhdr_auth_ind(rx_msg, rx_msg->param_len);
		break;

	case SM_ASSOCIATE_IND:
		/* association indication */
		mhdr_assoc_ind(rx_msg, rx_msg->param_len);
		break;

#else /* !CONFIG_SME*/
	case SM_CONNECT_IND:
		/* connect indication */
		if (scan_rst_set_ptr) {
			sr_release_scan_results(scan_rst_set_ptr);

			scan_rst_set_ptr = 0;
			resultful_scan_cfm = 0;
		}

		mhdr_connect_ind(rx_msg, rx_msg->param_len);
		break;

#endif /* CONFIG_SME */

#if defined(CFG_IEEE80211W) || defined(CONFIG_SME)
	case RXU_MGT_IND:
		// STA mgmt: FIXME: AP may sends RXU_MGT_IND?
		mhdr_mgmt_ind(rx_msg, rx_msg->param_len);
		break;
#endif

	case SM_DISCONNECT_IND:
		/* disconnect indication */
		os_printf("SM_DISCONNECT_IND\r\n");
		mhdr_disconnect_ind(rx_msg);
		extern UINT32 rwnx_sys_is_enable_hw_tpc(void);
		if (rwnx_sys_is_enable_hw_tpc() == 0)
			rwnx_cal_set_txpwr(20, 11);

#if CFG_ROLE_LAUNCH
		rl_pre_sta_set_status(RL_STATUS_STA_LAUNCH_FAILED);
#endif
		break;

	case SM_CONNCTION_START_IND:
		if (fn) {
			param = RW_EVT_STA_CONNECTING;
			(*fn)(&param);
		}
		mhdr_set_station_status(RW_EVT_STA_CONNECTING);

#if (RF_USE_POLICY == BLE_DEFAULT_WIFI_REQUEST)
		wifi_station_status_event_notice(0, RW_EVT_STA_CONNECTING);
#endif
		break;

	case SM_BEACON_LOSE_IND:
		if (fn) {
			param = RW_EVT_STA_BEACON_LOSE;
			(*fn)(&param);
		}

#if (RF_USE_POLICY == BLE_DEFAULT_WIFI_REQUEST)
		wifi_station_status_event_notice(0, RW_EVT_STA_BEACON_LOSE);
#endif
		mhdr_set_station_status(RW_EVT_STA_BEACON_LOSE);
		break;

	case MM_TAGGED_PARAM_CHANGE:
		bk_printf("[wzl]MM_TAGGED_PARAM_CHANGE\r\n");

#if RL_SUPPORT_FAST_CONNECT
		rl_clear_bssid_info();
#endif
		break;

	case SM_DISASSOC_IND: {
		struct sm_fail_stat *status_ind;

		status_ind = (struct sm_fail_stat *)rx_msg->param;
		switch (status_ind->status) {
		case WLAN_REASON_MICHAEL_MIC_FAILURE:
			param = RW_EVT_STA_PASSWORD_WRONG;

#if RL_SUPPORT_FAST_CONNECT
			rl_clear_bssid_info();
#endif
			break;

		default:
			param = RW_EVT_STA_DISCONNECTED;
			break;
		}
		mhdr_set_station_status(param);

		if (fn)
			(*fn)(&param);
	}

	break;

	case SM_AUTHEN_FAIL_IND: {
		struct sm_fail_stat *status_ind;

		status_ind = (struct sm_fail_stat *)rx_msg->param;
		switch (status_ind->status) {
		case WLAN_REASON_PREV_AUTH_NOT_VALID:
		case WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT:
			param = RW_EVT_STA_PASSWORD_WRONG;
			break;

		case MAC_RS_DIASSOC_TOO_MANY_STA:
			param = RW_EVT_STA_ASSOC_FULL;
			break;

		case WLAN_REASON_DEAUTH_LEAVING:
		default:
			param = RW_EVT_STA_CONNECT_FAILED;
			break;
		}
		if (fn)
			(*fn)(&param);
		mhdr_set_station_status(param);

#if (RF_USE_POLICY == BLE_DEFAULT_WIFI_REQUEST)
		wifi_station_status_event_notice(0, param);
#endif
	}
	break;

	case SM_ASSOC_FAIL_INID: {
		struct sm_fail_stat *assoc_state;

		assoc_state = (struct sm_fail_stat *)rx_msg->param;
		if (assoc_state->status == MAC_ST_ASSOC_TOO_MANY_STA)
			param = RW_EVT_STA_ASSOC_FULL;
		else
			param = RW_EVT_STA_DISCONNECTED;
		if (fn)
			(*fn)(&param);
		mhdr_set_station_status(param);

#if (RF_USE_POLICY == BLE_DEFAULT_WIFI_REQUEST)
		wifi_station_status_event_notice(0, param);
#endif
	}
	break;

	case SM_ASSOC_IND:
		if (fn) {
			param = RW_EVT_STA_CONNECTED;
			(*fn)(&param);
		}
		if (mhdr_get_station_status() < RW_EVT_STA_CONNECTED)
		{
			mhdr_set_station_status(RW_EVT_STA_CONNECTED);

#if (RF_USE_POLICY == BLE_DEFAULT_WIFI_REQUEST)
			wifi_station_status_event_notice(0, RW_EVT_STA_CONNECTED);
#endif
		}
		break;

	case APM_ASSOC_IND:
		if (fn) {
			param = RW_EVT_AP_CONNECTED;
			(*fn)(&param);
		}
		break;

	case APM_DEASSOC_IND:
		if (fn) {
			param = RW_EVT_AP_DISCONNECTED;
			(*fn)(&param);
		}
		break;

	case APM_ASSOC_FAILED_IND:
		if (fn) {
			param = RW_EVT_AP_CONNECT_FAILED;
			(*fn)(&param);
		}
		break;

#if CFG_USE_AP_PS
	case MM_PS_CHANGE_IND:
		rwm_msdu_ps_change_ind_handler(rx_msg);
		break;
#endif

	/**************************************************************************/
	/*							me_hdlrs									  */
	/**************************************************************************/
	case ME_TKIP_MIC_FAILURE_IND:
		break;

	/**************************************************************************/
	/*							mm_hdlrs									  */
	/**************************************************************************/
	case MM_CHANNEL_SWITCH_IND:
		break;
	case MM_CHANNEL_PRE_SWITCH_IND:
		break;
	case MM_REMAIN_ON_CHANNEL_EXP_IND:
		break;
	case MM_PS_CHANGE_IND:
		break;
	case MM_TRAFFIC_REQ_IND:
		break;
	case MM_P2P_VIF_PS_CHANGE_IND:
		break;
	case MM_CSA_COUNTER_IND:
		break;
	case MM_CSA_FINISH_IND:
		break;
	case MM_CSA_TRAFFIC_IND:
		break;
	case MM_CHANNEL_SURVEY_IND:
		break;
	case MM_P2P_NOA_UPD_IND:
		break;

	case MM_RSSI_STATUS_IND:
		//NL80211_ATTR_CQM_RSSI_THRESHOLD_EVENT
		break;

#ifdef CONFIG_SAE_EXTERNAL
	case SM_EXTERNAL_AUTH_REQUIRED_IND: {
	    struct ke_msg *msg_ptr;
	    struct sm_external_auth_required_ind *ind;

	    msg_ptr = (struct ke_msg *)rx_msg;
	    ind = (struct sm_external_auth_required_ind *)msg_ptr->param;

		wpa_ctrl_event_copy(WPA_CTRL_EVENT_EXTERNAL_AUTH_IND, ind, sizeof(*ind));
	}	break;
#endif

	default:
		break;
	}
}

void rwnx_recv_msg(void)
{
    struct ke_msg *rx_msg;
    MSG_SND_NODE_PTR tx_msg;
    struct co_list_hdr *rx_node, *tx_node;

    GLOBAL_INT_DECLARATION();

    while(1)
    {
        uint8_t find = 0;

        rx_node = co_list_pop_front(&rw_msg_rx_head);
        if(!rx_node)
            break;

        rx_msg = (struct ke_msg *)rx_node;

        GLOBAL_INT_DISABLE();
        tx_node = co_list_pick(&rw_msg_tx_head);
        GLOBAL_INT_RESTORE();

        while(tx_node)
        {
            tx_msg = (MSG_SND_NODE_PTR)tx_node;
            if(rx_msg->id == tx_msg->reqid)
            {
                find = 1;
                break;
            }

            GLOBAL_INT_DISABLE();
            tx_node = co_list_next(tx_node);
            GLOBAL_INT_RESTORE();
        }

        if(find)
        {
            int ret;
            GLOBAL_INT_DISABLE();
            co_list_extract(&rw_msg_rx_head, rx_node);
            co_list_extract(&rw_msg_tx_head, tx_node);
            GLOBAL_INT_RESTORE();

            if(tx_msg->cfm && rx_msg->param_len)
                os_memcpy(tx_msg->cfm, &rx_msg->param[0], rx_msg->param_len);

            ret = rtos_set_semaphore(&tx_msg->semaphore);
            ASSERT(0 == ret);
        }
        else
        {
            rwnx_handle_recv_msg(rx_msg);
        }

        ke_msg_free(rx_msg);
    }
}

// eof

