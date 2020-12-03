# This file contains the steps specific to board that has TFM feature enabled.

TFM_PSOC64_PATH=$(CY_EXTAPP_PATH)/psoc6/psoc64tfm
TFM_PSOC64_SECURE_POLICY_PATH=$(TFM_PSOC64_PATH)/security/policy
TFM_POLICY_FILE=$(TFM_PSOC64_SECURE_POLICY_PATH)/$(CY_SECURE_POLICY_NAME)_jitp.json
TFM_SIGN_SCRIPT=cysecuretools
TFM_DEVICE_NAME=cy8ckit-064s0s2-4343w
TFM_CM0_HEX= $(TFM_PSOC64_PATH)/COMPONENT_TFM_S_FW/tfm_s_unsigned.hex

CY_CM4_ELF=$(CY_CONFIG_DIR)/cm4.elf
CY_CM0_HEX=$(CY_CONFIG_DIR)/cm0.hex
CY_CM4_HEX=$(CY_CONFIG_DIR)/cm4.hex
CY_CM0_UNSIGNED_HEX=$(CY_CONFIG_DIR)/cm0_unsigned.hex
CY_CM4_UNSIGNED_HEX=$(CY_CONFIG_DIR)/cm4_unsigned.hex

ifeq ($(TOOLCHAIN),GCC_ARM)
POSTBUILD=$(CY_CROSSPATH)/bin/arm-none-eabi-objcopy -R .cy_sflash_user_data -R .cy_toc_part2 $(CY_CONFIG_DIR)/$(APPNAME).elf $(CY_CM4_ELF);
POSTBUILD+=$(CY_CROSSPATH)/bin/arm-none-eabi-objcopy -O ihex $(CY_CM4_ELF) $(CY_CM4_HEX);
else ifeq ($(TOOLCHAIN),IAR)
POSTBUILD=${CY_CROSSPATH}/bin/ielftool --ihex $(CY_CONFIG_DIR)/$(APPNAME).elf $(CY_CM4_HEX);
else ifeq ($(TOOLCHAIN),ARM)
POSTBUILD=$(CY_CROSSPATH)/bin/fromelf --i32 --output=$(CY_CM4_HEX) $(CY_CONFIG_DIR)/$(APPNAME).elf;
endif

POSTBUILD+=cp "${TFM_CM0_HEX}" "${CY_CM0_HEX}";
#For singing upgrade image, cysecuretools creates file "<name>_upgrade.hex" for signed image, then rename orignal file as "<name>_unsigned.hex".
POSTBUILD+=$(TFM_SIGN_SCRIPT) --policy "${TFM_POLICY_FILE}" --target "${TFM_DEVICE_NAME}" sign-image --hex "${CY_CM0_HEX}" --image-type UPGRADE --image-id 1;
POSTBUILD+=$(TFM_SIGN_SCRIPT) --policy "${TFM_POLICY_FILE}" --target "${TFM_DEVICE_NAME}" sign-image --hex "${CY_CM4_HEX}" --image-type UPGRADE --image-id 16;
POSTBUILD+=cp "${CY_CM0_UNSIGNED_HEX}"  "${CY_CM0_HEX}";
POSTBUILD+=cp "${CY_CM4_UNSIGNED_HEX}"  "${CY_CM4_HEX}";
# For signing boot image, cysecuretools over-write oringal file with signed image
POSTBUILD+=$(TFM_SIGN_SCRIPT) --policy "${TFM_POLICY_FILE}" --target "${TFM_DEVICE_NAME}" sign-image --hex "${CY_CM0_HEX}" --image-type BOOT --image-id 1;
POSTBUILD+=$(TFM_SIGN_SCRIPT) --policy "${TFM_POLICY_FILE}" --target "${TFM_DEVICE_NAME}" sign-image --hex "${CY_CM4_HEX}" --image-type BOOT --image-id 16;