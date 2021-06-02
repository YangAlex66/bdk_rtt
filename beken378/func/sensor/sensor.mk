NAME := sensor

$(NAME)_TYPE := kernel

-include $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/.config

$(NAME)_INCLUDES +=./ \


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
					
$(NAME)_INCLUDES +=  ../../ip/ke \
					../../ip/mac \
					../../ip//lmac/src/rx/rxl \
		    			../../ip/lmac/src/rwnx \

#sensor lib

$(NAME)_SOURCES +=  sensor.c \



