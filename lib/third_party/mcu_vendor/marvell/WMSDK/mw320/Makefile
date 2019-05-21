# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.

# Please refer to:
#  - build/README for quick instructions on writing a build.mk file
#  - Build System Documentation for detailed explanation of the build
#    system
# Default BUILD_MODE is release
BUILD_MODE ?= release
# Do not manually update .config.out.
# Modify .config if needed

-include .config
include build/amzsdk.mk

############################################
# Marvell internal Modules
-include internal.mk

############################################
# SDK Modules

subdir-y += sdk/src/core/cli
subdir-y += sdk/src/core/wmstdio
subdir-y += sdk/src/core/partition
subdir-y += sdk/src/core/mdev
subdir-y += sdk/src/core/pwrmgr
subdir-y += sdk/src/core/util
subdir-y += sdk/src/core/wmtime
subdir-y += sdk/src/libc/src
subdir-y += sdk/src/middleware/rfget
subdir-y += sdk/src/middleware/psm
subdir-y += sdk/src/wlan/wifidriver
subdir-y += sdk/src/wlan/wlcmgr
subdir-y += sdk/src/platform/os/freertos
subdir-y += sdk/src/platform/net/freertos-plus-tcp

ifneq ($(BUILD_TYPE),demo)
subdir-y += $(call b-abspath,../../../../../../demos/marvell/mw300_rd/common/application_code/marvell_code)
subdir-y += $(call b-abspath,../../../../../../tests/marvell/mw300_rd/common/application_code/marvell_code)
else
subdir-y += $(call b-abspath,../../../../../../demos/marvell/mw300_rd/common/application_code/marvell_code)
endif
subdir-y += $(call b-abspath,sdk/external/FreeRTOS)
subdir-y += $(call b-abspath,sdk/external/utils)
subdir-y += $(call b-abspath,sdk/external/unity)
subdir-y += $(call b-abspath,sdk/external/FreeRTOS-Plus-TCP)
subdir-y += $(call b-abspath,sdk/external/secure_sockets)
subdir-y += $(call b-abspath,sdk/external/mbedtls)
subdir-y += $(call b-abspath,sdk/external/pkcs11)
subdir-y += $(call b-abspath,sdk/external/crypto)
subdir-y += $(call b-abspath,sdk/external/tls)
subdir-y += $(call b-abspath,sdk/external/mqtt)
subdir-y += $(call b-abspath,sdk/external/bufferpool)
subdir-y += $(call b-abspath,sdk/external/ota)
subdir-y += $(call b-abspath,sdk/external/jsmn)
subdir-y += $(call b-abspath,sdk/external/tinycbor)
subdir-y += $(call b-abspath,sdk/external/wifi)

############################################
# Configurable components
subdir-$(CONFIG_CPU_MW300) += sdk/src/drivers/mw300
subdir-y += sdk/src/drivers/common/hardfault

ifeq ($(CONFIG_ENABLE_ROM_LIBS), )
subdir-$(CONFIG_XZ_DECOMPRESSION) += sdk/src/middleware/xz
endif

############################################
# APP management

ifndef APP
ifeq ($(CONFIG_ADK_SUPPORT),y)
  subdir-y += sample_apps/adk_accessories/sample_apps.mk
else
  subdir-y += sample_apps/sample_apps.mk
endif
else
  override APP := $(call b-abspath,$(APP))
  ifeq ($(wildcard $(APP)),)
    $(error "Please specify a valid APP path")
  endif
  subdir-y += $(APP)
endif

############################################
# Boot2
#subdir-$(tc-arm_gcc-env-y) += boot2

############################################
# Tools
subdir-y += sdk/tools/src/host-tools/axf2firmware
subdir-$(b-secboot-y) += sdk/tools/src/host-tools/secureboot
subdir-y += sdk/tools/src/host-tools/psm-create-part
subdir-y += sdk/tools/src/host-tools/uartboot
subdir-y += sdk/tools/src/host-tools/fw_generator
subdir-y += sdk/tools/src/host-tools/psm-create-part
subdir-y += sdk/tools/src/host-tools/layoutgen
subdir-y += sdk/tools/src/host-tools/salt-verifier
subdir-y += sdk/tools/src/host-tools/uartboot
subdir-y += sdk/tools/src/platform-tools/flashprog

############################################
# Documentation
#subdir-y += doxy

############################################

include build/amzsdk_rules.mk

############################################
ifneq ($(NOISY),1)
silent := -s
endif

############################################

### And now the targets
.PHONY: all build post-build-1 build-help help clean

# all: The build dependencies are:
#                all -> post-build-1 -> build
all: post-build-1
post-build-1: build

# The entire build system is dependency driven. By default, build
# all the apps and boot2. This will build the other dependencies as
# required
build: $(b-exec-apps-y) #boot2

debug release:
	$(error "This target is no longer supported. Please pass BUILD_MODE option as: make BUILD_MODE=$@")

#clean: boot2.clean

############################################
help: build-help

build-help:
	@echo ""
	@echo "Compile SDK and sample applications"
	@echo "-----------------------------------"
	@echo "   $$ make [BOARD=<board_name>]"
	@echo "   Supported boards can be found in sdk/src/boards/".
	@echo "   Default board is mw300_rd"
	@echo ""
	@echo "Build XIP enabled applications for 88MW30x"
	@echo "-------------------------------------"
	@echo "   $$ make [BOARD=<board_name>] XIP=1"
	@echo "   Note: With XIP ramload will not be functional for AXF image/s"
	@echo ""

############################################
