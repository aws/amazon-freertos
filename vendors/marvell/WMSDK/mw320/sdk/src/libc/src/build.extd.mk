libs-y += libc-extd

libc-extd-objs-y :=  \
		ctype_isalnum.c ctype_isalpha.c \
		ctype_isascii.c ctype_isblank.c \
		ctype_iscntrl.c ctype_isdigit.c \
		ctype_isgraph.c ctype_ispunct.c \
		ctype_islower.c ctype_isprint.c \
		ctype_isspace.c ctype_isupper.c \
		ctype_tolower.c ctype_toupper.c \
		ctype_isxdigit.c ctypes.c

global-cflags-y += -Isdk/src/incl/libc

libc-extd-supported-toolchain-y := arm_gcc iar
