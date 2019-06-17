#!/usr/bin/perl -w

# Print out the detailed footprint description of a image.
# 
# The algorithm works as follows:
# Step 0: Skip untill you find a string that starts with .text. This is the start of the
# image address space
# Step 1: Since each symbol is in a section of its own, look for .text.symbolname or 
# .data.symbolname and identify the symbol name and the correspond section (text/data). 
# Note that such symbols start with a space as the first character
# Step 2: In some cases, symbols are introduced by the linker script. These do not have a
# space as the first character. Nor is the object name (.a or .o) available for such symbols.
# Deal with this.
# Step 3: Now we have the section (text,data) and symbol name. The same line or the line 
# after that will contain the size and the object name (.a or .o). Read this.
# Now that we have all the information, create a hash object with all this extracted information.
# Display information in a format requested by the user.
# 
# 
# Note: This ignores any fill-bytes and such minor chunks in the analysis. Hence there is a slight
# difference in the output reported by this script and that of 'size'
# 
use strict;

use File::Basename;
use Getopt::Std;

my $flash_start = 0x1f000000;
my $flash_end = 0x1fffffff;
my $fname;
my $start_flag = 0;
my %options;
my $obj_address = 0x0;
sub VERSION_MESSAGE()
{
    print "";
}

sub HELP_MESSAGE() 
{
    print "\nUsage: footprint.pl -m map_file [ -l library ]\n";
    print "         footprint.pl -m map_file [ -s text|rodata|data|bss ]\n";
    print "         footprint.pl -m map_file [ -a mc200|mw300 ]\n";
    print "By default prints the footprint impact of all the libraries.\n";
    print "When used with '-l', lists symbols from a \n";
    print "given library that are included in the image.\n";
    print "When used with '-s', lists symbols from a \n";
    print "given section that are included in the image.\n";
    print "When used with '-a', lists address mappings \n";
    print "for various sections of the image.\n";
    exit;
}

if (getopts('ha:s:l:m:', \%options) != 1) {
    HELP_MESSAGE;
}

HELP_MESSAGE if $options{h};

if ( ! defined($options{m})) {
    HELP_MESSAGE;
}

$fname = $options{m};
if (! -e $fname) {
    print "Error: Can't access $fname\n";
    exit 1;
}

my @address_map;
my $cur_section_hdr = "";
my $g_minimum_heap_hex;

open TFILE, $fname;
# skip upto .text
while (my $readl = <TFILE>) {
    if ($readl =~ /^\.([^\.\s]*)\s*0x([0-9a-f]*)\s*0x([0-9a-f]*)/) {
#	print "$readl";
	$cur_section_hdr = $1;
	my $start_addr = $2;
	push(@address_map, { name => $cur_section_hdr,
			     start => $start_addr,
			     size => $3
	     }
	    );
	$start_flag = 1;
	last;
    } elsif ($readl =~ m/_heap_size/) {
	my @heap_data = split /\s+ /, $readl;
	$g_minimum_heap_hex = $heap_data[1];
    }
}

if (!$start_flag) {
    die "No .text found";
}

# Process .text
# This contains both .text and .rodata
#
my $mode = "text";
my $symbol = "default";
my $size = "-1";
my $object = "default";

my %bigdata;
my $g_heapsize = 0;
my $g_heapstart = 0;
my $g_heap2start = 0;
my $g_heapend = 0;
my $g_heap2end = 0;

while (my $readl = <TFILE>) {
# This can either be a .text line with size and all on the same line
# or it could be a remnant of the previous .text line
    $readl =~ s/\r\n$/\n/;
#    print "new line : $readl";
    if ($readl =~ /^OUTPUT\(/) {
	last;
    }

    if ($readl =~ /^ \.([^\.\s]*)\.([\S]*)/) {
	# This line contains the region and symbol name at the least
	$mode = $1;
	$symbol = $2;
    } elsif ($readl =~ /^ COMMON/) {
	$mode = "common";
    } elsif ($readl =~ m/0x([0-9a-f]*)\s*_heap_start/) {
	$g_heapstart = $1;
	next;
    } elsif ($readl =~ m/0x([0-9a-f]*)\s*_heap_end/) {
	$g_heapend = $1;
	$mode = "bss";
	$object = "linker_script";
	$size = hex($g_heapend) - hex($g_heapstart);
	$symbol = "heap1";
	push(@address_map, { name => "heap1",
			     start => $g_heapstart,
			     size => sprintf("0x%x", $size),
			     comment => "note1"
	     }
	    );
	$bigdata{$object}{$mode}{size} += $size;
	$bigdata{$object}{$mode}{symbols}{$size} .= " $symbol($size)";
	next;
    } elsif ($readl =~ m/0x([0-9a-f]*)\s*_heap_2_start/) {
	$g_heap2start = $1;
	next;
    } elsif ($readl =~ m/0x([0-9a-f]*)\s*_heap_2_end/) {
	$g_heap2end = $1;
	$mode = "bss";
	$object = "linker_script";
	$size = hex($g_heap2end) - hex($g_heap2start);
	$symbol = "heap2";
	push(@address_map, { name => "heap2",
			     start => $g_heap2start,
			     size => sprintf("0x%x", $size),
			     comment => "note1"
	     }
	    );
	$bigdata{$object}{$mode}{size} += $size;
	$bigdata{$object}{$mode}{symbols}{$size} .= " $symbol($size)";
	next;
    } elsif ($readl =~ m/_actual_heap_size/) {
	my @heap_data = split /\s+ /, $readl;
	$g_heapsize = hex ($heap_data[1]);
    } elsif ($readl =~ /^\.([^\.\s]*)\s*0x([0-9a-f]*)\s*0x([0-9a-f]*)/) {
#	print "$readl";
	$cur_section_hdr = $1;
	my $start_addr = $2;
	push(@address_map, { name => $cur_section_hdr,
			     start => $start_addr,
			     size => $3
	     }
	    );
#	print "Section hdr :$1:\n";
	# This matches the sections names along with their size in the map
	# We need to use this to determine some components that are 
	# directy addedin the linker script. These of course, are accounted
	# as bss. We ignore the standard sections, and everything else gets
	# added to the linker script.
#	$mode = "bss";
#	$symbol = $1;
#	if ($symbol eq "data" || $symbol eq "bss" || $symbol eq "text" ||
#	    $symbol =~ /debug_/) {
	    # ignore aggregate or debug symbols
#	    next;
#	}
#	if (hex($2) == 0) {
	    # ignore symbols with 0 size
#	    next;
#	}
	# Ensure that the logic below detects the object as linker_script
#	$readl .= "linker_script";
	next;
   } elsif ($readl =~ /^ \.([^\.\s]*)\s/) {
       $mode = "";
       foreach my $section ("text", "rodata", "data", "bss", "common") {
	   if ($1 eq $section) {
	       $mode = $1;
	       last;
	   }
       }
       if ($mode eq "") {
	   $mode = "bss";
	   $symbol = $1;
       } else {
	   $symbol = "unknown";
       }
   } elsif ($readl =~ /^ \*fill\*\s*0x[0-9a-f]*\s*0x([0-9a-f]*)/) {
       $mode = "bss";
       $symbol = $cur_section_hdr;
       $size = hex($1);
       $object = "linker_script";
       $bigdata{$object}{$mode}{size} += $size;
       $bigdata{$object}{$mode}{symbols}{$size} .= " $symbol($size)";
 #      print " $symbol($size)\n";
       next;
   } else {
#       print "Ignoring line $readl\n";
       next;
   }

    if ($readl =~ /0x[0-9a-f]*\s*0x([0-9a-f]*)\s(.*)/) {
	$size = hex($1);
	$object = $2;
    } else {
	# This line is spilled over to the next line, process that.
	$readl = <TFILE>;
	$readl =~ s/\r\n$/\n/;
#	print "new line: $readl";
	if ($readl =~ /0x([0-9a-f]*)\s*0x([0-9a-f]*)\s(.*)/) {
	    $obj_address = hex($1);
	    $size = hex($2);
	    $object = $3;
	}
    }
    if ($object ne "linker_script") {
	if ($obj_address == 0) {
		# Skip symbols with address 0, since they are not pulled in
		next;
	}
    }

    if ($size == 0) {
	# No use for symbols with 0 size
	next;
    }
    if ($object =~ /([\S]*\.a)/) {
	$object = basename($1);
    }
#    print "$mode $symbol $size $object\n";
    $bigdata{$object}{$mode}{size} += $size;
    $bigdata{$object}{$mode}{symbols}{$size} .= " $symbol($size)";
}

sub total_size($)
{
    my $libref = $_[0];
    my $total = 0;

    foreach my $section ("text", "rodata", "data", "bss", "common") {
	if (defined($libref->{$section}{size})) {
#	    print "$section $libref->{$section}{size}\n";
	    $total += $libref->{$section}{size};
	}
    }
    return $total;
}

sub print_all()
{
    printf("%-30s %10s %10s %10s %10s %10s %10s\n", "Library", "text", "rodata", "data", "bss", "common", "Total");
    printf("##########################\n");
    my %totals;
    $totals{text} = $totals{rodata} = $totals{data} = $totals{bss} = $totals{common} = 0;
    foreach $object (sort { total_size($bigdata{$b}) <=> total_size($bigdata{$a}) } (keys %bigdata)) {
	if ($object eq "linker_script") {
	    next;
	}

	printf("%-30s", "$object");
	if (defined($bigdata{$object}{text}{size})) {
	    printf(" %10s", " $bigdata{$object}{text}{size}");
	    $totals{text} += $bigdata{$object}{text}{size}
	} else { printf(" %10s", " 0"); }
	
	if (defined($bigdata{$object}{rodata}{size})) {
	    printf(" %10s",  " $bigdata{$object}{rodata}{size}");
	    $totals{rodata} += $bigdata{$object}{rodata}{size}
	} else { printf(" %10s",  " 0"); }
	
	if (defined($bigdata{$object}{data}{size})) {
	    printf(" %10s",  " $bigdata{$object}{data}{size}");
	    $totals{data} += $bigdata{$object}{data}{size}
	} else { printf(" %10s",  " 0"); }
	
	if (defined($bigdata{$object}{bss}{size})) {
	    printf(" %10s", " $bigdata{$object}{bss}{size}");
	    $totals{bss} += $bigdata{$object}{bss}{size}
	} else { printf(" %10s", " 0"); }
	
	if (defined($bigdata{$object}{common}{size})) {
	    printf(" %10s", " $bigdata{$object}{common}{size}");
	    $totals{common} += $bigdata{$object}{common}{size}
	} else { printf(" %10s", " 0"); }
	printf(" %10s", total_size($bigdata{$object}));
	print "\n";
    }

    printf("##########################\n");
    printf("%-30s %10s %10s %10s %10s %10s\n", " ", "text", "rodata", "data", "bss", "common");
    printf("%-30s %10s %10s %10s %10s %10s\n", "Totals:", $totals{text}, $totals{rodata}, $totals{data}, $totals{bss}, $totals{common});
    print "   + \n";
    printf("%-30s %10s\n", "linker_script", $bigdata{"linker_script"}{bss}{size});
    my $final = $totals{text} + $totals{rodata} + $totals{data} + $totals{bss} + $totals{common} + $bigdata{"linker_script"}{bss}{size};
    printf("%-30s %d", "Final:", $final);
    printf(" (%d KB )\n", $final / 1024);
    
    printf("##########################\n");
    printf("As per 'size' utility:\n");
    printf(".text = text + rodata = %d\n", $totals{text} + $totals{rodata});
    printf(".data = data = %d\n", $totals{data});
    printf(".bss = bss + common = %s\n", $totals{bss} + $totals{common});
}

sub print_library($)
{
    my ($library) = @_;
    my $syms;

    if (!defined($bigdata{$library})) {
	print "Error: No such library is in the map file\n";
	exit 1;
    }

    print "Library: $library\n\n";
    if (defined($bigdata{$library}{text}{symbols})) {
	print "text:\n";
	foreach $syms (sort {$b <=> $a} (keys (%{$bigdata{$library}{text}{symbols}}))) {
	    print "$bigdata{$library}{text}{symbols}{$syms}";
	}
	print "\n\n";
    }
    if (defined($bigdata{$library}{rodata}{symbols})) {
	print "rodata:\n";
	foreach $syms (sort {$b <=> $a} (keys (%{$bigdata{$library}{rodata}{symbols}}))) {
	    print "$bigdata{$library}{rodata}{symbols}{$syms}";
	}
	print "\n\n";
    }
    if (defined($bigdata{$library}{data}{symbols})) {
	print "data:\n";
	foreach $syms (sort {$b <=> $a} (keys (%{$bigdata{$library}{data}{symbols}}))) {
	    print "$bigdata{$library}{data}{symbols}{$syms}";
	}
	print "\n\n";
    }
    if (defined($bigdata{$library}{bss}{symbols})) {
	print "bss:\n";
	foreach $syms (sort {$b <=> $a} (keys (%{$bigdata{$library}{bss}{symbols}}))) {
	    print "$bigdata{$library}{bss}{symbols}{$syms}";
	}
	print "\n\n";
    }
    print "common:\n Couldn't infer. Refer to map.txt\n\n";
}

sub print_section($)
{
    my ($sec) = @_;
    my %sec_info;

#    print ":$sec:\n";
    foreach my $l (keys %bigdata) {
#	print "lib:$l:\n";
	if (! defined($bigdata{$l}{$sec}{symbols})) {
	    next;
	}
	foreach my $size (keys %{$bigdata{$l}{$sec}{symbols}}) {
	    $sec_info{$size} .= $bigdata{$l}{$sec}{symbols}{$size};
	}
    }

    foreach my $size (sort {$b <=> $a} (keys %sec_info)) {
	print "$sec_info{$size}";
    }
}

sub get_memory_regions($)
{
    my $processor = $_[0];
    my @mem_regions;
    if ($processor eq "mc200" || $processor eq "mw300") {

	push(@mem_regions, {  name => "SRAM0",
			     start => "00100000",
			     size => 0,
			     boundary => "start"
	     });
	push(@mem_regions, {  name => "SRAM0(end)",
			     start => "00160000",
			     size => 0,
			     boundary => "end"
	     });
	push(@mem_regions, {  name => "SRAM1",
			      start => "20000000",
			      size => 0,
			      boundary => "start"
	     });
	push(@mem_regions, {  name => "SRAM1(end)",
			      start => "20020000",
			      size => 0,
			      boundary => "end"
	     });
	push(@mem_regions, {  name => "NVRAM",
			      start => "480C0000",
			      size => 0,
			      boundary => "start"
	     });
	push(@mem_regions, {  name => "NVRAM(end)",
			      start => "480C1000",
			      size => 0,
			      boundary => "end"
	     });
	if ($processor eq "mw300") {
		push(@mem_regions, {  name => "FLASH",
				      start => '1f000000',
				      size => 0,
				      boundary => "start"
		});
		push(@mem_regions, {  name => "FLASH(end)",
				      start => get_flash_end_addr(),
				      size => 0,
				      boundary => "end"
		});

	}
	return @mem_regions;

    } else {
	printf("Processor:\"$processor\" not found\r\n");
	return;
    }

}

sub get_flash_end_addr()
{
   my $flash_size = 0;
   my @mem_regions;
   my $end_addr = 0;

   foreach my $section (sort{hex($$a{start}) <=> hex($$b{start}) }@address_map) {
		if ((hex($$section{start}) > $flash_start) && ((hex($$section{start}) < $flash_end))) {
			$end_addr = hex($$section{size}) + hex($$section{start});
			#$flash_size += hex($$section{size});
		}
	}
	$end_addr = sprintf("%x", $end_addr);
	return $end_addr;
}

sub print_addresses($)
{
    my $processor = $_[0];
    my @mem_regions = get_memory_regions($processor);

    exit(1) if !@mem_regions;
    # Push the map AFTER the memory regions, helps in sorting
    push (@mem_regions, @address_map);
    my $end_addr = 0;
    printf("%-30s %10s %10s %10s %10s\n", "section", "start", "end", "size", "comment");
    foreach my $section (sort { hex($$a{start}) <=> hex($$b{start}) } @mem_regions) {
	if (!defined $$section{comment}) {
	    $$section{comment} = "";
	}
	# There are other debug sections with zero sizes, ignore them,
	# but include our boundary markers with zero sizes
	if ($$section{size} || defined($$section{boundary})) {
	    # If there is a hole from the previous end address, print that hole.
	    if ($end_addr && $end_addr ne $$section{start} &&
		(! defined($$section{boundary}) || $$section{boundary} ne "start")) {
		# Unless of course this is a boundary start, since in that case the memory
		# doesn't exist
		printf("%-30s %10s %10s %10s\n", "--Free--", "0x$end_addr", "0x$$section{start}",
		       "(".(hex($$section{start}) - hex($end_addr)).")");
	    }
	    $end_addr = sprintf("%08x", hex($$section{start}) + hex($$section{size}));

	    if (defined($$section{boundary})) {
		if ($$section{boundary} eq "start") {
		    print "---------------------------------------------------------------------------\n";
		    print "$$section{name}:\n";
		}
	    } else {
		printf("%-30s %10s %10s %10s %10s\n", "$$section{name}", "0x$$section{start}",
		       "0x$end_addr", "".hex($$section{size}), $$section{comment});
	    }
	}
    }
    print "---------------------------------------------------------------------------\n";
    print "note1: Actual Heap Size = heap1 + heap2\n";
    my $sram0free = hex($g_heapend) - hex($g_heapstart);
    my $sram1free = hex($g_heap2end) - hex($g_heap2start);
    print "       Actual Heap Size = $sram0free + $sram1free = $g_heapsize\n";
}

if (defined $options{l}) {
    # print information specific to this library
    print_library($options{l});
} elsif (defined $options{s}) {
    # print information specific to this library
    print_section($options{s});
} elsif (defined $options{a}) {
    print_addresses($options{a});
} else {
    print_all();
}
