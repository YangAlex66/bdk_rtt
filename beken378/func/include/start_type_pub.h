#ifndef __START_TYPE_PUB_H_
#define __START_TYPE_PUB_H_

#if (CFG_SOC_NAME == SOC_BK7231N)
#define     START_TYPE_ADDR        (0x00800000 + 84 * 4)
#else
#define     START_TYPE_ADDR        (0x0080a080)
#endif

#define     CRASH_XAT0_VALUE              0xbedead00
#define     CRASH_UNDEFINED_VALUE         0xbedead01
#define     CRASH_PREFETCH_ABORT_VALUE    0xbedead02
#define     CRASH_DATA_ABORT_VALUE        0xbedead03
#define     CRASH_UNUSED_VALUE            0xbedead04
#define     CRASH_2ND_XAT0_VALUE          0xbedead05

#define     START_TYPE_DMEMORY_ADDR        (0x0040001c)


typedef enum { 
	RESET_SOURCE_POWERON = 0x0,
	RESET_SOURCE_REBOOT = 0x1,
	RESET_SOURCE_WATCHDOG = 0x2,

	RESET_SOURCE_DEEPPS_GPIO = 0x3,
	RESET_SOURCE_DEEPPS_RTC = 0x4,

	RESET_SOURCE_CRASH_XAT0 = 0x5,
	RESET_SOURCE_CRASH_UNDEFINED = 0x6,
	RESET_SOURCE_CRASH_PREFETCH_ABORT = 0x7,
	RESET_SOURCE_CRASH_DATA_ABORT = 0x8,
	RESET_SOURCE_CRASH_UNUSED = 0x9,
	RESET_SOURCE_CRASH_PER_XAT0 = 0xc,

    RESET_SOURCE_DEEPPS_USB = 0xa,

} RESET_SOURCE_STATUS;

RESET_SOURCE_STATUS bk_misc_get_start_type();
RESET_SOURCE_STATUS bk_misc_init_start_type(void);
void bk_misc_update_set_type(RESET_SOURCE_STATUS type);
extern void bk_misc_check_start_type();

#endif //__START_TYPE_PUB_H_
