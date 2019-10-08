# Copyright 2008-2019, Marvell International Ltd.

libs-y += libcli

libcli-objs-y :=  cli.c cli_utils.c cli_mem_simple.c

libcli-supported-toolchain-y := arm_gcc iar
