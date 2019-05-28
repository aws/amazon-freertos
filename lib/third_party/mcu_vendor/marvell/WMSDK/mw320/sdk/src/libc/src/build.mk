# Copyright 2008-2017, Marvell International Ltd.

libs-y += libc

libc-cflags-y += -Isdk/src/incl/libc

libc-objs-y :=  atof.c atoi.c atol.c
libc-objs-y +=  atoll.c jrand48.c lrand48.c
libc-objs-y +=  mrand48.c nrand48.c seed48.c
libc-objs-y +=  srand48.c memccpy.c
libc-objs-y +=  memswap.c memchr.c memrchr.c
libc-objs-y +=  memmem.c memcmp.c bsearch.c
libc-objs-y +=  qsort.c fnmatch.c snprintf.c
libc-objs-y +=  sprintf.c sscanf.c vsnprintf.c
libc-objs-y +=  vsprintf.c vsscanf.c strcmp.c
libc-objs-y +=  sprintf.c vsnprintf.c snprintf.c
libc-objs-y +=  strncmp.c strcasecmp.c strncasecmp.c
libc-objs-y +=  strcpy.c strlcpy.c strncpy.c
libc-objs-y +=  strcat.c strlcat.c strncat.c
libc-objs-y +=  strlen.c strnlen.c strchr.c
libc-objs-y +=  strrchr.c strsep.c strstr.c
libc-objs-y +=  strcspn.c strspn.c strxspn.c
libc-objs-y +=  strpbrk.c strtok.c strtok_r.c
libc-objs-y +=  strtol.c strtoll.c strtoul.c
libc-objs-y +=  strtoull.c strtoimax.c strtoumax.c
libc-objs-y +=  strntoimax.c strntoumax.c
libc-objs-y +=  stdlib.c common.c

libc-supported-toolchain-y := arm_gcc iar

subdir-$(USE_EXTD_LIBC) += build.extd.mk
