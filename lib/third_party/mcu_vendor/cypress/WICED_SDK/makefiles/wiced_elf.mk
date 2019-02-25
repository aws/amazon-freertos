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
ifeq ($(IAR),1)
MAKEFLAGS += -j1
endif

include $(MAKEFILES_PATH)/wiced_toolchain_common.mk

CONFIG_FILE := build/$(CLEANED_BUILD_STRING)/config.mk

include $(CONFIG_FILE)

# Include all toolchain makefiles - one of them will handle the architecture
include $(MAKEFILES_PATH)/wiced_toolchain_$(TOOLCHAIN_NAME).mk

.PHONY: display_map_summary build_done

##################################
# Filenames
##################################

ifeq ($(TOOLCHAIN_NAME),IAR)
LINK_OUTPUT_FILE          :=$(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING)$(LINK_OUTPUT_SUFFIX)
STRIPPED_LINK_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.stripped$(LINK_OUTPUT_SUFFIX))
FINAL_OUTPUT_FILE         :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=$(FINAL_OUTPUT_SUFFIX))
else
LINK_OUTPUT_FILE          :=$(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING)$(LINK_OUTPUT_SUFFIX)
STRIPPED_LINK_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.stripped$(LINK_OUTPUT_SUFFIX))
FINAL_OUTPUT_FILE         :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=$(FINAL_OUTPUT_SUFFIX))
endif

ifeq (1,$(XIP_SUPPORT))
XIP_OUTPUT_FILE           :=$(OUTPUT_DIR)/binary/XIP$(FINAL_OUTPUT_SUFFIX)
endif

MAP_OUTPUT_FILE           :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.map)
MAP_CSV_OUTPUT_FILE       :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_map.csv)

DCT_FILE                  ?= $(SOURCE_ROOT)WICED/internal/dct.c
LINK_DCT_FILE             := $(OUTPUT_DIR)/DCT$(LINK_OUTPUT_SUFFIX)
STRIPPED_LINK_DCT_FILE    :=$(LINK_DCT_FILE:$(LINK_OUTPUT_SUFFIX)=.stripped$(LINK_OUTPUT_SUFFIX))
FINAL_DCT_FILE            :=$(LINK_DCT_FILE:$(LINK_OUTPUT_SUFFIX)=$(FINAL_OUTPUT_SUFFIX))
MAP_DCT_FILE              :=$(LINK_DCT_FILE:$(LINK_OUTPUT_SUFFIX)=.map)

OPENOCD_LOG_FILE          ?= $(OUTPUT_DIR)/openocd_log.txt

LIBS_DIR                  := $(OUTPUT_DIR)/libraries
LINK_OPTS_FILE            := $(OUTPUT_DIR)/binary/link.opts

LINT_OPTS_FILE            := $(OUTPUT_DIR)/binary/lint.opts

# Include extra makefiles, but only when we are the root make
# When the root build starts another build, SUB_BUILD exists as: "sflash_write", "bootloader", "tiny_bootloader"
# SUB_BUILD is set to "1" by the Jenkins make so that no extra builds occur
ifeq (,$(SUB_BUILD))
ifneq (,$(EXTRA_TARGET_MAKEFILES))
$(foreach makefile_name,$(EXTRA_TARGET_MAKEFILES),$(eval include $(makefile_name)))
endif
endif

# Include extra platform-specific makefiles for *all* builds
# Do not use this to build "sflash_write", "bootloader", "tiny_bootloader"
# Use this for other uses that are necessary for a Jenkins build as well as normal builds
# This is to get around the SUB_BUILD flag in the use above
ifneq (,$(EXTRA_PLATFORM_MAKEFILES))
$(foreach makefile_name,$(EXTRA_PLATFORM_MAKEFILES),$(eval include $(makefile_name)))
endif

include $(MAKEFILES_PATH)/wiced_resources.mk
include $(MAKEFILES_PATH)/wiced_apps.mk

# include the correct FLASH positioning
ifeq (1,$(OTA2_SUPPORT))
GLOBAL_DEFINES	+= OTA2_SUPPORT=1
include platforms/$(subst .,/,$(PLATFORM))/ota2_image_defines.mk
#needed for wiced_apps_lut.c
WICED_SDK_DEFINES += -DOTA2_SUPPORT=1
endif

##################################
# Macros
##################################

###############################################################################
# MACRO: GET_BARE_LOCATION
# Returns a the location of the given component relative to source-tree-root
# rather than from the cwd
# $(1) is component
GET_BARE_LOCATION =$(patsubst $(call ESCAPE_BACKSLASHES,$(SOURCE_ROOT))%,%,$(strip $($(1)_LOCATION)))


###############################################################################
# MACRO: BUILD_C_RULE
# Creates a target for building C language files (*.c)
# $(1) is component, $(2) is the source file
define BUILD_C_RULE
-include $(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(2:.c=.d)
$(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(2:.c=.o): $(strip $($(1)_LOCATION))$(2) $(CONFIG_FILE) $$(dir $(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(2)).d $(RESOURCES_DEPENDENCY) $(LIBS_DIR)/$(1).c_opts | $(EXTRA_PRE_BUILD_TARGETS)
	$$(if $($(1)_START_PRINT),,$(eval $(1)_START_PRINT:=1) $(QUIET)$(ECHO) Compiling $(1) )
	$(QUIET)$(CC) $(OPTIONS_IN_FILE_OPTION)$(LIBS_DIR)/$(1).c_opts -o $$@ $$< $(COMPILER_SPECIFIC_STDOUT_REDIRECT)
endef

###############################################################################
# MACRO: CHECK_HEADER_RULE
# Compiles a C language header file to ensure it is stand alone complete
# $(1) is component, $(2) is the source header file
define CHECK_HEADER_RULE
$(eval $(1)_CHECK_HEADER_LIST+=$(OUTPUT_DIR)/Modules/$(strip $($(1)_LOCATION))$(2:.h=.chk) )
.PHONY: $(OUTPUT_DIR)/Modules/$(strip $($(1)_LOCATION))$(2:.h=.chk)
$(OUTPUT_DIR)/Modules/$(strip $($(1)_LOCATION))$(2:.h=.chk): $(strip $($(1)_LOCATION))$(2) $(CONFIG_FILE) $$(dir $(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(2)).d
	$(QUIET)$(ECHO) Checking header  $(2)
	$(QUIET)$(CC) -c $(WICED_SDK_CFLAGS) $(filter-out -pedantic -Werror, $($(1)_CFLAGS) $(C_BUILD_OPTIONS) ) $($(1)_INCLUDES) $($(1)_DEFINES) $(WICED_SDK_INCLUDES) $(WICED_SDK_DEFINES) -o $$@ $$<
endef

###############################################################################
# MACRO: BUILD_CPP_RULE
# Creates a target for building C++ language files (*.cpp)
# $(1) is component name, $(2) is the source file
define BUILD_CPP_RULE
-include $(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(patsubst %.cc,%.d,$(2:.cpp=.d))
$(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(patsubst %.cc,%.o,$(2:.cpp=.o)): $(strip $($(1)_LOCATION))$(2) $(CONFIG_FILE) $$(dir $(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(2)).d $(RESOURCES_DEPENDENCY) $(LIBS_DIR)/$(1).cpp_opts | $(EXTRA_PRE_BUILD_TARGETS)
	$$(if $($(1)_START_PRINT),,$(eval $(1)_START_PRINT:=1) $(ECHO) Compiling $(1))
	$(QUIET)$(CXX) $(OPTIONS_IN_FILE_OPTION)$(LIBS_DIR)/$(1).cpp_opts -o $$@ $$<  $(COMPILER_SPECIFIC_STDOUT_REDIRECT)
endef

###############################################################################
# MACRO: BUILD_S_RULE
# Creates a target for building Assembly language files (*.s & *.S)
# $(1) is component name, $(2) is the source file
define BUILD_S_RULE
-include $(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(patsubst %.S,%.d,$(2:.s=.d))
$(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(strip $(patsubst %.S,%.o, $(2:.s=.o))): $(strip $($(1)_LOCATION))$(2) $($(1)_PRE_BUILD_TARGETS) $(CONFIG_FILE) $$(dir $(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(2)).d $(RESOURCES_DEPENDENCY) $(LIBS_DIR)/$(1).as_opts | $(EXTRA_PRE_BUILD_TARGETS)
	$$(if $($(1)_START_PRINT),,$(eval $(1)_START_PRINT:=1) $(ECHO) Compiling $(1))
ifeq ($(TOOLCHAIN_NAME),IAR)
	$(QUIET)$(AS) $(OPTIONS_IN_FILE_OPTION) $(LIBS_DIR)/$(1).as_opts -o $$@ $$< $(COMPILER_SPECIFIC_STDOUT_REDIRECT)
else
	$(QUIET)$(CC) $(OPTIONS_IN_FILE_OPTION)$(LIBS_DIR)/$(1).c_opts -o $$@ $$< $(COMPILER_SPECIFIC_STDOUT_REDIRECT)
endif
endef

###############################################################################
# MACRO: BUILD_COMPONENT_RULES
# Creates targets for building an entire component
# Target for the component static library is created in this macro
# Targets for source files are created by calling the macros defined above
# $(1) is component name
define BUILD_COMPONENT_RULES

$(eval LINK_LIBS +=$(if $($(1)_SOURCES),$(LIBS_DIR)/$(1).a))


ifneq ($($(1)_PRE_BUILD_TARGETS),)
include $($(1)_MAKEFILE)
endif

$(1)_ASMFLAGS += $(if $(findstring debug,$($(1)_BUILD_TYPE)), $(COMPILER_SPECIFIC_DEBUG_ASFLAGS),  $(COMPILER_SPECIFIC_RELEASE_ASFLAGS))
$(1)_LDFLAGS  += $(if $(findstring debug,$($(1)_BUILD_TYPE)), $(COMPILER_SPECIFIC_DEBUG_LDFLAGS),  $(COMPILER_SPECIFIC_RELEASE_LDFLAGS))

# Make a list of the object files that will be used to build the static library
$(eval $(1)_LIB_OBJS := $(addprefix $(strip $(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))),  $(filter %.o, $($(1)_SOURCES:.cc=.o) $($(1)_SOURCES:.cpp=.o) $($(1)_SOURCES:.c=.o) $($(1)_SOURCES:.s=.o) $($(1)_SOURCES:.S=.o)))  $(patsubst %.c,%.o,$(call RESOURCE_FILENAME, $($(1)_RESOURCES))))

$(LIBS_DIR)/$(1).c_opts: $($(1)_PRE_BUILD_TARGETS) $(CONFIG_FILE) | $(LIBS_DIR)
	$(QUIET)$$(call WRITE_FILE_CREATE, $$@, $(subst $(COMMA),$$(COMMA), $(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(COMPILER_SPECIFIC_DEPS_FLAG) $(COMPILER_SPECIFIC_STANDARD_CFLAGS) $(if $(findstring debug,$($(1)_BUILD_TYPE)), $(COMPILER_SPECIFIC_DEBUG_CFLAGS), $(COMPILER_SPECIFIC_RELEASE_CFLAGS)) $($(1)_CFLAGS) $($(1)_INCLUDES) $($(1)_DEFINES) $(WICED_SDK_INCLUDES) $(WICED_SDK_DEFINES)))

$(LIBS_DIR)/$(1).cpp_opts: $($(1)_PRE_BUILD_TARGETS) $(CONFIG_FILE) | $(LIBS_DIR)
	 $(QUIET)$$(call WRITE_FILE_CREATE, $$@ ,$(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(COMPILER_SPECIFIC_DEPS_FLAG) $(COMPILER_SPECIFIC_STANDARD_CXXFLAGS) $(if $(findstring debug,$($(1)_BUILD_TYPE)), $(COMPILER_SPECIFIC_DEBUG_CXXFLAGS), $(COMPILER_SPECIFIC_RELEASE_CXXFLAGS)) $($(1)_CXXFLAGS)  $($(1)_INCLUDES) $($(1)_DEFINES) $(WICED_SDK_INCLUDES) $(WICED_SDK_DEFINES))

$(LIBS_DIR)/$(1).as_opts: $(CONFIG_FILE) | $(LIBS_DIR)
	$(QUIET)$$(call WRITE_FILE_CREATE, $$@ ,$(if $(findstring debug, $($(1)_BUILD_TYPE)), $(COMPILER_SPECIFIC_DEBUG_ASFLAGS), $(COMPILER_SPECIFIC_RELEASE_ASFLAGS)) $($(1)_ASMFLAGS) $($(1)_INCLUDES) $(WICED_SDK_INCLUDES) $(sort $($(1)_DEFINES)))

$(LIBS_DIR)/$(1).ar_opts: $(CONFIG_FILE) | $(LIBS_DIR)
	$(QUIET)$$(call WRITE_FILE_CREATE, $$@ ,$($(1)_LIB_OBJS))

# Allow checking of completeness of headers
$(foreach src, $(if $(findstring 1,$(CHECK_HEADERS)), $(filter %.h, $($(1)_CHECK_HEADERS)), ),$(eval $(call CHECK_HEADER_RULE,$(1),$(src))))

# Target for build-from-source
#$(OUTPUT_DIR)/libraries/$(1).a: $$($(1)_LIB_OBJS) $($(1)_CHECK_HEADER_LIST) $(OUTPUT_DIR)/libraries/$(1).ar_opts $$(if $(WICED_BUILT_WITH_ROM_SYMBOLS),$(ROMOBJCOPY_OPTS_FILE))
$(OUTPUT_DIR)/libraries/$(1).a: $$($(1)_LIB_OBJS) $($(1)_CHECK_HEADER_LIST) $(OUTPUT_DIR)/libraries/$(1).ar_opts
	$(ECHO) Making $$@
	$(QUIET)$(AR) $(WICED_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_CREATE) $$@ $(OPTIONS_IN_FILE_OPTION)$(OUTPUT_DIR)/libraries/$(1).ar_opts

# Create targets to built the component's source files into object files
$(foreach src, $(filter %.c, $($(1)_SOURCES)),$(eval $(call BUILD_C_RULE,$(1),$(src))))
$(foreach src, $(filter %.cpp, $($(1)_SOURCES)) $(filter %.cc, $($(1)_SOURCES)),$(eval $(call BUILD_CPP_RULE,$(1),$(src))))
$(foreach src, $(filter %.s %.S, $($(1)_SOURCES)),$(eval $(call BUILD_S_RULE,$(1),$(src))))


$(eval $(1)_LINT_FLAGS +=  $(filter -D% -I%, $($(1)_CFLAGS) $($(1)_INCLUDES) $($(1)_DEFINES) $(WICED_SDK_INCLUDES) $(WICED_SDK_DEFINES) ) )
$(eval LINT_FLAGS +=  $($(1)_LINT_FLAGS) )
$(eval LINT_FILES +=  $(addprefix $(strip $($(1)_LOCATION)), $(filter %.c, $($(1)_SOURCES))) )
endef

##################################
# Processing
##################################

# Create targets for resource files
$(eval $(if $(ALL_RESOURCES),$(call CREATE_ALL_RESOURCE_TARGETS,$(ALL_RESOURCES))))
LINK_LIBS += $(RESOURCES_LIBRARY)

# Create targets for components
$(foreach comp,$(COMPONENTS),$(eval $(call BUILD_COMPONENT_RULES,$(comp))))

# Add pre-built libraries
LINK_LIBS += $(WICED_SDK_PREBUILT_LIBRARIES)

##################################
# Build rules
##################################


$(LIBS_DIR):
	$(QUIET)$(call MKDIR, $@)

# Directory dependency - causes mkdir to be called once for each directory.
%/.d:
	$(QUIET)$(call MKDIR, $(dir $@))
	$(QUIET)$(TOUCH) $(@)

# Bin file target - uses objcopy to convert the stripped elf into a binary file
$(FINAL_OUTPUT_FILE): $(STRIPPED_LINK_OUTPUT_FILE)
	$(QUIET)$(ECHO) Making $(notdir $@)
	$(QUIET)$(OBJCOPY) $(call FINAL_OUTPUT_OPTIONS,$<, $@)

# Stripped elf file target - Strips the full elf file and outputs to a new .stripped.elf file
$(STRIPPED_LINK_OUTPUT_FILE): $(LINK_OUTPUT_FILE)
	$(QUIET)$(STRIP) $(call STRIP_OPTIONS,$<,$@)
# Remove XIP code from elf to minimize size
ifeq (1, $(XIP_SUPPORT))
	$(QUIET)$(OBJCOPY) $(call REMOVE_XIP_OPTIONS,$@, $@)
endif #ifeq (1, $(XIP_SUPPORT))

$(XIP_OUTPUT_FILE): $(LINK_OUTPUT_FILE)
	$(QUIET)$(ECHO) Making $(notdir $@)
	$(QUIET)$(OBJCOPY) $(call XIP_OUTPUT_OPTIONS,$<, $@)

$(LINK_OPTS_FILE): build/$(CLEANED_BUILD_STRING)/config.mk
#$(COMPILER_SPECIFIC_LINK_MAP) $(MAP_OUTPUT_FILE) $(LINK_OPTS_FILE)
	$(QUIET)$(call WRITE_FILE_CREATE, $@, $(CUSTOM_LD_FLAGS) $(WICED_SDK_LINK_SCRIPT_CMD) $(call COMPILER_SPECIFIC_LINK_MAP,$(MAP_OUTPUT_FILE))  $(call COMPILER_SPECIFIC_LINK_FILES, $(WICED_SDK_LINK_FILES) $(filter %.a,$^) $(LINK_LIBS)) $(WICED_SDK_LDFLAGS) )

$(LINT_OPTS_FILE): $(LINK_LIBS)
	$(QUIET)$(call WRITE_FILE_CREATE, $@ , )
	$(QUIET)$(foreach opt,$(sort $(subst \",",$(LINT_FLAGS))) $(sort $(LINT_FILES)),$(call WRITE_FILE_APPEND, $@ ,$(opt)))

runlint: $(LINT_OPTS_FILE)
	$(LINT_EXE) -f $(TOOLS_ROOT)/splint/opts.txt -f $(LINT_OPTS_FILE)

ifneq ($(origin LINT),default)
ifeq ($(LINT),1)
LINT_DEPENDENCY := runlint
endif
endif

#Create linker script with configurable XIP load address and XIP size
ifeq (1, $(XIP_SUPPORT))
include $(SOURCE_ROOT)platforms/$(PLATFORM)/platform_xip.mk

ifeq (,$(XIP_LINK_START_ADDRESS))
$(error NOT define XIP_LINK_START_ADDRESS for XIP)
endif

ifeq (,$(XIP_REGION_LENGTH))
$(error NOT define XIP_REGION_LENGTH for XIP)
endif

#Check if pre linker script exist or not
XIP_PRE_LINKER_SCRIPT := $(WICED_SDK_LINK_SCRIPT:$(LINK_SCRIPT_SUFFIX)=.preld)

ifeq ($(wildcard $(XIP_PRE_LINKER_SCRIPT)),)
$(info  $(XIP_PRE_LINKER_SCRIPT) does not exist! Expect it to generate dynamically)
endif

$(eval LINK_LOC_FLAGS = LINKER_SFLASH_XIP_ADDR=$(XIP_LINK_START_ADDRESS) LINKER_SFLASH_XIP_LENGTH=$(XIP_REGION_LENGTH))

$(WICED_SDK_LINK_SCRIPT): $(XIP_PRE_LINKER_SCRIPT) $(XIP_MAKEFILE)
	$(QUIET)$(RM) -rf $(WICED_SDK_LINK_SCRIPT)
	$(QUIET)$(ECHO) Making $(notdir $@)...
	$(QUIET)$(CC) -E -x c -P -C $(addprefix -D,$(LINK_LOC_FLAGS)) $(XIP_PRE_LINKER_SCRIPT) -o $@
endif
#endif $(XIP_SUPPORT) == 1

# Linker output target - This links all component & resource libraries and objects into an output executable
# CXX is used for compatibility with C++
$(LINK_OUTPUT_FILE): $(LINK_LIBS) $(WICED_SDK_LINK_SCRIPT) $(LINK_OPTS_FILE) $(LINT_DEPENDENCY) | $(EXTRA_PRE_LINK_TARGETS)
	$(QUIET)$(ECHO) Making $(notdir $@)
	$(QUIET)$(LINKER) $(OPTIONS_IN_FILE_OPTION)$(LINK_OPTS_FILE) -o  $@ $(COMPILER_SPECIFIC_STDOUT_REDIRECT)
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(call COMPILER_SPECIFIC_MAPFILE_TO_CSV,$(MAP_OUTPUT_FILE),$(MAP_CSV_OUTPUT_FILE))

$(WICED_SDK_CONVERTER_OUTPUT_FILE): $(LINK_OUTPUT_FILE)
	$(QUIET)$(ECHO) Making $(notdir $@)
	$(QUIET)$(CONVERTER) "--ihex" "--verbose" $(LINK_OUTPUT_FILE) $@

$(WICED_SDK_FINAL_OUTPUT_FILE): $(WICED_SDK_CONVERTER_OUTPUT_FILE)
	$(QUIET)$(ECHO) Making $(PYTHON_FULL_NAME) $(WICED_SDK_CHIP_SPECIFIC_SCRIPT) -i $(WICED_SDK_CONVERTER_OUTPUT_FILE) -o $(WICED_SDK_FINAL_OUTPUT_FILE)
	$(QUIET)$(PYTHON_FULL_NAME) $(WICED_SDK_CHIP_SPECIFIC_SCRIPT) -i $(WICED_SDK_CONVERTER_OUTPUT_FILE) -o $(WICED_SDK_FINAL_OUTPUT_FILE)

display_map_summary: $(LINK_OUTPUT_FILE) $(WICED_SDK_CONVERTER_OUTPUT_FILE) $(WICED_SDK_FINAL_OUTPUT_FILE)
	$(QUIET) $(call COMPILER_SPECIFIC_MAPFILE_DISPLAY_SUMMARY,$(MAP_OUTPUT_FILE))


# Device Config Table (DCT) binary file target - compiles, links, strips, and objdumps DCT source into a binary output file
$(LINK_DCT_FILE): $(OUTPUT_DIR)/generated_security_dct.h $(WICED_SDK_DCT_LINK_SCRIPT) $(DCT_FILE) $(WICED_SDK_APPLICATION_DCT) $(WICED_SDK_WIFI_CONFIG_DCT_H) $(WICED_SDK_BT_CONFIG_DCT_H) $(CONFIG_FILE) | $(EXTRA_PRE_BUILD_TARGETS)
	$(QUIET)$(ECHO) Making DCT image
	$(QUIET)$(CC) $(CPU_CFLAGS) $(COMPILER_SPECIFIC_COMP_ONLY_FLAG)  $(DCT_FILE) $(WICED_SDK_DEFINES) $(WICED_SDK_INCLUDES) $(COMPILER_SPECIFIC_DEBUG_CFLAGS)  $(COMPILER_SPECIFIC_STANDARD_CFLAGS) -I$(OUTPUT_DIR) -I$(SOURCE_ROOT). -o $(OUTPUT_DIR)/internal_dct.o $(COMPILER_SPECIFIC_STDOUT_REDIRECT)
	$(if $(WICED_SDK_APPLICATION_DCT),$(QUIET)$(CC) $(CPU_CFLAGS) $(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(WICED_SDK_APPLICATION_DCT) $(WICED_SDK_DEFINES) $(WICED_SDK_INCLUDES) $(COMPILER_SPECIFIC_DEBUG_CFLAGS)  $(COMPILER_SPECIFIC_STANDARD_CFLAGS) -I$(OUTPUT_DIR) -I$(SOURCE_ROOT). -o $(OUTPUT_DIR)/app_dct.o)
	$(QUIET)$(LINKER) $(WICED_SDK_LDFLAGS) $(COMPILER_SPECIFIC_EXTRA_DCT_LDFLAGS) $(WICED_SDK_DCT_LINK_CMD) $(call COMPILER_SPECIFIC_LINK_MAP,$(MAP_DCT_FILE)) -o $@  $(OUTPUT_DIR)/internal_dct.o $(if $(WICED_SDK_APPLICATION_DCT),$(OUTPUT_DIR)/app_dct.o)  $(COMPILER_SPECIFIC_STDOUT_REDIRECT)

$(STRIPPED_LINK_DCT_FILE): $(LINK_DCT_FILE)
	$(QUIET)$(STRIP) $(call STRIP_OPTIONS,$<,$@)

$(FINAL_DCT_FILE): $(STRIPPED_LINK_DCT_FILE)
	$(QUIET)$(OBJCOPY) $(call FINAL_OUTPUT_OPTIONS,$<, $@)

# Certificates header target - Converts and concatenates text certificate files into a header file
$(OUTPUT_DIR)/generated_security_dct.h: $(WICED_SDK_CERTIFICATE) $(WICED_SDK_PRIVATE_KEY) | $(EXTRA_PRE_BUILD_TARGETS)
	$(QUIET)$(ECHO) Creating security credentials
	$(QUIET)$(PERL) $(TOOLS_ROOT)/text_to_c/certs_to_h.pl CERTIFICATE_STRING $(WICED_SDK_CERTIFICATE) > $@
	$(QUIET)$(PERL) $(TOOLS_ROOT)/text_to_c/certs_to_h.pl PRIVATE_KEY_STRING $(WICED_SDK_PRIVATE_KEY) >> $@

# Main Target - Ensures the required parts get built
build_done: $(EXTRA_PRE_BUILD_TARGETS) $(if $(filter 1,$(XIP_SUPPORT)),$(XIP_OUTPUT_FILE))  $(FINAL_OUTPUT_FILE) $(if $(filter 1,$(NODCT)),,$(FINAL_DCT_FILE)) display_map_summary

$(BUILD_STRING): build_done $(EXTRA_POST_BUILD_TARGETS)

# Stack usage target - Currently not working outputs a CSV file showing function stack usage
$(OUTPUT_DIR)/stack_usage.csv: $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING)$(LINK_OUTPUT_SUFFIX)
	$(QUIET)$(ECHO) Extracting call tree
	$(QUIET)cd $(OUTPUT_DIR); find -name *.optimized | xargs cat > call_tree.txt
	$(QUIET)$(ECHO) Extracting individual stack usage
	$(QUIET)cd $(OUTPUT_DIR); find -name *.su | xargs cat > stack_usage.txt
	$(QUIET)$(ECHO) Processing stack data
	$(QUIET)cd $(OUTPUT_DIR); $(QUIET)$(PERL) $(TOOLS_ROOT)/stack_usage/stack_usage/stack_usage.pl > $@


