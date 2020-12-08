################################################################################
# \file ide.mk
#
# \brief
# IDE-specific targets and variables
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


#
# Print information before file generation
#
CY_IDE_preprint:
	$(info )
	$(info ==============================================================================)
	$(info = Generating IDE files =)
	$(info ==============================================================================)


################################################################################
# ModusToolbox
################################################################################

ifneq ($(SEARCH_LIBS_AND_INCLUDES),)
CY_SHARED_ALL_LIB_FILES=$(call CY_MACRO_SEARCH,.lib,$(CY_SHARED_PATH))
CY_SHARED_USED_LIB_NAMES=$(foreach item,$(SEARCH_LIBS_AND_INCLUDES),$(notdir $(item).lib))
CY_SHARED_USED_LIB_FILES=$(foreach name,$(CY_SHARED_USED_LIB_NAMES),$(filter %/$(name),$(CY_SHARED_ALL_LIB_FILES)))
endif

CY_HELP_get_app_info=Prints the app info for the eclipse IDE.
get_app_info:
	@echo;\
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
	echo "CY_DEPENDENT_PROJECTS=$(CY_DEPENDENT_PROJECTS)"


################################################################################
# Eclipse
################################################################################

CY_ECLIPSE_OUT_PATH=$(CY_INTERNAL_APP_PATH)/.mtbLaunchConfigs
CY_ECLIPSE_TEMPLATE_PATH=$(CY_INTERNAL_BASELIB_PATH)/make/scripts/eclipse
CY_ECLIPSE_TEMPFILE=$(CY_CONFIG_DIR)/eclipse_launch.temp
CY_ECLIPSE_TEMPLATES_WILDCARD?=*

# Default name for the launch configs
ifeq ($(CY_IDE_PRJNAME),)
CY_IDE_PRJNAME=$(APPNAME)
endif

# Source files outside of the project directory
CY_ECLIPSE_SOURCES_INTERNAL=$(filter-out $(CY_INTERNAL_APPLOC)/% $(CY_APP_LOCATION)/%, $(abspath $(SOURCES)))
ifeq ($(OS),Windows_NT)
#prepend an extra '/' on windows because it's a URI.
ifneq ($(CY_WHICH_CYGPATH),)
ifneq ($(CY_ECLIPSE_SOURCES_INTERNAL),)
CY_ECLIPSE_SOURCES=$(patsubst %, /%, $(shell cygpath -m --absolute $(CY_ECLIPSE_SOURCES_INTERNAL)))
endif
else
CY_ECLIPSE_SOURCES=$(patsubst %, /%, $(CY_ECLIPSE_SOURCES_INTERNAL))
endif
else
CY_ECLIPSE_SOURCES=$(CY_ECLIPSE_SOURCES_INTERNAL)
endif

# Include paths outside of the project directory
CY_ECLIPSE_INCLUDES_INTERNAL=$(filter-out $(CY_INTERNAL_APPLOC) $(CY_INTERNAL_APPLOC)/% $(CY_APP_LOCATION) $(CY_APP_LOCATION)/%, $(abspath $(INCLUDES)))
ifeq ($(OS),Windows_NT)
#prepend an extra '/' on windows because it's a URI.
ifneq ($(CY_WHICH_CYGPATH),)
ifneq ($(CY_ECLIPSE_INCLUDES_INTERNAL),)
CY_ECLIPSE_INCLUDES=$(patsubst %, /%, $(shell cygpath -m --absolute $(CY_ECLIPSE_INCLUDES_INTERNAL)))
endif
else
CY_ECLIPSE_INCLUDES=$(patsubst %, /%, $(CY_ECLIPSE_INCLUDES_INTERNAL))
endif
else
CY_ECLIPSE_INCLUDES=$(CY_ECLIPSE_INCLUDES_INTERNAL)
endif

# Eclipse knows to include everything under the CY_INTERNAL_EXTAPP_PATH
CY_ECLIPSE_EXTAPP_INTERNAL=$(filter-out $(CY_INTERNAL_APPLOC) $(CY_INTERNAL_APPLOC)/% $(CY_APP_LOCATION) $(CY_APP_LOCATION)/%, $(abspath $(CY_INTERNAL_EXTAPP_PATH)))
ifeq ($(OS),Windows_NT)
#prepend an extra '/' on windows because it's a URI.
ifneq ($(CY_WHICH_CYGPATH),)
ifneq ($(CY_ECLIPSE_EXTAPP_INTERNAL),)
CY_ECLIPSE_EXTAPP=$(patsubst %, /%, $(shell cygpath -m --absolute $(CY_ECLIPSE_EXTAPP_INTERNAL)))
endif
else
CY_ECLIPSE_EXTAPP=$(patsubst %, /%, $(CY_ECLIPSE_EXTAPP_INTERNAL))
endif
else
CY_ECLIPSE_EXTAPP=$(CY_ECLIPSE_EXTAPP_INTERNAL)
endif

# Create eclipse project external sources and includes elements 
CY_ECLIPSE_PROJECT_SOURCES=$(foreach file,$(CY_ECLIPSE_SOURCES), \
	\t\t<link>\n\t\t\t<name>external_source/$(notdir $(file))</name>\n\t\t\t<type>1</type>\n\t\t\t<locationURI>file://$(file)</locationURI>\n\t\t</link>\n)
CY_ECLIPSE_PROJECT_INCLUDES=$(foreach file,$(CY_ECLIPSE_INCLUDES), \
	\t\t<link>\n\t\t\t<name>external_includes/$(notdir $(file))</name>\n\t\t\t<type>2</type>\n\t\t\t<locationURI>file://$(file)</locationURI>\n\t\t</link>\n)
CY_ECLIPSE_PROJECT_EXTAPP=$(foreach file,$(CY_ECLIPSE_EXTAPP), \
	\t\t<link>\n\t\t\t<name>$(notdir $(file))</name>\n\t\t\t<type>2</type>\n\t\t\t<locationURI>file://$(file)</locationURI>\n\t\t</link>\n)

CY_ECLIPSE_PROJECT_ARGS=\
	"s|&&APP_NAME&&|$(APPNAME)|;"\
	"s|&&LINKED_SOURCES&&|$(CY_ECLIPSE_PROJECT_SOURCES)|;"\
	"s|&&LINKED_INCLUDES&&|$(CY_ECLIPSE_PROJECT_INCLUDES)|;"\
	"s|&&LINKED_EXTAPP&&|$(CY_ECLIPSE_PROJECT_EXTAPP)|;"\

CY_HELP_eclipse=Generates eclipse IDE launch configs (preliminary: eclipse project).
eclipse: CY_IDE_preprint
ifeq ($(LIBNAME),)
# Generate launch configurations
	@mkdir -p $(CY_CONFIG_DIR);\
	mkdir -p $(CY_ECLIPSE_OUT_PATH);\
	echo $(CY_ECLIPSE_ARGS) > $(CY_ECLIPSE_TEMPFILE);\
	for launch in $(CY_ECLIPSE_TEMPLATE_PATH)/$(CY_ECLIPSE_TEMPLATES_WILDCARD); do\
		if [[ $$launch == *"project" ]]; then\
			continue;\
		fi;\
		launchFile="$${launch##*/}";\
		launchFileName="$${launchFile%.*}";\
		sed -f $(CY_ECLIPSE_TEMPFILE) "$$launch" > "$(CY_ECLIPSE_OUT_PATH)/$(CY_IDE_PRJNAME) $$launchFileName.launch";\
		launchConfigs="$$launchConfigs \"$(CY_IDE_PRJNAME) $$launchFileName.launch"\";\
	done;\
	echo;\
	echo Generated Eclipse launch config files: "$$launchConfigs"
# Generate .project and .cproject files
	@if [ -f $(CY_ECLIPSE_TEMPLATE_PATH)/project/.cproject ] && [ -f $(CY_ECLIPSE_TEMPLATE_PATH)/project/.project ]; then\
		if [ ! -f $(CY_INTERNAL_APP_PATH)/.cproject ] && [ ! -f $(CY_INTERNAL_APP_PATH)/.project ]; then\
			cp -rf $(CY_ECLIPSE_TEMPLATE_PATH)/project/.cproject $(CY_INTERNAL_APP_PATH);\
			echo $(CY_ECLIPSE_PROJECT_ARGS) > $(CY_ECLIPSE_TEMPFILE);\
			sed -f $(CY_ECLIPSE_TEMPFILE) $(CY_ECLIPSE_TEMPLATE_PATH)/project/.project > $(CY_INTERNAL_APP_PATH)/.project;\
			rm -f $(CY_ECLIPSE_TEMPFILE);\
			echo;\
			echo Generated Eclipse project files: ".project .cproject";\
			echo;\
			echo WARNING: The eclipse project generation is preliminary...;\
		else\
			echo;\
			echo WARNING: Eclipse project files already exist. Skipping project generation...;\
		fi;\
	fi;
else
	@echo
endif


################################################################################
# IAR
################################################################################

CY_IAR_TEMPFILE=$(CY_CONFIG_DIR)/iardata.temp
CY_IAR_OUTFILE=$(APPNAME).ipcf
CY_IAR_CYIGNORE_PATH=$(CY_INTERNAL_APP_PATH)/.cyignore

# All paths are expected to be relative of the Makefile(Project Directory)
# Get all defines
CY_IAR_DEFINES_LIST=$(subst -D,,$(CY_RECIPE_DEFINES))
CY_IAR_DEFINES=$(foreach onedef,$(CY_IAR_DEFINES_LIST),\"$(onedef)\",)

# Get all include paths 
CY_IAR_INCLUDES_LIST=$(subst -I,,$(CY_RECIPE_INCLUDES))
CY_IAR_INCLUDES=$(foreach onedef,$(CY_IAR_INCLUDES_LIST),\"$(onedef)\",)

# Get all .c/.cpp source
CY_IAR_ALL_C_CPP_SRC=$(filter %.$(CY_TOOLCHAIN_SUFFIX_C) %.$(CY_TOOLCHAIN_SUFFIX_CPP),$(CY_RECIPE_SOURCE) $(CY_RECIPE_GENERATED) $(SOURCES))
CY_IAR_ALL_C_CPP_QUOTED=$(foreach onedef,$(CY_IAR_ALL_C_CPP_SRC),\"$(onedef)\",)

# Get all .s/.S source
CY_IAR_ALL_S_SRC=$(filter %.$(CY_TOOLCHAIN_SUFFIX_S) %.$(CY_TOOLCHAIN_SUFFIX_s),$(CY_RECIPE_SOURCE) $(CY_RECIPE_GENERATED) $(SOURCES))
CY_IAR_ALL_S_QUOTED=$(foreach onedef,$(CY_IAR_ALL_S_SRC),\"$(onedef)\",)

# Get list of all header files.
CY_IAR_ALL_H_FILES=$(sort $(call CY_MACRO_FILTER_FILES,H) $(call CY_MACRO_FILTER_FILES,HPP))
CY_IAR_ALL_H_QUOTED=$(foreach onedef,$(CY_IAR_ALL_H_FILES),\"$(onedef)\",)

CY_IAR_ALL_LIB_FILES=$(CY_RECIPE_LIBS)
CY_IAR_ALL_LIBS_QUOTED=$(foreach onedef,$(CY_IAR_ALL_LIB_FILES),\"$(onedef)\",)

CY_HELP_ewarm8=Generates IAR-EW v8 IDE .ipcf file (preliminary).
ewarm8: CY_IDE_preprint
ifneq ($(TOOLCHAIN), IAR)
	$(error Unable to proceed. The TOOLCHAIN variable must be set to IAR. i.e. TOOLCHAIN=IAR)
endif
	@mkdir -p $(CY_CONFIG_DIR);\
	echo $(APPNAME) > $(CY_IAR_TEMPFILE);\
	echo $(DEVICE) >> $(CY_IAR_TEMPFILE);\
	echo $(CORE) >> $(CY_IAR_TEMPFILE);\
	echo $(LINKER_SCRIPT) >> $(CY_IAR_TEMPFILE);\
	echo $(CY_IAR_DEFINES) >> $(CY_IAR_TEMPFILE);\
	echo $(CY_IAR_INCLUDES) >> $(CY_IAR_TEMPFILE);\
	echo $(CY_IAR_ALL_C_CPP_QUOTED) >> $(CY_IAR_TEMPFILE);\
	echo $(CY_IAR_ALL_S_QUOTED) >> $(CY_IAR_TEMPFILE);\
	echo $(CY_IAR_ALL_H_QUOTED) >> $(CY_IAR_TEMPFILE);\
	echo $(CY_IAR_ALL_LIBS_QUOTED) >> $(CY_IAR_TEMPFILE);\
	echo
	$(CY_NOISE)$(CY_PYTHON_PATH) $(CY_INTERNAL_BASELIB_PATH)/make/scripts/iar/iar_export.py -i $(CY_IAR_TEMPFILE) -o $(CY_IAR_OUTFILE);
	@rm -rf $(CY_IAR_TEMPFILE);\
	if  [ ! -f $(CY_IAR_CYIGNORE_PATH) ] || ! grep -q 'Debug' $(CY_IAR_CYIGNORE_PATH) || ! grep -q 'Release' $(CY_IAR_CYIGNORE_PATH);\
	then \
		echo;\
		echo Note: Added default IAR-EW output folders \"Debug\" and \"Release\" to $(CY_IAR_CYIGNORE_PATH) file. \
		For custom IAR output directories, manually add them to the $(CY_IAR_CYIGNORE_PATH) file to exclude them from auto-discovery.; \
		echo >> $(CY_IAR_CYIGNORE_PATH);\
		echo "# Automatically added by ewarm8 make target" >> $(CY_IAR_CYIGNORE_PATH);\
		echo "Debug" >> $(CY_IAR_CYIGNORE_PATH);\
		echo "Release" >> $(CY_IAR_CYIGNORE_PATH);\
	fi;\
	echo;\
	echo WARNING: The ewarm8 target is preliminary...


################################################################################
# VSCode
################################################################################

CY_VSCODE_OUT_PATH=$(CY_INTERNAL_APP_PATH)/.vscode
CY_VSCODE_OUT_TEMPLATE_PATH=$(CY_VSCODE_OUT_PATH)/cytemplates
CY_VSCODE_BACKUP_PATH=$(CY_VSCODE_OUT_PATH)/backup
CY_VSCODE_TEMPLATE_PATH=$(CY_INTERNAL_BASELIB_PATH)/make/scripts/vscode
CY_VSCODE_TEMPFILE=$(CY_CONFIG_DIR)/vscode_launch.temp

CY_HELP_vscode=Generates VSCode IDE json files (preliminary).
vscode: CY_IDE_preprint
ifeq ($(LIBNAME),)
	@mkdir -p $(CY_CONFIG_DIR);\
	mkdir -p $(CY_VSCODE_OUT_TEMPLATE_PATH);\
	mkdir -p $(CY_VSCODE_BACKUP_PATH);\
	echo $(CY_VSCODE_ARGS) > $(CY_VSCODE_TEMPFILE);\
	echo "s|&&JSONINCLUDELIST&&|$(foreach onedef,$(subst -I,,$(CY_RECIPE_INCLUDES)),\"$(onedef)\",)|" >> $(CY_VSCODE_TEMPFILE);\
	echo "s|&&JSONDEFINELIST&&|$(foreach onedef,$(subst -D,,$(CY_RECIPE_DEFINES)),\"$(onedef)\",)|" >> $(CY_VSCODE_TEMPFILE);\
	for json in $(CY_VSCODE_TEMPLATE_PATH)/*; do\
		jsonFile="$${json##*/}";\
		if [[ $$jsonFile == *"c_cpp_properties"* ]] && [[ $$jsonFile != *"c_cpp_properties_$(TOOLCHAIN).json" ]]; then\
			continue;\
		fi;\
		sed -f $(CY_VSCODE_TEMPFILE) $(CY_VSCODE_TEMPLATE_PATH)/$$jsonFile > $(CY_VSCODE_OUT_TEMPLATE_PATH)/$$jsonFile;\
		jsonFiles="$$jsonFiles $$jsonFile";\
		if [ -f $(CY_VSCODE_OUT_PATH)/$$jsonFile ] && [[ $$jsonFile == *"settings.json" ]]; then\
			echo;\
			echo "Modifying existing settings.json file. Check against the backup copy in .vscode/backup";\
			mv -f $(CY_VSCODE_OUT_PATH)/$$jsonFile $(CY_VSCODE_BACKUP_PATH)/$$jsonFile;\
			sed -e 's/\"\"/\"\"\,/g' $(CY_VSCODE_OUT_TEMPLATE_PATH)/$$jsonFile | grep -v -e "}" > $(CY_VSCODE_OUT_PATH)/$$jsonFile;\
			sed -e '/cortex-debug.armToolchainPath/d' -e '/cortex-debug.openocdPath/d' -e '/cortex-debug.JLinkGDBServerPath/d' -e '/^{/d'\
				$(CY_VSCODE_BACKUP_PATH)/$$jsonFile >> $(CY_VSCODE_OUT_PATH)/$$jsonFile;\
		else\
			cp $(CY_VSCODE_OUT_TEMPLATE_PATH)/$$jsonFile $(CY_VSCODE_OUT_PATH)/$$jsonFile;\
		fi;\
	done;\
	mv $(CY_VSCODE_OUT_PATH)/c_cpp_properties_$(TOOLCHAIN).json $(CY_VSCODE_OUT_PATH)/c_cpp_properties.json;\
	mv $(CY_VSCODE_OUT_PATH)/openocd.tcl $(CY_INTERNAL_APP_PATH)/openocd.tcl;\
	rm $(CY_VSCODE_TEMPFILE);\
	rm -rf $(CY_VSCODE_OUT_TEMPLATE_PATH);\
	echo;\
	echo Generated Visual Studio Code files: $$jsonFiles;\
	echo;\
	echo WARNING: The vscode target is preliminary...
else
	@echo 
endif


#
# Identify the phony targets
#
.PHONY: get_app_info eclipse ewarm8 vscode CY_IDE_preprint
