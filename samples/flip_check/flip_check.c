#include "include.h"
#include <stdio.h>
#include <string.h>
#include "typedef.h"
#include "arm_arch.h"
#include "uart_pub.h"
#include "rtos_pub.h"
#include "error.h"


#include "jpeg_dec.h"
#include "imgproc.h"
#include "motion_detect.h"
#include "video_transfer.h"
#include "samples_config.h"


#ifdef FLIP_CHECK_TEST
#define DIFF_THRESHOLD_LEVEL 35 //parma1
#define FLIP_MINI_INTERVAL 200 //param2

#define MAX_JPG_PIC_SIZE (45*1024)

#define MIN_THREASHOLD 35
#define MAX_THREASHOLD 100
#define PIC_WIDTH 640
#define PIC_HEIGHT 480
#define PIC_RATIO 3

typedef struct _FLIP_CHECK_ST
{
	unsigned char *jpg_src_buf;
	int jpg_file_size;
	uint16_t Y_width;
	uint16_t Y_height;
	rt_mq_t flip_mq;
	rt_sem_t flip_sema;
	MotionDetection md;
}FLIP_CHECK_ST;

typedef struct _FLIP_MSG
{
	uint32_t type;
    unsigned char* arg;
    uint32_t size;
}FLIP_MSG;

static FLIP_CHECK_ST flip;
static uint16_t stop_flag = 0;

static void flip_msg_send(void *buffer, int size,int type)
{
    int ret = RT_EOK;
    FLIP_MSG msg;

    msg.arg = (unsigned char*)buffer;
    msg.size = size;
	msg.type = type;
    ret = rt_mq_send(flip.flip_mq, (void *)&msg, sizeof(FLIP_MSG));
    if (ret != RT_EOK)
        rt_kprintf("send msg failed!!!!\r\n");
}

static int flip_check_init(void)
{
	flip.flip_mq = rt_mq_create("flip_mq", sizeof(FLIP_MSG), 3, RT_IPC_FLAG_FIFO);
	if(NULL == flip.flip_mq)
	{
		rt_kprintf("mq create error!!\r\n");
		return -1;
	}
	flip.flip_sema = rt_sem_create("flip_sema",0,RT_IPC_FLAG_FIFO);
	if(NULL == flip.flip_sema)
	{
		rt_kprintf("sema create error!!\r\n");
		return -1;	
	}
	if(0 != jpg_decoder_init(PIC_WIDTH,PIC_HEIGHT,PIC_RATIO))
		return -1;
	video_buffer_open();
//	video_transfer_set_video_param(QVGA_320_240,TYPE_20FPS);
	rt_kprintf("flip chcek init ok\r\n");
	return RT_EOK;
}

static void flip_check_deinit(void)
{
	rt_kprintf("flip chcek deinit\r\n");
	if(flip.flip_mq)
		rt_mq_delete(flip.flip_mq);
	if(flip.flip_sema)
		rt_sem_delete(flip.flip_sema);
	uinitMotionDetection(&flip.md);
	jpg_decoder_deinit();
	video_buffer_close();
}

static void flip_check_thread(void *parameter)
{
	FLIP_MSG msg;
	unsigned char *Y_buffer;
	int ret;
	int pic_width,pic_heigth;
	
	initMotionDetection(&flip.md,MIN_THREASHOLD,MAX_THREASHOLD,DIFF_THRESHOLD_LEVEL,PIC_HEIGHT>>PIC_RATIO,FLIP_MINI_INTERVAL,rt_tick_get());
	rt_sem_release(flip.flip_sema);//start get one frame jpg pic
	while(1)
	{		
		long tmp = rt_tick_get();
		if(rt_mq_recv(flip.flip_mq, &msg, sizeof(FLIP_MSG), 3000) == RT_EOK)
		{
			flip.jpg_src_buf = msg.arg;
			flip.jpg_file_size = msg.size;
			if(0 == flip.jpg_file_size)
			{
				rt_kprintf("pic error-----\r\n");
				rt_sem_release(flip.flip_sema);//can get another jpg pic
			}
			else
			{
				ret = jpg_decoder_fun(flip.jpg_src_buf, &Y_buffer, flip.jpg_file_size);
				rt_sem_release(flip.flip_sema);//can get another jpg pic

				if(0 == ret)
				{
					jpg_get_pic_size(&pic_width,&pic_heigth);
					ret = getMotionResult(&flip.md,Y_buffer,pic_width>>PIC_RATIO,pic_heigth>>PIC_RATIO,rt_tick_get());
					if(1 == ret)
					{
						rt_kprintf("\r\n---flipped:%x,%x---\r\n",flip.md.lastMdTimer,flip.md.serverWaitTime);
					}
				}
				else
				{
					rt_kprintf("error:%d\r\n",ret);
				}
				
				rt_kprintf("delta2:%x\r\n",rt_tick_get()-tmp);			
			}
			
			if(NULL != Y_buffer)
			{
				rt_free(Y_buffer);
				Y_buffer = NULL;
			}
		}
		else
		{
			rt_kprintf("get msg error!!\r\n");
			break;
		}
	}
	
	stop_flag = 0;
	flip_check_deinit();
}

static void get_pic_thread(void* parameter)
{
	int ret;
	int frame_len;
	unsigned char *jpg_buf = rt_malloc(MAX_JPG_PIC_SIZE);
	ASSERT(NULL != jpg_buf);
	
	while(1)
	{
		ret = rt_sem_take(flip.flip_sema,3000);
		if(RT_EOK == ret)
		{
			if(1 == stop_flag)
				break;
			frame_len = video_buffer_read_frame(jpg_buf, MAX_JPG_PIC_SIZE);
			flip_msg_send(jpg_buf,frame_len,0);
		}
	}
}


static void flipcheck(int argc,char *argv[])
{
	static int thread_onging = 0;
    rt_thread_t tid1 = RT_NULL;
	rt_thread_t tid2 = RT_NULL;

    if(strcmp(argv[1], "start") == 0)
    {
    	if(1 == thread_onging)
    	{
    		rt_kprintf("service is ongoing\r\n");
			return;
    	}
		
		if(RT_EOK != flip_check_init())
			goto exit;


        /* create flip thread */
        tid1 = rt_thread_create("flip",
                               flip_check_thread,
                               RT_NULL,
                               1024,
                               19,
                               10);

        /* create get-jpg-pic thread */
        tid2 = rt_thread_create("get_pic",
                               get_pic_thread,
                               RT_NULL,
                               512,
                               20,
                               10);
        if ((RT_NULL == tid1)||(RT_NULL == tid2))
        {
        	rt_kprintf("error!!\r\n");
			if(tid1)
				rt_thread_delete(tid1);
			if(tid2)
				rt_thread_delete(tid2);
			goto exit;
        }
		else
		{
            rt_thread_startup(tid1);
			rt_thread_startup(tid2);

			
			return;
		}
    }
	else if(strcmp(argv[1], "stop") == 0)
	{
		stop_flag = 1;
		return;
	}
	else
	{
		rt_kprintf("invalid arg!\r\n");
		return;
	}
exit:
	flip_check_deinit(); 
}
MSH_CMD_EXPORT(flipcheck,flip check);
#endif
