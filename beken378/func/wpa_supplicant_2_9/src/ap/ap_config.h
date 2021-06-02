/*
 * hostapd / Configuration definitions and helpers functions
 * Copyright (c) 2003-2015, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef HOSTAPD_CONFIG_H
#define HOSTAPD_CONFIG_H

#include "common/defs.h"
#include "utils/list.h"
#include "ip_addr.h"
#include "common/wpa_common.h"
#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"
#include "crypto/sha256.h"
#include "wps/wps.h"
#include "vlan.h"
#include "hostapd_cfg.h"

/**
 * mesh_conf - local MBSS state and settings
 */
struct mesh_conf {
	u8 meshid[32];
	u8 meshid_len;
	/* Active Path Selection Protocol Identifier */
	u8 mesh_pp_id;
	/* Active Path Selection Metric Identifier */
	u8 mesh_pm_id;
	/* Congestion Control Mode Identifier */
	u8 mesh_cc_id;
	/* Synchronization Protocol Identifier */
	u8 mesh_sp_id;
	/* Authentication Protocol Identifier */
	u8 mesh_auth_id;
	u8 *rsn_ie;
	int rsn_ie_len;
#define MESH_CONF_SEC_NONE BIT(0)
#define MESH_CONF_SEC_AUTH BIT(1)
#define MESH_CONF_SEC_AMPE BIT(2)
	unsigned int security;
	enum mfp_options ieee80211w;
	int ocv;
	unsigned int pairwise_cipher;
	unsigned int group_cipher;
	unsigned int mgmt_group_cipher;
	int dot11MeshMaxRetries;
	int dot11MeshRetryTimeout; /* msec */
	int dot11MeshConfirmTimeout; /* msec */
	int dot11MeshHoldingTimeout; /* msec */
};

#define MAX_STA_COUNT CFG_SUPPORTED_MAX_STA_NUM /* 2007 */
#define MAX_VLAN_ID 4094

typedef u8 macaddr[ETH_ALEN];

struct mac_acl_entry {
	macaddr addr;
	struct vlan_description vlan_id;
};

struct hostapd_radius_servers;
struct ft_remote_r0kh;
struct ft_remote_r1kh;

#define NUM_WEP_KEYS 4
struct hostapd_wep_keys {
	u8 idx;
	u8 *key[NUM_WEP_KEYS];
	size_t len[NUM_WEP_KEYS];
	int keys_set;
	size_t default_len; /* key length used for dynamic key generation */
};

typedef enum hostap_security_policy {
	SECURITY_PLAINTEXT = 0,
	SECURITY_STATIC_WEP = 1,
	SECURITY_IEEE_802_1X = 2,
	SECURITY_WPA_PSK = 3,
	SECURITY_WPA = 4,
	SECURITY_OSEN = 5
} secpolicy;

struct hostapd_ssid {
	u8 ssid[SSID_MAX_LEN];
	size_t ssid_len;
	unsigned int ssid_set:1;
	unsigned int utf8_ssid:1;
	unsigned int wpa_passphrase_set:1;
	unsigned int wpa_psk_set:1;

#ifndef CONFIG_NO_VLAN
	char vlan[IFNAMSIZ + 1];
#endif
	secpolicy security_policy;

	struct hostapd_wpa_psk *wpa_psk;
	char *wpa_passphrase;

	struct hostapd_wep_keys wep;

#define DYNAMIC_VLAN_DISABLED 0
#define DYNAMIC_VLAN_OPTIONAL 1
#define DYNAMIC_VLAN_REQUIRED 2
	int dynamic_vlan;
#define DYNAMIC_VLAN_NAMING_WITHOUT_DEVICE 0
#define DYNAMIC_VLAN_NAMING_WITH_DEVICE 1
#define DYNAMIC_VLAN_NAMING_END 2
	int vlan_naming;
	int per_sta_vif;
#ifdef CONFIG_FULL_DYNAMIC_VLAN
	char *vlan_tagged_interface;
#endif /* CONFIG_FULL_DYNAMIC_VLAN */
};


#define VLAN_ID_WILDCARD -1

struct hostapd_vlan {
	struct hostapd_vlan *next;
	int vlan_id; /* VLAN ID or -1 (VLAN_ID_WILDCARD) for wildcard entry */
	struct vlan_description vlan_desc;
	char ifname[IFNAMSIZ + 1];
	char bridge[IFNAMSIZ + 1];
	int configured;
	int dynamic_vlan;
#ifdef CONFIG_FULL_DYNAMIC_VLAN

#define DVLAN_CLEAN_WLAN_PORT	0x8
	int clean;
#endif /* CONFIG_FULL_DYNAMIC_VLAN */
};

#define PMK_LEN 32
#define KEYID_LEN 32
#define MIN_PASSPHRASE_LEN 8
#define MAX_PASSPHRASE_LEN 63
struct hostapd_sta_wpa_psk_short {
	struct hostapd_sta_wpa_psk_short *next;
	unsigned int is_passphrase:1;
	u8 psk[PMK_LEN];
	char passphrase[MAX_PASSPHRASE_LEN + 1];
	int ref; /* (number of references held) - 1 */
};

struct hostapd_wpa_psk {
	struct hostapd_wpa_psk *next;
	int group;
	char keyid[KEYID_LEN];
	u8 psk[PMK_LEN];
	u8 addr[ETH_ALEN];
	u8 p2p_dev_addr[ETH_ALEN];
	int vlan_id;
};

struct hostapd_eap_user {
	struct hostapd_eap_user *next;
	u8 *identity;
	size_t identity_len;
	struct {
		int vendor;
		u32 method;
	} methods[EAP_MAX_METHODS];
	u8 *password;
	size_t password_len;
	u8 *salt;
	size_t salt_len; /* non-zero when password is salted */
	int phase2;
	int force_version;
	unsigned int wildcard_prefix:1;
	unsigned int password_hash:1; /* whether password is hashed with
				       * nt_password_hash() */
	unsigned int remediation:1;
	unsigned int macacl:1;
	int ttls_auth; /* EAP_TTLS_AUTH_* bitfield */
	struct hostapd_radius_attr *accept_attr;
	u32 t_c_timestamp;
};

struct hostapd_radius_attr {
	u8 type;
	struct wpabuf *val;
	struct hostapd_radius_attr *next;
};


#define NUM_TX_QUEUES 4

struct hostapd_tx_queue_params {
	int aifs;
	int cwmin;
	int cwmax;
	int burst; /* maximum burst time in 0.1 ms, i.e., 10 = 1 ms */
};


#define MAX_ROAMING_CONSORTIUM_LEN 15

struct hostapd_roaming_consortium {
	u8 len;
	u8 oi[MAX_ROAMING_CONSORTIUM_LEN];
};

struct hostapd_lang_string {
	u8 lang[3];
	u8 name_len;
	u8 name[252];
};

struct hostapd_venue_url {
	u8 venue_number;
	u8 url_len;
	u8 url[254];
};

#define MAX_NAI_REALMS 10
#define MAX_NAI_REALMLEN 255
#define MAX_NAI_EAP_METHODS 5
#define MAX_NAI_AUTH_TYPES 4
struct hostapd_nai_realm_data {
	u8 encoding;
	char realm_buf[MAX_NAI_REALMLEN + 1];
	char *realm[MAX_NAI_REALMS];
	u8 eap_method_count;
	struct hostapd_nai_realm_eap {
		u8 eap_method;
		u8 num_auths;
		u8 auth_id[MAX_NAI_AUTH_TYPES];
		u8 auth_val[MAX_NAI_AUTH_TYPES];
	} eap_method[MAX_NAI_EAP_METHODS];
};

struct anqp_element {
	struct dl_list list;
	u16 infoid;
	struct wpabuf *payload;
};

struct sae_password_entry {
	struct sae_password_entry *next;
	char *password;
	char *identifier;
	u8 peer_addr[ETH_ALEN];
	int vlan_id;
};

struct dpp_controller_conf {
	struct dpp_controller_conf *next;
	u8 pkhash[SHA256_MAC_LEN];
	struct hostapd_ip_addr ipaddr;
};

/**
 * struct hostapd_bss_config - Per-BSS configuration
 */
struct hostapd_bss_config {
	char iface[IFNAMSIZ + 1];
#ifdef CONFIG_FULL_HOSTAPD
	char bridge[IFNAMSIZ + 1];
	char vlan_bridge[IFNAMSIZ + 1];
	char wds_bridge[IFNAMSIZ + 1];
#endif
	enum hostapd_logger_level logger_syslog_level, logger_stdout_level;

	int max_num_sta; /* maximum number of STAs in station table */

	int dtim_period;
#ifdef CONFIG_FULL_HOSTAPD
	unsigned int bss_load_update_period;
	unsigned int chan_util_avg_period;
#endif

	int ieee802_1x; /* use IEEE 802.1X */
	int eapol_version;
#ifdef CONFIG_FULL_HOSTAPD
	int eap_server; /* Use internal EAP server instead of external
			 * RADIUS server */
	struct hostapd_eap_user *eap_user;
	char *eap_user_sqlite;
	char *eap_sim_db;
	unsigned int eap_sim_db_timeout;
	int eap_server_erp; /* Whether ERP is enabled on internal EAP server */
	struct hostapd_ip_addr own_ip_addr;
	char *nas_identifier;
	struct hostapd_radius_servers *radius;
	int acct_interim_interval;
	int radius_request_cui;
	struct hostapd_radius_attr *radius_auth_req_attr;
	struct hostapd_radius_attr *radius_acct_req_attr;
	char *radius_req_attr_sqlite;
	int radius_das_port;
	unsigned int radius_das_time_window;
	int radius_das_require_event_timestamp;
	int radius_das_require_message_authenticator;
	struct hostapd_ip_addr radius_das_client_addr;
	u8 *radius_das_shared_secret;
	size_t radius_das_shared_secret_len;
#endif

	struct hostapd_ssid ssid;

	char *eap_req_id_text; /* optional displayable message sent with
				* EAP Request-Identity */
	size_t eap_req_id_text_len;
	int eapol_key_index_workaround;

	size_t default_wep_key_len;
	int individual_wep_key_len;
	int wep_rekeying_period;
	int broadcast_key_idx_min, broadcast_key_idx_max;
	int eap_reauth_period;
	int erp_send_reauth_start;

#ifdef CONFIG_FULL_HOSTAPD
	char *erp_domain;

	int ieee802_11f; /* use IEEE 802.11f (IAPP) */
	char iapp_iface[IFNAMSIZ + 1]; /* interface used with IAPP broadcast
					* frames */

	enum macaddr_acl {
		ACCEPT_UNLESS_DENIED = 0,
		DENY_UNLESS_ACCEPTED = 1,
		USE_EXTERNAL_RADIUS_AUTH = 2
	} macaddr_acl;
	struct mac_acl_entry *accept_mac;
	int num_accept_mac;
	struct mac_acl_entry *deny_mac;
	int num_deny_mac;
	int wds_sta;
#endif
	int isolate;
	int start_disabled;

	int auth_algs; /* bitfield of allowed IEEE 802.11 authentication
			* algorithms, WPA_AUTH_ALG_{OPEN,SHARED,LEAP} */

	int wpa; /* bitfield of WPA_PROTO_WPA, WPA_PROTO_RSN */
	int wpa_key_mgmt;
#ifdef CONFIG_IEEE80211W_AP
	enum mfp_options ieee80211w;
	int group_mgmt_cipher;
	/* dot11AssociationSAQueryMaximumTimeout (in TUs) */
	unsigned int assoc_sa_query_max_timeout;
	/* dot11AssociationSAQueryRetryTimeout (in TUs) */
	int assoc_sa_query_retry_timeout;
#endif /* CONFIG_IEEE80211W_AP */
#ifdef CONFIG_OCV
	int ocv; /* Operating Channel Validation */
#endif /* CONFIG_OCV */
	enum {
		PSK_RADIUS_IGNORED = 0,
		PSK_RADIUS_ACCEPTED = 1,
		PSK_RADIUS_REQUIRED = 2
	} wpa_psk_radius;
	int wpa_pairwise;
	int group_cipher; /* wpa_group value override from configuation */
	int wpa_group;
	int wpa_group_rekey;
	int wpa_group_rekey_set;
	int wpa_strict_rekey;
	int wpa_gmk_rekey;
	int wpa_ptk_rekey;
	u32 wpa_group_update_count;
	u32 wpa_pairwise_update_count;
	int wpa_disable_eapol_key_retries;
	int rsn_pairwise;
	int rsn_preauth;
	char *rsn_preauth_interfaces;

#ifdef CONFIG_IEEE80211R_AP
	/* IEEE 802.11r - Fast BSS Transition */
	u8 mobility_domain[MOBILITY_DOMAIN_ID_LEN];
	u8 r1_key_holder[FT_R1KH_ID_LEN];
	u32 r0_key_lifetime; /* PMK-R0 lifetime seconds */
	int rkh_pos_timeout;
	int rkh_neg_timeout;
	int rkh_pull_timeout; /* ms */
	int rkh_pull_retries;
	u32 reassociation_deadline;
	struct ft_remote_r0kh *r0kh_list;
	struct ft_remote_r1kh *r1kh_list;
	int pmk_r1_push;
	int ft_over_ds;
	int ft_psk_generate_local;
	int r1_max_key_lifetime;
#endif /* CONFIG_IEEE80211R_AP */

#ifdef CONFIG_FULL_HOSTAPD
	char *ctrl_interface; /* directory for UNIX domain sockets */
#ifndef CONFIG_NATIVE_WINDOWS
	int ctrl_interface_gid;
#endif /* CONFIG_NATIVE_WINDOWS */
	int ctrl_interface_gid_set;
#endif

	int use_pae_group_addr; /* Whether to send EAPOL frames to PAE group
				 * address instead of individual address
				 * (for driver_wired.c).
				 */

	int ap_max_inactivity;
	int ignore_broadcast_ssid;
	int no_probe_resp_if_max_sta;

	int wmm_enabled;
	int wmm_uapsd;

	struct hostapd_vlan *vlan;

	macaddr bssid;

	/*
	 * Maximum listen interval that STAs can use when associating with this
	 * BSS. If a STA tries to use larger value, the association will be
	 * denied with status code 51.
	 */
	u16 max_listen_interval;

	int disable_pmksa_caching;
	int okc; /* Opportunistic Key Caching */

	int wps_state;
	int pbc_in_m1;
	char *server_id;

#define P2P_ENABLED BIT(0)
#define P2P_GROUP_OWNER BIT(1)
#define P2P_GROUP_FORMATION BIT(2)
#define P2P_MANAGE BIT(3)
#define P2P_ALLOW_CROSS_CONNECTION BIT(4)
	int p2p;
#ifdef CONFIG_P2P
	u8 ip_addr_go[4];
	u8 ip_addr_mask[4];
	u8 ip_addr_start[4];
	u8 ip_addr_end[4];
#endif /* CONFIG_P2P */

	int disassoc_low_ack;
	int skip_inactivity_poll;

#ifdef CONFIG_TDLS
#define TDLS_PROHIBIT BIT(0)
#define TDLS_PROHIBIT_CHAN_SWITCH BIT(1)
	int tdls;
#endif

	int disable_11n;
	int disable_11ac;

#ifdef CONFIG_FULL_HOSTAPD
	/* IEEE 802.11v */
	int time_advertisement;
	char *time_zone;
	int wnm_sleep_mode;
	int wnm_sleep_mode_no_keys;
	int bss_transition;
#endif

#ifdef CONFIG_GAS
	u16 gas_comeback_delay;
	size_t gas_frag_limit;
	int gas_address3;
#endif

	u8 qos_map_set[16 + 2 * 21];
	unsigned int qos_map_set_len;

	int osen;
	int proxy_arp;
	int na_mcast_to_ucast;

#ifdef CONFIG_WPS
	u8 wps_rf_bands; /* RF bands for WPS (WPS_RF_*) */
#endif

#ifdef CONFIG_RADIUS_TEST
	char *dump_msk_file;
#endif /* CONFIG_RADIUS_TEST */

	struct wpabuf *vendor_elements;
	struct wpabuf *assocresp_elements;

#ifdef CONFIG_SAE_AP
	unsigned int sae_anti_clogging_threshold;
	unsigned int sae_sync;
	int sae_require_mfp;
	int *sae_groups;
	struct sae_password_entry *sae_passwords;
#endif

#ifdef CONFIG_FULL_HOSTAPD
	char *wowlan_triggers; /* Wake-on-WLAN triggers */
#endif

#define MESH_ENABLED BIT(0)
	int mesh;

#ifdef CONFIG_FULL_HOSTAPD
	u8 radio_measurements[RRM_CAPABILITIES_IE_LEN];
#endif
#ifdef CONFIG_IEEE80211AC
	int vendor_vht;
	int use_sta_nsts;
#endif

	char *no_probe_resp_if_seen_on;
	char *no_auth_if_seen_on;

#ifdef CONFIG_FULL_HOSTAPD
	int pbss;
#endif

#ifdef CONFIG_MBO
	int mbo_enabled;
	/**
	 * oce - Enable OCE in AP and/or STA-CFON mode
	 *  - BIT(0) is Reserved
	 *  - Set BIT(1) to enable OCE in STA-CFON mode
	 *  - Set BIT(2) to enable OCE in AP mode
	 */
	unsigned int oce;
	int mbo_cell_data_conn_pref;
#endif /* CONFIG_MBO */

	int ftm_responder;
	int ftm_initiator;
	int multicast_to_unicast;

	int broadcast_deauth;

#ifdef CONFIG_DPP
	char *dpp_connector;
	struct wpabuf *dpp_netaccesskey;
	unsigned int dpp_netaccesskey_expiry;
	struct wpabuf *dpp_csign;
#ifdef CONFIG_DPP2
	struct dpp_controller_conf *dpp_controller;
#endif /* CONFIG_DPP2 */
#endif /* CONFIG_DPP */

#ifdef CONFIG_OWE
	macaddr owe_transition_bssid;
	u8 owe_transition_ssid[SSID_MAX_LEN];
	size_t owe_transition_ssid_len;
	char owe_transition_ifname[IFNAMSIZ + 1];
	int *owe_groups;
#endif /* CONFIG_OWE */

	u8 send_probe_response;

	int coloc_intf_reporting;

#ifdef CONFIG_FULL_HOSTAPD
#define BACKHAUL_BSS 1
#define FRONTHAUL_BSS 2
	int multi_ap; /* bitmap of BACKHAUL_BSS, FRONTHAUL_BSS */
#endif
};

/**
 * struct he_phy_capabilities_info - HE PHY capabilities
 */
struct he_phy_capabilities_info {
	Boolean he_su_beamformer;
	Boolean he_su_beamformee;
	Boolean he_mu_beamformer;
};

/**
 * struct he_operation - HE operation
 */
struct he_operation {
	u8 he_bss_color;
	u8 he_default_pe_duration;
	u8 he_twt_required;
	u16 he_rts_threshold;
	u16 he_basic_mcs_nss_set;
};

/**
 * struct spatial_reuse - Spatial reuse
 */
struct spatial_reuse {
	u8 sr_control;
	u8 non_srg_obss_pd_max_offset;
	u8 srg_obss_pd_min_offset;
	u8 srg_obss_pd_max_offset;
	u8 srg_obss_color_bitmap;
	u8 srg_obss_color_partial_bitmap;
};

/**
 * struct hostapd_config - Per-radio interface configuration
 */
struct hostapd_config {
	struct hostapd_bss_config **bss, *last_bss;
	size_t num_bss;

	u16 beacon_int;
	int rts_threshold;
	int fragm_threshold;
	u8 channel;
	u8 acs;
	struct wpa_freq_range_list acs_ch_list;
	int acs_exclude_dfs;
	enum hostapd_hw_mode hw_mode; /* HOSTAPD_MODE_IEEE80211A, .. */
	enum {
		LONG_PREAMBLE = 0,
		SHORT_PREAMBLE = 1
	} preamble;

	int *supported_rates;
	int *basic_rates;
	unsigned int beacon_rate;
	enum beacon_rate_type rate_type;

	const struct wpa_driver_ops *driver;
	char *driver_params;

	int ap_table_max_size;
	int ap_table_expiration_time;

	unsigned int track_sta_max_num;
	unsigned int track_sta_max_age;

	char country[3]; /* first two octets: country code as described in
			  * ISO/IEC 3166-1. Third octet:
			  * ' ' (ascii 32): all environments
			  * 'O': Outdoor environemnt only
			  * 'I': Indoor environment only
			  * 'X': Used with noncountry entity ("XXX")
			  * 0x00..0x31: identifying IEEE 802.11 standard
			  *	Annex E table (0x04 = global table)
			  */

	int ieee80211d;

	int ieee80211h; /* DFS */

	/*
	 * Local power constraint is an octet encoded as an unsigned integer in
	 * units of decibels. Invalid value -1 indicates that Power Constraint
	 * element will not be added.
	 */
	int local_pwr_constraint;

	/* Control Spectrum Management bit */
	int spectrum_mgmt_required;

	struct hostapd_tx_queue_params tx_queue[NUM_TX_QUEUES];

	/*
	 * WMM AC parameters, in same order as 802.1D, i.e.
	 * 0 = BE (best effort)
	 * 1 = BK (background)
	 * 2 = VI (video)
	 * 3 = VO (voice)
	 */
	struct hostapd_wmm_ac_params wmm_ac_params[4];

	int ht_op_mode_fixed;
	u16 ht_capab;
	int ieee80211n;
	int secondary_channel;
	int no_pri_sec_switch;
	int require_ht;
	int obss_interval;
	u32 vht_capab;
	int ieee80211ac;
	int require_vht;
	u8 vht_oper_chwidth;
	u8 vht_oper_centr_freq_seg0_idx;
	u8 vht_oper_centr_freq_seg1_idx;
	u8 ht40_plus_minus_allowed;

	/* Use driver-generated interface addresses when adding multiple BSSs */
	u8 use_driver_iface_addr;

#ifdef CONFIG_FST
	struct fst_iface_cfg fst_cfg;
#endif /* CONFIG_FST */

#ifdef CONFIG_P2P
	u8 p2p_go_ctwindow;
#endif /* CONFIG_P2P */

#ifdef CONFIG_TESTING_OPTIONS
	double ignore_probe_probability;
	double ignore_auth_probability;
	double ignore_assoc_probability;
	double ignore_reassoc_probability;
	double corrupt_gtk_rekey_mic_probability;
	int ecsa_ie_only;
#endif /* CONFIG_TESTING_OPTIONS */

#ifdef CONFIG_ACS
	unsigned int acs_num_scans;
	struct acs_bias {
		int channel;
		double bias;
	} *acs_chan_bias;
	unsigned int num_acs_chan_bias;
#endif /* CONFIG_ACS */

#ifdef CONFIG_FULL_HOSTAPD
	struct wpabuf *lci;
	struct wpabuf *civic;
	int stationary_ap;
#endif

	int ieee80211ax;
#ifdef CONFIG_IEEE80211AX
	struct he_phy_capabilities_info he_phy_capab;
	struct he_operation he_op;
	struct ieee80211_he_mu_edca_parameter_set he_mu_edca;
	struct spatial_reuse spr;
	u8 he_oper_chwidth;
	u8 he_oper_centr_freq_seg0_idx;
	u8 he_oper_centr_freq_seg1_idx;
#endif /* CONFIG_IEEE80211AX */

	/* VHT enable/disable config from CHAN_SWITCH */
#define CH_SWITCH_VHT_ENABLED BIT(0)
#define CH_SWITCH_VHT_DISABLED BIT(1)
	unsigned int ch_switch_vht_config;

	int rssi_reject_assoc_rssi;
	int rssi_reject_assoc_timeout;
};


static inline u8 hostapd_get_oper_chwidth(struct hostapd_config *conf)
{
#ifdef CONFIG_IEEE80211AX
	if (conf->ieee80211ax)
		return conf->he_oper_chwidth;
#endif /* CONFIG_IEEE80211AX */
	return conf->vht_oper_chwidth;
}

static inline void
hostapd_set_oper_chwidth(struct hostapd_config *conf, u8 oper_chwidth)
{
#ifdef CONFIG_IEEE80211AX
	if (conf->ieee80211ax)
		conf->he_oper_chwidth = oper_chwidth;
#endif /* CONFIG_IEEE80211AX */
	conf->vht_oper_chwidth = oper_chwidth;
}

static inline u8
hostapd_get_oper_centr_freq_seg0_idx(struct hostapd_config *conf)
{
#ifdef CONFIG_IEEE80211AX
	if (conf->ieee80211ax)
		return conf->he_oper_centr_freq_seg0_idx;
#endif /* CONFIG_IEEE80211AX */
	return conf->vht_oper_centr_freq_seg0_idx;
}

static inline void
hostapd_set_oper_centr_freq_seg0_idx(struct hostapd_config *conf,
				     u8 oper_centr_freq_seg0_idx)
{
#ifdef CONFIG_IEEE80211AX
	if (conf->ieee80211ax)
		conf->he_oper_centr_freq_seg0_idx = oper_centr_freq_seg0_idx;
#endif /* CONFIG_IEEE80211AX */
	conf->vht_oper_centr_freq_seg0_idx = oper_centr_freq_seg0_idx;
}

static inline u8
hostapd_get_oper_centr_freq_seg1_idx(struct hostapd_config *conf)
{
#ifdef CONFIG_IEEE80211AX
	if (conf->ieee80211ax)
		return conf->he_oper_centr_freq_seg1_idx;
#endif /* CONFIG_IEEE80211AX */
	return conf->vht_oper_centr_freq_seg1_idx;
}

static inline void
hostapd_set_oper_centr_freq_seg1_idx(struct hostapd_config *conf,
				     u8 oper_centr_freq_seg1_idx)
{
#ifdef CONFIG_IEEE80211AX
	if (conf->ieee80211ax)
		conf->he_oper_centr_freq_seg1_idx = oper_centr_freq_seg1_idx;
#endif /* CONFIG_IEEE80211AX */
	conf->vht_oper_centr_freq_seg1_idx = oper_centr_freq_seg1_idx;
}


int hostapd_mac_comp(const void *a, const void *b);
struct hostapd_config * hostapd_config_defaults(void);
void hostapd_config_defaults_bss(struct hostapd_bss_config *bss);
void hostapd_config_free_radius_attr(struct hostapd_radius_attr *attr);
void hostapd_config_free_eap_user(struct hostapd_eap_user *user);
void hostapd_config_free_eap_users(struct hostapd_eap_user *user);
void hostapd_config_clear_wpa_psk(struct hostapd_wpa_psk **p);
void hostapd_config_free_bss(struct hostapd_bss_config *conf);
void hostapd_config_free(struct hostapd_config *conf);
int hostapd_maclist_found(struct mac_acl_entry *list, int num_entries,
			  const u8 *addr, struct vlan_description *vlan_id);
int hostapd_rate_found(int *list, int rate);
const u8 * hostapd_get_psk(const struct hostapd_bss_config *conf,
			   const u8 *addr, const u8 *p2p_dev_addr,
			   const u8 *prev_psk, int *vlan_id);
int hostapd_setup_wpa_psk(struct hostapd_bss_config *conf);
int hostapd_vlan_valid(struct hostapd_vlan *vlan,
		       struct vlan_description *vlan_desc);
const char * hostapd_get_vlan_id_ifname(struct hostapd_vlan *vlan,
					int vlan_id);
struct hostapd_radius_attr *
hostapd_config_get_radius_attr(struct hostapd_radius_attr *attr, u8 type);
int hostapd_config_check(struct hostapd_config *conf, int full_config);
void hostapd_set_security_params(struct hostapd_bss_config *bss,
				 int full_config);

#endif /* HOSTAPD_CONFIG_H */
