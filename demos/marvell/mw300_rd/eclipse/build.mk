exec-y += hello_world
global-cflags-y += -I$(d)/../../../../lib/include \
				 -I$(d)/../../../../lib/include/private \
				 -I$(d)/../../../../lib/FreeRTOS/portable/GCC/ARM_CM4F \
				 -I$(d)/../../../../lib/third_party/unity/src \
				 -I$(d)/../../../../lib/third_party/unity/extras/fixture/src \
				 -I$(d)/../../../../lib/third_party/mbedtls/include \
				 -I$(d)/../../../../tests/common/ota \
				 -I$(d)/../../../../demos/common/include \
				 -I$(d)/../../../../tests/common/include \
				 -I$(d)/../../../../demos/marvell/mw300_rd/common/config_files \
				 -I$(d)/../../../../tests/marvell/mw300_rd/common/config_files \
				 -DAMAZON_FREERTOS_ENABLE_UNIT_TESTS=1 \
				 -DUNITY_INCLUDE_CONFIG_H=1

hello_world-objs-y := ../common/application_code/main.c
