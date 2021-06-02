#include "RomCallFlash.h"
#include "common_bt.h"
#include "common_utils.h"
#include "rwip.h"
#include "ble_uart.h"
#if (BLE_ADV_TEST_MODE)
#include "lld_adv_test.h"
#endif

struct rwip_rf_api rwip_rf;
struct rom_env_tag rom_env;

/// Default BD address
struct bd_addr common_default_bdaddr = {{0x88, 0x77, 0x22, 0x24, 0x34, 0xbb}};

void rom_env_init(struct rom_env_tag *api)
{
    memset(&rom_env, 0, sizeof(struct rom_env_tag));
    rom_env.prf_get_id_from_task = prf_get_id_from_task;
    rom_env.prf_get_task_from_id = prf_get_task_from_id;
    rom_env.prf_init = prf_init;
    rom_env.prf_create = prf_create;
    rom_env.prf_cleanup = prf_cleanup;
    rom_env.prf_add_profile = prf_add_profile;
    rom_env.rwble_hl_reset = rwble_hl_reset;
    rom_env.rwip_reset = rwip_reset;

#if (BLE_ADV_TEST_MODE)
    rom_env.lld_adv_test_rx_isr_cb = lld_adv_test_rx_isr_cb;
    rom_env.lld_adv_test_end_cb = lld_adv_test_end_cb;
#endif

#if (SYSTEM_SLEEP)
    rom_env.rwip_prevent_sleep_set = rwip_prevent_sleep_set;
    rom_env.rwip_prevent_sleep_clear = rwip_prevent_sleep_clear;
    rom_env.rwip_sleep_lpcycles_2_us = rwip_sleep_lpcycles_2_us;
    rom_env.rwip_us_2_lpcycles = rwip_us_2_lpcycles;
    rom_env.rwip_wakeup_delay_set = rwip_wakeup_delay_set;
#endif
    rom_env.platform_reset = platform_reset;

#if (PLF_DEBUG)
    rom_env.assert_err = assert_err;
    rom_env.assert_param = assert_param;
    rom_env.assert_warn = assert_warn;
#endif
    //rom_env.Read_Uart_Buf = read_uart_ringbuf_data;
    rom_env.uart_clear_rxfifo = uart_clear_rxfifo;
    rom_env.Read_Uart_Buf = Read_Uart_Buf;
}


