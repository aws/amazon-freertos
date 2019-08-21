# Copyright 2008-2017, Marvell International Ltd.

libs-y += libxz

libxz-objs-y :=  xz_crc32.c xz_dec_lzma2.c xz_dec_stream.c
libxz-objs-y +=  decompress.c

libxz-supported-toolchain-y := arm_gcc iar
