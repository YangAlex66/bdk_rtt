NAME := usb

$(NAME)_TYPE := kernel

-include $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/.config

$(NAME)_INCLUDES +=./ \
					./include \
					./include/class \
					./src/cd \
					./src/drivers \
					./src/drivers/comm \
					./src/drivers/hid \
					./src/drivers/msd \
					./src/examples/msd \
					./src/hid \
					./src/lib \
					./src/msc \
					./src/systems/none/afs \
					./src/systems/none \
					./src/uvc \

#usb lib
ifeq ($(CFG_USB),1)
$(NAME)_SOURCES += 	src/cd/mu_cntlr.c \
					src/cd/mu_descs.c \
					src/cd/mu_drc.c \
					src/cd/mu_fc.c \
					src/cd/mu_fdr.c \
					src/cd/mu_fun.c \
					src/cd/mu_funex.c \
					src/cd/mu_hc.c \
					src/cd/mu_hdr.c \
					src/cd/mu_hsdma.c \
					src/cd/mu_hst.c \
					src/cd/mu_list.c \
					src/cd/mu_mdr.c \
					src/cd/mu_pip.c \
					src/drivers/comm/mu_comif.c \
					src/drivers/hid/mu_hidif.c \
					src/drivers/hid/mu_hidkb.c \
					src/drivers/hid/mu_hidmb.c \
					src/drivers/msd/mu_mapi.c \
					src/drivers/msd/mu_mbot.c \
					src/drivers/msd/mu_mscsi.c \
					src/examples/msd/mu_msdfn.c \
					src/hid/usb_hid.c \
					src/lib/mu_bits.c \
					src/lib/mu_stack.c \
					src/lib/mu_stdio.c \
					src/lib/mu_strng.c \
					src/msc/usb_msd.c \
					src/systems/none/afs/board.c \
					src/systems/none/plat_uds.c \
					src/uvc/usb_uvc.c \
					src/uvc/uvc_driver.c
endif

