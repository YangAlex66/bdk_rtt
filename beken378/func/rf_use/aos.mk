NAME := rf_use

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := 

$(NAME)_INCLUDES := . \
					../include \
                    ../rwnx_intf

$(NAME)_INCLUDES += ../../common 

$(NAME)_INCLUDES += ../../driver/include \
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

$(NAME)_SOURCES	:= arbitrate.c