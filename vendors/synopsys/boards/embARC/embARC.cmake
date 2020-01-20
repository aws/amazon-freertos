# ========================================================
#   embARC cmake for external projects
# ========================================================

# source directory
if(NOT DEFINED embARC_src_dir)
	message(FATAL_ERROR "embARC: embARC_src_dir is not defined")
endif()
message("embARC: embARC_src_dir is ${embARC_src_dir}")

if(NOT DEFINED embARC_ROOT)
	message(WARNING "embARC: embARC_ROOT is not defined")
else()
	message("embARC: embARC_ROOT is ${embARC_ROOT}")
	set(embARC_ROOT EMBARC_ROOT=${embARC_ROOT})
endif()

#parameters check
if(NOT DEFINED embARC_BOARD)
	message(WARNING "embARC: embARC_BOARD is not defined, setting to default")
	set(embARC_BOARD "emsdp")
endif()
if(NOT DEFINED embARC_BOARD_VER)
	message(WARNING "embARC: embARC_BOARD_VER is not defined, setting to default")
	if(${embARC_BOARD} STREQUAL "emsdp")
	    set(embARC_BOARD_VER rev2)
	elseif(${embARC_BOARD} STREQUAL "emsk")
	    set(embARC_BOARD_VER 22)
	endif()
endif()
if(NOT DEFINED embARC_CUR_CORE)
	message(WARNING "embARC: embARC_CUR_CORE is not defined, setting to default")
	if(${embARC_BOARD} STREQUAL "emsdp")
	    set(embARC_CUR_CORE em11d_dfss)
	elseif(${embARC_BOARD} STREQUAL "emsk")
	    set(embARC_CUR_CORE arcem7d)
	endif()
endif()
if(NOT DEFINED embARC_TOOLCHAIN)
	message(WARNING "embARC: embARC_TOOLCHAIN is not defined, setting to default")
	set(embARC_TOOLCHAIN "gnu")
endif()

if(${embARC_TOOLCHAIN} STREQUAL "gnu")
    set(make_exe make)
elseif(${embARC_TOOLCHAIN} STREQUAL "mw")
    set(make_exe gmake)
endif()

# output directory
message("embARC: CMAKE_BINARY_DIR is ${CMAKE_BINARY_DIR}")
# library settings
message("embARC: embARC_BOARD is ${embARC_BOARD}")
message("embARC: embARC_BOARD_VER is ${embARC_BOARD_VER}")
message("embARC: embARC_CUR_CORE is ${embARC_CUR_CORE}")
message("embARC: embARC_TOOLCHAIN is ${embARC_TOOLCHAIN}")

include(ExternalProject)

ExternalProject_Add(
        embARC                 # Name for custom target
        GIT_REPOSITORY		https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_bsp.git
        GIT_TAG    			upstream	#branch name
        PATCH_COMMAND		git apply --check ${CMAKE_CURRENT_LIST_DIR}/0001-arc-add-support-to-nest-exception.patch && git am ${CMAKE_CURRENT_LIST_DIR}/0001-arc-add-support-to-nest-exception.patch || git status
        PREFIX    			${CMAKE_BINARY_DIR} # Root dir for entire project
        SOURCE_DIR 			${embARC_src_dir}
        BINARY_DIR 			${CMAKE_BINARY_DIR}/embARC_build
        CONFIGURE_COMMAND 	cp ${CMAKE_CURRENT_LIST_DIR}/makefile_template ${CMAKE_BINARY_DIR}/embARC_build/makefile
        BUILD_COMMAND 		${make_exe} BOARD=${embARC_BOARD} ${embARC_ROOT} TOOLCHAIN=${embARC_TOOLCHAIN} BD_VER=${embARC_BOARD_VER} CUR_CORE=${embARC_CUR_CORE} embarc_lib
        INSTALL_COMMAND 	cp ${CMAKE_BINARY_DIR}/embARC_build/obj_${embARC_BOARD}_${embARC_BOARD_VER}/${embARC_TOOLCHAIN}_${embARC_CUR_CORE}/linker_${embARC_TOOLCHAIN}.ldf ${CMAKE_BINARY_DIR}/embARC_build/linker_${embARC_TOOLCHAIN}.ldf
        BUILD_BYPRODUCTS 	${CMAKE_BINARY_DIR}/embARC_build/obj_${embARC_BOARD}_${embARC_BOARD_VER}/${embARC_TOOLCHAIN}_${embARC_CUR_CORE}/libembARC.a
    )

#force embARC_lib to compile/update
add_custom_target(embARC_lib
	ALL
	DEPENDS embARC
	COMMAND pwd
	# COMMAND cp ${embARC_src_dir}/options/makefile_template ${CMAKE_BINARY_DIR}/embARC_build/makefile
	# COMMAND make BOARD=${embARC_BOARD} EMBARC_ROOT=../../vendors/synopsys/embarc_bsp TOOLCHAIN=${embARC_TOOLCHAIN} BD_VER=${embARC_BOARD_VER} CUR_CORE=${embARC_CUR_CORE} embarc_lib
	# COMMAND cp ${CMAKE_BINARY_DIR}/embARC_build/obj_${embARC_BOARD}_${embARC_BOARD_VER}/${embARC_TOOLCHAIN}_${embARC_CUR_CORE}/linker_${embARC_TOOLCHAIN}.ldf ${CMAKE_BINARY_DIR}/embARC_build/linker_${embARC_TOOLCHAIN}.ldf
	BYPRODUCTS ${CMAKE_BINARY_DIR}/embARC_build/obj_${embARC_BOARD}_${embARC_BOARD_VER}/${embARC_TOOLCHAIN}_${embARC_CUR_CORE}/libembARC.a
	)

if(${embARC_BOARD} STREQUAL "emsdp")
    set(BOARD_INC
		${embARC_src_dir}/board
    	${embARC_src_dir}/board/${embARC_BOARD}/${embARC_BOARD_VER}/configs
    	${embARC_src_dir}/board/${embARC_BOARD}/${embARC_BOARD_VER}/configs/${embARC_CUR_CORE}/include
    )
else()
    set(BOARD_INC
		${embARC_src_dir}/board
    	${embARC_src_dir}/board/${embARC_BOARD}/configs/${embARC_BOARD_VER}
    )
endif()

set(
	embARC_LIB_DIR
	"${CMAKE_BINARY_DIR}/embARC_build/obj_${embARC_BOARD}_${embARC_BOARD_VER}/${embARC_TOOLCHAIN}_${embARC_CUR_CORE}"
)
set(
	embARC_INC_DIR
	"${embARC_src_dir}/include"
	"${embARC_src_dir}/library"
	${BOARD_INC}
	# ${embARC_LIB_DIR}/embARC_generated
)

# include_directories(${embARC_INC_DIR})

# mdb -nooptions -nogoifmain -toggle=include_local_symbols=1 -hard -digilent -OS=FreeRTOS -run obj_emsk_23/mw_arcem9d/blinky_mw_arcem9d.elf
# mdb -nooptions -nogoifmain -toggle=include_local_symbols=1 -hard -digilent -OS=FreeRTOS
# mdb  -source_path=${embARC_src_dir}/arc/startup -nooptions -nogoifmain -toggle=include_local_symbols=1 -hard -digilent ${target_name}.elf
# arc-elf32-gdb -ex "target remote | openocd --pipe -s  c:/arc_gnu/bin/../lib/gcc/arc-elf32/8.3.1/../../../../arc-elf32/bin/../../share/openocd/scripts -f  c:/arc_gnu/bin/../lib/gcc/arc-elf32/8.3.1/../../../../arc-elf32/bin/../../share/openocd/scripts/board/snps_em_sk_v2.2.cfg" -ex "load" -ex "c" obj_emsk_23/gnu_arcem9d/blinky_gnu_arcem9d.elf
# arc-elf32-gdb -ex "target remote | openocd --pipe -s  c:/arc_gnu/bin/../lib/gcc/arc-elf32/8.3.1/../../../../arc-elf32/bin/../../share/openocd/scripts -f  c:/arc_gnu/bin/../lib/gcc/arc-elf32/8.3.1/../../../../arc-elf32/bin/../../share/openocd/scripts/board/snps_em_sk_v2.2.cfg" -ex "load" obj_emsk_23/gnu_arcem9d/blinky_gnu_arcem9d.elf
