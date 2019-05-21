# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.

################# Build Configuration

b-output-dir-y := bin
#################################################
# BIN_DIR Handling

ifdef BIN_DIR
  override BIN_DIR := $(call b-abspath,$(BIN_DIR))
endif

BIN_DIR ?= $(b-output-dir-y)
#################################################

b-autoconf-file := sdk/src/incl/autoconf.h
global-preinclude-y := $(b-autoconf-file)
