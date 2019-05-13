# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.

# Description:
# -------------
# This file, toolchain.mk
#
# 	Sets up list of supported toolchains
#	It also includes appropriate toolchain file
#	based on input from user, default toolchain is arm_gcc.

TOOLCHAIN ?= arm_gcc
toolchain := $(TOOLCHAIN)
b-supported-toolchain-y := arm_gcc iar

define toolchain_error_log

*** $(1) toolchain is not supported.
*** Supported set of toolchains is $(b-supported-toolchain-y)

endef


ifeq ($(filter $(toolchain),$(b-supported-toolchain-y)),)
  $(error $(call toolchain_error_log,$(toolchain)))
endif

include build/toolchains/toolchain_$(toolchain).mk
