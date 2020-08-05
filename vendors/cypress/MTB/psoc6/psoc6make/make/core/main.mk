################################################################################
# \file main.mk
#
# \brief
# Defines the public facing build targets common to all recipes and includes
# the core makefiles.
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
# Macros
################################################################################

#
# Prints for getting time
# $(1) : Type - firststage, secondstage
# $(2) : Identifier
# $(3) : BEGIN or END
#
ifneq ($(CY_INSTRUMENT_BUILD),)
# Note: Use perl as "date" in macOS is based on ancient BSD
CY_LOG_TIME=$1 $2 $3:$(CY_TAB)$(CY_TAB)$(shell perl -MTime::HiRes -e 'printf("%.0f\n",Time::HiRes::time()*1000)')
endif


################################################################################
# User-facing make targets
################################################################################

all: build

getlibs:

build:

qbuild:

program:

qprogram:

debug:

qdebug:

clean: shared_libs dependent_apps
	rm -rf $(CY_BUILDTARGET_DIR)

# Note: Define the help target in BSP/recipe for custom help
help:

open:

modlibs:

config:

config_bt:

config_usbdev:

get_app_info:

eclipse:

ewarm8:

uvision5:

vscode:

#
# Targets that do not require a second build stage
#
all getlibs clean help:
open modlibs config config_bt config_usbdev:
bsp check get_app_info get_env_info printlibs:
app memcalc help_default:

#
# Targets that require a second build stage
#
build qbuild app: secondstage
program qprogram debug qdebug erase attach: secondstage
eclipse vscode ewarm8 uvision5: secondstage


################################################################################
# Applicable for both first and second build stages
################################################################################

CY_TIMESTAMP_MAIN_MK_BEGIN=$(call CY_LOG_TIME,bothstages,main.mk,BEGIN)

##########################
# Paths
##########################

#
# Set the build location. Append app dir if CY_BUILD_LOCATION is defined
#
ifneq ($(CY_BUILD_LOCATION),)
CY_BUILD_LOC:=$(CY_BUILD_LOCATION)/$(notdir $(CY_APP_LOCATION))
else
CY_BUILD_LOC:=$(CY_APP_LOCATION)/build
endif

#
# Windows paths
#
ifeq ($(OS),Windows_NT)

#
# CygWin/MSYS
#
ifneq ($(CY_WHICH_CYGPATH),)
CY_INTERNAL_BUILD_LOC:=$(shell cygpath -m --absolute $(subst \,/,$(CY_BUILD_LOC)))
ifneq ($(CY_BUILD_LOCATION),)
CY_INTERNAL_BUILD_LOCATION:=$(shell cygpath -m --absolute $(subst \,/,$(CY_BUILD_LOCATION)))
endif
ifneq ($(CY_DEVICESUPPORT_PATH),)
CY_INTERNAL_DEVICESUPPORT_PATH:=$(shell cygpath -m --absolute $(subst \,/,$(CY_DEVICESUPPORT_PATH)))
endif

#
# Other Windows environments
#
else
CY_INTERNAL_BUILD_LOC:=$(subst \,/,$(CY_BUILD_LOC))
CY_INTERNAL_BUILD_LOCATION:=$(subst \,/,$(CY_BUILD_LOCATION))
CY_INTERNAL_DEVICESUPPORT_PATH:=$(abspath $(subst \,/,$(CY_DEVICESUPPORT_PATH)))
endif # ifneq ($(CY_WHICH_CYGPATH),)

#
# Linux and macOS paths
#
else
CY_INTERNAL_BUILD_LOC:=$(CY_BUILD_LOC)
CY_INTERNAL_BUILD_LOCATION:=$(CY_BUILD_LOCATION)
CY_INTERNAL_DEVICESUPPORT_PATH:=$(abspath $(CY_DEVICESUPPORT_PATH))
endif # ifeq ($(OS),Windows_NT)

#
# Build directories
#
CY_RECIPE_DIR:=$(CY_INTERNAL_BUILD_LOC)
CY_BUILDTARGET_DIR:=$(CY_RECIPE_DIR)/$(TARGET)
CY_CONFIG_DIR:=$(CY_BUILDTARGET_DIR)/$(CONFIG)
CY_GENERATED_DIR:=$(CY_BUILDTARGET_DIR)/generated

#
# Default toolchain locations
#
CY_COMPILER_GCC_ARM_DEFAULT_DIR:=$(CY_COMPILER_DEFAULT_DIR)
CY_COMPILER_IAR_DEFAULT_DIR:="C:/Program Files (x86)/IAR Systems/Embedded Workbench 8.4/arm"
CY_COMPILER_ARM_DEFAULT_DIR:="C:/Program Files/ARMCompiler6.11"
CY_COMPILER_A_Clang_DEFAULT_DIR:=/Library/Developer/CommandLineTools/usr/lib/clang/10.0.0

#
# Toolchain locations
#
CY_COMPILER_GCC_ARM_DIR?=$(CY_COMPILER_GCC_ARM_DEFAULT_DIR)
CY_COMPILER_IAR_DIR?=$(CY_COMPILER_IAR_DEFAULT_DIR)
CY_COMPILER_ARM_DIR?=$(CY_COMPILER_ARM_DEFAULT_DIR)
CY_COMPILER_A_Clang_DIR?=$(CY_COMPILER_A_Clang_DEFAULT_DIR)


##########################
# Dependent libs
##########################

# Externally use DEPENDENT_LIB_PATHS. Internally use SEARCH_LIBS_AND_INCLUDES to preserve BWC
ifneq ($(DEPENDENT_LIB_PATHS),)
SEARCH_LIBS_AND_INCLUDES+=$(DEPENDENT_LIB_PATHS)
endif


##########################
# Include make files
##########################

CY_MAKECMDGOAL_LIST=clean qprogram qdebug erase attach bsp modlibs check get_env_info get_app_info help config open

# Make a decision on including logic pertinent to builds.
# If it's not any of these targets, then it's an actual build.
ifeq ($(sort $(foreach target,$(CY_MAKECMDGOAL_LIST),$(if $(findstring $(target),$(MAKECMDGOALS)),false))),false)
CY_COMMENCE_BUILD=false
else
CY_COMMENCE_BUILD=true
endif

#
# Include utilities used by all make files
#
CY_TIMESTAMP_UTILS_MK_BEGIN=$(call CY_LOG_TIME,bothstages,utils.mk,BEGIN)
include $(CY_BASELIB_CORE_PATH)/make/core/utils.mk
CY_TIMESTAMP_UTILS_MK_END=$(call CY_LOG_TIME,bothstages,utils.mk,END)

#
# Include any extra makefiles defined by app
#
CY_TIMESTAMP_EXTRA_INC_BEGIN=$(call CY_LOG_TIME,bothstages,EXTRA_INC,BEGIN)
include $(CY_EXTRA_INCLUDES)
CY_TIMESTAMP_EXTRA_INC_END=$(call CY_LOG_TIME,bothstages,EXTRA_INC,END)

#
# Find the target and check that the device is valid
#
CY_TIMESTAMP_TARGET_MK_BEGIN=$(call CY_LOG_TIME,bothstages,target.mk,BEGIN)
include $(CY_BASELIB_CORE_PATH)/make/core/target.mk
CY_TIMESTAMP_TARGET_MK_END=$(call CY_LOG_TIME,bothstages,target.mk,END)
CY_TIMESTAMP_FEATURES_MK_BEGIN=$(call CY_LOG_TIME,bothstages,features.mk,BEGIN)
-include $(CY_INTERNAL_BASELIB_PATH)/make/udd/features.mk
CY_TIMESTAMP_FEATURES_MK_END=$(call CY_LOG_TIME,bothstages,features.mk,END)
CY_TIMESTAMP_DEFINES_MK_BEGIN=$(call CY_LOG_TIME,bothstages,defines.mk,BEGIN)
include $(CY_INTERNAL_BASELIB_PATH)/make/recipe/defines.mk
CY_TIMESTAMP_DEFINES_MK_END=$(call CY_LOG_TIME,bothstages,defines.mk,END)

#
# Choose local or default toolchain makefile
#
CY_TIMESTAMP_TOOLCHAIN_MK_BEGIN=$(call CY_LOG_TIME,bothstages,toolchain.mk,BEGIN)
ifeq ($(TOOLCHAIN_MK_PATH),)
include $(CY_INTERNAL_BASELIB_PATH)/make/toolchains/$(TOOLCHAIN).mk
else
# Include the custom app-specific toolchain file
include $(TOOLCHAIN_MK_PATH)
endif
CY_TIMESTAMP_TOOLCHAIN_MK_END=$(call CY_LOG_TIME,bothstages,toolchain.mk,END)

#
# Configurator-related routines
#
CY_TIMESTAMP_CONFIG_MK_BEGIN=$(call CY_LOG_TIME,bothstages,config.mk,BEGIN)
include $(CY_BASELIB_CORE_PATH)/make/core/config.mk
CY_TIMESTAMP_CONFIG_MK_END=$(call CY_LOG_TIME,bothstages,config.mk,END)

#
# Launch tools
#
CY_TIMESTAMP_TOOLS_MK_BEGIN=$(call CY_LOG_TIME,bothstages,tools.mk,BEGIN)
-include $(CY_MAKEFILES_DIR)/tools.mk
CY_TIMESTAMP_TOOLS_MK_END=$(call CY_LOG_TIME,bothstages,tools.mk,END)
CY_TIMESTAMP_OPEN_MK_BEGIN=$(call CY_LOG_TIME,bothstages,open.mk,BEGIN)
include $(CY_BASELIB_CORE_PATH)/make/core/open.mk
CY_TIMESTAMP_OPEN_MK_END=$(call CY_LOG_TIME,bothstages,open.mk,END)

#
# Help documentation
#
CY_TIMESTAMP_HELP_MK_BEGIN=$(call CY_LOG_TIME,bothstages,help.mk,BEGIN)
include $(CY_BASELIB_CORE_PATH)/make/core/help.mk
CY_TIMESTAMP_HELP_MK_END=$(call CY_LOG_TIME,bothstages,help.mk,END)


################################################################################
# Include make files continued only for first build stage
################################################################################

ifeq ($(CY_SECONDSTAGE),)

# Check that there's only 1 version of tools and inform the user if there is not.
ifeq ($(sort $(notdir $(wildcard $(CY_TOOLS_PATHS)))),$(notdir $(CY_TOOLS_DIR)))
CY_TOOLS_MESSAGE=
else
CY_TOOLS_MESSAGE=INFO: Multiple tools versions were found in CY_TOOLS_PATHS="$(sort $(CY_TOOLS_PATHS))".\
				This build is currently using CY_TOOLS_DIR="$(CY_TOOLS_DIR)".\
				Check that this is the correct version that should be used in this build.\
				To stop seeing this message, explicitly set the CY_TOOLS_PATHS environment variable to the location of\
				the tools directory. This can be done either as an environment variable or set in the application Makefile.
$(info )
$(info $(CY_TOOLS_MESSAGE))
endif

CY_TIMESTAMP_PREBUILD_MK_BEGIN=$(call CY_LOG_TIME,firststage,prebuild.mk,BEGIN)
include $(CY_BASELIB_CORE_PATH)/make/core/prebuild.mk
CY_TIMESTAMP_PREBUILD_MK_END=$(call CY_LOG_TIME,firststage,prebuild.mk,END)
CY_TIMESTAMP_RECIPE_MK_BEGIN=$(call CY_LOG_TIME,firststage,recipe.mk,BEGIN)
include $(CY_INTERNAL_BASELIB_PATH)/make/recipe/recipe.mk
CY_TIMESTAMP_RECIPE_MK_END=$(call CY_LOG_TIME,firststage,recipe.mk,END)

##########################
# Environment check
##########################

CY_TIMESTAMP_PYTHON_BEGIN=$(call CY_LOG_TIME,firststage,PYTHON,BEGIN)

#
# Find Python path
# Note: This check has a dependency on target.mk and features.mk and
# is hence placed after these files are included.
#
ifeq ($(filter uvision5,$(MAKECMDGOALS)),uvision5)
CY_PYTHON_REQUIREMENT=true
endif
ifeq ($(filter ewarm8,$(MAKECMDGOALS)),ewarm8)
CY_PYTHON_REQUIREMENT=true
endif

ifeq ($(CY_PYTHON_REQUIREMENT),true)
ifeq ($(CY_PYTHON_PATH),)

ifeq ($(OS),Windows_NT)
#
# CygWin/MSYS
#

#
# On Windows, when using windows store python, cygwin or msys are not
# able to run the python executable downloaded from windows store. So,
# we run python from command prompt (in cygwin/msys) by prepending
# cmd /c.
# Do not remove the space at the end of the following variable assignment
#
CY_PYTHON_FROM_CMD=cmd /c 

#
# Other Windows environments
#
else
CY_PYTHON_FROM_CMD=
endif

#
# Check for python 3 intallation in the user's PATH
#   py -3 Windows python installer from python.org
#   python3 - Standard python3
#   python - Mapped python3 to python
#
CY_PYTHON_SEARCH_PATH:=$(shell \
	if [[ $$(py -3 --version 2>&1) == "Python 3"* ]]; then\
		echo py -3;\
	elif [[ $$($(CY_PYTHON_FROM_CMD)python3 --version 2>&1) == "Python 3"* ]]; then\
		echo $(CY_PYTHON_FROM_CMD)python3;\
	elif [[ $$($(CY_PYTHON_FROM_CMD)python --version 2>&1) == "Python 3"* ]]; then\
		echo $(CY_PYTHON_FROM_CMD)python;\
	else\
		echo NotFoundError;\
	fi)

ifeq ($(CY_PYTHON_SEARCH_PATH),NotFoundError)
$(info )
$(info Python 3 was not found in the user's PATH and it was not explicitly defined in the CY_PYTHON_PATH variable.\
This target requires a python 3 installation. You can obtain python 3 from "https://www.python.org" or you may\
obtain it using the following alternate methods.$(CY_NEWLINE)\
$(CY_NEWLINE)\
Windows: Windows Store$(CY_NEWLINE)\
macOS: brew install python3 $(CY_NEWLINE)\
Linux (Debian/Ubuntu): sudo apt-get python3 $(CY_NEWLINE)\
)
$(error )
endif

export CY_PYTHON_PATH=$(CY_PYTHON_SEARCH_PATH)

# User specified python path
else

ifeq ($(shell [[ $$($(CY_PYTHON_FROM_CMD)$(CY_PYTHON_PATH) --version 2>&1) == "Python 3"* ]] && { echo true; } || { echo false; }),false)
$(info The path "$(CY_PYTHON_PATH)" is either an invalid path or contains an incorrect version of python.$(CY_NEWLINE)\
Please provide the path to the python 3 executable. For example, "usr/bin/python3".$(CY_NEWLINE) )
$(error )
endif

endif # ifeq ($(CY_PYTHON_PATH),)
endif # ifeq ($(CY_PYTHON_REQUIREMENT),true)

CY_TIMESTAMP_PYTHON_END=$(call CY_LOG_TIME,firststage,PYTHON ,END)

##########################
# Second build stage target
##########################

secondstage: | prebuild
	$(CY_NOISE)echo "Commencing build operations..."; echo;
	$(CY_NOISE)$(MAKE) $(MAKECMDGOALS) \
	$(CY_SHAREDLIB_BUILD_LOCATIONS) \
	$(CY_DEPAPP_BUILD_LOCATIONS) \
	CY_SECONDSTAGE=true \
	--no-print-directory
	$(CY_NOISE)echo $(CY_TOOLS_MESSAGE) 


################################################################################
# Include make files continued for second build stage
################################################################################

else

ifeq ($(CY_COMMENCE_BUILD),true)
$(info )
$(info Initializing build: $(APPNAME)$(LIBNAME) $(CONFIG) $(TARGET) $(TOOLCHAIN))
ifeq ($(wildcard $(CY_INTERNAL_BASELIB_PATH)),)
$(info )
$(error Cannot find the base library. Run "make getlibs" and/or check\
that the library location is correct in the CY_BASELIB_PATH variable)
endif
endif

##########################
# User input check
##########################

ifneq ($(APPNAME),)
ifneq ($(LIBNAME),)
$(error An application cannot define both APPNAME and LIBNAME. Define one or the other)
endif
endif
ifneq ($(findstring -I,$(INCLUDES)),)
$(error INCLUDES must be directories without -I prepended)
endif
ifneq ($(findstring -D,$(DEFINES)),)
$(error DEFINES must be specified without -D prepended)
endif
ifneq ($(findstring -I,$(CFLAGS)),)
$(error Include paths must be specified in the INCLUDES variable instead\
of directly in CFLAGS. These must be directories without -I prepended)
endif
ifneq ($(findstring -D,$(CFLAGS)),)
$(error Defines must be specified in the DEFINES variable instead\
of directly in CFLAGS. These must be specified without -D prepended)
endif
ifneq ($(findstring -I,$(CXXFLAGS)),)
$(error Include paths must be specified in the INCLUDES variable instead\
of directly in CXXFLAGS. These must be directories without -I prepended)
endif
ifneq ($(findstring -D,$(CXXFLAGS)),)
$(error Defines must be specified in the DEFINES variable instead\
of directly in CXXFLAGS. These must be specified without -D prepended)
endif
ifneq ($(findstring -I,$(ASFLAGS)),)
$(error Include paths must be specified in the INCLUDES variable instead\
of directly in ASFLAGS. These must be directories without -I prepended)
endif
ifneq ($(findstring -D,$(ASFLAGS)),)
$(error Defines must be specified in the DEFINES variable instead\
of directly in ASFLAGS. These must be specified without -D prepended)
endif

##########################
# Search and build
##########################

#
# Build-related routines
#
ifeq ($(CY_COMMENCE_BUILD),true)

ifneq ($(findstring qbuild,$(MAKECMDGOALS)),qbuild)
CY_TIMESTAMP_SEARCH_MK_BEGIN=$(call CY_LOG_TIME,secondstage,search.mk,BEGIN)
include $(CY_BASELIB_CORE_PATH)/make/core/search.mk
CY_TIMESTAMP_SEARCH_MK_END=$(call CY_LOG_TIME,secondstage,search.mk,END)
else
CY_TIMESTAMP_CYQBUILD_MK_BEGIN=$(call CY_LOG_TIME,secondstage,cyqbuild.mk,BEGIN)
# Skip the auto-discovery and re-use the last build's source list
-include $(CY_CONFIG_DIR)/cyqbuild.mk
CY_QBUILD:=$(if $(wildcard $(CY_CONFIG_DIR)/cyqbuild.mk),true)
ifneq ($(CY_QBUILD),true)
$(info WARNING: Cannot find the auto-discovery make file. Run "make build" to generate it.)
endif
CY_TIMESTAMP_CYQBUILD_MK_END=$(call CY_LOG_TIME,secondstage,cyqbuild.mk,END)
endif

CY_TIMESTAMP_RECIPE_MK_BEGIN=$(call CY_LOG_TIME,secondstage,recipe.mk,BEGIN)
include $(CY_INTERNAL_BASELIB_PATH)/make/recipe/recipe.mk
CY_TIMESTAMP_RECIPE_MK_END=$(call CY_LOG_TIME,secondstage,recipe.mk,END)

ifneq ($(findstring vscode,$(MAKECMDGOALS)),vscode)
CY_TIMESTAMP_BUILD_MK_BEGIN=$(call CY_LOG_TIME,secondstage,build.mk,BEGIN)
include $(CY_BASELIB_CORE_PATH)/make/core/build.mk
CY_TIMESTAMP_BUILD_MK_END=$(call CY_LOG_TIME,secondstage,build.mk,END)
endif

endif # ifeq ($(CY_COMMENCE_BUILD),true)

#
# Optional recipe-specific program routine 
#
ifndef CY_BSP_PROGRAM
CY_TIMESTAMP_PROGRAM_MK_BEGIN=$(call CY_LOG_TIME,secondstage,program.mk,BEGIN)
-include $(CY_INTERNAL_BASELIB_PATH)/make/recipe/program.mk
CY_TIMESTAMP_PROGRAM_MK_END=$(call CY_LOG_TIME,secondstage,program.mk,END)
endif

#
# IDE file generation
#
CY_TIMESTAMP_IDE_MK_BEGIN=$(call CY_LOG_TIME,secondstage,ide.mk,BEGIN)
include $(CY_BASELIB_CORE_PATH)/make/core/ide.mk
CY_TIMESTAMP_IDE_MK_END=$(call CY_LOG_TIME,secondstage,ide.mk,END)

# Empty on purpose
secondstage:

endif # ifeq ($(CY_SECONDSTAGE),)

CY_TIMESTAMP_MAIN_MK_END=$(call CY_LOG_TIME,bothstages,main.mk,END)

#
# Print the timestamps
#
ifneq ($(CY_INSTRUMENT_BUILD),)
CY_TIMESTAMP_LIST=UTILS_MK EXTRA_INC TARGET_MK FEATURES_MK DEFINES_MK TOOLCHAIN_MK CONFIG_MK TOOLS_MK OPEN_MK HELP_MK\
					PREBUILD_MK RECIPE_MK PYTHON \
					SEARCH_MK CYQBUILD_MK RECIPE_MK BUILD_MK PROGRAM_MK IDE_MK

$(info )
$(info ==============================================================================)
$(info = Begin timestamps $(CY_TAB)$(CY_TAB)$(CY_TAB)(milliseconds) = )
$(info ==============================================================================)
$(info $(CY_TIMESTAMP_MAIN_MK_BEGIN))
$(foreach timestamp,$(CY_TIMESTAMP_LIST),\
	$(if $(CY_TIMESTAMP_$(timestamp)_BEGIN),\
		$(info $(CY_TIMESTAMP_$(timestamp)_BEGIN))\
		$(info $(CY_TIMESTAMP_$(timestamp)_END))\
	)\
)
$(info $(CY_TIMESTAMP_MAIN_MK_END))
$(info ==============================================================================)
$(info = End timestamps = )
$(info ==============================================================================)
$(info )
endif

#
# Identify the phony targets
#
.PHONY: all getlibs clean help
.PHONY: open modlibs config config_bt config_usbdev
.PHONY: bsp check get_app_info get_env_info printlibs
.PHONY: app memcalc help_default

.PHONY: build qbuild
.PHONY: program qprogram debug qdebug erase attach
.PHONY: eclipse vscode ewarm8 uvision5
