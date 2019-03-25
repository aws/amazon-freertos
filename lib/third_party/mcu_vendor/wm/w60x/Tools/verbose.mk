ifndef WM_VERBOSE
	WM_VERBOSE :=
endif

ifdef V
ifeq ("$(origin V)", "command line")
WM_VERBOSE := $(V)
endif
endif

export WM_VERBOSE
