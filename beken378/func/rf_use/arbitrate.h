#include "co_list.h"

#ifndef ARBITRATE_H_
#define ARBITRATE_H_

#include "sys_config.h"

#define RF_MSG_QUEUE_COUNT          (20)
#define THD_RF_PRIORITY             (1)
#define RF_STACK_SIZE               (2 * 1024)
#define RF_TIMER_ID                 (1)

/*
Rf message
*/
typedef struct
{
    uint8_t msg_id;
    void *msg_param;
} rf_msg_t;

enum
{
    RF_IDLE = 0,
    RF_IN_GUARD,
    RF_WIFI_USE,
    RF_BLE_USE,
};

#if (RF_USE_POLICY == WIFI_DEFAULT_BLE_REQUEST)

#elif (RF_USE_POLICY == BLE_DEFAULT_WIFI_REQUEST)
#define WIFI_STA_HD_BCN_TIMER        1

#define WIFI_WAKE_FORWARD_TIME      (3)
#define WIFI_GUARD_TIME             (2)
#define BLE_GUARD_TIME              (2)
#define RF_WIFI_TIME_MAX            (60)
#define RF_WIFI_TIME_MIN            (10)
#define RF_BLE_TIME_MAX             (80)
#define RF_BLE_TIME_MIN             (6)

#if WIFI_STA_HD_BCN_TIMER
#define WSTA_BCN_TIMER_ID           (0)
#define WSTA_BCN_TIMER_DIV          (1)
#endif

struct pre_send_node
{
    struct co_list_hdr list_hdr;
    struct tx_hd *first_thd;
    struct tx_hd *last_thd;
    uint8_t access_category;
};

struct beacon_infor
{
    uint16_t status;
    uint16_t tatol_miss_bcn_cnt;
};

enum
{
    WIFI_REQUEST_RF = 0,
    WIFI_RELEASE_RF,
    WIFI_SEND_DATA,
    WIFI_SEND_OK,
    WIFI_READY_TO_SEND,
    WIFI_PS_BEACON,
    BLE_REQUEST_RF,
    BLE_RELEASE_RF,
    RF_NULL_MSG,
};

typedef struct 
{
    uint16_t dtim_period;
    uint8_t dtim_count;

    uint16_t bcn_t1;

    uint16_t interval;
    uint32_t bcn_int;

    unsigned bcn_timer_run : 1;
    uint32_t g_time_us;
}wifi_sta_state_t;

#elif (RF_USE_POLICY == BLE_WIFI_CO_REQUEST)
enum
{
    RF_BREAK_EN_MASK = 0x1,
    RF_BREAK_EN_POS  = 0,
    
    RF_REQUESTER_MASK     = 0x2,
    RF_REQUESTER_POS      = 1,
};

enum
{
    RF_WIFI_REQUEST = 0,
    RF_BLE_REQUEST  = 1,
};

enum
{
    WIFI_BREAK_WIFI = 0,
    WIFI_BREAK_BLE,
    BLE_BREAK_BLE,
    BLE_BREAK_WIFI,
    RF_NULL_MSG,
};

/*
Rf requester information
*/
typedef struct
{
    uint8_t  event_type;
    uint8_t  event_priority;
    uint8_t  event_flag;
    uint8_t  reverse;
    uint32_t use_time;
} rf_req_t;

/*
Current rf user information
*/
typedef struct
{
    uint8_t  event_type;
    uint8_t  event_priority;
    uint8_t  event_flag;
    uint8_t  reverse;
    uint32_t start_time;
    uint32_t use_time;
    uint32_t used_time;
} rf_user_t;

/*
Rf break event
*/
typedef struct
{
    rf_req_t request_user;
    rf_user_t current_user;
} rf_break_msg_t;

typedef struct
{
    uint8_t status;
    rf_user_t rf_current_user;
} rf_info_t;
#endif

#if (RF_USE_POLICY == WIFI_DEFAULT_BLE_REQUEST)

#elif (RF_USE_POLICY == BLE_DEFAULT_WIFI_REQUEST)
uint8_t get_current_user(void);
extern int wifi_beacon_info_sender(int status,int param);
extern int wifi_beacon_info_handler(rf_msg_t *msg);
#elif (RF_USE_POLICY == BLE_WIFI_CO_REQUEST)
OSStatus request_rf(rf_req_t *rf_request, rf_user_t *rf_user);
#endif

void rf_thread_init(void);
void rf_thread_uninit(void);

#endif
