#include "sys_config.h"

#if CFG_USE_SPI_MST_FLASH

#if !CFG_USE_SPI_MASTER
#error "SPI FLASH NEED CFG_USE_SPI_MASTER ENABLE!!!"
#endif

#include <fal.h>
#include "flash.h"
#include "flash_pub.h"
#include <string.h>
#include "spi_flash.h"

//#define DBG_ENABLE
#define DBG_SECTION_NAME "fal_flash_spi_nor_flash_port"
#define DBG_LEVEL        DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>


static int init(void)
{
	LOG_D("****init");
	return spi_flash_init();
}

static int erase(long offset, size_t size)
{
	spi_flash_unprotect();

	LOG_D("****erase");
	spi_flash_erase(offset,size);

	return size;
}

static int read(long offset, uint8_t *buf, size_t size)
{
	spi_flash_read(offset,size,buf);
	LOG_D("****read");
    return size;
}

static int write(long offset, const uint8_t *buf, size_t size)
{
	spi_flash_unprotect();
	spi_flash_write(offset,size, (uint8_t*)buf);
	LOG_D("***write");
	return size;
}

#ifndef NOR_FLASH_DEV_NAME
#define NOR_FLASH_DEV_NAME             "beken_ext_flash"
#endif

/* spi nor flash dev */
const struct fal_flash_dev beken_ext_flash_device = 
{
    NOR_FLASH_DEV_NAME, 
    0, 
    4*1024*1024, 
    4096,
    {init, read, write, erase} 
};
#endif
