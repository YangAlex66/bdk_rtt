/*
 * 程序清单： 这是一个以littlefs挂载外部存储flash设备的使用例
 * 例程导出了spi_flash_lfs_test 命令到控制终端
 * 程序功能： 将spi flash以lfs文件系统类型挂载到设备虚拟文件系统DFS;2)lfs raw api的使用(未挂载到RT-THREAD DFS框架)
 */

#include "test_config.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <finsh.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <dfs_posix.h>
#include "fal.h"
#ifdef SPI_FLASH_LFS_TEST
#include "lfs.h"

#define LFS_SPI_NOR_FLASH_PARTITION_NAME "download"
#define MOUNT_PATH  "/flash1"

// variables used by the filesystem
static lfs_t lfs;
static struct lfs_config cfg;
static lfs_file_t file;


// Read a region in a block. Negative error codes are propogated
// to the user.
static int lfs_flash_read(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size)
{
    struct rt_mtd_nor_device* mtd_nor;

    RT_ASSERT(c != RT_NULL);
    RT_ASSERT(c->context != RT_NULL);

    mtd_nor = (struct rt_mtd_nor_device*)c->context;
    if (rt_mtd_nor_read(mtd_nor, block * c->block_size + off, buffer, size) != size)
    {
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

// Program a region in a block. The block must have previously
// been erased. Negative error codes are propogated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
static int lfs_flash_prog(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size)
{
    struct rt_mtd_nor_device* mtd_nor;

    RT_ASSERT(c != RT_NULL);
    RT_ASSERT(c->context != RT_NULL);

    mtd_nor = (struct rt_mtd_nor_device*)c->context;
    if (rt_mtd_nor_write(mtd_nor, block * c->block_size + off, buffer, size) != size)
    {
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

// Erase a block. A block must be erased before being programmed.
// The state of an erased block is undefined. Negative error codes
// are propogated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
static int lfs_flash_erase(const struct lfs_config* c, lfs_block_t block)
{
    struct rt_mtd_nor_device* mtd_nor;

    RT_ASSERT(c != RT_NULL);
    RT_ASSERT(c->context != RT_NULL);

    mtd_nor = (struct rt_mtd_nor_device*)c->context;
    if (rt_mtd_nor_erase_block(mtd_nor, block * c->block_size, c->block_size) != RT_EOK)
    {
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

// Sync the state of the underlying block device. Negative error codes
// are propogated to the user.
static int lfs_flash_sync(const struct lfs_config* c)
{
    return LFS_ERR_OK;
}


#ifndef LFS_READ_SIZE
    #define LFS_READ_SIZE 256
#endif

#ifndef LFS_PROG_SIZE
    #define LFS_PROG_SIZE 256
#endif

#ifndef LFS_BLOCK_SIZE
    #define LFS_BLOCK_SIZE 4096
#endif

#ifndef LFS_CACHE_SIZE
    #define LFS_CACHE_SIZE LFS_PROG_SIZE
#endif

#ifndef LFS_BLOCK_CYCLES
    #define LFS_BLOCK_CYCLES (-1)
#endif

#ifndef LFS_LOOKAHEAD_MAX
    #define LFS_LOOKAHEAD_MAX 128
#endif
static void lfs_load_config(struct lfs_config* lfs_cfg, struct rt_mtd_nor_device * mtd_nor) 
{
    lfs_cfg->context = (void*)mtd_nor;

    lfs_cfg->read_size = LFS_READ_SIZE;
    lfs_cfg->prog_size = LFS_PROG_SIZE;

    lfs_cfg->block_size = mtd_nor->block_size;
    if (lfs_cfg->block_size < LFS_BLOCK_SIZE)
    {
        lfs_cfg->block_size = LFS_BLOCK_SIZE;
    }

    lfs_cfg->cache_size = LFS_CACHE_SIZE;
    lfs_cfg->block_cycles = LFS_BLOCK_CYCLES;

    lfs_cfg->block_count = mtd_nor->block_end - mtd_nor->block_start;

    lfs_cfg->lookahead_size = 32 * ((lfs_cfg->block_count + 31) / 32);
    if (lfs_cfg->lookahead_size > LFS_LOOKAHEAD_MAX)
    {
        lfs_cfg->lookahead_size = LFS_LOOKAHEAD_MAX;
    }
	
    lfs_cfg->read = &lfs_flash_read;
    lfs_cfg->prog = &lfs_flash_prog;
    lfs_cfg->erase = &lfs_flash_erase;
    lfs_cfg->sync = &lfs_flash_sync;
}

// entry point
/*feature:
1.support dfs_posix api ops lfs
2.support direct lfs raw api ops lfs
*/
//#define DFS_LFS_TEST
static void spi_flash_lfs_test(int argc, char **argv)
{
#ifdef DFS_LFS_TEST/*LFS 挂载到DFS TEST*/
    if(argc != 2)
    {
        rt_kprintf("argc error!\n");
        return;
    }
    // read current count
    uint16_t boot_count = 0;
    int fd,size;
    off_t offset;
    int ret = 0;

    if(!strcmp(argv[1],"mount"))
    {
        struct rt_device *mtd_dev = fal_mtd_nor_device_create(LFS_SPI_NOR_FLASH_PARTITION_NAME);
        if (!mtd_dev)
        {
            rt_kprintf("Can't create a mtd device on '%s' partition.", LFS_SPI_NOR_FLASH_PARTITION_NAME);
            return;
        }

        rt_kprintf("create mtd nor flash device ok\n");

    	/* mount lfs */
    	if(dfs_mount(LFS_SPI_NOR_FLASH_PARTITION_NAME,MOUNT_PATH, "lfs", 0, 0) == 0)
    	{
    		rt_kprintf("lfs spi flash File System initialized!\n");
    	}
    	else
    	{
    		ret = dfs_mkfs("lfs",LFS_SPI_NOR_FLASH_PARTITION_NAME);
    		if(dfs_mount(LFS_SPI_NOR_FLASH_PARTITION_NAME,MOUNT_PATH, "lfs", 0, 0) == 0)
    		{
    			rt_kprintf("lfs spi flash lfs File System initialized!\n");
    		}
    		else
    		{
    			rt_kprintf("lfs spi flash lfs File System initialzation failed!ret:%d\n",ret);
    		}
    	}
    }
    else if(!strcmp(argv[1],"file"))
    {
        fd = open("/flash1/hello.txt", O_RDWR | O_CREAT);
        if (fd>= 0)
        {
            offset = lseek(fd, 0, SEEK_SET);
            rt_kprintf("offset: 0x%08x\n", offset);
            size = read(fd, &boot_count, sizeof(boot_count));
            if (size < 0)
            {
                rt_kprintf("read fail!\n");
                goto exit;
            }
            rt_kprintf("=============first time read boot_count: %d===========\n", boot_count);
        }
        else
        {
            rt_kprintf("open fail!\n");
            goto exit;
        }

        int i;
        for(i=0;i<10;i++)
        {
            offset = lseek(fd, 0, SEEK_SET);
            rt_kprintf("------offset: 0x%08x\n", offset);
            // update boot count
            boot_count += 1;
            rt_kprintf("update boot_count: %d\n", boot_count);
            write(fd, &boot_count, sizeof(boot_count));
            
            offset = lseek(fd, 0, SEEK_SET);
            rt_kprintf("======offset: 0x%08x\n", offset);
            size = read(fd, &boot_count, sizeof(boot_count));
            rt_kprintf("after updte.read boot_count: %d\n", boot_count);
        }

        ret = fsync(fd);
        if(ret)rt_kprintf("fsync data to storage fail!\n");
        
        ret = close(fd);
        if(ret)rt_kprintf("close fail!\n");
        
    exit:
        return;
    }
    else if(!strcmp(argv[1],"unmount"))
    {
        // release any resources we were using
        ret = dfs_unmount(MOUNT_PATH);
        if(ret)rt_kprintf("unmount fail!\n");

    }
    else
    {
        rt_kprintf("parm error!\n");
    }
#else/*LFS RAW API TEST*/
    struct rt_device *mtd_dev = fal_mtd_nor_device_create(LFS_SPI_NOR_FLASH_PARTITION_NAME);
    if (!mtd_dev)
    {
        rt_kprintf("Can't create a mtd device on '%s' partition.", LFS_SPI_NOR_FLASH_PARTITION_NAME);
        return;
    }

    lfs_load_config(&cfg, (struct rt_mtd_nor_device*)mtd_dev);

    // mount the filesystem
    int err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) 
    {
        lfs_format(&lfs, &cfg);
        err = lfs_mount(&lfs, &cfg);
    	if(err)
    	{
    		rt_kprintf("lfs_mount fail!\n");
    	}
    }

    // read current count
    uint32_t boot_count = 0;
    lfs_file_open(&lfs, &file, "hello.txt", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));
    // print the boot count
    rt_kprintf("read boot_count: %d\n", boot_count);

    // update boot count
    boot_count += 10;
	rt_kprintf("update boot_count: %d\n", boot_count);
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &file);

    // release any resources we were using
    err = lfs_unmount(&lfs);
    if(err)rt_kprintf("lfs lfs_unmount fail!\n");
#endif
}

MSH_CMD_EXPORT(spi_flash_lfs_test, spi_flash_lfs_test);
#endif
