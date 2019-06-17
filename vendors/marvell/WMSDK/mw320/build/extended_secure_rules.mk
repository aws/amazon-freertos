# Copyright (C) 2008-2017, Marvell International Ltd.
# All Rights Reserved.
#
# Description:
# ------------
# This file, extended_secure_rules.mk contains rules/functions for:
#
# 	.axf file 	--> secure .bin file 	(create_secure_prog)

ifeq ($(b-secboot-y),y)

##################### Secureboot Tools creation

$(t_secboot): $(t_secboot_exe)

##################### Secure boot configuration file and make helper
$(eval $(call create_dir,$(sec_conf_dir)))

$(sec_conf): $(t_secconf) | $(sec_conf_dir)
	$(AT)$(t_python) $(t_secconf) -d $(sec_conf_dir) -c $(SECURE_BOOT) -s sdk/tools/src/host-tools/secureboot/sample_keys
	@echo " [sec][conf] $@"

define b-cmd-sec-type
$(t_python) $(t_secboot) -c $(sec_conf) -s $(ks_hdr) -M $(sec_type_mk) $(secboot_flags)
endef

define b-gen-sec-type
  $(AT)$(call b-cmd-sec-type)
  $(call cmd_save,$(call b-cmd-sec-type))
endef

$(sec_type_mk): $(sec_conf) $(ks_hdr) $(t_secboot) $(tc-force-opt)
	$(call if_changed_0,b-cmd-sec-type,b-gen-sec-type)

secboot.clean:
	$(AT)$(t_rm) -f $(sec_type_mk) $(sec_type_mk).cmd

clean: secboot.clean

##################### Secure Program ([.][e][s].bin) creation

define create_secure_prog
  $(1).app: $($(1)-output-dir-y)/$(1)$(sec_mcufw_type).bin

  $($(1)-output-dir-y)/$(1)$(sec_mcufw_type).bin: $($(1)-output-dir-y)/$(1).axf $$(t_axf2fw) $$(sec_conf) $$(sec_type_mk) $$(ks_hdr) $$(t_secboot)
	$$(AT)$$(t_rm) -f $$(wildcard $($(1)-output-dir-y)/$(1)*.bin)
	$$(AT)$$(t_axf2fw) $$< $($(1)-output-dir-y)/$(1).bin
  ifneq ($(sec_mcufw_type),)
	$$(AT)$$(t_python) $$(t_secboot) -c $$(sec_conf) -s $$(ks_hdr) -m $($(1)-output-dir-y)/$(1).bin $$(secboot_flags)
	$$(AT)$$(t_rm) $($(1)-output-dir-y)/$(1).bin
  endif
	@echo " [sbin] $$(call b-abspath,$$@)"

  .PHONY: $(1).app.clean_bin

  $(1).app.clean: $(1).app.clean_bin

  $(1).app.clean_bin:
	$$(AT)$$(t_rm) -f $$(wildcard $($(1)-output-dir-y)/$(1)*.bin)
endef

$(foreach p,$(filter-out $(b-axf-only),$(b-exec-y)), $(eval $(call create_secure_prog,$(p))))

##################### End of Secure Program creation rule ########
endif
