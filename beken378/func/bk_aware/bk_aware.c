#include "sys_config.h"
#include "param_config.h"
#include "compiler.h"
#include "bk_aware.h"
#include "bk_err.h"
#include "doubly_list.h"
#include "mem_pub.h"
#include "common.h" // for is_multicast_ether_addr
#include "ieee802_11_defs.h"
#include "mac_frame.h"
#include "rw_msdu.h"
#include "wlan_ui_pub.h"
#include "uart_pub.h"
#include "mm.h"
#include "reg_mac_core.h"
#include "rtos_pub.h"
#include "vif_mgmt.h"

#define BK_AWARE_DEBUG       0
#if BK_AWARE_DEBUG
#define bk_aware_dbg         os_printf
#else
#define bk_aware_dbg(...)    do {} while(0)
#endif
#define bk_aware_printf      os_printf

#if CFG_BK_AWARE

/**
 * @list       all bk_aware peers
 * @sema       semaphore
 * @local_addr local mac address
 * @oui        OUI
 * @fetch_pos  used by function bk_aware_fetch_peer
 * @inited     indicate bk aware is inited
 * @recv_cb    the callback of bk aware packet is received
 * @send_cb    the callback of bk aware packet is sent
 */
struct bk_aware_env_tag {
	struct list_head list;	/* TODO: hlist */
	beken_semaphore_t sema;
	uint8_t local_addr[ETH_ALEN];
	uint8_t oui[3];  // 0x18fe34
	int fetch_pos;
	bool inited;

	bk_aware_recv_cb_t recv_cb;
	bk_aware_send_cb_t send_cb;
};

struct bk_aware_peer_priv {
	struct list_head node;
	bk_aware_peer_info_t *peer;
};

static struct bk_aware_env_tag bk_aware_env;
void bk_aware_monitor_cb(uint8_t *data, int len, wifi_link_info_t *info);
extern int bk_rand();
uint32_t bk_wlan_reg_rx_mgmt_cb(mgmt_rx_cb_t cb, uint32_t rx_mgmt_flag);

static inline bool bk_aware_inited()
{
	return bk_aware_env.inited;
}

/**
  * @brief     Initialize BKAWARE function
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_INTERNAL : Internal error
  */
bk_err_t bk_aware_init(void)
{
	if (bk_aware_env.inited)
		return BK_OK;

	os_memset(&bk_aware_env, 0, sizeof(bk_aware_env));
	INIT_LIST_HEAD(&bk_aware_env.list);
	rtos_init_semaphore_adv(&bk_aware_env.sema, 1, 1);
	wifi_get_mac_address((char *)(&bk_aware_env.local_addr), CONFIG_ROLE_STA);
	os_memcpy(bk_aware_env.oui, CFG_BK_AWARE_OUI, 3);

	/* TODO: only receive mgmt frame, ignore probe, etc. */
	if (!vif_mgmt_used_cnt()) {
		bk_aware_dbg("enable bk aware under monitor mode\n");

		/* monitor mode */
		bk_wlan_register_monitor_cb(bk_aware_monitor_cb);
		bk_wlan_start_monitor();

		// Enable ACK
	    nxmac_mac_cntrl_1_set(nxmac_mac_cntrl_1_get() & ~NXMAC_DISABLE_ACK_RESP_BIT);

#if 0
	    // Enable reception of Other Mgmt frames.
	    mm_rx_filter_umac_set(0xFFFFFFFF & ~(NXMAC_EXC_UNENCRYPTED_BIT
	    									| NXMAC_ACCEPT_BAR_BIT
	    									| NXMAC_ACCEPT_ERROR_FRAMES_BIT

	///////////////
	    									| NXMAC_ACCEPT_PROBE_REQ_BIT
	    									| NXMAC_ACCEPT_PROBE_RESP_BIT
	    									| NXMAC_ACCEPT_BEACON_BIT
	    									| NXMAC_ACCEPT_NOT_EXPECTED_BA_BIT
	    									| NXMAC_ACCEPT_ALL_BEACON_BIT
	    									| NXMAC_ACCEPT_BFMEE_FRAMES_BIT
	    									| NXMAC_ACCEPT_OTHER_CNTRL_FRAMES_BIT
	    									| NXMAC_ACCEPT_Q_DATA_BIT
	    									| NXMAC_ACCEPT_QCFWO_DATA_BIT
	///////////////

	                                        | NXMAC_ACCEPT_BA_BIT
	                                        | NXMAC_ACCEPT_CTS_BIT
	                                        | NXMAC_ACCEPT_RTS_BIT
	                                        | NXMAC_ACCEPT_ACK_BIT
	                                        | NXMAC_ACCEPT_PS_POLL_BIT
	                                        | NXMAC_ACCEPT_QO_S_NULL_BIT
	                                        | NXMAC_ACCEPT_CF_END_BIT
	                                        | NXMAC_ACCEPT_UNKNOWN_BIT
	                                        | NXMAC_ACCEPT_CFWO_DATA_BIT
	                                        | NXMAC_ACCEPT_DATA_BIT));
#endif
	} else {
		os_printf("enable bk aware under STA/AP mode\n");

		bk_wlan_reg_rx_mgmt_cb((mgmt_rx_cb_t)bk_aware_monitor_cb, PRESERVE_ACTION);

		bk_aware_dbg("accept other bssid frame\n");
		// More than one entity will now be available, so disable HW filtering on BSSID
		mm_rx_filter_lmac_enable_set(NXMAC_ACCEPT_OTHER_BSSID_BIT);
	}

	bk_aware_env.inited = true;

	/* READ FROM FLASH ?? */
	return BK_OK;
}

static inline void bk_aware_lock()
{
	rtos_get_semaphore(&bk_aware_env.sema, BEKEN_WAIT_FOREVER);
}

static inline void bk_aware_unlock()
{
	rtos_set_semaphore(&bk_aware_env.sema);
}

/**
 * Parse IEEE80211 Action Frame to find out BK AWARE packet.
 * The Action frame format:
 * ------------------------------------------------------------------------------------------------------------
 * | MAC Header | Category Code | Organization Identifier | Random Values | Vendor Specific Content |   FCS	|
 * ------------------------------------------------------------------------------------------------------------
 *   24 bytes 		1 byte				3 bytes 			  4 bytes			  7~255 bytes		 4 bytes
 *
 * +-----------------------------------------------------------------------------+
 * | Element ID | Length | Organization Identifier | Type | Version |    Body    |
 * +-----------------------------------------------------------------------------+
 *    1 byte 	1 byte			  3 bytes		  1 byte   1 byte	0~250 bytes
 *
 */
void bk_aware_monitor_cb(uint8_t *data, int len, wifi_link_info_t *info)
{
	//g_monitor_cb
	struct mac_hdr *hdr = (struct mac_hdr *)data;
	uint16_t framectrl = hdr->fctl;
	uint8_t *p = data;

	if ((uint32_t)info == PRESERVE_ACTION) {
		// RXed from `rx_mgmt_filter`
	}

	if (!bk_aware_inited())
		return;

	if ((framectrl & MAC_FCTRL_TYPESUBTYPE_MASK) == MAC_FCTRL_ACTION) {
		u8 category;
		u8 user_len;
		u8 version __maybe_unused;

#if BK_AWARE_DEBUG
		print_hex_dump("MON: ", data, len);
#endif

		p += sizeof(*hdr);

		// Forward to Action Frame Body
		category = *p++;

		// Not Vendor Specific Action
		if (category != WLAN_ACTION_VENDOR_SPECIFIC) {
			bk_aware_dbg("not vendor specific\n");
			return;
		}

		// Not our OUI
		if (os_memcmp(p, bk_aware_env.oui, 3)) {
			bk_aware_dbg("oui mismatch\n");
			return;
		}
		p += 3;

		p += 4; // Ignore random value

		// Vendor Specific Content
		if (*p++ != WLAN_EID_VENDOR_SPECIFIC) {
			bk_aware_dbg("not eid\n");
			return;
		}

		user_len = *p++; // Extract Application Len

		if (os_memcmp(p, bk_aware_env.oui, 3)) {
			bk_aware_dbg("not oui 2\n");
			return;
		}
		p += 3;

		// Type
		if (*p++ != 4) {
			bk_aware_dbg("type mismatch\n");
			return;
		}

		// Version
		version = *p++;
		bk_aware_dbg("bk_aware version: %d\n", version);

		if (bk_aware_env.recv_cb) {
#if BK_AWARE_DEBUG
			print_hex_dump("MAC_HDR: ", hdr, sizeof(*hdr));
			print_hex_dump("AWARE: ", p, user_len - 5);
#endif
			// If Peer exists and destination addr is to us, or
			// this frame is configuration frame with addr3 multicast bit set
			if ((bk_aware_is_peer_exist((uint8_t *)&hdr->addr2) &&
				(os_memcmp(&hdr->addr1, bk_aware_env.local_addr, ETH_ALEN)) == 0)||
				is_multicast_ether_addr((uint8_t *)&hdr->addr3))
				bk_aware_env.recv_cb((uint8_t *)&hdr->addr2, p, user_len - 5);
		}
	}
}

/**
  * @brief     De-initialize BKAWARE function
  *
  * @return
  *          - BK_OK : succeed
  */
bk_err_t bk_aware_deinit(void)
{
	/* free all bk aware peers */
	struct bk_aware_peer_priv *priv, *tmp;

	if (!bk_aware_inited())
		return BK_OK;

	bk_aware_lock();
	list_for_each_entry_safe(priv, tmp, &bk_aware_env.list, node) {
		os_free(priv->peer);
		priv->peer = 0;
		list_del(&priv->node);
		os_free(priv);
	}
	bk_aware_env.inited = false;
	bk_aware_unlock();
	rtos_deinit_semaphore(&bk_aware_env.sema);
	os_memset(&bk_aware_env, 0, sizeof(bk_aware_env));

	if (!vif_mgmt_used_cnt()) {
		os_printf("disable bk aware under monitor mode\n");
		bk_wlan_stop_monitor();
	} else if (vif_mgmt_used_cnt() == 1) {
		os_printf("disable bk aware under STA/AP mode\n");

		bk_wlan_reg_rx_mgmt_cb(NULL, PRESERVE_ACTION);

		bk_aware_dbg("drop other bssid frame\n");
		// More than one entity will now be available, so reenable HW filtering on BSSID
		mm_rx_filter_lmac_enable_clear(NXMAC_ACCEPT_OTHER_BSSID_BIT);
	}

	return BK_OK;
}

/**
  * @brief     Get the version of BKAWARE
  *
  * @param     version  BKAWARE version
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  */
bk_err_t bk_aware_get_version(uint32_t *version)
{
	*version = 1;
	return BK_OK;
}

/**
  * @brief     Register callback function of receiving BKAWARE data
  *
  * @param     cb  callback function of receiving BKAWARE data
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BKAWARE is not initialized
  *          - BK_ERR_BK_AWARE_INTERNAL : internal error
  */
bk_err_t bk_aware_register_recv_cb(bk_aware_recv_cb_t cb)
{
	if (!bk_aware_inited())
		return BK_ERR_BK_AWARE_NOT_INIT;

	bk_aware_env.recv_cb = cb;
	return BK_OK;
}

/**
  * @brief     Unregister callback function of receiving BKAWARE data
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BKAWARE is not initialized
  */
bk_err_t bk_aware_unregister_recv_cb(void)
{
	if (!bk_aware_inited())
		return BK_ERR_BK_AWARE_NOT_INIT;

	bk_aware_env.recv_cb = 0;
	return BK_OK;
}

/**
  * @brief     Register callback function of sending BKAWARE data
  *
  * @param     cb  callback function of sending BKAWARE data
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BKAWARE is not initialized
  *          - BK_ERR_BK_AWARE_INTERNAL : internal error
  */
bk_err_t bk_aware_register_send_cb(bk_aware_send_cb_t cb)
{
	if (!bk_aware_inited())
		return BK_ERR_BK_AWARE_NOT_INIT;

	bk_aware_env.send_cb = cb;
	return BK_OK;
}

/**
  * @brief     Unregister callback function of sending BKAWARE data
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BKAWARE is not initialized
  */
bk_err_t bk_aware_unregister_send_cb(void)
{
	if (!bk_aware_inited())
		return BK_ERR_BK_AWARE_NOT_INIT;

	bk_aware_env.send_cb = 0;
	return BK_OK;
}

/**
 * ------------------------------------------------------------------------------------------------------------
 * | MAC Header | Category Code | Organization Identifier | Random Values | Vendor Specific Content |   FCS	|
 * ------------------------------------------------------------------------------------------------------------
 *   24 bytes 		1 byte				3 bytes 			  4 bytes			  7~255 bytes		 4 bytes


 * +-----------------------------------------------------------------------------+
 * | Element ID | Length | Organization Identifier | Type | Version |    Body    |
 * +-----------------------------------------------------------------------------+
 *    1 byte 	1 byte			  3 bytes		  1 byte   1 byte	0~250 bytes
 *
 */
static uint8_t *bk_aware_construct_frame(const uint8_t *peer_addr, const uint8_t *data, size_t *len)
{
	struct mac_hdr *hdr;
	uint8_t *frame;
	uint8_t *p;
	uint32_t random_value;

	frame = os_malloc(sizeof(*hdr) + 16 /* Addtitonal Fields */ + *len);
	if (!frame)
		return NULL;

	hdr = (struct mac_hdr *)frame;
	hdr->fctl = MAC_FCTRL_ACTION;	// ACTION Frame
	hdr->durid = 0;
	hdr->seq = 0;

	/* FIXME: MAC HDR */
	os_memcpy(&hdr->addr1, peer_addr, ETH_ALEN);
	os_memcpy(&hdr->addr2, bk_aware_env.local_addr, ETH_ALEN);
	if (is_multicast_ether_addr(peer_addr))
		os_memcpy(&hdr->addr3, peer_addr, ETH_ALEN);
	else
		os_memcpy(&hdr->addr3, bk_aware_env.local_addr, ETH_ALEN);

	p = frame + sizeof(*hdr);

	// Category Code
	*p++ = WLAN_ACTION_VENDOR_SPECIFIC;
	// OUI
	os_memcpy(p, bk_aware_env.oui, 3);
	p += 3;

	// Random Values
	random_value = bk_rand();
	os_memcpy(p, &random_value, 4);
	p += 4;

	// Vendor Specific Content
	*p++ = WLAN_EID_VENDOR_SPECIFIC;
	*p++ = 3 + 1 + 1 + *len; // The length is the total length of Organization Identifier, Type, Version and Body
	os_memcpy(p, bk_aware_env.oui, 3);
	p += 3;

	// Type: BK AWARE
	*p++ = 4;

	// Version: BK aware version
	*p++ = 1;

	// Copy User Data
	os_memcpy(p, data, *len);
	p += *len;

	*len = p - frame;

	return frame;
}

// refer ieee80211_data_tx_cb
static void bk_aware_send_callback(void *param)
{
	struct txdesc *txdesc_new = (struct txdesc *)param;
	MSDU_NODE_T *node = (MSDU_NODE_T *)txdesc_new->host.msdu_node;
	struct tx_hd *txhd = &txdesc_new->lmac.hw_desc->thd;
	uint32_t status = txhd->statinfo;
	uint8_t *frame = rwm_get_msdu_content_ptr(node);
	struct mac_hdr *hdr = (struct mac_hdr *)frame;

	if (!node) {
		bk_aware_dbg("zero_node\r\n");
		return;
	}

	bk_aware_dbg("pkt to %pM done\n", &hdr->addr1);
	if (bk_aware_env.send_cb)
		bk_aware_env.send_cb((uint8_t *)&hdr->addr1, !(status & FRAME_SUCCESSFUL_TX_BIT));
}

/**
  * @brief     Send BKAWARE data
  *
  * @attention 1. If peer_addr is not NULL, send data to the peer whose MAC address matches peer_addr
  * @attention 2. If peer_addr is NULL, send data to all of the peers that are added to the peer list
  * @attention 3. The maximum length of data must be less than BK_AWARE_MAX_DATA_LEN
  * @attention 4. The buffer pointed to by data argument does not need to be valid after bk_aware_send returns
  *
  * @param     peer_addr  peer MAC address
  * @param     data  data to send
  * @param     len  length of data
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BKAWARE is not initialized
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  *          - BK_ERR_BK_AWARE_INTERNAL : internal error
  *          - BK_ERR_BK_AWARE_NO_MEM : out of memory
  *          - BK_ERR_BK_AWARE_NOT_FOUND : peer is not found
  *          - BK_ERR_BK_AWARE_IF : current WiFi interface doesn't match that of peer
  */
bk_err_t bk_aware_send(const uint8_t *peer_addr, const uint8_t *data, size_t len)
{
	if (!bk_aware_inited())
		return BK_ERR_BK_AWARE_NOT_INIT;

	if (!data || len > BK_AWARE_MAX_DATA_LEN)
		return BK_ERR_BK_AWARE_ARG;

	if (!peer_addr) {
		/* send data to all of the peers that are added to the peer list */
		bk_aware_printf("%s %d: Not Implemented\n", __func__, __LINE__);

		return BK_ERR_BK_AWARE_INTERNAL;
	}

	/* unicast peer */
	if (bk_aware_is_peer_exist(peer_addr)) {
		uint8_t *frame = bk_aware_construct_frame(peer_addr, data, &len);
		if (!frame)
			return BK_ERR_BK_AWARE_NO_MEM;

		//print_hex_dump("AWARE SS: ", frame, len);
		//bk_aware_dbg("frame: %p / %p\n", frame, frame+68);
		bk_wlan_send_raw_frame_with_cb(frame, len, bk_aware_send_callback, NULL);
		os_free(frame);
		return BK_OK;
	} else {
		bk_aware_dbg("%s %d\n", __func__, __LINE__);
	}

	return BK_ERR_BK_AWARE_NOT_FOUND;
}

/**
  * @brief     Add a peer to peer list
  *
  * @param     peer  peer information
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BKAWARE is not initialized
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  *          - BK_ERR_BK_AWARE_FULL : peer list is full
  *          - BK_ERR_BK_AWARE_NO_MEM : out of memory
  *          - BK_ERR_BK_AWARE_EXIST : peer has existed
  */
bk_err_t bk_aware_add_peer(const bk_aware_peer_info_t *peer)
{
	struct bk_aware_peer_priv *priv;

	bk_aware_dbg("XXX: %s %d: mac %pM\n", __func__, __LINE__, peer->peer_addr);

	if (!peer)
		return BK_ERR_BK_AWARE_ARG;

	if (bk_aware_is_peer_exist(peer->peer_addr))
		return BK_ERR_BK_AWARE_EXIST;

	if ((priv = os_malloc(sizeof(*priv))) == 0)
		return BK_ERR_BK_AWARE_NO_MEM;

	if ((priv->peer = os_malloc(sizeof(*peer))) == 0) {
		os_free(priv);
		return BK_ERR_BK_AWARE_NO_MEM;
	}

	os_memcpy(priv->peer, peer, sizeof(*peer));
	priv->peer->priv = priv;

	bk_aware_lock();
	list_add_tail(&priv->node, &bk_aware_env.list);
	bk_aware_unlock();

	return BK_OK;
}

/**
  * @brief     Delete a peer from peer list
  *
  * @param     peer_addr  peer MAC address
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BKAWARE is not initialized
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  *          - BK_ERR_BK_AWARE_NOT_FOUND : peer is not found
  */
bk_err_t bk_aware_del_peer(const uint8_t *peer_addr)
{
	struct bk_aware_peer_priv *priv, *tmp;
	bk_aware_peer_info_t *peer;
	bool deleted = false;

	if (!peer_addr)
		return BK_ERR_BK_AWARE_ARG;

	if (!bk_aware_inited())
		return BK_ERR_BK_AWARE_NOT_INIT;

	bk_aware_lock();
	list_for_each_entry_safe(priv, tmp, &bk_aware_env.list, node) {
		peer = priv->peer;

		if (os_memcmp(peer->peer_addr, peer_addr, ETH_ALEN) == 0) {
			os_free(priv->peer);
			priv->peer = 0;
			list_del(&priv->node);
			os_free(priv);
			deleted = true;

			break;
		}
	}
	bk_aware_unlock();

	if (!deleted)
		return BK_ERR_BK_AWARE_NOT_FOUND;

	return BK_OK;
}

/**
  * @brief     Modify a peer
  *
  * @param     peer  peer information
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BKAWARE is not initialized
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  *          - BK_ERR_BK_AWARE_FULL : peer list is full
  */
bk_err_t bk_aware_mod_peer(const bk_aware_peer_info_t *peer_mod)
{
	struct bk_aware_peer_priv *priv;
	bk_aware_peer_info_t *peer;

	if (!peer_mod)
		return BK_ERR_BK_AWARE_ARG;

	if (!bk_aware_inited())
		return BK_ERR_BK_AWARE_NOT_INIT;

	bk_aware_lock();
	list_for_each_entry(priv, &bk_aware_env.list, node) {
		peer = priv->peer;
		if (os_memcmp(peer->peer_addr, peer_mod->peer_addr, ETH_ALEN) == 0) {
			os_memcpy(peer, peer_mod, sizeof(*peer));
			break;
		}
	}
	bk_aware_unlock();

	return BK_OK;
}

/**
  * @brief     Get a peer whose MAC address matches peer_addr from peer list
  *
  * @param     peer_addr  peer MAC address
  * @param     peer  peer information
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BKAWARE is not initialized
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  *          - BK_ERR_BK_AWARE_NOT_FOUND : peer is not found
  */
bk_err_t bk_aware_get_peer(const uint8_t *peer_addr, bk_aware_peer_info_t *peer_out)
{
	/* free all bk aware peers */
	struct bk_aware_peer_priv *priv;
	bk_aware_peer_info_t *peer;

	if (!bk_aware_inited())
		return BK_ERR_BK_AWARE_NOT_INIT;

	if (!peer_addr || !peer_out)
		return BK_ERR_BK_AWARE_ARG;

	bk_aware_lock();
	list_for_each_entry(priv, &bk_aware_env.list, node) {
		peer = priv->peer;
		if (os_memcmp(peer->peer_addr, peer_addr, ETH_ALEN) == 0) {
			os_memcpy(peer_out, peer, sizeof(*peer_out));
			bk_aware_unlock();
			return BK_OK;
		}
	}
	bk_aware_unlock();

	return BK_ERR_BK_AWARE_NOT_FOUND;
}

/**
  * @brief     Fetch a peer from peer list. Only return the peer which address is unicast,
  *            for the multicast/broadcast address, the function will ignore and try to find
  *            the next in the peer list.
  *
  * @param     from_head  fetch from head of list or not
  * @param     peer  peer information
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BKAWARE is not initialized
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  *          - BK_ERR_BK_AWARE_NOT_FOUND : peer is not found
  */
bk_err_t bk_aware_fetch_peer(bool from_head, bk_aware_peer_info_t *peer)
{
	if (!bk_aware_inited())
		return BK_ERR_BK_AWARE_NOT_INIT;

	if (!peer)
		return BK_ERR_BK_AWARE_ARG;

	// Not Implemented
	return BK_ERR_BK_AWARE_INTERNAL;
}

/**
  * @brief     Peer exists or not
  *
  * @param     peer_addr  peer MAC address
  *
  * @return
  *          - true : peer exists
  *          - false : peer not exists
  */
bool bk_aware_is_peer_exist(const uint8_t *peer_addr)
{
	struct bk_aware_peer_priv *priv;
	bk_aware_peer_info_t *peer;

	bk_aware_lock();
	list_for_each_entry(priv, &bk_aware_env.list, node) {
		peer = priv->peer;
		if (os_memcmp(peer->peer_addr, peer_addr, ETH_ALEN) == 0) {
			bk_aware_unlock();
			return true;
		}
	}
	bk_aware_unlock();

	return false;
}

/**
  * @brief     Get the number of peers
  *
  * @param     num  number of peers
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BKAWARE is not initialized
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  */
bk_err_t bk_aware_get_peer_num(bk_aware_peer_num_t *num)
{
	int count = 0;

	if (!bk_aware_inited())
		return BK_ERR_BK_AWARE_NOT_INIT;

	if (!num)
		return BK_ERR_BK_AWARE_ARG;

	bk_aware_lock();
	count = list_size(&bk_aware_env.list);
	bk_aware_unlock();

	num->total_num = count;
	num->encrypt_num = 0;

	return count;
}

/**
  * @brief     Set the primary master key
  *
  * @param     pmk  primary master key
  *
  * @attention 1. primary master key is used to encrypt local master key
  *
  * @return
  *          - BK_OK : succeed
  *          - BK_ERR_BK_AWARE_NOT_INIT : BKAWARE is not initialized
  *          - BK_ERR_BK_AWARE_ARG : invalid argument
  */
bk_err_t bk_aware_set_pmk(const uint8_t *pmk)
{
	if (!bk_aware_inited())
		return BK_ERR_BK_AWARE_NOT_INIT;

	return BK_OK;
}

#endif // CFG_BK_AWARE

