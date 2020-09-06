################################################################################
# \file CY8CKIT_064S0S2_4343W.mk
#
# \brief
# Define the CY8CKIT_064S0S2_4343W target.
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

# MCU device selection
DEVICE:=CYB0644ABZI-S2D44
# Additional devices on the board
ADDITIONAL_DEVICES:=CYW4343WKUBG
# Default target core to CM4 if not already set
CORE?=CM4
# Define default type of bootloading method [single, dual]
# single -> CM4 only, dual -> CM0 and CM4
# multi is similar to dual, but cybootloader checks both images
SECURE_CORE_MODE=multi

ifeq ($(CORE),CM4)
# Additional components supported by the target
COMPONENTS+=PSOC6HAL 4343W BSP_DESIGN_MODUS
# Use CyHAL
DEFINES+=CY_USING_HAL

ifeq ($(SECURE_CORE_MODE),single)
CY_LINKERSCRIPT_SUFFIX=cm4
CY_SECURE_POLICY_NAME=policy_single_stage_CM4
else ifeq ($(SECURE_CORE_MODE),dual)
CY_LINKERSCRIPT_SUFFIX=cm4_dual
CY_SECURE_POLICY_NAME=policy_dual_stage_CM0_CM4
else
CY_LINKERSCRIPT_SUFFIX=cm4_dual
CY_SECURE_POLICY_NAME=policy_multi_CM0_CM4
endif

else
CY_SECURE_POLICY_NAME=policy_dual_stage_CM0_CM4
endif

# Python path definition
ifeq ($(OS),Windows_NT)
CY_PYTHON_PATH?=python
else
CY_PYTHON_PATH?=python3
endif

ifneq ($(CY_TFM_ENABLED), true)
# BSP-specific post-build action
CY_BSP_POSTBUILD=$(CY_PYTHON_PATH) -c "import cysecuretools; \
	tools = cysecuretools.CySecureTools('cy8ckit-064s0s2-4343w', 'policy/$(CY_SECURE_POLICY_NAME).json'); \
	tools.sign_image('$(CY_CONFIG_DIR)/$(APPNAME).hex');"
else

# BSP programming flow
CY_BSP_PROGRAM=true
CY_OPENOCD_PROGRAM_FLASH= -s ${CY_OPENOCD_DIR}/scripts \
                          -f interface/kitprog3.cfg \
                          -f target/psoc6_2m_secure.cfg \
                          -c "init; reset init;" \
                          -c "flash erase_address 0x101c0000 0x10000" \
                          -c "flash write_image erase ${CY_CM0_HEX}" \
                          -c "flash write_image erase ${CY_CM4_HEX}" \
                          -c "resume; reset; exit"

program: build qprogram

qprogram: memcalc
	@echo;\
	echo "Programming PSoC64 ... ";\
	$(CY_OPENOCD_DIR)/bin/openocd $(CY_OPENOCD_PROGRAM_FLASH)
endif
