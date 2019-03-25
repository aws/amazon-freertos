include ../toplevel.mk
include $(TOPDIR)/Tools/subdir.mk

COMPILER_OS=$(shell uname -o)
$(warning COMPILER_OS: $(COMPILER_OS))
export COMPILER_OS
ifeq ($(COMPILER_OS),Cygwin)
COMPILER_OS_CYGWIN=1
else
COMPILER_OS_CYGWIN=0
endif
export COMPILER_OS_CYGWIN
