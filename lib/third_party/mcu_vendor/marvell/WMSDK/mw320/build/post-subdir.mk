# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.

# This file will be iteratively included after including every
# Makefile
# This file should santise all variables and reset them to null

# Note: All variables in this file should be defined as immediate
# variables in the rules.mk
#
# Description:
# ------------
# This file, post-subdir.mk contains following things in
# reference to exec-y and libs-y:
#
# 	Object files name handling, appending destination path.
# 	Assignment of default values, if not defined by Makefile.

### Libraries
# Fix liba-objs-y as follows
#  - include the full path to the directory
#  - change .c with .o
#  - add $(b-output-dir-y)/ as a prefix, so all the objects are in a directory together
#    note that this requires a corresponding stripping of $(b-output-dir-y)/ in the %.o:%.c rule
$(foreach l,$(libs-y),$(eval $(l)-objs-all-y := $(foreach s,$($(l)-objs-y),$(subst $(escape_let),$(escape_dir_name),$(d))/$(s))))
$(foreach l,$(libs-y),$(eval $(l)-objs-y := $(foreach s,$($(l)-objs-y),$(b-objs-output-dir-y)/$(subst $(escape_let),$(escape_dir_name),$(d))/$(s:%.c=%.o))))
$(foreach l,$(libs-y),$(eval $(l)-objs-y := $(foreach s,$($(l)-objs-y),$(s:%.S=%.o))))
$(foreach l,$(libs-y),$(eval $(l)-objs-y := $(foreach s,$($(l)-objs-y),$(s:%.cc=%.o))))
$(foreach l,$(libs-y),$(eval $(l)-objs-y := $(foreach s,$($(l)-objs-y),$(s:%.cpp=%.o))))
$(foreach l,$(libs-y),$(eval $(l)-objs-y := $(foreach s,$($(l)-objs-y),$(s:%.s=%.o))))

b-object-dir-y += $(foreach l,$(libs-y),$(sort $(dir $($(l)-objs-y))))

# Copy the libs-y in a separate collection variable
# Remove excl-lib-paths-y libraries as they are included through global-prebuilt-libs-y variable
b-libs-paths-y += $(foreach l,$(filter-out $(excl-lib-paths-y),$(libs-y)),$(b-libs-output-dir-y)/$(l).a)

# Copy the dependencies
b-deps-y += $(foreach l,$(libs-y),$(foreach s,$($(l)-objs-y),$(s:%.o=%.d)))
b-deps-y += $(foreach l,$(libs-y),$(foreach s,$($(l)-objs-y),$(s:%.o=%.o.cmd)))

$(foreach l,$(libs-y),$(eval $(l)-dir-y := $(d)))

b-deps-y += $(foreach l,$(libs-y),$(b-libs-output-dir-y)/$(l).a.cmd)

### Programs
# Board file handling
#  - use myprog-board-y if defined by myprog, otherwise assign BOARD_FILE to
#    myprog-board-y
#  - add board file to myprog-objs-y

exec-y += $(exec-cpp-y)
$(foreach l,$(exec-y),$(eval $(l)-board-y ?= $(BOARD_FILE)))
$(foreach l,$(exec-y),$(eval $(l)-objs-all-y := $($(l)-board-y) $(foreach s,$($(l)-objs-y),$(subst $(escape_let),$(escape_dir_name),$(d))/$(s))))
$(foreach l,$(exec-y),$(eval $(l)-objs-y += $(notdir $($(l)-board-y))))
$(foreach l,$(exec-y),$(eval $(l)-output-dir-y := $(BIN_DIR)$(if $($(l)-board-y),/,)$(notdir ${$(l)-board-y:.c=})))

# Fix myprog-objs-y as follows
#  - include the full path to the directory
#  - change .c with .o
#  - add $(b-output-dir-y)/ as a prefix, so all the objects are in a directory together
#    note that this requires a corresponding stripping of $(b-output-dir-y)/ in the %.o:%.c rule
$(foreach l,$(exec-y),$(eval $(l)-objs-y := $(foreach s,$($(l)-objs-y),$(b-objs-output-dir-y)/$(subst $(escape_let),$(escape_dir_name),$(d))/$(s:%.c=%.o))))
$(foreach l,$(exec-y),$(eval $(l)-objs-y := $(foreach s,$($(l)-objs-y),$(s:%.cc=%.o))))
$(foreach l,$(exec-y),$(eval $(l)-objs-y := $(foreach s,$($(l)-objs-y),$(s:%.cpp=%.o))))
$(foreach l,$(exec-y),$(eval $(l)-objs-y := $(foreach s,$($(l)-objs-y),$(s:%.s=%.o))))
$(foreach l,$(exec-y),$(eval $(l)-objs-y := $(foreach s,$($(l)-objs-y),$(s:%.S=%.o))))
b-object-dir-y += $(foreach l,$(exec-y),$(sort $(dir $($(l)-objs-y))))

# Copy exec-y in a separate collection variable
b-exec-apps-y += $(foreach l,$(exec-y),$(l).app)

# Copy the dependencies
b-deps-y += $(foreach l,$(exec-y),$(foreach s,$($(l)-objs-y),$(s:%.o=%.d)))
b-deps-y += $(foreach l,$(exec-y),$(foreach s,$($(l)-objs-y),$(s:%.o=%.o.cmd)))

$(foreach l,$(exec-y),$(eval $(l)-dir-y := $(d)))

# Assigning ld options for exec-y and exec-cpp-y, which is used for axf
# generation
$(foreach l,$(exec-y),$(eval $(l)-LD := $(LD)))
$(foreach l,$(exec-cpp-y),$(eval $(l)-LD := $(CPP)))

b-deps-y += $(foreach e,$(exec-y),$($(e)-output-dir-y)/$(e).axf.cmd)

# Unify variables

b-exec-y += $(exec-y)
b-exec-cpp-y += $(exec-cpp-y)
b-libs-y += $(libs-y)

# Nullify variables

exec-y :=
exec-cpp-y :=
libs-y :=

