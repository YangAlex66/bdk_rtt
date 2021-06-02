#include "include.h"
#include "arm_arch.h"

#if ((CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF) && (CFG_USE_APP_DEMO_VIDEO_TRANSFER))
//#include "FreeRTOS.h"
//#include "task.h"
#include "rtos_pub.h"
#include "error.h"
#include "co_list.h"

#include "video_transfer.h"

#include "drv_model_pub.h"
#include "mem_pub.h"

#include "spidma_intf_pub.h"
#include "camera_intf_pub.h"

#if CFG_GENERAL_DMA
#include "general_dma_pub.h"
#endif

#define TVIDEO_DEBUG                1
#include "uart_pub.h"
#if TVIDEO_DEBUG
#define TVIDEO_PRT                  os_printf
#define TVIDEO_WPRT                 warning_prf
#define TVIDEO_FATAL                fatal_prf
#else
#define TVIDEO_PRT                  null_prf
#define TVIDEO_WPRT                 null_prf
#define TVIDEO_FATAL                null_prf
#endif

#define TVIDEO_DROP_DATA_NONODE     0
#define TVIDEO_USE_HDR              1

#define TVIDEO_RXNODE_SIZE_UDP      1472
#define TVIDEO_RXNODE_SIZE_TCP      1460
#ifndef TVIDEO_RXNODE_SIZE
#define TVIDEO_RXNODE_SIZE          TVIDEO_RXNODE_SIZE_UDP
#endif

#define TVIDEO_DROP_DATA_FLAG       0x01

#if TVIDEO_DROP_DATA_NONODE
#define TVIDEO_POOL_LEN             (TVIDEO_RXNODE_SIZE * 38)  // 54KB
#else
#define TVIDEO_POOL_LEN             (TVIDEO_RXNODE_SIZE * 25)  // 7KB
#endif

#define TVIDEO_RXBUF_LEN            (TVIDEO_RXNODE_SIZE_UDP * 4)

UINT8 tvideo_rxbuf[TVIDEO_RXBUF_LEN];
TVIDEO_DESC_ST tvideo_st;

typedef struct tvideo_elem_st
{
    struct co_list_hdr hdr;
    void *buf_start;
    UINT32 buf_len;
} TVIDEO_ELEM_ST, *TVIDEO_ELEM_PTR;

typedef struct tvideo_pool_st
{
    //UINT8*  pool[TVIDEO_POOL_LEN];
    UINT8 *pool;
    TVIDEO_ELEM_ST elem[TVIDEO_POOL_LEN / TVIDEO_RXNODE_SIZE];
    struct co_list free;
    struct co_list ready;

    #if TVIDEO_DROP_DATA_NONODE
    struct co_list receiving;
    UINT32 drop_pkt_flag;
    #endif

    UINT16 open_type;
    UINT16 send_type;
    video_transfer_send_func send_func;
    video_transfer_start_cb start_cb;
    video_transfer_end_cb end_cb;

    #if(TVIDEO_USE_HDR && CFG_USE_CAMERA_INTF)
    UINT16 frame_id;
    UINT16 pkt_header_size;
    tvideo_add_pkt_header add_pkt_header;
    #endif
} TVIDEO_POOL_ST, *TVIDEO_POOL_PTR;

TVIDEO_POOL_ST tvideo_pool;

enum
{
    TV_INT_POLL          = 0,
    TV_EXIT,
};

typedef struct tvideo_message
{
    UINT32 data;
} TV_MSG_T;

#define TV_QITEM_COUNT      (60)
beken_thread_t  tvideo_thread_hdl = NULL;
beken_queue_t tvideo_msg_que = NULL;

void tvideo_intfer_send_msg(UINT32 new_msg)
{
    OSStatus ret;
    TV_MSG_T msg;

    if (tvideo_msg_que)
    {
        msg.data = new_msg;

        ret = rtos_push_to_queue(&tvideo_msg_que, &msg, BEKEN_NO_WAIT);
        if (kNoErr != ret)
        {
            TVIDEO_FATAL("tvideo_intfer_send_msg failed\r\n");
        }
    }
}

static void tvideo_pool_init(void *data)
{
    UINT32 i = 0;
    TVIDEO_SETUP_DESC_PTR setup = (TVIDEO_SETUP_DESC_PTR)((int)data);

    if (tvideo_pool.pool == NULL)
    {
        tvideo_pool.pool = os_malloc(sizeof(UINT8) * TVIDEO_POOL_LEN);
        if (tvideo_pool.pool == NULL)
        {
            TVIDEO_FATAL("tvideo_pool alloc failed\r\n");
            ASSERT(1);
        }
    }

    os_memset(&tvideo_pool.pool[0], 0, sizeof(UINT8)*TVIDEO_POOL_LEN);

    co_list_init(&tvideo_pool.free);
    co_list_init(&tvideo_pool.ready);
    #if TVIDEO_DROP_DATA_NONODE
    co_list_init(&tvideo_pool.receiving);
    tvideo_pool.drop_pkt_flag = 0;
    #endif

    for (i = 0; i < (TVIDEO_POOL_LEN / TVIDEO_RXNODE_SIZE); i++)
    {
        tvideo_pool.elem[i].buf_start =
            (void *)&tvideo_pool.pool[i * TVIDEO_RXNODE_SIZE];
        tvideo_pool.elem[i].buf_len = 0;

        co_list_push_back(&tvideo_pool.free,
                          (struct co_list_hdr *)&tvideo_pool.elem[i].hdr);
    }

    TVIDEO_PRT("video transfer send type:%d, open type:%d\r\n",
               setup->send_type, setup->open_type);

    tvideo_pool.open_type = setup->open_type;
    tvideo_pool.send_type = setup->send_type;
    tvideo_pool.send_func = setup->send_func;
    tvideo_pool.start_cb = setup->start_cb;
    tvideo_pool.end_cb = setup->end_cb;

    #if(TVIDEO_USE_HDR && CFG_USE_CAMERA_INTF)
    // sccb with camera interface on chip, or default
    if ((tvideo_pool.open_type != TVIDEO_OPEN_SPIDMA)
            && ((setup->pkt_header_size % 4) != 0))
    {
        TVIDEO_WPRT("pkt header-size should 4byte-aligned, but:%d\r\n",
                    setup->pkt_header_size);
    }

    tvideo_pool.frame_id = 0;
    tvideo_pool.add_pkt_header = setup->add_pkt_header;
    tvideo_pool.pkt_header_size = setup->pkt_header_size;
    #endif
}

static void tvideo_rx_handler(void *curptr, UINT32 newlen, UINT32 is_eof, UINT32 frame_len)
{
    TVIDEO_ELEM_PTR elem = NULL;
    do
    {
        if (!newlen)
        {
            break;
        }

        #if TVIDEO_DROP_DATA_NONODE
        // drop pkt has happened, so drop it, until spidma timeout handler.
        if (tvideo_pool.drop_pkt_flag & TVIDEO_DROP_DATA_FLAG)
        {
            break;
        }
        #endif

        elem = (TVIDEO_ELEM_PTR)co_list_pick(&tvideo_pool.free);
        if (elem)
        {
            if (newlen > tvideo_st.node_len)
            {
                newlen = tvideo_st.node_len;
            }

            #if(TVIDEO_USE_HDR && CFG_USE_CAMERA_INTF)
            // sccb with camera interface on chip, or default
            if (tvideo_pool.open_type != TVIDEO_OPEN_SPIDMA)
            {
                UINT32 pkt_cnt = 0;
                TV_HDR_PARAM_ST param;

                if (is_eof)
                {
                    pkt_cnt = frame_len / tvideo_st.node_len;
                    if (frame_len % tvideo_st.node_len)
                    {
                        pkt_cnt += 1;
                    }
                }

                param.ptk_ptr = (UINT8 *)elem->buf_start;
                param.ptklen = newlen;
                param.frame_id = tvideo_pool.frame_id;
                param.is_eof = is_eof;
                param.frame_len = pkt_cnt;

                if (tvideo_pool.add_pkt_header)
                {
                    tvideo_pool.add_pkt_header(&param);
                }

                gdma_memcpy(param.ptk_ptr + tvideo_pool.pkt_header_size, curptr, newlen);
                if (tvideo_st.node_len > newlen)
                {
                    //UINT32 left = tvideo_st.node_len - newlen;
                    //os_memset((elem_tvhdr + 1 + newlen), 0, left);
                }
                //elem->buf_len = tvideo_st.node_len + sizeof(TV_HDR_ST);
                elem->buf_len = newlen + tvideo_pool.pkt_header_size;
            }
            else
            #endif //#if (TVIDEO_USE_HDR && CFG_USE_CAMERA_INTF)
            {
                // only copy data
                gdma_memcpy(elem->buf_start, curptr, newlen);
                if (tvideo_st.node_len > newlen)
                {
                    //UINT32 left = tvideo_st.node_len - newlen;
                    //os_memset(((UINT8*)elem->buf_start + newlen), 0, left);
                }
                //elem->buf_len = tvideo_st.node_len;
                elem->buf_len = newlen;
            }

            co_list_pop_front(&tvideo_pool.free);
            #if TVIDEO_DROP_DATA_NONODE
            co_list_push_back(&tvideo_pool.receiving, (struct co_list_hdr *)&elem->hdr);
            #else
            co_list_push_back(&tvideo_pool.ready, (struct co_list_hdr *)&elem->hdr);
            #endif
        }
        else
        {
            #if TVIDEO_DROP_DATA_NONODE
            // not node for receive pkt, drop aready received, and also drop
            // the new come.
            UINT32 cnt_rdy = co_list_cnt(&tvideo_pool.receiving);

            tvideo_pool.drop_pkt_flag |= TVIDEO_DROP_DATA_FLAG;
            if (cnt_rdy)
            {
                co_list_concat(&tvideo_pool.free, &tvideo_pool.receiving);
            }
            #else
            TVIDEO_WPRT("lost\r\n");
            #endif
        }
    }
    while (0);

    tvideo_intfer_send_msg(TV_INT_POLL);
}

static void tvideo_end_frame_handler(void)
{
    #if TVIDEO_DROP_DATA_NONODE
    // reset drop flag, new pkt can receive
    tvideo_pool.drop_pkt_flag &= (~TVIDEO_DROP_DATA_FLAG);
    if (!co_list_is_empty(&tvideo_pool.receiving))
    {
        co_list_concat(&tvideo_pool.ready, &tvideo_pool.receiving);
    }
    #endif

    #if(TVIDEO_USE_HDR && CFG_USE_CAMERA_INTF)
    if (tvideo_pool.open_type != TVIDEO_OPEN_SPIDMA)
    {
        tvideo_pool.frame_id++;
    }
    #endif

    tvideo_intfer_send_msg(TV_INT_POLL);
}

static void tvideo_config_desc(void)
{
    UINT32 node_len = TVIDEO_RXNODE_SIZE_TCP;

    if (tvideo_pool.send_type == TVIDEO_SND_UDP)
    {
        #if(TVIDEO_USE_HDR && CFG_USE_CAMERA_INTF)
        if (tvideo_pool.open_type != TVIDEO_OPEN_SPIDMA)
        {
            node_len = TVIDEO_RXNODE_SIZE_UDP - tvideo_pool.pkt_header_size;
        }
        else
        #endif
            node_len = TVIDEO_RXNODE_SIZE_UDP;

    }
    else if (tvideo_pool.send_type == TVIDEO_SND_TCP)
    {
        node_len = TVIDEO_RXNODE_SIZE_TCP;
    }
    else if (tvideo_pool.send_type == TVIDEO_SND_INTF)
    {
        #if(TVIDEO_USE_HDR && CFG_USE_CAMERA_INTF)
        node_len = TVIDEO_RXNODE_SIZE_UDP - tvideo_pool.pkt_header_size;
        #else
        node_len = TVIDEO_RXNODE_SIZE_UDP;
        #endif
    }
    else if (tvideo_pool.send_type == TVIDEO_SND_BUFFER)
    {
        node_len = TVIDEO_RXNODE_SIZE_TCP;
    }
    else
    {
        TVIDEO_WPRT("Err snd tpye in spidma\r\n");
    }

    tvideo_st.rxbuf = &tvideo_rxbuf[0];
    tvideo_st.rxbuf_len = node_len * 4;
    tvideo_st.node_len = node_len;
    tvideo_st.rx_read_len = 0;

    tvideo_st.sener_cfg = 0;
    CMPARAM_SET_PPI(tvideo_st.sener_cfg, VGA_640_480);
    // only bk7236 support 720P
    //CMPARAM_SET_PPI(tvideo_st.sener_cfg, VGA_1280_720);

    CMPARAM_SET_FPS(tvideo_st.sener_cfg, TYPE_20FPS);

    tvideo_st.node_full_handler = tvideo_rx_handler;
    tvideo_st.data_end_handler = tvideo_end_frame_handler;
}

static void tvideo_poll_handler(void)
{
    UINT32 send_len;
    TVIDEO_ELEM_PTR elem = NULL;

    do
    {
        elem = (TVIDEO_ELEM_PTR)co_list_pick(&tvideo_pool.ready);
        if (elem)
        {
            if (tvideo_pool.send_func)
            {
                //REG_WRITE((0x00802800+(18*4)), 0x02);
                send_len = tvideo_pool.send_func(elem->buf_start, elem->buf_len);
                //REG_WRITE((0x00802800+(18*4)), 0x00);
                if (send_len != elem->buf_len)
                {
                    break;
                }
            }

            co_list_pop_front(&tvideo_pool.ready);
            co_list_push_back(&tvideo_pool.free, (struct co_list_hdr *)&elem->hdr);
        }
    }
    while (elem);
}

/*---------------------------------------------------------------------------*/
static void video_transfer_main(beken_thread_arg_t data)
{
    OSStatus err;
    TVIDEO_PRT("video_transfer_main entry\r\n");

    tvideo_pool_init(data);
    tvideo_config_desc();

    {
        if (tvideo_pool.open_type == TVIDEO_OPEN_SPIDMA)
        {
            #if CFG_USE_SPIDMA
            spidma_intfer_init(&tvideo_st);
            #endif
        }
        else //if(tvideo_pool.open_type == TVIDEO_OPEN_SCCB)
        {
            camera_intfer_init(&tvideo_st);
        }
    }

    if (tvideo_pool.start_cb != NULL)
    {
        tvideo_pool.start_cb();
    }

    while (1)
    {
        TV_MSG_T msg;
        err = rtos_pop_from_queue(&tvideo_msg_que, &msg, BEKEN_WAIT_FOREVER);
        if (kNoErr == err)
        {
            switch (msg.data)
            {
            case TV_INT_POLL:
                tvideo_poll_handler();
                break;

            case TV_EXIT:
                goto tvideo_exit;
                break;

            default:
                break;
            }
        }
    }

    if (tvideo_pool.end_cb != NULL)
    {
        tvideo_pool.end_cb();
    }

tvideo_exit:
    TVIDEO_PRT("video_transfer_main exit\r\n");

    if (tvideo_pool.pool)
    {
        os_free(tvideo_pool.pool);
        tvideo_pool.pool = NULL;
    }

    if (tvideo_pool.open_type == TVIDEO_OPEN_SPIDMA)
    {
        #if CFG_USE_SPIDMA
        spidma_intfer_deinit();
        #endif
    }
    else //if(tvideo_pool.open_type == TVIDEO_OPEN_SCCB)
    {
        camera_intfer_deinit();
    }

    rtos_deinit_queue(&tvideo_msg_que);
    tvideo_msg_que = NULL;

    tvideo_thread_hdl = NULL;
    rtos_delete_thread(NULL);
}

TVIDEO_SETUP_DESC_ST video_transfer_setup_bak = {0};
int video_transfer_init(TVIDEO_SETUP_DESC_PTR setup_cfg)
{
    int ret;

    TVIDEO_PRT("video_transfer_init %d,%d\r\n", setup_cfg->send_type, setup_cfg->open_type);

    if ((!tvideo_thread_hdl) && (!tvideo_msg_que))
    {
        // bakup setup_cfg, because of that 'setup_cfg' may not static value.
        os_memcpy(&video_transfer_setup_bak, setup_cfg, sizeof(TVIDEO_SETUP_DESC_ST));

        ret = rtos_init_queue(&tvideo_msg_que,
                              "tvideo_queue",
                              sizeof(TV_MSG_T),
                              TV_QITEM_COUNT);
        if (kNoErr != ret)
        {
            TVIDEO_FATAL("spidma_intfer ceate queue failed\r\n");
            return kGeneralErr;
        }

        ret = rtos_create_thread(&tvideo_thread_hdl,
                                 4,
                                 "video_intf",
                                 (beken_thread_function_t)video_transfer_main,
                                 1024,
                                 (beken_thread_arg_t)&video_transfer_setup_bak);
        if (ret != kNoErr)
        {
            rtos_deinit_queue(&tvideo_msg_que);
            tvideo_msg_que = NULL;
            tvideo_thread_hdl = NULL;
            TVIDEO_FATAL("Error: Failed to create spidma_intfer: %d\r\n", ret);
            return kGeneralErr;
        }

        return kNoErr;
    }
    else
    {
        return kInProgressErr;
    }
}

int video_transfer_deinit(void)
{
    TVIDEO_PRT("video_transfer_deinit\r\n");

    tvideo_intfer_send_msg(TV_EXIT);

    while (tvideo_thread_hdl)
    {
        rtos_delay_milliseconds(10);
    }

    return kNoErr;
}

UINT32 video_transfer_set_video_param(UINT32 ppi, UINT32 fps)
{
    #if CFG_USE_CAMERA_INTF
    return camera_intfer_set_video_param(ppi, fps);
    #endif // CFG_USE_CAMERA_INTF
}
#endif  // (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)

