NAME := wpa_supplicant_2_9

$(NAME)_TYPE := kernel

-include $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/.config

$(NAME)_INCLUDES += ../../ip/lmac/src/mm\
                    ../../ip/mac\
                    ../../ip/lmac/src/hal \
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
                    ../../ip/umac/src/rc\
                    ../../ip/ke

$(NAME)_INCLUDES += ../../driver/dma\
                    ../../driver/common/reg\
                    ../../driver/phy\
                    ../../driver/uart\
                    ../../func/user_driver\
                    ../../func/temp_detect\
                    ../../func/rwnx_intf\
                    ../../app/standalone-station\
                    ../../demo


$(NAME)_INCLUDES += src \
                    src/ap \
                    src/utils \
                    src/common \
                    src/drivers \
                    bk_patch \
                    hostapd \
                    wpa_supplicant

$(NAME)_SOURCES +=  bk_patch/ddrv.c \
                    bk_patch/signal.c \
                    bk_patch/sk_intf.c \
                    bk_patch/fake_socket.c \
                    src/common/hw_features_common.c \
                    src/common/ieee802_11_common.c \
                    src/common/wpa_common.c \
                    src/common/notifier.c \
                    src/common/wpa_psk_cache.c \
                    src/crypto/aes-unwrap.c \
                    src/crypto/rc4.c \
                    src/crypto/sha1-pbkdf2.c \
                    src/crypto/sha1-prf.c \
                    src/crypto/tls_none.c \
                    src/crypto/aes-wrap.c \
                    src/drivers/driver_beken.c \
                    src/drivers/driver_common.c \
                    src/drivers/drivers.c \
                    src/l2_packet/l2_packet_none.c \
                    src/rsn_supp/pmksa_cache.c \
                    src/rsn_supp/wpa.c \
                    src/rsn_supp/wpa_ie.c \
                    src/utils/common.c \
                    src/utils/eloop.c \
                    src/utils/os_none.c \
                    src/utils/wpabuf.c \
                    src/utils/wpa_debug.c \
                    src/ap/ap_config.c \
                    src/ap/ap_drv_ops.c \
                    src/ap/ap_list.c \
                    src/ap/ap_mlme.c \
                    src/ap/beacon.c \
                    src/ap/drv_callbacks.c \
                    src/ap/hostapd.c \
                    src/ap/hw_features.c \
                    src/ap/ieee802_11.c \
                    src/ap/ieee802_11_auth.c \
                    src/ap/ieee802_11_ht.c \
                    src/ap/ieee802_11_shared.c \
                    src/ap/ieee802_1x.c \
                    src/ap/pmksa_cache_auth.c \
                    src/ap/sta_info.c \
                    src/ap/tkip_countermeasures.c \
                    src/ap/utils.c \
                    src/ap/wmm.c \
                    src/ap/wpa_auth.c \
                    src/ap/wpa_auth_glue.c \
                    src/ap/wpa_auth_ie.c \
                    hostapd/main_none.c \
                    wpa_supplicant/blacklist.c \
                    wpa_supplicant/bss.c \
                    wpa_supplicant/config.c \
                    wpa_supplicant/config_none.c \
                    wpa_supplicant/events.c \
                    wpa_supplicant/main_supplicant.c \
                    wpa_supplicant/notify.c \
                    wpa_supplicant/wmm_ac.c \
                    wpa_supplicant/wpa_scan.c \
                    wpa_supplicant/wpa_supplicant.c \
                    wpa_supplicant/wpas_glue.c \
                    wpa_supplicant/ctrl_iface.c \
                    wpa_supplicant/wlan.c \
                    wpa_supplicant/sme.c\
                    wpa_supplicant/op_classes.c\
                    src/crypto/crypto_ali.c
