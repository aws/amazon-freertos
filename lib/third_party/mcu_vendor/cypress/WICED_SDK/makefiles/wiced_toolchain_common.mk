#
# Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 # Cypress Semiconductor Corporation. All Rights Reserved.
 # 
 # This software, associated documentation and materials ("Software")
 # is owned by Cypress Semiconductor Corporation,
 # or one of its subsidiaries ("Cypress") and is protected by and subject to
 # worldwide patent protection (United States and foreign),
 # United States copyright laws and international treaty provisions.
 # Therefore, you may use this Software only as provided in the license
 # agreement accompanying the software package from which you
 # obtained this Software ("EULA").
 # If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 # non-transferable license to copy, modify, and compile the Software
 # source code solely for use in connection with Cypress's
 # integrated circuit products. Any reproduction, modification, translation,
 # compilation, or representation of this Software except as specified
 # above is prohibited without the express written permission of Cypress.
 #
 # Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 # EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 # WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 # reserves the right to make changes to the Software without notice. Cypress
 # does not assume any liability arising out of the application or use of the
 # Software or any product or circuit described in the Software. Cypress does
 # not authorize its products for use in any products where a malfunction or
 # failure of the Cypress product may reasonably be expected to result in
 # significant property damage, injury or death ("High Risk Product"). By
 # including Cypress's product in a High Risk Product, the manufacturer
 # of such system or application assumes all risk of such use and in doing
 # so agrees to indemnify Cypress against all liability.
#

#TOOLS_ROOT ?= $(SOURCE_ROOT)tools

OPENOCD_PATH      := $(TOOLS_ROOT)/OpenOCD/
PATH :=

JTAG         ?= CYW9WCD1EVAL1


ifeq ($(HOST_OS),Win32)
################
# Windows settings
################
COMMON_TOOLS_PATH := $(TOOLS_ROOT)/common/Win32/
export SHELL       = cmd.exe
EXECUTABLE_SUFFIX  := .exe
OPENOCD_FULL_NAME := $(OPENOCD_PATH)Win32/openocd-all-brcm-libftdi.exe -s $(OPENOCD_PATH)scripts

# Python
ifneq ($(wildcard C:\Python34\python.exe),)
PYTHON_FULL_NAME := C:\Python34\python.exe
endif
ifneq ($(wildcard C:\Python27\python.exe),)
PYTHON_FULL_NAME := C:\Python27\python.exe
endif

ifneq ($(IAR),1)
SLASH_QUOTE_START :=\"
SLASH_QUOTE_END :=\"
else
SLASH_QUOTE_START :="\"
SLASH_QUOTE_END :=\""
endif

ESC_QUOTE:="
ESC_SPACE:=$(SPACE)
CAT               := type
ECHO_BLANK_LINE   := "$(COMMON_TOOLS_PATH)echo$(EXECUTABLE_SUFFIX)"
ECHO_NO_NEWLINE   := "$(COMMON_TOOLS_PATH)echo$(EXECUTABLE_SUFFIX)" -n
ECHO              := echo
QUOTES_FOR_ECHO   :=
CMD_TRUNC         := "$(COMMON_TOOLS_PATH)trunc$(EXECUTABLE_SUFFIX)"
PERL              := "$(COMMON_TOOLS_PATH)perl$(EXECUTABLE_SUFFIX)"
LINT_EXE          := "$(TOOLS_ROOT)/splint/splint/bin/splint$(EXECUTABLE_SUFFIX)"
PERL_ESC_DOLLAR   :=$$
PYTHON            := $(PYTHON_FULL_NAME)
CLEAN_COMMAND     := if exist build $(call CONV_SLASHES,$(COMMON_TOOLS_PATH))rmdir /s /q build
MKDIR              = if not exist $(subst /,\,$1) mkdir $(subst /,\,$1)
CONV_SLASHES       = $(subst /,\,$1)
DIR                = $(dir $(subst /,\,$1))
TOUCH              = $(ECHO) >
CYGWIN :=
DEV_NULL          := nul
TRUE_CMD          := call
FALSE_CMD         := fail > nul 2>&1
LINE_SYMBOL       := &

# $(1) is the content, $(2) is the file to print to.
define PRINT
@$(ECHO) $(1)>>$(2)

endef

WRITE_FILE_CREATE =$(file >$(1),$(2))
WRITE_FILE_APPEND =$(file >>$(1),$(2))

else  # Win32
ifeq ($(HOST_OS),Linux32)
################
# Linux 32-bit settings
################

COMMON_TOOLS_PATH := $(TOOLS_ROOT)/common/Linux32/
export SHELL       = $(COMMON_TOOLS_PATH)dash
EXECUTABLE_SUFFIX  :=
OPENOCD_FULL_NAME := "$(OPENOCD_PATH)Linux32/openocd-all-brcm-libftdi" -s "$(OPENOCD_PATH)scripts"
SLASH_QUOTE_START :=\"
SLASH_QUOTE_END   :=\"
ESC_QUOTE         :=\"
ESC_SPACE         :=\$(SPACE)
CAT               := "$(COMMON_TOOLS_PATH)cat"
ECHO_BLANK_LINE   := "$(COMMON_TOOLS_PATH)echo"
ECHO_NO_NEWLINE   := "$(COMMON_TOOLS_PATH)echo" -n
# dash command shell has built in echo command
ECHO              := echo
QUOTES_FOR_ECHO   :="
CMD_TRUNC         := $(ECHO)
PERL              := "/usr/bin/perl"
PERL_ESC_DOLLAR   :=\$$
PYTHON            := "/usr/bin/python"
CLEAN_COMMAND     := "$(COMMON_TOOLS_PATH)rm" -rf build
MKDIR              = "$(COMMON_TOOLS_PATH)mkdir$(EXECUTABLE_SUFFIX)" -p $1
CONV_SLASHES       = $1
TOUCH              = $(ECHO) >
DEV_NULL          := /dev/null
TRUE_CMD          := true
FALSE_CMD         := false
LINE_SYMBOL       := ;

# $(1) is the content, $(2) is the file to print to.
define PRINT
@$(ECHO) '$(1)'>>$(2)

endef

WRITE_FILE_CREATE =$(ECHO) '$(subst ',\047,$(subst \,\\,$(2)))' > $(1);
WRITE_FILE_APPEND =$(ECHO) '$(subst ',\047,$(subst \,\\,$(2)))' >> $(1);

else # Linux32
ifeq ($(HOST_OS),Linux64)
################
# Linux 64-bit settings
################

COMMON_TOOLS_PATH := $(TOOLS_ROOT)/common/Linux64/
export SHELL       = $(COMMON_TOOLS_PATH)dash
EXECUTABLE_SUFFIX  :=
OPENOCD_FULL_NAME := "$(OPENOCD_PATH)Linux64/openocd-all-brcm-libftdi" -s "$(OPENOCD_PATH)scripts"
SLASH_QUOTE_START :=\"
SLASH_QUOTE_END   :=\"
ESC_QUOTE         :=\"
ESC_SPACE         :=\$(SPACE)
CAT               := "$(COMMON_TOOLS_PATH)cat"
ECHO_BLANK_LINE   := "$(COMMON_TOOLS_PATH)echo"
ECHO_NO_NEWLINE   := "$(COMMON_TOOLS_PATH)echo" -n
# dash command shell has built in echo command
ECHO              := echo
QUOTES_FOR_ECHO   :="
CMD_TRUNC         := $(ECHO)
PERL              := "/usr/bin/perl"
PERL_ESC_DOLLAR   :=\$$
PYTHON            := "/usr/bin/python"
CLEAN_COMMAND     := "$(COMMON_TOOLS_PATH)rm" -rf build
MKDIR              = "$(COMMON_TOOLS_PATH)mkdir$(EXECUTABLE_SUFFIX)" -p $1
CONV_SLASHES       = $1
TOUCH              = $(ECHO) >
DEV_NULL          := /dev/null
TRUE_CMD          := true
FALSE_CMD         := false
LINE_SYMBOL       := ;

# $(1) is the content, $(2) is the file to print to.
define PRINT
@$(ECHO) '$(1)'>>$(2)

endef

WRITE_FILE_CREATE =$(ECHO) '$(subst ',\047,$(subst \,\\,$(2)))' > $(1);
WRITE_FILE_APPEND =$(ECHO) '$(subst ',\047,$(subst \,\\,$(2)))' >> $(1);
# # Check the file writing is working correctly
# # should result in: $'""\"\"\\"\\\"\\
# TEST_DATA := $$'""\"\"\\"\\\"\\
# $(info TEST_DATA=$(TEST_DATA))
# $(info $(call WRITE_FILE_CREATE,test.txt,$(TEST_DATA)))
# $(info done=$(shell $(call WRITE_FILE_CREATE,test.txt,$(TEST_DATA))))

else # Linux64
ifeq ($(HOST_OS),OSX)
################
# OSX settings
################

COMMON_TOOLS_PATH := $(TOOLS_ROOT)/common/OSX/
export SHELL       = $(COMMON_TOOLS_PATH)dash
EXECUTABLE_SUFFIX  :=
OPENOCD_FULL_NAME := "$(OPENOCD_PATH)OSX/openocd-all-brcm-libftdi" -s "$(OPENOCD_PATH)scripts"
SLASH_QUOTE_START :=\"
SLASH_QUOTE_END   :=\"
ESC_QUOTE         :=\"
ESC_SPACE         :=\$(SPACE)
CAT               := "$(COMMON_TOOLS_PATH)cat"
ECHO_BLANK_LINE   := "$(COMMON_TOOLS_PATH)echo"
ECHO_NO_NEWLINE   := "$(COMMON_TOOLS_PATH)echo" -n
ECHO              := "$(COMMON_TOOLS_PATH)echo"
QUOTES_FOR_ECHO   :="
CMD_TRUNC         := $(ECHO)
PERL              := "/usr/bin/perl"
PERL_ESC_DOLLAR   :=\$$
PYTHON            := "/usr/bin/python"
CLEAN_COMMAND     := "$(COMMON_TOOLS_PATH)rm" -rf build
MKDIR              = "$(COMMON_TOOLS_PATH)mkdir" -p $1
CONV_SLASHES       = $1
TOUCH              = $(ECHO) >
DEV_NULL          := /dev/null
TRUE_CMD          := true
FALSE_CMD         := false
LINE_SYMBOL       := ;

# $(1) is the content, $(2) is the file to print to.
define PRINT
@$(ECHO) '$(1)'>>$(2)

endef

WRITE_FILE_CREATE =$(ECHO) '$(2)' > $(1);
WRITE_FILE_APPEND =$(ECHO) '$(2)' >> $(1);

else # OSX

$(error incorrect 'make' used ($(MAKE)) - please use:  (Windows) .\make.exe <target_string>    (OS X, Linux) ./make <target_string>)
endif # OSX
endif # Linux64
endif # Linux32
endif # Win32


# Set shortcuts to the compiler and other tools
RM      := "$(COMMON_TOOLS_PATH)rm$(EXECUTABLE_SUFFIX)" -f
CP      := "$(COMMON_TOOLS_PATH)cp$(EXECUTABLE_SUFFIX)" -f
MAKE    := "$(COMMON_TOOLS_PATH)make$(EXECUTABLE_SUFFIX)"
BIN2C   := "$(COMMON_TOOLS_PATH)bin2c$(EXECUTABLE_SUFFIX)"


SHOULD_I_WAIT_FOR_DOWNLOAD := $(filter download download_apps ota2_download ota2_factory_download ota2_apps_download, $(MAKECMDGOALS))
BUILD_STRING ?= $(strip $(filter-out $(MAKEFILE_TARGETS) debug download download_apps download_only package_apps run terminal no_dct no_tinybl ota2_image ota2_download ota2_apps_download ota2_factory_image ota2_factory_download ota2_manuf_image ota2_manuf_download package factory_reset_dct, $(MAKECMDGOALS)))
BUILD_STRING_TO_DIR = $(subst .,/,$(1))
DIR_TO_BUILD_STRING = $(subst /,.,$(1))
CLEANED_BUILD_STRING := $(BUILD_STRING)
BUILD_DIR    :=  build

OUTPUT_DIR   := $(BUILD_DIR)/$(CLEANED_BUILD_STRING)

# Newline Macro
define newline


endef


# Use VERBOSE=1 to get full output
ifneq ($(VERBOSE),1)
QUIET:=@
SILENT:=-s
QUIET_SHELL =$(shell $(1))
QUIET_SHELL_IN_MACRO =$$(shell $(1))
else
QUIET:=
SILENT:=
QUIET_SHELL =$(shell $(1)$(info $(1)))
QUIET_SHELL_IN_MACRO =$$(shell $(1)$$(info $(1)))
endif



COMMA :=,

SPACE :=
SPACE +=

# $(1) is a string to be escaped
ESCAPE_BACKSLASHES =$(subst \,\\,$(1))


# Cypress internal only - Add gerrit hook for changeid
ifneq ($(wildcard $(TOOLS_ROOT)/style/gerrit_commit-msg),)
ifneq ($(wildcard $(SOURCE_ROOT).git),)

CYPRESS_INTERNAL :=yes

TOOLCHAIN_HOOK_TARGETS += $(SOURCE_ROOT).git/hooks/commit-msg

$(SOURCE_ROOT).git/hooks/commit-msg:  $(TOOLS_ROOT)/style/gerrit_commit-msg
	$(QUIET)$(ECHO) Adding gerrit git hook
	$(QUIET)$(CP) $(TOOLS_ROOT)/style/gerrit_commit-msg $(SOURCE_ROOT).git/hooks/commit-msg


endif
endif

# Cypress internal only - Add git commit hook for style checker
ifneq ($(wildcard $(TOOLS_ROOT)/style/git_style_checker.pl),)
ifneq ($(wildcard $(TOOLS_ROOT)/style/pre-commit),)
ifneq ($(wildcard $(SOURCE_ROOT).git),)

TOOLCHAIN_HOOK_TARGETS += $(SOURCE_ROOT).git/hooks/pre-commit

$(SOURCE_ROOT).git/hooks/pre-commit: $(TOOLS_ROOT)/style/pre-commit
	$(QUIET)$(ECHO) Adding style checker git hook
	$(QUIET)$(CP) $(TOOLS_ROOT)/style/pre-commit $(SOURCE_ROOT).git/hooks/pre-commit

endif
endif
endif

# Cypress internal only - Add git push hook for commit checker
ifneq ($(wildcard $(TOOLS_ROOT)/style/git_commit_checker.pl),)
ifneq ($(wildcard $(TOOLS_ROOT)/style/pre-push),)
ifneq ($(wildcard $(SOURCE_ROOT).git),)

TOOLCHAIN_HOOK_TARGETS += $(SOURCE_ROOT).git/hooks/pre-push

$(SOURCE_ROOT).git/hooks/pre-push: $(TOOLS_ROOT)/style/pre-push
	$(QUIET)$(ECHO) Adding commit checker git hook
	$(QUIET)$(CP) $(TOOLS_ROOT)/style/pre-push $(SOURCE_ROOT).git/hooks/pre-push

endif
endif
endif

# Cypress internal only - Copy Eclipse .project file if it doesn't exist
ifeq ($(wildcard $(SOURCE_ROOT).project),)
ifneq ($(wildcard $(TOOLS_ROOT)/eclipse_project/.project),)

$(info Copying Eclipse .project file to source tree root)

$(shell $(CP) $(TOOLS_ROOT)/eclipse_project/.project $(SOURCE_ROOT) )

endif
endif
# Cypress internal only - Copy Eclipse .cproject file if it doesn't exist
ifeq ($(wildcard $(SOURCE_ROOT).cproject),)
ifneq ($(wildcard $(TOOLS_ROOT)/eclipse_project/.cproject),)

$(info Copying Eclipse .cproject file to source tree root)

$(shell $(CP) $(TOOLS_ROOT)/eclipse_project/.cproject $(SOURCE_ROOT) )

endif
endif

#########
# Expand wildcard platform names.
# Consider all platforms in platforms/* and platform/*/*
define EXPAND_WILDCARD_PLATFORMS
$(eval POSSIBLE_PLATFORMS :=) \
$(eval EXPANDED_PLATFORMS :=) \
$(foreach ENTRY, $(1), \
$(eval WILDCARD_PLATFORM := $(call BUILD_STRING_TO_DIR, $(ENTRY))) \
$(eval POSSIBLE_PLATFORMS += $(subst platforms/,,$(wildcard platforms/$(WILDCARD_PLATFORM)))) \
$(eval POSSIBLE_PLATFORMS += $(subst platforms/,,$(wildcard platforms/$(WILDCARD_PLATFORM)/*)))) \
$(eval $(foreach PLATFORM, $(POSSIBLE_PLATFORMS), \
$(eval $(if $(wildcard platforms/$(PLATFORM)/$(notdir $(PLATFORM)).mk), EXPANDED_PLATFORMS += $(call DIR_TO_BUILD_STRING, $(PLATFORM))))))\
${EXPANDED_PLATFORMS}
endef

##########
# Recurse directories to find valid WICED components.
# $(1) = starting directory
# $(2) = name of variable to which to add components that are found
define RECURSE_DIR_COMPONENT_SEARCH
$(foreach file, $(wildcard $(1)/*), $(if $(wildcard $(file)/*), $(if $(wildcard $(file)/$(notdir $(file)).mk), $(eval $(2) += $(file)),) $(call RECURSE_DIR_COMPONENT_SEARCH,$(file),$(2)),))
endef


##########
# Find all valid WICED components.
# $(1) = name of variable to which to add components that are found
# $(2) = list of component directories
define FIND_VALID_COMPONENTS
$(call RECURSE_DIR_COMPONENT_SEARCH, $(patsubst %/,%,$(SOURCE_ROOT)),$(1)) \
$(eval $(1) := $(subst ./,,$($(strip $(1))))) \
$(foreach compdir, $(2),$(eval $(1) := $(patsubst $(compdir)/%,%,$($(strip $(1)))))) \
$(eval $(1) := $(subst /,.,$($(strip $(1)))))
endef

##########
# Strip duplicate items in list without sorting
# $(1) = List of items to de-duplicate
unique = $(eval seen :=)$(foreach _,$1,$(if $(filter $_,${seen}),,$(eval seen += $_)))${seen}


CURRENT_MAKEFILE = $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

##########
# For JLINK common script
ifeq ($(JTAG), jlink-native)
JLINK_NATIVE := 1
endif
ifeq ($(JLINK_NATIVE), 1)
# User can assign specific JLINK_PATH & JLINK_EXE manually:
# ex :
#		[Win32]
#		test.console-CYW943907AEVAL1F JTAG=jlink-native JLINK_PATH="../../../../Program\ Files\ \(x86\)/SEGGER/JLink_V630c/" JLINK_EXE="Jlink.exe" download run
#
#		[Linux]
#		test.console-CYW943907AEVAL1F JTAG=jlink-native JLINK_PATH="/opt/SEGGER/JLink/" JLINK_EXE="JLinkExe" download run
JLINK_PATH		?=
JLINK_EXE		?=
JLINK_CMD_SCRIPT:= jlink_commander_script.txt
JLINK_MAKE_SCRIPT_ALL = $(shell $(ECHO) r > $(JLINK_CMD_SCRIPT)$(LINE_SYMBOL) \
					$(3) \
					$(ECHO) loadbin $(1) $(shell $(PERL) -le 'print sprintf("0x%X", $(2) + $(JLINK_SFLASH_BASE_LOC))') >> $(JLINK_CMD_SCRIPT)$(LINE_SYMBOL) \
					$(ECHO) qc >> $(JLINK_CMD_SCRIPT))
JLINK_EXE_SCRIPT = $(call CONV_SLASHES,$(JLINK_PATH)$(JLINK_EXE)) -device $(JLINK_DEVICE) -if $(JLINK_INTERFACE) -speed $(JLINK_SPEED) $(JLINK_JTAG_CONF) -autoconnect $(JLINK_AUTO_CONNECT) -CommanderScript $(JLINK_CMD_SCRIPT)
endif
