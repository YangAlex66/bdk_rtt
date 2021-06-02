
#include <rtthread.h>
#include <rtdevice.h>
#include <finsh.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "board.h"
#include "audio_device.h"
#include "vad.h"
#include "test_config.h"
#ifdef MICPHONE_TEST
static uint8_t *test_buf;
#define TEST_BUFF_LEN 80*1024
#define READ_SIZE 2048

/********************************************************
 	argv[1]: vad  off:0  on:1
 	argv[2]: work_mode  	
 	0: one micphone(mic1)
	1: two micphones( mic1 && mic2 ) -- use two micphone data
	2: two micphones( mic1 && mic2 ) -- only use mic1 data
	3: two micphones( mic1 && mic2 ) -- only use mic2 data
 	argv[3]: sample_rate  8000/16000
 	
	cmd format :record_and_play 0/1 0/1/2/3 8000/16000
	eg: record_and_play 1 0 8000

**********************************************************/

void record_and_play(int argc,char *argv[])
{
	int mic_read_len = 0;
	int actual_len,i;
	int dac_wr_len=0;
	uint16_t *buffer = NULL;
	uint16_t read_size;
	int sample_rate;
	int index;
    uint16_t *ptr;
	int work_mode;
	int vad_on;
	if(argc<4)
	{
		rt_kprintf("parameter errorr\r\n");
	}
	vad_on = atoi(argv[1]);
	work_mode = atoi(argv[2]);
	sample_rate = atoi(argv[3]);

	test_buf = sdram_malloc(TEST_BUFF_LEN);
	if(test_buf == NULL)
	{
		rt_kprintf("===not enough memory===\r\n");
		return;
	}
	
	audio_device_init();

	audio_device_mic_open();

	if(work_mode == 0)
	{
		audio_device_mic_set_channel(1);
		read_size = READ_SIZE;
	}
	else
	{
		audio_device_mic_set_channel(2);
		read_size = READ_SIZE *2;
	}

	audio_device_mic_set_rate(sample_rate);


	if (vad_on)
	{
	    rt_kprintf("Vad is ON !!!!!!!!\r\n");	/*进入vad检测*/
		wb_vad_enter();
	}

	while(1)
	{
		rt_thread_delay(10);
		if(mic_read_len > TEST_BUFF_LEN - READ_SIZE)
			break;
		actual_len = audio_device_mic_read(test_buf+mic_read_len,read_size);
		mic_read_len += actual_len;
		if(vad_on)
		{
			if(wb_vad_entry((char*)test_buf+mic_read_len, 320))/*vad process*/
			{
				rt_kprintf("------------vad end----------\r\n");
				break;
			}
		}
	}
	rt_kprintf("mic_read_len is %d\r\n", mic_read_len);
	audio_device_mic_close();
	if (vad_on)
	{
		wb_vad_deinit();			/*关闭vad检测*/		
	}

	audio_device_open();
	audio_device_set_rate(sample_rate);

	while(1)
	{
		buffer = (uint16_t *)audio_device_get_buffer(RT_NULL);
		if(dac_wr_len >= mic_read_len)
		{
			audio_device_put_buffer(buffer);
			break;
		}
		
		memcpy(buffer,test_buf+dac_wr_len,read_size);
		dac_wr_len += read_size;

		switch(work_mode)
		{
			case 0:
			//expand to 2 channels
				ptr = (uint16_t *)((uint8_t *)buffer + read_size * 2);
				ptr -= 1;
		        for (index = 1; index < read_size / 2; index ++)
		        {
		            *ptr = *(ptr - 1) = buffer[read_size / 2 - index];
		            ptr -= 2;
		        }
				audio_device_write((uint8_t *)buffer, read_size * 2);
				break;
			case 1:
				audio_device_write((uint8_t *)buffer, read_size);
				break;
			case 2:
				ptr = (uint16_t*)buffer;
				for(index = 0;index < read_size/2;)
				{
					ptr[index+1] = ptr[index];
					index += 2;
				}
				audio_device_write((uint8_t *)buffer, read_size);
				break;
			case 3:
				ptr = (uint16_t*)buffer;
				for(index = 0;index < read_size/2;)
				{
					ptr[index] = ptr[index+1];
					index += 2;
				}
				audio_device_write((uint8_t *)buffer, read_size);
				break;
			default:
				break;
		}
	}
	audio_device_close();

	if(test_buf)
		sdram_free(test_buf);
}
MSH_CMD_EXPORT(record_and_play, record play);

#endif