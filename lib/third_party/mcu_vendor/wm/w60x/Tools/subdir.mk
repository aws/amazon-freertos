define get_subdirs_l2
	main_dir:=$(1)
	sub_dir:=$(2)
${main_dir}_${sub_dir}_dirs := $(shell find ${TOPDIR}/${main_dir}/${sub_dir}/ -maxdepth 1 -type d)
endef

Src_App_exclude_dirs := ${TOPDIR}/Src/App/ \
       			${TOPDIR}/Src/App/infrared \
			${TOPDIR}/Src/App/cloud \
			${TOPDIR}/Src/App/ajtcl-15.04.00a \
			${TOPDIR}/Src/App/makelib 
#$(call get_subdirs_l2, Src, App)
Src_App_dirs := $(shell find ${TOPDIR}/Src/App/ -maxdepth 1 -type d)
Src_App_dirs := $(basename $(patsubst ./%,%,$(Src_App_dirs))) 
Src_App_dirs := $(filter-out $(Src_App_exclude_dirs),$(Src_App_dirs))
Src_App_dirs += ${TOPDIR}/Src/App/libwebsockets-2.1-stable
#Src_App_dirs += ${TOPDIR}/Src/App/libupnp-1.6.19

Src_Net_exclude_dirs := ${TOPDIR}/Src/Net/lwip1.4.1 \
			$(TOPDIR)/Src/Network/api
Src_Net_dirs := $(shell find ${TOPDIR}/Src/Network -maxdepth 1 -type d)
Src_Net_dirs := $(basename $(patsubst ./%,%,$(Src_Net_dirs))) 
Src_Net_dirs := $(filter-out $(Src_Net_exclude_dirs),$(Src_Net_dirs))


Src_Wlan_exclude_dirs := ${TOPDIR}/Src/Wlan/ \
                         ${TOPDIR}/Src/Wlan/Api \
                         ${TOPDIR}/Src/Wlan/Supplicant \
                         ${TOPDIR}/Src/Wlan/litepoint \
                         ${TOPDIR}/Src/Wlan/Driver
Src_Wlan_dirs := $(shell find ${TOPDIR}/Src/Wlan -maxdepth 1 -type d)
Src_Wlan_dirs := $(basename $(patsubst ./%,%,$(Src_Wlan_dirs)))
Src_Wlan_dirs := $(filter-out $(Src_Wlan_exclude_dirs),$(Src_Wlan_dirs))

Src_RTOS_exclude_dirs := ${TOPDIR}/Src/OS/RTOS/include \
			 ${TOPDIR}/Src/OS/RTOS/source \
			 ${TOPDIR}/Src/OS/RTOS/ports
Src_RTOS_dirs := $(shell find ${TOPDIR}/Src/OS/RTOS -maxdepth 1 -type d)
Src_RTOS_dirs := $(basename $(patsubst ./%,%,$(Src_RTOS_dirs))) 
Src_RTOS_dirs := $(filter-out $(Src_RTOS_exclude_dirs),$(Src_RTOS_dirs))

WM_Demo_exclude_dirs := ${TOPDIR}/Demo/console
WM_Demo_dirs := $(shell find ${TOPDIR}/Demo -maxdepth 1 -type d)
WM_Demo_dirs := $(basename $(patsubst ./%,%,$(WM_Demo_dirs))) 
WM_Demo_dirs := $(filter-out $(WM_Demo_exclude_dirs),$(WM_Demo_dirs))

Platform_exclude_dirs := ${TOPDIR}/Platform \
			 ${TOPDIR}/Platform/Inc \
			 ${TOPDIR}/Platform/Common/crypto \
			 ${TOPDIR}/Platform/Common/crypto/symmetric \
			 ${TOPDIR}/Platform/Common/crypto/math \
			 ${TOPDIR}/Platform/Common/crypto/pubkey \
			 ${TOPDIR}/Platform/Common/crypto/prng \
			 ${TOPDIR}/Platform/Common/crypto/digest \
			 ${TOPDIR}/Platform/Common/crypto/keyformat \
			 ${TOPDIR}/Platform/Boot \
			 ${TOPDIR}/Platform/Boot/gcc/bk \
			 ${TOPDIR}/Platform/Boot/armcc \
			 ${TOPDIR}/Platform/Boot/iccarm \
			 ${TOPDIR}/Platform/Drivers/pmu \
			 ${TOPDIR}/Platform/Drivers/VS10XX \
			 ${TOPDIR}/Platform/Drivers/7816
Platform_dirs := $(shell find ${TOPDIR}/Platform -type d)
Platform_dirs := $(basename $(patsubst ./%,%,$(Platform_dirs))) 
Platform_dirs := $(filter-out $(Platform_exclude_dirs),$(Platform_dirs))
