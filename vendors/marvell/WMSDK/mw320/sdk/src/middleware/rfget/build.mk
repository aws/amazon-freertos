#                Copyright 2008-2017, Marvell International Ltd.
# This code contains confidential information of Marvell Semiconductor, Inc.
# No rights are granted herein under any patent, mask work right or copyright
# of Marvell or any third party.
# Marvell reserves the right at its sole discretion to request that this code
# be immediately returned to Marvell. This code is provided "as is".
# Marvell makes no warranties, express, implied or otherwise, regarding its
# accuracy, completeness or performance.
#

libs-y += librfget

librfget-objs-y := write_firmware.c verify_firmware.c write_fs.c rfget.c #secure_upgrade.c
#librfget-objs-$(CONFIG_WIFI_FW_UPGRADE) += write_wifi_firmware.c
#librfget-objs-$(CONFIG_BT_FW_UPGRADE) += write_bt_firmware.c
#librfget-objs-$(CONFIG_FWUPG_ED_CHACHA) += fw_upgrade_ed_chacha.c
#librfget-objs-$(CONFIG_FWUPG_RSA_AES) += fw_upgrade_rsa_aes.c



librfget-supported-toolchain-y := arm_gcc iar
