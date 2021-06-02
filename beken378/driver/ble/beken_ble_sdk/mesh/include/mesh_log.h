// Copyright (C) Beken 2019-2020
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _MESH_LOG_H_
#define _MESH_LOG_H_

#include "mesh_config.h"
#include "ble_uart.h"

/* Define trace levels */
#define MESH_TRACE_LEVEL_NONE    0          /* No trace messages to be generated    */
#define MESH_TRACE_LEVEL_ERROR   1          /* Error condition trace messages       */
#define MESH_TRACE_LEVEL_WARNING 2          /* Warning condition trace messages     */
#define MESH_TRACE_LEVEL_INFO    3          /* Information traces                           */
#define MESH_TRACE_LEVEL_DEBUG   4          /* Debug messages for events            */

//#define MESH_APP_LOG_LEVEL       MESH_TRACE_LEVEL_DEBUG    /* Define the Mesh Application log default value to Debug. */
#define MESH_APP_LOG_LEVEL       MESH_TRACE_LEVEL_INFO

#define MESH_NET_LOG_LEVEL       MESH_TRACE_LEVEL_INFO  /* Define the Mesh Network log default value to Warning. */
 
//#define MESH_MODEL_LOG_LEVEL     MESH_TRACE_LEVEL_NONE  /* Define the Mesh Model log default value to Warning. */
#define MESH_MODEL_LOG_LEVEL     MESH_TRACE_LEVEL_INFO  /* Define the Mesh Model log default value to Warning. */

#define MESH_PRO_LOG_LEVEL       MESH_TRACE_LEVEL_INFO  /* Define the Mesh Provisioning log default value to Warning. */
#define MESH_TRANS_LOG_LEVEL     MESH_TRACE_LEVEL_INFO  /* Define the Mesh Transation log default value to Warning. */
 
#define MESH_BEAR_LOG_LEVEL      MESH_TRACE_LEVEL_INFO  /* Define the Mesh Bearer log default value to Warning. */
#define MESH_ACCESS_LOG_LEVEL    MESH_TRACE_LEVEL_INFO  /* Define the Mesh Access log default value to Warning. */
#define MESH_TB_LOG_LEVEL        MESH_TRACE_LEVEL_INFO  /* Define the Mesh TB log default value to Warning. */


#define MAX_TRACE_LEVEL        MESH_TRACE_LEVEL_INFO

/* The log color feature only available for Linux and Mac system, for Windows system please don't config it. */
#if CONFIG_LOG_COLORS
#define LOG_COLOR_BLACK   "30"
#define LOG_COLOR_RED     "31"
#define LOG_COLOR_GREEN   "32"
#define LOG_COLOR_BROWN   "33"
#define LOG_COLOR_BLUE    "34"
#define LOG_COLOR_PURPLE  "35"
#define LOG_COLOR_CYAN    "36"
#define LOG_COLOR(COLOR)  "\033[0;" COLOR "m"
#define LOG_BOLD(COLOR)   "\033[1;" COLOR "m"
#define LOG_RESET_COLOR   "\033[0m"
#define LOG_COLOR_E       LOG_COLOR(LOG_COLOR_RED)
#define LOG_COLOR_W       LOG_COLOR(LOG_COLOR_BROWN)
#define LOG_COLOR_I       LOG_COLOR(LOG_COLOR_BLUE)
#define LOG_COLOR_D       LOG_COLOR(LOG_COLOR_GREEN)
#else //CONFIG_LOG_COLORS
#define LOG_COLOR_E
#define LOG_COLOR_W
#define LOG_COLOR_I
#define LOG_COLOR_D
#define LOG_RESET_COLOR
#endif //CONFIG_LOG_COLORS

#define LOG_FORMAT(tag, letter, format)  #tag " " LOG_COLOR_ ## letter #letter ": " format "\n"

#if (BLE_MESH_LOG_OUTPUT)
/* Define tracing for Mesh Network uint
 */
#if (MESH_NET_LOG_LEVEL >= MESH_TRACE_LEVEL_ERROR)
#define MESH_NET_PRINT_ERR(format, ...)        {uart_printf(LOG_FORMAT([Net], E, format), ##__VA_ARGS__); }
#else
#define MESH_NET_PRINT_ERR(format, ...)
#endif /* #if (MESH_NET_LOG_LEVEL >= MESH_TRACE_LEVEL_ERROR) */

#if (MESH_NET_LOG_LEVEL >= MESH_TRACE_LEVEL_WARNING)
#define MESH_NET_PRINT_WARN(format, ...)       {uart_printf(LOG_FORMAT([Net], W, format), ##__VA_ARGS__); }
#else
#define MESH_NET_PRINT_WARN(format, ...)
#endif /* #if (MESH_NET_LOG_LEVEL >= MESH_TRACE_LEVEL_WARNING) */

#if (MESH_NET_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO)
#define MESH_NET_PRINT_INFO(format, ...)       {uart_printf(LOG_FORMAT([Net], I, format), ##__VA_ARGS__); }
#else
#define MESH_NET_PRINT_INFO(format, ...)
#endif /* MESH_NET_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO */

#if (MESH_NET_LOG_LEVEL >= MESH_TRACE_LEVEL_DEBUG)
#define MESH_NET_PRINT_DEBUG(format, ...)      {uart_printf(LOG_FORMAT([Net], D, format), ##__VA_ARGS__);}
#else
#define MESH_NET_PRINT_DEBUG(format, ...)
#endif /* MESH_NET_LOG_LEVEL >= MESH_TRACE_LEVEL_DEBUG */

/* Define tracing for Mesh Model uint
 */
#if (MESH_MODEL_LOG_LEVEL >= MESH_TRACE_LEVEL_ERROR)
#define MESH_MODEL_PRINT_ERR(format, ...)      {uart_printf(LOG_FORMAT([Model], E, format), ##__VA_ARGS__); }
#else
#define  MESH_MODEL_PRINT_ERR(format, ...)
#endif /* MESH_MODEL_LOG_LEVEL >= MESH_TRACE_LEVEL_ERROR */

#if (MESH_MODEL_LOG_LEVEL >= MESH_TRACE_LEVEL_WARNING)
#define MESH_MODEL_PRINT_WARN(format, ...)     {uart_printf(LOG_FORMAT([Model], W, format), ##__VA_ARGS__); }
#else
#define MESH_MODEL_PRINT_WARN(format, ...)
#endif /* MESH_MODEL_LOG_LEVEL >= MESH_TRACE_LEVEL_WARNING */

#if (MESH_MODEL_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO)
#define MESH_MODEL_PRINT_INFO(format, ...)     {uart_printf(LOG_FORMAT([Model], I, format), ##__VA_ARGS__); }
#else
#define MESH_MODEL_PRINT_INFO(format, ...)
#endif /* MESH_MODEL_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO */

#if (MESH_MODEL_LOG_LEVEL >= MESH_TRACE_LEVEL_DEBUG)
#define MESH_MODEL_PRINT_DEBUG(format, ...)    {uart_printf(LOG_FORMAT([Model], D, format), ##__VA_ARGS__); }
#else
#define MESH_MODEL_PRINT_DEBUG(format, ...)
#endif /* MESH_MODEL_LOG_LEVEL >= MESH_TRACE_LEVEL_DEBUG */
/* Define tracing for the Mesh Provisioning unit
 */
#if (MESH_PRO_LOG_LEVEL >= MESH_TRACE_LEVEL_ERROR)
#define MESH_PRO_PRINT_ERR(format, ...)        {uart_printf(LOG_FORMAT([Prov], E, format), ##__VA_ARGS__); }
#else
#define MESH_PRO_PRINT_ERR(format, ...)
#endif /* MESH_PRO_LOG_LEVEL >= MESH_TRACE_LEVEL_ERROR */

#if (MESH_PRO_LOG_LEVEL >= MESH_TRACE_LEVEL_WARNING)
#define MESH_PRO_PRINT_WARN(format, ...)       {uart_printf(LOG_FORMAT([Prov], W, format), ##__VA_ARGS__);}
#else
#define MESH_PRO_PRINT_WARN(format, ...)
#endif /* MESH_PRO_LOG_LEVEL >= MESH_TRACE_LEVEL_WARNING */

#if (MESH_PRO_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO)
#define MESH_PRO_PRINT_INFO(format, ...)       {uart_printf(LOG_FORMAT([Prov], I, format), ##__VA_ARGS__);}
#else
#define MESH_PRO_PRINT_INFO(format, ...)
#endif /* MESH_PRO_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO */

#if (MESH_PRO_LOG_LEVEL >= MESH_TRACE_LEVEL_DEBUG)
#define MESH_PRO_PRINT_DEBUG(format, ...)      {uart_printf(LOG_FORMAT([Prov], D, format), ##__VA_ARGS__);}
#else
#define MESH_PRO_PRINT_DEBUG(format, ...)
#endif /* MESH_PRO_LOG_LEVEL >= MESH_TRACE_LEVEL_DEBUG */
/* Define tracing for the Mesh transport unit
 */
#if (MESH_TRANS_LOG_LEVEL >= MESH_TRACE_LEVEL_ERROR)
#define MESH_TRANS_PRINT_ERR(format, ...)        {uart_printf(LOG_FORMAT([Trans], E, format), ##__VA_ARGS__); }
#else
#define MESH_TRANS_PRINT_ERR(format, ...)
#endif /* MESH_TRANS_LOG_LEVEL >= MESH_TRACE_LEVEL_ERROR */

#if (MESH_TRANS_LOG_LEVEL >= MESH_TRACE_LEVEL_WARNING)
#define MESH_TRANS_PRINT_WARN(format, ...)       {uart_printf(LOG_FORMAT([Trans], W, format), ##__VA_ARGS__); }
#else
#define MESH_TRANS_PRINT_WARN(format, ...)
#endif /* MESH_TRANS_LOG_LEVEL >= MESH_TRACE_LEVEL_WARNING */

#if (MESH_TRANS_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO)
#define MESH_TRANS_PRINT_INFO(format, ...)       {uart_printf(LOG_FORMAT([Trans], I, format), ##__VA_ARGS__); }
#else
#define MESH_TRANS_PRINT_INFO(format, ...)
#endif /* MESH_TRANS_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO */

#if (MESH_TRANS_LOG_LEVEL >= MESH_TRACE_LEVEL_DEBUG)
#define MESH_TRANS_PRINT_DEBUG(format, ...)      {uart_printf(LOG_FORMAT([Trans], D, format), ##__VA_ARGS__); }
#else
#define MESH_TRANS_PRINT_DEBUG(format, ...)
#endif /* MESH_TRANS_LOG_LEVEL >= MESH_TRACE_LEVEL_DEBUG */
/* Define tracing for the Mesh APP unit
 */
#if (MESH_APP_LOG_LEVEL >= MESH_TRACE_LEVEL_ERROR)
#define MESH_APP_PRINT_ERR(format, ...)          {uart_printf(LOG_FORMAT([App], E, format), ##__VA_ARGS__); }
#else
#define MESH_APP_PRINT_ERR(format, ...)
#endif /* MESH_APP_LOG_LEVEL >= MESH_TRACE_LEVEL_ERROR */

#if (MESH_APP_LOG_LEVEL >= MESH_TRACE_LEVEL_WARNING)
#define MESH_APP_PRINT_WARN(format, ...)         {uart_printf(LOG_FORMAT([App], W, format), ##__VA_ARGS__); }
#else
#define MESH_APP_PRINT_WARN(format, ...)
#endif /* MESH_APP_LOG_LEVEL >= MESH_TRACE_LEVEL_WARNING */

#if (MESH_APP_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO)
#define MESH_APP_PRINT_INFO(format, ...)         {uart_printf(LOG_FORMAT([App], I, format), ##__VA_ARGS__); }
#else
#define MESH_APP_PRINT_INFO(format, ...)
#endif /* MESH_APP_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO */

#if (MESH_APP_LOG_LEVEL >= MESH_TRACE_LEVEL_DEBUG)
#define MESH_APP_PRINT_DEBUG(format, ...)        {uart_printf(LOG_FORMAT([App], D, format), ##__VA_ARGS__); }
#else
#define MESH_APP_PRINT_DEBUG(format, ...)
#endif /* MESH_APP_LOG_LEVEL >= MESH_TRACE_LEVEL_DEBUG */

/* Define tracing for the Mesh Bearer unit
 */
#if (MESH_BEAR_LOG_LEVEL >= MESH_TRACE_LEVEL_ERROR)
#define MESH_BEAR_PRINT_ERR(format, ...)          {uart_printf(LOG_FORMAT([Bear], E, format), ##__VA_ARGS__); }
#else
#define MESH_BEAR_PRINT_ERR(format, ...)
#endif /* MESH_BEAR_LOG_LEVEL >= MESH_TRACE_LEVEL_ERROR */

#if (MESH_BEAR_LOG_LEVEL >= MESH_TRACE_LEVEL_WARNING)
#define MESH_BEAR_PRINT_WARN(format, ...)         {uart_printf(LOG_FORMAT([Bear], W, format), ##__VA_ARGS__); }
#else
#define MESH_BEAR_PRINT_WARN(format, ...)
#endif /* MESH_BEAR_LOG_LEVEL >= MESH_TRACE_LEVEL_WARNING */

#if (MESH_BEAR_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO)
#define MESH_BEAR_PRINT_INFO(format, ...)         {uart_printf(LOG_FORMAT([Bear], I, format), ##__VA_ARGS__); }
#else
#define MESH_BEAR_PRINT_INFO(format, ...)
#endif /* MESH_BEAR_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO */

#if (MESH_BEAR_LOG_LEVEL >= MESH_TRACE_LEVEL_DEBUG)
#define MESH_BEAR_PRINT_DEBUG(format, ...)        {uart_printf(LOG_FORMAT([Bear], D, format), ##__VA_ARGS__); }
#else
#define MESH_BEAR_PRINT_DEBUG(format, ...)
#endif /* MESH_BEAR_LOG_LEVEL >= MESH_TRACE_LEVEL_DEBUG */

/* Define tracing for the Mesh Access unit
 */
#if (MESH_ACCESS_LOG_LEVEL >= MESH_TRACE_LEVEL_ERROR)
#define MESH_ACCESS_PRINT_ERR(format, ...)          {uart_printf(LOG_FORMAT([Access], E, format), ##__VA_ARGS__); }
#else
#define MESH_ACCESS_PRINT_ERR(format, ...)
#endif /* MESH_ACCESS_LOG_LEVEL >= MESH_TRACE_LEVEL_ERROR */

#if (MESH_ACCESS_LOG_LEVEL >= MESH_TRACE_LEVEL_WARNING)
#define MESH_ACCESS_PRINT_WARN(format, ...)         {uart_printf(LOG_FORMAT([Access], W, format), ##__VA_ARGS__); }
#else
#define MESH_ACCESS_PRINT_WARN(format, ...)
#endif /* MESH_ACCESS_LOG_LEVEL >= MESH_TRACE_LEVEL_WARNING */

#if (MESH_ACCESS_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO)
#define MESH_ACCESS_PRINT_INFO(format, ...)         {uart_printf(LOG_FORMAT([Access], I, format), ##__VA_ARGS__); }
#else
#define MESH_ACCESS_PRINT_INFO(format, ...)
#endif /* MESH_ACCESS_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO */

#if (MESH_ACCESS_LOG_LEVEL >= MESH_TRACE_LEVEL_DEBUG)
#define MESH_ACCESS_PRINT_DEBUG(format, ...)        {uart_printf(LOG_FORMAT([Access], D, format), ##__VA_ARGS__); }
#else
#define MESH_ACCESS_PRINT_DEBUG(format, ...)
#endif /* MESH_ACCESS_LOG_LEVEL >= MESH_TRACE_LEVEL_DEBUG */

/* Define tracing for the Mesh TB unit
 */
#if (MESH_TB_LOG_LEVEL >= MESH_TRACE_LEVEL_ERROR)
#define MESH_TB_PRINT_ERR(format, ...)          {uart_printf(LOG_FORMAT([Tb], E, format), ##__VA_ARGS__); }
#else
#define MESH_TB_PRINT_ERR(format, ...)
#endif /* MESH_TB_LOG_LEVEL >= MESH_TRACE_LEVEL_ERROR */

#if (MESH_TB_LOG_LEVEL >= MESH_TRACE_LEVEL_WARNING)
#define MESH_TB_PRINT_WARN(format, ...)         {uart_printf(LOG_FORMAT([Tb], W, format), ##__VA_ARGS__); }
#else
#define MESH_TB_PRINT_WARN(format, ...)
#endif /* MESH_TB_LOG_LEVEL >= MESH_TRACE_LEVEL_WARNING */

#if (MESH_TB_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO)
#define MESH_TB_PRINT_INFO(format, ...)         {uart_printf(LOG_FORMAT([Tb], I, format), ##__VA_ARGS__); }
#else
#define MESH_TB_PRINT_INFO(format, ...)
#endif /* MESH_TB_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO */

#if (MESH_TB_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO)
#define MESH_TB_PRINT_DEBUG(format, ...)        {uart_printf(LOG_FORMAT([Tb], D, format), ##__VA_ARGS__); }
#else
#define MESH_TB_PRINT_DEBUG(format, ...)
#endif /* MESH_TB_LOG_LEVEL >= MESH_TRACE_LEVEL_INFO */

#else /* !CONFIG_MESH_LOG_OUTPUT */
/* Define tracing for Mesh Network uint
 */
#define MESH_NET_PRINT_ERR(format, ...)
#define MESH_NET_PRINT_WARN(format, ...)
#define MESH_NET_PRINT_INFO(format, ...)
#define MESH_NET_PRINT_DEBUG(format, ...)

/* Define tracing for Mesh Model uint
 */
#define MESH_MODEL_PRINT_ERR(format, ...)
#define MESH_MODEL_PRINT_WARN(format, ...)
#define MESH_MODEL_PRINT_INFO(format, ...)
#define MESH_MODEL_PRINT_DEBUG(format, ...)

/* Define tracing for the Mesh Provisioning unit
 */
#define MESH_PRO_PRINT_ERR(format, ...)
#define MESH_PRO_PRINT_WARN(format, ...)
#define MESH_PRO_PRINT_INFO(format, ...)
#define MESH_PRO_PRINT_DEBUG(format, ...)

/* Define tracing for the Mesh transport unit
 */
#define MESH_TRANS_PRINT_ERR(format, ...)
#define MESH_TRANS_PRINT_WARN(format, ...)
#define MESH_TRANS_PRINT_INFO(format, ...)
#define MESH_TRANS_PRINT_DEBUG(format, ...)

/* Define tracing for the Mesh APP unit
 */
#define MESH_APP_PRINT_ERR(format, ...)
#define MESH_APP_PRINT_WARN(format, ...)
#define MESH_APP_PRINT_INFO(format, ...)
#define MESH_APP_PRINT_DEBUG(format, ...)

/* Define tracing for the Mesh Bearer unit
 */
#define MESH_BEAR_PRINT_ERR(format, ...)
#define MESH_BEAR_PRINT_WARN(format, ...)
#define MESH_BEAR_PRINT_INFO(format, ...)
#define MESH_BEAR_PRINT_DEBUG(format, ...)

/* Define tracing for the Mesh Access unit
 */
#define MESH_ACCESS_PRINT_ERR(format, ...)
#define MESH_ACCESS_PRINT_WARN(format, ...)
#define MESH_ACCESS_PRINT_INFO(format, ...)
#define MESH_ACCESS_PRINT_DEBUG(format, ...)

/* Define tracing for the Mesh TB unit
 */
#define MESH_TB_PRINT_ERR(format, ...)
#define MESH_TB_PRINT_WARN(format, ...)
#define MESH_TB_PRINT_INFO(format, ...)
#define MESH_TB_PRINT_DEBUG(format, ...)


#endif /* CONFIG_MESH_LOG_OUTPUT */

#endif /* _MESH_LOG_H_ */

const char *mesh_buffer_to_hex(const void *buf, unsigned int len);

void mem_rcopy(uint8_t *dst, uint8_t const *src, uint16_t len);
