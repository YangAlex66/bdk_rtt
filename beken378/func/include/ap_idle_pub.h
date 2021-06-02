#ifndef __AP_IDLE_H_
#define __AP_IDLE_H_

#include "typedef.h"

extern void ap_ps_enable_set ( void );
extern void ap_ps_enable_clear ( void );
extern UINT32 ap_ps_enable_get ( void );
UINT32 ap_if_ap_rf_sleep ( void );

#endif //__AP_IDLE_H_
