#ifndef _WIFI_SENSOR_H_
#define _WIFI_SENSOR_H_

#define SENSOR_DEBUG
#ifdef 	SENSOR_DEBUG
#define SENSOR_PRINTF        bk_printf
#define SENSOR_PRINTF_UART2  bk_printf
#define SENSOR_DEGUB_PRINT   os_null_printf
#else
#define SENSOR_PRINTF        os_null_printf
#define SENSOR_DEGUB_PRINT   os_null_printf
#define SENSOR_PRINTF_UART2  os_null_printf

#endif

extern void bk_wifi_detect_movement_start(void);
extern void bk_wifi_detect_movement_stop(void);

/** @brief  Get the detecting movement result.
 *
 *
 *  @return   1        : something move.
 *  @return   0   	   : no movement in there
 */
extern int  bk_get_movement_status(void);
extern float get_movement_detection_param(void);

#endif

