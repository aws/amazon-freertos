#/**************************************************************************
# * Kevin 2014-02-24                                                       *
# **************************************************************************/

#---------------------------------------------------------------------------
# Constant Variable definition
#---------------------------------------------------------------------------
include ../rules.mk
include ../verbose.mk

ifeq ($(COMPILER_OS_CYGWIN),1)
TOPDIR=../..
endif

include $(TOPDIR)/Tools/toolchain.def

OUTPUT_PATH=.

#---------------------------------------------------------------------------
# Target definition (User)
#---------------------------------------------------------------------------
GOAL = W600

#---------------------------------------------------------------------------
# Source section (User)
#---------------------------------------------------------------------------
ASM_SRC := $(TOPDIR)/Platform/Boot/gcc/startup_ARMCM3.s
C_SRC := $(TOPDIR)/Platform/Boot/gcc/retarget_gcc.c \
	 $(TOPDIR)/Platform/Boot/gcc/misc.c \
	 $(TOPDIR)/App/main.c

#---------------------------------------------------------------------------
# Linker options
#---------------------------------------------------------------------------
LDFLAGS	   += -static
LDFLAGS    += -nostartfiles
LDFLAGS    += -Tlink_w600.ld
LDFLAGS    += -mthumb
LDFLAGS    += -mcpu=cortex-m3
#---------------------------------------------------------------------------
# Link Library Section
#---------------------------------------------------------------------------
#LIB_SRC := $(LIB_DIR)/wmboot.a
LIB_SRC += $(LIB_DIR)/oneshot.a \
	   $(LIB_DIR)/wmcJSON.a \
	   $(LIB_DIR)/wmcmd.a \
	   $(LIB_DIR)/wmcommon.a \
	   $(LIB_DIR)/wmdemo.a \
	   $(LIB_DIR)/wmdhcpserver.a \
	   $(LIB_DIR)/wmdnsserver.a \
	   $(LIB_DIR)/wmdriver.a \
	   $(LIB_DIR)/wmhttpclient.a \
	   $(LIB_DIR)/wmiperf.a \
	   $(LIB_DIR)/wmlwip.a \
	   $(LIB_DIR)/wmmain.a \
	   $(LIB_DIR)/wmota.a \
	   $(LIB_DIR)/wmntp.a \
	   $(LIB_DIR)/wmping.a \
	   $(LIB_DIR)/wmrtos.a \
	   $(LIB_DIR)/wmssl.a \
	   $(LIB_DIR)/wmweb.a \
	   $(LIB_DIR)/wmwebsocket.a \
	   $(LIB_DIR)/wmsslserver.a \
	   $(LIB_DIR)/libairkiss_log.a

ifeq ($(COST_DOWN),1)
LIB_SRC += $(LIB_DIR)/wlan_costdown.a
else
LIB_SRC += $(LIB_DIR)/wlan.a
endif

#---------------------------------------------------------------------------
# Implicit rules
#---------------------------------------------------------------------------
.c.o:
	$(CC) $(CFLAGS) -c -o $*.o $< $(INCLUDES)

.s.o:
	$(ASM) $(ASMFLAGS) -c -o $*.o $< $(INCLUDES)

#---------------------------------------------------------------------------
# Explicit ruls
#---------------------------------------------------------------------------
OBJ_FILES	= $(C_SRC:%.c=%.o) $(ASM_SRC:%.s=%.o)
_ENV_OS=$(subst /,-,$(COMPILER_OS))
ENV_OS=$(shell echo $(_ENV_OS) | tr '[A-Z]' '[a-z]')

MODULES := library
REVISION:=$(shell $(TOPDIR)/scripts/getver.sh)

all: $(MODULES) $(GOAL)
	@./createimg.sh
	#@./createimg.sh -e $(ENV_OS) -r ${REVISION}

$(MODULES):
	@for i in ${Src_App_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i || exit $?; done
	@for i in ${Src_Net_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w || exit $?; done
	@for i in ${Src_Wlan_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w || exit $?; done
	@for i in ${Src_RTOS_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w || exit $?; done
	@for i in ${WM_Demo_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w || exit $?; done
	@for i in ${Platform_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w || exit $?; done
	@echo "finish MODULES"

$(GOAL): $(GOAL).elf 

$(GOAL).elf: $(OBJ_FILES) $(LIB_SRC)
	$(CC) -o $(GOAL).elf $(OBJ_FILES) $(LDFLAGS) \
		-Wl,--gc-sections \
		-Wl,--start-group $(LIB_SRC) -Wl,--end-group \
		-Wl,-Map,$(OUTPUT_PATH)/$(GOAL).map -Wl,--warn-common
	$(ELF) \
		--output-target=binary \
		-S -g -x -X -R .sbss -R .bss -R .reginfo -R .stack \
		$(GOAL).elf $(GOAL).bin	
	
clean:
	@for i in ${Src_App_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i clean || exit $?; done
	@for i in ${Src_Net_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w clean || exit $?; done
	@for i in ${Src_Wlan_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w clean || exit $?; done
	@for i in ${Src_RTOS_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w clean || exit $?; done
	@for i in ${WM_Demo_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w clean || exit $?; done
	@for i in ${Platform_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w clean || exit $?; done
	$(RM) -f $(OBJ_FILES:.o=.d) \
		$(OBJ_FILES) $(GOAL) \
		$(OUTPUT_PATH)/$(GOAL).map \
		$(GOAL).elf \
		$(GOAL).text \
		$(OUTPUT_PATH)/$(GOAL).bin \
		$(GOAL).nm \
		*.bin \
		*.hex
	
