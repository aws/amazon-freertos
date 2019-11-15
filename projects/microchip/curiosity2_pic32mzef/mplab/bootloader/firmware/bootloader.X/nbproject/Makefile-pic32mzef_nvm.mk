#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-pic32mzef_nvm.mk)" "nbproject/Makefile-local-pic32mzef_nvm.mk"
include nbproject/Makefile-local-pic32mzef_nvm.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=pic32mzef_nvm
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/crypto/tinycrypt/asn1utility.c ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/crypto/tinycrypt/aws_boot_crypto.c ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_flash.c ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_nvm.c ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_partition.c ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/loader/aws_boot_loader.c ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/loader/aws_boot_pal.c ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/source/app.c ../src/config/pic32mzef_nvm/bsp/bsp.c ../src/config/pic32mzef_nvm/peripheral/clk/plib_clk.c ../src/config/pic32mzef_nvm/peripheral/evic/plib_evic.c ../src/config/pic32mzef_nvm/peripheral/gpio/plib_gpio.c ../src/config/pic32mzef_nvm/peripheral/nvm/plib_nvm.c ../src/config/pic32mzef_nvm/peripheral/uart/plib_uart1.c ../src/config/pic32mzef_nvm/stdio/xc32_monitor.c ../src/config/pic32mzef_nvm/system/int/src/sys_int.c ../src/config/pic32mzef_nvm/initialization.c ../src/config/pic32mzef_nvm/interrupts.c ../src/config/pic32mzef_nvm/exceptions.c ../../../../../../../libraries/3rdparty/tinycrypt/asn1/asn1parse.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/aes_decrypt.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/aes_encrypt.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/cbc_mode.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ccm_mode.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/cmac_mode.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ctr_mode.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ctr_prng.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_dh.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_dsa.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_platform_specific.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/hmac.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/hmac_prng.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/sha256.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/utils.c ../src/main.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/671897462/asn1utility.o ${OBJECTDIR}/_ext/671897462/aws_boot_crypto.o ${OBJECTDIR}/_ext/15205594/aws_boot_flash.o ${OBJECTDIR}/_ext/15205594/aws_boot_nvm.o ${OBJECTDIR}/_ext/15205594/aws_boot_partition.o ${OBJECTDIR}/_ext/645904489/aws_boot_loader.o ${OBJECTDIR}/_ext/645904489/aws_boot_pal.o ${OBJECTDIR}/_ext/1657551691/app.o ${OBJECTDIR}/_ext/1530740410/bsp.o ${OBJECTDIR}/_ext/1950090572/plib_clk.o ${OBJECTDIR}/_ext/323196359/plib_evic.o ${OBJECTDIR}/_ext/323142531/plib_gpio.o ${OBJECTDIR}/_ext/1950079689/plib_nvm.o ${OBJECTDIR}/_ext/322739588/plib_uart1.o ${OBJECTDIR}/_ext/2116527036/xc32_monitor.o ${OBJECTDIR}/_ext/128587639/sys_int.o ${OBJECTDIR}/_ext/770344950/initialization.o ${OBJECTDIR}/_ext/770344950/interrupts.o ${OBJECTDIR}/_ext/770344950/exceptions.o ${OBJECTDIR}/_ext/1214204543/asn1parse.o ${OBJECTDIR}/_ext/1193839791/aes_decrypt.o ${OBJECTDIR}/_ext/1193839791/aes_encrypt.o ${OBJECTDIR}/_ext/1193839791/cbc_mode.o ${OBJECTDIR}/_ext/1193839791/ccm_mode.o ${OBJECTDIR}/_ext/1193839791/cmac_mode.o ${OBJECTDIR}/_ext/1193839791/ctr_mode.o ${OBJECTDIR}/_ext/1193839791/ctr_prng.o ${OBJECTDIR}/_ext/1193839791/ecc.o ${OBJECTDIR}/_ext/1193839791/ecc_dh.o ${OBJECTDIR}/_ext/1193839791/ecc_dsa.o ${OBJECTDIR}/_ext/1193839791/ecc_platform_specific.o ${OBJECTDIR}/_ext/1193839791/hmac.o ${OBJECTDIR}/_ext/1193839791/hmac_prng.o ${OBJECTDIR}/_ext/1193839791/sha256.o ${OBJECTDIR}/_ext/1193839791/utils.o ${OBJECTDIR}/_ext/1360937237/main.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/671897462/asn1utility.o.d ${OBJECTDIR}/_ext/671897462/aws_boot_crypto.o.d ${OBJECTDIR}/_ext/15205594/aws_boot_flash.o.d ${OBJECTDIR}/_ext/15205594/aws_boot_nvm.o.d ${OBJECTDIR}/_ext/15205594/aws_boot_partition.o.d ${OBJECTDIR}/_ext/645904489/aws_boot_loader.o.d ${OBJECTDIR}/_ext/645904489/aws_boot_pal.o.d ${OBJECTDIR}/_ext/1657551691/app.o.d ${OBJECTDIR}/_ext/1530740410/bsp.o.d ${OBJECTDIR}/_ext/1950090572/plib_clk.o.d ${OBJECTDIR}/_ext/323196359/plib_evic.o.d ${OBJECTDIR}/_ext/323142531/plib_gpio.o.d ${OBJECTDIR}/_ext/1950079689/plib_nvm.o.d ${OBJECTDIR}/_ext/322739588/plib_uart1.o.d ${OBJECTDIR}/_ext/2116527036/xc32_monitor.o.d ${OBJECTDIR}/_ext/128587639/sys_int.o.d ${OBJECTDIR}/_ext/770344950/initialization.o.d ${OBJECTDIR}/_ext/770344950/interrupts.o.d ${OBJECTDIR}/_ext/770344950/exceptions.o.d ${OBJECTDIR}/_ext/1214204543/asn1parse.o.d ${OBJECTDIR}/_ext/1193839791/aes_decrypt.o.d ${OBJECTDIR}/_ext/1193839791/aes_encrypt.o.d ${OBJECTDIR}/_ext/1193839791/cbc_mode.o.d ${OBJECTDIR}/_ext/1193839791/ccm_mode.o.d ${OBJECTDIR}/_ext/1193839791/cmac_mode.o.d ${OBJECTDIR}/_ext/1193839791/ctr_mode.o.d ${OBJECTDIR}/_ext/1193839791/ctr_prng.o.d ${OBJECTDIR}/_ext/1193839791/ecc.o.d ${OBJECTDIR}/_ext/1193839791/ecc_dh.o.d ${OBJECTDIR}/_ext/1193839791/ecc_dsa.o.d ${OBJECTDIR}/_ext/1193839791/ecc_platform_specific.o.d ${OBJECTDIR}/_ext/1193839791/hmac.o.d ${OBJECTDIR}/_ext/1193839791/hmac_prng.o.d ${OBJECTDIR}/_ext/1193839791/sha256.o.d ${OBJECTDIR}/_ext/1193839791/utils.o.d ${OBJECTDIR}/_ext/1360937237/main.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/671897462/asn1utility.o ${OBJECTDIR}/_ext/671897462/aws_boot_crypto.o ${OBJECTDIR}/_ext/15205594/aws_boot_flash.o ${OBJECTDIR}/_ext/15205594/aws_boot_nvm.o ${OBJECTDIR}/_ext/15205594/aws_boot_partition.o ${OBJECTDIR}/_ext/645904489/aws_boot_loader.o ${OBJECTDIR}/_ext/645904489/aws_boot_pal.o ${OBJECTDIR}/_ext/1657551691/app.o ${OBJECTDIR}/_ext/1530740410/bsp.o ${OBJECTDIR}/_ext/1950090572/plib_clk.o ${OBJECTDIR}/_ext/323196359/plib_evic.o ${OBJECTDIR}/_ext/323142531/plib_gpio.o ${OBJECTDIR}/_ext/1950079689/plib_nvm.o ${OBJECTDIR}/_ext/322739588/plib_uart1.o ${OBJECTDIR}/_ext/2116527036/xc32_monitor.o ${OBJECTDIR}/_ext/128587639/sys_int.o ${OBJECTDIR}/_ext/770344950/initialization.o ${OBJECTDIR}/_ext/770344950/interrupts.o ${OBJECTDIR}/_ext/770344950/exceptions.o ${OBJECTDIR}/_ext/1214204543/asn1parse.o ${OBJECTDIR}/_ext/1193839791/aes_decrypt.o ${OBJECTDIR}/_ext/1193839791/aes_encrypt.o ${OBJECTDIR}/_ext/1193839791/cbc_mode.o ${OBJECTDIR}/_ext/1193839791/ccm_mode.o ${OBJECTDIR}/_ext/1193839791/cmac_mode.o ${OBJECTDIR}/_ext/1193839791/ctr_mode.o ${OBJECTDIR}/_ext/1193839791/ctr_prng.o ${OBJECTDIR}/_ext/1193839791/ecc.o ${OBJECTDIR}/_ext/1193839791/ecc_dh.o ${OBJECTDIR}/_ext/1193839791/ecc_dsa.o ${OBJECTDIR}/_ext/1193839791/ecc_platform_specific.o ${OBJECTDIR}/_ext/1193839791/hmac.o ${OBJECTDIR}/_ext/1193839791/hmac_prng.o ${OBJECTDIR}/_ext/1193839791/sha256.o ${OBJECTDIR}/_ext/1193839791/utils.o ${OBJECTDIR}/_ext/1360937237/main.o

# Source Files
SOURCEFILES=../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/crypto/tinycrypt/asn1utility.c ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/crypto/tinycrypt/aws_boot_crypto.c ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_flash.c ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_nvm.c ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_partition.c ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/loader/aws_boot_loader.c ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/loader/aws_boot_pal.c ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/source/app.c ../src/config/pic32mzef_nvm/bsp/bsp.c ../src/config/pic32mzef_nvm/peripheral/clk/plib_clk.c ../src/config/pic32mzef_nvm/peripheral/evic/plib_evic.c ../src/config/pic32mzef_nvm/peripheral/gpio/plib_gpio.c ../src/config/pic32mzef_nvm/peripheral/nvm/plib_nvm.c ../src/config/pic32mzef_nvm/peripheral/uart/plib_uart1.c ../src/config/pic32mzef_nvm/stdio/xc32_monitor.c ../src/config/pic32mzef_nvm/system/int/src/sys_int.c ../src/config/pic32mzef_nvm/initialization.c ../src/config/pic32mzef_nvm/interrupts.c ../src/config/pic32mzef_nvm/exceptions.c ../../../../../../../libraries/3rdparty/tinycrypt/asn1/asn1parse.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/aes_decrypt.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/aes_encrypt.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/cbc_mode.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ccm_mode.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/cmac_mode.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ctr_mode.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ctr_prng.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_dh.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_dsa.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_platform_specific.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/hmac.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/hmac_prng.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/sha256.c ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/utils.c ../src/main.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-pic32mzef_nvm.mk dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MZ2048EFM144
MP_LINKER_FILE_OPTION=,--script="..\..\..\..\..\..\..\vendors\microchip\boards\curiosity2_pic32mzef\bootloader\linker_script\btl_mz.ld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/671897462/asn1utility.o: ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/crypto/tinycrypt/asn1utility.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/671897462" 
	@${RM} ${OBJECTDIR}/_ext/671897462/asn1utility.o.d 
	@${RM} ${OBJECTDIR}/_ext/671897462/asn1utility.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671897462/asn1utility.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/671897462/asn1utility.o.d" -o ${OBJECTDIR}/_ext/671897462/asn1utility.o ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/crypto/tinycrypt/asn1utility.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/671897462/aws_boot_crypto.o: ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/crypto/tinycrypt/aws_boot_crypto.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/671897462" 
	@${RM} ${OBJECTDIR}/_ext/671897462/aws_boot_crypto.o.d 
	@${RM} ${OBJECTDIR}/_ext/671897462/aws_boot_crypto.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671897462/aws_boot_crypto.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/671897462/aws_boot_crypto.o.d" -o ${OBJECTDIR}/_ext/671897462/aws_boot_crypto.o ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/crypto/tinycrypt/aws_boot_crypto.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/15205594/aws_boot_flash.o: ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/15205594" 
	@${RM} ${OBJECTDIR}/_ext/15205594/aws_boot_flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/15205594/aws_boot_flash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/15205594/aws_boot_flash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/15205594/aws_boot_flash.o.d" -o ${OBJECTDIR}/_ext/15205594/aws_boot_flash.o ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_flash.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/15205594/aws_boot_nvm.o: ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_nvm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/15205594" 
	@${RM} ${OBJECTDIR}/_ext/15205594/aws_boot_nvm.o.d 
	@${RM} ${OBJECTDIR}/_ext/15205594/aws_boot_nvm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/15205594/aws_boot_nvm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/15205594/aws_boot_nvm.o.d" -o ${OBJECTDIR}/_ext/15205594/aws_boot_nvm.o ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_nvm.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/15205594/aws_boot_partition.o: ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_partition.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/15205594" 
	@${RM} ${OBJECTDIR}/_ext/15205594/aws_boot_partition.o.d 
	@${RM} ${OBJECTDIR}/_ext/15205594/aws_boot_partition.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/15205594/aws_boot_partition.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/15205594/aws_boot_partition.o.d" -o ${OBJECTDIR}/_ext/15205594/aws_boot_partition.o ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_partition.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/645904489/aws_boot_loader.o: ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/loader/aws_boot_loader.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/645904489" 
	@${RM} ${OBJECTDIR}/_ext/645904489/aws_boot_loader.o.d 
	@${RM} ${OBJECTDIR}/_ext/645904489/aws_boot_loader.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/645904489/aws_boot_loader.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/645904489/aws_boot_loader.o.d" -o ${OBJECTDIR}/_ext/645904489/aws_boot_loader.o ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/loader/aws_boot_loader.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/645904489/aws_boot_pal.o: ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/loader/aws_boot_pal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/645904489" 
	@${RM} ${OBJECTDIR}/_ext/645904489/aws_boot_pal.o.d 
	@${RM} ${OBJECTDIR}/_ext/645904489/aws_boot_pal.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/645904489/aws_boot_pal.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/645904489/aws_boot_pal.o.d" -o ${OBJECTDIR}/_ext/645904489/aws_boot_pal.o ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/loader/aws_boot_pal.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1657551691/app.o: ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/source/app.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1657551691" 
	@${RM} ${OBJECTDIR}/_ext/1657551691/app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1657551691/app.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1657551691/app.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1657551691/app.o.d" -o ${OBJECTDIR}/_ext/1657551691/app.o ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/source/app.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1530740410/bsp.o: ../src/config/pic32mzef_nvm/bsp/bsp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1530740410" 
	@${RM} ${OBJECTDIR}/_ext/1530740410/bsp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1530740410/bsp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1530740410/bsp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1530740410/bsp.o.d" -o ${OBJECTDIR}/_ext/1530740410/bsp.o ../src/config/pic32mzef_nvm/bsp/bsp.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1950090572/plib_clk.o: ../src/config/pic32mzef_nvm/peripheral/clk/plib_clk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1950090572" 
	@${RM} ${OBJECTDIR}/_ext/1950090572/plib_clk.o.d 
	@${RM} ${OBJECTDIR}/_ext/1950090572/plib_clk.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1950090572/plib_clk.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1950090572/plib_clk.o.d" -o ${OBJECTDIR}/_ext/1950090572/plib_clk.o ../src/config/pic32mzef_nvm/peripheral/clk/plib_clk.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/323196359/plib_evic.o: ../src/config/pic32mzef_nvm/peripheral/evic/plib_evic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/323196359" 
	@${RM} ${OBJECTDIR}/_ext/323196359/plib_evic.o.d 
	@${RM} ${OBJECTDIR}/_ext/323196359/plib_evic.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/323196359/plib_evic.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/323196359/plib_evic.o.d" -o ${OBJECTDIR}/_ext/323196359/plib_evic.o ../src/config/pic32mzef_nvm/peripheral/evic/plib_evic.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/323142531/plib_gpio.o: ../src/config/pic32mzef_nvm/peripheral/gpio/plib_gpio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/323142531" 
	@${RM} ${OBJECTDIR}/_ext/323142531/plib_gpio.o.d 
	@${RM} ${OBJECTDIR}/_ext/323142531/plib_gpio.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/323142531/plib_gpio.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/323142531/plib_gpio.o.d" -o ${OBJECTDIR}/_ext/323142531/plib_gpio.o ../src/config/pic32mzef_nvm/peripheral/gpio/plib_gpio.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1950079689/plib_nvm.o: ../src/config/pic32mzef_nvm/peripheral/nvm/plib_nvm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1950079689" 
	@${RM} ${OBJECTDIR}/_ext/1950079689/plib_nvm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1950079689/plib_nvm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1950079689/plib_nvm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1950079689/plib_nvm.o.d" -o ${OBJECTDIR}/_ext/1950079689/plib_nvm.o ../src/config/pic32mzef_nvm/peripheral/nvm/plib_nvm.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/322739588/plib_uart1.o: ../src/config/pic32mzef_nvm/peripheral/uart/plib_uart1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/322739588" 
	@${RM} ${OBJECTDIR}/_ext/322739588/plib_uart1.o.d 
	@${RM} ${OBJECTDIR}/_ext/322739588/plib_uart1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/322739588/plib_uart1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/322739588/plib_uart1.o.d" -o ${OBJECTDIR}/_ext/322739588/plib_uart1.o ../src/config/pic32mzef_nvm/peripheral/uart/plib_uart1.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/2116527036/xc32_monitor.o: ../src/config/pic32mzef_nvm/stdio/xc32_monitor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2116527036" 
	@${RM} ${OBJECTDIR}/_ext/2116527036/xc32_monitor.o.d 
	@${RM} ${OBJECTDIR}/_ext/2116527036/xc32_monitor.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2116527036/xc32_monitor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/2116527036/xc32_monitor.o.d" -o ${OBJECTDIR}/_ext/2116527036/xc32_monitor.o ../src/config/pic32mzef_nvm/stdio/xc32_monitor.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/128587639/sys_int.o: ../src/config/pic32mzef_nvm/system/int/src/sys_int.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/128587639" 
	@${RM} ${OBJECTDIR}/_ext/128587639/sys_int.o.d 
	@${RM} ${OBJECTDIR}/_ext/128587639/sys_int.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/128587639/sys_int.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/128587639/sys_int.o.d" -o ${OBJECTDIR}/_ext/128587639/sys_int.o ../src/config/pic32mzef_nvm/system/int/src/sys_int.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/770344950/initialization.o: ../src/config/pic32mzef_nvm/initialization.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/770344950" 
	@${RM} ${OBJECTDIR}/_ext/770344950/initialization.o.d 
	@${RM} ${OBJECTDIR}/_ext/770344950/initialization.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/770344950/initialization.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/770344950/initialization.o.d" -o ${OBJECTDIR}/_ext/770344950/initialization.o ../src/config/pic32mzef_nvm/initialization.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/770344950/interrupts.o: ../src/config/pic32mzef_nvm/interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/770344950" 
	@${RM} ${OBJECTDIR}/_ext/770344950/interrupts.o.d 
	@${RM} ${OBJECTDIR}/_ext/770344950/interrupts.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/770344950/interrupts.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/770344950/interrupts.o.d" -o ${OBJECTDIR}/_ext/770344950/interrupts.o ../src/config/pic32mzef_nvm/interrupts.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/770344950/exceptions.o: ../src/config/pic32mzef_nvm/exceptions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/770344950" 
	@${RM} ${OBJECTDIR}/_ext/770344950/exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/770344950/exceptions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/770344950/exceptions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/770344950/exceptions.o.d" -o ${OBJECTDIR}/_ext/770344950/exceptions.o ../src/config/pic32mzef_nvm/exceptions.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1214204543/asn1parse.o: ../../../../../../../libraries/3rdparty/tinycrypt/asn1/asn1parse.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1214204543" 
	@${RM} ${OBJECTDIR}/_ext/1214204543/asn1parse.o.d 
	@${RM} ${OBJECTDIR}/_ext/1214204543/asn1parse.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1214204543/asn1parse.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1214204543/asn1parse.o.d" -o ${OBJECTDIR}/_ext/1214204543/asn1parse.o ../../../../../../../libraries/3rdparty/tinycrypt/asn1/asn1parse.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/aes_decrypt.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/aes_decrypt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/aes_decrypt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/aes_decrypt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/aes_decrypt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/aes_decrypt.o.d" -o ${OBJECTDIR}/_ext/1193839791/aes_decrypt.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/aes_decrypt.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/aes_encrypt.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/aes_encrypt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/aes_encrypt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/aes_encrypt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/aes_encrypt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/aes_encrypt.o.d" -o ${OBJECTDIR}/_ext/1193839791/aes_encrypt.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/aes_encrypt.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/cbc_mode.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/cbc_mode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/cbc_mode.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/cbc_mode.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/cbc_mode.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/cbc_mode.o.d" -o ${OBJECTDIR}/_ext/1193839791/cbc_mode.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/cbc_mode.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/ccm_mode.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ccm_mode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ccm_mode.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ccm_mode.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/ccm_mode.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/ccm_mode.o.d" -o ${OBJECTDIR}/_ext/1193839791/ccm_mode.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ccm_mode.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/cmac_mode.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/cmac_mode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/cmac_mode.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/cmac_mode.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/cmac_mode.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/cmac_mode.o.d" -o ${OBJECTDIR}/_ext/1193839791/cmac_mode.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/cmac_mode.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/ctr_mode.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ctr_mode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ctr_mode.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ctr_mode.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/ctr_mode.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/ctr_mode.o.d" -o ${OBJECTDIR}/_ext/1193839791/ctr_mode.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ctr_mode.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/ctr_prng.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ctr_prng.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ctr_prng.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ctr_prng.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/ctr_prng.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/ctr_prng.o.d" -o ${OBJECTDIR}/_ext/1193839791/ctr_prng.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ctr_prng.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/ecc.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ecc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ecc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/ecc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/ecc.o.d" -o ${OBJECTDIR}/_ext/1193839791/ecc.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/ecc_dh.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_dh.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ecc_dh.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ecc_dh.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/ecc_dh.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/ecc_dh.o.d" -o ${OBJECTDIR}/_ext/1193839791/ecc_dh.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_dh.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/ecc_dsa.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_dsa.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ecc_dsa.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ecc_dsa.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/ecc_dsa.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/ecc_dsa.o.d" -o ${OBJECTDIR}/_ext/1193839791/ecc_dsa.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_dsa.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/ecc_platform_specific.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_platform_specific.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ecc_platform_specific.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ecc_platform_specific.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/ecc_platform_specific.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/ecc_platform_specific.o.d" -o ${OBJECTDIR}/_ext/1193839791/ecc_platform_specific.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_platform_specific.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/hmac.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/hmac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/hmac.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/hmac.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/hmac.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/hmac.o.d" -o ${OBJECTDIR}/_ext/1193839791/hmac.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/hmac.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/hmac_prng.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/hmac_prng.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/hmac_prng.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/hmac_prng.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/hmac_prng.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/hmac_prng.o.d" -o ${OBJECTDIR}/_ext/1193839791/hmac_prng.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/hmac_prng.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/sha256.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/sha256.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/sha256.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/sha256.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/sha256.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/sha256.o.d" -o ${OBJECTDIR}/_ext/1193839791/sha256.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/sha256.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/utils.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/utils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/utils.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/utils.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/utils.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/utils.o.d" -o ${OBJECTDIR}/_ext/1193839791/utils.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/utils.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
else
${OBJECTDIR}/_ext/671897462/asn1utility.o: ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/crypto/tinycrypt/asn1utility.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/671897462" 
	@${RM} ${OBJECTDIR}/_ext/671897462/asn1utility.o.d 
	@${RM} ${OBJECTDIR}/_ext/671897462/asn1utility.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671897462/asn1utility.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/671897462/asn1utility.o.d" -o ${OBJECTDIR}/_ext/671897462/asn1utility.o ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/crypto/tinycrypt/asn1utility.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/671897462/aws_boot_crypto.o: ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/crypto/tinycrypt/aws_boot_crypto.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/671897462" 
	@${RM} ${OBJECTDIR}/_ext/671897462/aws_boot_crypto.o.d 
	@${RM} ${OBJECTDIR}/_ext/671897462/aws_boot_crypto.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/671897462/aws_boot_crypto.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/671897462/aws_boot_crypto.o.d" -o ${OBJECTDIR}/_ext/671897462/aws_boot_crypto.o ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/crypto/tinycrypt/aws_boot_crypto.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/15205594/aws_boot_flash.o: ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/15205594" 
	@${RM} ${OBJECTDIR}/_ext/15205594/aws_boot_flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/15205594/aws_boot_flash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/15205594/aws_boot_flash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/15205594/aws_boot_flash.o.d" -o ${OBJECTDIR}/_ext/15205594/aws_boot_flash.o ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_flash.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/15205594/aws_boot_nvm.o: ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_nvm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/15205594" 
	@${RM} ${OBJECTDIR}/_ext/15205594/aws_boot_nvm.o.d 
	@${RM} ${OBJECTDIR}/_ext/15205594/aws_boot_nvm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/15205594/aws_boot_nvm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/15205594/aws_boot_nvm.o.d" -o ${OBJECTDIR}/_ext/15205594/aws_boot_nvm.o ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_nvm.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/15205594/aws_boot_partition.o: ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_partition.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/15205594" 
	@${RM} ${OBJECTDIR}/_ext/15205594/aws_boot_partition.o.d 
	@${RM} ${OBJECTDIR}/_ext/15205594/aws_boot_partition.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/15205594/aws_boot_partition.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/15205594/aws_boot_partition.o.d" -o ${OBJECTDIR}/_ext/15205594/aws_boot_partition.o ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash/aws_boot_partition.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/645904489/aws_boot_loader.o: ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/loader/aws_boot_loader.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/645904489" 
	@${RM} ${OBJECTDIR}/_ext/645904489/aws_boot_loader.o.d 
	@${RM} ${OBJECTDIR}/_ext/645904489/aws_boot_loader.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/645904489/aws_boot_loader.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/645904489/aws_boot_loader.o.d" -o ${OBJECTDIR}/_ext/645904489/aws_boot_loader.o ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/loader/aws_boot_loader.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/645904489/aws_boot_pal.o: ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/loader/aws_boot_pal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/645904489" 
	@${RM} ${OBJECTDIR}/_ext/645904489/aws_boot_pal.o.d 
	@${RM} ${OBJECTDIR}/_ext/645904489/aws_boot_pal.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/645904489/aws_boot_pal.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/645904489/aws_boot_pal.o.d" -o ${OBJECTDIR}/_ext/645904489/aws_boot_pal.o ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/loader/aws_boot_pal.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1657551691/app.o: ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/source/app.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1657551691" 
	@${RM} ${OBJECTDIR}/_ext/1657551691/app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1657551691/app.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1657551691/app.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1657551691/app.o.d" -o ${OBJECTDIR}/_ext/1657551691/app.o ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/source/app.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1530740410/bsp.o: ../src/config/pic32mzef_nvm/bsp/bsp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1530740410" 
	@${RM} ${OBJECTDIR}/_ext/1530740410/bsp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1530740410/bsp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1530740410/bsp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1530740410/bsp.o.d" -o ${OBJECTDIR}/_ext/1530740410/bsp.o ../src/config/pic32mzef_nvm/bsp/bsp.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1950090572/plib_clk.o: ../src/config/pic32mzef_nvm/peripheral/clk/plib_clk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1950090572" 
	@${RM} ${OBJECTDIR}/_ext/1950090572/plib_clk.o.d 
	@${RM} ${OBJECTDIR}/_ext/1950090572/plib_clk.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1950090572/plib_clk.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1950090572/plib_clk.o.d" -o ${OBJECTDIR}/_ext/1950090572/plib_clk.o ../src/config/pic32mzef_nvm/peripheral/clk/plib_clk.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/323196359/plib_evic.o: ../src/config/pic32mzef_nvm/peripheral/evic/plib_evic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/323196359" 
	@${RM} ${OBJECTDIR}/_ext/323196359/plib_evic.o.d 
	@${RM} ${OBJECTDIR}/_ext/323196359/plib_evic.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/323196359/plib_evic.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/323196359/plib_evic.o.d" -o ${OBJECTDIR}/_ext/323196359/plib_evic.o ../src/config/pic32mzef_nvm/peripheral/evic/plib_evic.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/323142531/plib_gpio.o: ../src/config/pic32mzef_nvm/peripheral/gpio/plib_gpio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/323142531" 
	@${RM} ${OBJECTDIR}/_ext/323142531/plib_gpio.o.d 
	@${RM} ${OBJECTDIR}/_ext/323142531/plib_gpio.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/323142531/plib_gpio.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/323142531/plib_gpio.o.d" -o ${OBJECTDIR}/_ext/323142531/plib_gpio.o ../src/config/pic32mzef_nvm/peripheral/gpio/plib_gpio.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1950079689/plib_nvm.o: ../src/config/pic32mzef_nvm/peripheral/nvm/plib_nvm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1950079689" 
	@${RM} ${OBJECTDIR}/_ext/1950079689/plib_nvm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1950079689/plib_nvm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1950079689/plib_nvm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1950079689/plib_nvm.o.d" -o ${OBJECTDIR}/_ext/1950079689/plib_nvm.o ../src/config/pic32mzef_nvm/peripheral/nvm/plib_nvm.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/322739588/plib_uart1.o: ../src/config/pic32mzef_nvm/peripheral/uart/plib_uart1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/322739588" 
	@${RM} ${OBJECTDIR}/_ext/322739588/plib_uart1.o.d 
	@${RM} ${OBJECTDIR}/_ext/322739588/plib_uart1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/322739588/plib_uart1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/322739588/plib_uart1.o.d" -o ${OBJECTDIR}/_ext/322739588/plib_uart1.o ../src/config/pic32mzef_nvm/peripheral/uart/plib_uart1.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/2116527036/xc32_monitor.o: ../src/config/pic32mzef_nvm/stdio/xc32_monitor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2116527036" 
	@${RM} ${OBJECTDIR}/_ext/2116527036/xc32_monitor.o.d 
	@${RM} ${OBJECTDIR}/_ext/2116527036/xc32_monitor.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2116527036/xc32_monitor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/2116527036/xc32_monitor.o.d" -o ${OBJECTDIR}/_ext/2116527036/xc32_monitor.o ../src/config/pic32mzef_nvm/stdio/xc32_monitor.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/128587639/sys_int.o: ../src/config/pic32mzef_nvm/system/int/src/sys_int.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/128587639" 
	@${RM} ${OBJECTDIR}/_ext/128587639/sys_int.o.d 
	@${RM} ${OBJECTDIR}/_ext/128587639/sys_int.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/128587639/sys_int.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/128587639/sys_int.o.d" -o ${OBJECTDIR}/_ext/128587639/sys_int.o ../src/config/pic32mzef_nvm/system/int/src/sys_int.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/770344950/initialization.o: ../src/config/pic32mzef_nvm/initialization.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/770344950" 
	@${RM} ${OBJECTDIR}/_ext/770344950/initialization.o.d 
	@${RM} ${OBJECTDIR}/_ext/770344950/initialization.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/770344950/initialization.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/770344950/initialization.o.d" -o ${OBJECTDIR}/_ext/770344950/initialization.o ../src/config/pic32mzef_nvm/initialization.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/770344950/interrupts.o: ../src/config/pic32mzef_nvm/interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/770344950" 
	@${RM} ${OBJECTDIR}/_ext/770344950/interrupts.o.d 
	@${RM} ${OBJECTDIR}/_ext/770344950/interrupts.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/770344950/interrupts.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/770344950/interrupts.o.d" -o ${OBJECTDIR}/_ext/770344950/interrupts.o ../src/config/pic32mzef_nvm/interrupts.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/770344950/exceptions.o: ../src/config/pic32mzef_nvm/exceptions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/770344950" 
	@${RM} ${OBJECTDIR}/_ext/770344950/exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/770344950/exceptions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/770344950/exceptions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/770344950/exceptions.o.d" -o ${OBJECTDIR}/_ext/770344950/exceptions.o ../src/config/pic32mzef_nvm/exceptions.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1214204543/asn1parse.o: ../../../../../../../libraries/3rdparty/tinycrypt/asn1/asn1parse.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1214204543" 
	@${RM} ${OBJECTDIR}/_ext/1214204543/asn1parse.o.d 
	@${RM} ${OBJECTDIR}/_ext/1214204543/asn1parse.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1214204543/asn1parse.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1214204543/asn1parse.o.d" -o ${OBJECTDIR}/_ext/1214204543/asn1parse.o ../../../../../../../libraries/3rdparty/tinycrypt/asn1/asn1parse.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/aes_decrypt.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/aes_decrypt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/aes_decrypt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/aes_decrypt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/aes_decrypt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/aes_decrypt.o.d" -o ${OBJECTDIR}/_ext/1193839791/aes_decrypt.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/aes_decrypt.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/aes_encrypt.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/aes_encrypt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/aes_encrypt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/aes_encrypt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/aes_encrypt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/aes_encrypt.o.d" -o ${OBJECTDIR}/_ext/1193839791/aes_encrypt.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/aes_encrypt.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/cbc_mode.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/cbc_mode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/cbc_mode.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/cbc_mode.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/cbc_mode.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/cbc_mode.o.d" -o ${OBJECTDIR}/_ext/1193839791/cbc_mode.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/cbc_mode.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/ccm_mode.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ccm_mode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ccm_mode.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ccm_mode.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/ccm_mode.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/ccm_mode.o.d" -o ${OBJECTDIR}/_ext/1193839791/ccm_mode.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ccm_mode.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/cmac_mode.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/cmac_mode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/cmac_mode.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/cmac_mode.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/cmac_mode.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/cmac_mode.o.d" -o ${OBJECTDIR}/_ext/1193839791/cmac_mode.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/cmac_mode.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/ctr_mode.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ctr_mode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ctr_mode.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ctr_mode.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/ctr_mode.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/ctr_mode.o.d" -o ${OBJECTDIR}/_ext/1193839791/ctr_mode.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ctr_mode.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/ctr_prng.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ctr_prng.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ctr_prng.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ctr_prng.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/ctr_prng.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/ctr_prng.o.d" -o ${OBJECTDIR}/_ext/1193839791/ctr_prng.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ctr_prng.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/ecc.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ecc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ecc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/ecc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/ecc.o.d" -o ${OBJECTDIR}/_ext/1193839791/ecc.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/ecc_dh.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_dh.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ecc_dh.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ecc_dh.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/ecc_dh.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/ecc_dh.o.d" -o ${OBJECTDIR}/_ext/1193839791/ecc_dh.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_dh.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/ecc_dsa.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_dsa.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ecc_dsa.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ecc_dsa.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/ecc_dsa.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/ecc_dsa.o.d" -o ${OBJECTDIR}/_ext/1193839791/ecc_dsa.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_dsa.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/ecc_platform_specific.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_platform_specific.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ecc_platform_specific.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/ecc_platform_specific.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/ecc_platform_specific.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/ecc_platform_specific.o.d" -o ${OBJECTDIR}/_ext/1193839791/ecc_platform_specific.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/ecc_platform_specific.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/hmac.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/hmac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/hmac.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/hmac.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/hmac.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/hmac.o.d" -o ${OBJECTDIR}/_ext/1193839791/hmac.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/hmac.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/hmac_prng.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/hmac_prng.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/hmac_prng.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/hmac_prng.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/hmac_prng.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/hmac_prng.o.d" -o ${OBJECTDIR}/_ext/1193839791/hmac_prng.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/hmac_prng.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/sha256.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/sha256.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/sha256.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/sha256.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/sha256.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/sha256.o.d" -o ${OBJECTDIR}/_ext/1193839791/sha256.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/sha256.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1193839791/utils.o: ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/utils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1193839791" 
	@${RM} ${OBJECTDIR}/_ext/1193839791/utils.o.d 
	@${RM} ${OBJECTDIR}/_ext/1193839791/utils.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1193839791/utils.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1193839791/utils.o.d" -o ${OBJECTDIR}/_ext/1193839791/utils.o ../../../../../../../libraries/3rdparty/tinycrypt/lib/source/utils.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/include" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/config_files" -I"../../../../../../../libraries/3rdparty/tinycrypt/lib/include" -I"../../../../../../../libraries/3rdparty/tinycrypt/asn1" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/bootloader/flash" -I"../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/common/include" -I"../src" -I"../src/config/pic32mzef_nvm" -I"../src/packs/PIC32MZ2048EFM144_DFP" -I"../src/mips" -Wall -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -fgnu89-inline
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/linker_script/btl_mz.ld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -g   -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)   -mreserve=data@0x0:0x37F   -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D=__DEBUG_D,--defsym=_min_heap_size=512,--gc-sections,--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ../../../../../../../vendors/microchip/boards/curiosity2_pic32mzef/bootloader/linker_script/btl_mz.ld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_pic32mzef_nvm=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=512,--gc-sections,--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	${MP_CC_DIR}\\xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/pic32mzef_nvm
	${RM} -r dist/pic32mzef_nvm

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
