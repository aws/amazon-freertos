# Copyright (C) 2008-2017 Marvell International Ltd.
# All Rights Reserved.

# This variable is populated in toolchain specific file.
# If not populated then it is assigned value below.
t_axf2fw ?= sdk/tools/bin/$(os_dir)/axf2firmware$(file_ext)

$(t_axf2fw):
	@echo " [$(notdir $@)]"
	$(AT)$(MAKE) $(SILENT) -C sdk/tools/src/host-tools/axf2firmware CC=$(HOST_CC) SDK_PATH=$(CURDIR) TARGET=$(notdir $@) TOOLCHAIN=$(TOOLCHAIN) NOISY=$(NOISY) all
	$(AT)$(t_cp) -a sdk/tools/src/host-tools/axf2firmware/$(notdir $@) $@
	$(AT)$(MAKE) $(SILENT) -C sdk/tools/src/host-tools/axf2firmware CC=$(HOST_CC) SDK_PATH=$(CURDIR) TARGET=$(notdir $@) TOOLCHAIN=$(TOOLCHAIN) NOISY=$(NOISY) clean

$(t_axf2fw).clean:
	@echo " [clean] $(notdir $(t_axf2fw))"
	$(AT)$(MAKE) $(SILENT) -C sdk/tools/src/host-tools/axf2firmware CC=$(HOST_CC) SDK_PATH=$(CURDIR) TARGET=$(notdir $(t_axf2fw)) TOOLCHAIN=$(TOOLCHAIN) NOISY=$(NOISY) clean
	$(AT)$(t_rm) -f $(t_axf2fw)

$(notdir $(t_axf2fw)): $(t_axf2fw)
$(notdir $(t_axf2fw)).clean: $(t_axf2fw).clean

tools.install: $(notdir $(t_axf2fw))
tools.clean: $(notdir $(t_axf2fw)).clean

