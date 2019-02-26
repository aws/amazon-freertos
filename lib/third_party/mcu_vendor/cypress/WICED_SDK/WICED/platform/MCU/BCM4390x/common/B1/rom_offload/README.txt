(\
MAKE_LD_PL=ROM_offload/make_ld.pl; \
CHIP=43909B1; \
SRC_DIR=GCC/$CHIP/generator_script; \
DST_DIR=ROM_offload/$CHIP; \
~/git/Wiced-SDK/tools/common/Win32/perl.exe $MAKE_LD_PL \
-rom_use_input_file=$SRC_DIR/rom_use_nm.txt \
-exclude_pattern_input_file=$SRC_DIR/symbol_pattern_matching_exclude_list.txt \
-rom_symbols_output=$DST_DIR/rom_symbols.txt \
-rom_use_output_file=$DST_DIR/GCC_rom.ld \
-rom_objs_output_file=$DST_DIR/GCC_rom_sections.ld \
-reference_s_output_file=/dev/null \
-ram_stubs_output_file=$DST_DIR/GCC_rom_sections.ld \
-ram_stub_list_output_file=$DST_DIR/ram_stub_list.txt \ 
)
