#ifndef __DRV_WDT_H__
#define __DRV_WDT_H__

#include "wdt_pub.h"

int rt_hw_wdt_init(void);
int rt_hw_wdt_exit(void);
void rt_hw_wdg_start(int argc, char **argv);
void rt_hw_wdg_stop(void);

#endif
