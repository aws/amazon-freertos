################################################################
# ParseDNSReply
#
# The proof depends on two parameters
#   NETWORK_BUFFER_SIZE is the size of the buffer being parsed
#   NAME_SIZE is the size of the buffer into which a hostname is being written
#
# We need to know the sizes of these buffers (the buffer sizes cannot be
# unconstrained) because the number of loop iterations that must be
# unwound depend on them.  The proofs are valid for any values of these
# but values in the code have NAME_SIZE=254 and NETWORK_BUFFER_SIZE>12
# and >>NAME_SIZE.
#
# The proof specifies and validates two subfunctions ReadNameField and
# SkipNameField independently, and uses these specifications in the
# proof of ParseDNSReply (a manual implementation of function
# contracts).  The proof is driven by Makefile-ParseDNSReply,
# Makefile-ReadNameField, Makefile-SkipNameField.
#

ENTRY=ParseDNSReply

# This is the network buffer size.  Reasonable values are size > 12 = sizeof(xDNSMessage)
NETWORK_BUFFER_SIZE = 40

################################################################
#Loop prvParseDNSReply.0:
#  file lib/FreeRTOS-Plus-TCP/source/FreeRTOS_DNS.c line 915
PARSELOOP0 = prvParseDNSReply.0

# Demonstrated loop bounds
# 20 -> 2
# 30 -> 4
# 40 -> 6

# M = sizeof( DNSMessage_t ) = 12
# U = sizeof( uint32_t) = 4
# Loop bound is (NETWORK_BUFFER_SIZE - M) div (U+1) + 1 tight for SIZE >= M
# Loop bound is 1 for 0 <= SIZE < M
CODE0 += if ($(NETWORK_BUFFER_SIZE) < 12)
CODE0 +=   1
CODE0 += else
CODE0 +=   ($(NETWORK_BUFFER_SIZE) - 12) / 5 + 1
PARSELOOP0_UNWIND = $(shell echo "$(CODE0)" | bc)

################################################################
#Loop prvParseDNSReply.1:
#  file lib/FreeRTOS-Plus-TCP/source/FreeRTOS_DNS.c line 989
PARSELOOP1 = prvParseDNSReply.1

# Demonstrated loop bounds
# 20 -> 9
# 30 -> 11
# 40 -> 12

# A = sizeof( DNSAnswerRecord_t ) = 10
# M = sizeof( DNSMessage_t ) = 12
# U = sizeof( uint32_t) = 4
# Loop bound is (NETWORK_BUFFER_SIZE - M - A) div (A+1) + A + 1 tight for SIZE >= M + A
# Loop bound is (NETWORK_BUFFER_SIZE - M) + 1 for M <= SIZE < M + A
# Loop bound is 1 for 0 <= SIZE < M
CODE1 += if ($(NETWORK_BUFFER_SIZE) < 12)
CODE1 +=   1
CODE1 += else if ($(NETWORK_BUFFER_SIZE) < 22)
CODE1 +=   ($(NETWORK_BUFFER_SIZE) - 12) + 1
CODE1 += else
CODE1 +=   ($(NETWORK_BUFFER_SIZE) - 12 - 10) / (11) + 11
PARSELOOP1_UNWIND = $(shell echo "$(CODE1)" | bc)

################################################################

UNWINDING = \
	--unwind $(UNWIND) \
	--unwindset $(PARSELOOP0):$(PARSELOOP0_UNWIND),$(PARSELOOP1):$(PARSELOOP1_UNWIND) \
	--unwinding-assertions \

OBJS = \
	$(ENTRY)_harness.goto \
        $(FREERTOS)/lib/FreeRTOS-Plus-TCP/source/FreeRTOS_DNS.goto \
        $(FREERTOS)/lib/FreeRTOS/tasks.goto \

DEF = -DNETWORK_BUFFER_SIZE=$(NETWORK_BUFFER_SIZE)

include ../Makefile.common
