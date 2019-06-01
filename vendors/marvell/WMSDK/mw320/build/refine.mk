# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.
#
# Description:
# ------------
# This file, refine.mk contains:
#
# 	Refining cflags for various options
# 	Refining libs-paths-y for app
# 	Default assignment of linkerscript-y
# 	Default assignment for lflags-y for CPP apps

disable-lto-for += $(DISABLE_LTO_FOR)

lto-cflags-$(CONFIG_ENABLE_LTO) := -flto -ffat-lto-objects

define append_lto_flags
	$(1)-cflags-y += $(lto-cflags-y)
endef


$(foreach l, $(b-libs-y), $(if $(findstring $(l), $(disable-lto-for)),,$(eval $(call append_lto_flags,$(l)))))
$(foreach e, $(b-exec-y), $(if $(findstring $(e), $(disable-lto-for)),,$(eval $(call append_lto_flags,$(e)))))


#--------------------------------------------------------------#
# Rules to handle $(1)-select-libs-y

define handle_select_libs
  ifdef $(1)-select-libs-y
    $(1)-libs-paths-y :=
    $(foreach v,$($(1)-select-libs-y),$$(eval $(1)-libs-paths-y += $(b-libs-output-dir-y)/$(v)))
  else
    $(1)-libs-paths-y := $(b-libs-paths-y)
  endif
endef

$(foreach p,$(b-exec-y),$(eval $(call handle_select_libs,$(p))))
#--------------------------------------------------------------#
# Rules to handle $(1)-prune-libs-y
# Note: This handling done after "handle_select_libs"

define handle_prune_libs
  $(foreach v,$($(1)-prune-libs-y),$(eval $(1)-filter-y += $(b-libs-output-dir-y)/$(v)))
  $(eval $(1)-libs-paths-y := $(filter-out $($(1)-filter-y),$($(1)-libs-paths-y)))
endef

$(foreach p,$(b-exec-y), $(eval $(call handle_prune_libs,$(p))))
#--------------------------------------------------------------#
# Default assignment of linkerscript

define handle_linkerscript
  ifndef $(1)-linkerscript-y
     $(1)-linkerscript-y := $$(global-linkerscript-y)
  endif
endef

$(foreach p,$(b-exec-y), $(eval $(call handle_linkerscript,$(p))))
#--------------------------------------------------------------#
# Default assignment of lflags for cpp

define handle_lflags
  ifndef $(1)-lflags-y
     $(1)-lflags-y := $(2)
  else
     $(1)-lflags-y += $(2)
  endif
endef

$(foreach p,$(b-exec-cpp-y), $(eval $(call handle_lflags,$(p),$(global-cpp-cflags-y))))
$(foreach p,$(b-exec-cpp-y), $(eval $(call handle_lflags,$(p),$(tc-cpp-lflags-y))))

ifeq ($(XIP), 1)
  $(foreach e,$(b-exec-y), $(eval $(e)-cflags-y += -DAPPCONFIG_XIP_ENABLE))
endif


include build/toolchains/virtual_targets_toolchain.mk
