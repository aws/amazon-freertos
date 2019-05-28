# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.

os_dir := Darwin

# Function to resolve input path
define b-abspath
$(abspath $(1))
endef
