
WPA_VERSION := wpa_supplicant_2_9
ifeq ($(CFG_USE_WPA_29),0)
WPA_VERSION := hostapd-2.5
endif

AT_SERVICE_CFG ?= 0
ifeq ($(AT_SERVICE_CFG),1)
ATSVR_CFG ?= 1
else
ATSVR_CFG ?= 0
endif

# -------------------------------------------------------------------
# Include folder list
# -------------------------------------------------------------------

INCLUDES += -I./beken378/common
INCLUDES += -I./beken378/release
INCLUDES += -I./beken378/demo
INCLUDES += -I./beken378/app
INCLUDES += -I./beken378/app/config
INCLUDES += -I./beken378/app/standalone-station
INCLUDES += -I./beken378/app/standalone-ap
INCLUDES += -I./beken378/ip/common
INCLUDES += -I./beken378/ip/ke/
INCLUDES += -I./beken378/ip/mac/
INCLUDES += -I./beken378/ip/lmac/src/hal
INCLUDES += -I./beken378/ip/lmac/src/mm
INCLUDES += -I./beken378/ip/lmac/src/ps
INCLUDES += -I./beken378/ip/lmac/src/rd
INCLUDES += -I./beken378/ip/lmac/src/rwnx
INCLUDES += -I./beken378/ip/lmac/src/rx
INCLUDES += -I./beken378/ip/lmac/src/scan
INCLUDES += -I./beken378/ip/lmac/src/sta
INCLUDES += -I./beken378/ip/lmac/src/tx
INCLUDES += -I./beken378/ip/lmac/src/vif
INCLUDES += -I./beken378/ip/lmac/src/rx/rxl
INCLUDES += -I./beken378/ip/lmac/src/tx/txl
INCLUDES += -I./beken378/ip/lmac/src/p2p
INCLUDES += -I./beken378/ip/lmac/src/chan
INCLUDES += -I./beken378/ip/lmac/src/td
INCLUDES += -I./beken378/ip/lmac/src/tpc
INCLUDES += -I./beken378/ip/lmac/src/tdls
INCLUDES += -I./beken378/ip/umac/src/mesh
INCLUDES += -I./beken378/ip/umac/src/mfp
INCLUDES += -I./beken378/ip/umac/src/rc
INCLUDES += -I./beken378/ip/umac/src/apm
INCLUDES += -I./beken378/ip/umac/src/bam
INCLUDES += -I./beken378/ip/umac/src/llc
INCLUDES += -I./beken378/ip/umac/src/me
INCLUDES += -I./beken378/ip/umac/src/rxu
INCLUDES += -I./beken378/ip/umac/src/scanu
INCLUDES += -I./beken378/ip/umac/src/sm
INCLUDES += -I./beken378/ip/umac/src/txu
INCLUDES += -I./beken378/app/video_work
INCLUDES += -I./beken378/app/net_work
INCLUDES += -I./beken378/driver/include
INCLUDES += -I./beken378/driver/common/reg
INCLUDES += -I./beken378/driver/entry
INCLUDES += -I./beken378/driver/dma
INCLUDES += -I./beken378/driver/intc
INCLUDES += -I./beken378/driver/phy
INCLUDES += -I./beken378/driver/pwm
INCLUDES += -I./beken378/driver/rc_beken
INCLUDES += -I./beken378/driver/flash
INCLUDES += -I./beken378/driver/rw_pub
INCLUDES += -I./beken378/driver/common/reg
INCLUDES += -I./beken378/driver/common
INCLUDES += -I./beken378/driver/uart
INCLUDES += -I./beken378/driver/sys_ctrl
INCLUDES += -I./beken378/driver/gpio
INCLUDES += -I./beken378/driver/general_dma
INCLUDES += -I./beken378/driver/spidma
INCLUDES += -I./beken378/driver/icu
INCLUDES += -I./beken378/driver/i2c
INCLUDES += -I./beken378/driver/jpeg
INCLUDES += -I./beken378/driver/usb
INCLUDES += -I./beken378/func/include
INCLUDES += -I./beken378/func/ble_wifi_exchange
INCLUDES += -I./beken378/func/rf_test
INCLUDES += -I./beken378/func/user_driver
INCLUDES += -I./beken378/func/power_save
INCLUDES += -I./beken378/func/uart_debug
INCLUDES += -I./beken378/func/ethernet_intf
INCLUDES += -I./beken378/func/camera_intf
INCLUDES += -I./beken378/func/video_transfer
INCLUDES += -I./beken378/func/$(WPA_VERSION)/hostapd
INCLUDES += -I./beken378/func/$(WPA_VERSION)/bk_patch
INCLUDES += -I./beken378/func/$(WPA_VERSION)/src/utils
INCLUDES += -I./beken378/func/$(WPA_VERSION)/src/ap
INCLUDES += -I./beken378/func/$(WPA_VERSION)/src/common
INCLUDES += -I./beken378/func/$(WPA_VERSION)/src/drivers
INCLUDES += -I./beken378/func/$(WPA_VERSION)/src
INCLUDES += -I./beken378/func/$(WPA_VERSION)/wpa_supplicant
INCLUDES += -I./beken378/func/$(WPA_VERSION)/bk_patch
INCLUDES += -I./beken378/func/lwip_intf/lwip-2.0.2/src
INCLUDES += -I./beken378/func/lwip_intf/lwip-2.0.2/port
INCLUDES += -I./beken378/func/lwip_intf/lwip-2.0.2/src/include
INCLUDES += -I./beken378/func/lwip_intf/lwip-2.0.2/src/include/netif
INCLUDES += -I./beken378/func/lwip_intf/lwip-2.0.2/src/include/lwip
INCLUDES += -I./beken378/func/temp_detect
INCLUDES += -I./beken378/func/spidma_intf
INCLUDES += -I./beken378/func/saradc_intf
INCLUDES += -I./beken378/func/rwnx_intf
INCLUDES += -I./beken378/func/joint_up
INCLUDES += -I./beken378/func/base64
INCLUDES += -I./beken378/func/easy_flash
INCLUDES += -I./beken378/func/easy_flash/inc
INCLUDES += -I./beken378/func/easy_flash/port
INCLUDES += -I./beken378/func/rf_use
INCLUDES += -I./beken378/func/usb
INCLUDES += -I./beken378/func/sensor
INCLUDES += -I./beken378/os/include
INCLUDES += -I./beken378/os/FreeRTOSv9.0.0
INCLUDES += -I./beken378/func/utf8
INCLUDES += -I./beken378/app/http

ifeq ($(CFG_BK_AWARE),1)
INCLUDES += -I./beken378/func/bk_aware
endif

ifeq ($(CFG_USE_SDCARD_HOST),1)
INCLUDES += -I./beken378/driver/usb/src/msc
INCLUDES += -I./beken378/func/fatfs
endif

# For BK7251
ifeq ($(CFG_SOC_NAME), 3)
INCLUDES += -I./beken378/driver/audio
INCLUDES += -I./beken378/driver/sdcard
endif

# For WPA3
ifeq ($(CFG_WPA3),1)
INCLUDES += -I./beken378/func/wolfssl
endif

#paho-mqtt
INCLUDES += -I./beken378/func/paho-mqtt/client
INCLUDES += -I./beken378/func/paho-mqtt/client/src
INCLUDES += -I./beken378/func/paho-mqtt/packet/src
INCLUDES += -I./beken378/func/paho-mqtt/mqtt_ui
INCLUDES += -I./beken378/func/paho-mqtt/mqtt_ui/ssl_mqtt
INCLUDES += -I./beken378/func/paho-mqtt/mqtt_ui/tcp_mqtt

ifeq ($(CFG_SUPPORT_BLE),1)
ifeq ($(CFG_BLE_VERSION),$(BLE_VERSION_4_2))
INCLUDES += -I./beken378/driver/ble
INCLUDES += -I./beken378/driver/ble/beken_ble_sdk/controller/include
INCLUDES += -I./beken378/driver/ble/beken_ble_sdk/hci/include
INCLUDES += -I./beken378/driver/ble/beken_ble_sdk/host/include
INCLUDES += -I./beken378/driver/ble/beken_ble_sdk/sys/include
INCLUDES += -I./beken378/driver/ble/config
INCLUDES += -I./beken378/driver/ble/modules/app/api
INCLUDES += -I./beken378/driver/ble/modules/gernel_api
INCLUDES += -I./beken378/driver/ble/modules/mesh_model/ali
INCLUDES += -I./beken378/driver/ble/plactform/arch
INCLUDES += -I./beken378/driver/ble/plactform/driver/ble_icu
INCLUDES += -I./beken378/driver/ble/plactform/driver/ir
INCLUDES += -I./beken378/driver/ble/plactform/driver/reg
INCLUDES += -I./beken378/driver/ble/plactform/driver/sys_ctrl
INCLUDES += -I./beken378/driver/ble/plactform/driver/uart
INCLUDES += -I./beken378/driver/ble/plactform/include
INCLUDES += -I./beken378/driver/ble/plactform/modules/include
INCLUDES += -I./beken378/driver/ble/profiles/comm/api
INCLUDES += -I./beken378/driver/ble/profiles/prf/include
INCLUDES += -I./beken378/driver/ble/profiles/sdp/api
INCLUDES += -I./beken378/driver/ble/beken_ble_sdk/mesh/include
INCLUDES += -I./beken378/driver/ble/beken_ble_sdk/mesh/src/dbg
INCLUDES += -I./beken378/driver/ble/beken_ble_sdk/mesh/src/models/include
endif
ifeq ($(CFG_BLE_VERSION),$(BLE_VERSION_5_x))
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/hl/api
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/hl/inc
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gap/gapc
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gap/gapm
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt/attc
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt/attm
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt/atts
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt/gattc
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt/gattm
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/l2c/l2cc
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/l2c/l2cm
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/ll/api
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/ll/import/reg
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/ll/src
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/ll/src/llc
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/ll/src/lld
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/ble/ll/src/llm
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/em/api
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/hci/api
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/hci/src
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/sch/api
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/ip/sch/import
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/modules/aes/api
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/modules/aes/src
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/modules/common/api
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/modules/dbg/api
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/modules/dbg/src
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/modules/ecc_p256/api
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/modules/h4tl/api
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/modules/ke/api
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_lib/modules/ke/src
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_pub/prf
INCLUDES += -I./beken378/driver/ble_5_x_rw/platform/7231n/rwip/api
INCLUDES += -I./beken378/driver/ble_5_x_rw/platform/7231n/rwip/import/reg
INCLUDES += -I./beken378/driver/ble_5_x_rw/platform/7231n/nvds/api
INCLUDES += -I./beken378/driver/ble_5_x_rw/platform/7231n/config
INCLUDES += -I./beken378/driver/ble_5_x_rw/platform/7231n/driver/reg
INCLUDES += -I./beken378/driver/ble_5_x_rw/platform/7231n/driver/rf
INCLUDES += -I./beken378/driver/ble_5_x_rw/platform/7231n/driver/uart
INCLUDES += -I./beken378/driver/ble_5_x_rw/platform/7231n/entry
INCLUDES += -I./beken378/driver/ble_5_x_rw/arch/armv5
INCLUDES += -I./beken378/driver/ble_5_x_rw/arch/armv5/ll
INCLUDES += -I./beken378/driver/ble_5_x_rw/arch/armv5/compiler
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_pub/profiles/comm/api
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_pub/profiles/sdp/api
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_pub/app/api
INCLUDES += -I./beken378/driver/ble_5_x_rw/ble_pub/ui
endif
endif

#usb module
#ifeq ($(CFG_USB),1)
INCLUDES += -I./beken378/driver/usb/include
INCLUDES += -I./beken378/driver/usb/include/class
INCLUDES += -I./beken378/driver/usb/src/cd
INCLUDES += -I./beken378/driver/usb/src/drivers/
INCLUDES += -I./beken378/driver/usb/src/drivers/comm
INCLUDES += -I./beken378/driver/usb/src/drivers/hid
INCLUDES += -I./beken378/driver/usb/src/drivers/msd
INCLUDES += -I./beken378/driver/usb/src/drivers/compl
INCLUDES += -I./beken378/driver/usb/src/drivers/hub
INCLUDES += -I./beken378/driver/usb/src/drivers/trans
INCLUDES += -I./beken378/driver/usb/src/example/msd
INCLUDES += -I./beken378/driver/usb/src/hid
INCLUDES += -I./beken378/driver/usb/src/lib
INCLUDES += -I./beken378/driver/usb/src/msc
INCLUDES += -I./beken378/driver/usb/src/systems/none/afs
INCLUDES += -I./beken378/driver/usb/src/systems/none
INCLUDES += -I./beken378/driver/usb/src/uvc
#endif

ifeq ("${CFG_MBEDTLS}", "1")
#CFG_DEFINE_INCLUDE += MBEDTLS_CONFIG_FILE=\"tls_config.h\"
INCLUDES += -I./beken378/func/mbedtls/mbedtls/include
INCLUDES += -I./beken378/func/mbedtls/mbedtls/include/mbedtls
INCLUDES += -I./beken378/func/mbedtls/mbedtls-port/inc
INCLUDES += -I./beken378/func/mbedtls/mbedtls_ui/
endif


ifeq ($(ATSVR_CFG),1)
INCLUDES += -I./beken378/func/at_server/
INCLUDES += -I./beken378/func/at_server/_at_server
INCLUDES += -I./beken378/func/at_server/_at_server_port
INCLUDES += -I./beken378/func/at_server/at_server_func
INCLUDES += -I./beken378/func/at_server/atsvr_cmd
endif

# -------------------------------------------------------------------
# Source file list
# -------------------------------------------------------------------

#application layer
SRC_C += ./beken378/app/app.c
SRC_C += ./beken378/app/ate_app.c
SRC_C += ./beken378/app/config/param_config.c
SRC_C += ./beken378/app/standalone-ap/sa_ap.c
SRC_C += ./beken378/app/standalone-station/sa_station.c
SRC_C += ./beken378/app/video_work/video_transfer_tcp.c
SRC_C += ./beken378/app/video_work/video_transfer_udp.c
SRC_C += ./beken378/app/video_work/video_buffer.c
SRC_C += ./beken378/app/net_work/video_demo_main.c
SRC_C += ./beken378/app/net_work/video_demo_station.c
SRC_C += ./beken378/app/net_work/video_demo_softap.c

#demo module
SRC_C += ./beken378/demo/ieee802_11_demo.c

#driver layer
SRC_C += ./beken378/driver/common/dd.c
SRC_C += ./beken378/driver/common/drv_model.c
SRC_C += ./beken378/driver/dma/dma.c
SRC_C += ./beken378/driver/driver.c
SRC_C += ./beken378/driver/entry/arch_main.c
SRC_C += ./beken378/driver/fft/fft.c
SRC_C += ./beken378/driver/flash/flash.c
SRC_C += ./beken378/driver/general_dma/general_dma.c
SRC_C += ./beken378/driver/gpio/gpio.c
SRC_C += ./beken378/driver/i2s/i2s.c
SRC_C += ./beken378/driver/icu/icu.c
SRC_C += ./beken378/driver/intc/intc.c
SRC_C += ./beken378/driver/irda/irda.c
SRC_C += ./beken378/driver/macphy_bypass/mac_phy_bypass.c
SRC_C += ./beken378/driver/phy/phy_trident.c
SRC_C += ./beken378/driver/pwm/pwm.c
SRC_C += ./beken378/driver/pwm/pwm_bk7231n.c
SRC_C += ./beken378/driver/pwm/mcu_ps_timer.c
SRC_C += ./beken378/driver/pwm/bk_timer.c
SRC_C += ./beken378/driver/pwm/pwm_mutex.c
SRC_C += ./beken378/driver/qspi/qspi.c
SRC_C += ./beken378/driver/rw_pub/rw_platf_pub.c
SRC_C += ./beken378/driver/saradc/saradc.c
SRC_C += ./beken378/driver/spidma/spidma.c
SRC_C += ./beken378/driver/sys_ctrl/sys_ctrl.c
SRC_C += ./beken378/driver/uart/Retarget.c
SRC_C += ./beken378/driver/uart/uart.c
SRC_C += ./beken378/driver/uart/printf.c
SRC_C += ./beken378/driver/wdt/wdt.c
SRC_C += ./beken378/driver/calendar/calendar.c
SRC_C += ./beken378/driver/security/security.c
SRC_C += ./beken378/driver/security/hal_aes.c
SRC_C += ./beken378/driver/security/hal_sha.c
SRC_C += ./beken378/driver/jpeg/jpeg_encoder.c
SRC_C += ./beken378/driver/i2c/i2c1.c
SRC_C += ./beken378/driver/i2c/i2c2.c

ifeq ($(CFG_SDIO),1)
SRC_C += ./beken378/driver/sdio/sdio.c
SRC_C += ./beken378/driver/sdio/sdma.c
SRC_C += ./beken378/driver/sdio/sutil.c
endif

#function layer
SRC_C += ./beken378/func/func.c
SRC_C += ./beken378/func/usb_plug/usb_plug.c
SRC_C += ./beken378/func/security/security_func.c
SRC_C += ./beken378/func/joint_up/role_launch.c
SRC_C += ./beken378/app/http/utils_httpc.c
SRC_C += ./beken378/app/http/utils_net.c
SRC_C += ./beken378/app/http/utils_timer.c
SRC_C += ./beken378/app/http/lite-log.c

ifeq ($(CFG_BK_AWARE),1)
SRC_C += ./beken378/func/bk_aware/bk_aware.c
endif

SRC_WPA_C += ./beken378/func/hostapd_intf/hostapd_intf.c
ifeq ($(CFG_USE_SDCARD_HOST),1)
SRC_C += ./beken378/func/fatfs/cc936.c
SRC_C += ./beken378/func/fatfs/ccsbcs.c
SRC_C += ./beken378/func/fatfs/disk_io.c
SRC_C += ./beken378/func/fatfs/driver_udisk.c
SRC_C += ./beken378/func/fatfs/ff.c
SRC_C += ./beken378/func/fatfs/playmode.c
endif

ifeq ($(CFG_SDIO),1)
SRC_C += ./beken378/func/sdio_intf/sdio_intf.c
endif

SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/port/ethernetif.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/port/net.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/port/sys_arch.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/api/api_lib.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/api/api_msg.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/api/err.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/api/netbuf.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/api/netdb.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/api/netifapi.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/api/sockets.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/api/tcpip.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/apps/ping/ping.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/def.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/dns.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/inet_chksum.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/init.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/ip.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/ipv4/dhcp.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/ipv4/etharp.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/ipv4/icmp.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/ipv4/igmp.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/ipv4/ip4_addr.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/ipv4/ip4.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/ipv4/ip4_frag.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/ipv6/dhcp6.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/ipv6/ethip6.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/ipv6/icmp6.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/ipv6/inet6.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/ipv6/ip6_addr.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/ipv6/ip6.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/ipv6/ip6_frag.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/ipv6/mld6.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/ipv6/nd6.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/mem.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/memp.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/netif.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/pbuf.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/raw.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/stats.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/sys.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/tcp.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/tcp_in.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/tcp_out.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/timeouts.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/core/udp.c
SRC_C += ./beken378/func/lwip_intf/lwip-2.0.2/src/netif/ethernet.c
SRC_C += ./beken378/func/lwip_intf/dhcpd/dhcp-server.c
SRC_C += ./beken378/func/lwip_intf/dhcpd/dhcp-server-main.c
SRC_C += ./beken378/func/misc/fake_clock.c
SRC_C += ./beken378/func/misc/pseudo_random.c
SRC_C += ./beken378/func/misc/target_util.c
SRC_C += ./beken378/func/misc/start_type.c
SRC_C += ./beken378/func/power_save/power_save.c
SRC_C += ./beken378/func/power_save/manual_ps.c
SRC_C += ./beken378/func/power_save/mcu_ps.c
SRC_C += ./beken378/func/power_save/ap_idle.c
SRC_C += ./beken378/func/saradc_intf/saradc_intf.c
SRC_C += ./beken378/func/rwnx_intf/rw_ieee80211.c
SRC_C += ./beken378/func/rwnx_intf/rw_msdu.c
SRC_C += ./beken378/func/rwnx_intf/rw_msg_rx.c
SRC_C += ./beken378/func/rwnx_intf/rw_msg_tx.c
SRC_C += ./beken378/func/sim_uart/gpio_uart.c
SRC_C += ./beken378/func/sim_uart/pwm_uart.c
SRC_C += ./beken378/func/spidma_intf/spidma_intf.c
SRC_C += ./beken378/func/temp_detect/temp_detect.c
SRC_C += ./beken378/func/user_driver/BkDriverFlash.c
SRC_C += ./beken378/func/user_driver/BkDriverGpio.c
SRC_C += ./beken378/func/user_driver/BkDriverPwm.c
SRC_C += ./beken378/func/user_driver/BkDriverUart.c
SRC_C += ./beken378/func/user_driver/BkDriverWdg.c
SRC_C += ./beken378/func/user_driver/BkDriverRng.c
SRC_C += ./beken378/func/user_driver/BkDriverTimer.c
SRC_C += ./beken378/func/wlan_ui/wlan_cli.c
SRC_C += ./beken378/func/wlan_ui/bk_peripheral_test.c
# utf8
SRC_C += ./beken378/func/utf8/conv_utf8.c

# video / jpeg
SRC_C += ./beken378/func/camera_intf/camera_intf.c
SRC_C += ./beken378/func/video_transfer/video_transfer.c

# For WPA3: wolfssl
ifeq ($(CFG_WPA3),1)
SRC_C += ./beken378/func/wolfssl/wolfcrypt/src/wolfmath.c
SRC_C += ./beken378/func/wolfssl/wolfcrypt/src/memory.c
SRC_C += ./beken378/func/wolfssl/wolfcrypt/src/tfm.c
SRC_C += ./beken378/func/wolfssl/wolfcrypt/src/ecc.c

# wpa_supplicant 2.9 needs random generator
SRC_C += ./beken378/func/wolfssl/wolfcrypt/src/random.c
SRC_C += ./beken378/func/wolfssl/wolfcrypt/src/sha256.c
endif

# For BK7251
ifeq ($(CFG_SOC_NAME), 3)
SRC_C += ./beken378/driver/audio/audio.c
SRC_C += ./beken378/driver/audio/audio_adc.c
SRC_C += ./beken378/driver/audio/audio_dac.c
SRC_C += ./beken378/driver/audio/ring_buffer.c
SRC_C += ./beken378/driver/audio/ring_buffer_dma_read.c
SRC_C += ./beken378/driver/audio/ring_buffer_dma_write.c

SRC_C += ./beken378/func/audio/audio_intf.c

SRC_C += ./beken378/driver/sdcard/sdcard.c
SRC_C += ./beken378/driver/sdcard/sdio_driver.c
SRC_C += ./beken378/driver/spi/spi.c
SRC_C += ./beken378/driver/spi/spi_master.c
SRC_C += ./beken378/driver/spi/spi_slave.c
SRC_C += ./beken378/func/sd_music/sdcard_test.c
endif

# For BK7231u
ifeq ($(CFG_SOC_NAME), 2)
SRC_C += ./beken378/driver/spi/spi.c
SRC_C += ./beken378/driver/spi/spi_master.c
SRC_C += ./beken378/driver/spi/spi_slave.c
endif

# For BK7231N
ifeq ($(CFG_SOC_NAME), 5)
SRC_C += ./beken378/driver/spi/spi_bk7231n.c
SRC_C += ./beken378/driver/spi/spi_master_bk7231n.c
SRC_C += ./beken378/driver/spi/spi_slave_bk7231n.c
endif

SRC_C += ./beken378/func/wlan_ui/wlan_ui.c
SRC_C += ./beken378/func/net_param_intf/net_param.c
SRC_C += ./beken378/func/base64/base_64.c
SRC_C += ./beken378/func/airkiss/bk_airkiss.c
SRC_C += ./beken378/func/airkiss/airkiss_main.c
SRC_C += ./beken378/func/airkiss/airkiss_pingpong.c

#easy flash
SRC_C += ./beken378/func/easy_flash/bk_ef.c
SRC_C += ./beken378/func/easy_flash/src/easyflash.c
SRC_C += ./beken378/func/easy_flash/src/ef_env.c
SRC_C += ./beken378/func/easy_flash/src/ef_env_wl.c
SRC_C += ./beken378/func/easy_flash/src/ef_iap.c
SRC_C += ./beken378/func/easy_flash/src/ef_log.c
SRC_C += ./beken378/func/easy_flash/src/ef_utils.c
SRC_C += ./beken378/func/easy_flash/port/ef_port.c

#paho-mqtt
SRC_C += ./beken378/func/paho-mqtt/client/src/MQTTClient.c
SRC_C += ./beken378/func/paho-mqtt/client/src/MQTTFreeRTOS.c
SRC_C += ./beken378/func/paho-mqtt/client/paho_mqtt_udp.c
SRC_C += ./beken378/func/paho-mqtt/packet/src/MQTTConnectClient.c
SRC_C += ./beken378/func/paho-mqtt/packet/src/MQTTConnectServer.c
SRC_C += ./beken378/func/paho-mqtt/packet/src/MQTTDeserializePublish.c
SRC_C += ./beken378/func/paho-mqtt/packet/src/MQTTFormat.c
SRC_C += ./beken378/func/paho-mqtt/packet/src/MQTTPacket.c
SRC_C += ./beken378/func/paho-mqtt/packet/src/MQTTSerializePublish.c
SRC_C += ./beken378/func/paho-mqtt/packet/src/MQTTSubscribeClient.c
SRC_C += ./beken378/func/paho-mqtt/packet/src/MQTTSubscribeServer.c
SRC_C += ./beken378/func/paho-mqtt/packet/src/MQTTUnsubscribeClient.c
SRC_C += ./beken378/func/paho-mqtt/packet/src/MQTTUnsubscribeServer.c
SRC_C += ./beken378/func/paho-mqtt/mqtt_ui/ssl_mqtt/ssl_mqtt_client_port.c
SRC_C += ./beken378/func/paho-mqtt/mqtt_ui/tcp_mqtt/tcp_mqtt_client_port.c
SRC_C += ./beken378/func/paho-mqtt/mqtt_ui/mqtt_client_core.c
SRC_C += ./beken378/func/paho-mqtt/mqtt_ui/mqtt_client_com_port.c


ifeq ("${CFG_MBEDTLS}", "1")
#MBEDTLS_SRC_DIRS += $(shell find beken378/func/mbedtls/mbedtls/library -type d)
#MBEDTLS_SRC_DIRS += $(shell find beken378/func/mbedtls/mbedtls-port/src -type d)
##MBEDTLS_SRC_DIRS += ./beken378/func/mbedtls/mbedtls/library/
##MBEDTLS_SRC_DIRS += ./beken378/func/mbedtls/mbedtls-port/src/
#SRC_C += $(foreach dir, $(MBEDTLS_SRC_DIRS), $(wildcard $(dir)/*.c))

SRC_C += ./beken378/func/mbedtls/mbedtls-port/src/timing_alt.c
SRC_C += ./beken378/func/mbedtls/mbedtls-port/src/tls_certificate.c
SRC_C += ./beken378/func/mbedtls/mbedtls-port/src/tls_client.c
SRC_C += ./beken378/func/mbedtls/mbedtls-port/src/tls_hardware.c
SRC_C += ./beken378/func/mbedtls/mbedtls-port/src/tls_mem.c
SRC_C += ./beken378/func/mbedtls/mbedtls-port/src/tls_net.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/aes.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/aesni.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/arc4.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/asn1parse.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/asn1write.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/base64.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/bignum.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/blowfish.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/camellia.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/ccm.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/certs.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/cipher.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/cipher_wrap.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/cmac.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/ctr_drbg.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/debug.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/des.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/dhm.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/ecdh.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/ecdsa.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/ecjpake.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/ecp.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/ecp_curves.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/entropy.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/entropy_poll.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/error.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/gcm.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/havege.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/hmac_drbg.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/md.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/md_wrap.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/md2.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/md4.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/md5.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/memory_buffer_alloc.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/net_sockets.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/oid.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/padlock.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/pem.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/pk.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/pk_wrap.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/pkcs5.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/pkcs11.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/pkcs12.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/pkparse.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/pkwrite.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/platform.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/ripemd160.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/rsa.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/sha1.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/sha256.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/sha512.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/ssl_cache.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/ssl_ciphersuites.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/ssl_cli.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/ssl_cookie.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/ssl_srv.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/ssl_ticket.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/ssl_tls.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/threading.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/timing.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/version.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/version_features.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/x509.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/x509_create.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/x509_crl.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/x509_crt.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/x509_csr.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/x509write_crt.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/x509write_csr.c
SRC_C += ./beken378/func/mbedtls/mbedtls/library/xtea.c
SRC_C += ./beken378/func/mbedtls/mbedtls_ui/sl_tls.c
endif

ifeq ($(CFG_SUPPORT_BLE),1)
ifeq ($(CFG_BLE_VERSION),$(BLE_VERSION_4_2))
#ble pub
SRC_C += ./beken378/driver/ble/ble.c
SRC_C += ./beken378/driver/ble/modules/app/src/app_ble.c
SRC_C += ./beken378/driver/ble/modules/app/src/app_comm.c
SRC_C += ./beken378/driver/ble/modules/app/src/app_sdp.c
SRC_C += ./beken378/driver/ble/modules/app/src/app_sec.c
SRC_C += ./beken378/driver/ble/modules/app/src/app_task.c
SRC_C += ./beken378/driver/ble/plactform/driver/ble_icu/ble_icu.c
SRC_C += ./beken378/driver/ble/plactform/driver/uart/ble_uart.c
SRC_C += ./beken378/driver/ble/plactform/modules/arch/ble_arch_main.c
SRC_C += ./beken378/driver/ble/plactform/modules/common/RomCallFlash.c
SRC_C += ./beken378/driver/ble/plactform/modules/dbg/dbg.c
SRC_C += ./beken378/driver/ble/plactform/modules/dbg/dbg_mwsgen.c
SRC_C += ./beken378/driver/ble/plactform/modules/dbg/dbg_swdiag.c
SRC_C += ./beken378/driver/ble/plactform/modules/dbg/dbg_task.c
SRC_C += ./beken378/driver/ble/plactform/modules/rf/src/ble_rf_xvr.c
SRC_C += ./beken378/driver/ble/profiles/comm/src/comm.c
SRC_C += ./beken378/driver/ble/profiles/comm/src/comm_task.c
SRC_C += ./beken378/driver/ble/profiles/prf/src/prf.c
SRC_C += ./beken378/driver/ble/profiles/prf/src/prf_utils.c
SRC_C += ./beken378/driver/ble/profiles/sdp/src/sdp_service.c
SRC_C += ./beken378/driver/ble/profiles/sdp/src/sdp_service_task.c
#ble mesh pub
ifeq ($(CFG_SUPPORT_BLE_MESH),1)
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/mesh_api/mesh_api.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/mesh_api/mesh_api_msg.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/mesh_api/mesh_param_int.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/mesh_api/mm_api.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/mesh_api/mm_api_msg.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/mesh_api/m_api.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/mesh_api/m_api_msg.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/common/mm_route.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/common/mm_tb.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/common/mm_tb_bind.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/common/mm_tb_replay.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/common/mm_tb_state.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_bat.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_dtt.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_loc.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_lvl.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_oo.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_plvl.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_poo.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_prop.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lightc/mm_lightc.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lightc/mm_lightc_ctl.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lightc/mm_lightc_hsl.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lightc/mm_lightc_ln.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lightc/mm_lightc_xyl.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lights/mm_lights.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lights/mm_lights_ctl.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lights/mm_lights_hsl.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lights/mm_lights_ln.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/Scenes/m_fnd_Scenes.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/transition_time/m_fnd_generic_transition_time.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/vendor/mm_vendors.c
SRC_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/vendor/mm_vendor_midea.c
SRC_C += ./beken378/driver/ble/modules/app/src/app_mesh.c
SRC_C += ./beken378/driver/ble/modules/app/src/app_mm_msg.c
SRC_C += ./beken378/driver/ble/modules/gernel_api/mesh_general_api.c
SRC_C += ./beken378/driver/ble/modules/mesh_model/ali/app_light_ali_server.c
endif
endif #BLE_VERSION_4_2
ifeq ($(CFG_BLE_VERSION),$(BLE_VERSION_5_x))
# ble pub
SRC_C += ./beken378/driver/ble_5_x_rw/ble_pub/prf/prf.c
SRC_C += ./beken378/driver/ble_5_x_rw/ble_pub/prf/prf_utils.c
SRC_C += ./beken378/driver/ble_5_x_rw/ble_pub/profiles/comm/src/comm.c
SRC_C += ./beken378/driver/ble_5_x_rw/ble_pub/profiles/comm/src/comm_task.c
SRC_C += ./beken378/driver/ble_5_x_rw/ble_pub/app/src/app_comm.c
SRC_C += ./beken378/driver/ble_5_x_rw/ble_pub/app/src/app_ble.c
SRC_C += ./beken378/driver/ble_5_x_rw/ble_pub/app/src/app_task.c
SRC_C += ./beken378/driver/ble_5_x_rw/ble_pub/ui/ble_ui.c
SRC_C += ./beken378/driver/ble_5_x_rw/platform/7231n/rwip/src/rwip.c
SRC_C += ./beken378/driver/ble_5_x_rw/platform/7231n/rwip/src/rwble.c
SRC_C += ./beken378/driver/ble_5_x_rw/platform/7231n/entry/ble_main.c
SRC_C += ./beken378/driver/ble_5_x_rw/platform/7231n/driver/rf/rf_xvr.c
SRC_C += ./beken378/driver/ble_5_x_rw/platform/7231n/driver/rf/ble_rf_port.c
SRC_C += ./beken378/driver/ble_5_x_rw/platform/7231n/driver/uart/uart_ble.c
SRC_C += ./beken378/driver/ble_5_x_rw/ble_pub/app/src/app_ble_init.c
SRC_C += ./beken378/driver/ble_5_x_rw/ble_pub/app/src/app_sdp.c
SRC_C += ./beken378/driver/ble_5_x_rw/ble_pub/profiles/sdp/src/sdp_common.c
SRC_C += ./beken378/driver/ble_5_x_rw/ble_pub/profiles/sdp/src/sdp_comm_task.c
endif #BLE_VERSION_5_x
endif #CFG_SUPPORT_BLE

#operation system module
SRC_OS += ./beken378/os/FreeRTOSv9.0.0/rtos_pub.c
SRC_C  += ./beken378/os/mem_arch.c
SRC_C  += ./beken378/os/platform_stub.c
SRC_C  += ./beken378/os/str_arch.c

ifeq ($(ATSVR_CFG),1)
SRC_C += ./beken378/func/at_server/_at_server_port/atsvr_core.c
SRC_C += ./beken378/func/at_server/_at_server_port/atsvr_port.c
SRC_C += ./beken378/func/at_server/atsvr_cmd/atsvr_cmd.c
SRC_C += ./beken378/func/at_server/atsvr_cmd/atsvr_wlan.c
SRC_C += ./beken378/func/at_server/atsvr_cmd/atsvr_misc.c
SRC_C += ./beken378/func/at_server/_at_server/_at_server.c
SRC_C += ./beken378/func/at_server/_at_server/_at_server_port.c
SRC_C += ./beken378/func/at_server/at_server_func/_atsvr_func.c
SRC_C += ./beken378/func/at_server/at_server.c
endif

ifeq ($(CFG_USB),1)
SRC_C += ./beken378/driver/usb/usb.c
SRC_C += ./beken378/func/usb/fusb.c
endif

SRC_C += ./beken378/func/ble_wifi_exchange/ble_wifi_port.c

#assembling files
SRC_S +=  ./beken378/driver/entry/boot_handlers.S
SRC_S +=  ./beken378/driver/entry/boot_vectors.S


# -------------------------------------------------------------------
# Lib source file list
# -------------------------------------------------------------------

# Lib files will be deleted when making SDK.
SRC_IP_C =
-include ./beken378/ip/ip_lib_src.mk

SRC_BLE_C =
ifeq ($(CFG_SUPPORT_BLE),1)
ifeq ($(CFG_BLE_VERSION),$(BLE_VERSION_4_2))
-include ./beken378/driver/ble/ble_lib_src.mk
endif
ifeq ($(CFG_BLE_VERSION),$(BLE_VERSION_5_x))
-include ./beken378/driver/ble_5_x_rw/ble_lib_src.mk
endif
endif

SRC_USB_C =
ifeq ($(CFG_USB),1)
-include ./beken378/driver/usb/usb_lib_src.mk
endif

SRC_SENSOR_C =
ifeq ($(CFG_WIFI_SENSOR),1)
-include ./beken378/func/sensor/sensor_lib_src.mk
endif

SRC_CAL_C =
-include ./beken378/func/bk7011_cal/cal_lib_src.mk

SRC_SUPPLICANT_C =
-include ./beken378/func/wpa_supplicant_2_9/supplicant_lib_src.mk

SRC_UART_DEBUG_C =
-include ./beken378/func/uart_debug/uart_debug_lib_src.mk

SRC_RF_TEST_C =
-include ./beken378/func/rf_test/rf_test_lib_src.mk

SRC_RF_USE_C =
-include ./beken378/func/rf_use/rf_use_lib_src.mk


