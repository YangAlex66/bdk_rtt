#include <rtthread.h>

#ifdef PLAY_LIST_TEST
#include "codec_helixmp3.h" 
#include "player_init.h"
#include "player.h"
#include "test_config.h"

#define PLAY_LIST_SIZE  3

struct music_list 
{
    int list_size;
    int cur_index;
    char *url[PLAY_LIST_SIZE];
};

static rt_mq_t play_list_mq;
static struct music_list test_list = 
{  
    PLAY_LIST_SIZE, 
    0, 
    {
        "http://183.193.243.90:9151/mp3/73865964.mp3",
        "http://183.193.243.90:9151/mp3/108479485.mp3",
        "http://183.193.243.90:9151/mp3/84986462.mp3",
    }
};

static void play_music_callback(int event, void *user_data)
{
/*
/////////////////////////////////////////////////////////////////////////////////
NOTE: any function in player.h is forbidden to be called in the callback funtion
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ 
*/
    switch(event)
    {
	    case PLAYER_AUDIO_CLOSED:
	        rt_mq_send(play_list_mq, &event, sizeof(int));
	        rt_kprintf("recv event PLAYER_AUDIO_CLOSED\r\n");
	        break;
	    case PLAYER_PLAYBACK_FAILED:
	        rt_mq_send(play_list_mq, &event, sizeof(int));
	        rt_kprintf("recv event PLAYER_PLAYBACK_FAILED\r\n");
	        break;
	    case PLAYER_PLAYBACK_BREAK:
	        rt_mq_send(play_list_mq, &event, sizeof(int));
	        rt_kprintf("recv event PLAYER_PLAYBACK_BREAK\r\n");
	        break;
	    case PLAYER_PLAYBACK_STOP:
	        rt_kprintf("recv event PLAYER_PLAYBACK_STOP\r\n");
	        break;
	   
		default:
			rt_kprintf("recv event :%d\r\n",event);
			break;
    }
}

static void play_switch(void)
{
    if (test_list.cur_index < test_list.list_size)
    {
        rt_kprintf("play url: %s\r\n", test_list.url[test_list.cur_index]);
        player_set_uri(test_list.url[test_list.cur_index]);
        player_play();
        test_list.cur_index++;
    }
    else
    {
    	rt_kprintf("---list play is finished---\r\n");
        player_stop();
    }
}

static void play_list_entry(void *param)
{
    int event;

    play_switch();
    while(1)
    {
        rt_mq_recv(play_list_mq, &event, sizeof(int), RT_WAITING_FOREVER);
        switch (event)
        {
        case PLAYER_AUDIO_CLOSED:
        case PLAYER_PLAYBACK_FAILED:
        case PLAYER_PLAYBACK_BREAK:
            play_switch();
            break;
            
        default:
            break;
        }
    }
}

static void play_list(int argc, char **argv)
{
    rt_thread_t tid;

    player_set_volume(50);
    player_set_event_callback(play_music_callback, RT_NULL); 

    play_list_mq = rt_mq_create("play_list_mq", sizeof(int), 10, RT_IPC_FLAG_FIFO);
    if(RT_NULL == play_list_mq)
    {
        rt_kprintf("create play_list_mq failed");
        return;
    }

    tid = rt_thread_create("play_list", play_list_entry, RT_NULL, 1024 * 4, 3, 10);
    if (RT_NULL != tid)
    {
        rt_thread_startup(tid);
    }
}
MSH_CMD_EXPORT(play_list, play_list);

#endif
