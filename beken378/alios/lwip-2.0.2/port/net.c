#include "include.h"
#include <stdio.h>
#include <string.h>

#include <lwip/inet.h>

#include "netif/etharp.h"
#include "lwip/netif.h"
#include <lwip/netifapi.h>
#include <lwip/tcpip.h>
#include <lwip/dns.h>
#include <lwip/dhcp.h>
#include "lwip/prot/dhcp.h"

#include <lwip/sockets.h>
#include "ethernetif.h"


#include "sa_station.h"

#include "drv_model_pub.h"
#include "mem_pub.h"
#include "common.h"

#include "rw_pub.h"

#include "lwip_netif_address.h"
#include "rtos_pub.h"
#include "wlan_ui_pub.h"

#if CFG_ROLE_LAUNCH
#include "role_launch.h"
#endif
#include "param_config.h"

struct ipv4_config sta_ip_settings;
struct ipv4_config uap_ip_settings; 
static int up_iface;
uint32_t sta_ip_start_flag = 0;
uint32_t uap_ip_start_flag = 0;

#ifdef CONFIG_IPV6
#define IPV6_ADDR_STATE_TENTATIVE       "Tentative"
#define IPV6_ADDR_STATE_PREFERRED       "Preferred"
#define IPV6_ADDR_STATE_INVALID         "Invalid"
#define IPV6_ADDR_STATE_VALID           "Valid"
#define IPV6_ADDR_STATE_DEPRECATED      "Deprecated"
#define IPV6_ADDR_TYPE_LINKLOCAL	    "Link-Local"
#define IPV6_ADDR_TYPE_GLOBAL		    "Global"
#define IPV6_ADDR_TYPE_UNIQUELOCAL	    "Unique-Local"
#define IPV6_ADDR_TYPE_SITELOCAL	    "Site-Local"
#define IPV6_ADDR_UNKNOWN		        "Unknown"
#endif


#define net_e warning_prf
#define net_d warning_prf

typedef void (*net_sta_ipup_cb_fn)(void* data);

struct interface {
	struct netif netif;
	ip_addr_t ipaddr;
	ip_addr_t nmask;
	ip_addr_t gw;
};

static struct interface g_mlan = {{0}};
static struct interface g_uap = {{0}};

void *net_get_sta_handle(void);
void *net_get_uap_handle(void);
err_t lwip_netif_init(struct netif *netif);
err_t lwip_netif_uap_init(struct netif *netif);
void handle_data_packet(const u8_t interface, const u8_t *rcvdata,const u16_t datalen);			
int net_get_if_ip_addr(uint32_t *ip, void *intrfc_handle);
int net_get_if_ip_mask(uint32_t *nm, void *intrfc_handle);
int net_configure_address(struct ipv4_config *addr, void *intrfc_handle);
extern int dhcp_server_start(void *intrfc_handle);
extern void dhcp_server_stop(void);

#ifdef CONFIG_IPV6
char *ipv6_addr_state_to_desc(unsigned char addr_state)
{
	if (ip6_addr_istentative(addr_state))
		return IPV6_ADDR_STATE_TENTATIVE;
	else if (ip6_addr_ispreferred(addr_state))
		return IPV6_ADDR_STATE_PREFERRED;
	else if (ip6_addr_isinvalid(addr_state))
		return IPV6_ADDR_STATE_INVALID;
	else if (ip6_addr_isvalid(addr_state))
		return IPV6_ADDR_STATE_VALID;
	else if (ip6_addr_isdeprecated(addr_state))
		return IPV6_ADDR_STATE_DEPRECATED;
	else
		return IPV6_ADDR_UNKNOWN;
}

char *ipv6_addr_type_to_desc(struct ipv6_config *ipv6_conf)
{
	if (ip6_addr_islinklocal((ip6_addr_t *)ipv6_conf->address))
		return IPV6_ADDR_TYPE_LINKLOCAL;
	else if (ip6_addr_isglobal((ip6_addr_t *)ipv6_conf->address))
		return IPV6_ADDR_TYPE_GLOBAL;
	else if (ip6_addr_isuniquelocal((ip6_addr_t *)ipv6_conf->address))
		return IPV6_ADDR_TYPE_UNIQUELOCAL;
	else if (ip6_addr_issitelocal((ip6_addr_t *)ipv6_conf->address))
		return IPV6_ADDR_TYPE_SITELOCAL;
	else
		return IPV6_ADDR_UNKNOWN;
}
#endif /* CONFIG_IPV6 */

int net_dhcp_hostname_set(char *hostname)
{
	netif_set_hostname(&g_mlan.netif, hostname);
	return 0;
}

void net_ipv4stack_init(void)
{
	static bool tcpip_init_done = 0;
	if (tcpip_init_done)
		return;
	
	net_d("Initializing TCP/IP stack\r\n");
	tcpip_init(NULL, NULL);
	tcpip_init_done = true;
}

#ifdef CONFIG_IPV6
void net_ipv6stack_init(struct netif *netif)
{
	uint8_t mac[6];

	netif->flags |= NETIF_IPV6_FLAG_UP;

	/* Set Multicast filter for IPV6 link local address
	 * It contains first three bytes: 0x33 0x33 0xff (fixed)
	 * and last three bytes as last three bytes of device mac */
	mac[0] = 0x33;
	mac[1] = 0x33;
	mac[2] = 0xff;
	mac[3] = netif->hwaddr[3];
	mac[4] = netif->hwaddr[4];
	mac[5] = netif->hwaddr[5];
	wifi_add_mcast_filter(mac);

	netif_create_ip6_linklocal_address(netif, 1);
	netif->ip6_autoconfig_enabled = 1;

	/* IPv6 routers use multicast IPv6 ff02::1 and MAC address
	   33:33:00:00:00:01 for router advertisements */
	mac[0] = 0x33;
	mac[1] = 0x33;
	mac[2] = 0x00;
	mac[3] = 0x00;
	mac[4] = 0x00;
	mac[5] = 0x01;
	wifi_add_mcast_filter(mac);
}

static void wm_netif_ipv6_status_callback(struct netif *n)
{
	/*	TODO: Implement appropriate functionality here*/
	net_d("Received callback on IPv6 address state change");
	wlan_wlcmgr_send_msg(WIFI_EVENT_NET_IPV6_CONFIG,
				     WIFI_EVENT_REASON_SUCCESS, NULL);
}
#endif /* CONFIG_IPV6 */

void net_wlan_init(void)
{
	static int wlan_init_done = 0;
	int ret;
	if (!wlan_init_done) {
		net_ipv4stack_init();
		ip_addr_set_ip4_u32(&g_mlan.ipaddr, INADDR_ANY);
		ret = netifapi_netif_add(&g_mlan.netif, &g_mlan.ipaddr,
					 &g_mlan.ipaddr, &g_mlan.ipaddr, NULL,
					 lwip_netif_init, tcpip_input);
		if (ret) {
			/*FIXME: Handle the error case cleanly */
			net_e("MLAN interface add failed");
		}
#ifdef CONFIG_IPV6
		net_ipv6stack_init(&g_mlan.netif);
#endif /* CONFIG_IPV6 */

		ret = netifapi_netif_add(&g_uap.netif, &g_uap.ipaddr,
					 &g_uap.ipaddr, &g_uap.ipaddr, NULL,
					 lwip_netif_uap_init, tcpip_input);
		if (ret) {
			/*FIXME: Handle the error case cleanly */
			net_e("UAP interface add failed");
		}
		wlan_init_done = 1;
	}

	return;
}

static void dhcp_up(uint32_t ip, uint32_t netmask, uint32_t gateway, uint32_t dnsServer)
{
	IPStatusTypedef netpara;
	IPStatusTypedef *pnetpara = &netpara;
	unsigned char mac[6];
	char macstr[14];
	struct in_addr addr;
	
	os_memset(pnetpara, 0, sizeof(IPStatusTypedef));
	wifi_get_mac_address((char*)mac, CONFIG_ROLE_STA);

	sprintf(macstr, "%02x%02x%02x%02x%02x%02x", mac[0],
			mac[1], mac[2], mac[3], mac[4], mac[5]);
	os_memcpy(pnetpara->mac, macstr, 12);

	addr.s_addr = ip;
	os_strcpy(pnetpara->ip, inet_ntoa(addr));
	addr.s_addr = netmask;
	os_strcpy(pnetpara->mask, inet_ntoa(addr));
	addr.s_addr = gateway;
	os_strcpy(pnetpara->gate, inet_ntoa(addr));
	addr.s_addr = dnsServer;
	os_strcpy(pnetpara->dns, inet_ntoa(addr));
	pnetpara->dhcp = 1;
	sprintf(pnetpara->broadcastip, "255.255.255.255");
	NetCallback(pnetpara);
}

static void wifi_station_changed(int connected)
{
	static int last_state = 0;

	if (connected == last_state)
		return;
	
	last_state = connected;
	if (connected) {
		apinfo_adv_t ap_info;
		uint8_t *key;
		int key_len;

		bk_wlan_get_bssid_info(&ap_info, &key, &key_len);
		connected_ap_info(&ap_info, key, key_len);
	
		WifiStatusHandler(1);
	} else
		WifiStatusHandler(2);
}

void wifi_uap_changed(int connected)
{
	static int last_state = 0;

	if (connected == last_state)
		return;
	
	last_state = connected;
	if (connected) {
		WifiStatusHandler(3);
	} else
		WifiStatusHandler(4);
}

static void wm_netif_status_callback(struct netif *n)
{
    struct dhcp *dhcp;
	ip_addr_t *dns_server;
	uint32_t sed = (uint32_t)aos_now_ms();
	u32 val;
	FUNC_1PARAM_PTR fn;

	/* use current system time to set the start tcp/udp port number RANDOMIZE */
	srand(sed);
	tcp_init();
	udp_init();
    
	if (n->flags & NETIF_FLAG_UP)
	{
		dhcp = netif_dhcp_data(n);        
		if(dhcp != NULL)
		{ 
			if (dhcp->state == DHCP_STATE_BOUND) 
            {
				// dhcp success
				os_printf("IP up: %x\r\n", ip_addr_get_ip4_u32(&n->ip_addr));
				dns_server = (ip_addr_t*)dns_getserver(0);
				dhcp_up(ip_addr_get_ip4_u32(&n->ip_addr), 
                    ip_addr_get_ip4_u32(&n->netmask), 
                    ip_addr_get_ip4_u32(&n->gw),
					ip_addr_get_ip4_u32(dns_server));
				wifi_station_changed(1);
                
#if CFG_ROLE_LAUNCH
                rl_pre_sta_set_status(RL_STATUS_STA_LAUNCHED);
#endif
				fn = bk_wlan_get_status_cb();
				if(fn)
				{
					val = RW_EVT_STA_GOT_IP;
					(*fn)(&val);
				}
				mhdr_set_station_status(RW_EVT_STA_GOT_IP);
				/* dhcp success*/
			} 
			else 
			{
				// dhcp fail
			}
		} 
		else 
		{
			// static IP success;
		}
	} 
	else 
	{
		// dhcp fail;
	}
}

static int check_iface_mask(void *handle, uint32_t ipaddr)
{
	uint32_t interface_ip, interface_mask;
	net_get_if_ip_addr(&interface_ip, handle);
	net_get_if_ip_mask(&interface_mask, handle);
	if (interface_ip > 0)
		if ((interface_ip & interface_mask) ==
		    (ipaddr & interface_mask))
			return 0;
	return -1;
}

void *net_ip_to_interface(uint32_t ipaddr)
{
	int ret;
	void *handle;
	/* Check mlan handle */
	handle = net_get_sta_handle();
	ret = check_iface_mask(handle, ipaddr);
	if (ret == 0)
		return handle;

	/* Check uap handle */
	handle = net_get_uap_handle();
	ret = check_iface_mask(handle, ipaddr);
	if (ret == 0)
		return handle;

	/* If more interfaces are added then above check needs to done for
	 * those newly added interfaces
	 */
	return NULL;
}

void *net_sock_to_interface(int sock)
{
	struct sockaddr_in peer;
	unsigned long peerlen = sizeof(struct sockaddr_in);
	void *req_iface = NULL;

	getpeername(sock, (struct sockaddr *)&peer, &peerlen);
	req_iface = net_ip_to_interface(peer.sin_addr.s_addr);
	return req_iface;
}

void *net_get_sta_handle(void)
{
	return &g_mlan.netif;
}

void *net_get_uap_handle(void)
{
	return &g_uap.netif;
}

void *net_get_netif_handle(uint8_t iface)
{

    return NULL;
}

void net_interface_up(void *intrfc_handle)
{
	struct interface *if_handle = (struct interface *)intrfc_handle;
	netifapi_netif_set_up(&if_handle->netif);
}

void net_interface_down(void *intrfc_handle)
{
	struct interface *if_handle = (struct interface *)intrfc_handle;
	netifapi_netif_set_down(&if_handle->netif);
}

#ifdef CONFIG_IPV6
void net_interface_deregister_ipv6_callback(void *intrfc_handle)
{
	struct interface *if_handle = (struct interface *)intrfc_handle;
	if (intrfc_handle == &g_mlan)
		netif_set_ipv6_status_callback(&if_handle->netif, NULL);
}
#endif

void net_interface_dhcp_stop(void *intrfc_handle)
{
	struct interface *if_handle = (struct interface *)intrfc_handle;
	netifapi_dhcp_stop(&if_handle->netif);
	netif_set_status_callback(&if_handle->netif, NULL);
}

void sta_ip_down(void)
{
	if(sta_ip_start_flag)
	{
		os_printf("sta_ip_down\r\n");
		wifi_station_changed(0);
		sta_ip_start_flag = 0;
		
		netifapi_netif_set_down(&g_mlan.netif);
		netif_set_status_callback(&g_mlan.netif, NULL);
		netifapi_dhcp_stop(&g_mlan.netif);
	}
}

void sta_ip_start(void)
{
    struct wlan_ip_config address = {0};

	if(!sta_ip_start_flag)
	{
		os_printf("sta_ip_start\r\n");
		sta_ip_start_flag = 1;
		net_configure_address(&sta_ip_settings, net_get_sta_handle());

		return;
	}
	
	os_printf("sta_ip_start2:0x%x\r\n", address.ipv4.address);	
    net_get_if_addr(&address, net_get_sta_handle());
    if((mhdr_get_station_status() == RW_EVT_STA_CONNECTED)
		&& (0 != address.ipv4.address))
    {
        mhdr_set_station_status(RW_EVT_STA_GOT_IP);
    }
}

void sta_set_vif_netif(void)
{
    rwm_mgmt_set_vif_netif(&g_mlan.netif);
}

void ap_set_vif_netif(void)
{
    rwm_mgmt_set_vif_netif(&g_uap.netif);
}

void sta_set_default_netif(void)
{
    netifapi_netif_set_default(net_get_sta_handle());
}

void ap_set_default_netif(void)
{
    // as the default netif is sta's netif, so ap need to send
    // boardcast or not sub net packets, need set ap netif before
    // send those packets, after finish sending, reset default netif
    // to sat's netif.
    netifapi_netif_set_default(net_get_uap_handle());
}

void reset_default_netif(void)
{
    netifapi_netif_set_default(NULL);
}

uint32_t sta_ip_is_start(void)
{
	return sta_ip_start_flag;
}

void uap_ip_down(void)
{
	if (uap_ip_start_flag )
	{
		os_printf("uap_ip_down\r\n");
		uap_ip_start_flag = 0;
		wifi_uap_changed(0);
		netifapi_netif_set_down(&g_uap.netif);
		netif_set_status_callback(&g_uap.netif, NULL);
		dhcp_server_stop();
	}
}

void uap_ip_start(void)
{
	if ( !uap_ip_start_flag )
	{
		os_printf("uap_ip_start\r\n");
		uap_ip_start_flag = 1;
    	net_configure_address(&uap_ip_settings, net_get_uap_handle());
    }
}

uint32_t uap_ip_is_start(void)
{
	return uap_ip_start_flag;
}

#define DEF_UAP_IP	0xc0a80a01UL /* 192.168.10.1 */

void ip_address_set(int iface, int dhcp, char *ip, char *mask, char*gw, char*dns)
{
	uint32_t tmp;
	struct ipv4_config addr;
	
	memset(&addr, 0, sizeof(struct ipv4_config));
	if (dhcp == 1) {
		addr.addr_type = ADDR_TYPE_DHCP;
	} else {
		addr.addr_type = ADDR_TYPE_STATIC;
	    tmp = inet_addr((char*)ip);
	    addr.address = (tmp);
	    tmp = inet_addr((char*)mask);
	    if (tmp == 0xFFFFFFFF)
	        tmp = 0x00FFFFFF;// if not set valid netmask, set as 255.255.255.0
	    addr.netmask= (tmp);
	    tmp = inet_addr((char*)gw);
	    addr.gw = (tmp);

	    tmp = inet_addr((char*)dns);
	    addr.dns1 = (tmp);
	}
	if (iface == 1) // STATION
		memcpy(&sta_ip_settings, &addr, sizeof(addr));
	else
		memcpy(&uap_ip_settings, &addr, sizeof(addr));
}

int net_configure_address(struct ipv4_config *addr, void *intrfc_handle)
{
	if (!intrfc_handle)
		return -1;

	struct interface *if_handle = (struct interface *)intrfc_handle;

	net_d("\r\nconfiguring interface %s (with %s)",
		(if_handle == &g_mlan) ? "mlan" :"uap",
		(addr->addr_type == ADDR_TYPE_DHCP)
		? "DHCP client" : "Static IP");
	netifapi_netif_set_down(&if_handle->netif);

	/* De-register previously registered DHCP Callback for correct
	 * address configuration.
	 */
	netif_set_status_callback(&if_handle->netif, NULL);
#ifdef CONFIG_IPV6
	if (if_handle == &g_mlan) {
		netif_set_ipv6_status_callback(&if_handle->netif,
			wm_netif_ipv6_status_callback);
		/* Explicitly call this function so that the linklocal address
		 * gets updated even if the interface does not get any IPv6
		 * address in its lifetime */
		wm_netif_ipv6_status_callback(&if_handle->netif);
	}
#endif
	switch (addr->addr_type) {
	case ADDR_TYPE_STATIC:
		ip_addr_set_ip4_u32(&if_handle->ipaddr, addr->address);
		ip_addr_set_ip4_u32(&if_handle->nmask, addr->netmask);
		ip_addr_set_ip4_u32(&if_handle->gw, addr->gw);
		netifapi_netif_set_addr(&if_handle->netif, &if_handle->ipaddr,
					&if_handle->nmask, &if_handle->gw);
		netifapi_netif_set_up(&if_handle->netif);
		break;

	case ADDR_TYPE_DHCP:
		/* Reset the address since we might be transitioning from static to DHCP */
		memset(&if_handle->ipaddr, 0, sizeof(ip_addr_t));
		memset(&if_handle->nmask, 0, sizeof(ip_addr_t));
		memset(&if_handle->gw, 0, sizeof(ip_addr_t));
		netifapi_netif_set_addr(&if_handle->netif, &if_handle->ipaddr,
				&if_handle->nmask, &if_handle->gw);

		netif_set_status_callback(&if_handle->netif,
					wm_netif_status_callback);
		netifapi_netif_set_up(&if_handle->netif);
		netifapi_dhcp_start(&if_handle->netif);
		break;

	default:
		break;
	}
	/* Finally this should send the following event. */
	if (if_handle == &g_mlan) {
		/* XXX For DHCP, the above event will only indicate that the
		 * DHCP address obtaining process has started. Once the DHCP
		 * address has been obtained, another event,
		 * WD_EVENT_NET_DHCP_CONFIG, should be sent to the wlcmgr.
		 */
		 up_iface = 1;
		 wifi_station_changed(1);
        
         // we always set sta netif as the default.
         sta_set_default_netif();
	} else {
		// softap IP up, start dhcp server;
		dhcp_server_start(net_get_uap_handle());
		up_iface = 0;
		wifi_uap_changed(1);

        // as the default netif is sta's netif, so ap need to send
        // boardcast or not sub net packets, need set ap netif before
        // send those packets, after finish sending, reset default netif
        // to sta's netif.
        os_printf("def netif is no ap's netif, sending boardcast or no-subnet ip packets may failed\r\n");
	}

	return 0;
}

int net_get_if_addr(struct wlan_ip_config *addr, void *intrfc_handle)
{
	const ip_addr_t *tmp;
	struct interface *if_handle = (struct interface *)intrfc_handle;

    if(netif_is_up(&if_handle->netif)) {
	addr->ipv4.address = ip_addr_get_ip4_u32(&if_handle->netif.ip_addr);
	addr->ipv4.netmask = ip_addr_get_ip4_u32(&if_handle->netif.netmask);
	addr->ipv4.gw = ip_addr_get_ip4_u32(&if_handle->netif.gw);

	tmp = dns_getserver(0);
	addr->ipv4.dns1 = ip_addr_get_ip4_u32(tmp);
	tmp = dns_getserver(1);
	addr->ipv4.dns2 = ip_addr_get_ip4_u32(tmp);
    }

	return 0;
}

int net_get_if_macaddr(void *macaddr, void *intrfc_handle)
{
	struct interface *if_handle = (struct interface *)intrfc_handle;

    os_memcpy(macaddr, &if_handle->netif.hwaddr[0], if_handle->netif.hwaddr_len);

	return 0;
}

#ifdef CONFIG_IPV6
int net_get_if_ipv6_addr(struct wlan_ip_config *addr, void *intrfc_handle)
{
	struct interface *if_handle = (struct interface *)intrfc_handle;
	int i;

	for (i = 0; i < MAX_IPV6_ADDRESSES; i++) {
		memcpy(addr->ipv6[i].address,
			if_handle->netif.ip6_addr[i].addr, 16);
		addr->ipv6[i].addr_state = if_handle->netif.ip6_addr_state[i];
	}
	/* TODO carry out more processing based on IPv6 fields in netif */
	return 0;
}

int net_get_if_ipv6_pref_addr(struct wlan_ip_config *addr, void *intrfc_handle)
{
	int i, ret = 0;
	struct interface *if_handle = (struct interface *)intrfc_handle;

	for (i = 0; i < MAX_IPV6_ADDRESSES; i++) {
		if (if_handle->netif.ip6_addr_state[i] == IP6_ADDR_PREFERRED) {
			memcpy(addr->ipv6[ret++].address,
				if_handle->netif.ip6_addr[i].addr, 16);
		}
	}
	return ret;
}
#endif /* CONFIG_IPV6 */

int net_get_if_ip_addr(uint32_t *ip, void *intrfc_handle)
{
	struct interface *if_handle = (struct interface *)intrfc_handle;

	*ip = ip_addr_get_ip4_u32(&if_handle->netif.ip_addr);
	return 0;
}

int net_get_if_gw_addr(uint32_t *ip, void *intrfc_handle)
{
	struct interface *if_handle = (struct interface *)intrfc_handle;

	*ip = ip_addr_get_ip4_u32(&if_handle->netif.gw);
    
	return 0;
}

int net_get_if_ip_mask(uint32_t *nm, void *intrfc_handle)
{
	struct interface *if_handle = (struct interface *)intrfc_handle;

	*nm = ip_addr_get_ip4_u32(&if_handle->netif.netmask);
	return 0;
}

void net_configure_dns(struct wlan_ip_config *ip)
{
	ip_addr_t tmp;

	if (ip->ipv4.addr_type == ADDR_TYPE_STATIC) {
		
		if (ip->ipv4.dns1 == 0)
			ip->ipv4.dns1 = ip->ipv4.gw;
		if (ip->ipv4.dns2 == 0)
			ip->ipv4.dns2 = ip->ipv4.dns1;

		ip_addr_set_ip4_u32(&tmp, ip->ipv4.dns1);
		dns_setserver(0, &tmp);
		ip_addr_set_ip4_u32(&tmp, ip->ipv4.dns2);
		dns_setserver(1, &tmp);
	}

	/* DNS MAX Retries should be configured in lwip/dns.c to 3/4 */
	/* DNS Cache size of about 4 is sufficient */
}

void net_wlan_initial(void)
{
    net_ipv4stack_init();

#ifdef CONFIG_IPV6
    net_ipv6stack_init(&g_mlan.netif);
#endif /* CONFIG_IPV6 */
}

void net_wlan_add_netif(void *mac)
{
    VIF_INF_PTR vif_entry = NULL;  
    struct interface *wlan_if = NULL;
    err_t err;
    u8 vif_idx;
    u8 *b = (u8*)mac;
    
    if(!b || (!(b[0] | b[1] | b[2] | b[3] | b[4] | b[5])))
        return;

    vif_idx = rwm_mgmt_vif_mac2idx(mac);
    if(vif_idx == 0xff) {
        os_printf("net_wlan_add_netif not vif idx found\r\n");
        return ;
    }        
    vif_entry = rwm_mgmt_vif_idx2ptr(vif_idx);
    if(!vif_entry) {
        os_printf("net_wlan_add_netif not vif found, %d\r\n", vif_idx);
        return ;
    }

    if(vif_entry->type == VIF_AP) {
        wlan_if = &g_uap;
    } else if(vif_entry->type == VIF_STA) {
        wlan_if = &g_mlan;
    } else {
        os_printf("net_wlan_add_netif with other role\r\n");
        return ;
    }

    ip_addr_set_ip4_u32(&wlan_if->ipaddr, INADDR_ANY);
    
	err = netifapi_netif_add(&wlan_if->netif, &wlan_if->ipaddr,
				 &wlan_if->ipaddr, &wlan_if->ipaddr, (void*)vif_entry,
				 ethernetif_init, tcpip_input); 

	if (err) {
    	os_printf("net_wlan_add_netif failed\r\n");
    } else {
        vif_entry->priv = &wlan_if->netif;
    }

    os_printf("net_wlan_add_netif done!, vif_idx:%d\r\n", vif_idx);
}

void net_wlan_remove_netif(void *mac)
{
    err_t err;
    u8 vif_idx;    
    VIF_INF_PTR vif_entry = NULL; 
    struct netif *netif = NULL;
    u8 *b = (u8*)mac;
    
    if(!b || (!(b[0] | b[1] | b[2] | b[3] | b[4] | b[5])))
        return;

    vif_idx = rwm_mgmt_vif_mac2idx(mac);
    if(vif_idx == 0xff) {
        os_printf("net_wlan_add_netif not vif idx found\r\n");
        return ;
    }        
    vif_entry = rwm_mgmt_vif_idx2ptr(vif_idx);
    if(!vif_entry) {
        os_printf("net_wlan_add_netif not vif found, %d\r\n", vif_idx);
        return ;
    }

    netif = (struct netif *)vif_entry->priv;
    if(!netif) {
        os_printf("net_wlan_remove_netif netif is null\r\n");
        return;
    }

    err = netifapi_netif_remove(netif); 
    if(err != ERR_OK) {
        os_printf("net_wlan_remove_netif failed\r\n");
    } else {
        netif->state = NULL;
    }

    os_printf("net_wlan_remove_netif done!, vif_idx:%d\r\n", vif_idx);
}




