#
# Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 # Cypress Semiconductor Corporation. All Rights Reserved.
 # 
 # This software, associated documentation and materials ("Software")
 # is owned by Cypress Semiconductor Corporation,
 # or one of its subsidiaries ("Cypress") and is protected by and subject to
 # worldwide patent protection (United States and foreign),
 # United States copyright laws and international treaty provisions.
 # Therefore, you may use this Software only as provided in the license
 # agreement accompanying the software package from which you
 # obtained this Software ("EULA").
 # If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 # non-transferable license to copy, modify, and compile the Software
 # source code solely for use in connection with Cypress's
 # integrated circuit products. Any reproduction, modification, translation,
 # compilation, or representation of this Software except as specified
 # above is prohibited without the express written permission of Cypress.
 #
 # Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 # EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 # WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 # reserves the right to make changes to the Software without notice. Cypress
 # does not assume any liability arising out of the application or use of the
 # Software or any product or circuit described in the Software. Cypress does
 # not authorize its products for use in any products where a malfunction or
 # failure of the Cypress product may reasonably be expected to result in
 # significant property damage, injury or death ("High Risk Product"). By
 # including Cypress's product in a High Risk Product, the manufacturer
 # of such system or application assumes all risk of such use and in doing
 # so agrees to indemnify Cypress against all liability.
#

#
# Tester for WicedFS
#

NAME := wicedfs_test


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
CFLAGS += -I/include/
CFLAGS += -D$(HOST)
CFLAGS += -I.
CFLAGS += -I..
CFLAGS += -I../mk_wicedfs/

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
SOURCES         := ../wicedfs.c \
                   ../mk_wicedfs/wicedfs_create.c \
                  tester.c




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
	 rm -rfv $(EXE32_NAME) $(EXE64_NAME) $(OBJS) v32_obj v64_obj image.bin

