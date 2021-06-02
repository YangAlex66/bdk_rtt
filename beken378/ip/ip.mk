NAME := ip

$(NAME)_TYPE := kernel

-include $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/.config

WPA_VERSION := wpa_supplicant_2_9
ifeq ($(CFG_USE_WPA_29),0)
WPA_VERSION := hostapd-2.5
endif

$(NAME)_INCLUDES := ke \
					mac \
					lmac/src/hal \
					lmac/src/mm \
					lmac/src/ps \
					lmac/src/rd \
					lmac/src/rwnx \
					lmac/src/rx \
					lmac/src/scan \
					lmac/src/sta \
					lmac/src/tx \
					lmac/src/vif \
					lmac/src/rx/rxl \
					lmac/src/tx/txl \
					lmac/src/tpc \
					lmac/src/tdls \
					lmac/src/p2p \
					lmac/src/chan \
					lmac/src/td \
					umac/src/bam \
					umac/src/llc \
					umac/src/me \
					umac/src/rxu \
					umac/src/scanu \
					umac/src/sm \
					umac/src/txu \
					umac/src/apm \
					umac/src/mesh \
					umac/src/rc 
					
$(NAME)_INCLUDES += ../app/standalone-ap \
                    ../app/standalone-station \
					../driver/sdio \
                    ../driver/uart \
                    ../driver/sys_ctrl \
                    ../driver/gpio \
                    ../driver/common/reg \
                    ../driver/dma \
                    ../driver/intc \
                    ../driver/phy \
                    ../driver/rc_beken \
                    ../driver/general_dma \
                    ../driver/spidma \
                    ../driver/rw_pub \
                    ../func/sdio_intf \
                    ../func/power_save \
					../func/temp_detect \
                    ../func/spidma_intf \
                    ../func/ethernet_intf \
                    ../func/rwnx_intf \
					../func/rf_test \
					../func/joint_up
					
$(NAME)_INCLUDES += ../func/$(WPA_VERSION)/src/utils \
                    ../func/$(WPA_VERSION)/src \
                    ../func/$(WPA_VERSION)/bk_patch \
                    ../func/$(WPA_VERSION)/src/ap \
                    ../func/$(WPA_VERSION)/hostapd \
                    ../func/$(WPA_VERSION)/src/common \
                    ../func/$(WPA_VERSION)/src/drivers 

ifeq ($(CFG_BLE_VERSION),1)
$(NAME)_INCLUDES += ../driver/ble \
					../driver/ble/beken_ble_sdk/controller/include \
					../driver/ble/beken_ble_sdk/hci/include \
					../driver/ble/beken_ble_sdk/host/include \
					../driver/ble/beken_ble_sdk/sys/include \
					../driver/ble/config \
					../driver/ble/modules/app/api \
					../driver/ble/modules/gernel_api \
					../driver/ble/modules/mesh_model/ali \
					../driver/ble/plactform/arch \
					../driver/ble/plactform/driver/ble_icu \
					../driver/ble/plactform/driver/ir \
					../driver/ble/plactform/driver/reg \
					../driver/ble/plactform/driver/sys_ctrl \
					../driver/ble/plactform/driver/uart \
					../driver/ble/plactform/include \
					../driver/ble/plactform/modules/include \
					../driver/ble/profiles/comm/api \
					../driver/ble/profiles/prf/include \
					../driver/ble/profiles/sdp/api \
					../driver/ble/beken_ble_sdk/mesh/include \
					../driver/ble/beken_ble_sdk/mesh/src/dbg \
					../driver/ble/beken_ble_sdk/mesh/src/models/include 
else
ifeq ($(CFG_BLE_VERSION),2)
$(NAME)_INCLUDES += ../driver/ble_5_x_rw/ble_lib/ip/ble/hl/api \
					../driver/ble_5_x_rw/ble_lib/ip/ble/hl/inc \
					../driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gap/gapc \
					../driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gap/gapm \
					../driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt \
					../driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt/attc \
					../driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt/attm \
					../driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt/atts \
					../driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt/gattc \
					../driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt/gattm \
					../driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/l2c/l2cc \
					../driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/l2c/l2cm \
					../driver/ble_5_x_rw/ble_lib/ip/ble/ll/api \
					../driver/ble_5_x_rw/ble_lib/ip/ble/ll/import/reg \
					../driver/ble_5_x_rw/ble_lib/ip/ble/ll/src \
					../driver/ble_5_x_rw/ble_lib/ip/ble/ll/src/llc \
					../driver/ble_5_x_rw/ble_lib/ip/ble/ll/src/lld \
					../driver/ble_5_x_rw/ble_lib/ip/ble/ll/src/llm \
					../driver/ble_5_x_rw/ble_lib/ip/em/api \
					../driver/ble_5_x_rw/ble_lib/ip/hci/api \
					../driver/ble_5_x_rw/ble_lib/ip/hci/src \
					../driver/ble_5_x_rw/ble_lib/ip/sch/api \
					../driver/ble_5_x_rw/ble_lib/ip/sch/import \
					../driver/ble_5_x_rw/ble_lib/modules/aes/api \
					../driver/ble_5_x_rw/ble_lib/modules/aes/src \
					../driver/ble_5_x_rw/ble_lib/modules/common/api \
					../driver/ble_5_x_rw/ble_lib/modules/dbg/api \
					../driver/ble_5_x_rw/ble_lib/modules/dbg/src \
					../driver/ble_5_x_rw/ble_lib/modules/ecc_p256/api \
					../driver/ble_5_x_rw/ble_lib/modules/h4tl/api \
					../driver/ble_5_x_rw/ble_lib/modules/ke/api \
					../driver/ble_5_x_rw/ble_lib/modules/ke/src \
					../driver/ble_5_x_rw/ble_pub/prf \
					../driver/ble_5_x_rw/ble_pub/profiles/comm/api \
					../driver/ble_5_x_rw/ble_pub/app/api \
					../driver/ble_5_x_rw/ble_pub/ui \
					../driver/ble_5_x_rw/platform/7231n/rwip/api \
					../driver/ble_5_x_rw/platform/7231n/rwip/import/reg \
					../driver/ble_5_x_rw/platform/7231n/nvds/api \
					../driver/ble_5_x_rw/platform/7231n/config \
					../driver/ble_5_x_rw/platform/7231n/driver/reg \
					../driver/ble_5_x_rw/platform/7231n/driver/rf \
					../driver/ble_5_x_rw/platform/7231n/driver/uart \
					../driver/ble_5_x_rw/platform/7231n/entry \
					../driver/ble_5_x_rw/arch/armv5 \
					../driver/ble_5_x_rw/arch/armv5/ll \
					../driver/ble_5_x_rw/arch/armv5/compiler
endif
endif
					
$(NAME)_SOURCES	:=  common/co_dlist.c \
					common/co_list.c \
					common/co_math.c \
					common/co_pool.c \
					common/co_ring.c \
					ke/ke_env.c \
					ke/ke_event.c \
					ke/ke_msg.c \
					ke/ke_queue.c \
					ke/ke_task.c \
					ke/ke_timer.c \
					lmac/src/chan/chan.c \
					lmac/src/hal/hal_desc.c \
					lmac/src/hal/hal_dma.c \
					lmac/src/hal/hal_machw.c \
					lmac/src/hal/hal_mib.c \
					lmac/src/mm/mm.c \
					lmac/src/mm/mm_bcn.c \
					lmac/src/mm/mm_task.c \
					lmac/src/mm/mm_timer.c \
					lmac/src/p2p/p2p.c \
					lmac/src/ps/ps.c \
					lmac/src/rd/rd.c \
					lmac/src/rwnx/rwnx.c \
					lmac/src/rx/rx_swdesc.c \
					lmac/src/rx/rxl/rxl_cntrl.c \
					lmac/src/rx/rxl/rxl_hwdesc.c \
					lmac/src/scan/scan.c \
					lmac/src/scan/scan_shared.c \
					lmac/src/scan/scan_task.c \
					lmac/src/sta/sta_mgmt.c \
					lmac/src/td/td.c \
					lmac/src/tdls/tdls.c \
					lmac/src/tdls/tdls_task.c \
					lmac/src/tpc/tpc.c \
					lmac/src/tx/tx_swdesc.c \
					lmac/src/tx/txl/txl_buffer.c \
					lmac/src/tx/txl/txl_buffer_shared.c \
					lmac/src/tx/txl/txl_cfm.c \
					lmac/src/tx/txl/txl_cntrl.c \
					lmac/src/tx/txl/txl_frame.c \
					lmac/src/tx/txl/txl_frame_shared.c \
					lmac/src/tx/txl/txl_hwdesc.c \
					lmac/src/vif/vif_mgmt.c \
					mac/mac.c \
					mac/mac_ie.c \
					umac/src/apm/apm.c \
					umac/src/apm/apm_task.c \
					umac/src/bam/bam.c \
					umac/src/bam/bam_task.c \
					umac/src/me/me.c \
					umac/src/me/me_mgmtframe.c \
					umac/src/me/me_mic.c \
					umac/src/me/me_task.c \
					umac/src/me/me_utils.c \
					umac/src/rc/rc.c \
					umac/src/rc/rc_basic.c \
					umac/src/rxu/rxu_cntrl.c \
					umac/src/scanu/scanu.c \
					umac/src/scanu/scanu_shared.c \
					umac/src/scanu/scanu_task.c \
					umac/src/sm/sm.c \
					umac/src/sm/sm_task.c \
					umac/src/txu/txu_cntrl.c 
