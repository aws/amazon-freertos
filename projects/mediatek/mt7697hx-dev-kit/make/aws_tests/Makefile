#
# verbose build or less build log
#


V          ?= 0


##############################################################################
#
# Disassemble after ELF file is linked
#
##############################################################################


ASM        ?= n


##############################################################################
#
# build log output
#
##############################################################################


ifeq ($(V),0)
Q           = @
else
Q           =
endif


include feature.mk


##############################################################################
#
# Directories Relative Paths
#
##############################################################################


ROOT_DIR        = ./../../../..

MTK_BUILD_DIR   = out

MTK_PROJ_DIR    = $(ROOT_DIR)/tests/mediatek/mt7697hx-dev-kit/gnu
MTK_COMMON_DIR  = $(ROOT_DIR)/tests/mediatek/mt7697hx-dev-kit/common
AWS_COMMON_DIR  = $(ROOT_DIR)/tests/common
LIB_DIR         = $(ROOT_DIR)/lib

MTK_SDK_DIR     = $(LIB_DIR)/third_party/mcu_vendor/mediatek/mt7697hx-dev-kit

MTK_PREBUILT_DIR = $(MTK_SDK_DIR)/prebuilt


##############################################################################
#
# Files and Include directories
#
##############################################################################


#
# assembly and linker script are toolchain specific, keep in toolchain folder.
#

S_FILES         =   $(MTK_PROJ_DIR)/startup_mt7687.s
LD_SCRIPT       =   $(MTK_PROJ_DIR)/mt7687_flash.ld
C_FILES        +=   $(MTK_COMMON_DIR)/../gnu/syscalls.c

#
# project specific code
#

C_FLAGS        += -I$(MTK_COMMON_DIR)/config_files
C_FLAGS        += -I$(MTK_COMMON_DIR)/application_code/mediatek_code/include

C_FILES        +=   $(MTK_COMMON_DIR)/application_code/main.c
C_FILES        +=   $(MTK_COMMON_DIR)/application_code/lwipopts.c
ifeq ($(MTK_MINICLI_ENABLE),y)
C_FILES        +=   $(MTK_COMMON_DIR)/application_code/mediatek_code/source/cli.c
endif
C_FILES        +=   $(MTK_COMMON_DIR)/application_code/mediatek_code/source/ept_eint_var.c
C_FILES        +=   $(MTK_COMMON_DIR)/application_code/mediatek_code/source/ept_gpio_var.c
C_FILES        +=   $(MTK_COMMON_DIR)/application_code/mediatek_code/source/sys_init.c
C_FILES        +=   $(MTK_COMMON_DIR)/application_code/mediatek_code/source/system_mt7687.c

#
# Amazon FreeRTOS Library
#

C_FLAGS        += -I$(LIB_DIR)/include
C_FLAGS        += -I$(LIB_DIR)/include/private

C_FLAGS        += -I$(LIB_DIR)/FreeRTOS/portable/GCC/ARM_CM4F
C_FILES        +=   $(LIB_DIR)/FreeRTOS/tasks.c
C_FILES        +=   $(LIB_DIR)/FreeRTOS/list.c
C_FILES        +=   $(LIB_DIR)/FreeRTOS/queue.c
C_FILES        +=   $(LIB_DIR)/FreeRTOS/stream_buffer.c
C_FILES        +=   $(LIB_DIR)/FreeRTOS/timers.c
C_FILES        +=   $(LIB_DIR)/FreeRTOS/event_groups.c

# purposely removed to be included by heap_wrap.c
#C_FILES        +=   $(LIB_DIR)/FreeRTOS/portable/MemMang/heap_4.c

C_FILES        +=   $(LIB_DIR)/bufferpool/aws_bufferpool_static_thread_safe.c
C_FILES        +=   $(LIB_DIR)/crypto/aws_crypto.c
C_FILES        +=   $(LIB_DIR)/greengrass/aws_greengrass_discovery.c
C_FILES        +=   $(LIB_DIR)/greengrass/aws_helper_secure_connect.c
C_FILES        +=   $(LIB_DIR)/mqtt/aws_mqtt_agent.c
C_FILES        +=   $(LIB_DIR)/mqtt/aws_mqtt_lib.c

C_FLAGS        += -I$(LIB_DIR)/third_party/pkcs11
C_FILES        +=   $(LIB_DIR)/pkcs11/portable/mediatek/mt7697hx-dev-kit/aws_pkcs11_pal.c
C_FILES        +=   $(LIB_DIR)/pkcs11/mbedtls/aws_pkcs11_mbedtls.c

C_FILES        +=   $(LIB_DIR)/secure_sockets/portable/lwip/aws_secure_sockets.c
C_FILES        +=   $(LIB_DIR)/shadow/aws_shadow.c
C_FILES        +=   $(LIB_DIR)/shadow/aws_shadow_json.c

C_FILES        +=   $(LIB_DIR)/tls/aws_tls.c
C_FILES        +=   $(LIB_DIR)/utils/aws_system_init.c
C_FILES        +=   $(LIB_DIR)/wifi/portable/mediatek/mt7697hx-dev-kit/aws_wifi.c

C_FLAGS        += -I$(LIB_DIR)/third_party/jsmn
C_FILES        +=   $(LIB_DIR)/third_party/jsmn/jsmn.c

C_FLAGS        += -I$(LIB_DIR)/third_party/mbedtls/include
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/aes.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/aesni.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/arc4.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/asn1parse.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/asn1write.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/base64.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/bignum.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/blowfish.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/camellia.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/ccm.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/certs.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/cipher.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/cipher_wrap.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/cmac.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/ctr_drbg.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/debug.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/des.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/dhm.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/ecdh.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/ecdsa.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/ecjpake.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/ecp.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/ecp_curves.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/entropy.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/entropy_poll.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/error.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/gcm.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/havege.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/hmac_drbg.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/md.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/md2.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/md4.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/md5.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/md_wrap.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/memory_buffer_alloc.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/net_sockets.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/oid.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/padlock.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/pem.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/pk.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/pkcs12.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/pkcs5.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/pkparse.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/pkwrite.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/pk_wrap.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/platform.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/platform_util.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/ripemd160.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/rsa.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/rsa_internal.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/sha1.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/sha256.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/sha512.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/ssl_cache.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/ssl_ciphersuites.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/ssl_cli.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/ssl_cookie.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/ssl_srv.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/ssl_ticket.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/ssl_tls.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/threading.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/timing.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/version.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/version_features.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/x509.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/x509write_crt.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/x509write_csr.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/x509_create.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/x509_crl.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/x509_crt.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/x509_csr.c
C_FILES        +=   $(LIB_DIR)/third_party/mbedtls/library/xtea.c

C_FLAGS        += -I$(LIB_DIR)/third_party/tracealyzer_recorder/Include
C_FILES        +=   $(LIB_DIR)/third_party/tracealyzer_recorder/trcSnapshotRecorder.c
C_FILES        +=   $(LIB_DIR)/third_party/tracealyzer_recorder/trcKernelPort.c
C_FILES        +=   $(LIB_DIR)/third_party/tracealyzer_recorder/trcStreamingRecorder.c


#
# MediaTek SDK
#

C_FLAGS     += -DMTK_WIFI_OS_MM_OPTION=1
include $(MTK_SDK_DIR)/mtk_sdk.mk

C_FLAGS     += $(MTK_C_FLAGS)
C_FILES     += $(MTK_C_FILES)
LINK_LIBS   += $(MTK_LIBS)


##############################################################################
#
# LWIP
#
##############################################################################


C_FLAGS        += -I$(LIB_DIR)/third_party/lwip/src/portable/mediatek/include
C_FLAGS        += -I$(LIB_DIR)/third_party/lwip/src/portable

C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/portable/mediatek/netif/ethernetif.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/portable/arch/sys_arch.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/portable/mediatek/lwip_log.c

C_FLAGS        += -I$(LIB_DIR)/third_party/lwip/src/include
C_FLAGS        += -I$(LIB_DIR)/third_party/lwip/src/include/lwip
C_FLAGS        += -I$(LIB_DIR)/third_party/lwip/src/include/netif

C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/api/api_lib.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/api/api_msg.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/api/err.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/api/netbuf.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/api/netdb.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/api/netifapi.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/api/sockets.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/api/tcpip.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/def.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ip.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/dns.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/inet_chksum.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/init.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/mem.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/memp.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/netif.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/raw.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/stats.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/sys.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/tcp.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/tcp_in.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/timeouts.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/udp.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ipv4/autoip.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ipv4/dhcp.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ipv4/icmp.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ipv4/igmp.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ipv4/ip4.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ipv4/ip4_addr.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ipv4/ip4_frag.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ipv6/dhcp6.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ipv6/ethip6.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ipv6/icmp6.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ipv6/inet6.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ipv6/ip6.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ipv6/ip6_addr.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ipv6/ip6_frag.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ipv6/mld6.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ipv6/nd6.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/ipv4/etharp.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/netif/ethernet.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/pbuf.c
C_FILES        +=   $(LIB_DIR)/third_party/lwip/src/core/tcp_out.c


#
# Amazon AWS COMMON Library
#

C_FLAGS        += -I$(AWS_COMMON_DIR)/include

C_FILES        +=   $(AWS_COMMON_DIR)/framework/aws_test_framework.c
C_FILES        +=   $(AWS_COMMON_DIR)/memory_leak/aws_memory_leak.c
C_FILES        +=   $(AWS_COMMON_DIR)/test_runner/aws_test_runner.c
C_FILES        +=   $(AWS_COMMON_DIR)/crypto/aws_test_crypto.c
C_FILES        +=   $(AWS_COMMON_DIR)/greengrass/aws_test_greengrass_discovery.c
C_FILES        +=   $(AWS_COMMON_DIR)/greengrass/aws_test_helper_secure_connect.c
C_FILES        +=   $(AWS_COMMON_DIR)/mqtt/aws_test_mqtt_agent.c
C_FILES        +=   $(AWS_COMMON_DIR)/mqtt/aws_test_mqtt_lib.c
C_FILES        +=   $(AWS_COMMON_DIR)/pkcs11/aws_test_pkcs11.c
C_FILES        +=   $(AWS_COMMON_DIR)/secure_sockets/aws_test_tcp.c
C_FILES        +=   $(AWS_COMMON_DIR)/shadow/aws_test_shadow.c
C_FILES        +=   $(AWS_COMMON_DIR)/tls/aws_test_tls.c
C_FILES        +=   $(AWS_COMMON_DIR)/wifi/aws_test_wifi.c

C_FILES        +=   $(ROOT_DIR)/demos/common/logging/aws_logging_task_dynamic_buffers.c
C_FILES        +=   $(ROOT_DIR)/demos/common/devmode_key_provisioning/aws_dev_mode_key_provisioning.c


#
# Unity
#

C_FLAGS        += -DUNITY_INCLUDE_CONFIG_H
C_FLAGS        += -DAMAZON_FREERTOS_ENABLE_UNIT_TESTS

C_FLAGS        += -I$(LIB_DIR)/third_party/unity/src
C_FLAGS        += -I$(LIB_DIR)/third_party/unity/extras/fixture/src

C_FILES        +=   $(LIB_DIR)/third_party/unity/src/unity.c
C_FILES        +=   $(LIB_DIR)/third_party/unity/extras/fixture/src/unity_fixture.c


##############################################################################
#
# substitution and build rules
#
##############################################################################


C_OBJS          = $(C_FILES:./../../../../%.c=$(MTK_BUILD_DIR)/%.o)
S_OBJS          = $(S_FILES:./../../../../%.s=$(MTK_BUILD_DIR)/%.O)

$(MTK_BUILD_DIR)/%.o:   ./../../../../%.c
	$(Q)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(Q)echo CC $^
	$(Q)$(CC) -c -o $@ $^ $(C_FLAGS)

$(MTK_BUILD_DIR)/%.O:   ./../../../../%.s
	$(Q)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(Q)echo AS $^
	$(Q)$(CC) -c -o $@ $^ $(C_FLAGS)


##############################################################################
#
# external tools
#
##############################################################################


CC              = arm-none-eabi-gcc
OBJCOPY         = arm-none-eabi-objcopy
OBJDUMP         = arm-none-eabi-objdump
NM              = arm-none-eabi-nm

CPU_FLAGS       = -mlittle-endian -mthumb -mcpu=cortex-m4
FPU_FLAGS       = -fsingle-precision-constant -Wdouble-promotion -mfpu=fpv4-sp-d16 -mfloat-abi=hard

C_FLAGS         += $(CPU_FLAGS) $(FPU_FLAGS) -ffunction-sections -fdata-sections -fno-builtin -Wimplicit-function-declaration
C_FLAGS         += -gdwarf-2 -Os -Wall -fno-strict-aliasing -fno-common
C_FLAGS         += -Wall -Wimplicit-function-declaration -Werror=uninitialized -Wno-error=maybe-uninitialized -Werror=return-type
C_FLAGS         += -DPCFG_OS=2 -D_REENT_SMALL -Wno-error -Wno-switch
C_FLAGS         += -DPRODUCT_VERSION=$(PRODUCT_VERSION)
C_FLAGS         += -std=c99

LD_FLAGS        += -Wl,-T$(LD_SCRIPT) $(LINK_LIBS)
LD_FLAGS        += --specs=nano.specs -lnosys -nostartfiles -Wl,-wrap=malloc
LD_FLAGS        += -Wl,-wrap=calloc -Wl,-wrap=realloc -Wl,-wrap=free

##############################################################################
#
# dependencies
#
##############################################################################


all:            $(MTK_BUILD_DIR)/aws_tests.bin $(MTK_BUILD_DIR)/aws_tests.asm


clean:
	$(Q)for f in $(C_OBJS) $(S_OBJS) $(MTK_BUILD_DIR)/aws_tests.elf ; do if [ -f $$f ]; then rm -f $$f; fi; done


$(MTK_BUILD_DIR)/aws_tests.elf:   $(C_OBJS) $(S_OBJS)
	@echo LD $@
	$(Q)$(CC) -o $@ $^ $(C_FLAGS) $(LD_FLAGS) -Xlinker -Map=out/aws_tests.map

$(MTK_BUILD_DIR)/aws_tests.bin: $(MTK_BUILD_DIR)/aws_tests.elf
	@echo CP $@
	$(Q)$(OBJCOPY) -O binary $^ $@

$(MTK_BUILD_DIR)/aws_tests.asm: $(MTK_BUILD_DIR)/aws_tests.elf
ifeq ($(ASM),y)
	@echo DISASSEMBLE $@
	$(Q)$(OBJDUMP) -D $< > $@
endif


##############################################################################
#
# help
#
##############################################################################


help:
	@echo "use 'make all' to build and 'make clean' to remove obj files"
	@echo "note:"
	@echo "  if using GCC, please make sure you have toolchain in PATH"
	@echo "      gcc-arm-none-eabi-4_8-2014q3-20140805"
	@echo "  which can be downloaded from"
	@echo "      https://launchpad.net/gcc-arm-embedded/4.8/4.8-2014-q3-update"


##############################################################################
#
# release build
#
##############################################################################


-include release.mk
