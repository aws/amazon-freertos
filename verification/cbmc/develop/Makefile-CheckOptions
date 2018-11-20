################################################################
# CheckOptions
#
# The proof depends on no parameters: all loops are independent of
# buffer sizes.
#
# The proof depends on a simple refactoring of the code: The loop bodies
# of two nested inner and outer loops are extracted into inner and outer
# functions (a manual implementation of loop invariants).  The inner
# function (loop body), then the outer function (loop body), then the
# CheckOptions function itself are independently specified and verified,
# with specifications of nested functions used in the verification of
# the enclosing function).  The proof is driven by
# Makefile-CheckOptions, Makefile-CheckOptionsOuter,
# Makefile-CheckOptionsInner.
#

ENTRY=CheckOptions
UNWIND_GOTO=0

BUFFER_SIZE = 100

#Note prvCheckOptions.0 needs to be 41 even though buffer is 16

UNWINDING = \
	--unwind $(UNWIND) \
	--unwindset prvCheckOptions.0:41 \
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
