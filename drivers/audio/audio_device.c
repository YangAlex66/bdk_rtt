#include <stdint.h>
#include <stdlib.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/audio.h>

#include "audio_device.h"
#if CONFIG_SOUND_MIXER
#include "mixer.h"
#endif

struct audio_device
{
    struct rt_device *snd;
    struct rt_device *mic;
    struct rt_mempool mp;

    int state;

    void (*evt_handler)(void *parameter, int state);
    void *parameter;
};

static struct audio_device *_audio_device = NULL;

#if !CONFIG_SOUND_MIXER
#define FADE_IN 0
#define FADE_OUT 1
#define NO_FADE 2

#define MAX_FADE_STEP_SFT 3
static unsigned short fade_flag = 0;//0:fade in; 1:fade out; 2:no fade
static unsigned short fade_step = 0;

void set_fade_out_flag(void)
{
	fade_step = 1 << MAX_FADE_STEP_SFT;
	fade_flag = FADE_OUT;
	rt_kprintf("\r\n\r\n===set fade out flag===\r\n\r\n\r\n");
}

void set_fade_in_flag(void)
{
	fade_step = 0;
	fade_flag = FADE_IN;
	rt_kprintf("\r\n\r\n===set fade in flag===\r\n\r\n\r\n");
}

void pcm_fade_handle(void *buffer,int size)
{
	int i,j;
	long tmp;
	int adj_size;
	short *ptr2 = (short*)buffer;

	size = size >> 1;
	adj_size = size >> MAX_FADE_STEP_SFT;

	if(NO_FADE == fade_flag)
	{
		return;
	}
	else 
	{
		if((0 == fade_step)&&(FADE_OUT == fade_flag))
		{
			for(i=0;i<size;i++)
				ptr2[i] = 0;

			rt_kprintf("======fade out fill zero:%d====\r\n",size);
			return;
		}
	}


	for(i=0;i<(1<<MAX_FADE_STEP_SFT);i++)
	{
		if(FADE_IN == fade_flag)
		{
			if(++fade_step >= (1<<MAX_FADE_STEP_SFT))
			{
				fade_flag = NO_FADE;
				return;
			}
		}
		else
		{
			if(fade_step)
				fade_step--;
		}
		for(j=0;j<adj_size;j++)
		{
			tmp = (long)ptr2[i*adj_size+j];
			tmp = tmp *fade_step >>  MAX_FADE_STEP_SFT;
			ptr2[i*adj_size+j] = tmp;
		}
	}
	rt_kprintf("---fade out end:size=%d----\r\n",size);
}

#endif
///
void *audio_device_get_buffer(int *bufsz)
{
    if (bufsz)
    {
        *bufsz = AUDIO_DEVICE_DECODE_MP_BLOCK_SZ * 2;
    }

    return rt_mp_alloc(&(_audio_device->mp), RT_WAITING_FOREVER);
}

void audio_device_put_buffer(void *ptr)
{
    if (ptr) rt_mp_free(ptr);
    return ;
}

static rt_err_t audio_device_write_done(struct rt_device *device, void *ptr)
{
    if (!ptr)
    {
        rt_kprintf("device buf_release NULL\n");
        return -RT_ERROR;
    }

    rt_mp_free(ptr);
    return RT_EOK;
}

#define MUTE_LEN 128

#define USE_L_R_SINGLE_CHANNEL	0//设置1，使用单路输出左右声道的声音

void audio_device_write(void *buffer, int size)
{
    if (_audio_device->snd && size != 0)
    {

#if (USE_L_R_SINGLE_CHANNEL)
		signed short *buf;
		int j, tmp_sample;
		int len = size >> 1;
		buf = (signed short *)buffer;
		for(j=0; j < len; j+=2)
		{
			tmp_sample = (buf[j] + buf[j+1])>>1;
			if(tmp_sample > 32767)
			{
				tmp_sample = 32767;
			}else if(tmp_sample < -32767){
				tmp_sample = -32767;
			}

			buf[j] = (signed short)tmp_sample;
			buf[j+1] = buf[j];
		}
#endif 	

        if (_audio_device->state == AUDIO_DEVICE_IDLE)
        {
            if (_audio_device->evt_handler)
                _audio_device->evt_handler(_audio_device->parameter, AUDIO_DEVICE_PLAYBACK);

            /* change audio device state */
            _audio_device->state = AUDIO_DEVICE_PLAYBACK;
			
		    #if CONFIG_SOUND_MIXER
            //When first open, need mute PA 
//		    audio_dac_eable_mute(1);
			rt_device_control(_audio_device->snd, CODEC_CMD_RESET, NULL);

			if (size > MUTE_LEN)
		    {
			    if(mixer_device_write(buffer,MUTE_LEN)) {
                    rt_device_write(_audio_device->snd, 0, buffer, MUTE_LEN);
			    }
		    }
			else
			{
			    if(mixer_device_write(buffer, size)) {
                    rt_device_write(_audio_device->snd, 0, buffer, size);
		        }
			}

			{
				long cur_tick;
							
				cur_tick = fclk_get_tick();
				while(1)
				{
					if(fclk_get_tick() - cur_tick > 1)
						break;
				}
							
				audio_dac_eable_mute(0);

				if (size > MUTE_LEN)
				{
				    void *tmp_buf = audio_device_get_buffer(NULL);

					rt_memcpy(tmp_buf, buffer + MUTE_LEN, size - MUTE_LEN);
					if(mixer_device_write(tmp_buf, size - MUTE_LEN)) {
						rt_device_write(_audio_device->snd, 0, tmp_buf, size - MUTE_LEN);
					}
				}

			}
			#else
			set_fade_in_flag();
			pcm_fade_handle( buffer,  size);
			rt_device_write(_audio_device->snd, 0, buffer, size);
			#endif

		
        }
		else
		{
		    #if CONFIG_SOUND_MIXER
		    if(mixer_device_write(buffer, size)) {
                rt_device_write(_audio_device->snd, 0, buffer, size);
		    }
		    #else
			
			pcm_fade_handle( buffer,  size);
            rt_device_write(_audio_device->snd, 0, buffer, size);
		    #endif
		}
    }
    else
    {
        /* release buffer directly */
        rt_mp_free(buffer);
    }

    return ;
}

int audio_device_init(void)
{
    uint8_t *mempool_ptr;

    if (!_audio_device)
    {
        _audio_device = (struct audio_device *) rt_malloc(sizeof(struct audio_device) + AUDIO_DEVICE_DECODE_MP_SZ);
        if (_audio_device == NULL)
        {
            rt_kprintf("malloc memeory for _audio_device failed! \n");
            return -RT_ERROR;
        }

        _audio_device->evt_handler = NULL;
        _audio_device->parameter   = NULL;

        mempool_ptr = (uint8_t *)(_audio_device + 1);
        rt_mp_init(&(_audio_device->mp), "adbuf", mempool_ptr, 
							AUDIO_DEVICE_DECODE_MP_SZ, 
							AUDIO_DEVICE_DECODE_MP_BLOCK_SZ * 2);

        /* find snd device */
        _audio_device->snd = rt_device_find("sound");
        if (_audio_device->snd == NULL)
        {
            rt_kprintf("sound device not found \n");
            return -1;
        }

        /* set tx complete call back function */
        rt_device_set_tx_complete(_audio_device->snd, audio_device_write_done);

        _audio_device->mic = rt_device_find("mic");
        if(_audio_device->mic == NULL)
        {
            rt_kprintf("mic device not found \r\n");
            return -1;
        }

        #if CONFIG_SOUND_MIXER
        mixer_init();
        #endif
    }

    return RT_EOK;
}

int audio_device_set_evt_handler(void (*handler)(void *parameter, int state), void *parameter)
{
    if (_audio_device)
    {
        _audio_device->evt_handler = handler;
        _audio_device->parameter   = parameter;
    }

    return 0;
}

void audio_device_set_rate(int sample_rate)
{
    if (_audio_device->snd)
    {
        int rate = sample_rate;

		#if CONFIG_SOUND_MIXER
		audio_dac_eable_mute(1);
		mixer_device_set_rate(sample_rate);
        #else
        rt_device_control(_audio_device->snd, CODEC_CMD_SAMPLERATE, &rate);
		#endif
    }
}

void audio_device_set_volume(int value)
{
    if (_audio_device->snd)
    {
        rt_device_control(_audio_device->snd, CODEC_CMD_SET_VOLUME, &value);
    }
}

int audio_device_get_volume(void)
{
    int value = 0;

    if (_audio_device->snd)
    {
        rt_device_control(_audio_device->snd, CODEC_CMD_GET_VOLUME, &value);
    }

    return value;
}

void audio_device_wait_free(void)
{
    extern void wait_codec_free(void);
    wait_codec_free();
}

void audio_device_open(void)
{
    _audio_device->state = AUDIO_DEVICE_IDLE;

	#if CONFIG_SOUND_MIXER
    mixer_send_msg_audio_src_flow();
    #else
    rt_device_open(_audio_device->snd, RT_DEVICE_OFLAG_WRONLY);
	#endif
    rt_kprintf("audio_device_opened \n");
}

void audio_device_close(void)
{
#if CONFIG_SOUND_MIXER
	mixer_send_msg_audio_src_static();
#else
    rt_device_close(_audio_device->snd);
#endif

    if (_audio_device->state == AUDIO_DEVICE_PLAYBACK)
    {
        if (_audio_device->evt_handler)
        {
            _audio_device->evt_handler(_audio_device->parameter, AUDIO_DEVICE_CLOSE);
        }
    }

    /* set to idle */
    _audio_device->state = AUDIO_DEVICE_CLOSE;

    rt_kprintf("audio_device_closed \n");
}

uint32_t audio_device_is_closed(void)
{
    return (_audio_device->state == AUDIO_DEVICE_CLOSE);
}

void audio_device_mic_open(void)
{
    RT_ASSERT(_audio_device->mic);
    
    rt_device_open(_audio_device->mic, RT_DEVICE_OFLAG_RDONLY);

    rt_kprintf("audio_device_mic_opened \n");
}

void audio_device_mic_set_rate(int sample_rate)
{
    if (_audio_device->mic)
    {
        int rate = sample_rate;

        rt_device_control(_audio_device->mic, CODEC_CMD_SAMPLERATE, &rate);
        
        rt_kprintf("audio_device_mic_set_rate:%d \n", sample_rate);
    }
}

void audio_device_mic_set_channel(int channel)
{
    if (_audio_device->mic)
    {
        int chan = channel;

        rt_device_control(_audio_device->mic, CODEC_CMD_SET_CHANNEL, &chan);

        rt_kprintf("audio_device_mic_set_channel:%d \n", channel);
    }
}
void audio_device_mic_set_volume(int volume)
{
    if (_audio_device->mic)
    {
    	int value = volume;
		
        rt_device_control(_audio_device->mic, CODEC_CMD_SET_VOLUME, &value);
		
        rt_kprintf("audio_device_mic_set_volume:%d \n", volume);
    }
}
int audio_device_mic_read(void *buffer, int size)
{
    int length = 0;
    
    if (_audio_device->mic && (size != 0) && buffer)
    {
        length = rt_device_read(_audio_device->mic, 0, buffer, size);
    }

    return length;
}

void audio_device_mic_close(void)
{
	if(_audio_device->mic)
	{
    	rt_device_close(_audio_device->mic);
	}

    rt_kprintf("audio_device_mic_closed \n");
}

