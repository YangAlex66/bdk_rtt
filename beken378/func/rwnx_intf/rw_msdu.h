#ifndef _RW_MSDU_H_
#define _RW_MSDU_H_

#include "sys_config.h"
#include "doubly_list.h"
#include "tx_swdesc.h"
#include "lwip/pbuf.h"
#include "rtos_pub.h"
#include "rwnx.h"

#define MSDU_TX_MAX_CNT               (32)
#define MSDU_RX_MAX_CNT               (32)

#define ETH_ADDR_LEN	               6		/* Octets in one ethernet addr	 */

static inline int is_broadcast_eth_addr(const u8 *a)
{
    return (a[0] & a[1] & a[2] & a[3] & a[4] & a[5]) == 0xff;
}

typedef struct _eth_hdr
{
    UINT8 e_dest[ETH_ADDR_LEN];
    UINT8 e_src[ETH_ADDR_LEN];
    UINT16 e_proto;
} __attribute__((packed)) ETH_HDR_T, *ETH_HDR_PTR;

struct ieee80211_tx_cb {
	beken_semaphore_t sema;
	int result;
};

typedef struct _msdu_node_
{
    LIST_HEADER_T hdr;

    UINT8 *msdu_ptr;
    UINT32 len;

    UINT8 vif_idx;
    UINT8 sta_idx;
	void *args;
	int sync;
} MSDU_NODE_T, *MSDU_NODE_PTR;

extern void rwm_push_rx_list(MSDU_NODE_T *node);
extern MSDU_NODE_T *rwm_pop_rx_list(void);
extern void rwm_tx_confirm(void *);
extern void rwm_tx_msdu_renew(UINT8 *buf, UINT32 len, UINT8 *orig_addr);
extern UINT8 *rwm_get_msdu_content_ptr(MSDU_NODE_T *node);
extern void rwm_txdesc_copy(struct txdesc *dst_local, ETH_HDR_PTR eth_hdr_ptr);
extern int rwm_raw_frame_with_cb(uint8_t *buffer, int len, void *cb, void *param);
extern MSDU_NODE_T *rwm_tx_node_alloc(UINT32 len);
extern void rwm_node_free(MSDU_NODE_T *node);
extern UINT8 *rwm_rx_buf_alloc(UINT32 len);
extern UINT32 rwm_upload_data(RW_RXIFO_PTR rx_info);
extern UINT32 rwm_get_rx_free_node(struct pbuf **p_ret, UINT32 len);
extern UINT32 rwm_get_rx_valid(void);
extern UINT8 rwm_get_tid();
extern void rwm_set_tid(UINT8 tid);
uint8_t classify8021d(UINT8 *buf);

#endif // _RW_MSDU_H_
// eof

