# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.

OS := $(shell python sdk/tools/bin/get_os.py -s)

ifeq ($(OS), Linux)
  include build/host/devhost_linux.mk
endif

ifneq ($(findstring CYGWIN, ${OS}), )
  include build/host/devhost_cygwin.mk
endif

ifneq ($(findstring MINGW, ${OS}), )
  include build/host/devhost_mingw.mk
endif

ifneq ($(findstring windows, ${OS}), )
  include build/host/devhost_gnuwin32.mk
endif

ifeq ($(OS), Darwin)
  include build/host/devhost_darwin.mk
endif
