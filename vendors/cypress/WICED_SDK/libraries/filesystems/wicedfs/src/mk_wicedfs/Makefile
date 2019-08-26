#
# $ Copyright Cypress Semiconductor $
#

#
# Filesystem creation tool for WicedFS
#

NAME := mk_wicedfs


# Determine what host platform is being used
UNAME = $(shell uname)
ifeq ($(findstring CYGWIN,$(UNAME)),CYGWIN)
HOST:=WIN32
CFLAGS += -I/include
EXECUTABLE_EXTENSION:=.exe
else # UNAME
ifeq ($(findstring MINGW,$(UNAME)),MINGW)
CFLAGS += -I/mingw/include
HOST:=WIN32
EXECUTABLE_EXTENSION:=.exe
else # UNAME
HOST:=LINUX
EXECUTABLE_EXTENSION:=
endif
endif


CC:=gcc

# Add some includes and define
CFLAGS += -D$(HOST)
CFLAGS += -I.
CFLAGS += -I..

# Enable Debug, disable optimisation
CFLAGS += -g -O0

# Flags to enable fseeko64/ftello64 on Linux
CFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -D__USE_LARGEFILE64

# Add pedantic warning flags
CFLAGS += -Wall -Werror -W -Wstrict-prototypes -Wshadow  -Wwrite-strings -std=gnu99 -pedantic -Wno-format -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef

# Flags for 32/64 bit support builds - 32 bit is default, hence no defines
CFLAGS_32 +=
CFLAGS_64 += -DWICEDFS_64BIT_LENGTHS


# Macro to convert a C filename into an Object filename
# $(1) is the C source filename
GET_OBJ_FN = $(patsubst %.c,obj/%.o,$(notdir $(1)))


# Macro to create target to compile C source files
# $(1) is the C source filename, $(2) number of bits
define CREATE_C_TARGET

v$(2)_$(call GET_OBJ_FN,$(1)): $(1)
	mkdir -pv $$(@D)
	$(CC) -c $(CFLAGS) $(CFLAGS_$(2)) -o $$@ $$^

$(eval OBJS_$(2) += v$(2)_$(call GET_OBJ_FN,$(1)))

endef


# List of source files
SOURCES         := mk_wicedfs.c \
                   wicedfs_create.c


# Create Executable names
EXE32_NAME          := $(NAME)32$(EXECUTABLE_EXTENSION)
EXE64_NAME          := $(NAME)64$(EXECUTABLE_EXTENSION)


# Main target - clean first, then build 32 and 64 bit versions
all: clean $(EXE32_NAME) $(EXE64_NAME)

# Create the targets for compiling C files
$(foreach SRC, $(SOURCES), $(eval $(call CREATE_C_TARGET,$(SRC),32)))
$(foreach SRC, $(SOURCES), $(eval $(call CREATE_C_TARGET,$(SRC),64)))


# Final link targets
$(EXE32_NAME): $(OBJS_32)
	$(CC) $(LDFLAGS) -o $@ $^

$(EXE64_NAME): $(OBJS_64)
	$(CC) $(LDFLAGS) -o $@ $^


.PHONY: all clean

clean:
	 rm -rfv $(EXE32_NAME) $(EXE64_NAME) $(OBJS) v32_obj v64_obj

