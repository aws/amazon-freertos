exec-y += test_message
global-cflags-y += -I$(d)/../../../../../tests/include \
				 -I$(d)/../../../../../freertos_kernel/include \
				 -I$(d)/../../../../../freertos_kernel/include/private \
				 -I$(d)/../../../../../vendors/marvell/WMSDK/mw320/sdk/src/incl/platform/os/freertos/ \
				 -I$(d)/../../../../../freertos_kernel/portable/GCC/ARM_CM4F \
				 -I$(d)/../../../../../libraries/3rdparty/unity/src/ \
				 -I$(d)/../../../../../libraries/3rdparty/unity/extras/fixture/src \
				 -I$(d)/../../../../../libraries/3rdparty/mbedtls/include/ \
				 -I$(d)/../../../../../libraries/abstractions/pkcs11/mbedtls/ \
				 -I$(d)/../../../../../libraries/abstractions/platform/freertos/include/ \
				 -I$(d)/../../../../../vendors/marvell/boards/mw300_rd/aws_tests/config_files/ \
				 -I$(d)/../../../../../vendors/marvell/boards/mw300_rd/aws_demos/config_files/ \
				 -I$(d)/../../../../../libraries/freertos_plus/standard/freertos_plus_tcp/include/ \
				 -I$(d)/../../../../../libraries/freertos_plus/standard/freertos_plus_tcp/source/portable/Compiler/GCC/ \
				 -I$(d)/../../../../../libraries/freertos_plus/standard/utils/include/ \
				 -I$(d)/../../../../../libraries/freertos_plus/standard/crypto/include/ \
				 -I$(d)/../../../../../libraries/c_sdk/standard/common/include/private/ \
				 -I$(d)/../../../../../libraries/c_sdk/standard/mqtt/include/ \
				 -I$(d)/../../../../../libraries/c_sdk/standard/common/include/ \
				 -I$(d)/../../../../../libraries/abstractions/wifi/include/ \
				 -I$(d)/../../../../../demos/dev_mode_key_provisioning/include/ \
				 -I$(d)/../../../../../libraries/abstractions/pkcs11/include/ \
				 -I$(d)/../../../../../libraries/freertos_plus/standard/pkcs11/include/\
				 -I$(d)/../../../../../libraries/abstractions/secure_sockets/include/ \
				 -I$(d)/../../../../../libraries/abstractions/platform/include/ \
				 -I$(d)/../../../../../libraries/3rdparty/pkcs11/ \
				 -I$(d)/../../../../../libraries/3rdparty/mbedtls/ \
				 -I$(d)/../../../../../libraries/3rdparty/tinycbor/src/ \
				 -I$(d)/../../../../../libraries/3rdparty/tinycrypt/ \
				 -DAMAZON_FREERTOS_ENABLE_UNIT_TESTS=1 \
				 -DUNITY_INCLUDE_CONFIG_H=1

test_message-objs-y := ../../../../../vendors/marvell/boards/mw300_rd/aws_tests/application_code/main.c \
			../../../../../tests/common/aws_test.c
