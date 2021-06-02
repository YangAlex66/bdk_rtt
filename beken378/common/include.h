#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#include "sys_config.h"
#include "typedef.h"
#include "generic.h"
#include "compiler.h"
#include "release.h"
#include "arch.h"

#if CFG_ENABLE_DEMO_TEST
#include "demos_config.h"
#endif

#ifndef __maybe_unused
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define __maybe_unused __attribute__((unused))
#else
#define __maybe_unused
#endif /* __GNUC__ */
#endif /* __maybe_unused */

#ifndef __maybe_unused_var
#define __maybe_unused_var(_var) do {\
	(void)(_var);\
} while(0)
#endif

#endif // _INCLUDES_H_
// eof
