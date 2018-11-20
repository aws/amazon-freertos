################################################################
# ReadNameField

ENTRY=ReadNameField
UNWIND_GOTO=0

# This is the network buffer size.  Set to any positive value.
NETWORK_BUFFER_SIZE = 10

# This is the size of the buffer into which the name is copied.
# Set to any positive value.
# In the source, NAME_SIZE=254 and NETWORK_BUFFER_SIZE >> NAME_SIZE
NAME_SIZE = 10

################################################################

min = $(shell echo "if ($(1) < $(2)) $(1) else $(2)" | bc)
max = $(shell echo "if ($(1) > $(2)) $(1) else $(2)" | bc)

################################################################
# Loop prvReadNameField.0:
#   file lib/FreeRTOS-Plus-TCP/source/FreeRTOS_DNS.c line 725
# should be min of buffer size and name size
# but loop must be unwound at least once, so max of this and 1+1
READLOOP0=prvReadNameField.0
READLOOP0_UNWIND=$(call max,2,$(call min,$(NETWORK_BUFFER_SIZE),$(NAME_SIZE)))

################################################################
# Loop prvReadNameField.1:
#   file lib/FreeRTOS-Plus-TCP/source/FreeRTOS_DNS.c line 715
# should be min of buffer size and name size
# but loop must be unwound at least twice, so max of this and 2+1
READLOOP1=prvReadNameField.1
#READLOOP1_UNWIND=31
READLOOP1_UNWIND=$(call max,3,$(call min,$(NETWORK_BUFFER_SIZE),$(NAME_SIZE)))

################################################################

UNWINDING = \
	--unwind $(UNWIND) \
	--unwindset $(READLOOP0):$(READLOOP0_UNWIND),$(READLOOP1):$(READLOOP1_UNWIND) \
	--unwinding-assertions

OBJS = \
	$(ENTRY)_harness.goto \
	$(FREERTOS)/lib/FreeRTOS-Plus-TCP/source/FreeRTOS_DNS.goto \

DEF += \
    -DNETWORK_BUFFER_SIZE=$(NETWORK_BUFFER_SIZE) \
    -DNAME_SIZE=$(NAME_SIZE) \

include ../Makefile.common

