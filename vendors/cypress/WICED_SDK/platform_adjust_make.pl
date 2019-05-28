#
# Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 # Cypress Semiconductor Corporation. All Rights Reserved.
 # 
 # This software, associated documentation and materials ("Software")
 # is owned by Cypress Semiconductor Corporation,
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

use strict;
use warnings;

# Descriptions:
# 1. It runs without any arguments
# 2. Opens the .cproject files specified in @projfiles array and replaces
#    the make.exe appropriately for non-windows platform. By default, this file
#    points to make.exe and for any non-windows, this needs to be replaced with "make".
# 3. If any new .cproject file added with new app or with new platform, that need to be
#    included in @projfiles array with full path.

my @projfiles=("../../../../../tests/cypress/CYW954907AEVAL1F/wicedstudio/.cproject",
               "../../../../../demos/cypress/CYW954907AEVAL1F/wicedstudio/.cproject",
               "../../../../../tests/cypress/CYW943907AEVAL1F/wicedstudio/.cproject", "../../../../../demos/cypress/CYW943907AEVAL1F/wicedstudio/.cproject");
my @execfiles;
my ($cmd, $status);
if ($^O =~ m/darwin/i) {
    # TODO: Revisit this method of setting the permissions selectively.  For some reasons, it does not work.
    #$cmd = "find . -type f -exec file {} \\; | grep executable | grep -v \"\\.pl\" | grep -v \"\\.elf\" | grep -v \"MS Windows\" | grep -v \"GNU/Linux\" | cut -d: -f1 | cut -d\\\( -f1";
    $cmd = "if ((test -d Tools && chmod -R 755 Tools/) || (test -d tools && chmod -R 755 tools/)) then echo Found Tools or tools folder; fi";
} elsif ($^O =~ m/linux/i) {
    #$cmd = "find . -type f -exec file {} \\; | grep executable | grep -v \"\\.pl\" | grep -v \"\\.elf\" | grep \"GNU/Linux\" | cut -d: -f1 | cut -d\\\( -f1";
    $cmd = "if ((test -d Tools && chmod -R 755 Tools/) || (test -d tools && chmod -R 755 tools/)) then echo Found Tools or tools folder; fi";
}
print("DEBUG:[$cmd]\n");
print("DEBUG:OS:[$^O]\n");

$status = `$cmd`;
print("DEBUG:[$status]\n");
print("Permissions on Tools folder set.\n");

# TODO: Uncomment this block, when the selective permissions setting issue is resolved.
#if (($status ne "") && (!($status =~ m/No such file or directory/))) {
#    @execfiles = split("\n", $status);
#    chmod 0755, @execfiles;
#    print("Permissions on SDK folder set.\n");
#}

# Determine platform

my $platform = 0; # Assume it is windows

if (!(($^O =~ m/msys/) || ($^O =~ m/Win/))) {

    $platform = 1;

    print("Non Windows Platform.[$^O]\n");

}


if ($platform == 0) {print("No operation by this script in Windows platform.[$^O]\n"); exit (0);}

foreach my $file (@projfiles)
{
	print(".cproject full path $file");
	if (!(-f "$file")) {
	    print("No operation by this script as no .cproject file exists in the current directory\n"); next;
	}


	# Read the entire .cproject file
	my $filecontents;
	eval {open (CPROJ, "<$file");};
	if ($@) {
	    print("ERROR:Unable to open the file [$file] for reading.$@\n"); next;
	} else {
	    local $/ = undef;
	    binmode CPROJ;
	    $filecontents = <CPROJ>;
	    close CPROJ;
	}

	# Do the replacement
	my $replacement_count = () = $filecontents =~ m/make\.exe/gi;
	if ($replacement_count == 0) {print("No need to perform replacements.[Replacement Count:$replacement_count]\n"); next}
	print("Total replacements [$replacement_count]...");
	$filecontents =~ s/make\.exe/make/g;
	$filecontents =~ s/HOST_OS\=Win32//g;
	print("Done.\n");

	# Write the entire file back
	eval {open (CPROJ, ">$file");};
	if ($@) {
	    print("ERROR:Unable to open the file [$file] for writing.$@\n"); next;
	} else {
	   print (CPROJ $filecontents);
	   close CPROJ;
	   print("SUCCESS:The file [$file] is written successfully.\n");
	}
}
#
exit (0);


