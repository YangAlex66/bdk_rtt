#include <rtthread.h>
#include <finsh.h>
#include "common.h"
#include "param_config.h"
#include "ble_pub.h"

#if (CFG_SUPPORT_BLE)

#include "ble.h"
#if (CFG_BLE_VERSION == BLE_VERSION_5_x)
#include "app_ble.h"
#include "ble_api_5_x.h"
#else
#include "application.h"
#include "ble_api.h"
#endif


#if (CFG_BLE_VERSION == BLE_VERSION_4_2)
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
	TEST_IDX_NB,
};

bk_attm_desc_t test_att_db[6] =
{
	//  Service Declaration
	[TEST_IDX_SVC]              = {BK_ATT_DECL_PRIMARY_SERVICE_128, BK_PERM_SET(RD, ENABLE), 0, 0},
	
	//  Level Characteristic Declaration
	[TEST_IDX_FF01_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,  BK_PERM_SET(RD, ENABLE), 0, 0},
	//  Level Characteristic Value
	[TEST_IDX_FF01_VAL_VALUE]   = {WRITE_REQ_CHARACTERISTIC_128,    BK_PERM_SET(WRITE_REQ, ENABLE), BK_PERM_SET(RI, ENABLE) , 128},
	
	[TEST_IDX_FF02_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,  BK_PERM_SET(RD, ENABLE), 0, 0},
	//  Level Characteristic Value
	[TEST_IDX_FF02_VAL_VALUE]   = {INDICATE_CHARACTERISTIC_128,     BK_PERM_SET(IND, ENABLE) , BK_PERM_SET(RI, ENABLE) , 128},

	//  Level Characteristic - Client Characteristic Configuration Descriptor

	[TEST_IDX_FF02_VAL_IND_CFG] = {BK_ATT_DESC_CLIENT_CHAR_CFG_128, BK_PERM_SET(RD, ENABLE)|BK_PERM_SET(WRITE_REQ, ENABLE), 0, 0},
};

ble_err_t bk_ble_init(void)
{
    ble_err_t status;
    struct bk_ble_db_cfg ble_db_cfg;

    ble_db_cfg.att_db = test_att_db;
    ble_db_cfg.att_db_nb = TEST_IDX_NB;
    ble_db_cfg.prf_task_id = 0;
    ble_db_cfg.start_hdl = 0;
    ble_db_cfg.svc_perm = 0;
    memcpy(&(ble_db_cfg.uuid[0]), &test_svc_uuid[0], 16);

    status = bk_ble_create_db(&ble_db_cfg);

    return status;
}

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

void ble_write_callback(write_req_t *write_req)
{
    bk_printf("write_cb[prf_id:%d, att_idx:%d, len:%d]\r\n", write_req->prf_id, write_req->att_idx, write_req->len);
}

uint8_t ble_read_callback(read_req_t *read_req)
{
    bk_printf("read_cb[prf_id:%d, att_idx:%d]\r\n", read_req->prf_id, read_req->att_idx);
    read_req->value[0] = 0x10;
    read_req->value[1] = 0x20;
    read_req->value[2] = 0x30;
    return 3;
}

void ble_event_callback(ble_event_t event, void *param)
{
    switch(event)
    {
        case BLE_STACK_OK:
        {
            bk_printf("STACK INIT OK\r\n");
            bk_ble_init();
        }
        break;
        case BLE_STACK_FAIL:
        {
            bk_printf("STACK INIT FAIL\r\n");
        }
        break;
        case BLE_CONNECT:
        {
            bk_printf("BLE CONNECT\r\n");
        }
        break;
        case BLE_DISCONNECT:
        {
            bk_printf("BLE DISCONNECT\r\n");
        }
        break;
        case BLE_MTU_CHANGE:
        {
            bk_printf("BLE_MTU_CHANGE:%d\r\n", *(uint16_t *)param);
        }
        break;
        case BLE_TX_DONE:
        {
            bk_printf("BLE_TX_DONE\r\n");
        }
        break;
        case BLE_GEN_DH_KEY:
        {
            bk_printf("BLE_GEN_DH_KEY\r\n");
        }    
        break;
        case BLE_GET_KEY:
        {
            bk_printf("BLE_GET_KEY\r\n");
        }    
        break;
        case BLE_CREATE_DB_OK:
        {
            bk_printf("CREATE DB SUCCESS\r\n");
        }
        break;
        default:
            bk_printf("UNKNOW EVENT\r\n");
        break;
    }
}

static void ble_command_usage(void)
{
    bk_printf("ble help           - Help information\n");
    bk_printf("ble active         - Active ble to with BK7231BTxxx\n");    
	bk_printf("ble start_adv      - Start advertising as a slave device\n");
	bk_printf("ble stop_adv       - Stop advertising as a slave device\n");
    bk_printf("ble notify prf_id att_id value\n");
    bk_printf("                   - Send ntf value to master\n");
    bk_printf("ble indicate prf_id att_id value\n");
    bk_printf("                   - Send ind value to master\n");

    bk_printf("ble disc           - Disconnect\n");
    bk_printf("ble dut            - dut test\n");
}

__maybe_unused static void ble_get_info_handler(void);
static void ble_get_info_handler(void)
{
    UINT8 *ble_mac;
    bk_printf("\r\n****** ble information ************\r\n");

    if (ble_is_start() == 0) {
        bk_printf("no ble startup          \r\n");
        return;
    }
    ble_mac = ble_get_mac_addr();    
    bk_printf("* name: %s             \r\n", ble_get_name());
    bk_printf("* mac:%02x-%02x-%02x-%02x-%02x-%02x\r\n", ble_mac[0], ble_mac[1],ble_mac[2],ble_mac[3],ble_mac[4],ble_mac[5]);  
    bk_printf("***********  end  *****************\r\n");     
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
        bk_printf("ERROR\r\n");
    }
}

static void ble(int argc, char **argv)
{
    if ((argc < 2) || (os_strcmp(argv[1], "help") == 0))
    {
        ble_command_usage();
        return ;
    }

    if (os_strcmp(argv[1], "active") == 0)
    {
        ble_set_write_cb(ble_write_callback);
        ble_set_read_cb(ble_read_callback);
        ble_set_event_cb(ble_event_callback);
        ble_activate(NULL);
    }
	else if(os_strcmp(argv[1], "start_adv") == 0)
    { 
        ble_advertise();
    }
    else if(os_strcmp(argv[1], "stop_adv") == 0)
    {
        if(ERR_SUCCESS != appm_stop_advertising())
        {
            bk_printf("ERROR\r\n");
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
            return ;
        }

        len = os_strlen(argv[4]);
        if((len % 2 != 0) || (len > 40))
        {
            bk_printf("notify buffer len error\r\n");
            return ;
        }
        hexstr2bin(argv[4], write_buffer, len / 2);

        prf_id = atoi(argv[2]);
        att_id = atoi(argv[3]);

        if(ERR_SUCCESS != bk_ble_send_ntf_value(len / 2, write_buffer, prf_id, att_id))
        {
            bk_printf("ERROR\r\n");
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
            return ;
        }

        len = os_strlen(argv[4]);
        if((len % 2 != 0) || (len > 40))
        {
            bk_printf("indicate buffer len error\r\n");
            return ;
        }
        hexstr2bin(argv[4], write_buffer, len / 2);

        prf_id = atoi(argv[2]);
        att_id = atoi(argv[3]);

        if(ERR_SUCCESS != bk_ble_send_ind_value(len / 2, write_buffer, prf_id, att_id))
        {
            bk_printf("ERROR\r\n");
        }
    }
    else if(os_strcmp(argv[1], "disc") == 0)
    {
        appm_disconnect();
    }
    else if(os_strcmp(argv[1], "dut") == 0)
    {
        ble_dut_start();
    }
}

MSH_CMD_EXPORT(ble, ble command);
#endif

#if (CFG_BLE_VERSION == BLE_VERSION_5_x)
extern struct app_env_tag app_ble_ctx;

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
    ble_err_t status;
    struct bk_ble_db_cfg ble_db_cfg;

    ble_db_cfg.att_db = test_att_db;
    ble_db_cfg.att_db_nb = TEST_IDX_NB;
    ble_db_cfg.prf_task_id = 0;
    ble_db_cfg.start_hdl = 0;
    ble_db_cfg.svc_perm = 0;
    memcpy(&(ble_db_cfg.uuid[0]), &test_svc_uuid[0], 16);

    status = bk_ble_create_db(&ble_db_cfg);

    return status;
}

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
#endif
#define BLE_VSN5_DEFAULT_MASTER_IDX      0


static void ble(int argc, char **argv)
{
	uint8_t adv_data[31];
	uint8_t actv_idx;

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
#if CFG_BLE_MASTER_ROLE_NUM
	if (os_strcmp(argv[1], "con_create") == 0)
	{
		ble_set_notice_cb(ble_notice_cb);
#if BLE_SDP_CLIENT
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
#endif
#endif
}


MSH_CMD_EXPORT(ble, ble command);
#endif
#endif
