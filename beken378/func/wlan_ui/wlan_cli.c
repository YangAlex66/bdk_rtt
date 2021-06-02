/**
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 BEKEN Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of BEKEN Corporation.
 *
 */
#include "sys_rtos.h"
#include "rtos_pub.h"
#include "error.h"
#include "wlan_cli_pub.h"
#include "stdarg.h"
#include "include.h"
#include "mem_pub.h"
#include "str_pub.h"
#include "uart_pub.h"
#include "BkDriverFlash.h"
#include "wlan_ui_pub.h"
#include "param_config.h"
#include "gpio_pub.h"
#include "sys_ctrl_pub.h"
#include "icu_pub.h"
#include "power_save_pub.h"
#include "cmd_rx_sensitivity.h"
#include "cmd_evm.h"
#include "BkDriverGpio.h"
#include "ieee802_11_demo.h"
#include "command_line.h"
#include "role_launch.h"
#include "wdt_pub.h"
#include "saradc_pub.h"
#include "bk7011_cal_pub.h"
#include "flash_pub.h"
#include "mcu_ps_pub.h"
#include "manual_ps_pub.h"
#include "phy_trident.h"
#include "BkDriverPwm.h"
#include "lwip/ping.h"
#include "ble_pub.h"
#include "sensor.h"
#include "spi_pub.h"
#include "i2c_pub.h"
#include "BkDriverTimer.h"
#include "saradc_intf.h"
#include "spi_pub.h"
#if CFG_BK_AWARE
#include "bk_aware.h"
#endif

#if (CFG_SUPPORT_BLE == 1)
#if (CFG_BLE_VERSION == BLE_VERSION_4_2)
#include "application.h"
#endif

#if (CFG_BLE_VERSION == BLE_VERSION_5_x)
#include "app_ble_task.h"
#endif
#endif

#if (CFG_SOC_NAME == SOC_BK7221U)
#include "security_pub.h"
extern void sec_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#endif

#include "temp_detect_pub.h"

#ifdef monitor_printf_debug
#define monitor_dbg(fmt, ...)   bk_printf(fmt, ##__VA_ARGS__)
#else
#define monitor_dbg(fmt, ...)
#endif

#ifndef MOC
static void task_Command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );
#endif

int mico_debug_enabled;
static struct cli_st *pCli = NULL;
beken_semaphore_t log_rx_interrupt_sema = NULL;

extern u8* wpas_get_sta_psk(void);
extern int cli_putstr(const char *msg);
extern int hexstr2bin(const char *hex, u8 *buf, size_t len);
extern void make_tcp_server_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
extern void net_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
uint32_t bk_wlan_reg_rx_mgmt_cb(mgmt_rx_cb_t cb, uint32_t rx_mgmt_flag);

#if CFG_AIRKISS_TEST
extern u32 airkiss_process(u8 start);
extern uint32_t airkiss_is_at_its_context(void);
#endif


#if CFG_SARADC_CALIBRATE
static void adc_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#endif

static void cli_rx_callback(int uport, void *param);

static void efuse_cmd_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void efuse_mac_cmd_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);

#if CFG_SUPPORT_BKREG
#define BKREG_MAGIC_WORD0                 (0x01)
#define BKREG_MAGIC_WORD1                 (0xE0)
#define BKREG_MAGIC_WORD2                 (0xFC)
#define BKREG_MIN_LEN                     3
#endif

/* Find the command 'name' in the cli commands table.
* If len is 0 then full match will be performed else upto len bytes.
* Returns: a pointer to the corresponding cli_command struct or NULL.
*/
static const struct cli_command *lookup_command(char *name, int len)
{
    int i = 0;
    int n = 0;

    while (i < MAX_COMMANDS && n < pCli->num_commands)
    {
        if (pCli->commands[i]->name == NULL)
        {
            i++;
            continue;
        }
        /* See if partial or full match is expected */
        if (len != 0)
        {
            if (!os_strncmp(pCli->commands[i]->name, name, len))
                return pCli->commands[i];
        }
        else
        {
            if (!os_strcmp(pCli->commands[i]->name, name))
                return pCli->commands[i];
        }

        i++;
        n++;
    }

    return NULL;
}

/* Parse input line and locate arguments (if any), keeping count of the number
* of arguments and their locations.  Look up and call the corresponding cli
* function if one is found and pass it the argv array.
*
* Returns: 0 on success: the input line contained at least a function name and
*          that function exists and was called.
*          1 on lookup failure: there is no corresponding function for the
*          input line.
*          2 on invalid syntax: the arguments list couldn't be parsed
*/
static int handle_input(char *inbuf)
{
    struct
    {
        unsigned inArg: 1;
        unsigned inQuote: 1;
        unsigned done: 1;
    } stat;
    static char *argv[16];
    int argc = 0;
    int i = 0;
    const struct cli_command *command = NULL;
    const char *p;

    os_memset((void *)&argv, 0, sizeof(argv));
    os_memset(&stat, 0, sizeof(stat));

    do
    {
        switch (inbuf[i])
        {
        case '\0':
            if (stat.inQuote)
                return 2;
            stat.done = 1;
            break;

        case '"':
            if (i > 0 && inbuf[i - 1] == '\\' && stat.inArg)
            {
                os_memcpy(&inbuf[i - 1], &inbuf[i],
                          os_strlen(&inbuf[i]) + 1);
                --i;
                break;
            }
            if (!stat.inQuote && stat.inArg)
                break;
            if (stat.inQuote && !stat.inArg)
                return 2;

            if (!stat.inQuote && !stat.inArg)
            {
                stat.inArg = 1;
                stat.inQuote = 1;
                argc++;
                argv[argc - 1] = &inbuf[i + 1];
            }
            else if (stat.inQuote && stat.inArg)
            {
                stat.inArg = 0;
                stat.inQuote = 0;
                inbuf[i] = '\0';
            }
            break;

        case ' ':
            if (i > 0 && inbuf[i - 1] == '\\' && stat.inArg)
            {
                os_memcpy(&inbuf[i - 1], &inbuf[i],
                          os_strlen(&inbuf[i]) + 1);
                --i;
                break;
            }
            if (!stat.inQuote && stat.inArg)
            {
                stat.inArg = 0;
                inbuf[i] = '\0';
            }
            break;

        default:
            if (!stat.inArg)
            {
                stat.inArg = 1;
                argc++;
                argv[argc - 1] = &inbuf[i];
            }
            break;
        }
    }
    while (!stat.done && ++i < INBUF_SIZE);

    if (stat.inQuote)
        return 2;

    if (argc < 1)
        return 0;

    if (!pCli->echo_disabled)
        os_printf("\r\n");

    /*
    * Some comamands can allow extensions like foo.a, foo.b and hence
    * compare commands before first dot.
    */
    i = ((p = os_strchr(argv[0], '.')) == NULL) ? 0 :
        (p - argv[0]);
    command = lookup_command(argv[0], i);
    if (command == NULL)
        return 1;

    os_memset(pCli->outbuf, 0, OUTBUF_SIZE);
    cli_putstr("\r\n");

    #if CFG_USE_STA_PS
    /*if cmd,exit dtim ps*/
    if (os_strncmp(command->name, "ps", 2))
    {
    }
    #endif

    command->function(pCli->outbuf, OUTBUF_SIZE, argc, argv);
    cli_putstr(pCli->outbuf);
    return 0;
}

/* Perform basic tab-completion on the input buffer by string-matching the
* current input line against the cli functions table.  The current input line
* is assumed to be NULL-terminated. */
static void tab_complete(char *inbuf, unsigned int *bp)
{
    int i, n, m;
    const char *fm = NULL;

    os_printf("\r\n");

    /* show matching commands */
    for (i = 0, n = 0, m = 0; i < MAX_COMMANDS && n < pCli->num_commands;
            i++)
    {
        if (pCli->commands[i]->name != NULL)
        {
            if (!os_strncmp(inbuf, pCli->commands[i]->name, *bp))
            {
                m++;
                if (m == 1)
                    fm = pCli->commands[i]->name;
                else if (m == 2)
                    os_printf("%s %s ", fm,
                              pCli->commands[i]->name);
                else
                    os_printf("%s ",
                              pCli->commands[i]->name);
            }
            n++;
        }
    }

    /* there's only one match, so complete the line */
    if (m == 1 && fm)
    {
        n = os_strlen(fm) - *bp;
        if (*bp + n < INBUF_SIZE)
        {
            os_memcpy(inbuf + *bp, fm + *bp, n);
            *bp += n;
            inbuf[(*bp)++] = ' ';
            inbuf[*bp] = '\0';
        }
    }

    /* just redraw input line */
    os_printf("%s%s", PROMPT, inbuf);
}

/* Get an input line.
*
* Returns: 1 if there is input, 0 if the line should be ignored. */
static int get_input(char *inbuf, unsigned int *bp)
{
	if (inbuf == NULL){
		os_printf("inbuf_null\r\n");
		return 0;
	}

	while (cli_getchar(&inbuf[*bp]) == 1)
	{
#if CFG_SUPPORT_BKREG
		if ((0x01U == (UINT8)inbuf[*bp]) && (*bp == 0)) {
			(*bp)++;
			continue;
		} else if ((0xe0U == (UINT8)inbuf[*bp]) && (*bp == 1)) {
			(*bp)++;
			continue;
		} else if ((0xfcU == (UINT8)inbuf[*bp]) && (*bp == 2)) {
			(*bp)++;
			continue;
		} else {
			if ((0x01U == (UINT8)inbuf[0])
				&& (0xe0U == (UINT8)inbuf[1])
				&& (0xfcU == (UINT8)inbuf[2])
				&& (*bp == 3)) {
				uint8_t ch = inbuf[*bp];
				uint8_t left = ch, len = 4 + (uint8_t)ch;
				inbuf[*bp] = ch;
				(*bp)++;

				if (ch >= INBUF_SIZE) {
					os_printf("Error: input buffer overflow\r\n");
					os_printf(PROMPT);
					*bp = 0;
					return 0;
				}

				while (left--) {
					if (0 == cli_getchar((char*)&ch))
						break;

					inbuf[*bp] = ch;
					(*bp)++;
				}

				extern int bkreg_run_command(const char *content, int cnt);
				bkreg_run_command(inbuf, len);
				memset(inbuf, 0, len);
				*bp = 0;
				continue;
			}
		}
#endif
		if (inbuf[*bp] == RET_CHAR)
			continue;
		if (inbuf[*bp] == END_CHAR) {   /* end of input line */
			inbuf[*bp] = '\0';
			*bp = 0;
			return 1;
		}

		if ((inbuf[*bp] == 0x08) || /* backspace */
			(inbuf[*bp] == 0x7f)) { /* DEL */
			if (*bp > 0) {
				(*bp)--;
				if (!pCli->echo_disabled)
					os_printf("%c %c", 0x08, 0x08);
			}
			continue;
		}

		if (inbuf[*bp] == '\t') {
			inbuf[*bp] = '\0';
			tab_complete(inbuf, bp);
			continue;
		}

		if (!pCli->echo_disabled)
			os_printf("%c", inbuf[*bp]);

		(*bp)++;
		if (*bp >= INBUF_SIZE) {
			os_printf("Error: input buffer overflow\r\n");
			os_printf(PROMPT);
			*bp = 0;
			return 0;
		}

	}

	return 0;
}



/* Print out a bad command string, including a hex
* representation of non-printable characters.
* Non-printable characters show as "\0xXX".
*/
static void print_bad_command(char *cmd_string)
{
    if (cmd_string != NULL)
    {
        char *c = cmd_string;
        os_printf("command '");
        while (*c != '\0')
        {
            if (is_print(*c))
            {
                os_printf("%c", *c);
            }
            else
            {
                os_printf("\\0x%x", *c);
            }
            ++c;
        }
        os_printf("' not found\r\n");
    }
}

/* Main CLI processing thread
*
* Waits to receive a command buffer pointer from an input collector, and
* then processes.  Note that it must cleanup the buffer when done with it.
*
* Input collectors handle their own lexical analysis and must pass complete
* command lines to CLI.
*/
static void cli_main( uint32_t data )
{
    bk_uart_set_rx_callback(CLI_UART, cli_rx_callback, NULL);

	#if CFG_RF_OTA_TEST
	demo_sta_app_init("CMW-AP", "12345678");
	#endif /* CFG_RF_OTA_TEST*/

    while (1)
    {
        int ret;
        char *msg = NULL;

        rtos_get_semaphore(&log_rx_interrupt_sema, BEKEN_NEVER_TIMEOUT);

        if(get_input(pCli->inbuf, &pCli->bp))
        {
            msg = pCli->inbuf;

            if (os_strcmp(msg, EXIT_MSG) == 0)
                break;

            ret = handle_input(msg);
            if (ret == 1)
                print_bad_command(msg);
            else if (ret == 2)
                os_printf("syntax error\r\n");

            os_printf(PROMPT);
        }
    }

    os_printf("CLI exited\r\n");
    os_free(pCli);
    pCli = NULL;

    bk_uart_set_rx_callback(CLI_UART, NULL, NULL);
    rtos_delete_thread(NULL);
}

#ifndef MOC
static void task_Command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    rtos_print_thread_status( pcWriteBuffer, xWriteBufferLen );
}
#endif

void tftp_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
}

static void partShow_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    bk_partition_t i;
    bk_logic_partition_t *partition;

    for( i = BK_PARTITION_BOOTLOADER; i <= BK_PARTITION_MAX; i++ )
    {
        partition = bk_flash_get_info( i );
        if (partition == NULL)
            continue;

        os_printf( "%4d | %11s |  Dev:%d  | 0x%08lx | 0x%08lx |\r\n", i,
                   partition->partition_description, partition->partition_owner,
                   partition->partition_start_addr, partition->partition_length);
    };

}

static void uptime_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("UP time %ldms\r\n", rtos_get_time());
}

void tftp_ota_thread( beken_thread_arg_t arg )
{
    rtos_delete_thread( NULL );
}

void ota_Command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    rtos_create_thread( NULL,
                        BEKEN_APPLICATION_PRIORITY,
                        "LOCAL OTA",
                        (beken_thread_function_t)tftp_ota_thread,
                        0x4096,
                        0 );
}

void help_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);

/*
*  Command buffer API
*/
void wifiscan_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
#if CFG_ROLE_LAUNCH
    LAUNCH_REQ param;

    param.req_type = LAUNCH_REQ_PURE_STA_SCAN;
    rl_sta_request_enter(&param, 0);
#else
    demo_scan_app_init();
#endif
}

void wifiadvscan_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t *ap_ssid;

    if ( argc < 2 )
    {
        os_printf("Please input ssid\r\n");
        return;
    }
    ap_ssid = (uint8_t*)argv[1];

    demo_scan_adv_app_init(ap_ssid);
}

void softap_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    char *ap_ssid = NULL;
    char *ap_key;

    os_printf("SOFTAP_COMMAND\r\n\r\n");
    if (argc == 2)
    {
        ap_ssid = argv[1];
        ap_key = "1";
    }
    else if (argc == 3)
    {
        ap_ssid = argv[1];
        ap_key = argv[2];
    }

    if(ap_ssid)
    {
        demo_softap_app_init(ap_ssid, ap_key);
    }
}

void stop_wlan_intface_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t inface = 0;

    os_printf("stop_wlan_intface_Command\r\n");
    if (argc == 2)
    {
        inface = strtoul(argv[1], NULL, 0);
        os_printf("stop wlan intface:%d\r\n", inface);

        if(inface == 0)
        {
            bk_wlan_stop(BK_SOFT_AP);
        }
        else if(inface == 1)
        {
            bk_wlan_stop(BK_STATION);
        }
    }
    else
    {
        os_printf("bad parameters\r\n");
    }
}

void add_virtual_intface(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    VIF_ADDCFG_ST cfg;
    u8 argc_cnt = 1;

    if(argc <= 1)
        return;

    if (!os_strncmp(argv[argc_cnt], "softap", sizeof("softap")))
    {
        cfg.wlan_role = BK_SOFT_AP;
    }
    else if (!os_strncmp(argv[argc_cnt], "sta", sizeof("sta")))
    {
        cfg.wlan_role = BK_STATION;
    }
    else
    {
        os_printf("role error:%s, must be softap or sta\r\n", argv[argc_cnt]);
        return;
    }
    argc_cnt++;

    if(argc == argc_cnt)
        cfg.ssid = "default ssid";
    else
        cfg.ssid = argv[argc_cnt];
    argc_cnt++;

    if(argc == argc_cnt)
        cfg.key = "1234567890";
    else
        cfg.key = argv[argc_cnt];
    argc_cnt++;

    if((cfg.wlan_role == BK_STATION) && (argc > argc_cnt))
    {
        if (!os_strncmp(argv[argc_cnt], "adv", sizeof("adv")))
            cfg.adv = 1;
        else
            cfg.adv = 0;
    }
    else
    {
        cfg.adv = 0;
    }
    argc_cnt++;

    cfg.name = NULL;

    os_printf("role: %d\r\n", cfg.wlan_role);
    os_printf("ssid: %s\r\n", cfg.ssid);
    os_printf("key : %s\r\n", cfg.key);
    os_printf("adv : %d\r\n", cfg.adv);

    demo_wlan_app_init(&cfg);
}

void del_virtual_intface(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    char *type;
    u8 role = 0xff;

	#if CFG_ROLE_LAUNCH
    LAUNCH_REQ param;
	#endif

    if(argc <= 1)
        return;

    type = argv[1];

    if (!os_strncmp(type, "softap", sizeof("softap")))
    {
        role = BK_SOFT_AP;

		#if CFG_ROLE_LAUNCH
        param.req_type = LAUNCH_REQ_DELIF_AP;

		rl_ap_request_enter(&param, 0);
		#endif
    }
    else if(!os_strncmp(type, "sta", sizeof("sta")))
    {
        role = BK_STATION;

		#if CFG_ROLE_LAUNCH
        param.req_type = LAUNCH_REQ_DELIF_STA;

		rl_sta_request_enter(&param, 0);
		#endif
    }

    if(role == 0xff)
    {
        os_printf("%s virtual intface not found\r\n", type );
        return;
    }

#if (0 == CFG_ROLE_LAUNCH)
    bk_wlan_stop(role);
#endif
}

void show_virtual_intface(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    u8 i = 1;
    u8 *mac_ptr;
    char *role;
    const char *name;
    struct netif *lwip_if;
    VIF_INF_PTR vif_entry;

    os_printf("\r\nshow_virtual_intface:\r\n");
    vif_entry = (VIF_INF_PTR)rwm_mgmt_is_vif_first_used();
    while(vif_entry)
    {
        mac_ptr = (u8 *)&vif_entry->mac_addr;
        lwip_if = (struct netif *)vif_entry->priv;
        name = lwip_if->hostname;
        if(vif_entry->type == VIF_AP)
        {
            role = "softap\0";
        }
        else if(vif_entry->type == VIF_STA)
        {
            role = "sta\0";
        }
        else
        {
            role = "others\0";
        }

        os_printf("%1d: %s, %s, %02x:%02x:%02x:%02x:%02x:%02x\r\n", i, name, role,
                  mac_ptr[0], mac_ptr[1], mac_ptr[2], mac_ptr[3], mac_ptr[4], mac_ptr[5]);

        vif_entry = (VIF_INF_PTR)rwm_mgmt_next(vif_entry);
        i++;
    }

    os_printf("\r\nend of show\r\n");

}
#if 1
uint32_t channel_count = 0;
void cli_monitor_cb(uint8_t *data, int len, wifi_link_info_t *info)
{
    uint32_t count, i;

    count = MIN(32, len);
    monitor_dbg("cli_monitor_cb:%d:%d\r\n", count, len);
    for(i = 0; i < count; i ++)
    {
        monitor_dbg("%x ", data[i]);
    }
    monitor_dbg("\r\n");

    channel_count ++;
}

xTaskHandle mtr_thread_handle = NULL;
xTaskHandle cli_thread_handle = NULL;

uint32_t  mtr_stack_size = 2000;
#define THD_MTR_PRIORITY                    5

void mtr_thread_main( void *arg )
{
    static uint32_t channel_num = 1;

    // stop monitor mode, need stop hal mac first
    bk_wlan_stop_monitor();
    // then set monitor callback
    bk_wlan_register_monitor_cb(NULL);

    // start monitor, need set callback
    bk_wlan_register_monitor_cb(cli_monitor_cb);
    // then start hal mac
    bk_wlan_start_monitor();

    while(1)
    {
        channel_count = 0;

        bk_wlan_set_channel_sync(channel_num);
        channel_num ++;

        if(14 == channel_num)
        {
            channel_num = 1;
        }

        rtos_delay_milliseconds(100);
        os_printf("channel:%d count:%x\r\n", channel_num, channel_count);
    }
}
#endif

void mtr_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint32_t channel_num;

    if(argc != 2)
    {
        os_printf("monitor_parameter invalid\r\n");
        return;
    }

    channel_num = os_strtoul(argv[1], NULL, 10);
    if(99 == channel_num)
    {
        cmd_printf("stop monitor\r\n");
        bk_wlan_stop_monitor();
    }
    else
    {
        bk_wlan_register_monitor_cb(cli_monitor_cb);
        bk_wlan_start_monitor();
        bk_wlan_set_channel_sync(channel_num);
    }
}

void sta_adv_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    char *oob_ssid = NULL;
    char *connect_key;

    os_printf("sta_adv_Command\r\n");
    if (argc == 2)
    {
        oob_ssid = argv[1];
        connect_key = "1";
    }
    else if (argc == 3)
    {
        oob_ssid = argv[1];
        connect_key = argv[2];
    }
    else
    {
        os_printf("parameter invalid\r\n");
        return;
    }

    if(oob_ssid)
    {
        demo_sta_adv_app_init(oob_ssid, connect_key);
    }
}

void show_sta_psk(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	uint8_t *psk;
	int i;

	psk = wpas_get_sta_psk();
	os_printf("John# show_sta_psk.r\n");
	for ( i = 0 ; i < 32 ; i++ )
	{
	    os_printf("%02x ", psk[i]);
	}
	os_printf("\r\n");
}

#include "conv_utf8_pub.h"
void sta_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    char *oob_ssid = NULL;
    char *connect_key;

    os_printf("sta_Command\r\n");
    if (argc == 2)
    {
        oob_ssid = argv[1];
        connect_key = "1";
    }
    else if (argc == 3)
    {
        oob_ssid = argv[1];
        connect_key = argv[2];
    }
    else
    {
        os_printf("parameter invalid\r\n");
    }

	if (oob_ssid)
    {
        unsigned char *oob_ssid_tp = conv_utf8((uint8_t*)oob_ssid);

		if (oob_ssid_tp)
		{
#if CFG_AIRKISS_TEST
			if (airkiss_is_at_its_context()) {
				os_printf("airkiss is on-the-go\r\n");
				return;
			}
#endif

			demo_sta_app_init((char *)oob_ssid_tp, connect_key);
			os_free(oob_ssid_tp);
		}
        else
        {
            os_printf("not buf for utf8\r\n");
        }
    }
}

#if CFG_WPA_CTRL_IFACE
void wifi_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    char *oob_ssid = NULL;
    char *connect_key;

    os_printf("wifi_Command\r\n");
    if (argc == 2)
    {
        oob_ssid = argv[1];
        connect_key = "1";
    }
    else if (argc == 3)
    {
        oob_ssid = argv[1];
        connect_key = argv[2];
    }
    else
    {
        os_printf("parameter invalid\r\n");
    }

    if(oob_ssid)
    {
        demo_sta_app_init(oob_ssid, connect_key);
    }
}
#endif

void easylink_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
}

#if CFG_AIRKISS_TEST
void airkiss_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    u8 start = 0;

    if(argc != 2)
    {
        os_printf("need 2 parameters: airkiss 1(start), 0(stop)\r\n");
        return;
    }

    start = strtoul(argv[1], NULL, 0);

    airkiss_process(start);
}

#endif

#if CFG_USE_TEMPERATURE_DETECT
void temp_detect_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    u8 start = 0;

    if(argc != 2)
    {
        os_printf("need 2 parameters: airkiss 1(start), 0(stop)\r\n");
        return;
    }

    start = strtoul(argv[1], NULL, 0);

    if(start == 0)
        temp_detect_pause_timer();
    else
        temp_detect_restart_detect();
}
#endif

void wifistate_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("wifistate_Command\r\n");
    demo_state_app_init();
}

void wifidebug_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("wifidebug_Command\r\n");
}

void ifconfig_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    demo_ip_app_init();
}

void arp_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("arp_Command\r\n");
}

void ping_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("ping_Command\r\n");

    if (argc == 1)
    {
        os_printf("Please input: ping <host address>\n");
    }
    else
    {
		os_printf("ping IP address:%s\n",argv[1]);
		ping(argv[1], 4, 0);
	}
}

void dns_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("dns_Command\r\n");
}

void socket_show_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("socket_show_Command\r\n");
}

void memory_show_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    cmd_printf("free memory %d\r\n", xPortGetFreeHeapSize());
}


void memory_dump_Command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    int i;
    uint8_t *pstart;
    uint32_t start, length;

    if (argc != 3)
    {
        cmd_printf("Usage: memdump <addr> <length>.\r\n");
        return;
    }

    start = strtoul(argv[1], NULL, 0);
    length = strtoul(argv[2], NULL, 0);
    pstart = (uint8_t *)start;

    for(i = 0; i < length; i++)
    {
        cmd_printf("%02x ", pstart[i]);
        if (i % 0x10 == 0x0F)
        {
            cmd_printf("\r\n");

        }
    }
}


void memory_set_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("memory_set_Command\r\n");
}

void driver_state_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("driver_state_Command\r\n");
}

void get_version(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("get_version\r\n");
    os_printf("firmware version : %s", BEKEN_SDK_REV);
}

void reboot(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    bk_reboot();
}

static void echo_cmd_handler(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc == 1)
    {
        os_printf("Usage: echo on/off. Echo is currently %s\r\n",
                  pCli->echo_disabled ? "Disabled" : "Enabled");
        return;
    }

    if (!os_strcasecmp(argv[1], "on"))
    {
        os_printf("Enable echo\r\n");
        pCli->echo_disabled = 0;
    }
    else if (!os_strcasecmp(argv[1], "off"))
    {
        os_printf("Disable echo\r\n");
        pCli->echo_disabled = 1;
    }
}

static void cli_exit_handler(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    // exit command not executed
}

/*
CMD FORMAT: GPIO CMD index PARAM
exmaple:GPIO 0 18 2               (config GPIO18 input & pull-up)
*/
static void Gpio_op_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint32_t ret, id, mode, i;
    char cmd0 = 0;
    char cmd1 = 0;
    char cmd;

    for(i = 0; i < argc; i++)
    {
        os_printf("Argument %d = %s\r\n", i + 1, argv[i]);
    }

    if(argc == 4)
    {
        cmd = argv[1][0];
        mode = argv[3][0];

        cmd0 = argv[2][0] - 0x30;
        cmd1 = argv[2][1] - 0x30;

        id = (uint32_t)(cmd0 * 10 + cmd1);
        os_printf("---%x,%x----\r\n", id, mode);
        ret = BKGpioOp(cmd, id, mode);
        os_printf("gpio op:%x\r\n", ret);
    }
    else
        os_printf("cmd param error\r\n");
}

void test_fun(char para)
{
    os_printf("---%d---\r\n", para);
}
/*
cmd format: GPIO_INT cmd index  triggermode
enable: GPIO_INT 1 18 0
*/
static void Gpio_int_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint32_t id, mode;
    char cmd0 = 0;
    char cmd1 = 0;
    char cmd;

    if(argc == 4)
    {
        cmd = argv[1][0] - 0x30;
        mode = argv[3][0] - 0x30;

        cmd0 = argv[2][0] - 0x30;
        cmd1 = argv[2][1] - 0x30;

        id = (uint32_t)(cmd0 * 10 + cmd1);
        BKGpioIntcEn(cmd, id, mode, test_fun);
    }
    else
        os_printf("cmd param error\r\n");

}


#if CFG_USE_SDCARD_HOST
/*
sdtest I 0 --
sdtest R secnum
sdtest W secnum
*/
extern uint32_t sdcard_intf_test(void);
extern uint32_t test_sdcard_read(uint32_t blk);
extern uint32_t test_sdcard_write(uint32_t blk);
extern void sdcard_intf_close(void);

static void sd_operate(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint32_t cmd;
    uint32_t blknum;
    uint32_t ret;
    if(argc == 3)
    {
        cmd = argv[1][0];
        blknum = os_strtoul(argv[2], NULL, 16);
        switch(cmd)
        {
        case 'I':
            ret = sdcard_intf_test();
            os_printf("init :%x\r\n", ret);
            break;
        case 'R':
            ret = test_sdcard_read(blknum);
            os_printf("read :%x\r\n", ret);
            break;
        case 'W':
            ret = test_sdcard_write(blknum);
            os_printf("write :%x\r\n", ret);
            break;
        case 'C':
            sdcard_intf_close();
            os_printf("close \r\n");
            break;
        default:
            break;
        }
    }
    else
        os_printf("cmd param error\r\n");
}

#endif
/*
format: FLASH  E/R/W  0xABCD
example:	    FLASH  R  0x00100

*/

extern OSStatus test_flash_write(volatile uint32_t start_addr, uint32_t len);
extern OSStatus test_flash_erase(volatile uint32_t start_addr, uint32_t len);
extern OSStatus test_flash_read(volatile uint32_t start_addr, uint32_t len);
extern OSStatus test_flash_read_time(volatile uint32_t start_addr, uint32_t len);

static void flash_command_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    char cmd = 0;
    uint32_t len = 0;
    uint32_t addr = 0;

    if(argc == 4)
    {
        cmd = argv[1][0];
		addr = atoi(argv[2]);
		len = atoi(argv[3]);

       	switch(cmd)
        {
	        case 'E':
				bk_flash_enable_security(FLASH_PROTECT_NONE);
				test_flash_erase(addr,len);
				bk_flash_enable_security(FLASH_UNPROTECT_LAST_BLOCK);
	         break;

        	case 'R':
				test_flash_read(addr,len);
	            break;
	        case 'W':
		        bk_flash_enable_security(FLASH_PROTECT_NONE);
				test_flash_write(addr,len);
				bk_flash_enable_security(FLASH_UNPROTECT_LAST_BLOCK);
	         break;
	//to check whether protection mechanism can work
			case 'N':
				test_flash_erase(addr,len);
				break;
			case 'M':
				test_flash_write(addr,len);
				break;
            case 'T':
				test_flash_read_time(addr,len);
				break;
        	default:
            break;
        }
    }
	else
	{
		os_printf("FLASH <R/W/E/M/N/T> <start_addr> <len>\r\n");
	}
}

/*UART  I  index
example:   UART I 0
*/
static void Uart_command_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    char cmd;
    uint8_t index = 0;
    if(argc == 3)
    {
        cmd = argv[1][0];
        index = argv[2][0] - 0x30;

        if(cmd == 'I')
        {
            bk_uart_initialize_test(0, index, NULL);
        }
    }
    else
        os_printf("cmd param error\r\n");

}
static void tx_evm_cmd_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int ret = do_evm(NULL, 0, argc, argv);
    if(ret)
    {
        os_printf("tx_evm bad parameters\r\n");
    }
}

static void rx_sens_cmd_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int ret = do_rx_sensitivity(NULL, 0, argc, argv);
    if(ret)
    {
        os_printf("rx sensitivity bad parameters\r\n");
    }
}

#if (CFG_SOC_NAME != SOC_BK7231)
static void efuse_cmd_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t addr, data;

    if(argc == 3)
    {
        if (os_strncmp(argv[1], "-r", 2) == 0) {
            hexstr2bin(argv[2], &addr, 1);
            os_printf("efuse read: addr-0x%02x, data-0x%02x\r\n",
                        addr, wifi_read_efuse(addr));
        }
    }
    else if(argc == 4)
    {
        if(os_strncmp(argv[1], "-w", 2) == 0)  {
            hexstr2bin(argv[2], &addr, 1);
            hexstr2bin(argv[3], &data, 6);
            os_printf("efuse write: addr-0x%02x, data-0x%02x, ret:%d\r\n",
                        addr, data, wifi_write_efuse(addr, data));
        }
    }
    else {
        os_printf("efuse [-r addr] [-w addr data]\r\n");
    }
}

static void efuse_mac_cmd_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t mac[6];

    if (argc == 1)
    {
        if(wifi_get_mac_address_from_efuse(mac))
            os_printf("MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    else if(argc == 2)
    {
        if (os_strncmp(argv[1], "-r", 2) == 0) {
            if(wifi_get_mac_address_from_efuse(mac))
                os_printf("MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        }
    }
    else if(argc == 3)
    {
        if(os_strncmp(argv[1], "-w", 2) == 0)  {
            hexstr2bin(argv[2], mac, 6);
            //if(wifi_set_mac_address_to_efuse(mac))
                os_printf("Set MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        }
    }
    else {
        os_printf("efusemac [-r] [-w] [mac]\r\n");
    }
}
#endif //(CFG_SOC_NAME != SOC_BK7231)


#if CFG_SUPPORT_BKREG
#define BKCMD_RXSENS_R      'r'
#define BKCMD_RXSENS_X      'x'
#define BKCMD_RXSENS_s      's'

#define BKCMD_TXEVM_T       't'
#define BKCMD_TXEVM_X       'x'
#define BKCMD_TXEVM_E       'e'

void bkreg_cmd_handle_input(char *inbuf, int len)
{
    if(((char)BKREG_MAGIC_WORD0 == inbuf[0])
            && ((char)BKREG_MAGIC_WORD1 == inbuf[1])
            && ((char)BKREG_MAGIC_WORD2 == inbuf[2]))
    {
        if(cli_getchars(inbuf, len))
        {
            bkreg_run_command(inbuf, len);
            os_memset(inbuf, 0, len);
        }
    }
    else if((((char)BKCMD_RXSENS_R == inbuf[0])
             && ((char)BKCMD_RXSENS_X == inbuf[1])
             && ((char)BKCMD_RXSENS_s == inbuf[2]))
            || (((char)BKCMD_TXEVM_T == inbuf[0])
                && ((char)BKCMD_TXEVM_X == inbuf[1])
                && ((char)BKCMD_TXEVM_E == inbuf[2])) )
    {
        if(cli_getchars(inbuf, len))
        {
            handle_input(inbuf);
            os_memset(inbuf, 0, len);
        }
    }

}
#endif

static void phy_cca_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	if(argc != 2)
	{
		os_printf("cca open\r\n");
		os_printf("cca close\r\n");
		os_printf("cca show\r\n");
		return;
	}

	if (os_strncmp(argv[1], "open", 4) == 0) {
		phy_open_cca();
		os_printf("cca opened\r\n");
	} else if (os_strncmp(argv[1], "close", 4) == 0) {
		phy_close_cca();
		os_printf("cca closed\r\n");
	} else if (os_strncmp(argv[1], "show", 4) == 0) {
		phy_show_cca();
	} else {
		os_printf("cca open\r\n");
		os_printf("cca close\r\n");
		os_printf("cca show\r\n");
	}
}

#if CFG_SUPPORT_OTA_TFTP
extern void tftp_start(void);
extern void string_to_ip(char *s);
static void tftp_ota_get_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    short len = 0,i;
    extern char		BootFile[] ;

    if(argc > 3 )
        {
        os_printf("ota server_ip ota_file\r\n");
        return;
    }

     os_printf("%s\r\n",argv[1]);

    os_strcpy(BootFile,argv[2]);
    os_printf("%s\r\n",BootFile);
    string_to_ip (argv[1]);


    tftp_start();

    return;

}
#endif

#if CFG_SUPPORT_OTA_HTTP
void http_ota_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int ret;
    if ( argc != 2 ) {
        goto HTTP_CMD_ERR;
    }
    ret = http_ota_download(argv[1]);

    if (0 != ret) {
        os_printf("http_ota download failed.");
    }

    return;

HTTP_CMD_ERR:
    os_printf("Usage:http_ota [url:]\r\n");
}
#endif

static void reg_write_read_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    UINT32 reg_addr = 0, reg_value = 0;
    UINT8 optr_len = 0, optr_tab[9];

    os_memset(optr_tab, 0, 9);
    os_memset(optr_tab, 0x30, 8);

    if(os_strncmp(argv[1], "-r", 2) == 0)
    {
        if(argc != 3)
        {
            os_printf("regshow -r addr\r\n");
            return;
        }

        optr_len = os_strlen(argv[2]);
        if(optr_len > 8)
        {
            os_printf("addr 0-FFFFFFFF\r\n");
            return;
        }
        optr_len = 8 - optr_len;
        os_memcpy(&optr_tab[optr_len], argv[2], os_strlen(argv[2]));

        hexstr2bin((char*)optr_tab, (u8 *)&reg_addr, 4);
        reg_addr = ntohl(reg_addr);
        os_printf("regshow R: addr:0x%08x, value:0x%08x\r\n", reg_addr, REG_READ(reg_addr));
    }
    else if(os_strncmp(argv[1], "-w", 2) == 0)
    {
        if(argc != 4)
        {
            os_printf("regshow -w addr value\r\n");
            return;
        }

        optr_len = os_strlen(argv[2]);
        if(optr_len > 8)
        {
            os_printf("addr 0-FFFFFFFF\r\n");
            return;
        }
        optr_len = 8 - optr_len;
        os_memcpy(&optr_tab[optr_len], argv[2], os_strlen(argv[2]));

        hexstr2bin((char*)optr_tab, (u8 *)&reg_addr, 4);
        reg_addr = ntohl(reg_addr);


        os_memset(optr_tab, 0x30, 8);
        optr_len = os_strlen(argv[3]);
        if(optr_len > 8)
        {
            os_printf("value 0-FFFFFFFF\r\n");
            return;
        }
        optr_len = 8 - optr_len;
        os_memcpy(&optr_tab[optr_len], argv[3], os_strlen(argv[3]));
        hexstr2bin((char*)optr_tab, (u8 *)&reg_value, 4);
        reg_value = ntohl(reg_value);

        REG_WRITE(reg_addr, reg_value);

        extern INT32 rwnx_cal_save_trx_rcbekn_reg_val(void);
        // when write trx and rc beken regs, updata registers save.
        if( (reg_addr & 0xfff0000) == 0x1050000)
            rwnx_cal_save_trx_rcbekn_reg_val();

        os_printf("regshow W: addr:0x%08x, value:0x%08x - check:0x%08x\r\n",
            reg_addr, reg_value, REG_READ(reg_addr));
    }
    else
    {
        os_printf("regshow -w/r addr [value]\r\n");
    }
}

#if ((CFG_SOC_NAME != SOC_BK7231) && (CFG_SUPPORT_BLE == 1))
#if (CFG_BLE_VERSION == BLE_VERSION_4_2)
#include "ble_api.h"
#elif (CFG_BLE_VERSION == BLE_VERSION_5_x)
#include "ble_api_5_x.h"
#endif

#define BUILD_UINT16(loByte, hiByte) \
          ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define BK_ATT_DECL_PRIMARY_SERVICE_128     {0x00,0x28,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_DECL_CHARACTERISTIC_128      {0x03,0x28,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_DESC_CLIENT_CHAR_CFG_128     {0x02,0x29,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

#define WRITE_REQ_CHARACTERISTIC_128        {0x01,0xFF,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0}
#define INDICATE_CHARACTERISTIC_128         {0x02,0xFF,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0}
#define NOTIFY_CHARACTERISTIC_128           {0x03,0xFF,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0}

static const uint8_t test_svc_uuid[16] = {0xFF,0xFF,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0};

enum
{
	TEST_IDX_SVC,
	TEST_IDX_FF01_VAL_CHAR,
	TEST_IDX_FF01_VAL_VALUE,
	TEST_IDX_FF02_VAL_CHAR,
	TEST_IDX_FF02_VAL_VALUE,
	TEST_IDX_FF02_VAL_IND_CFG,
	TEST_IDX_FF03_VAL_CHAR,
	TEST_IDX_FF03_VAL_VALUE,
	TEST_IDX_FF03_VAL_NTF_CFG,
	TEST_IDX_NB,
};

bk_attm_desc_t test_att_db[TEST_IDX_NB] =
{
	//  Service Declaration
	[TEST_IDX_SVC]              = {BK_ATT_DECL_PRIMARY_SERVICE_128, BK_PERM_SET(RD, ENABLE), 0, 0},

	//  Level Characteristic Declaration
	[TEST_IDX_FF01_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,  BK_PERM_SET(RD, ENABLE), 0, 0},
	//  Level Characteristic Value
	[TEST_IDX_FF01_VAL_VALUE]   = {WRITE_REQ_CHARACTERISTIC_128,    BK_PERM_SET(WRITE_REQ, ENABLE), BK_PERM_SET(RI, ENABLE)|BK_PERM_SET(UUID_LEN, UUID_16), 128},

	[TEST_IDX_FF02_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,  BK_PERM_SET(RD, ENABLE), 0, 0},
	//  Level Characteristic Value
	[TEST_IDX_FF02_VAL_VALUE]   = {INDICATE_CHARACTERISTIC_128,     BK_PERM_SET(IND, ENABLE), BK_PERM_SET(RI, ENABLE)|BK_PERM_SET(UUID_LEN, UUID_16), 128},

	//  Level Characteristic - Client Characteristic Configuration Descriptor

	[TEST_IDX_FF02_VAL_IND_CFG] = {BK_ATT_DESC_CLIENT_CHAR_CFG_128, BK_PERM_SET(RD, ENABLE)|BK_PERM_SET(WRITE_REQ, ENABLE), 0, 0},

    [TEST_IDX_FF03_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,  BK_PERM_SET(RD, ENABLE), 0, 0},
	//  Level Characteristic Value
	[TEST_IDX_FF03_VAL_VALUE]   = {NOTIFY_CHARACTERISTIC_128,       BK_PERM_SET(NTF, ENABLE), BK_PERM_SET(RI, ENABLE)|BK_PERM_SET(UUID_LEN, UUID_16), 128},

	//  Level Characteristic - Client Characteristic Configuration Descriptor

	[TEST_IDX_FF03_VAL_NTF_CFG] = {BK_ATT_DESC_CLIENT_CHAR_CFG_128, BK_PERM_SET(RD, ENABLE)|BK_PERM_SET(WRITE_REQ, ENABLE), 0, 0},
};

ble_err_t bk_ble_init(void)
{
    ble_err_t status = ERR_SUCCESS;

    struct bk_ble_db_cfg ble_db_cfg;

    ble_db_cfg.att_db = test_att_db;
    ble_db_cfg.att_db_nb = TEST_IDX_NB;
    ble_db_cfg.prf_task_id = 0;
    ble_db_cfg.start_hdl = 0;
    ble_db_cfg.svc_perm = BK_PERM_SET(SVC_UUID_LEN, UUID_16);
    memcpy(&(ble_db_cfg.uuid[0]), &test_svc_uuid[0], 16);

    status = bk_ble_create_db(&ble_db_cfg);

    return status;
}

void cli_ble_write_callback(write_req_t *write_req)
{
    bk_printf("write_cb[prf_id:%d, att_idx:%d, len:%d]\r\n", write_req->prf_id, write_req->att_idx, write_req->len);
    for(int i = 0; i < write_req->len; i++)
    {
        bk_printf("0x%x ", write_req->value[i]);
    }
    bk_printf("\r\n");
}

uint8_t cli_ble_read_callback(read_req_t *read_req)
{
    bk_printf("read_cb[prf_id:%d, att_idx:%d]\r\n", read_req->prf_id, read_req->att_idx);
    read_req->value[0] = 0x10;
    read_req->value[1] = 0x20;
    read_req->value[2] = 0x30;
    return 3;
}

#if (CFG_BLE_VERSION == BLE_VERSION_4_2)
void appm_adv_data_decode(uint8_t len, const uint8_t *data)
{
    uint8_t index;
	uint8_t i;
    for(index = 0; index < len;)
    {
        switch(data[index + 1])
        {
            case 0x01:
            {
                bk_printf("AD_TYPE : ");
                for(i = 0; i < data[index] - 1; i++)
                {
                    bk_printf("%02x ",data[index + 2 + i]);
                }
                bk_printf("\r\n");
                index +=(data[index] + 1);
            }
            break;
            case 0x08:
            case 0x09:
            {
                bk_printf("ADV_NAME : ");
                for(i = 0; i < data[index] - 1; i++)
                {
                    bk_printf("%c",data[index + 2 + i]);
                }
                bk_printf("\r\n");
                index +=(data[index] + 1);
            }
            break;
            case 0x02:
            {
                bk_printf("UUID : ");
                for(i = 0; i < data[index] - 1;)
                {
                    bk_printf("%02x%02x  ",data[index + 2 + i],data[index + 3 + i]);
                    i+=2;
                }
                bk_printf("\r\n");
                index +=(data[index] + 1);
            }
            break;
            default:
            {
                index +=(data[index] + 1);
            }
            break;
        }
    }
    return ;
}

void cli_ble_event_callback(ble_event_t event, void *param)
{
    switch(event)
    {
        case BLE_STACK_OK:
        {
            os_printf("STACK INIT OK\r\n");
            bk_ble_init();
        }
        break;
        case BLE_STACK_FAIL:
        {
            os_printf("STACK INIT FAIL\r\n");
        }
        break;
        case BLE_CONNECT:
        {
            os_printf("BLE CONNECT\r\n");
        }
        break;
        case BLE_DISCONNECT:
        {
            os_printf("BLE DISCONNECT\r\n");
        }
        break;
        case BLE_MTU_CHANGE:
        {
            os_printf("BLE_MTU_CHANGE:%d\r\n", *(uint16_t *)param);
        }
        break;
        case BLE_TX_DONE:
        {
            os_printf("BLE_TX_DONE\r\n");
        }
        break;
        case BLE_GEN_DH_KEY:
        {
            os_printf("BLE_GEN_DH_KEY\r\n");
            os_printf("key_len:%d\r\n", ((struct ble_gen_dh_key_ind *)param)->len);
            for(int i = 0; i < ((struct ble_gen_dh_key_ind *)param)->len; i++)
            {
                os_printf("%02x ", ((struct ble_gen_dh_key_ind *)param)->result[i]);
            }
            os_printf("\r\n");
        }
        break;
        case BLE_GET_KEY:
        {
            os_printf("BLE_GET_KEY\r\n");
            os_printf("pub_x_len:%d\r\n", ((struct ble_get_key_ind *)param)->pub_x_len);
            for(int i = 0; i < ((struct ble_get_key_ind *)param)->pub_x_len; i++)
            {
                os_printf("%02x ", ((struct ble_get_key_ind *)param)->pub_key_x[i]);
            }
            os_printf("\r\n");
        }
        break;
        case BLE_CREATE_DB_OK:
        {
            os_printf("CREATE DB SUCCESS\r\n");
        }
        break;
        default:
            os_printf("UNKNOW EVENT\r\n");
        break;
    }
}

#if (CFG_SUPPORT_BLE_MESH)
void ble_mesh_event_callback(ble_mesh_event_t event, void *param)
{
    switch(event)
    {
        case BLE_MESH_CREATE_DB_OK:
        {
            os_printf("MESH CREATE DB SUCCESS\r\n");
			app_ai_lights_models_init(0);
			app_store_mesh_info();
        }
        break;

        case BLE_MESH_INIT_DONE:
        {
            os_printf("BLE_MESH_INIT_DONE\r\n");
        }
        break;

        case BLE_MESH_CREATE_DB_FAIL:
        {
            os_printf("MESH CREATE DB FAILED\r\n");
        }
        break;

        case BLE_MESH_APP_KEY_ADD_DONE:
        {
        	os_printf("BLE_MESH_APP_KEY_ADD_DONE\r\n");
        }
        break;

        case BLE_MESH_PROV_INVITE:
        {
        	os_printf("BLE_MESH_PROV_INVITE\r\n");
        }
        break;

        case BLE_MESH_PROV_START:
        {
        	os_printf("BLE_MESH_PROV_START\r\n");
        }
        break;

        case BLE_MESH_PROV_DONE:
        {
        	os_printf("BLE_MESH_PROV_DONE\r\n");
        }
        break;

        default:
        {
            os_printf("unknown mesh event\r\n");
        }
    }
}
#endif

static void ble_command_usage(void)
{
    os_printf("ble help           - Help information\r\n");
    os_printf("ble active         - Active ble to with BK7231BTxxx\r\n");
	os_printf("ble start_adv      - Start advertising as a slave device\r\n");
	os_printf("ble stop_adv       - Stop advertising as a slave device\r\n");
    os_printf("ble notify prf_id att_id value\r\n");
    os_printf("                   - Send ntf value to master\r\n");
    os_printf("ble indicate prf_id att_id value\r\n");
    os_printf("                   - Send ind value to master\r\n");

    os_printf("ble disc           - Disconnect\r\n");
}

typedef adv_info_t ble_adv_param_t;

static void ble_advertise(void)
{
    UINT8 mac[6];
    char ble_name[20];
    UINT8 adv_idx, adv_name_len;

    wifi_get_mac_address((char *)mac, CONFIG_ROLE_STA);
    adv_name_len = snprintf(ble_name, sizeof(ble_name), "bk72xx-%02x%02x", mac[4], mac[5]);

    memset(&adv_info, 0x00, sizeof(adv_info));

    adv_info.channel_map = 7;
    adv_info.interval_min = 160;
    adv_info.interval_max = 160;

    adv_idx = 0;
    adv_info.advData[adv_idx] = 0x02; adv_idx++;
    adv_info.advData[adv_idx] = 0x01; adv_idx++;
    adv_info.advData[adv_idx] = 0x06; adv_idx++;

    adv_info.advData[adv_idx] = adv_name_len + 1; adv_idx +=1;
    adv_info.advData[adv_idx] = 0x09; adv_idx +=1; //name
    memcpy(&adv_info.advData[adv_idx], ble_name, adv_name_len); adv_idx +=adv_name_len;

    adv_info.advDataLen = adv_idx;

    adv_idx = 0;

    adv_info.respData[adv_idx] = adv_name_len + 1; adv_idx +=1;
    adv_info.respData[adv_idx] = 0x08; adv_idx +=1; //name
    memcpy(&adv_info.respData[adv_idx], ble_name, adv_name_len); adv_idx +=adv_name_len;
    adv_info.respDataLen = adv_idx;

    if (ERR_SUCCESS != appm_start_advertising())
    {
        os_printf("ERROR\r\n");
    }
}

static void ble_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if ((argc < 2) || (os_strcmp(argv[1], "help") == 0))
    {
        ble_command_usage();
        return;
    }

    if (os_strcmp(argv[1], "active") == 0)
    {
        ble_set_write_cb(cli_ble_write_callback);
        ble_set_read_cb(cli_ble_read_callback);
        ble_set_event_cb(cli_ble_event_callback);
#if (CFG_SUPPORT_BLE_MESH)
        ble_set_mesh_event_cb(ble_mesh_event_callback);
#endif
        ble_activate(NULL);
    }
	else if(os_strcmp(argv[1], "start_adv") == 0)
    {
        ble_advertise();
    }
#if (CFG_SUPPORT_BLE_MESH)
    else if(os_strcmp(argv[1], "mesh_start") == 0)
    {
        bk_ble_mesh_create_db();
    }
    else if(os_strcmp(argv[1], "start_unpb") == 0)
    {
        app_mesh_start_unpb_adv();
    }
    else if(os_strcmp(argv[1], "erase") == 0)
    {
        os_printf("erase nvds and reboot!!\r\n");
        ble_flash_erase(0,(uint32_t)0x1F4000, (4 * 1024), NULL);
        bk_reboot();
    }
#endif
    else if(os_strcmp(argv[1], "stop_adv") == 0)
    {
        if(ERR_SUCCESS != appm_stop_advertising())
        {
            os_printf("ERROR\r\n");
        }
    }
    else if(os_strcmp(argv[1], "notify") == 0)
    {
        uint8 len;
        uint16 prf_id;
        uint16 att_id;
        uint8 write_buffer[20];

        if(argc != 5)
        {
            ble_command_usage();
            return;
        }

        len = os_strlen(argv[4]);
        if ((len % 2 != 0) || (len > 40)) {
            os_printf("notify buffer len error\r\n");
            return;
        }
        hexstr2bin(argv[4], write_buffer, len/2);

        prf_id = atoi(argv[2]);
        att_id = atoi(argv[3]);

        if(ERR_SUCCESS != bk_ble_send_ntf_value(len / 2, write_buffer, prf_id, att_id))
        {
            os_printf("ERROR\r\n");
        }
    }
    else if(os_strcmp(argv[1], "indicate") == 0)
    {
        uint8 len;
        uint16 prf_id;
        uint16 att_id;
        uint8 write_buffer[20];

        if(argc != 5)
        {
            ble_command_usage();
            return;
        }

        len = os_strlen(argv[4]);
        if ((len % 2 != 0) || (len > 40)) {
            os_printf("indicate buffer len error\r\n");
            return;
        }
        hexstr2bin(argv[4], write_buffer, len/2);

        prf_id = atoi(argv[2]);
        att_id = atoi(argv[3]);

        if(ERR_SUCCESS != bk_ble_send_ind_value(len / 2, write_buffer, prf_id, att_id))
        {
            os_printf("ERROR\r\n");
        }
    }
    else if(os_strcmp(argv[1], "disc") == 0)
    {
#if (CFG_BLE_VERSION == BLE_VERSION_4_2)
        appm_disconnect();
#elif (CFG_BLE_VERSION == BLE_VERSION_5_x)
        appm_disconnect(0x13);
#endif
    }
}
#elif (CFG_BLE_VERSION == BLE_VERSION_5_x)
#include "app_ble.h"
#include "app_sdp.h"
void ble_notice_cb(ble_notice_t notice, void *param)
{
	switch (notice) {
	case BLE_5_STACK_OK:
		bk_printf("ble stack ok");
		break;
	case BLE_5_WRITE_EVENT:
	{
		write_req_t *w_req = (write_req_t *)param;
		bk_printf("write_cb:conn_idx:%d, prf_id:%d, add_id:%d, len:%d, data[0]:%02x\r\n",
			w_req->conn_idx, w_req->prf_id, w_req->att_idx, w_req->len, w_req->value[0]);
		break;
	}
	case BLE_5_READ_EVENT:
	{
		read_req_t *r_req = (read_req_t *)param;
		bk_printf("read_cb:conn_idx:%d, prf_id:%d, add_id:%d\r\n",
			r_req->conn_idx, r_req->prf_id, r_req->att_idx);
		r_req->value[0] = 0x12;
		r_req->value[1] = 0x34;
		r_req->value[2] = 0x56;
		r_req->length = 3;
		break;
	}
	case BLE_5_REPORT_ADV:
	{
		recv_adv_t *r_ind = (recv_adv_t *)param;
		bk_printf("r_ind:actv_idx:%d, adv_addr:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
			r_ind->actv_idx, r_ind->adv_addr[0], r_ind->adv_addr[1], r_ind->adv_addr[2],
			r_ind->adv_addr[3], r_ind->adv_addr[4], r_ind->adv_addr[5]);
		break;
	}
	case BLE_5_MTU_CHANGE:
	{
		mtu_change_t *m_ind = (mtu_change_t *)param;
		bk_printf("m_ind:conn_idx:%d, mtu_size:%d\r\n", m_ind->conn_idx, m_ind->mtu_size);
		break;
	}
	case BLE_5_CONNECT_EVENT:
	{
		conn_ind_t *c_ind = (conn_ind_t *)param;
		bk_printf("c_ind:conn_idx:%d, addr_type:%d, peer_addr:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
			c_ind->conn_idx, c_ind->peer_addr_type, c_ind->peer_addr[0], c_ind->peer_addr[1],
			c_ind->peer_addr[2], c_ind->peer_addr[3], c_ind->peer_addr[4], c_ind->peer_addr[5]);
		break;
	}
	case BLE_5_DISCONNECT_EVENT:
	{
		discon_ind_t *d_ind = (discon_ind_t *)param;
		bk_printf("d_ind:conn_idx:%d,reason:%d\r\n", d_ind->conn_idx,d_ind->reason);
		break;
	}
	case BLE_5_ATT_INFO_REQ:
	{
		att_info_req_t *a_ind = (att_info_req_t *)param;
		bk_printf("a_ind:conn_idx:%d\r\n", a_ind->conn_idx);
		a_ind->length = 128;
		a_ind->status = ERR_SUCCESS;
		break;
	}
	case BLE_5_CREATE_DB:
	{
		create_db_t *cd_ind = (create_db_t *)param;
		bk_printf("cd_ind:prf_id:%d, status:%d\r\n", cd_ind->prf_id, cd_ind->status);
		break;
	}
	case BLE_5_INIT_CONNECT_EVENT:
	{
		conn_ind_t *c_ind = (conn_ind_t *)param;
		bk_printf("BLE_5_INIT_CONNECT_EVENT:conn_idx:%d, addr_type:%d, peer_addr:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
			c_ind->conn_idx, c_ind->peer_addr_type, c_ind->peer_addr[0], c_ind->peer_addr[1],
			c_ind->peer_addr[2], c_ind->peer_addr[3], c_ind->peer_addr[4], c_ind->peer_addr[5]);
		break;
	}
	case BLE_5_INIT_DISCONNECT_EVENT:
	{
		discon_ind_t *d_ind = (discon_ind_t *)param;
		bk_printf("BLE_5_INIT_DISCONNECT_EVENT:conn_idx:%d,reason:%d\r\n", d_ind->conn_idx,d_ind->reason);
		break;
	}
	case BLE_5_SDP_REGISTER_FAILED:
		bk_printf("BLE_5_SDP_REGISTER_FAILED\r\n");
		break;
	default:
		break;
	}
}

void ble_cmd_cb(ble_cmd_t cmd, ble_cmd_param_t *param)
{
	bk_printf("cmd:%d idx:%d status:%d\r\n", cmd, param->cmd_idx, param->status);
}
#if BLE_SDP_CLIENT
static void ble_app_sdp_characteristic_cb(unsigned char conidx,uint16_t chars_val_hdl,unsigned char uuid_len,unsigned char *uuid)
{
	bk_printf("[APP]characteristic conidx:%d,handle:0x%02x(%d),UUID:0x",conidx,chars_val_hdl,chars_val_hdl);
	for(int i = 0; i< uuid_len; i++)
	{
		bk_printf("%02x ",uuid[i]);
	}
	bk_printf("\r\n");
}

void app_sdp_charac_cb(CHAR_TYPE type,uint8 conidx,uint16_t hdl,uint16_t len,uint8 *data)
{
	bk_printf("[APP]type:%x conidx:%d,handle:0x%02x(%d),len:%d,0x",type,conidx,hdl,hdl,len);
	for(int i = 0; i< len; i++)
	{
		bk_printf("%02x ",data[i]);
	}
	bk_printf("\r\n");
}
static const app_sdp_service_uuid service_tab[] ={
	{
		.uuid_len = 0x02,
		.uuid[0] = 0x00,
		.uuid[1] = 0x18,
	},
	{
		.uuid_len = 0x02,
		.uuid[0] = 0x01,
		.uuid[1] = 0x18,
	},
};

#endif
#define BLE_VSN5_DEFAULT_MASTER_IDX      0

static void ble_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	uint8_t adv_data[31];
	uint8_t actv_idx;

	if (os_strcmp(argv[1], "dut") == 0) {
		ble_dut_start();
	}
	if (os_strcmp(argv[1], "active") == 0) {
		ble_set_notice_cb(ble_notice_cb);
		bk_ble_init();
	}
	if (os_strcmp(argv[1], "create_adv") == 0) {
		actv_idx = app_ble_get_idle_actv_idx_handle();
		bk_ble_create_advertising(actv_idx, 7, 160, 160, ble_cmd_cb);
	}
	if (os_strcmp(argv[1], "set_adv_data") == 0) {
		adv_data[0] = 0x02;
		adv_data[1] = 0x01;
		adv_data[2] = 0x06;
		adv_data[3] = 0x0B;
		adv_data[4] = 0x09;
		memcpy(&adv_data[5], "7231N_BLE", 10);
		bk_ble_set_adv_data(os_strtoul(argv[2], NULL, 10), adv_data, 0xF, ble_cmd_cb);
	}
	if (os_strcmp(argv[1], "set_rsp_data") == 0) {
		adv_data[0] = 0x07;
		adv_data[1] = 0x08;
		memcpy(&adv_data[2], "7231N", 6);
		bk_ble_set_scan_rsp_data(os_strtoul(argv[2], NULL, 10), adv_data, 0x8, ble_cmd_cb);
	}
	if (os_strcmp(argv[1], "start_adv") == 0) {
		bk_ble_start_advertising(os_strtoul(argv[2], NULL, 10), 0, ble_cmd_cb);
	}
	if (os_strcmp(argv[1], "stop_adv") == 0) {
		bk_ble_stop_advertising(os_strtoul(argv[2], NULL, 10), ble_cmd_cb);
	}
	if (os_strcmp(argv[1], "delete_adv") == 0) {
		bk_ble_delete_advertising(os_strtoul(argv[2], NULL, 10), ble_cmd_cb);
	}
	if (os_strcmp(argv[1], "create_scan") == 0) {
		actv_idx = app_ble_get_idle_actv_idx_handle();
		bk_ble_create_scaning(actv_idx, ble_cmd_cb);
	}
	if (os_strcmp(argv[1], "start_scan") == 0) {
		bk_ble_start_scaning(os_strtoul(argv[2], NULL, 10), 100, 30, ble_cmd_cb);
	}
	if (os_strcmp(argv[1], "stop_scan") == 0) {
		bk_ble_stop_scaning(os_strtoul(argv[2], NULL, 10), ble_cmd_cb);
	}
	if (os_strcmp(argv[1], "delete_scan") == 0) {
		bk_ble_delete_scaning(os_strtoul(argv[2], NULL, 10), ble_cmd_cb);
	}
	if (os_strcmp(argv[1], "update_conn") == 0) {
		bk_ble_update_param(os_strtoul(argv[2], NULL, 10), 50, 50, 0, 800, ble_cmd_cb);
	}
	if (os_strcmp(argv[1], "dis_conn") == 0) {
		bk_ble_disconnect(os_strtoul(argv[2], NULL, 10), ble_cmd_cb);
	}
	if (os_strcmp(argv[1], "mtu_change") == 0) {
		bk_ble_gatt_mtu_change(os_strtoul(argv[2], NULL, 10), ble_cmd_cb);
	}
	if (os_strcmp(argv[1], "init_adv") == 0) {
		struct adv_param adv_info;
		adv_info.channel_map = 7;
		adv_info.duration = 0;
		adv_info.interval_min = 160;
		adv_info.interval_max = 160;
		adv_info.advData[0] = 0x02;
		adv_info.advData[1] = 0x01;
		adv_info.advData[2] = 0x06;
		adv_info.advData[3] = 0x0B;
		adv_info.advData[4] = 0x09;
		memcpy(&adv_info.advData[5], "7231N_BLE", 10);
		adv_info.advDataLen = 0xF;
		adv_info.respData[0] = 0x07;
		adv_info.respData[1] = 0x08;
		memcpy(&adv_info.respData[2], "7231N", 6);
		adv_info.respDataLen = 0x8;
		actv_idx = app_ble_get_idle_actv_idx_handle();
		bk_ble_adv_start(actv_idx, &adv_info, ble_cmd_cb);
	}
	if (os_strcmp(argv[1], "deinit_adv") == 0) {
		bk_ble_adv_stop(os_strtoul(argv[2], NULL, 10), ble_cmd_cb);
	}
	if (os_strcmp(argv[1], "init_scan") == 0) {
		struct scan_param scan_info;
		scan_info.channel_map = 7;
		scan_info.interval = 100;
		scan_info.window = 30;
		actv_idx = app_ble_get_idle_actv_idx_handle();
		bk_ble_scan_start(actv_idx, &scan_info, ble_cmd_cb);
	}
	if (os_strcmp(argv[1], "deinit_scan") == 0) {
		bk_ble_scan_stop(os_strtoul(argv[2], NULL, 10), ble_cmd_cb);
	}
#if CFG_BLE_INIT_NUM
	if (os_strcmp(argv[1], "con_create") == 0)
	{
		ble_set_notice_cb(ble_notice_cb);
	#if BLE_SDP_CLIENT
		register_app_sdp_service_tab(sizeof(service_tab)/sizeof(app_sdp_service_uuid),service_tab);
		app_sdp_service_filtration(0);
		register_app_sdp_characteristic_callback(ble_app_sdp_characteristic_cb);
		register_app_sdp_charac_callback(app_sdp_charac_cb);
	#endif
		actv_idx = app_ble_get_idle_conn_idx_handle();
		bk_printf("------------->actv_idx:%d\r\n",actv_idx);
		///actv_idx = BLE_VSN5_DEFAULT_MASTER_IDX;
		///appm_create_init(actv_idx, 0, 0, 0);
		bk_ble_create_init(actv_idx, 0, 0, 0,ble_cmd_cb);
	}
	else if ((os_strcmp(argv[1], "con_start") == 0) && (argc >= 3))
	{
		struct bd_addr bdaddr;
		unsigned char addr_type = ADDR_PUBLIC;
		int addr_type_str = atoi(argv[3]);
		int actv_idx_str = atoi(argv[4]);
		bk_printf("idx:%d,addr_type:%d\r\n",actv_idx_str,addr_type_str);
		if((addr_type_str > ADDR_RPA_OR_RAND)||(actv_idx_str >= 0xFF)){
			return;
		}
		actv_idx = actv_idx_str;
		hexstr2bin(argv[2], bdaddr.addr, GAP_BD_ADDR_LEN);
		addr_type = addr_type_str;
		bk_ble_init_set_connect_dev_addr(actv_idx,&bdaddr,addr_type);
		bk_ble_init_start_conn(actv_idx,ble_cmd_cb);
	}
	else if ((os_strcmp(argv[1], "con_stop") == 0) && (argc >= 3))
	{
		int actv_idx_str = atoi(argv[2]);
		bk_printf("idx:%d\r\n",actv_idx_str);
		if(actv_idx_str >= 0xFF){
			return;
		}
		actv_idx = actv_idx_str;
		bk_ble_init_stop_conn(actv_idx,ble_cmd_cb);
	}
	else if ((os_strcmp(argv[1], "con_dis") == 0) && (argc >= 3))
	{
		int actv_idx_str = atoi(argv[2]);
		bk_printf("idx:%d\r\n",actv_idx_str);
		if(actv_idx_str >= 0xFF){
			return;
		}
		actv_idx = actv_idx_str;
		app_ble_master_appm_disconnect(actv_idx);
	}
#if BLE_SDP_CLIENT
	else if (os_strcmp(argv[1], "con_read") == 0)
	{
		if(argc < 4){
			bk_printf("param error\r\n");
			return;
		}
		int actv_idx_str = atoi(argv[3]);
		bk_printf("idx:%d\r\n",actv_idx_str);
		if(actv_idx_str >= 0xFF){
			return;
		}
		actv_idx = actv_idx_str;
		int handle = atoi(argv[2]);
		if(handle >=0 && handle <= 0xFFFF){
			bk_ble_read_service_data_by_handle_req(actv_idx,handle,ble_cmd_cb);
			///appm_read_service_data_by_handle_req(BLE_VSN5_DEFAULT_MASTER_IDX,handle);
		}
		else{
			bk_printf("handle(%x) error\r\n",handle);
		}
	}
	else if (os_strcmp(argv[1], "con_write") == 0)
	{
		if(argc < 4){
			bk_printf("param error\r\n");
			return;
		}
		int handle = atoi(argv[2]);
		int actv_idx_str = atoi(argv[3]);
		bk_printf("idx:%d\r\n",actv_idx_str);
		if(actv_idx_str >= 0xFF){
			return;
		}
		actv_idx = actv_idx_str;
		unsigned char test_buf[4] = {0x01,0x02,0x22,0x32};
		if(handle >=0 && handle <= 0xFFFF){
			bk_ble_write_service_data_req(actv_idx,handle,4,test_buf,ble_cmd_cb);
			///appc_write_service_data_req(BLE_VSN5_DEFAULT_MASTER_IDX,handle,4,test_buf);
		}else{
			bk_printf("handle(%x) error\r\n",handle);
		}
	}
	else if (os_strcmp(argv[1], "con_rd_sv_ntf_int_cfg") == 0)
	{
		if(argc < 4){
			bk_printf("param error\r\n");
			return;
		}
		int actv_idx_str = atoi(argv[3]);
		bk_printf("idx:%d\r\n",actv_idx_str);
		if(actv_idx_str >= 0xFF){
			return;
		}
		actv_idx = actv_idx_str;
		int handle = atoi(argv[2]);
		if(handle >=0 && handle <= 0xFFFF){
			appm_read_service_ntf_ind_cfg_by_handle_req(actv_idx,handle);
		}else{
			bk_printf("handle(%x) error\r\n",handle);
		}
	}
	else if (os_strcmp(argv[1], "con_rd_sv_ud_cfg") == 0)
	{
		if(argc < 4){
			bk_printf("param error\r\n");
			return;
		}
		int actv_idx_str = atoi(argv[3]);
		bk_printf("idx:%d\r\n",actv_idx_str);
		if(actv_idx_str >= 0xFF){
			return;
		}
		actv_idx = actv_idx_str;
		int handle = atoi(argv[2]);
		if(handle >=0 && handle <= 0xFFFF){
			appm_read_service_userDesc_by_handle_req(actv_idx,handle);
		}else{
			bk_printf("handle(%x) error\r\n",handle);
		}
	}
	else if(os_strcmp(argv[1], "svc_filt") == 0)
	{
		if(argc < 3){
			bk_printf("param error\r\n");
			return;
		}
		int en = atoi(argv[2]);
		bk_printf("svc_filt en:%d\r\n",en);
		app_sdp_service_filtration(en);
	}
#endif
#endif ///CFG_BLE_INIT_NUM
}
#endif
#endif

#if CFG_WIFI_SENSOR
static void wifi_sensor_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	int status;

    if ( argc != 2 )
    {
        bk_printf("param error");
    }


	if (os_strcmp(argv[1], "start") == 0)
    {
		bk_wifi_detect_movement_start();
    }

	if (os_strcmp(argv[1], "stop") == 0)
    {
		bk_wifi_detect_movement_stop();
    }

	if (os_strcmp(argv[1], "status") == 0)
    {
		status = bk_get_movement_status();

		if(status ==0)
		{
			bk_printf("detect something");
		}
		else
		{
			bk_printf("detect nothing");
		}
    }
}
#endif

extern void cmd_rfcali_cfg_mode(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
extern void cmd_rfcali_cfg_rate_dist(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
extern void cmd_rfcali_cfg_tssi_g(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
extern void cmd_rfcali_cfg_tssi_b(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
extern void cmd_rfcali_show_data(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);

static void wifi_mgmt_filter_cb(uint8_t *data, int len, void *info)
{
	if (!data) {
		bk_printf("null data\n");
		return;
	}

	uint16_t framectrl = co_read16(data);
	uint16_t frame_type_subtype = framectrl & MAC_FCTRL_TYPESUBTYPE_MASK;

	bk_printf("filter type=%x info=%u\n", frame_type_subtype, (uint32_t)info);
}

static void wifi_mgmt_filter_help(void)
{
	bk_printf("wifi_mgmt_filter 0(all)/1(probe req)/-1(stop)\n");
}

static void cmd_wifi_mgmt_filter(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	OSStatus ret = kGeneralErr;
	uint32_t filter = 0;

	if (argc != 2){
		wifi_mgmt_filter_help();
		return;
	}

	filter = os_strtoul(argv[1], NULL, 0);
	if (filter == -1) {
		ret = bk_wlan_reg_rx_mgmt_cb(NULL, 0);
	} else {
		ret = bk_wlan_reg_rx_mgmt_cb(wifi_mgmt_filter_cb, filter);
	}

	bk_printf("set filter ret=%x\n", ret);
}

#if CFG_WIFI_RAW_TX_CMD

typedef struct {
	uint32_t interval;
	uint32_t counter;
} wifi_raw_tx_param_t;

static void wifi_raw_tx_thread(void *arg)
{
	char frame[] = {
		0xB0, //version, type, subtype
		0x00, //frame control
		0x3A, 0x01, //duration
		0xC8, 0x47, 0x8C, 0x42, 0x00, 0x48, //Address1 - destination
		0x4C, 0xD1, 0xA1, 0xC5, 0x38, 0xE4, //Address2 - source
		0x4C, 0xD1, 0xA1, 0xC5, 0x38, 0xE4, //Address3 - bssid
		0x20, 0xC0, //sequence

		//Auth Response
		0x00, 0x00, //Auth algorithm - open system
		0x02, 0x00, //Auth seq num
		0x00, 0x00, //Status code
	};
	wifi_raw_tx_param_t *tx_param;
	int ret;

	tx_param = (wifi_raw_tx_param_t *)arg;
	os_printf("wifi raw tx begin, interval=%u counter=%d\n", tx_param->interval,
			tx_param->counter);

	for (uint32_t i = 0; i < tx_param->counter; i++) {
		ret = bk_wlan_send_80211_raw_frame((unsigned char*)frame, sizeof(frame));
		if (ret != kNoErr) {
			os_printf("raw tx error, ret=%d\n", ret);
		}

		rtos_delay_milliseconds(tx_param->interval);
	}

	os_free(arg);
	os_printf("wifi raw tx end\n");
	rtos_delete_thread(NULL);
}

static void wifi_raw_tx_command(char *pcWriteBuffer, int xWriteBufferLen,
				int argc, char **argv)
{
	OSStatus ret;

	if (argc != 3) {
		bk_printf("param error");
		bk_printf("usage: wifi_raw_tx interval counter");
		return;
	}

	wifi_raw_tx_param_t *tx_param;
	tx_param = (wifi_raw_tx_param_t *)os_malloc(sizeof(wifi_raw_tx_param_t));
	if (!tx_param) {
		bk_printf("out of memory\n");
		return;
	}

	tx_param->interval = os_strtoul(argv[1], NULL, 10);
	tx_param->counter = os_strtoul(argv[2], NULL, 10);
	ret = rtos_create_thread(NULL, THD_CORE_PRIORITY, "raw_tx",
				(beken_thread_function_t)wifi_raw_tx_thread,
				2048, tx_param);
	if (kNoErr != ret) {
		os_free(tx_param);
		os_printf("Create raw tx thread failed, ret=%d\r\n", ret);
		return;
	}
}
#endif

#if CFG_BK_AWARE
extern void bk_aware_demo_main(void);
extern void bk_aware_demo_stop(void);

static void bk_wifi_aware_command(char *pcWriteBuffer, int xWriteBufferLen,
				int argc, char **argv)
{
	if (argc == 2 && os_strcmp(argv[1], "start") == 0) {
		bk_aware_demo_main();
	} else if (argc == 2 && os_strcmp(argv[1], "stop") == 0) {
		bk_aware_demo_stop();
	} else {
		os_printf("Usage: bk_aware <start>|<stop>\n");
	}
}
#endif

static const struct cli_command built_ins[] =
{
    {"help", NULL, help_command},
    {"version", NULL, get_version},
    {"echo", NULL, echo_cmd_handler},
    {"exit", "CLI exit", cli_exit_handler},

    /// WIFI
    {"scan", "scan ap", wifiscan_Command},
    {"advscan", "scan ap", wifiadvscan_Command},
    {"softap", "softap ssid key", softap_Command},
    {"stopintf", "stopintf intfacename", stop_wlan_intface_Command},
    {"sta", "sta ap_ssid key", sta_Command},
#if (CFG_WFA_CERT || CFG_WPA_CTRL_IFACE)
    {"net", "wifi net config", net_Command},           // 8k rom size
#endif
    {"adv", "adv", sta_adv_Command},
    {"mtr", "mtr channel", mtr_Command},
    {"addif", "addif param", add_virtual_intface},
    {"delif", "delif role", del_virtual_intface},
    {"showif", "show", show_virtual_intface},
    {"psk", "show psk", show_sta_psk},

    {"wifistate", "Show wifi state", wifistate_Command},

    // network
    {"ifconfig", "Show IP address", ifconfig_Command},
    {"ping", "ping <ip>", ping_Command},
    {"dns", "show/clean/<domain>", dns_Command},
    {"sockshow", "Show all sockets", socket_show_Command},
    // os
    {"tasklist", "list all thread name status", task_Command},

    // others
    {"memshow", "print memory information", memory_show_Command},
    {"memdump", "<addr> <length>", memory_dump_Command},
    {"os_memset", "<addr> <value 1> [<value 2> ... <value n>]", memory_set_Command},
    {"memp", "print memp list", memp_dump_Command},

    {"reboot", "reboot system", reboot},

    {"time",     "system time",                 uptime_Command},
    {"flash",    "Flash memory map",            partShow_Command},

    {"GPIO", "GPIO <cmd> <arg1> <arg2>", Gpio_op_Command},
    {"GPIO_INT", "GPIO_INT <cmd> <arg1> <arg2>", Gpio_int_Command},
    {"FLASH", "FLASH <cmd(R/W/E/N)>", flash_command_test},
    {"UART", "UART I <index>", Uart_command_test},

    {"txevm", "txevm [-m] [-c] [-l] [-r] [-w]", tx_evm_cmd_test},
    {"rxsens", "rxsens [-m] [-d] [-c] [-l]", rx_sens_cmd_test},
    #if (CFG_SOC_NAME != SOC_BK7231)
    {"efuse",       "efuse [-r addr] [-w addr data]", efuse_cmd_test},
    {"efusemac",    "efusemac [-r] [-w] [mac]",       efuse_mac_cmd_test},
    #endif // (CFG_SOC_NAME != SOC_BK7231)

#if CFG_SARADC_CALIBRATE
    {"adc", "adc [func] [param]", adc_command},
#endif

    {"easylink", "start easylink", easylink_Command},
#if CFG_AIRKISS_TEST
    {"airkiss", "start airkiss", airkiss_Command},

#endif
#if CFG_SUPPORT_OTA_TFTP
	{"tftpota", "tftpota [ip] [file]", tftp_ota_get_Command},
#endif

#if CFG_USE_SDCARD_HOST
    {"sdtest", "sdtest <cmd>", sd_operate},
#endif

#if CFG_USE_TEMPERATURE_DETECT
    {"tmpdetect", "tmpdetect <cmd>", temp_detect_Command},
#endif
#if CFG_SUPPORT_OTA_HTTP
    {"http_ota", "http_ota url", http_ota_Command},
#endif
    {"regshow", "regshow -w/r addr [value]", reg_write_read_test},

	{"cca", "cca open\\close\\show", phy_cca_test},
#if ((CFG_SOC_NAME != SOC_BK7231) && (CFG_SUPPORT_BLE == 1))
    {"ble", "ble arg1 arg2",  ble_command},
#endif

#if (CFG_SOC_NAME != SOC_BK7231)
    {"rfcali_cfg_mode",      "1:manual, 0:auto",      cmd_rfcali_cfg_mode},
    {"rfcali_cfg_tssi_g",    "0-255",                 cmd_rfcali_cfg_tssi_g},
    {"rfcali_cfg_tssi_b",    "0-255",                 cmd_rfcali_cfg_tssi_b},
    {"rfcali_show_data",     "",                      cmd_rfcali_show_data},
    {"rfcali_cfg_rate_dist", "b g n40 ble (0-31)",    cmd_rfcali_cfg_rate_dist},
#endif
#if CFG_WIFI_SENSOR
	{"wifisensor", "wifi sensor", wifi_sensor_command},
#endif
    {"wifi_mgmt_filter", "wifi_mgmt_filter <0/1/-1>", cmd_wifi_mgmt_filter},
#if CFG_WIFI_RAW_TX_CMD
	{"wifi_raw_tx", "wifi_raw_tx", wifi_raw_tx_command},
#endif
#if CFG_BK_AWARE
	{"bk_aware", "bk_aware", bk_wifi_aware_command},
#endif
};

/* Built-in "help" command: prints all registered commands and their help
* text string, if any. */
void help_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int i, n;
    uint32_t build_in_count = sizeof(built_ins) / sizeof(struct cli_command);

#if (DEBUG)
    build_in_count++; //For command: micodebug
#endif

    os_printf( "====Build-in Commands====\r\n" );
    for (i = 0, n = 0; i < MAX_COMMANDS && n < pCli->num_commands; i++)
    {
        if (pCli->commands[i]->name)
        {
            os_printf("%s: %s\r\n", pCli->commands[i]->name,
                      pCli->commands[i]->help ?
                      pCli->commands[i]->help : "");
            n++;
            if( n == build_in_count )
            {
                os_printf("\r\n====User Commands====\r\n");
            }
        }
    }
}


int cli_register_command(const struct cli_command *command)
{
    int i;
    if (!command->name || !command->function)
        return 1;

    if (pCli->num_commands < MAX_COMMANDS)
    {
        /* Check if the command has already been registered.
        * Return 0, if it has been registered.
        */
        for (i = 0; i < pCli->num_commands; i++)
        {
            if (pCli->commands[i] == command)
                return 0;
        }
        pCli->commands[pCli->num_commands++] = command;
        return 0;
    }

    return 1;
}

int cli_unregister_command(const struct cli_command *command)
{
    int i;
    if (!command->name || !command->function)
        return 1;

    for (i = 0; i < pCli->num_commands; i++)
    {
        if (pCli->commands[i] == command)
        {
            pCli->num_commands--;
            int remaining_cmds = pCli->num_commands - i;
            if (remaining_cmds > 0)
            {
                os_memmove(&pCli->commands[i], &pCli->commands[i + 1],
                           (remaining_cmds *
                            sizeof(struct cli_command *)));
            }
            pCli->commands[pCli->num_commands] = NULL;
            return 0;
        }
    }

    return 1;
}


int cli_register_commands(const struct cli_command *commands, int num_commands)
{
    int i;
    for (i = 0; i < num_commands; i++)
        if (cli_register_command(commands++))
            return 1;
    return 0;
}

int cli_unregister_commands(const struct cli_command *commands,
                            int num_commands)
{
    int i;
    for (i = 0; i < num_commands; i++)
        if (cli_unregister_command(commands++))
            return 1;

    return 0;
}

static void micodebug_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc == 1)
    {
        os_printf("Usage: micodebug on/off. _BK_ debug is currently %s\r\n",
                  mico_debug_enabled ? "Enabled" : "Disabled");
        return;
    }

    if (!os_strcasecmp(argv[1], "on"))
    {
        os_printf("Enable _BK_ debug\r\n");
        mico_debug_enabled = 1;
    }
    else if (!os_strcasecmp(argv[1], "off"))
    {
        os_printf("Disable _BK_ debug\r\n");
        mico_debug_enabled = 0;
    }
}

void monitor(uint8_t *data, int len, wifi_link_info_t *info)
{
    int i;

    monitor_dbg("[%d]: ", len);
    for(i = 0; i < len; i++)
    {
        monitor_dbg("%02x ", data[i]);
    }
    monitor_dbg("\r\n");
}

static void monitor_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc == 1)
    {
        os_printf("Usage: monitor on/off.");
        return;
    }

    if (!os_strcasecmp(argv[1], "on"))
    {
        cmd_printf("start monitor\r\n");
        // start monitor, need set callback
        bk_wlan_register_monitor_cb(monitor);
        // then start hal mac
        bk_wlan_start_monitor();
    }
    else if (!os_strcasecmp(argv[1], "off"))
    {
        cmd_printf("stop monitor\r\n");
        mico_debug_enabled = 0;

        // stop monitor mode, need stop hal mac first
        bk_wlan_stop_monitor();
        // then set monitor callback
        bk_wlan_register_monitor_cb(NULL);
    }
}

static void channel_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int channel, i = 0;

    if (argc == 1)
    {
        os_printf("Usage: channel [1~13].");
        return;
    }

    while(argv[1][i])
    {
        if((argv[1][i] < '0') || (argv[1][i] > '9'))
        {
            os_printf("parameter should be a number\r\n");
            return ;
        }
        i++;
    }

    channel = atoi(argv[1]);

    if((channel < 1) || (channel > 13))
    {
        os_printf("Invalid channel number \r\n");
        return ;
    }
    cmd_printf("monitor mode :set to channel %d\r\n", channel);
    bk_wlan_set_channel_sync(channel);
}

void pwr_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int pwr = 0;

    if(argc != 2)
    {
        os_printf("Usage: pwr [hex:5~15].");
        return;
    }

    pwr = os_strtoul(argv[1], NULL, 16);

    rw_msg_set_power(0,pwr);
}

static void Deep_Sleep_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	PS_DEEP_CTRL_PARAM deep_sleep_param;

	deep_sleep_param.wake_up_way			= 0;

	deep_sleep_param.gpio_index_map      	= os_strtoul(argv[1], NULL, 16);
	deep_sleep_param.gpio_edge_map       	= os_strtoul(argv[2], NULL, 16);
	deep_sleep_param.gpio_last_index_map 	= os_strtoul(argv[3], NULL, 16);
	deep_sleep_param.gpio_last_edge_map  	= os_strtoul(argv[4], NULL, 16);
	deep_sleep_param.sleep_time     		= os_strtoul(argv[5], NULL, 16);
	deep_sleep_param.wake_up_way     		= os_strtoul(argv[6], NULL, 16);
	deep_sleep_param.gpio_stay_lo_map 	    = os_strtoul(argv[7], NULL, 16);
	deep_sleep_param.gpio_stay_hi_map  	    = os_strtoul(argv[8], NULL, 16);

	if(argc == 9)
	{
		os_printf("---deep sleep test param : 0x%0X 0x%0X 0x%0X 0x%0X %d %d\r\n",
					deep_sleep_param.gpio_index_map,
					deep_sleep_param.gpio_edge_map,
					deep_sleep_param.gpio_last_index_map,
					deep_sleep_param.gpio_last_edge_map,
					deep_sleep_param.sleep_time,
					deep_sleep_param.wake_up_way);

	#if (CFG_SOC_NAME != SOC_BK7271)
		bk_enter_deep_sleep_mode(&deep_sleep_param);
	#endif
	}
	else
	{
		os_printf("---argc error!!! \r\n");
	}
}

static void Ps_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    UINT32 dtim = 0;

	#if PS_SUPPORT_MANUAL_SLEEP
    UINT32 standby_time = 0;
    UINT32 dtim_wait_time = 0;
	#endif

    if(argc < 3)
    {
        goto IDLE_CMD_ERR;
    }
#if  CFG_USE_DEEP_PS

    if(0 == os_strcmp(argv[1], "deepps"))
    {
    	PS_DEEP_CTRL_PARAM deep_sleep_param;
        if(argc != 11)
        {
            goto IDLE_CMD_ERR;
        }

    	deep_sleep_param.gpio_index_map      	= os_strtoul(argv[2], NULL, 16);
    	deep_sleep_param.gpio_edge_map       	= os_strtoul(argv[3], NULL, 16);
    	deep_sleep_param.gpio_stay_lo_map 	    = os_strtoul(argv[4], NULL, 16);

    	deep_sleep_param.gpio_last_index_map 	= os_strtoul(argv[5], NULL, 16);
    	deep_sleep_param.gpio_last_edge_map  	= os_strtoul(argv[6], NULL, 16);
        deep_sleep_param.gpio_stay_hi_map  	    = os_strtoul(argv[7], NULL, 16);

    	deep_sleep_param.sleep_time     		= os_strtoul(argv[8], NULL, 16);
    	deep_sleep_param.lpo_32k_src     		= os_strtoul(argv[9], NULL, 16);

    	deep_sleep_param.wake_up_way     		= os_strtoul(argv[10], NULL, 16);

		os_printf("---deep sleep test param : 0x%0X 0x%0X 0x%0X 0x%0X %d %d\r\n",
					deep_sleep_param.gpio_index_map,
					deep_sleep_param.gpio_edge_map,
					deep_sleep_param.gpio_last_index_map,
					deep_sleep_param.gpio_last_edge_map,
					deep_sleep_param.sleep_time,
					deep_sleep_param.wake_up_way);
		os_printf("--- 0x%0X 0x%0X 0x%0X \r\n",
        			deep_sleep_param.gpio_stay_lo_map,
        			deep_sleep_param.gpio_stay_hi_map,
        			deep_sleep_param.lpo_32k_src);

		bk_enter_deep_sleep_mode(&deep_sleep_param);
    }
#endif

#if CFG_USE_FAKERTC_PS
    else if (0 == os_strcmp(argv[1], "idleps"))
    {
        GLOBAL_INT_DECLARATION();
        int count = 0;
        bk_printf("[ARF]rwnxl_reset_evt\r\n");
        HAL_FATAL_ERROR_RECOVER(0);	// rwnxl_reset_evt(0);

        rtos_delay_milliseconds(10);

        while (1) {
            GLOBAL_INT_DISABLE();
            evt_field_t field = ke_evt_get();
            GLOBAL_INT_RESTORE();

            if (!(field & KE_EVT_RESET_BIT))
                break;

            rtos_delay_milliseconds(10);
            if (++count > 10000) {
                bk_printf("%s: failed\r\n", __func__);
                break;
            }
        }

        //bk_enable_unconditional_sleep();
        count = 100;
        while((1 == bk_unconditional_normal_sleep(0xFFFFFFFF,1))) {
            rtos_delay_milliseconds(2);
			count--;
			if(count == 0){
				bk_printf("IDLE_SLEEP timeout\r\n");
				break;
        	}
        }

        bk_printf("idle Sleep out\r\n");
    }
#endif

#if CFG_USE_MCU_PS
    else if(0 == os_strcmp(argv[1], "mcudtim"))
    {
        if(argc != 3)
        {
            goto IDLE_CMD_ERR;
        }

        dtim = os_strtoul(argv[2], NULL, 10);
        if(dtim == 1)
        {
            bk_wlan_mcu_ps_mode_enable();
        }
        else if(dtim == 0)
        {
            bk_wlan_mcu_ps_mode_disable();
        }
        else
        {
            goto IDLE_CMD_ERR;
        }
    }
#endif
#if CFG_USE_STA_PS
    else if(0 == os_strcmp(argv[1], "rfdtim"))
    {
        if(argc != 3)
        {
            goto IDLE_CMD_ERR;
        }

        dtim = os_strtoul(argv[2], NULL, 10);
        if(dtim == 1)
        {
            if (bk_wlan_dtim_rf_ps_mode_enable())
				os_printf("dtim enable failed\r\n");
        }
        else if(dtim == 0)
        {
            if (bk_wlan_dtim_rf_ps_mode_disable())
				os_printf("dtim disable failed\r\n");
        }
        else
        {
            goto IDLE_CMD_ERR;
        }
    }
    else if(0 == os_strcmp(argv[1], "rfwkup"))
    {
        if(argc != 3)
        {
            goto IDLE_CMD_ERR;
        }

        power_save_rf_dtim_manual_do_wakeup();
    }
    else if(0 == os_strcmp(argv[1], "setwktm"))
    {
        if(argc != 3)
        {
            os_printf("beacon len:%d\r\n", power_save_beacon_len_get());
            os_printf("wktm:%d\r\n", power_save_radio_wkup_get());
            goto IDLE_CMD_ERR;
        }

        dtim = os_strtoul(argv[2], NULL, 10);

        if(dtim)
        {
            power_save_radio_wkup_set(dtim);
            os_printf("set ridio wkup:%d\r\n", dtim);
        }
        else
        {
            goto IDLE_CMD_ERR;
        }
    }
    else if(0 == os_strcmp(argv[1], "bcmc"))
    {
        if(argc != 3)
        {
            goto IDLE_CMD_ERR;
        }

        dtim = os_strtoul(argv[2], NULL, 10);

        if(dtim == 0 || dtim == 1)
        {
            power_save_sm_set_all_bcmc(dtim);
        }
        else
        {
            goto IDLE_CMD_ERR;
        }
    }
	#if PS_USE_KEEP_TIMER
    else if(0 == os_strcmp(argv[1], "rf_timer"))
    {
        if(argc != 3)
        {
            goto IDLE_CMD_ERR;
        }

        dtim = os_strtoul(argv[2], NULL, 10);

        if(dtim == 1)
        {
        	extern int bk_wlan_dtim_rf_ps_timer_start(void);
            bk_wlan_dtim_rf_ps_timer_start();
        }
        else  if(dtim == 0)
        {
        	extern int bk_wlan_dtim_rf_ps_timer_pause(void);
            bk_wlan_dtim_rf_ps_timer_pause();
        }
        else
        {
            goto IDLE_CMD_ERR;
        }
    }
	#endif
    else if(0 == os_strcmp(argv[1], "listen"))
    {
        if(argc != 4)
        {
            goto IDLE_CMD_ERR;
        }

        if(0 == os_strcmp(argv[2], "dtim"))
        {
            dtim = os_strtoul(argv[3], NULL, 10);
            power_save_set_dtim_multi(dtim);

        }
        else
        {
            goto IDLE_CMD_ERR;
        }

    }
    else if(0 == os_strcmp(argv[1], "dump"))
    {
        #if CFG_USE_MCU_PS
        mcu_ps_dump();
        #endif
        power_save_dump();
    }
#endif
    else
    {
        goto IDLE_CMD_ERR;
    }


    return;
IDLE_CMD_ERR:
    os_printf("Usage:ps [func] [param] []\r\n");
    os_printf("standby:ps deepps [gpio_map] ");
    os_printf("gpio_map is hex and every bits is map to gpio0-gpio31\r\n");
    os_printf("ieee:ps rfdtim [],[1/0] is open or close \r\n\r\n");
    os_printf("ieee:ps mcudtim [] ,[1/0] is open or close \r\n\r\n");

}

static void mac_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t mac[6];

    if (argc == 1)
    {
        wifi_get_mac_address((char *)mac, CONFIG_ROLE_STA);
        os_printf("MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    else if(argc == 2)
    {
        hexstr2bin(argv[1], mac, 6);
        wifi_set_mac_address((char *)mac);
        os_printf("Set MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    else
    {
        os_printf("invalid cmd\r\n");
    }

}

#if CFG_SARADC_CALIBRATE
/****channel 1 - 7***/
static ADC_OBJ test_adc;
extern uint8_t step_flag ;
extern uint8_t adctest_flag ;
extern uint8_t adc_accuracy;
int adc_offfset, adc_value_2v;

static void adc_detect_callback(int new_mv, void *user_data)
{
	static int cnt = 0;
	new_mv = (void *)(new_mv << (adc_accuracy - 1));
	test_adc.user_data = new_mv;

	if (cnt++ >= 100)
	{
		cnt = 0;
		os_printf("adc channel%d voltage:%d,%x\r\n", test_adc.channel, new_mv, test_adc.user_data);
	}
}

static void adc_detect_callback1(int new_mv, void *user_data)
{
	static int total = 0;
	static int cnt = 0;
	int reg, low_adc;
	static int temp = 0;
	if (temp++ < 100)
		return;
	new_mv = (void *)(new_mv << (adc_accuracy - 1));
	test_adc.user_data = new_mv;

	total += new_mv;
	cnt++;
	if (cnt >= 100)
	{
		low_adc = total / cnt;
		saradc_val.low = low_adc;
		cnt = 0;
		temp = 0;
		adc_offfset = low_adc - 2048;
		os_printf("step1: adc channel:%d adc_offfset:%d,low_adc:%d\r\n", test_adc.channel, adc_offfset, low_adc);
		os_printf("adc_low:%x,adc_high,%x\r\n", saradc_val.low, saradc_val.high);
		total = 0;
	}
}

static void adc_detect_callback2(int new_mv, void *user_data)
{
	static int total = 0;
	static int cnt = 0;
	static int temp = 0;
	if (temp++ < 100)
		return;
	int reg, high_adc;

	new_mv = (void *)(new_mv << (adc_accuracy - 1));
	test_adc.user_data = new_mv;

	total += new_mv;
	cnt++;
	if (cnt >= 100)
	{
		high_adc = total / cnt;
		saradc_val.high = high_adc;
		cnt = 0;
		temp = 0;
		adc_value_2v = high_adc;
		os_printf("step2: adc channel:%d adc_value_2v:%d\r\n", test_adc.channel, high_adc);
		total = 0;
		os_printf("adc_low:%x,adc_high,%x\r\n", saradc_val.low, saradc_val.high);

	}
}

static void adc_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	UINT32 status;
	DD_HANDLE flash_handle;
	DD_HANDLE saradc_handle;
	saradc_cal_val_t p_ADC_cal;
	float voltage = 0.0;
	saradc_desc_t *p_ADC_drv_desc = NULL;
	int channel;
	GLOBAL_INT_DECLARATION();

	if (argc < 2)
		goto IDLE_CMD_ERR;

	if (0 == os_strcmp(argv[1], "read"))
	{
		status = manual_cal_load_adc_cali_flash();
		if (status != 0) {
			os_printf("Can't read cali value, use default!\r\n");
			os_printf("calibrate low value:[%x]\r\n", saradc_val.low);
			os_printf("calibrate high value:[%x]\r\n", saradc_val.high);
		}
	} else if (0 == os_strcmp(argv[1], "set"))
	{
		p_ADC_drv_desc = (saradc_desc_t *)os_malloc(sizeof(saradc_desc_t));
		if (p_ADC_drv_desc == NULL) {
			os_printf("malloc1 failed!\r\n");
			return;
		}

		saradc_config_param_init(p_ADC_drv_desc);

		p_ADC_drv_desc->data_buff_size = ADC_TEMP_BUFFER_SIZE;
		p_ADC_drv_desc->pData = (UINT16 *)os_malloc(p_ADC_drv_desc->data_buff_size * sizeof(UINT16));
		os_memset(p_ADC_drv_desc->pData, 0x00, p_ADC_drv_desc->data_buff_size * sizeof(UINT16));

		if (p_ADC_drv_desc->pData == NULL) {
			os_printf("malloc1 failed!\r\n");
			os_free(p_ADC_drv_desc);
			return;
		}

		UINT32 ret = 0;
		do {
			GLOBAL_INT_DISABLE();
			if (saradc_check_busy() == 0) {
				saradc_handle = ddev_open(SARADC_DEV_NAME, &status, (UINT32)p_ADC_drv_desc);
				if (DD_HANDLE_UNVALID != saradc_handle) {
					GLOBAL_INT_RESTORE();
					break;
				}
			}
			GLOBAL_INT_RESTORE();

			rtos_delay_milliseconds(5);
			ret++;
		} while (ret < 5);

		if (ret == 5) {
			os_printf("saradc open failed!\r\n");
			os_free(p_ADC_drv_desc->pData);
			os_free(p_ADC_drv_desc);
			return;
		}

		while (1) {
			if (p_ADC_drv_desc->current_sample_data_cnt >=
				p_ADC_drv_desc->data_buff_size) {
				ddev_close(saradc_handle);
				saradc_ensure_close();
				break;
			}
		}

		{
			UINT32 sum = 0, sum1, sum2;
			UINT16 *pData = p_ADC_drv_desc->pData;
			sum1 = pData[1] + pData[2];
			sum2 = pData[3] + pData[4];
			sum = sum1 / 2  + sum2 / 2;
			sum = sum / 2;
			sum = sum / 4;
			p_ADC_drv_desc->pData[0] = sum;
		}

		if (0 == os_strcmp(argv[2], "low"))
			p_ADC_cal.mode = SARADC_CALIBRATE_LOW;
		else if (0 == os_strcmp(argv[2], "high"))
			p_ADC_cal.mode = SARADC_CALIBRATE_HIGH;
		else {
			os_printf("invalid parameter\r\n");
			return;
		}
		p_ADC_cal.val = p_ADC_drv_desc->pData[4];
		if (SARADC_FAILURE == ddev_control(saradc_handle, SARADC_CMD_SET_CAL_VAL, (VOID *)&p_ADC_cal)) {
			os_printf("set calibrate value failture\r\n");
			os_free(p_ADC_drv_desc->pData);
			os_free(p_ADC_drv_desc);
			return;
		}
		os_printf("set calibrate success\r\n");
		os_printf("type:[%s] value:[0x%x]\r\n", (p_ADC_cal.mode ? "high" : "low"), p_ADC_cal.val);
		os_free(p_ADC_drv_desc->pData);
		os_free(p_ADC_drv_desc);
	} else if (0 == os_strcmp(argv[1], "write"))
	{
		manual_cal_save_chipinfo_tab_to_flash();
		os_printf("calibrate low value:[%x]\r\n", saradc_val.low);
		os_printf("calibrate high value:[%x]\r\n", saradc_val.high);
	} else if (0 == os_strcmp(argv[1], "cal_low"))
	{
		/*adc calibration when first usd adc channel ,
		 *cal_low: caculate offset when voltage is 0v.
		 *cal_high:caculate voltage is 2v .
		 *start: get current adc value.
		 */
		channel = os_strtoul(argv[2], NULL, 10);
		os_printf("offset adc channel:%d \r\n", channel);
		step_flag = 0;
		adctest_flag = 0;
		saradc_work_create();
		adc_obj_init(&test_adc, adc_detect_callback1, channel, &test_adc);
		adc_obj_start(&test_adc);
	} else if (0 == os_strcmp(argv[1], "cal_high"))
	{
		channel = os_strtoul(argv[2], NULL, 10);
		os_printf("2v: adc channel:%d \r\n", channel);
		step_flag = 1;
		adctest_flag = 0;
		saradc_work_create();
		adc_obj_init(&test_adc, adc_detect_callback2, channel, &test_adc);
		adc_obj_start(&test_adc);
	} else if (0 == os_strcmp(argv[1], "start"))
	{
		channel = os_strtoul(argv[2], NULL, 10);
		os_printf("---adc channel:%d---\r\n", channel);
		adctest_flag = 1;
		saradc_work_create();
		adc_obj_init(&test_adc, adc_detect_callback, channel, &test_adc);
		adc_obj_start(&test_adc);
	} else if (0 == os_strcmp(argv[1], "stop"))
		adc_obj_stop(&test_adc);
	else
		goto IDLE_CMD_ERR;

	return;

IDLE_CMD_ERR:
	os_printf("Usage:ps [func] [param]\r\n");
}
#endif

static void cli_rx_callback(int uport, void *param)
{
	if(log_rx_interrupt_sema)
    	rtos_set_semaphore(&log_rx_interrupt_sema);
}

/* ========= CLI input&output APIs ============ */
int cli_printf(const char *msg, ...)
{
    va_list ap;
    char *pos, message[256];
    int sz;
    int nMessageLen = 0;

    os_memset(message, 0, 256);
    pos = message;

    sz = 0;
    va_start(ap, msg);
    nMessageLen = vsnprintf(pos, 256 - sz, msg, ap);
    va_end(ap);

    if( nMessageLen <= 0 ) return 0;

    cli_putstr((const char *)message);
    return 0;
}


int cli_putstr(const char *msg)
{
    if (msg[0] != 0)
        bk_uart_send( CLI_UART, (const char *)msg, os_strlen(msg) );

    return 0;
}

int cli_getchar(char *inbuf)
{
    if (bk_uart_recv(CLI_UART, inbuf, 1, BEKEN_WAIT_FOREVER) == 0)
        return 1;
    else
        return 0;
}

int cli_getchars(char *inbuf, int len)
{
    if(bk_uart_recv(CLI_UART, inbuf, len, BEKEN_WAIT_FOREVER) == 0)
        return 1;
    else
        return 0;
}

int cli_getchars_prefetch(char *inbuf, int len)
{
    if(bk_uart_recv_prefetch(CLI_UART, inbuf, len, BEKEN_WAIT_FOREVER) == 0)
        return 1;
    else
        return 0;
}

int cli_get_all_chars_len(void)
{
    return bk_uart_get_length_in_buffer(CLI_UART);
}

#if CFG_IPERF_TEST
extern void iperf(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#endif // CFG_IPERF_TEST

static const struct cli_command user_clis[] =
{
    {"micodebug", "micodebug on/off", micodebug_Command},
    {"monitor", "monitor on/off", monitor_Command},
    {"channel", "channel []", channel_Command},
    {"mac", "mac <mac>, Get mac/Set mac. <mac>: c89346000001", mac_command},
    {"ps", "ps [func] [param]", Ps_Command},
    {"deep_sleep", "deep_sleep [param]", Deep_Sleep_Command},

#ifdef TCP_CLIENT_DEMO
    {"tcp_cont", "tcp_cont [ip] [port]", tcp_make_connect_server_command},
#endif

#if CFG_TCP_SERVER_TEST
    {"tcp_server", "tcp_server [ip] [port]",make_tcp_server_command },
#endif

#if CFG_IPERF_TEST
    {"iperf", "iperf help", iperf },
#endif // CFG_IPERF_TEST

#if CFG_SUPPORT_TPC_PA_MAP
    {"pwr", "pwr help", pwr_Command },
#endif

#if (CFG_SOC_NAME == SOC_BK7221U)
    {"sec", "sec help", sec_Command },
#endif
};


extern int video_demo_register_cmd(void);

#if CFG_PERIPHERAL_TEST
void bk_peripheral_cli_init();
#endif

int cli_init(void)
{
    int ret;

    pCli = (struct cli_st *)os_malloc(sizeof(struct cli_st));
    if (pCli == NULL)
        return kNoMemoryErr;

    os_memset((void *)pCli, 0, sizeof(struct cli_st));
    rtos_init_semaphore(&log_rx_interrupt_sema, 10);

    if (cli_register_commands(&built_ins[0],
                              sizeof(built_ins) / sizeof(struct cli_command)))
    {
        goto init_general_err;
    }

    if(cli_register_commands(user_clis, sizeof(user_clis) / sizeof(struct cli_command))) {
        goto init_general_err;
    }

    if (video_demo_register_cmd()) {
        goto init_general_err;
    }

#if CFG_PERIPHERAL_TEST
	bk_peripheral_cli_init();
#endif

    ret = rtos_create_thread(&cli_thread_handle,
                             BEKEN_DEFAULT_WORKER_PRIORITY,
                             "cli",
                             (beken_thread_function_t)cli_main,
                             4096,
                             0);
    if (ret != kNoErr)
    {
        os_printf("Error: Failed to create cli thread: %d\r\n",
                  ret);
        goto init_general_err;
    }

    pCli->initialized = 1;
    pCli->echo_disabled = 0;

    return kNoErr;

init_general_err:
    if(pCli)
    {
        os_free(pCli);
        pCli = NULL;
    }

    return kGeneralErr;
}

// eof

