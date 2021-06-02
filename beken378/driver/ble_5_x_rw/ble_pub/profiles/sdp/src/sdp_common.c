#include "rwip_config.h"             // SW configuration

#if (BLE_APP_PRESENT && (BLE_CENTRAL) && (BLE_SDP_CLIENT))
#include "sdp_comm.h"
#include "sdp_comm_pub.h"
#include "sdp_comm_task.h"
#include "app_task.h"
#include "rwip_task.h"
#include "prf_utils.h"
#include "prf.h"
#include "kernel_mem.h"
#include "attm.h"
#include "prf_utils.h"

#include "app_ble.h"

extern struct app_env_tag app_ble_env;
struct sdp_env_init_t sdp_env_init;

void sdp_notice_event(sdp_notice_t sdp_evt, void *param)
{
	extern ble_notice_cb_t ble_event_notice;
	ble_notice_t notice;
	switch(sdp_evt){
		case SDP_NTC_REG_FAILED:
			notice = BLE_5_SDP_REGISTER_FAILED;
			break;
		default:
			return;
			///break;
	}
	if (ble_event_notice) {
		ble_event_notice(notice, NULL);
	}
}

uint16_t sdp_add_profiles_num_get(uint8_t conidx)
{
	return sdp_env_init.add_profiles_nums[conidx];
}

struct prf_sdp_db_env * sdp_profiles_sdp_db_env(unsigned char conhdl,unsigned short handle)
{
	struct prf_sdp_db_env *sdp_db_env = NULL;
	struct prf_svc *svc;
	int i;

	for(i = 0;i < BLE_NB_PROFILES_ADD_MAX; i++ ){
		if(sdp_env_init.used_status[i] == USED_STATUS){
			if(sdp_env_init.sdp_env[i].conidx == conhdl){
				svc = &sdp_env_init.sdp_env[i].prf_db_env->sdp_cont->svc;
				if((svc->shdl <= handle) && (svc->ehdl >= handle)){
					sdp_db_env = sdp_env_init.sdp_env[i].prf_db_env;
					break;
				}
			}
		}
	}
	return sdp_db_env;
}

struct prf_sdp_db_env * sdp_profiles_sdp_db_env_from_uuid(unsigned char conhdl,uint8_t uuid_len,const uint8_t *uuid)
{
	struct prf_sdp_db_env *sdp_db_env = NULL;
	struct sdp_content *sdp_cont;
	struct prf_char_inf *chars_inf;
	int i,j;

	for(i = 0;i < BLE_NB_PROFILES_ADD_MAX; i++ ){
		if(sdp_env_init.used_status[i] == USED_STATUS){
			if(sdp_env_init.sdp_env[i].conidx == conhdl){
				sdp_cont = sdp_env_init.sdp_env[i].prf_db_env->sdp_cont;
				chars_inf = sdp_cont->chars_descs_inf.chars_inf;
				for(j = 0; j < sdp_cont->chars_nb; j++){
					if((chars_inf[j].uuid_len == uuid_len) && (memcmp(chars_inf[j].uuid,uuid,uuid_len) == 0)){
						sdp_db_env = sdp_env_init.sdp_env[i].prf_db_env;
						break;
					}
				}
			}
		}
	}
	return sdp_db_env;
}


void sdp_service_init(void)
{
	memset((uint8_t *)&sdp_env_init,0,sizeof(struct sdp_env_init_t));
	for(int i = 0; i < BLE_NB_PROFILES_ADD_MAX ; i++)
	{
		struct prf_sdp_db_env *prf_sdp_db = NULL;
		struct sdp_content *sdp_cont = NULL;
		sdp_env_init.used_status[i] = FREE_STATUS;
		sdp_env_init.sdp_env[i].conidx = 0xff;
		prf_sdp_db = kernel_malloc(sizeof(struct prf_sdp_db_env),KERNEL_MEM_NON_RETENTION);
	#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("struct prf_sdp_db_env size:%d,addr:0x%x \r\n",sizeof(struct prf_sdp_db_env),prf_sdp_db);
	#endif
		sdp_cont = kernel_malloc(sizeof(struct sdp_content),KERNEL_MEM_NON_RETENTION);
	#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("struct sdp_content size:%d,addr:0x%x \r\n",sizeof(struct sdp_content),sdp_cont);
	#endif
		sdp_env_init.sdp_env[i].prf_db_env  = prf_sdp_db;
		prf_sdp_db->sdp_cont = sdp_cont;
	}
}

uint8_t sdp_service_free_env_find(void)
{
	uint8_t free_idx;

	for(free_idx = 0; free_idx < BLE_NB_PROFILES_ADD_MAX ; free_idx++)
	{
		if(FREE_STATUS == sdp_env_init.used_status[free_idx])
		{
			return free_idx;
		}
	}

	return free_idx;
}

uint8_t check_enough_mem_add_service(uint32_t size)
{
	// KE_MEM_BLOCK_MAX
	if( kernel_check_malloc(size,KERNEL_MEM_NON_RETENTION))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

static uint8_t sdp_write_service_ntf_cfg_req(uint8_t conidx,uint16_t handle,uint16_t ntf_cfg,uint16_t seq_num)
{
	uint8_t ret = 0;
	uint8_t conhdl = app_ble_env.connections[conidx].conhdl;

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("func %s\r\n,state = %x,",__func__,kernel_state_get(KERNEL_BUILD_ID(TASK_BLE_APP,conidx)));
	bk_printf("conidx:%d,handle = 0x%04x,ntf_cfg = 0x%x\r\n",conidx,handle,ntf_cfg);
#endif

	if((kernel_state_get(KERNEL_BUILD_ID(TASK_BLE_APP,conidx)) != APPC_LINK_IDLE))
	{
		struct sdp_write_ntf_cfg_req *req = KERNEL_MSG_ALLOC(SDP_WRITE_NTF_CFG_REQ,
		                        prf_get_task_from_id(KERNEL_BUILD_ID(TASK_BLE_ID_SDP,conhdl)),
		                        KERNEL_BUILD_ID(TASK_BLE_APP,conidx),
		                        sdp_write_ntf_cfg_req);
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_ALL)
		bk_printf("dest = 0x%04x\r\n",prf_get_task_from_id(KERNEL_BUILD_ID(TASK_BLE_ID_SDP,conidx)));
#endif
		// Fill in the parameter structure
		req->conidx = conhdl;
		req->handle = handle;
		req->ntf_cfg = ntf_cfg;
		req->seq_num = seq_num;

		// Send the message
		kernel_msg_send(req);
	}
	else
	{
		ret = -1;
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_IMPO)
		bk_printf("ke_state_get(TASK_APPC) = %x\r\n",kernel_state_get(KERNEL_BUILD_ID(TASK_BLE_APP,conidx)));
#endif
	}

    return ret;
}


void sdp_discover_all_service(uint8_t conidx)
{
	uint8_t conhdl = app_ble_env.connections[conidx].conhdl;

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_IMPO)
	bk_printf("[%s]conidx:%x,conhdl:%d\r\n",__FUNCTION__,conidx,conhdl);
#endif

	struct gattc_sdp_svc_disc_cmd * svc_req = KERNEL_MSG_ALLOC_DYN(GATTC_SDP_SVC_DISC_CMD,
								            KERNEL_BUILD_ID(TASK_BLE_GATTC,conhdl),
								            KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx)),
								            ///KERNEL_BUILD_ID(TASK_BLE_APP,conidx),///TASK_BLE_APPM,
								            gattc_sdp_svc_disc_cmd,
								            ATT_UUID_16_LEN);

	//gatt request type: by UUID
	svc_req->operation         = GATTC_SDP_DISC_SVC_ALL;
	//start handle;
	svc_req->start_hdl        = ATT_1ST_REQ_START_HDL;
	//end handle
	svc_req->end_hdl          = ATT_1ST_REQ_END_HDL;
	// UUID search
	svc_req->uuid_len = ATT_UUID_16_LEN;
	/// operation sequence number
	//  svc_req->seq_num = 2;
	//set the first two bytes to the value array, LSB to MSB:Health Thermometer Service UUID first
	///co_write16p(&(svc_req->uuid[0]), 0x0000);
	svc_req->uuid[0] = (0x0000) & 0xff;
	svc_req->uuid[1] = (0x0000 >> 8) & 0xff;

	///sdp_env_init.add_profiles_nums[conidx] = 0;
	sdp_env_init.add_profiles_nums[0] = 0;
	//send the message to GATT, which will send back the response when it gets it
	kernel_msg_send(svc_req);
}

uint8_t sdp_enable_all_server_ntf_ind(uint8_t conidx,uint8_t  reset)
{
	bool more_enable = false;
	static uint8_t server_num = 0,chars_num = 0;

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]conidx:%d,reset:%d\r\n",__func__,conidx,reset);
#endif
	if(reset == 1)
	{
		server_num = 0;
		chars_num = 0;
	}

	for(; server_num < SDP_NB_SERVICE_INSTANCES_MAX;)
	{
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("server_num :%d,chars_num:%d,condix:%d\r\n",server_num,chars_num,sdp_env_init.sdp_env[server_num].conidx);
#endif
		//bk_printf("server_num = 0x%x,use_status = 0x%x\r\n",server_num,sdp_env_init.used_status[server_num] );
		//bk_printf("p_use_status = 0x%x\r\n",&Sdp_env[server_num].use_status);

		if((sdp_env_init.used_status[server_num]) == USED_STATUS)
		{
			if(sdp_env_init.sdp_env[server_num].conidx == conidx)
			{
				//UART_PRINTF("server_num = %d,chars_nb = %d\r\n",server_num,sdp_env_init.sdp_env[server_num].prf_db_env->sdp_cont->chars_nb);
				for(; chars_num < sdp_env_init.sdp_env[server_num].prf_db_env->sdp_cont->chars_nb;)
				{
					#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
					bk_printf("server_num = %d,chars_num = %d,prop = 0x%x\r\n",server_num,chars_num,sdp_env_init.sdp_env[server_num].prf_db_env->sdp_cont->chars_descs_inf.chars_inf[chars_num].prop);
					#endif
					if(sdp_env_init.sdp_env[server_num].prf_db_env->sdp_cont->chars_descs_inf.chars_inf[chars_num].prop & ATT_CHAR_PROP_IND)
					{
						sdp_write_service_ntf_cfg_req(conidx,sdp_env_init.sdp_env[server_num].prf_db_env->sdp_cont->chars_descs_inf.chars_inf[chars_num].val_hdl,PRF_CLI_START_IND,0xaa);
						more_enable = true;
						chars_num++;
						return more_enable;
					}
					else if(sdp_env_init.sdp_env[server_num].prf_db_env->sdp_cont->chars_descs_inf.chars_inf[chars_num].prop & ATT_CHAR_PROP_NTF)
					{
						sdp_write_service_ntf_cfg_req(conidx,sdp_env_init.sdp_env[server_num].prf_db_env->sdp_cont->chars_descs_inf.chars_inf[chars_num].val_hdl,PRF_CLI_START_NTF,0xaa);
						more_enable = true;
						chars_num++;
						return more_enable;
					}
					else
					{
						chars_num++;
					}
				}
			}
			server_num++;
			chars_num = 0;
		}
		else
		{
			more_enable = false;
			return more_enable;
		}
	}

    return more_enable;
}

void sdp_enable_rsp_send(struct sdp_env_tag *sdp_env, uint8_t conidx, uint8_t status)
{
	// Send APP the details of the discovered attributes on
	struct sdp_enable_rsp * rsp = KERNEL_MSG_ALLOC(SDP_ENABLE_RSP,
	                          prf_dst_task_get(&(sdp_env->prf_env) ,conidx),
	                          prf_src_task_get(&(sdp_env->prf_env) ,conidx),
	                          sdp_enable_rsp);

	rsp->status = status;
	kernel_msg_send(rsp);
}

void sdp_add_profiles(uint8_t conidx,struct prf_sdp_db_env *db_env)
{
	uint8_t conhdl = app_ble_env.connections[conidx].conhdl;
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("[%s]conidx:%x\r\n",__func__,conhdl);
#endif
	struct prf_sdp_db_env *env;
	struct gapm_profile_task_add_cmd *req = KERNEL_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
												TASK_BLE_GAPM,
												///KERNEL_BUILD_ID(TASK_BLE_APP,conidx),
												KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx)),
												gapm_profile_task_add_cmd,
												sizeof(struct prf_sdp_db_env));
	////Fill message
	req->operation = GAPM_PROFILE_TASK_ADD;
	req->sec_lvl = 0;//PERM(SVC_AUTH, ENABLE);
	req->prf_task_id = TASK_BLE_ID_SDP + db_env->prf_idx;///KERNEL_BUILD_ID((TASK_BLE_ID_SDP + conhdl),db_env->prf_idx);
	req->app_task = KERNEL_BUILD_ID(TASK_BLE_APP,BLE_APP_INITING_INDEX(conidx));//KERNEL_BUILD_ID(TASK_BLE_APP,conidx);
	req->start_hdl = 0; //req->start_hdl = 0; dynamically allocated
	req->role = PRF_CLIENT;
	env = (struct prf_sdp_db_env *)req->param;
	env->prf_idx  = db_env->prf_idx;
	env->sdp_cont  = db_env->sdp_cont;
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("env->sdp_cont.chars_nb = %d,prf_idx :%d\r\n",env->sdp_cont->chars_nb,env->prf_idx);
	bk_printf("req->prf_task_id: %d,prf_idx:%d\r\n",req->prf_task_id,db_env->prf_idx);
#endif
	sdp_env_init.add_profiles_nums[conidx]++;

	kernel_msg_send(req);
}


//extern struct prf_env_tag prf_env;
struct prf_sdp_db_env* sdp_extract_svc_info(uint8_t conhdl,struct gattc_sdp_svc_ind const *ind)
{
	struct prf_sdp_db_env *prf_db_env = NULL;
	uint8_t free_env_idx;
	uint16_t malloc_size;
	uint16_t length;

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_ALL)
	bk_printf("[%s]service uuid len = 0x%x,uuid = 0x",__func__,ind->uuid_len);
	for(int i = 0; i < ind->uuid_len; i++)
	{
		bk_printf("%02x",ind->uuid[ind->uuid_len - i - 1]);
	}
	bk_printf("\r\n");

	length =  ind->end_hdl - ind->start_hdl;
	bk_printf("start_hdl:%02d[0x%02x],end_hdl:%02d[0x%02x],length:%d\r\n",ind->start_hdl,ind->start_hdl,ind->end_hdl,ind->end_hdl,length);
#endif

	//Counters
	uint8_t svc_char,svc_char_cnt;
	uint8_t svc_desc,svc_desc_cnt;
	uint8_t fnd_att;
	/**  search svc_char_cnt and desc_cnt for alloc memory **/
	svc_char_cnt = 0;
	svc_desc_cnt = 0;

	for (fnd_att=0; fnd_att< (ind->end_hdl - ind->start_hdl); fnd_att++)
	{
		if(ind->info[fnd_att].att_type == GATTC_SDP_ATT_CHAR)
		{
			uint16_t val_hdl  = ind->info[fnd_att].att_char.handle;
			// check that value handle is in a valid range
			if((val_hdl <= ind->end_hdl) && (val_hdl > (ind->start_hdl + fnd_att)))
			{
				//Look over requested characteristics
				for (svc_char=0; svc_char < ind->end_hdl -ind->start_hdl ; svc_char++)
				{
					svc_char_cnt = (svc_char_cnt + 1) % SDP_NB_CHAR_INSTANCES_MAX;
					// find end of characteristic handle and discover descriptors
					do
					{
						fnd_att++;
						// found a descriptor
						if(ind->info[fnd_att].att_type == GATTC_SDP_ATT_DESC)
						{
							//Retrieve characteristic descriptor handle using UUID
							for(svc_desc = 0; svc_desc<ind->end_hdl - ind->start_hdl; svc_desc++)
							{
								// check if it's expected descriptor
								if ( svc_desc == svc_char)
								{
									svc_desc_cnt = (svc_desc_cnt + 1 ) % SDP_NB_DESC_INSTANCES_MAX;
									// search for next descriptor
									break;
								}
							}
						}
					}
					while(((ind->start_hdl+ 1 + fnd_att) <= ind->end_hdl)
						&& (ind->info[fnd_att].att_type != GATTC_SDP_ATT_CHAR)
						&& (ind->info[fnd_att].att_type != GATTC_SDP_INC_SVC));
					// return to previous valid value
					fnd_att--;
					// search next characteristic
					break;
				}
			}
		}
	}
    /**************************end search ************************************/
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("svc_char_cnt:%d,svc_desc_cnt:%d\r\n",svc_char_cnt,svc_desc_cnt);
#endif

	malloc_size = ((sizeof(struct prf_char_inf) * svc_char_cnt) + (sizeof(struct prf_char_desc_inf) * svc_desc_cnt));
	if(check_enough_mem_add_service(malloc_size))
	{
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_IMPO)
		bk_printf("not enough mem to store profile!!!!!\r\n");
#endif
		sdp_notice_event(SDP_NTC_REG_FAILED,NULL);
		return prf_db_env;;
	}

	free_env_idx = sdp_service_free_env_find();
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("free_env_idx = %d\r\n",free_env_idx);
#endif
	if(free_env_idx >= BLE_NB_PROFILES_ADD_MAX)
	{
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_IMPO)
		bk_printf("not enough env to store profile!!!!!\r\n");
#endif
		sdp_notice_event(SDP_NTC_REG_FAILED,NULL);
		return prf_db_env;
	}

//    bk_printf("total need = 0x%x\r\n",sizeof(struct  prf_char_inf ) * svc_char_cnt + svc_desc_cnt * sizeof( struct prf_char_desc_inf ));
/*************************alloc memory *******************************/
//    bk_printf("chars need = 0x%x\r\n",sizeof(struct prf_char_inf) * svc_char_cnt);
	struct prf_char_inf *chars = (struct prf_char_inf *) kernel_malloc(sizeof(struct prf_char_inf) * svc_char_cnt,KERNEL_MEM_NON_RETENTION);
//    bk_printf("descs need = 0x%x\r\n",sizeof(struct prf_char_desc_inf) * svc_desc_cnt);
	struct prf_char_desc_inf *descs = (struct prf_char_desc_inf *)kernel_malloc(sizeof(struct prf_char_desc_inf) * svc_desc_cnt ,KERNEL_MEM_ATT_DB);
//    bk_printf("chars_inf = 0x%08x,descs_inf = 0x%08x\r\n",chars,descs);
//    bk_printf("alloc memory END\r\n");
// add_svc_flag = 0;
	///if(true == 1)
	{
		prf_db_env = sdp_env_init.sdp_env[free_env_idx].prf_db_env;
		sdp_env_init.sdp_env[free_env_idx].conidx = conhdl;
		sdp_env_init.used_status[free_env_idx] = USED_STATUS;
		prf_db_env->prf_idx = free_env_idx;
		prf_db_env->sdp_cont->chars_descs_inf.chars_inf = chars;
		prf_db_env->sdp_cont->chars_descs_inf.descs_inf = descs;
		prf_db_env->sdp_cont->svc.shdl = ind->start_hdl;
		prf_db_env->sdp_cont->svc.ehdl = ind->end_hdl;
		prf_db_env->sdp_cont->svc.uuid_len = ind->uuid_len;
		memcpy(prf_db_env->sdp_cont->svc.uuid,ind->uuid,ind->uuid_len);

		svc_char_cnt = 0;
		svc_desc_cnt = 0;
		prf_db_env->sdp_cont->chars_nb = 0;
		prf_db_env->sdp_cont->char_idx = 0;
		prf_db_env->sdp_cont->descs_nb = 0;
		for (fnd_att=0; fnd_att< (ind->end_hdl - ind->start_hdl); fnd_att++)
		{
			if(ind->info[fnd_att].att_type == GATTC_SDP_ATT_CHAR)
			{
				uint16_t char_hdl = ind->start_hdl + 1 + fnd_att;
				uint16_t val_hdl  = ind->info[fnd_att].att_char.handle;
				uint8_t  val_prop = ind->info[fnd_att].att_char.prop;
				uint8_t  char_idx = fnd_att;
				// check that value handle is in a valid range
				if((val_hdl <= ind->end_hdl) && (val_hdl > (ind->start_hdl + fnd_att)))
				{
					// retrieve value index
					uint8_t val_idx = (val_hdl - ind->start_hdl - 1);
					//Look over requested characteristics
					for (svc_char=0; svc_char < ind->end_hdl -ind->start_hdl ; svc_char++)
					{
						//    bk_printf("-----------------------------------------------------\r\n");
						//     bk_printf("char uuid len = 0x%x ,svc_char = %d ",ind->info[val_idx].att.uuid_len,svc_char);
						//  UART_PRINTF("char uuid = 0x%04x \r\n",attm_convert_to16((uint8_t *)ind->info[val_idx].att.uuid,ind->info[val_idx].att.uuid_len));
						//      bk_printf("char uuid = 0x");
						//      for(int i = 0; i < ind->info[val_idx].att.uuid_len; i++)
						{
						//          bk_printf("%02x",ind->info[val_idx].att.uuid[ind->info[val_idx].att.uuid_len - i - 1]);
						}
						//      bk_printf("\r\n\r\n");
						//      bk_printf("char_hdl:%d[0x%02x],val_hdl:%d[0x%02x],fnd_att = 0x%x ,val_idx = 0x%x,val_prop = 0x%x\r\n",char_hdl,char_hdl,val_hdl,val_hdl,fnd_att,val_idx,val_prop);

#if (0)
						{
							bk_printf("Characteristic Properties: 0x%02x\r\n",val_prop);
							bk_printf("Broadcast :                %d\r\n",PERM_GET(val_prop << 8,BROADCAST));
							bk_printf("Read :                     %d\r\n",PERM_GET(val_prop << 8,RD));
							bk_printf("Write Without Response :   %d\r\n",PERM_GET(val_prop << 8,WRITE_COMMAND));
							bk_printf("Write :                    %d\r\n",PERM_GET(val_prop << 8,WRITE_REQ));
							bk_printf("Notify :                   %d\r\n",PERM_GET(val_prop << 8,NTF));
							bk_printf("Indicate :                 %d\r\n",PERM_GET(val_prop << 8,IND));
							bk_printf("Write Signed :             %d\r\n",PERM_GET(val_prop << 8,WRITE_SIGNED));
							bk_printf("Extended properties :      %d\r\n",PERM_GET(val_prop << 8,EXT));
						}
#endif

						//Save properties and handles
						chars[svc_char_cnt].uuid_len = ind->info[val_idx].att.uuid_len;
						memcpy(chars[svc_char_cnt].uuid,ind->info[val_idx].att.uuid,ind->info[val_idx].att.uuid_len);

						chars[svc_char_cnt].char_hdl       = char_hdl;
						chars[svc_char_cnt].val_hdl        = val_hdl;
						chars[svc_char_cnt].prop           = val_prop;

						svc_char_cnt++;
						prf_db_env->sdp_cont->chars_nb = svc_char_cnt;
						// find end of characteristic handle and discover descriptors
						do
						{
							fnd_att++;
							// found a descriptor
							if(ind->info[fnd_att].att_type == GATTC_SDP_ATT_DESC)
							{
								//Retrieve characteristic descriptor handle using UUID
								for(svc_desc = 0; svc_desc<ind->end_hdl - ind->start_hdl; svc_desc++)
								{
									// check if it's expected descriptor
									if ( svc_desc == svc_char)
									{
										//          bk_printf("svc_desc = 0x%x,desc uuid = 0x%02x uuid_len = 0x%x ",svc_desc,
										//     attm_convert_to16((uint8_t*)ind->info[fnd_att].att.uuid,ind->info[fnd_att].att.uuid_len),ind->info[fnd_att].att.uuid_len);

										descs[svc_desc_cnt].uuid_len = ind->info[fnd_att].att.uuid_len;
										memcpy(descs[svc_desc_cnt].uuid,ind->info[fnd_att].att.uuid,ind->info[fnd_att].att.uuid_len);

										descs[svc_desc_cnt].desc_hdl = ind->start_hdl + 1 + fnd_att;
										descs[svc_desc_cnt].char_code =  svc_char_cnt - 1;
										//            bk_printf("desc_hdl:%d[0x%02x] desc_char_cnt = 0x%x,val_idx = 0x%x\r\n",descs[svc_desc_cnt].desc_hdl,descs[svc_desc_cnt].desc_hdl,svc_char_cnt - 1,val_idx);
										// search for next descriptor
										svc_desc_cnt++;
										prf_db_env->sdp_cont->descs_nb = svc_desc_cnt;
										//break;
									}
								}
							}
						} while(((ind->start_hdl+ 1 + fnd_att) <= ind->end_hdl)
							&& (ind->info[fnd_att].att_type != GATTC_SDP_ATT_CHAR)
							&& (ind->info[fnd_att].att_type != GATTC_SDP_INC_SVC));
						// return to previous valid value
						fnd_att--;
						// previous handle was end of the characteristic
						chars[svc_char_cnt - 1].char_ehdl_off    = fnd_att - char_idx;
						// search next characteristic
						break;
					}
				}
			}
		}
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_ALL)
		bk_printf("%s  end\r\n",__func__);
#endif
	}

    return prf_db_env;
}


/////////////////////////////////////////////////////////////////////////////////////////
static uint8_t sdp_init (struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task, uint8_t sec_lvl,  void* params)
{
	bk_printf("[%s]app_task:0x%x \r\n",__func__,app_task);

	struct prf_sdp_db_env *prf_db_env = (struct prf_sdp_db_env *)params;
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_ALL)
	bk_printf("prf_db_env->prf_idx = %x\r\n",prf_db_env->prf_idx);
	bk_printf("prf_db_env->sdp_cont->chars_nb = %x\r\n",prf_db_env->sdp_cont->chars_nb);
#endif
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_ALL)
	for(int i =0; i < prf_db_env->sdp_cont->chars_nb; i++)
	{
		uint16_t char_hdl,val_hdl;
		char_hdl = prf_db_env->sdp_cont->chars_descs_inf.chars_inf[i].char_hdl;
		val_hdl = prf_db_env->sdp_cont->chars_descs_inf.chars_inf[i].val_hdl;
		uint8_t uuid_len = prf_db_env->sdp_cont->chars_descs_inf.chars_inf[i].uuid_len;

		bk_printf("char uuid = 0x");
		for(uint8_t len = 0;len < prf_db_env->sdp_cont->chars_descs_inf.chars_inf[i].uuid_len;len++)
		{
			bk_printf("%02x",prf_db_env->sdp_cont->chars_descs_inf.chars_inf[i].uuid[uuid_len - len - 1]);
		}
		bk_printf(",");
		bk_printf("char_hdl:%d[0x%02x],val_hdl:%d[0x%02x],prop:0x%02x\r\n",char_hdl,char_hdl,val_hdl,val_hdl,
		                                                       prf_db_env->sdp_cont->chars_descs_inf.chars_inf[i].prop);
	}
#endif

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("prf_db_env->sdp_cont.descs_nb = %x\r\n",prf_db_env->sdp_cont->descs_nb);
#endif
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_ALL)
	for(int i =0; i < prf_db_env->sdp_cont->descs_nb; i++)
	{
		uint8_t uuid_len = prf_db_env->sdp_cont->chars_descs_inf.descs_inf[i].uuid_len;
		uint16_t desc_hdl = prf_db_env->sdp_cont->chars_descs_inf.descs_inf[i].desc_hdl;

		bk_printf("descs uuid = 0x");
		for(uint8_t len = 0;len < prf_db_env->sdp_cont->chars_descs_inf.descs_inf[i].uuid_len;len++)
		{
			bk_printf("%02x",prf_db_env->sdp_cont->chars_descs_inf.descs_inf[i].uuid[uuid_len - len - 1]);
		}
		bk_printf(",desc_hdl:%d[0x%02x]\r\n",desc_hdl,desc_hdl);
	}
#endif

	//-------------------- allocate memory required for the profile  ---------------------
	struct sdp_env_tag* sdp_env = &sdp_env_init.sdp_env[prf_db_env->prf_idx];
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("sdp_env->conidx = %x\r\n",sdp_env->conidx);
#endif
	// allocate  required environment variable
	env->env = (prf_env_t*) sdp_env;
	sdp_env->prf_env.app_task = app_task
	                            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
	sdp_env->prf_env.prf_task = env->task | PERM(PRF_MI, ENABLE);
	// initialize environment variable
	env->role                   = PRF_CLIENT;
	env->id                     = (KERNEL_BUILD_ID((TASK_BLE_ID_SDP + prf_db_env->prf_idx),0));/// KERNEL_BUILD_ID(TASK_BLE_ID_SDP,prf_db_env->prf_idx);
	*start_hdl = sdp_env->prf_db_env->sdp_cont->svc.shdl;
	bk_printf("env:%p,%x,sdp_env:%p\r\n",&env->env,env->env,sdp_env);
	sdp_task_init(&(env->desc),sdp_env);
	sdp_env->operation = NULL;
	bk_printf("[sdp_init]env->task:%d,conhdl:%d,cur_state:%x\r\n",env->task,sdp_env->conidx,kernel_state_get(KERNEL_BUILD_ID(env->task, 0)));
	// service is ready, go into an Idle state
	kernel_state_set(KERNEL_BUILD_ID(env->task,0), SDP_IDLE);

	if(0 != prf_db_env->sdp_cont->descs_nb)
	{
		uint16_t shdl,ehdl;
		shdl = sdp_env->prf_db_env->sdp_cont->svc.shdl;
		ehdl = sdp_env->prf_db_env->sdp_cont->svc.ehdl;
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("prf_register_atthdl2gatt start_hdl:%d[0x%02x],end_hdl:%d[0x%02x]\r\n",shdl,shdl,ehdl,ehdl);
#endif
		prf_register_atthdl2gatt(&(sdp_env->prf_env),sdp_env->conidx, &sdp_env->prf_db_env->sdp_cont->svc);
	}
	if(sdp_env_init.add_profiles_nums[sdp_env->conidx] > 0)
	{
		sdp_env_init.add_profiles_nums[sdp_env->conidx]--;
	}

#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("%s end!!\r\n",__func__);
#endif
	return GAP_ERR_NO_ERROR;
}

/**
 ****************************************************************************************
 * @brief Destruction of the BASC module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]	 env		Collector or Service allocated environment data.
 ****************************************************************************************
 */
static void sdp_destroy(struct prf_task_env* env)
{
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_IMPO)
	bk_printf("sdp_destroy START env->id = 0x%x,role = %x\r\n",env->id,env->role);
#endif
	struct sdp_env_tag* sdp_env = (struct sdp_env_tag*) env->env;

	if(sdp_env_init.sdp_need_dis_flag[sdp_env->prf_db_env->prf_idx] == SDP_SERVICE_NO_NEED_DISCOVER)
	{
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_IMPO)
		bk_printf("SDP_SERVICE_NO_NEED_DISCOVER cur connected!!");
#endif
		return;
	}
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("used_status = 0x%x,prf_idx:%d\r\n",sdp_env_init.used_status[sdp_env->prf_db_env->prf_idx],sdp_env->prf_db_env->prf_idx);
#endif
	sdp_env_init.used_status[sdp_env->prf_db_env->prf_idx] = INVALID_STATUS;////

	// cleanup environment variable for each task instances
	if(sdp_env->operation != NULL)
	{
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("sdp_destroy sdp_env->operation = 0x%x\r\n",sdp_env->operation);
#endif
		sdp_env->operation = NULL;
	}
	// free profile environment variables
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("sdp_destroy end\r\n");
#endif
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void sdp_create(struct prf_task_env* env, uint8_t conidx)
{
	struct sdp_env_tag* sdp_env = (struct sdp_env_tag*)env->env;
	struct prf_sdp_db_env  *prf_db_env = sdp_env->prf_db_env;
	bk_printf("[sdp_create]env->task:%d,sdp_env->conidx:%d,conhdl:%d,cur_state:%x\r\n",env->task,sdp_env->conidx,conidx,
		kernel_state_get(KERNEL_BUILD_ID(env->task, 0)));

	if(sdp_env->conidx == conidx){
		/* Put BAS Client in Idle state */
		kernel_state_set(KERNEL_BUILD_ID(env->task, 0), SDP_IDLE);
	}
}

/**
 ****************************************************************************************
 * @brief Handles Disconnection
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 ****************************************************************************************
 */
static void sdp_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_IMPO)
	bk_printf("sdp_cleanup env->id = 0x%x,role = %x,conidx = %x reason = 0x%x\r\n",env->id,env->role,conidx,reason);
#endif
	struct sdp_env_tag* sdp_env = (struct sdp_env_tag*) env->env;
	uint8_t idx;

	if(sdp_env->conidx != conidx)
	{
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_IMPO)
		bk_printf("sdp_cleanup not cur connected!!\r\n");
#endif
		return;
	}

	if(sdp_env_init.sdp_need_dis_flag[sdp_env->prf_db_env->prf_idx] == SDP_SERVICE_NO_NEED_DISCOVER)
	{
		bk_printf("SDP_SERVICE_NO_NEED_DISCOVER cur connected!!");
		kernel_state_set(KERNEL_BUILD_ID(env->task,0), SDP_FREE);
		return;
	}
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("used_status = 0x%x,prf_idx:%d\r\n",sdp_env_init.used_status[sdp_env->prf_db_env->prf_idx],sdp_env->prf_db_env->prf_idx);
#endif
	sdp_env_init.used_status[sdp_env->prf_db_env->prf_idx] = FREE_STATUS;
	env->id = TASK_BLE_ID_INVALID;
	sdp_env_init.sdp_env[sdp_env->prf_db_env->prf_idx].conidx = 0xff;

	for(idx = 0; idx < SDP_IDX_MAX ; idx++)
	{
		if(0 != sdp_env->prf_db_env->sdp_cont->descs_nb)
		{
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
			bk_printf("descs_nb:%d\r\n",sdp_env->prf_db_env->sdp_cont->descs_nb);
#endif
			sdp_env->prf_db_env->sdp_cont->descs_nb = 0;
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
			bk_printf("descs_inf = 0x%02x\r\n",sdp_env->prf_db_env->sdp_cont->chars_descs_inf.descs_inf);
#endif
			if(NULL != sdp_env->prf_db_env->sdp_cont->chars_descs_inf.descs_inf)
			{
				kernel_free(sdp_env->prf_db_env->sdp_cont->chars_descs_inf.descs_inf);
				sdp_env->prf_db_env->sdp_cont->chars_descs_inf.descs_inf = NULL;
			}
		}
	}

	// clean-up environment variable allocated for task instance
	if(sdp_env->operation != NULL)
	{
		struct kernel_msg *msg = kernel_msg2param(sdp_env->operation);
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("operation = 0x%08x\r\n",(uint32_t)sdp_env->operation);
		bk_printf("msgid = 0x%02x,dest_id = 0x%02x,src_id = 0x%02x\r\n",msg->id,msg->dest_id,msg->src_id);
#endif
		kernel_free(sdp_env->operation);
		sdp_env->operation = NULL;
	}

	if(0 != sdp_env->prf_db_env->sdp_cont->chars_nb)
	{
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
		bk_printf("chars_nb:%d\r\n",sdp_env->prf_db_env->sdp_cont->chars_nb);
#endif
		if(NULL != sdp_env->prf_db_env->sdp_cont->chars_descs_inf.chars_inf)
		{
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
			bk_printf("chars_inf = 0x%02x\r\n",sdp_env->prf_db_env->sdp_cont->chars_descs_inf.chars_inf);
#endif
			kernel_free(sdp_env->prf_db_env->sdp_cont->chars_descs_inf.chars_inf);
			sdp_env->prf_db_env->sdp_cont->chars_descs_inf.chars_inf = NULL;
		}
		sdp_env->prf_db_env->sdp_cont->chars_nb = 0;
	}
#if BLE_APP_SDP_DBG_CHECK(BLE_APP_SDP_WARN)
	bk_printf("sdp_cleanup end\r\n");
#endif
	/* Put BAS Client in Free state */
	kernel_state_set(KERNEL_BUILD_ID(env->task,0), SDP_FREE);
}


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
/// BASC Task interface required by profile manager
static const struct prf_task_cbs sdp_itf =
{
	sdp_init,
	sdp_destroy,
	sdp_create,
	sdp_cleanup,
};

/*
 * GLOBAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */
const struct prf_task_cbs* sdp_prf_itf_get(void)
{
	return &sdp_itf;
}


#endif

