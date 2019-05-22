# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.

os_dir := Windows
file_ext := .exe

t_mconf := mconf_not_on_gnuwin32
mconf_not_on_gnuwin32:
	@echo ""
	@echo "The 'menuconfig' option is not supported in GNUWin32"
	@echo "Please use 'make config' instead"
	@echo ""
	@exit 1

define gnuwin32_tools_error

*** The option $(1) is not supported in GNUWin32
*** Please refer to sdk/tools/bin/Windows/README for more details

endef

tools_goal := $(filter tools.install tools.clean,$(MAKECMDGOALS))
ifneq ($(tools_goal),)
  $(error $(call gnuwin32_tools_error,$(tools_goal)))
endif

# Function to resolve input path
define b-abspath
$(join $(filter %:,$(subst :,: ,$(1))),$(abspath $(filter-out %:,$(subst :,: ,$(subst \,/,$(1))))))
endef

# This is used to replace ":" in drive letter
# This will be handy in resolving issues in rules/targets
escape_dir_name := _wmdrive

# Alphabet to be escaped
escape_let := :

# List of  Drive letters
drive-list-y := c d e f C D E F

t_bin_path := sdk/tools/bin/GnuWin32/bin

t_which := "$(t_bin_path)/which.exe"
t_uname	:= "$(t_bin_path)/uname.exe"

t_cp	:= "$(t_bin_path)/cp.exe"
t_mv	:= "$(t_bin_path)/mv.exe"
t_cmp	:= "$(t_bin_path)/cmp.exe"
t_mkdir := "$(t_bin_path)/mkdir.exe"
t_cat	:= "$(t_bin_path)/cat.exe"
t_rm	:= "$(t_bin_path)/rm.exe"
t_printf := "$(t_bin_path)/printf.exe"
t_date  := "$(t_bin_path)/date.exe"
t_python := $(shell $(t_which) python)

cmd_mkdir = $(t_mkdir) -p $(subst :,:\,$(1))

# Check if both arguments has same arguments. Result is empty string if equal.
arg-check = $(strip $(filter-out $(1),$(subst ",\",$(cmd_$(subst $(escape_let),$(escape_dir_name),$@)))) \
                    $(filter-out $(subst ",\",$(cmd_$(subst $(escape_let),$(escape_dir_name),$@))),$(1)))

# execute command and store the command line in $@.cmd file
cmd_save = @$(t_printf) "cmd_$(subst $(escape_let),$(escape_dir_name),$@) := %%s\n" "$(1)" > $@.cmd
