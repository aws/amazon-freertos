################################################################
# SkipNameField

ENTRY=SkipNameField
UNWIND=0

# This is the network buffer size. This can be set to any positive value.
NETWORK_BUFFER_SIZE = 10

################################################################
#Loop prvSkipNameField.0:
#  file lib/FreeRTOS-Plus-TCP/source/FreeRTOS_DNS.c line 778
#  bound should be half network buffer size, since chunk length is at least 2
SKIPLOOP0 = prvSkipNameField.0

CODE0 += ($(NETWORK_BUFFER_SIZE) + 1)/2
SKIPLOOP0_UNWIND = $(shell echo "$(CODE0)" | bc)

################################################################

UNWINDING = \
	--unwind $(UNWIND) \
	--unwindset $(SKIPLOOP0):$(SKIPLOOP0_UNWIND) \
	--unwinding-assertions \

OBJS = \
	$(ENTRY)_harness.goto \
	$(FREERTOS)/lib/FreeRTOS-Plus-TCP/source/FreeRTOS_DNS.goto \

DEF = -DNETWORK_BUFFER_SIZE=$(NETWORK_BUFFER_SIZE)

include Makefile
