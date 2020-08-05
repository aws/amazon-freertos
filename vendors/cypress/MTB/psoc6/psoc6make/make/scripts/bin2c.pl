#!/usr/bin/perl

#
# Copyright 2019-2020, Cypress Semiconductor Corporation or a subsidiary of
# Cypress Semiconductor Corporation. All Rights Reserved.
#
# This software, including source code, documentation and related
# materials ("Software"), is owned by Cypress Semiconductor Corporation
# or one of its subsidiaries ("Cypress") and is protected by and subject to
# worldwide patent protection (United States and foreign),
# United States copyright laws and international treaty provisions.
# Therefore, you may use this Software only as provided in the license
# agreement accompanying the software package from which you
# obtained this Software ("EULA").
# If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
# non-transferable license to copy, modify, and compile the Software
# source code solely for use in connection with Cypress's
# integrated circuit products. Any reproduction, modification, translation,
# compilation, or representation of this Software except as specified
# above is prohibited without the express written permission of Cypress.
#
# Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
# reserves the right to make changes to the Software without notice. Cypress
# does not assume any liability arising out of the application or use of the
# Software or any product or circuit described in the Software. Cypress does
# not authorize its products for use in any products where a malfunction or
# failure of the Cypress product may reasonably be expected to result in
# significant property damage, injury or death ("High Risk Product"). By
# including Cypress's product in a High Risk Product, the manufacturer
# of such system or application assumes all risk of such use and in doing
# so agrees to indemnify Cypress against all liability.
#

if (! $ARGV[0] )
{
    print "Usage ./bin2c.pl  <variable_name> <text file>";
    exit;
}

# Print start of output
my $variable_name = shift @ARGV;
my $file = shift @ARGV;

# open the file
open INFILE, "<:raw", $file or die "cant open " . $file;
@file_cont_array = <INFILE>;
close INFILE;
$file_cont = join('',@file_cont_array);

# Format names
$variable_name =~ s/-/_/g;
$variable_name_uc = uc ${variable_name};

# Print head 
print "/******************************************************************************* \n";
print "* Auto-generated from ${file} \n";
print "*******************************************************************************/ \n";
print "\n";
print "#include <stdint.h> \n";
print "\n";
print "#if defined(__APPLE__) && defined(__clang__) \n";
print "__attribute__ ((__section__(\"__${variable_name_uc},__${variable_name}\"), used)) \n";
print "#elif defined(__GNUC__) || defined(__ARMCC_VERSION) \n";
print "__attribute__ ((__section__(\".${variable_name}\"), used)) \n";
print "#elif defined(__ICCARM__) \n";
print "#pragma  location=\".${variable_name}\" \n";
print "#else \n";
print "#error \"An unsupported toolchain\" \n";
print "#endif \n";
print "\n";

$name = $file;
$name=~s/^.+\/(.*?)/$1/;

print "const uint8_t ${variable_name}[" . (length( $file_cont )) . "] = {\n";
my @vals = unpack( "C*", $file_cont );

my $linepos;
my $firstval = 1;

foreach $val (@vals)
{
    my $valsize = 1;
    if ( ( $val >= 10 ) && ( $val < 100 ) )
    {
        $valsize = 2;
    }
    elsif ( $val >= 100 )
    {
        $valsize = 3;
    }

    if ( $firstval == 1 )
    {
        print "        $val";
        $linepos = 8 + $valsize;
        $firstval = 0;
    }
    elsif ( $linepos + 5 >= 79 )
    {
        print ",\n        $val";
        $linepos = 8 + $valsize;
    }
    else
    {
        print ", $val";
        $linepos += 2 + $valsize;
    }
}

print "\n};\n";
