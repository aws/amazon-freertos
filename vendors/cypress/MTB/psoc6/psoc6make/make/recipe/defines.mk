################################################################################
# \file defines.mk
#
# \brief
# Defines, needed for the PSoC 6 build recipe.
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
# List the supported toolchains
#
CY_SUPPORTED_TOOLCHAINS=GCC_ARM IAR ARM A_Clang

#
# Define the default core
#
CORE?=CM4
CY_START_FLASH=0x10000000
CY_START_SRAM=0x08000000

CY_OPEN_bt_configurator_DEVICE=--device PSoC6 

#
# Core specifics
#
CY_JLINKSCRIPT_CORE=CM4
CY_JLINK_DEVICE_CFG_DEBUG_SUFFIX=
CY_OPENOCD_CORE=cm4
CY_GDBINIT_FILE=gdbinit
CY_OPENOCD_SECOND_RESET_TYPE=run
CY_ECLIPSE_TEMPLATES_WILDCARD=$(CORE)/*
ifeq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_M0P)))
ifeq ($(CORE),CM0P)
$(call CY_MACRO_ERROR,$(DEVICE) does not have a CM0+ core)
endif
CY_LINKERSCRIPT_SUFFIX?=cm4
CY_OPENOCD_EXTRA_PORT_FLAG=
CY_OPENOCD_EXTRA_PORT_ECLIPSE=
CY_OPENOCD_CM0_DISABLE_FLAG=set ENABLE_CM0 0
CY_OPENOCD_CM0_DISABLE_ECLIPSE=-c \&quot;$(CY_OPENOCD_CM0_DISABLE_FLAG)\&quot;\&\#13;\&\#10;
else
ifeq ($(CORE),CM0P)
CY_LINKERSCRIPT_SUFFIX?=cm0plus
CY_JLINKSCRIPT_CORE=CM0p
CY_JLINK_DEVICE_CFG_DEBUG_SUFFIX=_tm
CY_OPENOCD_CORE=cm0
CY_OPENOCD_SECOND_RESET_TYPE=init
CY_GDBINIT_FILE=gdbinit_cm0
else
CY_LINKERSCRIPT_SUFFIX?=cm4_dual
CY_OPENOCD_EXTRA_PORT_FLAG=gdb_port 3332
CY_OPENOCD_EXTRA_PORT_ECLIPSE=-c \&quot;$(CY_OPENOCD_EXTRA_PORT_FLAG)\&quot;\&\#13;\&\#10;
CY_OPENOCD_CM0_DISABLE_FLAG=
CY_OPENOCD_CM0_DISABLE_ECLIPSE=
endif
endif

#
# Secure targets specifics
#
ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_SECURE)))
CY_GDBINIT_FILE=gdbinit_secure
CY_OPENOCD_CM0_DISABLE_FLAG=set TARGET_AP cm4_ap
CY_OPENOCD_CM0_DISABLE_ECLIPSE=-c \&quot;$(CY_OPENOCD_CM0_DISABLE_FLAG)\&quot;\&\#13;\&\#10;
CY_OPENOCD_SECOND_RESET_TYPE=init
CY_OPENOCD_EXTRA_PORT_FLAG=
CY_OPENOCD_EXTRA_PORT_ECLIPSE=
CY_OPENOCD_OTHER_RUN_CMD=
CY_OPENOCD_OTHER_RUN_CMD_ECLIPSE=
CY_OPENOCD_SMIF_DISABLE=set DISABLE_SMIF 1
CY_OPENOCD_SMIF_DISABLE_ECLIPSE=-c \&quot;$(CY_OPENOCD_SMIF_DISABLE)\&quot;\&\#13;\&\#10;
CY_ECLIPSE_TEMPLATES_WILDCARD=$(CORE)/*KitProg3*
endif

#
# Architecure specifics
#
CY_OPENOCD_CHIP_NAME=psoc6
CY_OPENOCD_OTHER_RUN_CMD?=mon psoc6 reset_halt sysresetreq
CY_OPENOCD_OTHER_RUN_CMD_ECLIPSE?=$(CY_OPENOCD_OTHER_RUN_CMD)\&\#13;\&\#10;
ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_DIE_PSOC6ABLE2)))

CY_PSOC_ARCH=psoc6_01
CY_PSOC_DIE_NAME=PSoC6ABLE2
CY_OPENOCD_DEVICE_CFG=psoc6.cfg
CY_JLINK_DEVICE_CFG_PROGRAM=CY8C6xx7_CM0p_sect256KB_tm
CY_JLINK_DEVICE_CFG_ATTACH=CY8C6xx7_$(CY_JLINKSCRIPT_CORE)_sect256KB
CY_JLINK_DEVICE_CFG_DEBUG=$(CY_JLINK_DEVICE_CFG_ATTACH)$(CY_JLINK_DEVICE_CFG_DEBUG_SUFFIX)
ifeq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_M0P)))
CY_JLINK_DEVICE_CFG_PROGRAM=CY8C6xx6_CM4_sect256KB
CY_JLINK_DEVICE_CFG_ATTACH=CY8C6xx6_$(CY_JLINKSCRIPT_CORE)_sect256KB
CY_JLINK_DEVICE_CFG_DEBUG=$(CY_JLINK_DEVICE_CFG_ATTACH)$(CY_JLINK_DEVICE_CFG_DEBUG_SUFFIX)
endif
ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_SECURE)))
CY_PSOC_ARCH=psoc6_secure
CY_PSOC_DIE_NAME=PSoC6ABLE2Secure
CY_OPENOCD_CHIP_NAME=psoc64
CY_OPENOCD_DEVICE_CFG=psoc6_secure.cfg
endif

else ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_DIE_PSOC6A2M)))

CY_PSOC_ARCH=psoc6_02
CY_PSOC_DIE_NAME=PSoC6A2M
CY_OPENOCD_DEVICE_CFG=psoc6_2m.cfg
CY_JLINK_DEVICE_CFG_PROGRAM=CY8C6xxA_CM0p_sect256KB_tm
CY_JLINK_DEVICE_CFG_ATTACH=CY8C6xxA_$(CY_JLINKSCRIPT_CORE)_sect256KB
CY_JLINK_DEVICE_CFG_DEBUG=$(CY_JLINK_DEVICE_CFG_ATTACH)$(CY_JLINK_DEVICE_CFG_DEBUG_SUFFIX)
ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_SECURE)))
CY_PSOC_ARCH=psoc6_2m_secure
CY_PSOC_DIE_NAME=PSoC6A2MSecure
CY_OPENOCD_CHIP_NAME=psoc64_2m
CY_OPENOCD_DEVICE_CFG=psoc6_2m_secure.cfg
endif

else ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_DIE_PSOC6A512K)))

CY_PSOC_ARCH=psoc6_03
CY_PSOC_DIE_NAME=PSoC6A512K
CY_OPENOCD_DEVICE_CFG=psoc6_512k.cfg
CY_JLINK_DEVICE_CFG_PROGRAM=CY8C6xx5_CM0p_sect256KB_tm
CY_JLINK_DEVICE_CFG_ATTACH=CY8C6xx5_$(CY_JLINKSCRIPT_CORE)_sect256KB
CY_JLINK_DEVICE_CFG_DEBUG=$(CY_JLINK_DEVICE_CFG_ATTACH)$(CY_JLINK_DEVICE_CFG_DEBUG_SUFFIX)
ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_SECURE)))
CY_PSOC_ARCH=psoc6_512k_secure
CY_PSOC_DIE_NAME=PSoC6A512KSecure
CY_OPENOCD_CHIP_NAME=psoc64_512k
CY_OPENOCD_DEVICE_CFG=psoc6_512k_secure.cfg
endif

else
$(call CY_MACRO_ERROR,Incorrect part number $(DEVICE). Check DEVICE variable.)
endif

#
# Flash memory specifics
#
ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_FLASH_KB_448)))
CY_MEMORY_FLASH=458752
else ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_FLASH_KB_512)))
CY_MEMORY_FLASH=524288
else ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_FLASH_KB_832)))
CY_MEMORY_FLASH=850944
else ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_FLASH_KB_1024)))
CY_MEMORY_FLASH=1048576
else ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_FLASH_KB_1856)))
CY_MEMORY_FLASH=1899520
else ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_FLASH_KB_2048)))
CY_MEMORY_FLASH=2097152
else
$(call CY_MACRO_ERROR,No Flash memory size defined for $(DEVICE))
endif

#
# SRAM memory specifics
#
ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_SRAM_KB_128)))
CY_MEMORY_SRAM=129024
else ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_SRAM_KB_256)))
CY_MEMORY_SRAM=260096
else ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_SRAM_KB_288)))
CY_MEMORY_SRAM=292864
else ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_SRAM_KB_512)))
CY_MEMORY_SRAM=522240
else ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_SRAM_KB_1024)))
CY_MEMORY_SRAM=1046528
else
$(call CY_MACRO_ERROR,No SRAM memory size defined for $(DEVICE))
endif

#
# linker scripts
#

# Secure parts
ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_SECURE)))

ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_DIE_PSOC6ABLE2)))
ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_FLASH_KB_832)))
CY_LINKER_SCRIPT_NAME=cyb06xx7
endif
else ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_DIE_PSOC6A2M)))
ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_FLASH_KB_1856)))
CY_LINKER_SCRIPT_NAME=cyb06xxa
endif
else ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_DIE_PSOC6A512K)))
ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_FLASH_KB_448)))
CY_LINKER_SCRIPT_NAME=cyb06xx5
endif
endif

# Non-secure part
else

ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_DIE_PSOC6A512K)))
ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_FLASH_KB_512)))
CY_LINKER_SCRIPT_NAME=cy8c6xx5
endif
else ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_DIE_PSOC6ABLE2)))
ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_FLASH_KB_512)))
CY_LINKER_SCRIPT_NAME=cy8c6xx6
else ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_FLASH_KB_1024)))
CY_LINKER_SCRIPT_NAME=cy8c6xx7
endif
else ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_DIE_PSOC6A2M)))
ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_FLASH_KB_1024)))
CY_LINKER_SCRIPT_NAME=cy8c6xx8
else ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_FLASH_KB_2048)))
CY_LINKER_SCRIPT_NAME=cy8c6xxa
endif
endif

endif

ifeq ($(CY_LINKER_SCRIPT_NAME),)
$(call CY_MACRO_ERROR,Could not resolve device series for linker script)
endif

#
# BSP generation
#
DEVICE_GEN?=$(DEVICE)
ADDITIONAL_DEVICES_GEN?=$(ADDITIONAL_DEVICES)

# Core specifics
ifeq (,$(findstring $(DEVICE_GEN),$(CY_DEVICES_WITH_M0P)))
CY_BSP_LINKERSCRIPT_CM4_SUFFIX=cm4
CY_BSP_STARTUP_CM4_SUFFIX=cm4
else
CY_BSP_LINKERSCRIPT_CM0P_SUFFIX=cm0plus
CY_BSP_STARTUP_CM0P_SUFFIX=cm0plus
CY_BSP_LINKERSCRIPT_CM4_SUFFIX=cm4_dual
CY_BSP_STARTUP_CM4_SUFFIX=cm4
endif

# Architecture specifics
ifneq (,$(findstring $(DEVICE_GEN),$(CY_DEVICES_WITH_DIE_PSOC6ABLE2)))
CY_BSP_STARTUP=psoc6_01
else ifneq (,$(findstring $(DEVICE_GEN),$(CY_DEVICES_WITH_DIE_PSOC6A2M)))
CY_BSP_STARTUP=psoc6_02
else ifneq (,$(findstring $(DEVICE_GEN),$(CY_DEVICES_WITH_DIE_PSOC6A512K)))
CY_BSP_STARTUP=psoc6_03
endif

# Linker scripts - Secure parts
ifneq (,$(findstring $(DEVICE_GEN),$(CY_DEVICES_SECURE)))

ifneq (,$(findstring $(DEVICE_GEN),$(CY_DEVICES_WITH_DIE_PSOC6ABLE2)))
ifneq (,$(findstring $(DEVICE_GEN),$(CY_DEVICES_WITH_FLASH_KB_832)))
CY_BSP_LINKER_SCRIPT=cyb06xx7
endif
else ifneq (,$(findstring $(DEVICE_GEN),$(CY_DEVICES_WITH_DIE_PSOC6A2M)))
ifneq (,$(findstring $(DEVICE_GEN),$(CY_DEVICES_WITH_FLASH_KB_1856)))
CY_BSP_LINKER_SCRIPT=cyb06xxa
endif
endif

# Linker scripts - Non-secure parts
else

ifneq (,$(findstring $(DEVICE_GEN),$(CY_DEVICES_WITH_DIE_PSOC6A512K)))
ifneq (,$(findstring $(DEVICE_GEN),$(CY_DEVICES_WITH_FLASH_KB_512)))
CY_BSP_LINKER_SCRIPT=cy8c6xx5
endif
else ifneq (,$(findstring $(DEVICE_GEN),$(CY_DEVICES_WITH_DIE_PSOC6ABLE2)))
ifneq (,$(findstring $(DEVICE_GEN),$(CY_DEVICES_WITH_FLASH_KB_512)))
CY_BSP_LINKER_SCRIPT=cy8c6xx6
else ifneq (,$(findstring $(DEVICE_GEN),$(CY_DEVICES_WITH_FLASH_KB_1024)))
CY_BSP_LINKER_SCRIPT=cy8c6xx7
endif
else ifneq (,$(findstring $(DEVICE_GEN),$(CY_DEVICES_WITH_DIE_PSOC6A2M)))
ifneq (,$(findstring $(DEVICE_GEN),$(CY_DEVICES_WITH_FLASH_KB_1024)))
CY_BSP_LINKER_SCRIPT=cy8c6xx8
else ifneq (,$(findstring $(DEVICE_GEN),$(CY_DEVICES_WITH_FLASH_KB_2048)))
CY_BSP_LINKER_SCRIPT=cy8c6xxa
endif
endif

endif

CY_BSP_LINKER_SCRIPT_CM0P=$(CY_BSP_LINKER_SCRIPT)_$(CY_BSP_LINKERSCRIPT_CM0P_SUFFIX)
CY_BSP_LINKER_SCRIPT_CM4=$(CY_BSP_LINKER_SCRIPT)_$(CY_BSP_LINKERSCRIPT_CM4_SUFFIX)
CY_BSP_STARTUP_CM0P=$(CY_BSP_STARTUP)_$(CY_BSP_STARTUP_CM0P_SUFFIX)
CY_BSP_STARTUP_CM4=$(CY_BSP_STARTUP)_$(CY_BSP_STARTUP_CM4_SUFFIX)

# Paths
CY_BSP_TEMPLATES_DIR=$(call CY_MACRO_DIR,$(firstword $(CY_DEVICESUPPORT_SEARCH_PATH)))/devices/templates/COMPONENT_MTB
CY_BSP_DESTINATION_CM0P_DIR=$(CY_TARGET_GEN_DIR)/COMPONENT_CM0P
CY_BSP_DESTINATION_CM4_DIR=$(CY_TARGET_GEN_DIR)/COMPONENT_CM4
CY_BSP_DESTINATION_ABSOLUTE=$(abspath $(CY_TARGET_GEN_DIR))

# Command for copying linker scripts and starups
ifeq ($(sort $(CY_BSP_LINKER_SCRIPT) $(CY_BSP_STARTUP)),)
CY_BSP_TEMPLATES_CMD=echo "Could not determine linker scripts and startup files for DEVICE $(DEVICE_GEN). Skipping update...";
else
CY_BSP_TEMPLATES_CMD=\
	if [ -d $(CY_BSP_TEMPLATES_DIR) ]; then \
		echo "Populating $(CY_BSP_LINKER_SCRIPT) linker scripts and $(CY_BSP_STARTUP) startup files...";\
		rm -rf $(CY_BSP_DESTINATION_CM0P_DIR) $(CY_BSP_DESTINATION_CM4_DIR);\
		pushd  $(CY_BSP_TEMPLATES_DIR) 1> /dev/null;\
		find . -type d -exec mkdir -p $(CY_BSP_DESTINATION_ABSOLUTE)/'{}' \; ;\
		find . -type f \( \
		-name "system_psoc6*" \
		-o -name "*$(CY_BSP_STARTUP_CM0P)*" \
		-o -name "*$(CY_BSP_STARTUP_CM4)*" \
		-o -name "*$(CY_BSP_LINKER_SCRIPT_CM0P)*" \
		-o -name "*$(CY_BSP_LINKER_SCRIPT_CM4)*" \
		\) -exec cp -p '{}' $(CY_BSP_DESTINATION_ABSOLUTE)/'{}' \; ;\
		popd 1> /dev/null;\
	else \
		echo "Could not detect template linker scripts and startup files. Skipping update...";\
	fi;
endif

# Command for updating the device(s) 
CY_BSP_DEVICES_CMD=\
	designFile=$$(find $(CY_TARGET_GEN_DIR) -name *.modus);\
	if [[ $$designFile ]]; then\
		echo "Running device-configurator for $(DEVICE_GEN) $(ADDITIONAL_DEVICES_GEN)...";\
		$(CY_CONFIG_MODUS_EXEC)\
		$(CY_CONFIG_LIBFILE)\
		--build $$designFile\
		--set-device=$(subst $(CY_SPACE),$(CY_COMMA),$(DEVICE_GEN) $(ADDITIONAL_DEVICES_GEN));\
		cfgStatus=$$(echo $$?);\
		if [ $$cfgStatus != 0 ]; then echo "ERROR: Device-configuration failed for $$designFile"; exit $$cfgStatus; fi;\
	else\
		echo "Could not detect .modus file. Skipping update...";\
	fi;

#
# Paths used in program/debug
#
ifeq ($(CY_DEVICESUPPORT_PATH),)
CY_OPENOCD_SVD_PATH?=
else
CY_OPENOCD_SVD_PATH?=
endif
CY_OPENOCD_QSPI_CFG_PATH=$(CY_TARGET_DIR)/COMPONENT_BSP_DESIGN_MODUS/GeneratedSource

#
# Set the output file paths
#
ifneq ($(CY_BUILD_LOCATION),)
CY_SYM_FILE?=$(CY_INTERNAL_BUILD_LOC)/$(TARGET)/$(CONFIG)/$(APPNAME).elf
CY_PROG_FILE?=$(CY_INTERNAL_BUILD_LOC)/$(TARGET)/$(CONFIG)/$(APPNAME).hex
else
CY_SYM_FILE?=\$$\{cy_prj_path\}/$(notdir $(CY_INTERNAL_BUILD_LOC))/$(TARGET)/$(CONFIG)/$(APPNAME).elf
CY_PROG_FILE?=\$$\{cy_prj_path\}/$(notdir $(CY_INTERNAL_BUILD_LOC))/$(TARGET)/$(CONFIG)/$(APPNAME).hex
endif

#
# IDE specifics
#

CY_VSCODE_ARGS="s|&&RELEASETARGET&&|build/$(TARGET)/Release/$(APPNAME).elf|g;"\
				"s|&&DEBUGTARGET&&|build/$(TARGET)/Debug/$(APPNAME).elf|g;"\
				"s|&&PSOCFAMILY&&|$(CY_PSOC_ARCH)|g;"\
				"s|&&MODUSSHELL&&|$(CY_MODUS_SHELL_DIR)|g;"\
				"s|&&OPENOCDFILE&&|$(CY_OPENOCD_DEVICE_CFG)|g;"\
				"s|&&SVDFILENAME&&|$(CY_OPENOCD_SVD_PATH)|g;"\
				"s|&&MODUSTOOLCHAIN&&|$(CY_COMPILER_DIR)|g;"\
				"s|&&MODUSTOOLCHAINVERSION&&|$(subst gcc-,,$(notdir $(CY_COMPILER_DIR)))|g;"\
				"s|&&CFLAGS&&|$(CY_RECIPE_CFLAGS)|g;"\
				"s|&&MODUSOPENCOD&&|$(CY_OPENOCD_DIR)|g;"\
				"s|&&MODUSLIBMANAGER&&|$(CY_LIBRARY_MANAGER_DIR)|g;"\

CY_ECLIPSE_ARGS="s|&&CY_OPENOCD_CFG&&|$(CY_OPENOCD_DEVICE_CFG)|g;"\
				"s|&&CY_OPENOCD_CHIP&&|$(CY_OPENOCD_CHIP_NAME)|g;"\
				"s|&&CY_OPENOCD_SECOND_RESET&&|$(CY_OPENOCD_SECOND_RESET_TYPE)|g;"\
				"s|&&CY_OPENOCD_OTHER_RUN_CMD&&|$(CY_OPENOCD_OTHER_RUN_CMD_ECLIPSE)|g;"\
				"s|&&CY_JLINK_CFG_PROGRAM&&|$(CY_JLINK_DEVICE_CFG_PROGRAM)|g;"\
				"s|&&CY_JLINK_CFG_DEBUG&&|$(CY_JLINK_DEVICE_CFG_DEBUG)|g;"\
				"s|&&CY_JLINK_CFG_ATTACH&&|$(CY_JLINK_DEVICE_CFG_ATTACH)|g;"\
				"s|&&CY_OPENOCD_PORT_SELECT&&|$(CY_OPENOCD_EXTRA_PORT_ECLIPSE)|g;"\
				"s|&&CY_OPENOCD_CM0_FLAG&&|$(CY_OPENOCD_CM0_DISABLE_ECLIPSE)|g;"\
				"s|&&CY_OPENOCD_SMIF_DISABLE&&|$(CY_OPENOCD_SMIF_DISABLE_ECLIPSE)|g;"\
				"s|&&CY_CORE&&|$(CY_OPENOCD_CORE)|g;"\
				"s|&&CY_APPNAME&&|$(CY_IDE_PRJNAME)|g;"\
				"s|&&CY_CONFIG&&|$(CONFIG)|g;"\
				"s|&&CY_QSPI_CFG_PATH&&|$(CY_OPENOCD_QSPI_CFG_PATH)|g;"\
				"s|&&CY_SVD_PATH&&|$(CY_OPENOCD_SVD_PATH)|g;"\
				"s|&&CY_SYM_FILE&&|$(CY_SYM_FILE)|g;"\
				"s|&&CY_PROG_FILE&&|$(CY_PROG_FILE)|g;"

#
# Tools specifics
#
ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_CAPSENSE)))
CY_SUPPORTED_TOOL_TYPES+=capsense-configurator capsense-tuner
endif
ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_BLE)))
CY_SUPPORTED_TOOL_TYPES+=bt-configurator
endif
ifneq (,$(findstring $(DEVICE),$(CY_DEVICES_WITH_FS_USB)))
CY_SUPPORTED_TOOL_TYPES+=usbdev-configurator
endif
CY_SUPPORTED_TOOL_TYPES+=\
	device-configurator\
	qspi-configurator\
	seglcd-configurator\
	smartio-configurator\
	cype-tool\
	dfuh-tool

# PSoC 6 smartio also uses the .modus extension
modus_DEFAULT_TYPE+=device-configurator smartio-configurator

# PSoC 6 capsense-tuner shares its existence with capsense-configurator
CY_OPEN_NEWCFG_XML_TYPES+=capsense-tuner
