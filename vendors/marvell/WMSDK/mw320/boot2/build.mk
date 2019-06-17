# Copyright (C) 2018 Marvell International Ltd.
# All Rights Reserved.

ifneq ($(arch_name-y),)
subdir-y += src/$(arch_name-y)
endif
#subdir-y += src/boot2.c

subdir-y += bh_generator.mk
