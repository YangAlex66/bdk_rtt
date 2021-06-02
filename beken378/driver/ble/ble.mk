NAME := ble

$(NAME)_TYPE := kernel

-include $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/.config

WPA_VERSION := wpa_supplicant-2.9
ifeq ($(CFG_USE_WPA_29),0)
WPA_VERSION := hostapd-2.5
endif

$(NAME)_INCLUDES := ../../app/standalone-ap \
					../../app/standalone-station \
					../../driver/sdio \
					../../driver/uart \
					../../driver/sys_ctrl \
					../../driver/gpio \
					../../driver/common/reg \
					../../driver/dma \
					../../driver/intc \
					../../driver/phy \
					../../driver/rc_beken \
					../../driver/general_dma \
					../../driver/spidma \
					../../driver/rw_pub \
					../../driver/wdt \
					../../func/sdio_intf \
					../../func/power_save \
					../../func/temp_detect \
					../../func/saradc_intf \
					../../func/spidma_intf \
					../../func/ethernet_intf \
					../../func/rwnx_intf \
					../../func/rf_test \
					../../func/joint_up

$(NAME)_INCLUDES += ../../ip/ke \
					../../ip/mac \
					../../ip/lmac/src/hal \
					../../ip/lmac/src/mm \
					../../ip/lmac/src/ps \
					../../ip/lmac/src/rd \
					../../ip/lmac/src/rwnx \
					../../ip/lmac/src/rx \
					../../ip/lmac/src/scan \
					../../ip/lmac/src/sta \
					../../ip/lmac/src/tx \
					../../ip/lmac/src/vif \
					../../ip/lmac/src/rx/rxl \
					../../ip/lmac/src/tx/txl \
					../../ip/lmac/src/tpc \
					../../ip/lmac/src/tdls \
					../../ip/lmac/src/p2p \
					../../ip/lmac/src/chan \
					../../ip/lmac/src/td \
					../../ip/umac/src/bam \
					../../ip/umac/src/llc \
					../../ip/umac/src/me \
					../../ip/umac/src/rxu \
					../../ip/umac/src/scanu \
					../../ip/umac/src/sm \
					../../ip/umac/src/txu \
					../../ip/umac/src/apm \
					../../ip/umac/src/mesh \
					../../ip/umac/src/rc

$(NAME)_INCLUDES += . \
					beken_ble_sdk/controller/include \
					beken_ble_sdk/hci/include \
					beken_ble_sdk/host/include \
					beken_ble_sdk/sys/include \
					config \
					modules/app/api \
					modules/gernel_api \
					modules/mesh_model/ali \
					plactform/arch \
					plactform/driver/ble_icu \
					plactform/driver/ir \
					plactform/driver/reg \
					plactform/driver/sys_ctrl \
					plactform/driver/uart \
					plactform/include \
					plactform/modules/include \
					profiles/comm/api \
					profiles/prf/include \
					profiles/sdp/api \
					beken_ble_sdk/mesh/include \
					beken_ble_sdk/mesh/src/dbg \
					beken_ble_sdk/mesh/src/models/include 

$(NAME)_INCLUDES += ../../func/$(WPA_VERSION)/src \
					../../func/$(WPA_VERSION)/src/ap \
					../../func/$(WPA_VERSION)/src/utils \
					../../func/$(WPA_VERSION)/src/common \
					../../func/$(WPA_VERSION)/src/drivers \
					../../func/$(WPA_VERSION)/bk_patch \
					../../func/$(WPA_VERSION)/hostapd \
					../../func/$(WPA_VERSION)/wpa_supplicant

#ble lib
$(NAME)_SOURCES +=  beken_ble_sdk/controller/src/ea/ea.c \
					beken_ble_sdk/controller/src/em/em_buf.c \
					beken_ble_sdk/controller/src/llc/llc.c \
					beken_ble_sdk/controller/src/llc/llc_ch_asses.c \
					beken_ble_sdk/controller/src/llc/llc_hci.c \
					beken_ble_sdk/controller/src/llc/llc_llcp.c \
					beken_ble_sdk/controller/src/llc/llc_task.c \
					beken_ble_sdk/controller/src/llc/llc_util.c \
					beken_ble_sdk/controller/src/lld/lld.c \
					beken_ble_sdk/controller/src/lld/lld_evt.c \
					beken_ble_sdk/controller/src/lld/lld_pdu.c \
					beken_ble_sdk/controller/src/lld/lld_sleep.c \
					beken_ble_sdk/controller/src/lld/lld_util.c \
					beken_ble_sdk/controller/src/lld/lld_wlcoex.c \
					beken_ble_sdk/controller/src/llm/llm.c \
					beken_ble_sdk/controller/src/llm/llm_hci.c \
					beken_ble_sdk/controller/src/llm/llm_task.c \
					beken_ble_sdk/controller/src/llm/llm_util.c \
					beken_ble_sdk/hci/ahi/ahi.c \
					beken_ble_sdk/hci/ahi/ahi_task.c \
					beken_ble_sdk/hci/h4tl.c \
					beken_ble_sdk/hci/hci.c \
					beken_ble_sdk/hci/hci_fc.c \
					beken_ble_sdk/hci/hci_msg.c \
					beken_ble_sdk/hci/hci_tl.c \
					beken_ble_sdk/hci/hci_util.c \
					beken_ble_sdk/host/src/attc/attc.c \
					beken_ble_sdk/host/src/attm/attm.c \
					beken_ble_sdk/host/src/attm/attm_db.c \
					beken_ble_sdk/host/src/atts/atts.c \
					beken_ble_sdk/host/src/gapc/gapc.c \
					beken_ble_sdk/host/src/gapc/gapc_hci.c \
					beken_ble_sdk/host/src/gapc/gapc_sig.c \
					beken_ble_sdk/host/src/gapc/gapc_task.c \
					beken_ble_sdk/host/src/gapm/gapm.c \
					beken_ble_sdk/host/src/gapm/gapm_hci.c \
					beken_ble_sdk/host/src/gapm/gapm_task.c \
					beken_ble_sdk/host/src/gapm/gapm_util.c \
					beken_ble_sdk/host/src/gattc/gattc.c \
					beken_ble_sdk/host/src/gattc/gattc_task.c \
					beken_ble_sdk/host/src/gattm/gattm.c \
					beken_ble_sdk/host/src/gattm/gattm_task.c \
					beken_ble_sdk/host/src/l2cc/l2cc.c \
					beken_ble_sdk/host/src/l2cc/l2cc_lecb.c \
					beken_ble_sdk/host/src/l2cc/l2cc_pdu.c \
					beken_ble_sdk/host/src/l2cc/l2cc_sig.c \
					beken_ble_sdk/host/src/l2cc/l2cc_task.c \
					beken_ble_sdk/host/src/l2cm/l2cm.c \
					beken_ble_sdk/host/src/smpc/smpc.c \
					beken_ble_sdk/host/src/smpc/smpc_api.c \
					beken_ble_sdk/host/src/smpc/smpc_crypto.c \
					beken_ble_sdk/host/src/smpc/smpc_util.c \
					beken_ble_sdk/host/src/smpm/smpm_api.c \
					beken_ble_sdk/sys/src/co/rwble.c \
					beken_ble_sdk/sys/src/co/rwble_hl.c \
					beken_ble_sdk/sys/src/co/rwip.c \
					beken_ble_sdk/sys/src/common/common_list.c \
					beken_ble_sdk/sys/src/common/common_utils.c \
					beken_ble_sdk/sys/src/ecc_p256/ecc_p256.c \
					beken_ble_sdk/sys/src/ke/kernel.c \
					beken_ble_sdk/sys/src/ke/kernel_event.c \
					beken_ble_sdk/sys/src/ke/kernel_mem.c \
					beken_ble_sdk/sys/src/ke/kernel_msg.c \
					beken_ble_sdk/sys/src/ke/kernel_queue.c \
					beken_ble_sdk/sys/src/ke/kernel_task.c \
					beken_ble_sdk/sys/src/ke/kernel_timer.c \
					beken_ble_sdk/sys/src/nvds/nvds.c

#ble mesh lib
ifeq ($(CFG_SUPPORT_BLE_MESH),1)
$(NAME)_SOURCES +=  beken_ble_sdk/mesh/src/aes/aes.c \
					beken_ble_sdk/mesh/src/aes/aes_ccm.c \
					beken_ble_sdk/mesh/src/aes/aes_cmac.c \
					beken_ble_sdk/mesh/src/aes/aes_k1.c \
					beken_ble_sdk/mesh/src/aes/aes_k2.c \
					beken_ble_sdk/mesh/src/aes/aes_k3.c \
					beken_ble_sdk/mesh/src/aes/aes_k4.c \
					beken_ble_sdk/mesh/src/aes/aes_s1.c \
					beken_ble_sdk/mesh/src/bcn/m_bcn.c \
					beken_ble_sdk/mesh/src/bearer/m_bearer.c \
					beken_ble_sdk/mesh/src/bearer/m_bearer_adv.c \
					beken_ble_sdk/mesh/src/bearer/m_bearer_gatt.c \
					beken_ble_sdk/mesh/src/common/mesh.c \
					beken_ble_sdk/mesh/src/common/mesh_log.c \
					beken_ble_sdk/mesh/src/common/mesh_tb.c \
					beken_ble_sdk/mesh/src/common/mesh_tb_buf.c \
					beken_ble_sdk/mesh/src/common/mesh_tb_sec.c \
					beken_ble_sdk/mesh/src/common/mesh_tb_timer.c \
					beken_ble_sdk/mesh/src/dbg/m_dbg.c \
					beken_ble_sdk/mesh/src/dbg/m_dbg_bcn.c \
					beken_ble_sdk/mesh/src/dbg/m_dbg_buf.c \
					beken_ble_sdk/mesh/src/dbg/m_dbg_data.c \
					beken_ble_sdk/mesh/src/dbg/m_dbg_friend.c \
					beken_ble_sdk/mesh/src/dbg/m_dbg_key.c \
					beken_ble_sdk/mesh/src/dbg/m_dbg_lay.c \
					beken_ble_sdk/mesh/src/dbg/m_dbg_lpn.c \
					beken_ble_sdk/mesh/src/dbg/m_dbg_mio.c \
					beken_ble_sdk/mesh/src/dbg/m_dbg_prov.c \
					beken_ble_sdk/mesh/src/dbg/m_dbg_sec.c \
					beken_ble_sdk/mesh/src/dbg/m_dbg_state.c \
					beken_ble_sdk/mesh/src/dbg/m_dbg_store.c \
					beken_ble_sdk/mesh/src/dbg/m_dbg_timer.c \
					beken_ble_sdk/mesh/src/fnd/m_fnd.c \
					beken_ble_sdk/mesh/src/fnd/m_fnd_confc.c \
					beken_ble_sdk/mesh/src/fnd/m_fnd_confs.c \
					beken_ble_sdk/mesh/src/fnd/m_fnd_hlths.c \
					beken_ble_sdk/mesh/src/lay/m_lay.c \
					beken_ble_sdk/mesh/src/lay/m_lay_access.c \
					beken_ble_sdk/mesh/src/lay/m_lay_friend.c \
					beken_ble_sdk/mesh/src/lay/m_lay_hb.c \
					beken_ble_sdk/mesh/src/lay/m_lay_lpn.c \
					beken_ble_sdk/mesh/src/lay/m_lay_ltrans.c \
					beken_ble_sdk/mesh/src/lay/m_lay_net.c \
					beken_ble_sdk/mesh/src/lay/m_lay_proxy.c \
					beken_ble_sdk/mesh/src/lay/m_lay_utrans.c \
					beken_ble_sdk/mesh/src/lld/lld_adv_test.c \
					beken_ble_sdk/mesh/src/mal/mal.c \
					beken_ble_sdk/mesh/src/mal/mal_adv.c \
					beken_ble_sdk/mesh/src/mal/mal_con.c \
					beken_ble_sdk/mesh/src/mal/mal_djob.c \
					beken_ble_sdk/mesh/src/mal/mal_sec.c \
					beken_ble_sdk/mesh/src/mal/mal_task.c \
					beken_ble_sdk/mesh/src/mal/mal_timer.c \
					beken_ble_sdk/mesh/src/prov/m_prov.c \
					beken_ble_sdk/mesh/src/prov/m_prov_adv_trans.c \
					beken_ble_sdk/mesh/src/prov/m_prov_bearer.c \
					beken_ble_sdk/mesh/src/tb/m_tb.c \
					beken_ble_sdk/mesh/src/tb/m_tb_friend.c \
					beken_ble_sdk/mesh/src/tb/m_tb_key.c \
					beken_ble_sdk/mesh/src/tb/m_tb_lpn.c \
					beken_ble_sdk/mesh/src/tb/m_tb_mio.c \
					beken_ble_sdk/mesh/src/tb/m_tb_state.c \
					beken_ble_sdk/mesh/src/tb/m_tb_store_nvds.c
endif

#ble pub
$(NAME)_SOURCES +=  ble.c \
					modules/app/src/app_ble.c \
					modules/app/src/app_comm.c \
					modules/app/src/app_sdp.c \
					modules/app/src/app_sec.c \
					modules/app/src/app_task.c \
					plactform/driver/ble_icu/ble_icu.c \
					plactform/driver/uart/ble_uart.c \
					plactform/modules/arch/ble_arch_main.c \
					plactform/modules/common/RomCallFlash.c \
					plactform/modules/dbg/dbg.c \
					plactform/modules/dbg/dbg_mwsgen.c \
					plactform/modules/dbg/dbg_swdiag.c \
					plactform/modules/dbg/dbg_task.c \
					plactform/modules/rf/src/ble_rf_xvr.c \
					profiles/comm/src/comm.c \
					profiles/comm/src/comm_task.c \
					profiles/prf/src/prf.c \
					profiles/prf/src/prf_utils.c \
					profiles/sdp/src/sdp_service.c \
					profiles/sdp/src/sdp_service_task.c

#ble mesh pub
ifeq ($(CFG_SUPPORT_BLE_MESH),1)
$(NAME)_SOURCES +=  beken_ble_sdk/mesh/src/mesh_api/mesh_api.c \
					beken_ble_sdk/mesh/src/mesh_api/mesh_api_msg.c \
					beken_ble_sdk/mesh/src/mesh_api/mesh_param_int.c \
					beken_ble_sdk/mesh/src/mesh_api/mm_api.c \
					beken_ble_sdk/mesh/src/mesh_api/mm_api_msg.c \
					beken_ble_sdk/mesh/src/mesh_api/m_api.c \
					beken_ble_sdk/mesh/src/mesh_api/m_api_msg.c \
					beken_ble_sdk/mesh/src/models/common/mm_route.c \
					beken_ble_sdk/mesh/src/models/common/mm_tb.c \
					beken_ble_sdk/mesh/src/models/common/mm_tb_bind.c \
					beken_ble_sdk/mesh/src/models/common/mm_tb_replay.c \
					beken_ble_sdk/mesh/src/models/common/mm_tb_state.c \
					beken_ble_sdk/mesh/src/models/gens/mm_gens.c \
					beken_ble_sdk/mesh/src/models/gens/mm_gens_bat.c \
					beken_ble_sdk/mesh/src/models/gens/mm_gens_dtt.c \
					beken_ble_sdk/mesh/src/models/gens/mm_gens_loc.c \
					beken_ble_sdk/mesh/src/models/gens/mm_gens_lvl.c \
					beken_ble_sdk/mesh/src/models/gens/mm_gens_oo.c \
					beken_ble_sdk/mesh/src/models/gens/mm_gens_plvl.c \
					beken_ble_sdk/mesh/src/models/gens/mm_gens_poo.c \
					beken_ble_sdk/mesh/src/models/gens/mm_gens_prop.c \
					beken_ble_sdk/mesh/src/models/lightc/mm_lightc.c \
					beken_ble_sdk/mesh/src/models/lightc/mm_lightc_ctl.c \
					beken_ble_sdk/mesh/src/models/lightc/mm_lightc_hsl.c \
					beken_ble_sdk/mesh/src/models/lightc/mm_lightc_ln.c \
					beken_ble_sdk/mesh/src/models/lightc/mm_lightc_xyl.c \
					beken_ble_sdk/mesh/src/models/lights/mm_lights.c \
					beken_ble_sdk/mesh/src/models/lights/mm_lights_ctl.c \
					beken_ble_sdk/mesh/src/models/lights/mm_lights_hsl.c \
					beken_ble_sdk/mesh/src/models/lights/mm_lights_ln.c \
					beken_ble_sdk/mesh/src/models/Scenes/m_fnd_Scenes.c \
					beken_ble_sdk/mesh/src/models/transition_time/m_fnd_generic_transition_time.c \
					beken_ble_sdk/mesh/src/models/vendor/mm_vendors.c \
					beken_ble_sdk/mesh/src/models/vendor/mm_vendor_midea.c \
					modules/app/src/app_mesh.c \
					modules/app/src/app_mm_msg.c \
					modules/gernel_api/mesh_general_api.c \
					modules/mesh_model/ali/app_light_ali_server.c
endif
