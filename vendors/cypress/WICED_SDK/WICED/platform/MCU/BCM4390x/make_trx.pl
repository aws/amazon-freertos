#!/usr/bin/perl

#
# $ Copyright Cypress Semiconductor $
#
# Adds an TRX (External Program Memory) header to a program
# ELF file, to allow it to be booted from serial flash
#
# TRX file header
#   uint32 magic;           /* "HDR0" or 0x30524448 */
#   uint32 len;             /* Length of file including header */
#   uint32 crc32;           /* Not currently used - 32-bit CRC from flag_version to end of file */
#   uint32 flag_version;    /* 0:15 flags, 16:31 version (Flags: 0x40=ImageIsBootloader 0x20=UncompressedImage)*/
#   uint32 image_length;    /* Size of the fw; used in uncompressed case */
#   uint32 dest_address;    /* RAM address for jumpto after download */
#   uint32 reserved;        /* must be zero */
#   uint32 reserved;        /* must be zero */
#   uint32 reserved;        /* must be zero */
#   Image Data
#
# ELF file header
#    uint32_t magic;  /* should be 0x7f 'E' 'L' 'F' */
#    uint8_t  elfclass;
#    uint8_t  data;
#    uint8_t  version;
#    uint8_t  reserved[9];
#    uint16_t type;
#    uint16_t machine;
#    uint32_t version;
#    uint32_t entry;
#    uint32_t program_header_offset; /* from start of file */
#    uint32_t section_header_table_offset; /* from start of file */
#    uint32_t flags;
#    uint16_t elf_header_size;
#    uint16_t program_header_entry_size;
#    uint16_t program_header_entry_count;
#    uint16_t section_header_entry_size;
#    uint16_t section_header_entry_count;
#    uint16_t string_table_index; /* index in the section header table */
#
# ELF Program Table Entry
#    uint32_t type;
#    uint32_t data_offset;
#    uint32_t virtual_address;
#    uint32_t physical_address;
#    uint32_t data_size_in_file;
#    uint32_t data_size_in_memory;
#    uint32_t flags;
#    uint32_t alignment;
#


if (! $ARGV[1] )
{
    print "Usage perl $0 <program_elf_name> <output_filename>";
    exit -1;
}

# Undefine input record separator.
# Even if file opened in binary mode, it is seen that script used record separator when unpacking binary records.
undef $/;

# Open Input and output files
open(my $in_file,  '<:raw', $ARGV[0]) or die "Unable to open: $!";
open(my $out_file, '>:raw', $ARGV[1]) or die "Unable to open: $!";

# allow alignment to be specified;
my $global_alignment_setting = 0;

if ( ( @ARGV >= 3 ) )
{
    $global_alignment_setting = ( $ARGV[2] );
}

# parse the input Elf file
my %elf = parse_elf( $in_file, 0 );

# Find the segments which would be loaded.
# Find first address and last address. Save first address and total size.

my $total_size = 0;
my $first_address = 0xFFFFFFFF;
my $last_address = 0x0;
my @save_segments = ();

print "prgm hdr cnt=$elf{\"program_header_count\"}\n";

for ( my $program_no = 0; $program_no < $elf{"program_header_count"}; $program_no++ )
{
    if ( ( $elf{"program_headers"}->[$program_no]->{"program_type"} == 0x1 ) &&
         ( $elf{"program_headers"}->[$program_no]->{"program_file_size"} != 0 ) )
    {
        my $segment_phys_addr = $elf{"program_headers"}->[$program_no]->{"program_physical_address"};
        my $segment_virt_addr = $elf{"program_headers"}->[$program_no]->{"program_virtual_address"};
        my $segment_file_size = $elf{"program_headers"}->[$program_no]->{"program_file_size"};
        my $segment_memory_size = $elf{"program_headers"}->[$program_no]->{"program_memory_size"};
        my $segment_fill_size = 0xFFFFFFFF;

        # Segments are aligned on page size.
        # Linker can due to alignment shift segment start address to lower addresses,
        # then add fill and only then contents of section. Let's find this fill size.
        for ( my $section_no = 0; $section_no < $elf{"section_header_count"}; $section_no++ )
        {
            # Only sections marked as Alloc are taken into account
            if ( $elf{"section_headers"}->[$section_no]->{"section_flags"} & 0x2 )
            {
                my $section_address = $elf{"section_headers"}->[$section_no]->{"section_address"};
                my $section_size = $elf{"section_headers"}->[$section_no]->{"section_size"};

                if ( ( $section_address >= $segment_virt_addr ) &&
                     ( $section_address + $section_size <= $segment_virt_addr + $segment_memory_size ) )
                {
                    my $new_segment_fill_size = $section_address - $segment_virt_addr;
                    if ( $new_segment_fill_size < $segment_fill_size )
                    {
                        $segment_fill_size = $new_segment_fill_size;
                    }
                }
            }
        }

        if ( $segment_fill_size >= $segment_file_size )
        {
            die( "Segment has no data to load while has non-zero file size" );
        }

        my $segment_data_phys_first_addr = $segment_fill_size + $segment_phys_addr;
        if ( $segment_data_phys_first_addr < $first_address )
        {
            $first_address = $segment_data_phys_first_addr;
        }

        my $segment_data_phys_last_addr = $segment_phys_addr + $segment_file_size;
        if ( $segment_data_phys_last_addr > $last_address )
        {
            $last_address = $segment_data_phys_last_addr;
        }

        push ( @save_segments, $program_no );
    }
}

if ( $last_address <= $first_address )
{
    die( "wrong first and last addresses" );
}
$total_size = $last_address - $first_address;

# Write TRX header

if ( ( ( $elf{"entry_point"} - $first_address ) > 255 ) || ( ( $elf{"entry_point"} - $first_address ) < 0 ) )
{
    die sprintf( "The entry point (0x%x) must be within 255 bytes of the start of the Bootloader (0x%x)",  $elf{"entry_point"}, $first_address );
}

if ( ( $first_address & 0xff ) != 0 )
{
    die sprintf( "The first address of the Bootloader in RAM (0x%x) must be 8 bit aligned to fit in the TRX header ", $first_address);
}

seek $out_file, 0, SEEK_SET;

print $out_file pack('cccc', ord('H'), ord('D'), ord('R'), ord('0') );  # Magic Signature
print $out_file pack('L', 36 + $total_size);  # file Length
print $out_file pack('L', 0);  # CRC - unused
print $out_file pack('L', 0x00020060);  # Flags & Version Number
print $out_file pack('L', $total_size);  # Image length
print $out_file pack('L', $elf{"entry_point"} );  # Upper 3 bytes = jump address - lower byte is entry point - image destination load address
print $out_file pack('L', 0);  # reserved
print $out_file pack('L', 0);  # reserved
print $out_file pack('L', 0);  # reserved

# Write app data

print "total_size = $total_size, entry_point = ";
printf "0x%2.2X \n", $elf{"entry_point"};

if ( $global_alignment_setting != 0 )
{
    print "requested segment alignment = $global_alignment_setting\n";
}


my $total_segment_size = 0;
my $rom_detect = 0;

#first check: do any segments in programs have romming?  If so, the programs cannot be padded out to an alignment
foreach $program_no ( @save_segments )
{
    for ( my $section_no = 0; $section_no < $elf{"section_header_count"}; $section_no++ )
    {
            my $section_name = $elf{"section_headers"}->[$section_no]->{"section_name"};

        if ( substr( $section_name, 0, 4 ) eq ".rom" )
        {
#it's a rom section, if applies to this program, then this program cannot be align changed
#if virtual address of segment is in the program address space, then avoid align of program
            if ( $elf{"section_headers"}->[$section_no]->{"section_address"} >= $elf{"program_headers"}->[$program_no]->{"program_virtual_address"} &&
                $elf{"section_headers"}->[$section_no]->{"section_address"} <= $elf{"program_headers"}->[$program_no]->{"program_virtual_address"} + $elf{"program_headers"}->[$program_no]->{"program_memory_size"} )
            {
                printf("Avoiding program align due to rommed section %d:%s at address %0x%08X\n", $section_no, $section_name, $elf{"section_headers"}->[$section_no]->{"section_address"});
                #disable alignment ops
                $rom_detect = 1;
                break;
            }
        }
    }
}

my $cur_byte_output   = 0;

foreach $program_no ( @save_segments )
{
    my $segment_offset = $elf{"program_headers"}->[$program_no]->{"program_offset"};
    my $segment_size = $elf{"program_headers"}->[$program_no]->{"program_file_size"};
    my $segment_phys_addr = $elf{"program_headers"}->[$program_no]->{"program_physical_address"};
#determine alignment need
    my $needed_alignment  = $global_alignment_setting;
    $cur_byte_output += $segment_size;
#allow global override to stick
    unless ( $global_alignment_setting )
    {
        $needed_alignment = $elf{"program_headers"}->[$program_no]->{"program_align"};
    }

    if ( $segment_phys_addr < $first_address )
    {
        my $offset = $first_address - $segment_phys_addr;

        if ( $segment_size <= $offset )
        {
            die( "whole segment outside address range" );
        }

        $segment_offset += $offset;
        $segment_size -= $offset;
    }

#only align if rommed sections not present in programs
    if ( $rom_detect == 0 )
    {
#alignment is needed to produce correct binary
#add padding to match needed alignment
        my $misalignment_amount = 0;
        my $segment_pad         = 0;

#alignment of 0 or 1 means no alignment
        if ( $needed_alignment > 1 )
        {
#For non-loadable segments, it's misaligned if prev segments left make next segment misaligned in the file
            print $elf{"program_headers"}->[$program_no]->{"program_type_description"};
            if ( $elf{"program_headers"}->[$program_no]->{"program_type_description"} ne "Load" )
            {
                $misalignment_amount = ( $segment_offset ) % $needed_alignment;
            }
            else
            {
#Loadable segments need to have a different alignment in the file, just as in the elf [actual offset alignment] - [needed offset alignment per virtual address]
                my $prog_alignment   = $elf{"program_headers"}->[$prog_alignment]->{"program_align"};
                my $align_actual     = ( ( $segment_offset - ( $total_segment_size + $segment_size - $cur_byte_output ) ) % $prog_alignment );
                my $align_required   = ( $elf{"program_headers"}->[$program_no]->{"program_virtual_address"} % $prog_alignment );

                if ( $align_actual < $align_required )
                {
                    $misalignment_amount = $align_required - $align_actual;
                }
                elsif ( $align_actual > $align_required )
                {
                    $misalignment_amount = ( $prog_alignment - $align_actual ) + $align_required;
                }
            }
        }

        if ( $misalignment_amount != 0 )
        {
            $segment_pad =  $needed_alignment - $misalignment_amount;
            $total_segment_size += $segment_pad;
        }
        else
        {
            $segment_pad = 0;
        }

        $total_segment_size += $segment_size;

        my @arr_segment_pad;
        for (my $idx=0; $idx < $segment_pad; $idx++)
        {
            $arr_segment_pad[$idx] = 0xFF;
        }

        printf "segment_offset = 0x%2.2X ", $segment_offset;
        print "segment_size = $segment_size, segment_pad = $segment_pad\n";

#dump out the padding to file (if any) to ensure alignment; this goes in front of segment to ensure alignment needs are met
        if ( $segment_pad != 0 )
        {
            print $out_file pack('C*', @arr_segment_pad);
            printf("start of next segment is 0x%2.2X\n", $segment_size + $segment_pad);
        }
    }

#write program data
    seek $in_file, $segment_offset, SEEK_SET;
    read $in_file, $file_data, $segment_size;
    print $out_file $file_data;
}

close($in_file);
close($out_file);

exit 0;


sub parse_elf( $file_handle, $verbose )
{
    my $file_handle = shift;
    my $verbose = shift;

    my %elf = ();

    seek $file_handle, 0, SEEK_SET;
    my ( $ch1, $ch2, $ch3, $ch4, $class, $byteorder, $hversion, $pad1, $pad2, $pad3, $filetype, $archtype, $fversion, $entry_point, $program_header_offset, $section_header_offset, $flags, $header_size, $program_header_entry_size, $program_header_count, $section_header_entry_size, $section_header_count, $string_section ) = unpack("CCCCCCCLLCSSLLLLLSSSSSS", <$file_handle>);

    if ( ( $ch1 != 0x7f ) ||
         ( $ch2 != ord('E') ) ||
         ( $ch3 != ord('L') ) ||
         ( $ch4 != ord('F') ) )
    {
        print "File is not an ELF file.";
        exit -1;
    }

    $elf{"class"} = $class;
    $elf{"byteorder"} = $byteorder;
    $elf{"hversion"} = $hversion;
    $elf{"filetype"} = $filetype;
    $elf{"archtype"} = $archtype;
    $elf{"fversion"} = $fversion;
    $elf{"entry_point"} = $entry_point;
    $elf{"program_header_offset"} = $program_header_offset;
    $elf{"section_header_offset"} = $section_header_offset;
    $elf{"flags"} = $flags;
    $elf{"header_size"} = $header_size;
    $elf{"program_header_entry_size"} = $program_header_entry_size;
    $elf{"program_header_count"} = $program_header_count;
    $elf{"section_header_entry_size"} = $section_header_entry_size;
    $elf{"section_header_count"} = $string_section;

    if ( $verbose )
    {
        print "ELF HEADER\n";
        print "-----------------------------------------\n";
        print "magic : $magic\n";
        print "class : $class\n";
        print "byteorder : $byteorder\n";
        print "hversion : $hversion\n";
        print "filetype : $filetype\n";
        print "archtype : $archtype\n";
        print "fversion : $fversion\n";
        printf "entry_point : 0x%08X\n", $entry_point;
        print "program_header_offset : $program_header_offset\n";
        print "section_header_offset : $section_header_offset\n";
        print "flags : $flags\n";
        print "header_size : $header_size\n";
        print "program_header_entry_size : $program_header_entry_size\n";
        print "program_header_count : $program_header_count\n";
        print "section_header_entry_size : $section_header_entry_size\n";
        print "section_header_count : $section_header_count\n";
        print "string_section : $string_section\n";
        print "-----------------------------------------\n";
    }

    my @section_headers = ();

    my $string_table_location = -1;

    for ( my $section_no = 0; $section_no < $section_header_count; $section_no++ )
    {
        seek $file_handle, $section_header_offset + $section_no * $section_header_entry_size, SEEK_SET;
        my ( $section_name, $section_type, $section_flags, $section_address, $section_offset, $section_size, $section_link, $section_info, $section_align, $section_entsize ) = unpack("LLLLLLLLLL", <$file_handle>);

        my %section = ();
        $section{"section_name_index"} = $section_name;
        $section{"section_type"} = $section_type;
        $section{"section_flags"} = $section_flags;
        $section{"section_address"} = $section_address;
        $section{"section_offset"} = $section_offset;
        $section{"section_size"} = $section_size;
        $section{"section_link"} = $section_link;
        $section{"section_info"} = $section_info;
        $section{"section_align"} = $section_align;
        $section{"section_entsize"} = $section_entsize;
        $section{"section_type_description"} = (($section_type == 0x0)?"Null":
                                                 ($section_type == 0x1)?"ProgBits":
                                                 ($section_type == 0x2)?"Symbol Table":
                                                 ($section_type == 0x3)?"String Table":
                                                 ($section_type == 0x4)?"Rela":
                                                 ($section_type == 0x5)?"Hash":
                                                 ($section_type == 0x6)?"Dynamic":
                                                 ($section_type == 0x7)?"Note":
                                                 ($section_type == 0x8)?"NoBits":
                                                 ($section_type == 9)?"Rel":
                                                 ($section_type == 10)?"ShLib":
                                                 ($section_type == 11)?"DynLib": $section_type );
        $section{"section_flags_description"} = (($section_flags & 0x1)?"Write ":"") . (($section_flags & 0x2)?"Alloc ":"") . (($section_flags & 0x4)?"ExecInstr ":"") . (($section_flags & 0xf0000000)?"MaskProc ":"");
        $section_headers[$section_no] = \%section;



        if ( ( $section_type == 0x3 ) && ( $string_table_location == -1 ) )
        {
            $string_table_location = $section_offset;
        }
    }
    $elf{"section_headers"} = \@section_headers;

    for ( my $section_no = 0; $section_no < $section_header_count; $section_no++ )
    {
        seek $file_handle, $string_table_location + $section_headers[$section_no]->{"section_name_index"}, SEEK_SET;
        my ( $string ) = unpack( "Z*", <$file_handle> );
        $section_headers[$section_no]->{"section_name"} = $string;

        if ( $verbose )
        {
            print "\n\nSection Header $section_no\n";
            print "-----------------------------------------\n";
            print "name : " . $section_headers[$section_no]->{"section_name"} . "\n";
            print "name index: ". $section_headers[$section_no]->{"section_name_index"} . "\n";
            print "type : " . $section_headers[$section_no]->{"section_type_description"} . "\n";
            print "flags : " . $section_headers[$section_no]->{"section_flags_description"}  . "\n";
            printf "Address : 0x%08X\n", $section_headers[$section_no]->{"section_address"};
            print "Offset : " . $section_headers[$section_no]->{"section_offset"} . "\n";
            print "Size : " . $section_headers[$section_no]->{"section_size"} . "\n";
            print "Link : " . $section_headers[$section_no]->{"section_link"} . "\n";
            print "Info : " . $section_headers[$section_no]->{"section_info"} . "\n";
            print "Align : " . $section_headers[$section_no]->{"section_align"} . "\n";
            print "Entry Size : " . $section_headers[$section_no]->{"section_entsize"} . "\n";
            print "-----------------------------------------\n";
        }
    }

    my @program_headers = ();

    for ( my $program_no = 0; $program_no < $program_header_count; $program_no++ )
    {
        seek $file_handle, $program_header_offset + $program_no * $program_header_entry_size, SEEK_SET;
        my ( $program_type, $program_offset, $program_virtual_address, $program_physical_address, $program_file_size, $program_memory_size, $program_flags, $program_align ) = unpack("LLLLLLLL", <$file_handle>);

        my %program = ();
        $program{"program_type"} = $program_type;
        $program{"program_offset"} = $program_offset;
        $program{"program_virtual_address"} = $program_virtual_address;
        $program{"program_physical_address"} = $program_physical_address;
        $program{"program_file_size"} = $program_file_size;
        $program{"program_memory_size"} = $program_memory_size;
        $program{"program_flags"} = $program_flags;
        $program{"program_align"} = $program_align;
        $program{"program_type_description"} = (($program_type == 0x0)?"Null":
                                                 ($program_type == 0x1)?"Load":
                                                 ($program_type == 0x2)?"Dynamic":
                                                 ($program_type == 0x3)?"Interp":
                                                 ($program_type == 0x4)?"Note":
                                                 ($program_type == 0x5)?"ShLib":
                                                 ($program_type == 0x6)?"PHdr": $program_type );
        $program_headers[$program_no] = \%program;

        if ( $verbose )
        {
            print "\n\nProgram Header $program_no\n";
            print "-----------------------------------------\n";
            print "type : " . $program{"program_type_description"} . "\n";
            print "offset : " . $program{"program_offset"} . "\n";
            print "flags : $program_flags\n";
            printf "Virtual Address : 0x%08X\n", $program_virtual_address;
            printf "Physical Address : 0x%08X\n", $program_physical_address;
            print "Size in file: $program_file_size\n";
            print "Size in memory: $program_memory_size\n";
            print "Align : $program_align\n";
            print "-----------------------------------------\n";
        }
    }

    $elf{"program_headers"} = \@program_headers;

    return %elf;
}
