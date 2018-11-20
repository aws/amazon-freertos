################################################################
# CheckOptionsInner

ENTRY=CheckOptionsInner
UNWIND_GOTO=0

BUFFER_SIZE = 100

# Without goto-analyzer
# Loop prvTCPWindowTxCheckAck.0:
#   file /Users/mrtuttle/freertos/lib/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_WIN.c line 1545 function prvTCPWindowTxCheckAck
# Loop prvTCPWindowTxCheckAck.1:
#   file /Users/mrtuttle/freertos/lib/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_WIN.c line 1459 function prvTCPWindowTxCheckAck
#
# Loop prvTCPWindowFastRetransmit.0:
#   file /Users/mrtuttle/freertos/lib/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_WIN.c line 1616 function prvTCPWindowFastRetransmit
# Loop prvTCPWindowFastRetransmit.1:
#   file /Users/mrtuttle/freertos/lib/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_WIN.c line 1619 function prvTCPWindowFastRetransmit
# Loop prvTCPWindowFastRetransmit.2:
#   file /Users/mrtuttle/freertos/lib/FreeRTOS-Plus-TCP/source/FreeRTOS_TCP_WIN.c line 1593 function prvTCPWindowFastRetransmit

ifndef SIMPLIFY
SIMPLIFY=0
endif

# Without goto-analyzer
ifeq ($(SIMPLIFY),0)
UNWINDING = \
	--unwind $(UNWIND) \
	--unwindset prvTCPWindowTxCheckAck.1:2,prvTCPWindowFastRetransmit.2:2 \
	--unwinding-assertions \

endif

# With goto-analyzer
ifeq ($(SIMPLIFY),1)
UNWINDING = \
	--unwind $(UNWIND) \
	--unwindset prvTCPWindowTxCheckAck.0:2,prvTCPWindowFastRetransmit.0:2 \
	--unwinding-assertions \

endif

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

include ../Makefile.common
