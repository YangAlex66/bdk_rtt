#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <rtthread.h>
#include <finsh.h>
#include <dfs_posix.h>
#include <sys/socket.h> 
#include "include.h"
#include "net.h"
#include "typedef.h"
#include "arm_arch.h"
#include "uart_pub.h"
#include "rtos_pub.h"
#include "error.h"
#include "video_transfer.h"
#include "test_config.h"

#ifdef WEB_CAMERA_TEST
#define MJPEG_BOUNDARY "boundarydonotcross"
#define MAX_BUF_SIZE    40*1024
static int  g_mjpeg_stop = 0;
static char g_send_buf[1024];

int send_first_response(int client)
{
    g_send_buf[0] = 0;

    rt_snprintf(g_send_buf, 1024,
             "HTTP/1.0 200 OK\r\n"
             "Connection: close\r\n"
             "Server: MJPG-Streamer/0.2\r\n"
             "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0,"
             " post-check=0, max-age=0\r\n"
             "Pragma: no-cache\r\n"
             "Expires: Mon, 3 Jan 2000 12:34:56 GMT\r\n"
             "Content-Type: multipart/x-mixed-replace;boundary="
             MJPEG_BOUNDARY "\r\n"
             "\r\n"
             "--" MJPEG_BOUNDARY "\r\n");
    if (send(client, g_send_buf, strlen(g_send_buf), 0) < 0)
    {
        close(client);
        return -1;
    }

    return 0;
}

int mjpeg_send_stream(int client, void *data, int size)
{
    g_send_buf[0] = 0;

    if (!g_mjpeg_stop)
    {
        snprintf(g_send_buf, 1024,
                 "Content-Type: image/jpeg\r\n"
                 "Content-Length: %d\r\n"
                 "\r\n", size);
        if (send(client, g_send_buf, strlen(g_send_buf), 0) < 0)
        {
            close(client);
            return -1;
        }

        if (send(client, data, size, 0) < 0)
        {
            close(client);
            return -1;
        }

        g_send_buf[0] = 0;
        snprintf(g_send_buf, 1024, "\r\n--" MJPEG_BOUNDARY "\r\n");
        if (send(client, g_send_buf, strlen(g_send_buf), 0) < 0)
        {
        	close(client);
        	return -1;
        }

        return 0;
    }

    return -1;
}

void mjpeg_server_thread(void *arg)
{
    int on;
    int srv_sock = -1;
    int fream_length=0;
    struct sockaddr_in addr;
    socklen_t sock_len = sizeof(struct sockaddr_in);

    int bufsz = 50 * 1024;
    uint8_t *buf = (uint8_t *) malloc (MAX_BUF_SIZE);

    if (!buf)
    {
        rt_kprintf("no buffer yet!\n");
        return ;
    }


    srv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (srv_sock < 0)
    {
        rt_kprintf("mjpeg_server: create server socket failed due to (%s)\n",
              strerror(errno));
        goto exit;
    }

    bzero(&addr, sock_len);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* ignore "socket already in use" errors */
    on = 1;
    lwip_setsockopt(srv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    lwip_setsockopt(srv_sock, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));

    if (bind(srv_sock, (struct sockaddr *)&addr, sock_len) != 0)
    {
        rt_kprintf("mjpeg_server: bind() failed due to (%s)\n",
              strerror(errno));
        goto exit;
    }

    if (listen(srv_sock , RT_LWIP_TCP_PCB_NUM) != 0)
    {
        rt_kprintf("mjpeg_server: listen() failed due to (%s)\n",
              strerror(errno));
        goto exit;
    }

    g_mjpeg_stop = 0;
    while (!g_mjpeg_stop)
    {
    	struct sockaddr_in client_addr;
		int client = accept(srv_sock, (struct sockaddr *)&client_addr, &sock_len);
		if (client < 0)
			continue;

		rt_kprintf("mjpeg_server: client connected\n");
		if (send_first_response(client) < 0)
		{
			client = -1;
			continue;
		}

		while (1)
		{
			fream_length = 0;
			/* capture a jpeg frame */
            fream_length = video_buffer_read_frame(buf, MAX_BUF_SIZE);
            rt_kprintf("len:%d\r\n",fream_length);
			if (fream_length !=0)
			{
				/* send out this frame */
				if (mjpeg_send_stream(client, (void*)buf, fream_length) < 0)
				{
					rt_kprintf("client disconnected!\n");
					break;
				}
			}
		}
	}

exit:
	if (srv_sock >= 0) 
        close(srv_sock);
	if (buf)
    {
        free(buf);
        buf=NULL;
    }
}

int web_jpeg_stream(int argc, char** argv)
{
    int frame_len;
    unsigned char *jpg_buf = NULL;
    if (argc != 2)
    {
        rt_kprintf("%s start|stop\n", argv[0]);
        return 0;
    }

    video_buffer_open();

    if (strcmp(argv[1], "start") == 0)
    {
        rt_kprintf("start web camerar\r\n");
        rt_thread_t tid;
        tid = rt_thread_create("jpeg_stream", mjpeg_server_thread, NULL, 2048,20, 10);
        if (tid) 
            rt_thread_startup(tid);
    }
    else
    {
        g_mjpeg_stop = 1;
    }

    return 0;
}

static void fream_set_video_param(int argc,char* argv[])
{

    if(1==atoi(argv[1]))
    {
        video_transfer_set_video_param(QVGA_320_240,TYPE_20FPS);
    }
    else if(2==atoi(argv[1]))
    {
        video_transfer_set_video_param(VGA_640_480,TYPE_20FPS);
    }
}


MSH_CMD_EXPORT(web_jpeg_stream, web_jpeg_stream server);
MSH_CMD_EXPORT(fream_set_video_param, fream_set_video_param cmd);
#endif
