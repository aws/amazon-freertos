# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.
#
# Description:
# ------------
# This file, virtual_targets_toolchain.mk contains:
#
# 	Virtual targets for building libraries and apps
# 	specific to supported toolchains.
#
#==============================================================#
# Toolchain specific libs and exec handling

# This function initialises variables to null
define init-tc-vars
  b-$(1)-libs-y :=
  b-$(1)-exec-y :=
endef
$(foreach t,$(b-supported-toolchain-y),$(eval $(call init-tc-vars,$(t))))
#--------------------------------------------------------------#
# These functions (filter-opt, check-toolchain-opt) performs,
#	1. Checks for validity for toolchain provided
#	   in <app_name/lib_name>-supported-toolchain-y
#	   variable.
#	2. If wrong toolchain found return error.

define filter-opt
  $(if $(filter $(1),$(b-supported-toolchain-y)),,\
  $(error "[Error] Incorrect toolchain option:\
  '$(1)' in '$(2)-supported-toolchain-y'"))
endef

define check-toolchain-opt
  ifdef $(1)-supported-toolchain-y
    $(foreach t,$($(1)-supported-toolchain-y),$(eval $(call filter-opt,$(t),$(1))))
  endif
endef

$(foreach l,$(b-libs-y),$(eval $(call check-toolchain-opt,$(e))))
$(foreach e,$(b-exec-y),$(eval $(call check-toolchain-opt,$(e))))
#--------------------------------------------------------------#

# This function performs,
# 	1. Check for <app/lib>-supported-toolchain-y variable.
# 	2. Populating toolchain specific <app/lib> list.

define handle-tc-list
  ifndef $(2)-supported-toolchain-y
    b-arm_gcc-$(3)-y += $(2).$(4)
  else
    ifneq ($(filter $(1),$($(2)-supported-toolchain-y)),)
      b-$(1)-$(3)-y += $(2).$(4)
    endif
  endif
endef

define get-tc-vars
  $(foreach l,$(b-libs-y),$(eval $(call handle-tc-list,$(1),$(l),libs,a)))
  $(foreach e,$(b-exec-y),$(eval $(call handle-tc-list,$(1),$(e),exec,app)))
endef

$(foreach t,$(b-supported-toolchain-y),$(eval $(call get-tc-vars,$(t))))
#--------------------------------------------------------------#

# This function will filter-out common things from gcc libs and apps
define prune-arm_gcc-vars
  b-arm_gcc-libs-y := $(filter-out $(b-$(1)-libs-y),$(b-arm_gcc-libs-y))
  b-arm_gcc-exec-y := $(filter-out $(b-$(1)-exec-y),$(b-arm_gcc-exec-y))
endef

$(foreach t,$(filter-out arm_gcc,$(b-supported-toolchain-y)),$(eval $(call prune-arm_gcc-vars,$(t))))
#--------------------------------------------------------------#

# This function creates toolchain specific target for libs and apps
define create-tc-target

$(1)-compat-libs: $(b-$(1)-libs-y)
$(1)-compat-apps: $(b-$(1)-exec-y)

# Application build depend on library build
$(b-$(1)-exec-y): $(b-$(1)-libs-y)

$(1)-compat-libs.clean: $(foreach l,$(b-$(1)-libs-y),$(l).clean)
$(1)-compat-apps.clean: $(foreach e,$(b-$(1)-exec-y),$(e).clean)

endef

$(foreach t,$(b-supported-toolchain-y),$(eval $(call create-tc-target,$(t))))
#==============================================================#

# Note:
#
# In context of 'ewp-trgt',
# libraries should be placed first and then execs.
#
# This order is carried forward
# in output IAR workspace (.eww) file.

ewp-lib-trgt := $(foreach l,$(b-iar-libs-y),${l:.a=})
ewp-exec-trgt := $(foreach e,$(b-iar-exec-y),${e:.app=})
ewp-trgt := $(ewp-lib-trgt) $(ewp-exec-trgt)

eww-trgt := sdk_workspace.eww

# This target will create IAR project (.ewp) files for lib and exec
# Then it will generate a single IAR workspace (.eww) file

.PHONY: all-ewps $(eww-trgt).clean

all-ewps: $(eww-trgt)

$(eww-trgt): $(create_workspace_pyc)
	@echo " [create] $@"
	$(call b-cmd-eww,$@,ewp-trgt)

all-ewps.clean: $(eww-trgt).clean

$(eww-trgt).clean:
	@echo " [clean] $(eww-trgt)"
	$(AT)$(t_rm) -f $(eww-trgt)
#==============================================================#
define create_ewp

  $(eww-trgt): $(1).ewp

  $(1).ewp: $($(1)-dir-y)/$(1).ewp

  $($(1)-dir-y)/$(1).ewp: $(t_create_workspace) $(t_update_ewp)
	@echo " [create] $$@"
	$$(call $(2),$(1))

  ifeq ($(FORCE_EWP),y)
    .PHONY: $($(1)-dir-y)/$(1).ewp
  endif

  .PHONY: $(1).ewp.clean  $(1).ewp

  $(eww-trgt).clean: $(1).ewp.clean

  $(1).ewp.clean:
	@echo " [clean] $(1).ewp"
	$$(AT)$$(t_rm) -f $($(1)-dir-y)/$(1).ewp

endef

$(foreach t,$(ewp-lib-trgt),$(eval $(call create_ewp,$(t),b-cmd-lib-ewp)))
$(foreach t,$(ewp-exec-trgt),$(eval $(call create_ewp,$(t),b-cmd-exec-ewp)))
