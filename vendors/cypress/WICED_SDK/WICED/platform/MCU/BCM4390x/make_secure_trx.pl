#!/usr/bin/perl
#
# $ Copyright Cypress Semiconductor $
#
# Adds an TRX (External Program Memory) header to
# encrypted and signed bin file, to allow it to be booted from serial flash

my ($trx, $bin, $out) = @ARGV;

# read and parse original trx file header
open TRX, '<', $trx or die $!;
binmode TRX;

my @trx_header = unpack('IIIIIIIII', <TRX>);
my ($magic, $total_size, $crc, $flag_ver, $size, $entry_point,
    $reserved0, $reserved1, $secure_delay) = @trx_header;

close TRX;

# get signed and encrypted file size
my $bin_size = -s $bin;

printf "\n[Original size in TRX header]\n";
printf "Total Size            : %d Bytes\n", $total_size;
printf "Size                  : %d Bytes\n", $size;
printf "Secure Option & Delay : 0x%08x\n", $secure_delay;

# update file size
$total_size = $bin_size + 36;
$size = $bin_size;
# default
# secure boot enabled, hmac-sha256 signed, aes-cbc128 encrypted, delay 400
# $secure_delay =    0x01900000;
# experimentally, default otp init delay, 20, is good enough.
$secure_delay =    0x00000000;

printf "\n[New aligned size in TRX header]\n";
printf "Total Size            : %d Bytes\n", $total_size;
printf "Size                  : %d Bytes\n", $size;
printf "Secure Option & Delay : 0x%08x\n", $secure_delay;

open BIN, '<:raw', $bin or die $!;
open OUT, '>:raw', $out or die $!;

seek OUT, 0, SEEK_SET;
print OUT pack('I', $magic);
print OUT pack('I', $total_size);
print OUT pack('I', $crc);
print OUT pack('I', $flag_ver);
print OUT pack('I', $size);
print OUT pack('I', $entry_point);
print OUT pack('I', $reserved0);
print OUT pack('I', $reserved1);
print OUT pack('I', $secure_delay);

my $buffer, $n;

while (($n = read BIN, $buffer, 16 * 1024) != 0) {
    print OUT $buffer;
}

close BIN;
close OUT;

print "\nDone.\n"
