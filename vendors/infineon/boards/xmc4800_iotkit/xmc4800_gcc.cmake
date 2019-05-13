# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------

#Set defined symbols for compiler and assembler 
set( compiler_defined_symbols
     XMC4800_F100x2048
)

#Set Compiler Debug options
set( compiler_debug_options "-g" "-gdwarf-2")

#Set compiler warnings level
set( compiler_warnings "-Wall" )

#Set compiler optimization level
set( compiler_optimization_options "-O2" "-ffunction-sections" "-fdata-sections")

#Set ARM GCC options
set( compiler_arm_options "-mfloat-abi=softfp" "-mcpu=cortex-m4" "-mfpu=fpv4-sp-d16" "-mthumb")

#Set other compiler options and flags
set( compiler_misc_options
     "-std=gnu99"
	 "-c"
	 "-fmessage-length=0" )

#All compiler options
set( compiler_flags
     ${compiler_debug_options}
	 ${compiler_warnings}
	 ${compiler_optimization_options}
	 ${compiler_arm_options}
	 ${compiler_misc_options}
	 "-pipe" )

#All assembler options
set( assembler_flags
     "-x"
	 "assembler-with-cpp"
     ${compiler_debug_options}
     ${compiler_warnings}
	 ${compiler_optimization_options}
	 ${compiler_arm_options}
	 ${compiler_misc_options}
)

#All linker options
set( linker_flags
     "-TXMC4800x2048.ld"
	 "-nostartfiles"
	 "-Xlinker"
	 "--gc-sections"
	 "-specs=nano.specs"
	 "-specs=nosys.specs"
	 "-Wl,-Map,aws_demos.map"
	 ${compiler_arm_options}
	 ${compiler_debug_options}
)

set(link_directories
    "${xmclib_dir}/CMSIS/Infineon/XMC4800_series/Source/GCC"
)

#Linker libraries
set(linker_libs "m")

#Source files specific to compiler
set(compiler_specific_src
    "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F/port.c"
    "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F/portmacro.h"
	"${xmclib_dir}/CMSIS/Infineon/XMC4800_series/Source/system_XMC4800.c"
)

#Include directories specific to compiler
set(compiler_specific_include
    "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F"
	"${xmclib_dir}/CMSIS/Infineon/XMC4800_series/Include"
)

set(executable_suffix ".ihex")
