NAME := rf_use

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
                    ../.././func/rwnx_intf
                  
$(NAME)_SOURCES := arbitrate.c
