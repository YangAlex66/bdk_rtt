#include "ble_wifi_port.h"
#include "sys_config.h"


extern int sta_check_tim(uint32_t a_tim, uint16_t aid);
void check_bcn_pm_on_rxl(uint32_t tim, struct vif_info_tag *vif_entry)
{
#if NX_POWERSAVE && (RF_USE_POLICY == BLE_DEFAULT_WIFI_REQUEST)
	uint16_t aid = get_sta_aid(vif_entry);
    int moredata = 0;
	
    do
    {
        // Check if unicast traffic is present
        if ((tim != 0) && (sta_check_tim(tim, aid)))
        {
            moredata = 1;
        }
    }while(0);

    ///bk_printf("[BCN]more data check:%d\r\n",moredata);
    wifi_beacon_info_sender((moredata == 0) ? BEACON_IND_NO_MOREDATA : BEACON_IND_MOREDATA, 0);
#endif
}



int wifi_notice_ble_status(int evt,unsigned int param,unsigned int param2)
{
#if (RF_USE_POLICY == BLE_DEFAULT_WIFI_REQUEST)
	int ret = 0;
	switch(evt){
		case WN_TX_CFM_ISR_EVT:
		{
	    	wifi_send_data_done();
		}
		break;
		case WN_TX_RESET_EVT:
		{
			wifi_send_data_reset();
		}
		break;
		case WN_NEXT_TBTT_CALC_EVT:
		{
			calc_next_tbtt_timer();
		}
		break;
		case WN_NX_MULTI_ROLE_CAL_BCN_DTIM_PER:
		{
			wifi_ps_cal_bcn_dtim_period(param,param2);
		}break;
		case WN_STA_BEACON_IND_MISS:
		{
			wifi_beacon_info_sender(BEACON_IND_MISS,param);
		}
		break;
		case WN_HALMAC_INIT:
		{
    		nxmac_tsf_mgt_disable_setf(0);
			ret = 1;
		}
		break;
		case WN_MM_HW_INFO_SET:
		{
		    nxmac_mac_addr_hi_mask_set(0);
		    nxmac_gen_int_ack_clear(MM_TBTT_IRQ_MASK);
		    nxmac_gen_int_enable_set(nxmac_gen_int_enable_get() | MM_TBTT_IRQ_MASK);
		}
		break;
		case WN_MM_SET_BEACON_INT_EVT:
		{
			nxmac_beacon_int_setf(param);
			wifi_ps_cal_bcn_int(param2);
			nxmac_imp_tbtt_period_setf(16);
		}
		break;
		case WN_IND_MOREDATA_EVT:
		{
			check_bcn_pm_on_rxl(param, param2);
		}break;
		default:
			break;
	}
	return ret;
#else
	return 0;
#endif
}

int wn_txl_hd_pending_is_allow(uint8_t flag,void *first_thd,
	                          void *last_thd,
	                          uint8_t access_category)
{
	int is_allow = 1;
	
#if (RF_USE_POLICY == BLE_DEFAULT_WIFI_REQUEST)
	// Chain the THD to the MAC HW
	if(!flag)
	{
		wifi_send_data(first_thd, last_thd, access_category);
		is_allow = 0;
	}
#endif

	return is_allow;
}






