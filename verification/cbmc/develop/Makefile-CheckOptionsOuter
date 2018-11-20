################################################################
# CheckOptionsOuter

ENTRY=CheckOptionsOuter
UNWIND_GOTO=0

BUFFER_SIZE = 100

UNWINDING = \
	--unwind $(UNWIND) \
	--unwindset prvCheckOptions_outer.0:32 \
	--unwinding-assertions

ABSTRACTIONS = \

OBJS = \
	$(ENTRY)_harness.goto \
        $(FREERTOS)/lib/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_IP.goto \
        $(FREERTOS)/lib/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_WIN.goto \
        $(FREERTOS)/lib/FreeRTOS-Plus-TCP/source/FreeRTOS_Stream_Buffer.goto \
        $(FREERTOS)/lib/FreeRTOS/tasks.goto \
        $(FREERTOS)/lib/FreeRTOS/list.goto

DEF += \
	-DBUFFER_SIZE=$(BUFFER_SIZE) \

include Makefile
