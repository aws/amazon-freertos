################################################################
# ProcessDHCPReplies
#
# The proof depends on one parameters
#   BUFFER_SIZE is the size of the buffer being parsed
#
# We need to know the size of the buffers (the buffer size cannot be
# unconstrained) because the number of loop iterations that must be
# unwound depends on it.
# In the code, useful value is BUFFER_SIZE > 241 = sizeof(DHCHMessage_t))
#

ENTRY=ProcessDHCPReplies

# This is the network buffer size.
# Reasonable values are size > 241 = sizeof(DHCPMessage_t))
# Size smaller than this causes CBMC to fail in simplify_byte_extract
BUFFER_SIZE = 300

# goto-analyzer --simplify currently makes some source empty and breaks coverage calculation
ifndef SIMPLIFY
SIMPLIFY=0
endif

################################################################
# Loop names in original code
# Loop memcmp.0:
#   file <builtin-library-memcmp> line 25 function memcmp
# Loop prvProcessDHCPReplies.0:
#   file /Users/mrtuttle/freertos/lib/FreeRTOS-Plus-TCP/source/FreeRTOS_DHCP.c line 661 function prvProcessDHCPReplies
#
# Loop names in code after goto-analyzer
# Loop memcmp.0:
#   file <builtin-library-memcmp> line 25 function memcmp
# Loop prvProcessDHCPReplies.0:
#   file <builtin-library-memcmp> line 25 function memcmp
# Loop prvProcessDHCPReplies.1:
#   file /Users/mrtuttle/freertos/lib/FreeRTOS-Plus-TCP/source/FreeRTOS_DHCP.c line 661 function prvProcessDHCPReplies

# sizeof(DHCHMessage_t) = 241
BUFFER_HEADER = 241

CODE += if ($(BUFFER_SIZE) <= $(BUFFER_HEADER))
CODE +=   1
CODE += else
CODE +=   $(BUFFER_SIZE) - $(BUFFER_HEADER)

BUFFER_PAYLOAD = $(shell echo "$(CODE)" | bc)

# Unwinding without goto-analyzer
ifeq ($(SIMPLIFY),0)
UNWINDING = \
	--unwind $(UNWIND) \
	--unwindset memcmp.0:7,prvProcessDHCPReplies.0:$(BUFFER_PAYLOAD) \
	--unwinding-assertions \

endif

# Unwinding with goto-analyzer
ifeq ($(SIMPLIFY),1)
UNWINDING = \
	--unwind $(UNWIND) \
	--unwindset prvProcessDHCPReplies.0:7,prvProcessDHCPReplies.1:$(BUFFER_PAYLOAD) \
	--unwinding-assertions \

endif

OBJS = \
	$(ENTRY)_harness.goto \
        $(FREERTOS)/lib/FreeRTOS-Plus-TCP/source/FreeRTOS_DHCP.goto \
        $(FREERTOS)/lib/FreeRTOS-Plus-TCP/source/portable/BufferManagement/BufferAllocation_2.goto \
        $(FREERTOS)/lib/FreeRTOS/event_groups.goto \
        $(FREERTOS)/lib/FreeRTOS/list.goto

DEF = -DBUFFER_SIZE=$(BUFFER_SIZE)

include ../Makefile.common
