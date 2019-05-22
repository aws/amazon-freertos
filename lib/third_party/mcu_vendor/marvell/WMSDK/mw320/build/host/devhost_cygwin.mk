# Copyright (C) 2008-2017, Marvell International Ltd.
# All Rights Reserved.

os_dir := Windows
file_ext := .exe

make_version_check-$(tc-arm_gcc-env-y) := $(shell expr $(MAKE_VERSION) \< 4.1)

ifeq ($(make_version_check-y),1)
  $(error "Please use make version 4.1 or higher")
endif

######################################
# Cygwin python check
cygwin_python_used := $(shell which python | tail -1)

define cygwin_python_error

*** It seems that you are using Cygwin Python. Please install Python separately for Windows.
*** For more details please refer to developement host setup.

endef

ifeq ($(cygwin_python_used),/usr/bin/python)
  $(error $(cygwin_python_error))
endif
######################################

t_mconf := mconf_not_on_cygwin
mconf_not_on_cygwin:
	@echo ""
	@echo "The 'menuconfig' option is not supported in Cygwin"
	@echo "Please use 'make config' instead"
	@echo ""
	@false

# This is used to replace ":" in drive letter
# This will be handy in resolving issues in rules/targets
escape_dir_name := _wmdrive

# Alphabet to be escaped
escape_let := :

# List of  Drive letters
drive-list-y := C D E F

# Function to resolve input path
define b-abspath
$(shell cygpath -m $(1))
endef

# Check if both arguments has same arguments. Result is empty string if equal.
arg-check = $(strip $(filter-out $(1),$(subst ",\",$(cmd_$(subst $(escape_let),$(escape_dir_name),$@)))) \
                    $(filter-out $(subst ",\",$(cmd_$(subst $(escape_let),$(escape_dir_name),$@))),$(1)))

# execute command and store the command line in $@.cmd file
cmd_save = @$(t_printf) "cmd_$(subst $(escape_let),$(escape_dir_name),$@) := %s\n" "$(1)" > $@.cmd
