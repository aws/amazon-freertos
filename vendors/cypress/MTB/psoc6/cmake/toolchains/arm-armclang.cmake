get_filename_component(dir ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)
include("${dir}/toolchains/features.cmake")


function(cy_cfg_toolchain)
    if("$ENV{CY_DEVICE}" STREQUAL "")
        message(FATAL_ERROR "Must be called after cy_cfg_env")
    endif()

    # set common/core compiler arguments
    set(ENV{OPTIMIZATION} "-Oz")
    set(ENV{DEBUG_FLAG} "DEBUG")
    # As of ARM Compiler 6.12, <assert.h> does not define static_assert (ISO/IEC 9899:2011).
    # This define may interfere with <cassert> or future versions of the ARM C library.
    set(ENV{DEFINE_FLAGS} "-Dstatic_assert=_Static_assert")
    set(ENV{COMMON_FLAGS} "-ffunction-sections;-fdata-sections;-fno-exceptions;-fshort-enums;-fshort-wchar;-g;-masm=auto")
    set(ENV{VFP_FLAGS} "-mfloat-abi=softfp;-mfpu=fpv4-sp-d16")
    set(ENV{ASFLAGS} "--cpu=${CMAKE_SYSTEM_PROCESSOR};--thumb;--fpu=FPv4-SP")
    set(ENV{LDFLAGS} "--fpu=FPv4-SP;--show_full_path;--undefined=uxTopUsedPriority")
    set(ENV{CORE_FLAGS} "-mthumb")

    # list(FIND ... doesn't work reliably with lists in environment variables :-/
    set(check "$ENV{CY_DEVICES_WITH_M0P}")
    list(FIND check $ENV{CY_DEVICE} device_m0p)

    set(check "$ENV{CY_DEVICES_WITH_DIE_PSOC6A12K}")
    list(FIND check $ENV{CY_DEVICE} device_512k)

    set(check "$ENV{CY_DEVICES_WITH_FLASH_KB_512}")
    list(FIND check $ENV{CY_DEVICE} flash_512k)

    set(check "$ENV{CY_DEVICES_WITH_FLASH_KB_1024}")
    list(FIND check $ENV{CY_DEVICE} flash_1024k)

    set(check "$ENV{CY_DEVICES_WITH_FLASH_KB_1856}")
    list(FIND check $ENV{CY_DEVICE} flash_1856k)

    set(check "$ENV{CY_DEVICES_WITH_FLASH_KB_2048}")
    list(FIND check $ENV{CY_DEVICE} flash_2048k)

    # core specifics (from defines.mk)
    if(${device_m0p} LESS 0)
        if($ENV{CY_CORE} STREQUAL "CM0P")
            message(FATAL_ERROR "$ENV{CY_DEVICE} does not have a CM0+ core")
        endif()
        set(ENV{CY_LINKERSCRIPT_SUFFIX "cm4")
    else()
        if($ENV{CY_CORE} STREQUAL "CM0P")
            set(ENV{CY_LINKERSCRIPT_SUFFIX} "cm0plus")
        else()
            set(ENV{CY_LINKERSCRIPT_SUFFIX} "cm4_dual")
        endif()
    endif()

    set(ENV{CY_LINKERSCRIPT_EXT} .sct)
    # ARMClang is more compatible with GCC than with RVDS so use the GCC port files for now
    set(ENV{CY_FREERTOS_TOOLCHAIN} GCC)

    # arch specifics (from defines.mk)
    if((${device_m0p} GREATER -1) AND (${device_512k} GREATER -1))
        set(ENV{CY_LINKER_SCRIPT_NAME} "cy8c6xx5")
        set(ENV{CY_PSOC6_ARCH} "psoc6_03")
    elseif(${flash_512k} GREATER -1)
        set(ENV{CY_LINKER_SCRIPT_NAME} "cy8c6xx6")
        set(ENV{CY_PSOC6_ARCH} "psoc6_01")
    elseif(${flash_1024k} GREATER -1)
        set(ENV{CY_LINKER_SCRIPT_NAME} "cy8c6xx7")
        set(ENV{CY_PSOC6_ARCH} "psoc6_01")
    elseif(${flash_1856k} GREATER -1)
        set(ENV{CY_LINKER_SCRIPT_NAME} "cy8c6xxa")
        set(ENV{CY_PSOC6_ARCH} "psoc6_02")
    elseif(${flash_2048k} GREATER -1)
        set(ENV{CY_LINKER_SCRIPT_NAME} "cy8c6xxa")
        set(ENV{CY_PSOC6_ARCH} "psoc6_02")
    else()
        message(FATAL_ERROR "Incorrect part number '$ENV{CY_DEVICE}'. Check DEVICE in board support package.")
    endif()
 
endfunction()

