################################################################################
# \file utils.mk
#
# \brief
# Global utilities used across the application recipes and BSPs
#
################################################################################
# \copyright
# Copyright 2018-2020 Cypress Semiconductor Corporation
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

ifeq ($(WHICHFILE),true)
$(info Processing $(lastword $(MAKEFILE_LIST)))
endif


################################################################################
# Components
################################################################################

#
# VFP-specific component
#
ifeq ($(VFP_SELECT),hardfp)
CY_COMPONENT_VFP:=HARDFP
else
CY_COMPONENT_VFP:=SOFTFP
endif

#
# Component list
#
CY_COMPONENT_LIST_DEFAULT=$(CORE) $(CY_COMPONENT_VFP) $(COMPONENTS)
# Note: CY_DEFAULT_COMPONENT is needed as DISABLE_COMPONENTS cannot be empty
DISABLE_COMPONENTS+=CY_DEFAULT_COMPONENT
CY_COMPONENT_LIST?=$(sort $(filter-out $(DISABLE_COMPONENTS),$(CY_COMPONENT_LIST_DEFAULT)))


################################################################################
# Utility variables
################################################################################

# Create a make variable that contains a space
CY_SPACE:= 
CY_SPACE+=

# Create a make variable that contains a soft tab
CY_INDENT:=$(CY_SPACE)$(CY_SPACE)$(CY_SPACE)$(CY_SPACE)

# Create a make variable that contains a line break
define CY_NEWLINE


endef

# Create a make variable that contains a comma
CY_COMMA:=,

# Displays/Hides the build steps
ifneq (,$(filter $(VERBOSE),true 1))
CY_NOISE:=
CY_CMD_TERM:=
else
CY_NOISE:=@
CY_CMD_TERM:= > /dev/null 2>&1
endif


################################################################################
# Environment check
################################################################################

# Set the location of the find utility (Avoid conflict with Windows system32/find.exe)
ifeq ($(findstring /usr/bin/find,$(shell whereis find)),)
CY_FIND:=find
else
CY_FIND:=/usr/bin/find
endif

################################################################################
# Macros
################################################################################

#
# Prints for bypassing TARGET/DEVICE checks
# $(1) : String to print
#
ifeq ($(CY_COMMENCE_BUILD),true)
CY_MACRO_ERROR=$(error $(1))
else
CY_MACRO_ERROR=$(info WARNING: $(1))
endif

# 
# Macros to find all COMPONENTS not listed in the component list.
# Step 1: Find all COMPONENT directories in app
# Step 2: Process the list and get a list of all COMPONENTS
# Step 3: Compare the found COMPONENTS with the expected components list
#
# $(1): List of files of a certain file type
#
CY_MACRO_FIND_COMPONENTS=$(strip $(foreach item,$(1),$(if $(findstring /COMPONENT_,/$(item)),$(item),)))
CY_MACRO_PROCESS_COMPONENTS=\
$(foreach item,$(1),\
    $(if $(findstring /COMPONENT_,/$(notdir $(item))),$(subst COMPONENT_,,$(notdir $(item))),)\
    $(if $(call CY_MACRO_EQUALITY,$(CY_INTERNAL_APP_PATH),$(item)),,$(call CY_MACRO_PROCESS_COMPONENTS,$(call CY_MACRO_DIR,$(item))))\
)
CY_MACRO_COMPARE_COMPONENTS=$(filter-out $(CY_COMPONENT_LIST),$(sort $(call CY_MACRO_PROCESS_COMPONENTS,$(call CY_MACRO_FIND_COMPONENTS,$(1)))))

#
# Filters for components. Iterates through CY_COMPONENT_LIST
# $(1) : List of files of a certain file type
#
CY_MACRO_MATCH_COMPONENT=$(sort $(foreach component,$(2),\
				$(foreach item,$(1),$(if $(findstring /COMPONENT_$(component)/,/$(item)/),$(item),))))
CY_MACRO_GET_COMPONENT=$(filter-out \
                $(call CY_MACRO_MATCH_COMPONENT,$(1),$(call CY_MACRO_COMPARE_COMPONENTS,$(1))),\
                $(call CY_MACRO_MATCH_COMPONENT,$(1),$(CY_COMPONENT_LIST)))
CY_MACRO_REMOVE_COMPONENT=$(strip $(foreach item,$(1),$(if $(findstring /COMPONENT_,/$(item)),,$(item))))
CY_MACRO_FILTER_COMPONENT=$(call CY_MACRO_REMOVE_COMPONENT,$(1)) $(call CY_MACRO_GET_COMPONENT,$(1))

#
# Filters for configurations
# $(1) : List of files of a certain file type
# $(2) : Filter string
#
CY_MACRO_MATCH_CONFIGURATION=$(strip $(foreach item,$(1),$(if $(findstring $(2),/$(item)/),$(item),)))
CY_MACRO_REMOVE_CONFIGURATION=$(strip $(foreach item,$(1),$(if $(findstring $(2),/$(item)),,$(item))))
CY_MACRO_FILTER_CONFIGURATION=$(call CY_MACRO_REMOVE_CONFIGURATION,$(1),/$(strip $(2))_)\
							$(call CY_MACRO_MATCH_CONFIGURATION,$(1),/$(strip $(2))_$($(strip $(2)))/)\
							$(call CY_MACRO_MATCH_CONFIGURATION,$(1),/$(strip $(2))_$(subst -,_,$($(strip $(2))))/)

#
# Filter for defined components and configurations 
# $(1) : List of files of a certain file type
#
CY_MACRO_FILTER=\
	$(strip \
	$(call CY_MACRO_FILTER_COMPONENT,\
	$(call CY_MACRO_FILTER_CONFIGURATION,\
	$(call CY_MACRO_FILTER_CONFIGURATION,\
	$(call CY_MACRO_FILTER_CONFIGURATION,\
	$(1),\
	TOOLCHAIN),\
	TARGET),\
	CONFIG)))

#
# Search for files
# $(1) : File type
#
CY_MACRO_FILTER_FILES=$(call CY_MACRO_FILTER,$(CY_SEARCH_PRUNED_$(1)_FILES))

#
# Test for equality
# $(1) : Base path
# $(2) : Directory containing header file
#
CY_MACRO_EQUALITY=$(if $(and $(findstring $1,$2),$(findstring $2,$1)),TRUE)

#
# Recursively search for the parent directories up to the given base directory
# $(1) : Directories containing header files
# $(2) : Base directory
#
CY_MACRO_SEARCH_PARENT=\
$(foreach item,$(1),\
    $(if $(call CY_MACRO_EQUALITY,$(2),$(item)),\
        $(2)\
    ,\
        $(call CY_MACRO_DIR,$(item))\
        $(call CY_MACRO_SEARCH_PARENT,$(call CY_MACRO_DIR,$(item)),$(2))\
    )\
)

#
# Convert to lower case
# $(1) : String to convert to lower case
#
CY_MACRO_LC=$(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst \
		H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst \
		Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst \
		W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

#
# Convert to upper case
# $(1) : String to convert to upper case
#
CY_MACRO_UC=$(subst a,A,$(subst b,B,$(subst c,C,$(subst d,D,$(subst e,E,$(subst f,F,$(subst g,G,$(subst \
		h,H,$(subst i,I,$(subst j,J,$(subst k,K,$(subst l,L,$(subst m,M,$(subst n,N,$(subst o,O,$(subst p,P,$(subst \
		q,Q,$(subst r,R,$(subst s,S,$(subst t,T,$(subst u,U,$(subst v,V,$(subst \
		w,W,$(subst x,X,$(subst y,Y,$(subst z,Z,$1))))))))))))))))))))))))))


################################################################################
# Utility targets
################################################################################

# Used to determine locations of dirs and files relative to devicesupport.xml
CY_DEVICESUPPORT_SEARCH_PATH:=$(call CY_MACRO_SEARCH,devicesupport.xml,$(CY_INTERNAL_APP_PATH))\
                    $(if $(CY_INTERNAL_EXTAPP_PATH),$(call CY_MACRO_SEARCH,devicesupport.xml,$(CY_INTERNAL_EXTAPP_PATH)))\
                    $(if $(SEARCH_LIBS_AND_INCLUDES),$(foreach d,$(SEARCH_LIBS_AND_INCLUDES),$(call CY_MACRO_SEARCH,devicesupport.xml,$(d))))

bsp:
ifeq ($(TARGET_GEN),)
	$(error TARGET_GEN variable must be specified to generate a BSP)
else
	$(info $(CY_NEWLINE)Creating $(TARGET_GEN) TARGET from $(TARGET)...)
	$(CY_NOISE)if [ -d $(CY_TARGET_GEN_DIR) ]; then\
		echo "ERROR: "$(TARGET_GEN)" TARGET already exists at "$(CY_TARGET_GEN_DIR)"";\
		exit 1;\
	else\
		cp -rf $(CY_TARGET_DIR) $(CY_TARGET_GEN_DIR);\
		rm -rf $(CY_TARGET_GEN_DIR)/.git;\
		sed -e s/$(TARGET)/"$(TARGET_GEN)"/g -e /DEVICE/s%=.*%="$(DEVICE_GEN)"\% -e /ADDITIONAL_DEVICES/s%=.*%="$(ADDITIONAL_DEVICES_GEN)"\% \
			$(CY_TARGET_GEN_DIR)/$(TARGET).mk > $(CY_TARGET_GEN_DIR)/$(TARGET_GEN).mk;\
		rm -rf $(CY_TARGET_GEN_DIR)/$(TARGET).mk;\
		$(CY_BSP_TEMPLATES_CMD)\
		$(CY_BSP_DEVICES_CMD)\
		echo ""$(TARGET_GEN)" TARGET created at "$(CY_TARGET_GEN_DIR)"";\
	fi
endif

ifneq ($(SEARCH_LIBS_AND_INCLUDES),)
CY_SHARED_ALL_LIB_FILES=$(call CY_MACRO_SEARCH,.lib,$(CY_SHARED_PATH))
CY_SHARED_USED_LIB_NAMES=$(foreach item,$(SEARCH_LIBS_AND_INCLUDES),$(notdir $(item).lib))
CY_SHARED_USED_LIB_FILES=$(foreach name,$(CY_SHARED_USED_LIB_NAMES),$(filter %/$(name),$(CY_SHARED_ALL_LIB_FILES)))
endif


get_app_info:
ifeq ($(CY_PROTOCOL),2)
	$(CY_NOISE)echo;\
	echo "=======================================";\
	echo " IDE variables";\
	echo "=======================================";\
	echo "CY_PROTOCOL=$(CY_PROTOCOL)";\
	echo "APP_NAME=$(APPNAME)";\
	echo "LIB_NAME=$(LIBNAME)";\
	echo "TARGET=$(TARGET)";\
	echo "TARGET_DEVICE=$(DEVICE)";\
	echo "CONFIGURATOR_FILES=$(CY_CONFIG_FILES)";\
	echo "SUPPORTED_TOOL_TYPES=$(CY_OPEN_FILTERED_SUPPORTED_TYPES)";\
	echo "CY_TOOLS_PATH=$(CY_TOOLS_DIR)";\
	echo "CY_GETLIBS_OFFLINE=$(CY_GETLIBS_OFFLINE)";\
	echo "CY_GETLIBS_PATH=$(CY_INTERNAL_GETLIBS_PATH)";\
	echo "CY_GETLIBS_DEPS_PATH=$(CY_INTERNAL_GETLIBS_DEPS_PATH)";\
	echo "CY_GETLIBS_CACHE_PATH=$(CY_INTERNAL_GETLIBS_CACHE_PATH)";\
	echo "CY_GETLIBS_OFFLINE_PATH=$(CY_INTERNAL_GETLIBS_OFFLINE_PATH)";\
	echo "SHAREDLIBS_ROOT=$(CY_SHARED_PATH)";\
	echo "SHAREDLIBS=$(SEARCH_LIBS_AND_INCLUDES)";\
	echo "SHAREDLIBS_FILES=$(CY_SHARED_USED_LIB_FILES)";\
	echo "CY_DEPENDENT_PROJECTS=$(CY_DEPENDENT_PROJECTS)";
else
# Default scenario. Covers CY_PROTOCOL=1 and CY_PROTOCOL=""
	$(CY_NOISE)echo;\
	echo "=======================================";\
	echo " IDE variables";\
	echo "=======================================";\
	echo "APP_NAME=$(APPNAME)";\
	echo "LIB_NAME=$(LIBNAME)";\
	echo "TARGET=$(TARGET)";\
	echo "TARGET_DEVICE=$(DEVICE)";\
	echo "CONFIGURATOR_FILES=$(CY_CONFIG_FILES)";\
	echo "SUPPORTED_TOOL_TYPES=$(CY_OPEN_FILTERED_SUPPORTED_TYPES)";\
	echo "CY_TOOLS_PATH=$(CY_TOOLS_DIR)";\
	echo "CY_GETLIBS_PATH=$(CY_INTERNAL_GETLIBS_PATH)";\
	echo "SHAREDLIBS_ROOT=$(CY_SHARED_PATH)";\
	echo "SHAREDLIBS=$(SEARCH_LIBS_AND_INCLUDES)";\
	echo "SHAREDLIBS_FILES=$(CY_SHARED_USED_LIB_FILES)";\
	echo "CY_DEPENDENT_PROJECTS=$(CY_DEPENDENT_PROJECTS)";
endif
# General debug info
	$(CY_NOISE)echo;\
	echo "=======================================";\
	echo " User: Basic configuration variables";\
	echo "=======================================";\
	echo "TARGET=$(TARGET)";\
	echo "APPNAME=$(APPNAME)";\
	echo "LIBNAME=$(LIBNAME)";\
	echo "TOOLCHAIN=$(TOOLCHAIN)";\
	echo "CONFIG=$(CONFIG)";\
	echo "VERBOSE=$(VERBOSE)";\
	echo;\
	echo "=======================================";\
	echo " User: Advanced configuration variables";\
	echo "=======================================";\
	echo "SOURCES=$(SOURCES)";\
	echo "INCLUDES=$(INCLUDES)";\
	echo "DEFINES=$(DEFINES)";\
	echo "VFP_SELECT=$(VFP_SELECT)";\
	echo "CFLAGS=$(CXXFLAGS)";\
	echo "ASFLAGS=$(ASFLAGS)";\
	echo "LDFLAGS=$(LDFLAGS)";\
	echo "LDLIBS=$(LDLIBS)";\
	echo "LINKER_SCRIPT=$(LINKER_SCRIPT)";\
	echo "PREBUILD=$(PREBUILD)";\
	echo "POSTBUILD=$(POSTBUILD)";\
	echo "COMPONENTS=$(COMPONENTS)";\
	echo "DISABLE_COMPONENTS=$(DISABLE_COMPONENTS)";\
	echo "DEPENDENT_LIB_PATHS=$(DEPENDENT_LIB_PATHS)";\
	echo "DEPENDENT_APP_PATHS=$(DEPENDENT_APP_PATHS)";\
	echo;\
	echo "=======================================";\
	echo " User: BSP variables";\
	echo "=======================================";\
	echo "DEVICE=$(DEVICE)";\
	echo "ADDITIONAL_DEVICES=$(ADDITIONAL_DEVICES)";\
	echo "TARGET_GEN=$(TARGET_GEN)";\
	echo "DEVICE_GEN=$(DEVICE_GEN)";\
	echo "ADDITIONAL_DEVICES_GEN=$(ADDITIONAL_DEVICES_GEN)";\
	echo;\
	echo "=======================================";\
	echo " User: Getlibs variables";\
	echo "=======================================";\
	echo "CY_GETLIBS_NO_CACHE=$(CY_GETLIBS_NO_CACHE)";\
	echo "CY_GETLIBS_OFFLINE=$(CY_GETLIBS_OFFLINE)";\
	echo "CY_GETLIBS_PATH=$(CY_INTERNAL_GETLIBS_PATH)";\
	echo "CY_GETLIBS_DEPS_PATH=$(CY_INTERNAL_GETLIBS_DEPS_PATH)";\
	echo "CY_GETLIBS_CACHE_PATH=$(CY_INTERNAL_GETLIBS_CACHE_PATH)";\
	echo "CY_GETLIBS_OFFLINE_PATH=$(CY_INTERNAL_GETLIBS_OFFLINE_PATH)";\
	echo "CY_GETLIBS_SEARCH_PATH=$(CY_INTERNAL_GETLIBS_SEARCH_PATH)";\
	echo;\
	echo "=======================================";\
	echo " User: Path variables";\
	echo "=======================================";\
	echo "CY_APP_PATH=$(CY_APP_PATH)";\
	echo "CY_BASELIB_PATH=$(CY_BASELIB_PATH)";\
	echo "CY_EXTAPP_PATH=$(CY_EXTAPP_PATH)";\
	echo "CY_DEVICESUPPORT_PATH=$(CY_DEVICESUPPORT_PATH)";\
	echo "CY_SHARED_PATH=$(CY_SHARED_PATH)";\
	echo "CY_COMPILER_PATH=$(CY_COMPILER_PATH)";\
	echo "CY_TOOLS_DIR=$(CY_TOOLS_DIR)";\
	echo "CY_BUILD_LOCATION=$(CY_BUILD_LOCATION)";\
	echo "CY_PYTHON_PATH=$(CY_PYTHON_PATH)";\
	echo "TOOLCHAIN_MK_PATH=$(TOOLCHAIN_MK_PATH)";\
	echo;\
	echo "=======================================";\
	echo " User: Miscellaneous variables";\
	echo "=======================================";\
	echo "CY_IGNORE=$(CY_IGNORE_DIRS)";\
	echo "CY_SKIP_RECIPE=$(CY_SKIP_RECIPE)";\
	echo "CY_EXTRA_INCLUDES=$(CY_EXTRA_INCLUDES)";\
	echo "CY_LIBS_SEARCH_DEPTH=$(CY_LIBS_SEARCH_DEPTH)";\
	echo "CY_UTILS_SEARCH_DEPTH=$(CY_UTILS_SEARCH_DEPTH)";\
	echo "CY_IDE_PRJNAME=$(CY_IDE_PRJNAME)";\
	echo "CY_CONFIG_FILE_EXT=$(CY_CONFIG_FILE_EXT)";\
	echo "CY_SUPPORTED_TOOL_TYPES=$(CY_SUPPORTED_TOOL_TYPES)";\
	echo;\
	echo "=======================================";\
	echo " Internal variables";\
	echo "=======================================";\
	echo "CY_INTERNAL_BUILD_LOC=$(CY_INTERNAL_BUILD_LOC)";\
	echo "CY_INTERNAL_APPLOC=$(CY_INTERNAL_APPLOC)";\
	echo "CY_DEVICESUPPORT_SEARCH_PATH=$(strip $(CY_DEVICESUPPORT_SEARCH_PATH))";\
	echo "CC="$(CC)"";\
	echo "CXX="$(CXX)"";\
	echo "AS="$(AS)"";\
	echo "AR="$(AR)"";\
	echo "LD="$(LD)""


################################################################################
# Test/debug targets
################################################################################

CY_TOOLS_LIST+=bash git find ls cp mkdir rm cat sed awk perl file whereis

check:
	$(info )
	$(foreach tool,$(CY_TOOLS_LIST),$(if $(shell which $(tool)),,$(error "$(tool) was not found in user's PATH")))
	$(CY_NOISE)if [ ! -d $(CY_BT_CONFIGURATOR_DIR) ]; then toolsTest+=("bt-configurator could not be found"); fi;\
	if [ ! -d $(CY_CAPSENSE_CONFIGURATOR_DIR) ]; then toolsTest+=("capsense-configurator could not be found"); fi;\
	if [ ! -d $(CY_CFG_BACKEND_CLI_DIR) ]; then toolsTest+=("cfg-backend-cli could not be found"); fi;\
	if [ ! -d $(CY_MCUELFTOOL_DIR) ]; then toolsTest+=("cymcuelftool could not be found"); fi;\
	if [ ! -d $(CY_PE_TOOL_DIR) ]; then toolsTest+=("cype-tool could not be found"); fi;\
	if [ ! -d $(CY_DEVICE_CONFIGURATOR_DIR) ]; then toolsTest+=("device-configurator could not be found"); fi;\
	if [ ! -d $(CY_DFUH_TOOL_DIR) ]; then toolsTest+=("dfuh-tool could not be found"); fi;\
	if [ ! -d $(CY_FW_LOADER_DIR) ]; then toolsTest+=("fw-loader could not be found"); fi;\
	if [ ! -d $(CY_COMPILER_DIR) ]; then toolsTest+=("Default ARM GCC toolchain could not be found"); fi;\
	if [ ! -d $(CY_JRE_DIR) ]; then toolsTest+=("Java Runtime Environment tool could not be found"); fi;\
	if [ ! -d $(CY_LIBRARY_MANAGER_DIR) ]; then toolsTest+=("Library Manager could not be found"); fi;\
	if [ ! -d $(CY_MODUS_SHELL_DIR) ]; then toolsTest+=("modus-shell could not be found"); fi;\
	if [ ! -d $(CY_OPENOCD_DIR) ]; then toolsTest+=("openocd could not be found"); fi;\
	if [ ! -d $(CY_PROJECT_CREATOR_DIR) ]; then toolsTest+=("Project Creator could not be found"); fi;\
	if [ ! -d $(CY_QSPI_CONFIGURATOR_DIR) ]; then toolsTest+=("qspi-configurator could not be found"); fi;\
	if [ ! -d $(CY_SEGLCD_CONFIGURATOR_DIR) ]; then toolsTest+=("seglcd-configurator could not be found"); fi;\
	if [ ! -d $(CY_SMARTIO_CONFIGURATOR_DIR) ]; then toolsTest+=("smartio-configurator could not be found"); fi;\
	if [ ! -d $(CY_MAKEFILES_DIR) ]; then toolsTest+=("Tools make files could not be found"); fi;\
	if [ $${#toolsTest[@]} -eq 0 ]; then\
		printf "SUCCESS: All tools are present";\
	else\
		printf "FAILED: The following tools are missing\n";\
		printf '  %s\n' "$${toolsTest[@]}";\
	fi;

get_env_info:
	$(CY_NOISE)echo;\
	echo "make location :" $$(which make);\
	echo "make version  :" $(MAKE_VERSION);\
	echo "git location  :" $$(which git);\
	echo "git version   :" $$(git --version);\
	echo "git remote    :";\
	git remote -v;\
	echo "git rev-parse :" $$(git rev-parse HEAD)

printlibs:

# Defined in recipe's program.mk
progtool:

# Empty libs on purpose. May be defined by the application
shared_libs:

#
# Identify the phony targets
#
.PHONY: bsp check get_env_info printlibs shared_libs
