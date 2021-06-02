// Copyright 2020-2021 Beken
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/* This file defines error code across the SDK.
 *
 * Rules for defining error code:
 * 1. Define the module error code base here
 * 2. Define the module error code in module specific headers
 * 3. Keep the error code unique across the whole SDK
 * 4. Carefully avoid to define same error code as 3rd party code, such as LWIP etc.
 *
 * Rules for using error code:
 * 1. Error code kXxx, such as kNoMemoryErr, are used for RTOS only, don't use it for
 *    other modules
 * 2. The return error code type SHALL be bk_err_t for new non-RTOS SDK API
 * 3. The return error code type for new beken modules SHALL be bk_err_t
 * 4. Don't return hard-coded error code, return the error code macro
 * 5. Recommend to check the API return value via BK_ERR_CHECK
 *
 * TODO:
 * 1. Update return error code type of non-RTOS API from OSStatus to bk_err_t in v4.0
 *
 * */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef int bk_err_t;

#define BK_OK                      0
#define BK_FAIL                    -1

#define BK_ERR_COMMON_BASE         (-0x1000)
#define BK_ERR_WLAN_BASE           (-0x1200)
#define BK_ERR_WPA_BASE            (-0x1400)
#define BK_ERR_BLE_BASE            (-0x1600)
#define BK_ERR_RWNX_BASE           (-0x1800)

/* -0x1a2c to -0x1a7b is reserved for kXxx error code
 * #define kGenericErrorBase           -6700
 * #define kGenericErrorEnd            -6779
 * */
#define BK_ERR_RTOS_BASE           (-0x1a2c) //-6700

#define BK_ERR_NOT_INIT            (BK_ERR_COMMON_BASE)
#define BK_ERR_PARAM               (BK_ERR_COMMON_BASE - 1)
#define BK_ERR_NOT_FOUND           (BK_ERR_COMMON_BASE - 2)
#define BK_ERR_OPEN                (BK_ERR_COMMON_BASE - 3)
#define BK_ERR_IN_PROGRESS         (BK_ERR_COMMON_BASE - 4)
#define BK_ERR_NO_MEM              (BK_ERR_COMMON_BASE - 5)
#define BK_ERR_TIMEOUT             (BK_ERR_COMMON_BASE - 6)
#define BK_ERR_STATE               (BK_ERR_COMMON_BASE - 7)
#define BK_ERR_TRY_AGAIN           (BK_ERR_COMMON_BASE - 8)

#define BK_ERR_CHECK(_x) do {\
	bk_err_t _err = (_x);\
	if (_err != BK_OK) {\
		os_printf("%s %d: ret=-0x%x\n", __FUNCTION__, __LINE__, -_err);\
	}\
} while(0)

#ifdef __cplusplus
}
#endif
