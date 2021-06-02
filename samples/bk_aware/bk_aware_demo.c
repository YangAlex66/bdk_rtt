/* BK_AWARE Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
   This example shows how to use BK_AWARE.
   Prepare two device, one for sending BK_AWARE data and another for receiving
   BK_AWARE data.
*/
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "sys_config.h"
#include "rtos_pub.h"
#include "wlan_ui_pub.h"
#include "common.h"
#include "mem_pub.h"
#include "bk_aware.h"
#include "bk_aware_demo.h"
#include "uart_pub.h"
#include "bk_aware_crc.h"


#if CFG_BK_AWARE
#define BK_AWARE_MAXDELAY 512
extern void bk_aware_monitor_cb(uint8_t *data, int len, wifi_link_info_t *info);
extern int bk_rand();

static beken_queue_t s_example_bk_aware_queue;
static bool bk_aware_demo_started = false;

static uint8_t s_example_broadcast_mac[BK_AWARE_ETH_ALEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
static uint16_t s_example_bk_aware_seq[EXAMPLE_BK_AWARE_DATA_MAX] = { 0, 0 };

static void example_bk_aware_deinit(example_bk_aware_send_param_t *send_param);

/* WiFi should start before using BK_AWARE */
static void example_wifi_init(void)
{
}

/* BK_AWARE sending or receiving callback function is called in WiFi task.
 * Users should not do lengthy operations from this task. Instead, post
 * necessary data to a queue and handle it from a lower priority task. */
void example_bk_aware_send_cb(const uint8_t *mac_addr, bk_aware_send_status_t status)
{
    example_bk_aware_event_t evt;
    example_bk_aware_event_send_cb_t *send_cb = &evt.info.send_cb;

    if (mac_addr == NULL) {
        os_printf("Send cb arg error\n");
        return;
    }

    evt.id = EXAMPLE_BK_AWARE_SEND_CB;
    os_memcpy(send_cb->mac_addr, mac_addr, BK_AWARE_ETH_ALEN);
    send_cb->status = status;
	//os_printf("%s %d: %pm\n", __func__, __LINE__, mac_addr);
    if (rtos_push_to_queue(&s_example_bk_aware_queue, &evt, BEKEN_NO_WAIT) != kNoErr) {
        os_printf("Send send queue fail: %p\n", s_example_bk_aware_queue);
    }
}

static void example_bk_aware_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len)
{
    example_bk_aware_event_t evt;
    example_bk_aware_event_recv_cb_t *recv_cb = &evt.info.recv_cb;

    if (mac_addr == NULL || data == NULL || len <= 0) {
        os_printf("Receive cb arg error\n");
        return;
    }

	//os_printf("XXX %s: mac_addr %pM\n", __func__, mac_addr);
    evt.id = EXAMPLE_BK_AWARE_RECV_CB;
    os_memcpy(recv_cb->mac_addr, mac_addr, BK_AWARE_ETH_ALEN);
    recv_cb->data = os_malloc(len);
    if (recv_cb->data == NULL) {
        os_printf("Malloc receive data fail\n");
        return;
    }
    os_memcpy(recv_cb->data, data, len);
    recv_cb->data_len = len;
    if (rtos_push_to_queue(&s_example_bk_aware_queue, &evt, BEKEN_NO_WAIT /* BEKEN_WAIT_FOREVER */) != kNoErr) {
        os_printf("Send receive queue fail\n");
        os_free(recv_cb->data);
    }
}

/* Parse received BK_AWARE data. */
int example_bk_aware_data_parse(uint8_t *data, uint16_t data_len, uint8_t *state, uint16_t *seq, int *magic)
{
    example_bk_aware_data_t *buf = (example_bk_aware_data_t *)data;
    uint16_t crc, crc_cal = 0;

    if (data_len < sizeof(example_bk_aware_data_t)) {
        os_printf("Receive BK_AWARE data too short, len:%d\n", data_len);
        return -1;
    }

	//print_hex_dump("DATA: ", data, data_len);

    *state = buf->state;
    *seq = buf->seq_num;
    *magic = buf->magic;
    crc = buf->crc;
    buf->crc = 0;
    crc_cal = crc16_le(UINT16_MAX, (uint8_t const *)buf, data_len);

    if (crc_cal == crc) {
        return buf->type;
    }

    return -1;
}

/* Prepare BK_AWARE data to be sent. */
void example_bk_aware_data_prepare(example_bk_aware_send_param_t *send_param)
{
    example_bk_aware_data_t *buf = (example_bk_aware_data_t *)send_param->buffer;

//  assert(send_param->len >= sizeof(example_bk_aware_data_t));

    buf->type = IS_BROADCAST_ADDR(send_param->dest_mac) ? EXAMPLE_BK_AWARE_DATA_BROADCAST : EXAMPLE_BK_AWARE_DATA_UNICAST;
    buf->state = send_param->state;
    buf->seq_num = s_example_bk_aware_seq[buf->type]++;
    buf->crc = 0;
    buf->magic = send_param->magic;
    /* Fill all remaining bytes after the data with random values */
    //bk_fill_random(buf->payload, send_param->len - sizeof(example_bk_aware_data_t));
    buf->crc = crc16_le(UINT16_MAX, (uint8_t const *)buf, send_param->len);
}

static void example_bk_aware_task(void *pvParameter)
{
    example_bk_aware_event_t evt;
    uint8_t recv_state = 0;
    uint16_t recv_seq = 0;
    int recv_magic = 0;
    bool is_broadcast = false;
    int ret;
	bk_err_t err;

	rtos_delay_milliseconds(1);
    os_printf("Start sending broadcast data\n");

    /* Start sending broadcast BK_AWARE data. */
    example_bk_aware_send_param_t *send_param = (example_bk_aware_send_param_t *)pvParameter;
    if ((err = bk_aware_send(send_param->dest_mac, send_param->buffer, send_param->len)) != BK_OK) {
        os_printf("Send error 1, err %d\n", err);
		goto exit;
    }

    while (rtos_pop_from_queue(&s_example_bk_aware_queue, &evt, BEKEN_WAIT_FOREVER) == kNoErr) {
        switch (evt.id) {
            case EXAMPLE_BK_AWARE_SEND_CB:
            {
                example_bk_aware_event_send_cb_t *send_cb = &evt.info.send_cb;
                is_broadcast = IS_BROADCAST_ADDR(send_cb->mac_addr);

                //os_printf("Send data to "MACSTR", status1: %d\n", MAC2STR(send_cb->mac_addr), send_cb->status);

                if (is_broadcast && (send_param->broadcast == false)) {
                    break;
                }

                if (!is_broadcast) {
                    send_param->count--;
                    if (send_param->count == 0) {
                        os_printf("Send done\n");
						goto exit;
                    }
                }

                /* Delay a while before sending the next data. */
                if (send_param->delay > 0) {
					rtos_delay_milliseconds(send_param->delay);
                }

                os_printf("send data to "MACSTR"\n", MAC2STR(send_cb->mac_addr));

                os_memcpy(send_param->dest_mac, send_cb->mac_addr, BK_AWARE_ETH_ALEN);
                example_bk_aware_data_prepare(send_param);

                /* Send the next data after the previous data is sent. */
                if (bk_aware_send(send_param->dest_mac, send_param->buffer, send_param->len) != BK_OK) {
                    os_printf("Send error 2\n");
					goto exit;
                }
                break;
            }
            case EXAMPLE_BK_AWARE_RECV_CB:
            {
                example_bk_aware_event_recv_cb_t *recv_cb = &evt.info.recv_cb;

                ret = example_bk_aware_data_parse(recv_cb->data, recv_cb->data_len, &recv_state, &recv_seq, &recv_magic);
                os_free(recv_cb->data);
                if (ret == EXAMPLE_BK_AWARE_DATA_BROADCAST) {
                    os_printf("Receive %dth broadcast data from: "MACSTR", len: %d\n", recv_seq, MAC2STR(recv_cb->mac_addr), recv_cb->data_len);

                    /* If MAC address does not exist in peer list, add it to peer list. */
                    if (bk_aware_is_peer_exist(recv_cb->mac_addr) == false) {
                        bk_aware_peer_info_t *peer = os_malloc(sizeof(bk_aware_peer_info_t));
                        if (peer == NULL) {
                            os_printf("Malloc peer information fail\n");
							goto exit;
                        }
                        os_memset(peer, 0, sizeof(bk_aware_peer_info_t));
                        peer->channel = CONFIG_BK_AWARE_CHANNEL;
                        peer->ifidx = BK_AWARE_WIFI_IF;
                        peer->encrypt = true;
                        os_memcpy(peer->lmk, CONFIG_BK_AWARE_LMK, BK_AWARE_KEY_LEN);
                        os_memcpy(peer->peer_addr, recv_cb->mac_addr, BK_AWARE_ETH_ALEN);
                        bk_aware_add_peer(peer);
                        os_free(peer);
                    } else {

					}

                    /* Indicates that the device has received broadcast BK_AWARE data. */
					//os_printf("recv_state %d, unicast %d, magic 0x%x/0x%x\n", recv_state,
					//	send_param->unicast, send_param->magic, recv_magic);
                    if (send_param->state == 0) {
                        send_param->state = 1;
                    }

                    /* If receive broadcast BK_AWARE data which indicates that the other device has received
                     * broadcast BK_AWARE data and the local magic number is bigger than that in the received
                     * broadcast BK_AWARE data, stop sending broadcast BK_AWARE data and start sending unicast
                     * BK_AWARE data.
                     */
                    if (recv_state == 1) {
                        /* The device which has the bigger magic number sends BK_AWARE data, the other one
                         * receives BK_AWARE data.
                         */
                        if (send_param->unicast == false && send_param->magic >= recv_magic) {
                    	    os_printf("Start sending unicast data\n");
                    	    os_printf("send data to "MACSTR"\n", MAC2STR(recv_cb->mac_addr));

                    	    /* Start sending unicast BK_AWARE data. */
                            os_memcpy(send_param->dest_mac, recv_cb->mac_addr, BK_AWARE_ETH_ALEN);
                            example_bk_aware_data_prepare(send_param);
                            if (bk_aware_send(send_param->dest_mac, send_param->buffer, send_param->len) != BK_OK) {
                                os_printf("Send error 3\n");
								goto exit;
                            } else {
                                send_param->broadcast = false;
                                send_param->unicast = true;
                            }
                        }
                    }
                } else if (ret == EXAMPLE_BK_AWARE_DATA_UNICAST) {
                    os_printf("Receive %dth unicast data from: "MACSTR", len: %d\n", recv_seq, MAC2STR(recv_cb->mac_addr), recv_cb->data_len);

                    /* If receive unicast BK_AWARE data, also stop sending broadcast BK_AWARE data. */
                    send_param->broadcast = false;
                } else {
                    os_printf("Receive error data from: "MACSTR"\n", MAC2STR(recv_cb->mac_addr));
                }
                break;
            }
			case EXAMPLE_BK_AWARE_EXIT:
				os_printf("Exiting BK AWARE\n");
				goto exit;

            default:
                os_printf("Callback type error: %d\n", evt.id);
                break;
        }
    }

exit:
	example_bk_aware_deinit(send_param);
	bk_aware_demo_started = false;
	rtos_delete_thread(NULL);
}

static bk_err_t example_bk_aware_init(void)
{
    example_bk_aware_send_param_t *send_param;
	beken_thread_t thd;

	rtos_init_queue(&s_example_bk_aware_queue, "bk_aware_queue", sizeof(example_bk_aware_event_t), BK_AWARE_QUEUE_SIZE);

    /* Initialize BK_AWARE and register sending and receiving callback function. */
	bk_aware_init();
	bk_aware_register_send_cb(example_bk_aware_send_cb);
	bk_aware_register_recv_cb(example_bk_aware_recv_cb);
    /* Set primary master key. */
	bk_aware_set_pmk((uint8_t *)CONFIG_BK_AWARE_PMK);

    /* Add broadcast peer information to peer list. */
    bk_aware_peer_info_t *peer = os_malloc(sizeof(bk_aware_peer_info_t));
    if (peer == NULL) {
        os_printf("Malloc peer information fail\n");
        rtos_deinit_queue(&s_example_bk_aware_queue);
        bk_aware_deinit();
        return BK_FAIL;
    }
    os_memset(peer, 0, sizeof(bk_aware_peer_info_t));
    peer->channel = CONFIG_BK_AWARE_CHANNEL;
    peer->ifidx = BK_AWARE_WIFI_IF;
    peer->encrypt = false;
    os_memcpy(peer->peer_addr, s_example_broadcast_mac, BK_AWARE_ETH_ALEN);
    bk_aware_add_peer(peer);
    os_free(peer);

    /* Initialize sending parameters. */
    send_param = os_malloc(sizeof(example_bk_aware_send_param_t));
    os_memset(send_param, 0, sizeof(example_bk_aware_send_param_t));
    if (send_param == NULL) {
        os_printf("Malloc send parameter fail\n");
        rtos_deinit_queue(&s_example_bk_aware_queue);
        bk_aware_deinit();
        return BK_FAIL;
    }
    send_param->unicast = false;
    send_param->broadcast = true;
    send_param->state = 0;
    //send_param->magic = 0xFFFFFFFF;//bk_rand();
    send_param->magic = bk_rand();
    send_param->count = CONFIG_BK_AWARE_SEND_COUNT;
    send_param->delay = CONFIG_BK_AWARE_SEND_DELAY;
    send_param->len = CONFIG_BK_AWARE_SEND_LEN;
    send_param->buffer = os_malloc(CONFIG_BK_AWARE_SEND_LEN);
    if (send_param->buffer == NULL) {
        os_printf("Malloc send buffer fail\n");
        os_free(send_param);
        rtos_deinit_queue(&s_example_bk_aware_queue);
        bk_aware_deinit();
        return BK_FAIL;
    }
    os_memcpy(send_param->dest_mac, s_example_broadcast_mac, BK_AWARE_ETH_ALEN);
    example_bk_aware_data_prepare(send_param);

	rtos_create_thread(&thd, THD_APPLICATION_PRIORITY, "example_bk_aware_task",
				(beken_thread_function_t)example_bk_aware_task,
				2048, send_param);

	bk_aware_demo_started = true;

    return BK_OK;
}

static void example_bk_aware_deinit(example_bk_aware_send_param_t *send_param)
{
    os_free(send_param->buffer);
    os_free(send_param);
    rtos_deinit_queue(&s_example_bk_aware_queue);
    bk_aware_deinit();
}

void bk_aware_demo_main(void)
{
	if (bk_aware_demo_started) {
		os_printf("Already started, use `bk_aware stop` to stop it first\n");
		return;
	}
    example_wifi_init();
    example_bk_aware_init();
}

void bk_aware_demo_stop()
{
	if (bk_aware_demo_started) {
	    example_bk_aware_event_t evt;

	    evt.id = EXAMPLE_BK_AWARE_EXIT;

	    if (rtos_push_to_queue(&s_example_bk_aware_queue, &evt, BEKEN_WAIT_FOREVER) != kNoErr) {
	        os_printf("Exit bk aware demo failed\n");
	    }
	}
}

#ifdef FINSH_USING_MSH
static void bk_wifi_aware_command(int argc, char **argv)
{
	if (argc == 2 && os_strcmp(argv[1], "start") == 0) {
		bk_aware_demo_main();
	} else if (argc == 2 && os_strcmp(argv[1], "stop") == 0) {
		bk_aware_demo_stop();
	} else {
		os_printf("Usage: bk_aware <start>|<stop>\n");
	}
}

MSH_CMD_EXPORT_ALIAS(bk_wifi_aware_command, bk_aware, bk_aware demo);
#endif

#endif // CFG_BK_AWARE

