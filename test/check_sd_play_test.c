#include <rtthread.h>
#include "sys_config.h"
#if defined(CHECK_SD_PLAY_TEST) && CFG_USE_SDCARD_HOST

#include "codec_helixmp3.h" 
#include "player_init.h"
#include "player.h"
#include "test_config.h"

#include <finsh.h>
#include <stdio.h>
#include <stdlib.h>
#include "dfs_posix.h"
#include "player.h"
#include "gpio_pub.h"
#include "rtos_pub.h"
#include "sys_ctrl_pub.h"
#include "saradc_intf.h"
#include "gpio.h"
#define REG_WRITE(addr, _data) 	(*((volatile UINT32 *)(addr)) = (_data))

#define CHECK_SD_PLAY_TEST


enum{
	OFFLINE_PLAYER_PLAY = (0x1 << (PLYAER_STATE_CHANGED+1)),
	OFFLINE_PLAYER_PAUSE = (0x1 << (PLYAER_STATE_CHANGED+2)),
	OFFLINE_PLAYER_STOP = (0x1 << (PLYAER_STATE_CHANGED+3)),
	OFFLINE_PLAYER_SONG_NEXT = (0x1 << (PLYAER_STATE_CHANGED+4)),
	OFFLINE_PLAYER_SONG_PREV = (0x1 << (PLYAER_STATE_CHANGED+5)),
	OFFLINE_PLAYER_SD_INSTER_EVENT = (0x1 << (PLYAER_STATE_CHANGED+6)),
	OFFLINE_PLAYER_SD_REMOVE_EVENT = (0x1 << (PLYAER_STATE_CHANGED+7)),
	OFFLINE_PLAYER_POSITION = (0x1 << (PLYAER_STATE_CHANGED+8)),
	OFFLINE_PLAYER_ALL_EVENT = 0x1FFFF,
};


#define	SD_ROOT "/sd"
/*max song directory*/
#define	MAX_SONG_DIR	64
/*max directory level*/
#define	MAX_DIR_LEVEL	8

#define MAX_LFN_LEN	256

typedef enum {
	GET_PREV,
	GET_NEXT
}PLAY_DIRECTION;


typedef struct _SONG_DIR
{
	char *path;
	unsigned short dir_music_num;
}SONG_DIR;


typedef struct _PLAY_INFO_
{
	SONG_DIR dir_info[MAX_SONG_DIR];
	char cur_file_path[MAX_LFN_LEN];
	unsigned short music_num;
	unsigned short music_index;
	unsigned short dir_num;
	unsigned short dir_index;
	
	unsigned char init_done : 1;
	unsigned char player_status : 1;
	unsigned char player_pause_flag : 1;
	unsigned char vol_step : 5;
	unsigned char vol;
}PLAY_INFO;


PLAY_INFO _sd_player = {0};
static 	struct rt_event offlineplay_evt;

////////////SD check//////////////////
typedef enum{
	OFFLINE_STATUS = 0x00,
	ONLINE_STATUS,
}CHECK_STATUS_E;


#define SOC_BK7251                               0
#define SOC_BK7253                               1
#define SOC_MODEL                               SOC_BK7253

#if(SOC_MODEL==SOC_BK7253)
	#define SD_STATUS_CHECK_PIN				23 
#else
	#define SD_STATUS_CHECK_PIN				12 
#endif


#define SD_DEBOUNCE_COUNT 			    10
#define SD_INSTER_STATUS_CHECK_PIN_LEVEL 0
#define SD_CHECK_SCAN_INTERVAL_MS		 20
CHECK_STATUS_E sd_status = OFFLINE_STATUS;
static beken_timer_t sd_check_handle_timer = {0};


/*
idx: song index in current dir ,start from 0
path: current dir full path name
fullname:the searched song's full path name(path name + filename)
*/
static int  read_file_in_dir(unsigned int idx,char *path,char * fullname)
{
	struct dirent  *ent  = NULL;
	DIR *pDir = NULL;
	unsigned short cur_idx = 0;
	int ret = -1;
	
	pDir = opendir(path);
	if(NULL != pDir)
	{
		while(1)
		{
			ent = readdir(pDir);
			if(NULL == ent)
				break;
			if(ent->d_type & DT_REG)
			{
				if( (0 == strncasecmp(strchr(ent->d_name,'.'),".mp3",4)) ||
				    (0 == strncasecmp(strchr(ent->d_name,'.'),".wav",4)) )
				{
					if( cur_idx == idx )
					{
						ret = 0;
						snprintf(fullname, MAX_LFN_LEN,"%s%s%s",path,"/",ent->d_name);						
						break;
					}
					cur_idx ++;
				}
			}
		}
		closedir(pDir);
	}
	return ret;
}

static int get_song(PLAY_DIRECTION prev_next)
{
	int i = 0;
	int ret = -1;
	unsigned short total = 0;
	if(_sd_player.music_num == 0)
		return ret;
	
	if(GET_PREV == prev_next)
	{//prev
		if(0 == _sd_player.music_index)
			_sd_player.music_index = _sd_player.music_num - 1;
		else
			_sd_player.music_index --;
	}
	else
	{//next
		if(_sd_player.music_index >= _sd_player.music_num - 1)
			_sd_player.music_index = 0;
		else
			_sd_player.music_index ++;
	}

	if(_sd_player.music_index != 0)
	{
		for(i=0;i<sizeof(_sd_player.dir_info)/sizeof(SONG_DIR);i++)
		{
			total += _sd_player.dir_info[i].dir_music_num;
			if(_sd_player.music_index <= total-1)
				break;		
		}
		total -= _sd_player.dir_info[i].dir_music_num;
	}
	memset(_sd_player.cur_file_path,0,sizeof(_sd_player.cur_file_path));
	ret = read_file_in_dir(_sd_player.music_index-total,_sd_player.dir_info[i].path,_sd_player.cur_file_path);
	printf("@@@@@ full name:%s @@@@@\r\n",_sd_player.cur_file_path);
	
	return ret;
}

/*
for directory play mode
find directory and open the first mp3 file 
*/
static int get_dir(PLAY_DIRECTION prev_next)
{
	int i = 0;
	int ret = -1;
	
	if(_sd_player.dir_num == 0)
		return ret;

	if(GET_PREV == prev_next)
	{
		if(0 == _sd_player.dir_index)
			_sd_player.dir_index = _sd_player.dir_num - 1;
		else
			_sd_player.dir_index --;
	}
	else
	{
		if(_sd_player.dir_index >= _sd_player.dir_num -1)
			_sd_player.dir_index = 0;
		else
			_sd_player.dir_index ++;
	}
	_sd_player.music_index = 0;
	
	for(i=0;i<_sd_player.dir_index;i++)
		_sd_player.music_index +=  _sd_player.dir_info[_sd_player.dir_index].dir_music_num;

	memset(_sd_player.cur_file_path,0,sizeof(_sd_player.cur_file_path));
	if(_sd_player.music_index != 0)
	{
		_sd_player.music_index -= 1;
		ret = read_file_in_dir(0,_sd_player.dir_info[_sd_player.dir_index].path,_sd_player.cur_file_path);
	}
	return ret;
}

/*
full disk scan: get mp3 total number and DIRs' pathname which have .mp3 files
*/
static void scan_files(char *path,rt_uint8_t recu_level)
{
    struct dirent  *ent  = NULL;
	DIR *pDir = NULL;
	short filecnt = 0;
	rt_uint8_t tmp;

	pDir = opendir(path);
	if(NULL != pDir)
	{
		tmp = strlen(path);
		while(1)
		{
			ent = readdir(pDir);
			if(NULL == ent)
				break;

			if( (0 == strcmp(ent->d_name,".")) || (0 == strcmp(ent->d_name,"..")) )
                continue;

			if(ent->d_type & DT_DIR)
			{
				if(recu_level < MAX_DIR_LEVEL)
				{
					snprintf(&path[tmp],strlen(ent->d_name)+1+1,"/%s",ent->d_name);
					recu_level++;
					scan_files(path,recu_level);
					path[tmp] = 0;
				}
				else
					break;
			}
			else
			{
				if((0 == strncasecmp(strchr(ent->d_name,'.'),".mp3",4))||
				   (0 == strncasecmp(strchr(ent->d_name,'.'),".wav",4)))
				{
					filecnt ++;
				}
			}
		}
		if(filecnt > 0)
		{
			if(_sd_player.dir_num < MAX_SONG_DIR -1)
			{
				_sd_player.dir_info[_sd_player.dir_num].path = rt_malloc(strlen(path)+1);
				memset(_sd_player.dir_info[_sd_player.dir_num].path,0,strlen(path)+1);
				snprintf(_sd_player.dir_info[_sd_player.dir_num].path,strlen(path)+1,"%s",path);
			
				_sd_player.dir_info[_sd_player.dir_num].dir_music_num = filecnt;
				_sd_player.music_num += filecnt;
				_sd_player.dir_num++;
			} 
		}
		closedir(pDir);
	}
}

static int scan_sd(void)
{
	int i;
	char *path = sdram_calloc(1,256);

	if(NULL == path)
	{
		rt_kprintf("scan malloc error\r\n");
		return -1;
	}
	snprintf(path,strlen(SD_ROOT)+1,"%s",SD_ROOT);
	scan_files(path,0);
	for(i=0;i<_sd_player.dir_num;i++)
	{
		rt_kprintf("====path[%d]:%s===\r\n",i,_sd_player.dir_info[i].path);
	}
	sdram_free(path);
	if(_sd_player.dir_num > 0)
		return 0;
	else
		return -1;
}
static int mount_sd(void)
{
	int ret;
	/* mount sd card fat partition 1 as root directory */
	ret = dfs_mount("sd0", SD_ROOT, "elm", 0, 0);
	if(ret == 0)
		rt_kprintf("SD Card initialized!\n");
	else
		rt_kprintf("SD Card initialzation failed!\n");
	return ret;
}

static void unmount_sd(void)
{
	int ret,i;
	ret = dfs_unmount(SD_ROOT);
	rt_kprintf("unmount sd :ret =%d\r\n",ret);
	for(i=0;i<_sd_player.dir_num;i++)
	{
		if(_sd_player.dir_info[i].path != NULL)
		{
			rt_free(_sd_player.dir_info[i].path);
			_sd_player.dir_info[i].path = NULL;
			_sd_player.dir_info[i].dir_music_num = 0;
		}
	}
	_sd_player.music_num = 0;
	_sd_player.music_index = 0;
	_sd_player.dir_num = 0;
	_sd_player.dir_index = 0;
}


static int offline_player_send_event(int event)
{	
	int ret;
	if(_sd_player.init_done)
	{
		ret=rt_event_send(&offlineplay_evt,event);
		if(ret==RT_EOK)
			rt_kprintf("rt_event_send OK\r\n");	
	}
	return ret;
}	

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
	       	offline_player_send_event(1 << PLAYER_AUDIO_CLOSED);
	        break;
	    case PLAYER_PLAYBACK_FAILED:
	        offline_player_send_event(1 << PLAYER_PLAYBACK_FAILED);
	        break;
	    case PLAYER_PLAYBACK_BREAK:
	      	offline_player_send_event(1 << PLAYER_PLAYBACK_BREAK);
	        break;   
		default:
			rt_kprintf("recv event :%d\r\n",event);
			break;
    }
}

static void sd_play_song(PLAY_DIRECTION prev_next)
{
	player_stop();
	get_song(prev_next);
	player_set_uri(_sd_player.cur_file_path);
	player_play();
}
static void sd_play_thread(void *param)
{	
	uint32_t recv_evt;
	int ret;
    rt_err_t result;
    int event;
	rt_thread_delay(500);
    while(1)
    {
		ret = rt_event_recv(&offlineplay_evt,OFFLINE_PLAYER_ALL_EVENT,
								RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER, &recv_evt);

		if(ret == RT_EOK)
		{
			rt_kprintf("recv_evt:%x\n",recv_evt);
			
			if(recv_evt & OFFLINE_PLAYER_SD_INSTER_EVENT)
			{
				
				if(0 == mount_sd())
				{
					rt_kprintf("---sd mount ok---\r\n");
					if(0 == scan_sd())
					{
					//first song
						_sd_player.music_index = _sd_player.music_num;
						sd_play_song(GET_NEXT);
					}
					else
					{
						rt_kprintf("---sd no song---\r\n");
					}
						
				}
				else
					rt_kprintf("---sd mount fail---\r\n");
			}
			if( recv_evt & OFFLINE_PLAYER_SD_REMOVE_EVENT )
			{
				rt_kprintf("---sd unmount---\r\n");
				player_stop();
				unmount_sd();
			}
			if( recv_evt &( 1 << PLAYER_AUDIO_CLOSED) )
			{ 
				sd_play_song(GET_NEXT);
				
			}
			if(recv_evt & OFFLINE_PLAYER_PLAY)
			{
				player_play();
			}
			if(recv_evt & OFFLINE_PLAYER_PAUSE)
			{
				player_pause();
			}
			if(recv_evt & OFFLINE_PLAYER_STOP)
			{
				player_stop();
			}
			if(recv_evt & OFFLINE_PLAYER_SONG_NEXT)
			{
				sd_play_song(GET_NEXT);
			}
			if(recv_evt & OFFLINE_PLAYER_SONG_PREV)
			{
				sd_play_song(GET_PREV);
			}
			if(recv_evt & OFFLINE_PLAYER_POSITION)
			{
				rt_kprintf("total time:%d s,cur pos:%d ms\r\n",player_get_duration(),player_get_position());	
			}
		}
		else
			rt_kprintf("recv_evt failed\r\n");
    }
}

static uint8_t get_sd_check_pin_status(void)
{
	return gpio_input(SD_STATUS_CHECK_PIN);
}

static void sd_check_handle_timer_callback( void * arg )  
{
	static uint16 cnt_online = 0;	
	if(get_sd_check_pin_status() == SD_INSTER_STATUS_CHECK_PIN_LEVEL)
	{				
		if(OFFLINE_STATUS == sd_status)
		{ 
			rt_kprintf("sd_status=%d\r\n",sd_status);
			if (cnt_online < SD_DEBOUNCE_COUNT)
	        {
	            cnt_online ++;			
	        }		 
			else
			{
				sd_status = ONLINE_STATUS;
				rt_kprintf(" sd_status = ONLINE_STATUS\r\n");
				offline_player_send_event(OFFLINE_PLAYER_SD_INSTER_EVENT);
	 		}
		}
	}
	else 
	{
		if(ONLINE_STATUS == sd_status)
		{ 
			cnt_online = 0;
			sd_status = OFFLINE_STATUS;
			rt_kprintf(" sd_status = OFFLINE_STATUS\r\n");
			offline_player_send_event(OFFLINE_PLAYER_SD_REMOVE_EVENT);		
		}
	}
}

static int sd_is_online(void)
{
	return (sd_status == ONLINE_STATUS)?1:0;
}

static void sd_check_init(void)
{
	int  err;	
	gpio_config(SD_STATUS_CHECK_PIN, GMODE_INPUT_PULLUP);
	err = rtos_init_timer(&sd_check_handle_timer, 
							SD_CHECK_SCAN_INTERVAL_MS, 
							sd_check_handle_timer_callback, 
							NULL);
	err = rtos_start_timer(&sd_check_handle_timer);
}

static void sd_play_test(int argc, char **argv)
{
    rt_thread_t tid;
	int event;
	
	if(argc == 2)
	{
		if(0 == strcmp(argv[1],"start"))
		{
			if(_sd_player.init_done)
				return;
			saradc_config_vddram_voltage(PSRAM_VDD_3_3V);
			sd_check_init();
			memset(&_sd_player,0,sizeof(_sd_player));
		    player_set_volume(50);
		    player_set_event_callback(play_music_callback, RT_NULL); 
			rt_event_init(&offlineplay_evt, "evt", RT_IPC_FLAG_FIFO);

		    tid = rt_thread_create("sd_play", sd_play_thread, RT_NULL, 1024 * 4, 10, 10);
		    if (RT_NULL != tid)
		    {
		        rt_thread_startup(tid);
				_sd_player.init_done= 1;
		    }
		}
		else
		{
			if(_sd_player.init_done)
			{
				if(0 == strcmp(argv[1],"next"))
				{
					event =OFFLINE_PLAYER_SONG_NEXT;
				}
				else if(0 == strcmp(argv[1],"prev"))
				{
					event =OFFLINE_PLAYER_SONG_PREV;
				}
				else if(0 == strcmp(argv[1],"play"))
				{
					event =OFFLINE_PLAYER_PLAY;
				}
				else if(0 == strcmp(argv[1],"pause"))
				{
					event = OFFLINE_PLAYER_PAUSE;
				}
				else if(0 == strcmp(argv[1],"stop"))
				{
					event = OFFLINE_PLAYER_STOP;
				}
				else if(0 == strcmp(argv[1],"position"))
				{
					event = OFFLINE_PLAYER_POSITION;
				}
				else
					event = OFFLINE_PLAYER_STOP;
				
				offline_player_send_event(event);
			}
		}
	}
}

#define SD_TEST_SIZE   	8*512
#define COUNTER  		5
#define RW_BUF_SIZE		COUNTER*SD_TEST_SIZE

int Blcok;
void sd_test_write(int argc, char *argv[])
#if 0
{
	unsigned char *pbuf;
	unsigned char *read_buf;
	unsigned char path_name[]="sd/test2.txt";
	long   fds; 
	int i,j,ret;
	int mode;
	if(argc != 2)
		return;
	
	saradc_config_vddram_voltage(PSRAM_VDD_3_3V);
	pbuf = rt_malloc(SD_TEST_SIZE);
	if(pbuf == NULL)
		return;
	read_buf = rt_malloc(SD_TEST_SIZE);
	if(read_buf == NULL)
		return;
	
	for(i=0;i<SD_TEST_SIZE;)
	{
		pbuf[i++] = 0x55;
		pbuf[i++] = 0x56;
		pbuf[i++] = 0x57;
		pbuf[i++] = 0x58;
		pbuf[i++] = 0x59;
		pbuf[i++] = 0x60;
		pbuf[i++] = 0x61;
		pbuf[i++] = 0x62;
	}

	mode = atoi(argv[1]);
	
#if 1
		if(mount_sd() == 0)
		{
			rt_kprintf("---sd mount ok---\r\n");
		}
		else
		{
			rt_kprintf("!!!! sd mount failed\r\n");
		}
#endif	

	rt_kprintf("start time:%d\r\n",rt_tick_get());
	if(mode == 0)
	{
		fds = open(path_name,O_RDWR|O_CREAT|O_BINARY|O_TRUNC ,0600);
		
		if(fds >= 0)
		{
			rt_kprintf("open ok\r\n");
			if(SD_TEST_SIZE == write(fds,pbuf,SD_TEST_SIZE))
			{
				Delay(1000);				
				rt_kprintf("write ok\r\n");
			}
			else
			{	
				rt_kprintf("------write failed\r\n");
			}		
		}
		else
		{
			rt_kprintf("!!open failed!!\r\n");
		}
	 
	 }
	
	else
	{
	#if 0
		if(mount_sd() == 0)
		{
			rt_kprintf("---sd mount ok---\r\n");
		}
		else
		{
			rt_kprintf("!!!! sd mount failed\r\n");
		}
	#endif	

		ret = read(fds,read_buf,SD_TEST_SIZE);	
		if(SD_TEST_SIZE == ret)
		{					
			rt_kprintf("read ok\r\n");
		}
		else
		{	
		
			rt_kprintf("ret=0x%x\r\n",ret);
			rt_kprintf("------read failed\r\n");
		}
#if 1					
		if(memcmp(pbuf, read_buf, SD_TEST_SIZE) == 0)
		{
			rt_kprintf("compare ok \r\n");
		}
		else
		{
			rt_kprintf("!!!compare failed \r\n");
		}
#endif			
		close(fds);
	}
	rt_kprintf("end time:%d\r\n",rt_tick_get());
	dfs_unmount("/sd");
	rt_free(pbuf);
}
#else
{
	unsigned char *pbuf;
	UINT8 *read_buf;
	UINT8 *write_buf;
	unsigned char path_name[]="sd/test1.txt";
	long   fds; 
	int i,j,ret,count=0;
	int mode;
	uint8 m = 0;
	if(argc != 2)
		return;
	
	saradc_config_vddram_voltage(PSRAM_VDD_3_3V);

	read_buf = rt_malloc(RW_BUF_SIZE);
	if(read_buf == NULL)
		return;
	
	write_buf = rt_malloc(RW_BUF_SIZE);
	if(write_buf == NULL)
		return;	
	
	pbuf = rt_malloc(SD_TEST_SIZE);
	if(pbuf == NULL)
		return;	

#if 0	
	for(i=0;i<4*SD_TEST_SIZE;)
	{
		write_buf[i++] = 0x55;
		write_buf[i++] = 0x56;
		write_buf[i++] = 0x57;
		write_buf[i++] = 0x58;
		write_buf[i++] = 0x59;
		write_buf[i++] = 0x60;
		write_buf[i++] = 0x61;
		write_buf[i++] = 0x62;
	}
	
	for(i=0;i<SD_TEST_SIZE;)
	{
		pbuf[i++] = 0x55;
		pbuf[i++] = 0x56;
		pbuf[i++] = 0x57;
		pbuf[i++] = 0x58;
		pbuf[i++] = 0x59;
		pbuf[i++] = 0x60;
		pbuf[i++] = 0x61;
		pbuf[i++] = 0x62;
	}
#endif
	mode = atoi(argv[1]);	
	rt_kprintf("start time:%d\r\n",rt_tick_get());

	if(mode == 0)
	{		
		for(i=0;i<SD_TEST_SIZE;)
		{
			pbuf[i++] = 0x55;
			pbuf[i++] = 0x56;
			pbuf[i++] = 0x57;
			pbuf[i++] = 0x58;
			pbuf[i++] = 0x59;
			pbuf[i++] = 0x60;
			pbuf[i++] = 0x61;
			pbuf[i++] = 0x62;
		}

		for(i=0;i<RW_BUF_SIZE;)
		{
			write_buf[i++] = 0x55;
			write_buf[i++] = 0x56;
			write_buf[i++] = 0x57;
			write_buf[i++] = 0x58;
			write_buf[i++] = 0x59;
			write_buf[i++] = 0x60;
			write_buf[i++] = 0x61;
			write_buf[i++] = 0x62;
		}
			
	#if 1
		if(mount_sd() == 0)
		{
			rt_kprintf("---sd mount ok---\r\n");
		}
		else
		{
			rt_kprintf("!!!! sd mount failed\r\n");
		}
	#endif
	
		fds = open(path_name,O_RDWR|O_CREAT|O_BINARY|O_TRUNC ,0600);
		
		if(fds >= 0)
		{
			rt_kprintf("open ok\r\n");
			for(j=0; j<2500; j++)
			{	
				ret = write(fds,pbuf,SD_TEST_SIZE);

				if(ret == 0)
				{	
					rt_kprintf("------write failed\r\n");
				}
				count ++;
				#if 0
				if(((count % COUNTER )== 0) && (count>=COUNTER))
				{

					if(0 != sdcard_read_multi_block(read_buf,Blcok,8*COUNTER))
						rt_kprintf("read error!!!\r\n");					

					if(memcmp(read_buf, write_buf, 4*SD_TEST_SIZE) == 0)
					{
						rt_kprintf("compare ok:block:%x\r\n",Blcok);
					}
					else
					{
						rt_kprintf("compare failed:block:%x\r\n",Blcok);
						for(i=0; i<4*SD_TEST_SIZE; i++)
						{
							if(write_buf[i] != read_buf[i])
							rt_kprintf("write_buf[%d]=0x%x,read_buf[%d]=0x%x\r\n",i,write_buf[i],i,read_buf[i]);
						}
						return;
					}
					
					#if 0
					if(Blcok == 0x43b0)
					{
					
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);	
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
					}
					
					if(Blcok == 0x43d0)
					{
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
					}
					#endif
					
					Blcok += 8*COUNTER;				
				}
				#endif
				
				#if 0
				if(((count % COUNTER )== 0) && (count>=COUNTER))
				{
					for(i = 0; i < (8*COUNTER); i++)
					{
						if(0 != sdcard_read_multi_block(read_buf + i*512, Blcok, 1))
							rt_kprintf("read error!!!\r\n");
						
						Blcok++;
					}
				
					if(memcmp(read_buf, write_buf, RW_BUF_SIZE) == 0)
					{
						rt_kprintf("compare ok:block:%x\r\n",Blcok-8*COUNTER);
					}
					else
					{
						rt_kprintf("compare failed:block:%x\r\n",Blcok-8*COUNTER);
						for(i=0; i<RW_BUF_SIZE; i++)
						{
							if(write_buf[i] != read_buf[i])
							rt_kprintf("write_buf[%d]=0x%x,read_buf[%d]=0x%x\r\n",i,write_buf[i],i,read_buf[i]);
						}
						return;
					}
			
				}
				#endif
				
			}
			
			close(fds);
		}
		else
		{
			rt_kprintf("!!open failed!!\r\n");
		}
	 
	 }
	
	else if(mode == 1)
	{		
		for(i=0;i<SD_TEST_SIZE;)
		{
			pbuf[i] = 0x00 + m;
			pbuf[i+1] =  0x00 + m;
			m++;
			
			i=i+2;	
		}

		for(i=0;i<RW_BUF_SIZE;)
		{
			write_buf[i] = 0x00 + m;
			write_buf[i+1] =  0x00 + m;
			m++;
			
			i=i+2;	
		}

		for(i=0;i<SD_TEST_SIZE;i++)
		{
			//rt_kprintf("pbuf[%d]=0x%x,write_buf[%d]=0x%x\r\n",i,pbuf[i],i,write_buf[i]);
		}
			
	#if 1
		if(mount_sd() == 0)
		{
			rt_kprintf("---sd mount ok---\r\n");
		}
		else
		{
			rt_kprintf("!!!! sd mount failed\r\n");
		}
	#endif
		fds = open(path_name,O_RDWR|O_CREAT|O_BINARY|O_TRUNC ,0600);
		
		if(fds >= 0)
		{
			rt_kprintf("open ok\r\n");
			for(j=0; j<2500; j++)
			{	
				ret = write(fds,pbuf,SD_TEST_SIZE);

				if(ret == 0)
				{	
					rt_kprintf("------write failed\r\n");
				}
				count ++;
				#if 0
				if(((count % 4 )== 0) && (count>=4))
				{

					if(0 != sdcard_read_multi_block(read_buf,Blcok,8*4))
						rt_kprintf("read error!!!\r\n");					

					if(memcmp(read_buf, write_buf, 4*SD_TEST_SIZE) == 0)
					{
						rt_kprintf("compare ok:block:%x\r\n",Blcok);
					}
					else
					{
						rt_kprintf("failed:block:%x\r\n",Blcok);
						for(i=0; i<4*SD_TEST_SIZE; i++)
						{
							if(write_buf[i] != read_buf[i])
							rt_kprintf("write_buf[%d]=0x%x,read_buf[%d]=0x%x\r\n",i,write_buf[i],i,read_buf[i]);
						}
						return;
					}

					if(Blcok == 0x43b0)
					{
					
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
			
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);

					}
					
					if(Blcok == 0x43d0){
					
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
					}
					Blcok += 4*8;				
				}
				#endif
				
				#if 1
				if(((count % COUNTER )== 0) && (count>=COUNTER))
				{
					for(i = 0; i < (8*COUNTER); i++)
					{
						if(0 != sdcard_read_multi_block(read_buf + i*512, Blcok, 1))
							rt_kprintf("read error!!!\r\n");
						
						Blcok++;
					}
				
					if(memcmp(read_buf, write_buf, RW_BUF_SIZE) == 0)
					{
						rt_kprintf("compare ok:block:%x\r\n",Blcok-8*COUNTER);
					}
					else
					{
						rt_kprintf("failed:block:%x\r\n",Blcok-8*COUNTER);
						for(i=0; i<RW_BUF_SIZE; i++)
						{
							if(write_buf[i] != read_buf[i])
							rt_kprintf("write_buf[%d]=0x%x,read_buf[%d]=0x%x\r\n",i,write_buf[i],i,read_buf[i]);
						}
						return;
					}
			
				}
				#endif
				
			}
			
			close(fds);
		}
		else
		{
			rt_kprintf("!!open failed!!\r\n");
		}
	 
	 }
	else if (mode == 2)
	{
		m = 0xFF;
		for(i=0;i<SD_TEST_SIZE;)
		{
			pbuf[i] = 0x00 + m;
			pbuf[i+1] =  0x00 + m;
			m--;
			
			i=i+2;	
		}
		
		m =0xFF;
		for(i=0;i<RW_BUF_SIZE;)
		{
			write_buf[i] = 0x00 + m;
			write_buf[i+1] =  0x00 + m;
			m--;
			
			i=i+2;	
		}

		for(i=0;i<SD_TEST_SIZE;i++)
		{
			//rt_kprintf("pbuf[%d]=0x%x,write_buf[%d]=0x%x\r\n",i,pbuf[i],i,write_buf[i]);
		}
		
		REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
		REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
		REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
		REG_WRITE(REG_GPIO_2_CONFIG, 0x00);

		
	#if 1
		if(mount_sd() == 0)
		{
			rt_kprintf("---sd mount ok---\r\n");
		}
		else
		{
			rt_kprintf("!!!! sd mount failed\r\n");
		}
	#endif
		fds = open(path_name,O_RDWR|O_CREAT|O_BINARY|O_TRUNC ,0600);
		
		if(fds >= 0)
		{
			rt_kprintf("open ok\r\n");
			for(j=0; j<2500; j++)
			{	
				ret = write(fds,pbuf,SD_TEST_SIZE);

				if(ret == 0)
				{	
					rt_kprintf("------write failed\r\n");
				}
				count ++;
				#if 0
				if(((count % 4 )== 0) && (count>=4))
				{

					if(0 != sdcard_read_multi_block(read_buf,Blcok,8*4))
						rt_kprintf("read error!!!\r\n");					

					if(memcmp(read_buf, write_buf, 4*SD_TEST_SIZE) == 0)
					{
						rt_kprintf("compare ok:block:%x\r\n",Blcok);
					}
					else
					{
						rt_kprintf("failed:block:%x\r\n",Blcok);
						for(i=0; i<4*SD_TEST_SIZE; i++)
						{
							if(write_buf[i] != read_buf[i])
							rt_kprintf("write_buf[%d]=0x%x,read_buf[%d]=0x%x\r\n",i,write_buf[i],i,read_buf[i]);
						}
						return;
					}

					if(Blcok == 0x43b0)
					{
					
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
			
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);

					}
					
					if(Blcok == 0x43d0){
					
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
					}
					Blcok += 4*8;				
				}
				#endif
				
				#if 1
				if(((count % COUNTER )== 0) && (count>=COUNTER))
				{
					for(i = 0; i < (8*COUNTER); i++)
					{
						if(0 != sdcard_read_multi_block(read_buf + i*512, Blcok, 1))
							rt_kprintf("read error!!!\r\n");
						
						
						Blcok++;
					}
				
					if(memcmp(read_buf, write_buf, RW_BUF_SIZE) == 0)
					{
						rt_kprintf("compare ok:block:%x\r\n", Blcok-8*COUNTER);
					}
					else
					{
						rt_kprintf("compare failed:block:%x\r\n",Blcok-8*COUNTER);
						for(i=0; i<RW_BUF_SIZE; i++)
						{
							if(write_buf[i] != read_buf[i])
							rt_kprintf("write_buf[%d]=0x%x,read_buf[%d]=0x%x\r\n",i,write_buf[i],i,read_buf[i]);
						}
						return;
					}
			
				}
				#endif
				
			}
			
			close(fds);
		}
		else
		{
			rt_kprintf("!!open failed!!\r\n");
		}
	 
	 }
	else if(mode == 3)
	{
		rt_kprintf("mode 3\r\n");
		for(i=0;i<SD_TEST_SIZE;i++)
		{
			pbuf[i] = 0x01;	
			
		}
		
		for(i=0;i<RW_BUF_SIZE;i++)
		{
			write_buf[i] = 0x01;
		}

		for(i=0;i<SD_TEST_SIZE;i++)
		{
			//rt_kprintf("pbuf[%d]=0x%x,write_buf[%d]=0x%x\r\n",i,pbuf[i],i,write_buf[i]);
		}
		
		REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
		REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
		REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
		REG_WRITE(REG_GPIO_2_CONFIG, 0x00);

		
	#if 1
		if(mount_sd() == 0)
		{
			rt_kprintf("---sd mount ok---\r\n");
		}
		else
		{
			rt_kprintf("!!!! sd mount failed\r\n");
		}
	#endif
		fds = open(path_name,O_RDWR|O_CREAT|O_BINARY|O_TRUNC ,0600);
		
		if(fds >= 0)
		{
			rt_kprintf("open ok\r\n");
			for(j=0; j<2500; j++)
			{	
				ret = write(fds,pbuf,SD_TEST_SIZE);

				if(ret == 0)
				{	
					rt_kprintf("------write failed\r\n");
				}
				count ++;
				#if 0
				if(((count % 4 )== 0) && (count>=4))
				{

					if(0 != sdcard_read_multi_block(read_buf,Blcok,8*4))
						rt_kprintf("read error!!!\r\n");					

					if(memcmp(read_buf, write_buf, 4*SD_TEST_SIZE) == 0)
					{
						rt_kprintf("compare ok:block:%x\r\n",Blcok);
					}
					else
					{
						rt_kprintf("failed:block:%x\r\n",Blcok);
						for(i=0; i<4*SD_TEST_SIZE; i++)
						{
							if(write_buf[i] != read_buf[i])
							rt_kprintf("write_buf[%d]=0x%x,read_buf[%d]=0x%x\r\n",i,write_buf[i],i,read_buf[i]);
						}
						return;
					}

					Blcok += 4*8;				
				}
				#endif
				
				#if 1
				if(((count % COUNTER )== 0) && (count>=COUNTER))
				{
					for(i = 0; i < (8*COUNTER); i++)
					{
						if(0 != sdcard_read_multi_block(read_buf + i*512, Blcok, 1))
							rt_kprintf("read error!!!\r\n");
						
						
						Blcok++;
					}
				
					if(memcmp(read_buf, write_buf, RW_BUF_SIZE) == 0)
					{
						rt_kprintf("compare ok:block:%x\r\n",Blcok-8*COUNTER);
					}
					else
					{
						rt_kprintf("compare failed:block:%x\r\n",Blcok-8*COUNTER);
						for(i=0; i<RW_BUF_SIZE; i++)
						{
							if(write_buf[i] != read_buf[i])
							rt_kprintf("write_buf[%d]=0x%x,read_buf[%d]=0x%x\r\n",i,write_buf[i],i,read_buf[i]);
						}
						return;
					}
			
				}
				#endif
				
			}
			
			close(fds);
		}
		else
		{
			rt_kprintf("!!open failed!!\r\n");
		}
	 
	 }
	else if(mode == 4)
	{
		for(i=0;i<SD_TEST_SIZE;i++)
		{
			pbuf[i] = 0x02;	
		}
		
		for(i=0;i<RW_BUF_SIZE;i++)
		{
			write_buf[i] = 0x02;
		}

		for(i=0;i<SD_TEST_SIZE;i++)
		{
			//rt_kprintf("pbuf[%d]=0x%x,write_buf[%d]=0x%x\r\n",i,pbuf[i],i,write_buf[i]);
		}
			
	#if 1
		if(mount_sd() == 0)
		{
			rt_kprintf("---sd mount ok---\r\n");
		}
		else
		{
			rt_kprintf("!!!! sd mount failed\r\n");
		}
	#endif
		fds = open(path_name,O_RDWR|O_CREAT|O_BINARY|O_TRUNC ,0600);
		
		if(fds >= 0)
		{
			rt_kprintf("open ok\r\n");
			for(j=0; j<2500; j++)
			{	
				ret = write(fds,pbuf,SD_TEST_SIZE);

				if(ret == 0)
				{	
					rt_kprintf("------write failed\r\n");
				}
				count ++;
				#if 0
				if(((count % 4 )== 0) && (count>=4))
				{

					if(0 != sdcard_read_multi_block(read_buf,Blcok,8*4))
						rt_kprintf("read error!!!\r\n");					

					if(memcmp(read_buf, write_buf, 4*SD_TEST_SIZE) == 0)
					{
						rt_kprintf("compare ok:block:%x\r\n",Blcok);
					}
					else
					{
						rt_kprintf("failed:block:%x\r\n",Blcok);
						for(i=0; i<4*SD_TEST_SIZE; i++)
						{
							if(write_buf[i] != read_buf[i])
							rt_kprintf("write_buf[%d]=0x%x,read_buf[%d]=0x%x\r\n",i,write_buf[i],i,read_buf[i]);
						}
						return;
					}

					if(Blcok == 0x43b0)
					{
					
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
			
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);

					}
					
					if(Blcok == 0x43d0){
					
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
					}
					Blcok += 4*8;				
				}
				#endif
				
				#if 1
				if(((count % COUNTER )== 0) && (count>=COUNTER))
				{
					for(i = 0; i < (8*COUNTER); i++)
					{
						if(0 != sdcard_read_multi_block(read_buf + i*512, Blcok, 1))
							rt_kprintf("read error!!!\r\n");
						
						
						Blcok++;
					}
				
					if(memcmp(read_buf, write_buf, RW_BUF_SIZE) == 0)
					{
						rt_kprintf("compare ok:block:%x\r\n",Blcok-8*COUNTER);
					}
					else
					{
						rt_kprintf("compare failed:block:%x\r\n",Blcok-8*COUNTER);
						for(i=0; i<RW_BUF_SIZE; i++)
						{
							if(write_buf[i] != read_buf[i])
							rt_kprintf("write_buf[%d]=0x%x,read_buf[%d]=0x%x\r\n",i,write_buf[i],i,read_buf[i]);
						}
						return;
					}
			
				}
				#endif
				
			}
			
			close(fds);
		}
		else
		{
			rt_kprintf("!!open failed!!\r\n");
		}
	 
	 }
	else if(mode == 5)
	{
		for(i=0;i<SD_TEST_SIZE;i++)
		{
			pbuf[i] = 0x03;	
		}
		
		for(i=0;i<RW_BUF_SIZE;i++)
		{
			write_buf[i] = 0x03;
		}

		for(i=0;i<SD_TEST_SIZE;i++)
		{
			//rt_kprintf("pbuf[%d]=0x%x,write_buf[%d]=0x%x\r\n",i,pbuf[i],i,write_buf[i]);
		}
		
		REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
		REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
		REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
		REG_WRITE(REG_GPIO_2_CONFIG, 0x00);

		
	#if 1
		if(mount_sd() == 0)
		{
			rt_kprintf("---sd mount ok---\r\n");
		}
		else
		{
			rt_kprintf("!!!! sd mount failed\r\n");
		}
	#endif
		fds = open(path_name,O_RDWR|O_CREAT|O_BINARY|O_TRUNC ,0600);
		
		if(fds >= 0)
		{
			rt_kprintf("open ok\r\n");
			for(j=0; j<2500; j++)
			{	
				ret = write(fds,pbuf,SD_TEST_SIZE);

				if(ret == 0)
				{	
					rt_kprintf("------write failed\r\n");
				}
				count ++;
				#if 0
				if(((count % 4 )== 0) && (count>=4))
				{

					if(0 != sdcard_read_multi_block(read_buf,Blcok,8*4))
						rt_kprintf("read error!!!\r\n");					

					if(memcmp(read_buf, write_buf, 4*SD_TEST_SIZE) == 0)
					{
						rt_kprintf("compare ok:block:%x\r\n",Blcok);
					}
					else
					{
						rt_kprintf("failed:block:%x\r\n",Blcok);
						for(i=0; i<4*SD_TEST_SIZE; i++)
						{
							if(write_buf[i] != read_buf[i])
							rt_kprintf("write_buf[%d]=0x%x,read_buf[%d]=0x%x\r\n",i,write_buf[i],i,read_buf[i]);
						}
						return;
					}

					if(Blcok == 0x43b0)
					{
					
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
			
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);
						 REG_WRITE(REG_GPIO_2_CONFIG, 0x02);

					}
					
					if(Blcok == 0x43d0){
					
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
						REG_WRITE(REG_GPIO_2_CONFIG, 0x00);
					}
					Blcok += 4*8;				
				}
				#endif
				
				#if 1
				if(((count % COUNTER )== 0) && (count>=COUNTER))
				{
					for(i = 0; i < (8*COUNTER); i++)
					{
						if(0 != sdcard_read_multi_block(read_buf + i*512, Blcok, 1))
							rt_kprintf("read error!!!\r\n");
						
						
						Blcok++;
					}
				
					if(memcmp(read_buf, write_buf, RW_BUF_SIZE) == 0)
					{
						rt_kprintf("compare ok:block:%x\r\n",Blcok-8*COUNTER);
					}
					else
					{
						rt_kprintf("compare failed:block:%x\r\n",Blcok-8*COUNTER);
						for(i=0; i<RW_BUF_SIZE; i++)
						{
							if(write_buf[i] != read_buf[i])
							rt_kprintf("write_buf[%d]=0x%x,read_buf[%d]=0x%x\r\n",i,write_buf[i],i,read_buf[i]);
						}
						return;
					}
			
				}
				#endif
				
			}
			
			close(fds);
		}
		else
		{
			rt_kprintf("!!open failed!!\r\n");
		}
	 
	 }
	rt_kprintf("end time:%d\r\n",rt_tick_get());
	//dfs_unmount("/sd");
	//rt_free(pbuf);
}
#endif


void sd_test_write1(int argc, char *argv[])
#if 1
{
	unsigned char *pbuf;
	unsigned char *read_buf,*read_buf1 ;
	unsigned char path_name[] ="sd/write_file1.txt";
	
	long   fds,fd; 
	int i,j,ret = 0;
	int mode;
	if(argc != 2)
		return;
	
	saradc_config_vddram_voltage(PSRAM_VDD_3_3V);
	pbuf = rt_malloc(SD_TEST_SIZE);
	if(pbuf == NULL)
		return;
	read_buf = rt_malloc(SD_TEST_SIZE);
	if(read_buf == NULL)
		return;
		
	for(i=0;i<SD_TEST_SIZE;)
	{
		pbuf[i++] = 0x55;
		pbuf[i++] = 0x56;
		pbuf[i++] = 0x57;
		pbuf[i++] = 0x58;
		pbuf[i++] = 0x59;
		pbuf[i++] = 0x60;
		pbuf[i++] = 0x61;
		pbuf[i++] = 0x62;
	}

	mode = atoi(argv[1]);
	
	rt_kprintf("start time:%d\r\n",rt_tick_get());
	
	if(mode == 0)
	{
		if(mount_sd() == 0)
		{
			rt_kprintf("---sd mount ok---\r\n");
		}
		else
		{
			rt_kprintf("!!!! sd mount failed\r\n");
		}
		for(i=0; i<100; i++)
		{
			fds = open(path_name,O_RDWR|O_CREAT|O_BINARY|O_TRUNC ,0600);
			if(fds >= 0)
			{
				ret = write(fds,pbuf,SD_TEST_SIZE);
				if(ret != SD_TEST_SIZE)
				{	
					rt_kprintf("------write failed\r\n");
				}		
			}
			else
			{
				rt_kprintf("!!open failed!!\r\n");
			}		
			close(fds);	

			fds = open(path_name,O_RDONLY);
			if(fds >= 0)
			{
				ret = read(fds,read_buf,SD_TEST_SIZE);
				if(ret != SD_TEST_SIZE)
				{	
					rt_kprintf("------write failed\r\n");
				}		
			}
			else
			{
				rt_kprintf("!!open failed!!\r\n");
			}		
			close(fds);	

			if(memcmp(read_buf, pbuf, SD_TEST_SIZE) == 0)
			{
				rt_kprintf("compare ok:%d\r\n",j);
			}
			else
			{
				rt_kprintf("!!!compare failed \r\n");
			}	
			j++;
		}
	 }
	
	if(mode == 1 )
	{
		if(mount_sd() == 0)
		{
			rt_kprintf("---sd mount ok---\r\n");
		}
		else
		{
			rt_kprintf("!!!! sd mount failed\r\n");
		}

		i =0x800000;
		j=300;
		while(j--)
		{
			if(0 != sdcard_write_multi_block(pbuf,i,4))
				rt_kprintf("write error!!!\r\n");

			i +=4;
			
			if(0 != sdcard_read_multi_block(read_buf,i,4))
				rt_kprintf("read error!!!\r\n");	

			if(memcmp(read_buf, pbuf, SD_TEST_SIZE) == 0)
			{
				rt_kprintf("ok:count:%d\r\n",j);
			}
			else
			{
				rt_kprintf("failed:count:%d\r\n",j);
			}
		}
		
	 }
	rt_kprintf("end time:%d\r\n",rt_tick_get());
}
#else
{
	unsigned char *pbuf;
	unsigned char *read_buf,*read_buf1 ;
	unsigned char path_name[] ="sd/write_file1.txt";
	unsigned char path_name1[]="sd/write_file2.txt";
	
	long   fds,fd; 
	int i,j,ret;
	int mode;
	if(argc != 2)
		return;
	
	saradc_config_vddram_voltage(PSRAM_VDD_3_3V);
	pbuf = rt_malloc(SD_TEST_SIZE);
	if(pbuf == NULL)
		return;
	read_buf = rt_malloc(SD_TEST_SIZE);
	if(read_buf == NULL)
		return;
	read_buf1 = rt_malloc(SD_TEST_SIZE);
	if(read_buf1 == NULL)
		return;	
	
	for(i=0;i<SD_TEST_SIZE;)
	{
		pbuf[i++] = 0x55;
		pbuf[i++] = 0x56;
		pbuf[i++] = 0x57;
		pbuf[i++] = 0x58;
		pbuf[i++] = 0x59;
		pbuf[i++] = 0x60;
		pbuf[i++] = 0x61;
		pbuf[i++] = 0x62;
	}

	mode = atoi(argv[1]);
	
	rt_kprintf("start time:%d\r\n",rt_tick_get());
	
	if(mode == 0)
	{
		if(mount_sd() == 0)
		{
			rt_kprintf("---sd mount ok---\r\n");
		}
		else
		{
			rt_kprintf("!!!! sd mount failed\r\n");
		}
		
		fds = open(path_name,O_RDWR|O_CREAT|O_BINARY|O_TRUNC ,0600);
		
		if(fds >= 0)
		{
			if(SD_TEST_SIZE == write(fds,pbuf,SD_TEST_SIZE))
			{
				Delay(1000);				
				rt_kprintf("write ok\r\n");
			}
			else
			{	
				rt_kprintf("------write failed\r\n");
			}		
		}
		else
		{
			rt_kprintf("!!open failed!!\r\n");
		}
		
		close(fds);	 
	 }
	
	if(mode == 1)
	{
		fd = open(path_name1,O_RDWR|O_CREAT|O_BINARY|O_TRUNC ,0600);
		
		if(fd >= 0)
		{
			if(SD_TEST_SIZE == write(fd,pbuf,SD_TEST_SIZE))
			{
				Delay(1000);				
				rt_kprintf("write ok\r\n");
			}
			else
			{	
				rt_kprintf("------write failed\r\n");
			}		
		}
		else
		{
			rt_kprintf("!!open failed!!\r\n");
		}
		close(fd);			 
	 }
	
	if(mode == 2)
	{
	
		fds = open(path_name,O_RDONLY);
		if(fds >= 0)
		{
				rt_kprintf("open ok\r\n");
				
				ret = read(fds,read_buf,SD_TEST_SIZE);	
				if(SD_TEST_SIZE == ret)
				{					
					rt_kprintf("read ok\r\n");
				}
				else
				{	
				
					rt_kprintf("ret=0x%x\r\n",ret);
					rt_kprintf("------read failed\r\n");
				}			
				close(fds);
		}
		
		fd = open(path_name1,O_RDONLY);
		if(fd >= 0)
		{
				rt_kprintf("open ok\r\n");
				
				ret = read(fd,read_buf1,SD_TEST_SIZE);	
				if(SD_TEST_SIZE == ret)
				{					
					rt_kprintf("read ok\r\n");
				}
				else
				{	
				
					rt_kprintf("ret=0x%x\r\n",ret);
					rt_kprintf("------read failed\r\n");
				}			
				close(fd);
		}
					
		if(memcmp(read_buf, read_buf1, SD_TEST_SIZE) == 0)
		{
			rt_kprintf("compare ok \r\n");
		}
		else
		{
			rt_kprintf("!!!compare failed \r\n");
		}
		
		dfs_unmount("/sd");
		rt_free(pbuf);
		rt_free(read_buf);
		rt_free(read_buf1);
	}
	rt_kprintf("end time:%d\r\n",rt_tick_get());
}

#endif
MSH_CMD_EXPORT(sd_test_write, sd_test_write);
MSH_CMD_EXPORT(sd_test_write1, sd_test_write1);
MSH_CMD_EXPORT(sd_play_test, sd_play_test);
#endif
