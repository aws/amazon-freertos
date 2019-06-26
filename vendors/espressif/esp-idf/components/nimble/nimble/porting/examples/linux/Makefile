# Toolchain commands
CROSS_COMPILE ?=
CC      := ccache $(CROSS_COMPLIE)gcc
CXX     := ccache $(CROSS_COMPILE)g++
LD      := $(CROSS_COMPILE)gcc
AR      := $(CROSS_COMPILE)ar
AS      := $(CROSS_COMPILE)as
NM      := $(CROSS_COMPILE)nm
OBJDUMP := $(CROSS_COMPILE)objdump
OBJCOPY := $(CROSS_COMPILE)objcopy
SIZE    := $(CROSS_COMPILE)size

# Configure NimBLE variables
NIMBLE_ROOT := ../../..
NIMBLE_CFG_TINYCRYPT := 1
include $(NIMBLE_ROOT)/porting/nimble/Makefile.defs

SRC := 	$(NIMBLE_SRC)

# Source files for NPL OSAL
SRC += \
	$(NIMBLE_ROOT)/porting/npl/linux/src/os_atomic.c   \
	$(NIMBLE_ROOT)/porting/npl/linux/src/os_callout.c  \
	$(NIMBLE_ROOT)/porting/npl/linux/src/os_eventq.cc  \
	$(NIMBLE_ROOT)/porting/npl/linux/src/os_mutex.c    \
	$(NIMBLE_ROOT)/porting/npl/linux/src/os_sem.c      \
	$(NIMBLE_ROOT)/porting/npl/linux/src/os_task.c     \
	$(NIMBLE_ROOT)/porting/npl/linux/src/os_time.c     \
	$(NIMBLE_ROOT)/nimble/transport/socket/src/ble_hci_socket.c \
	$(TINYCRYPT_SRC) \
	$(NULL)

# Source files for demo app
SRC += \
	./ble.c \
	./main.c \
	$(NULL)

# Add NPL and all NimBLE directories to include paths
INC = \
    ./include \
	$(NIMBLE_ROOT)/porting/npl/linux/include \
	$(NIMBLE_ROOT)/porting/npl/linux/src \
	$(NIMBLE_ROOT)/nimble/transport/socket/include \
	$(NIMBLE_INCLUDE) \
	$(TINYCRYPT_INCLUDE) \
	$(NULL)

INCLUDES := $(addprefix -I, $(INC))

SRC_C  = $(filter %.c,  $(SRC))
SRC_CC = $(filter %.cc, $(SRC))

OBJ := $(SRC_C:.c=.o)
OBJ += $(SRC_CC:.cc=.o)

TINYCRYPT_OBJ := $(TINYCRYPT_SRC:.c=.o)

CFLAGS =                    \
    $(NIMBLE_CFLAGS)        \
    $(INCLUDES)             \
    -g                      \
    -D_GNU_SOURCE           \
    $(NULL)

LIBS := -lrt -lpthread -lstdc++

.PHONY: all clean
.DEFAULT: all

all: nimble-linux

clean:
	rm $(OBJ) -f
	rm dummy -f

$(TINYCRYPT_OBJ): CFLAGS+=$(TINYCRYPT_CFLAGS)

%.o: %.c
	$(CC) -c $(INCLUDES) $(CFLAGS) -o $@ $<

%.o: %.cc
	$(CXX) -c $(INCLUDES) $(CFLAGS) -o $@ $<

nimble-linux: $(OBJ) $(TINYCRYPT_OBJ)
	$(LD) -o $@ $^ $(LIBS)
	$(SIZE) $@
