exec-y += test_message
global-cflags-y += -I$(d)/../../../../tests/common/include \
				 -I$(d)/../../../../lib/include \
				 -I$(d)/../../../../lib/include/private \
				 -I$(d)/../../../../lib/FreeRTOS/portable/GCC/ARM_CM4F \
				 -I$(d)/../../../../lib/third_party/unity/src \
				 -I$(d)/../../../../lib/third_party/unity/extras/fixture/src \
				 -I$(d)/../../../../tests/marvell/mw300_rd/common/config_files \
				 -I$(d)/../../../../demos/marvell/mw300_rd/common/config_files \
				 -DAMAZON_FREERTOS_ENABLE_UNIT_TESTS=1 \
				 -DUNITY_INCLUDE_CONFIG_H=1

test_message-objs-y := ../common/application_code/main.c
