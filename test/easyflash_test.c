

#include "rtthread.h"
#include <dfs.h>
#include <dfs_fs.h>
//#include "player.h"
#include "include.h"
#include "driver_pub.h"
#include "func_pub.h"
#include "app.h"
#include "ate_app.h"
#include "shell.h"
#include "flash.h" 	
#include <finsh.h>
#include "easyflash.h"
#include "test_config.h"

#ifdef EASY_FLASH_TEST

static void easy_flash_set(char *key, char *value)
{
	EfErrCode result = EF_NO_ERR;
	
	easyflash_init();					 			/*初始化 */		
	
    result = ef_set_env(key, value);				/*将要写入的数据存放到 easy flash  环境变量 */
	if(result != EF_NO_ERR)
	{
		rt_kprintf("easy_flash set error\r\n");
		return;
	}
	
    result = ef_save_env();							/*保存数据 */
	if(result != EF_NO_ERR)
	{
		rt_kprintf("easy_flash save error\r\n");
		return;
	}
	
    rt_kprintf("---Flash Write over \r\n");	
}

static void easy_flash_get(char *key, char *value)  /*读取easy flash 写入的数据*/
{
	easyflash_init();
	
	value = ef_get_env(key);			/*获取easy flash存入的数据*/	
	if( value )
	{
		rt_kprintf("%s\r\n",value);
	}
	else
	{
		rt_kprintf("easy_flash get error\r\n");	
	}
	return ;
}

static void easy_flash_erase(char *key)  /*读取easy flash 写入的数据*/
{
	EfErrCode result = EF_NO_ERR;
	char value = 0;
	
	easyflash_init();					 			/*初始化 */		
	
    result = ef_set_env(key, &value);				/*将要写入的数据存放到 easy flash  环境变量 */
	if(result != EF_NO_ERR)
	{
		rt_kprintf("easy_flash set error\r\n");
		return;
	}
	result = ef_save_env();		
	if(result != EF_NO_ERR)
	{
		rt_kprintf("easy_flash erase error\r\n");
	}else
	{
		rt_kprintf("easy_flash erase success\r\n");
	}
	return;
}

static int easy_flash(uint8_t argc, char **argv) 
{
	char *key = NULL;
	char *value = NULL;

    if (strcmp(argv[1], "set") == 0)
    {
		os_printf("easyflash set command\r\n");
		if (argc == 4)
		{
			key = argv[2];
			value = argv[3];
		}
		else
		{
			os_printf("parameter invalid\r\n");
			return -1;
		}

		easy_flash_set(key, value);

		return 0;
    }else if (strcmp(argv[1], "get") == 0)
	{
		os_printf("easyflash get command\r\n");
		if (argc == 3)
		{
			key = argv[2];
			easy_flash_get(key, value);
		}
		else
		{
			os_printf("parameter invalid\r\n");
			return -1;
		}

		return 0;
	}else if (strcmp(argv[1], "erase") == 0)
	{
		os_printf("easyflash erase command\r\n");
		if (argc == 3)
		{
			key = argv[2];
			easy_flash_erase(key);
		}
		else
		{
			os_printf("parameter invalid\r\n");
			return -1;
		}

		return 0;
	}
}

MSH_CMD_EXPORT(easy_flash, easy_flash_command: easy_flash <set/get/erase> <key> [value]);

#endif
