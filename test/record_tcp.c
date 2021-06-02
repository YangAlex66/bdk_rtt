#include <rtthread.h>
#include <rtdevice.h>

#ifdef RECORD_COM_TCP_TEST
#include <finsh.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "board.h"
#include "audio_device.h"
#include "vad.h"
#include "opus.h"
#include "amrnb_encoder.h"
#include <sys/socket.h> /* 使用BSD socket，需要包含sockets.h头文件 */
#include "netdb.h"
#include "test_config.h"



#define EVENT_TCP_START 	(1<<0)
#define EVENT_TCP_DATA 	(1<<1)
#define EVENT_TCP_CANCEL (1<<2)
#define EVENT_TCP_END 	(1<<3)
#define EVENT_TCP_ALL   (EVENT_TCP_START|EVENT_TCP_DATA|EVENT_TCP_CANCEL|EVENT_TCP_END)

/*record thread received msg*/
#define MSG_RECORD_START 	1
#define MSG_RECORD_CANCEL  	2
#define USING_VAD
#define DURATION_PER_FRAME 20
#define NOTIFY_FRAME_COUNT 	(200/DURATION_PER_FRAME) /*200ms*/
#define FRAME_COUNT_PER_SECOND (1000/DURATION_PER_FRAME)
#define AMR_MAGIC_NUMBER  "#!AMR\n"

#define MAX_DATA_BUF_SIZE (320 *60)

typedef struct tcp_net_worker
{
    char *url;
    int port;
    int sock;
}tcp_net_worker_t;

typedef enum
{
	REC_ENCODE_NONE = 0,
	REC_ENCODE_START,
	REC_ENCODE_GOING,
	REC_ENCODE_CANCEL,
}REC_ENCODE_STATE;

typedef enum
{
	ARM_ENCODE_MODE = 0,
	OPUS_ENCODE_MODE,
	NO_ENCODE_MODE,
}REC_ENCODE_MODE;

struct rec_encoder_manager
{
	rt_event_t rec_evt;
	rt_mailbox_t rec_mb;
	uint16_t encoded_len;
	REC_ENCODE_MODE encoder_mode;
    int sample_rate;
	rt_uint8_t data_buf[MAX_DATA_BUF_SIZE];
};

static tcp_net_worker_t tcpclient;
static struct rec_encoder_manager *rec_encoder = NULL;

/* opus */
static void int_to_char_big_endian(int i, unsigned char ch[4])
{
    ch[0] = i>>24;
    ch[1] = (i>>16)&0xFF;
    ch[2] = (i>>8)&0xFF;
    ch[3] = i&0xFF;
}

static void set_start_event(void)
{
	rt_event_send(rec_encoder->rec_evt, EVENT_TCP_START);
}

static void set_end_event(void)
{
	rt_event_send(rec_encoder->rec_evt, EVENT_TCP_END);
}

static void free_rec_enc(void)
{
	if(rec_encoder != RT_NULL)
	{
		if(rec_encoder->rec_evt)
			rt_event_delete(rec_encoder->rec_evt);
		if(rec_encoder->rec_mb)
			rt_mb_delete(rec_encoder->rec_mb);
		rt_free(rec_encoder);
		rec_encoder = RT_NULL;
	}
}

/*main thread process record and amr-encode*/
static void rec_encoder_thread(void *parameter)
{
    

    enum AMRNB_MODE amr_enc_mode = AMRNB_MODE_MR122;
    void *amr = NULL;

	REC_ENCODE_STATE rec_state;    
	OpusEncoder *opus_enc = RT_NULL;

	uint8_t vad_end_flag;
	int ret,i,read_bytes,enc_len;
	short *in_pcm_buf;
	rt_tick_t tmp_tick;
	rt_uint32_t mb_msg;
   	uint16_t pcm_len_per_frame;
	
	rt_kprintf("record encoder start \r\n");
	
	rec_state = REC_ENCODE_NONE;
	rec_encoder->encoded_len = 0;
	rec_encoder->sample_rate = 8000;
	//just one channel data
	pcm_len_per_frame = (rec_encoder->sample_rate / FRAME_COUNT_PER_SECOND) * 2;
	in_pcm_buf = (short*)rt_malloc(pcm_len_per_frame);
	ASSERT(NULL != in_pcm_buf);

	rt_kprintf("record encoder start len=%d\r\n",pcm_len_per_frame);
	rt_thread_delay(100);

	while(1)
	{
		ret = rt_mb_recv(rec_encoder->rec_mb,&mb_msg, RT_WAITING_NO);
		if(RT_EOK == ret)
		{
			rt_kprintf("---mb receive msg:%x----\r\n",mb_msg);
			if(REC_ENCODE_GOING == rec_state)
			{
				#ifdef USING_VAD
				wb_vad_deinit();
				#endif
				audio_device_mic_close();
				if(OPUS_ENCODE_MODE==rec_encoder->encoder_mode)
					opus_encoder_destroy(opus_enc);
				else if(ARM_ENCODE_MODE==rec_encoder->encoder_mode)
					amrnb_encoder_deinit(&amr);
			}
			
			if(MSG_RECORD_START == mb_msg )
			{
				rt_kprintf("----start---\r\n");
				rec_state = REC_ENCODE_START;
			}
			else if(MSG_RECORD_CANCEL == mb_msg )
			{
				if(REC_ENCODE_GOING == rec_state)
				{
					rt_kprintf("----cancel---\r\n");
					rec_state = REC_ENCODE_NONE;
				}
			}
		}
		
		switch(rec_state)
		{
			case REC_ENCODE_NONE:
				rt_thread_delay(100);
				break;
				
			case REC_ENCODE_START:
				set_start_event();//nofity	
				audio_device_mic_open();
				audio_device_mic_set_channel(1);
				audio_device_mic_set_rate(8000);
				
				#ifdef USING_VAD
				rt_kprintf("---wb_vad_enter---\r\n");
				vad_end_flag = 0;
				wb_vad_enter();//vad start
				#endif

				if(OPUS_ENCODE_MODE==rec_encoder->encoder_mode)
				{

					int channels,application,complexity,errors;
					opus_int32 bitrate_bps;
			        enc_len = opus_encoder_get_size(1);
			        rt_kprintf("opus_encoder_get_size: 1 channel size: %d \n", enc_len);
			        enc_len = opus_encoder_get_size(2);
			        rt_kprintf("opus_encoder_get_size: 2 channel size: %d \n", enc_len);

			        channels = 1;
			        application = OPUS_APPLICATION_VOIP;
			        complexity = 1; // 1 to 10
			        opus_enc = opus_encoder_create(rec_encoder->sample_rate, channels, application, &errors);
			        if(errors != OPUS_OK)
			        {
			            rt_kprintf("[opus]:create opus encoder failed : %d! \n", errors);
			        }
			        rt_kprintf("---start opus encode--- \r\n");
			        opus_encoder_set_complexity(opus_enc, complexity);
					opus_encoder_get_bitrate(opus_enc,bitrate_bps );	
			        rt_kprintf("[opus]:default bitrate %d\n", bitrate_bps);
			        rec_encoder->encoded_len=0;
			    }
			    else if(ARM_ENCODE_MODE==rec_encoder->encoder_mode)
			    {
			    	ret = amrnb_encoder_init(&amr, 0, rt_malloc, rt_free);
			    	if(0 != ret)
			        {
			            rt_kprintf("[amr]:create amr encoder failed \n");
			        }
					rt_kprintf("---start amr encode--- \r\n");
				    /* write amr head */
				    memcpy(rec_encoder->data_buf, AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER));
					rec_encoder->encoded_len = strlen(AMR_MAGIC_NUMBER);
			    }
				else if(NO_ENCODE_MODE==rec_encoder->encoder_mode)
				{
					rec_encoder->encoded_len=0;
				}
				rec_state = REC_ENCODE_GOING;
				break;
				
			case REC_ENCODE_GOING:
	        	read_bytes = 0;
				i = 0;
				tmp_tick = rt_tick_get();
				
				while(i<NOTIFY_FRAME_COUNT)
				{
					
					if(rec_encoder->encoded_len >=MAX_DATA_BUF_SIZE)
					{
						rt_kprintf("+++++++record done++++\r\n");
						rec_encoder->encoded_len=MAX_DATA_BUF_SIZE;
						break;	
					}
		            /* read data from sound device */
		            read_bytes=audio_device_mic_read(in_pcm_buf, pcm_len_per_frame);
		            rt_kprintf("read_bytes:%d\r\n",read_bytes);
		            /*encode ....*/
		            if(OPUS_ENCODE_MODE==rec_encoder->encoder_mode)
		            {
		                enc_len = opus_encode(opus_enc, in_pcm_buf, pcm_len_per_frame/2, rec_encoder->data_buf+rec_encoder->encoded_len + 8, 120 - 8);
		                if(enc_len>0)
		                {    
		                	 /* write head */
		                    opus_uint32 enc_final_range;
		                    int_to_char_big_endian(enc_len, rec_encoder->data_buf+rec_encoder->encoded_len);
		                    opus_encoder_get_final_range(opus_enc, enc_final_range);
		                    int_to_char_big_endian(enc_final_range, rec_encoder->data_buf+rec_encoder->encoded_len+4);
		                	enc_len += 8;
		                	rec_encoder->encoded_len+=enc_len;
		           		}
		           		else
		           		{
		           			rt_kprintf("opus encode error!!\r\n");
		           		}

		       
		            }
		            else if(ARM_ENCODE_MODE==rec_encoder->encoder_mode)
		            {
		            	enc_len = amrnb_encoder_encode(amr,amr_enc_mode,in_pcm_buf,rec_encoder->data_buf + rec_encoder->encoded_len);
						if(enc_len > 0)
						{
							rec_encoder->encoded_len += enc_len;
						}
						else
						{
							rt_kprintf("amr encode error!!\r\n");
						}
						
		            }
		            else if(NO_ENCODE_MODE==rec_encoder->encoder_mode)
					{
						memcpy(rec_encoder->data_buf + rec_encoder->encoded_len,in_pcm_buf,read_bytes);
						rec_encoder->encoded_len +=read_bytes;
					}
		        	#ifdef USING_VAD
					if(wb_vad_entry((char*)in_pcm_buf, read_bytes))/*vad process*/
					{
						rt_kprintf("------------vad end----------\r\n");
						vad_end_flag = 1;
						break;
					}
					#endif
					i++;
		   	
				}
				rt_kprintf("--time:%d ms---\r\n",rt_tick_get()-tmp_tick);
				//set_data_event();//optional
				#ifdef USING_VAD
				if((rec_encoder->encoded_len >=MAX_DATA_BUF_SIZE)||(1 == vad_end_flag)){	
					wb_vad_deinit();
				#else
				if(rec_encoder->encoded_len >=MAX_DATA_BUF_SIZE){
				#endif
					rt_kprintf("--record end:len = %d---\r\n",rec_encoder->encoded_len);
					rt_thread_delay(5);
					set_end_event();//nofity
					audio_device_mic_close();
					if(OPUS_ENCODE_MODE==rec_encoder->encoder_mode)
						opus_encoder_destroy(opus_enc);
					else if(ARM_ENCODE_MODE==rec_encoder->encoder_mode)
						amrnb_encoder_deinit(&amr);
					rec_state = REC_ENCODE_NONE;
				}
				break;
			default:
				rec_state = REC_ENCODE_NONE;
				break;
		}
	}
	free_rec_enc();
}



static rt_err_t get_record_event(rt_uint32_t *event,rt_int32_t timeout)
{
	 return rt_event_recv(rec_encoder->rec_evt,EVENT_TCP_ALL,\
				RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,timeout,event);
}

/*be called by talk&wechat proces*/
static char *get_data_buf(void)
{
	return rec_encoder->data_buf;
}
/*be called by talk&wechat proces*/
static int get_data_len(void)
{
	return rec_encoder->encoded_len;
}

static void net_transmit_thread_entry(void *parameter)
{
    int cmd;
    int ret,size;
    int sock;
    rt_uint32_t evt;
    char *buf=NULL;
	struct hostent *host;
    struct sockaddr_in server_addr;
    host = gethostbyname(tcpclient.url);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        rt_kprintf("[tcp]:Socket error\n");;
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(tcpclient.port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        rt_kprintf("[tcp]:Connect fail!\n");
        closesocket(sock);
        return ;
    }
    else
    {
    	rt_kprintf("tcp connected success\r\n");
    }
    while(1)
    {
    	ret = get_record_event(&evt,RT_WAITING_FOREVER);
        if(RT_EOK == ret)
		{

			if(evt & EVENT_TCP_START)
			{
				rt_kprintf("record start\r\n");
			}
			else if(evt & EVENT_TCP_END)
			{
				buf = get_data_buf();
				size = get_data_len();
				rt_kprintf("=====================tcp send mic data===================\r\n");
				rt_kprintf("size:%d\r\n",size);
				send(sock,buf,size,0);
			} 
        }  
    }
}


static void record_start()
{
	if(NULL!=rec_encoder->rec_mb)
    	rt_mb_send(rec_encoder->rec_mb, MSG_RECORD_START);
}


static void record_cancel()
{
	if(NULL!=rec_encoder->rec_mb)
		rt_mb_send(rec_encoder->rec_mb, MSG_RECORD_CANCEL);
}

static void record_help()
{
	rt_kprintf("eg:  record_main start 0 192.168.1.100 8080 \r\n");
	rt_kprintf("eg:  record_main record_again \r\n");
	rt_kprintf("eg:  record_main record_stop \r\n");
	rt_kprintf("eg:  record_main enc_mode_change 1 \r\n");
}

static void record_main(int argc,char **argv)
{    
	rt_thread_t tid;

	if (strcmp(argv[1], "start") == 0)
    {
    	if(NULL == rec_encoder)
	    {
	    	if(argc<5)
	    	{
	    		rt_kprintf("argc error\r\n");
	    		record_help();
	    		return;
	    	}

	        rec_encoder = rt_calloc(1, sizeof(struct rec_encoder_manager));
	        if(NULL == rec_encoder)
	        {
	            rt_kprintf("rec_enoder_init error!!\r\n");
	            return;
	        }
	      	rec_encoder->rec_evt = rt_event_create("rec evt",RT_IPC_FLAG_FIFO);
			if(NULL == rec_encoder->rec_evt)
				goto exit;
			rec_encoder->rec_mb = rt_mb_create("rec mb",3,RT_IPC_FLAG_FIFO);
			if(NULL == rec_encoder->rec_mb)
				goto exit;    

			rec_encoder->encoder_mode = atoi(argv[2]);//ARM_ENCODE_MODE:0,OPUS_ENCODE_MODE:1,PCM:2
			tcpclient.url = rt_strdup(argv[3]);      // eg:192.168.1.100
			tcpclient.port = atoi(argv[4]);			

			/* create rec-encoder thread */
		    tid = rt_thread_create("rec_enc",rec_encoder_thread,NULL,1024 * 32,20,10);
		    if (tid)
		    	rt_thread_startup(tid);

			tid = rt_thread_create("net_send",net_transmit_thread_entry,RT_NULL,1024 * 8,21,10);
		    if (tid)
		    	rt_thread_startup(tid);
		}

        record_start();
    }
    else if (strcmp(argv[1], "record_again") == 0)
    {	
    	record_start();

    }
    else if (strcmp(argv[1], "enc_mode_change") == 0)
    {	
    	if(argc<3)
    	{
    		rt_kprintf("argc error\r\n");
    		record_help();
    		return;
    	}
    	rec_encoder->encoder_mode = atoi(argv[2]);//ARM_ENCODE_MODE:0,OPUS_ENCODE_MODE:1,PCM:2
    	record_start();
    }
   	else if (strcmp(argv[1], "record_stop") == 0)
    {	
    	record_cancel();
    }
    else
    {	
    	rt_kprintf("error argv!!!!\r\n");
    	record_help();
    }

	return;
exit:
	rt_kprintf("error,exit\r\n");
	free_rec_enc(); 
}

//eg:
MSH_CMD_EXPORT(record_main,record_main);

#endif