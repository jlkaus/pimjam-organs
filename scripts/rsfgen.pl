#!/usr/bin/perl -w

use strict;
use POSIX;
use Getopt::Long;

my %options = (
	keys => 61,
	aboves => 0,
	belows => 0,
	detune => 0.0,
	output => "",
	length => "8",
	canlen => 0
	);
GetOptions(\%options, "keys|k=i","aboves|a=i","belows|b=i","detune|d=f","output|o=s", "length|l=s");

# process remaining argv to get input fis files, and lengths
# [[file][:len]*]*
# nothing    -> base:8
# file       -> file:8
# :len       -> base:len
# file:len   -> file:len
# :l1:l2     -> base:l1 and base:l2
# file:l1:l2 -> file:l1 and file:l2

my @breaks = ();

# normalize breaks, and set output if it wasn't given.  May need to set the length as well, if not specified, and not in output filename.
# breaks should be sorted by break length, with shortest lenghts are at the top (left), under the assumption that as we get higher pitched, we have to break lower pitched, so that we still have pipes that are manageable. breaks with the same length but different names should be left in original order.
# also need to figure out, based on number of breaks, and number of keys, where the breaks are at:
#   prefer key 0 to start a break set. work backwords for negative keys
#   keys-1 + aboves*12 + belows*12 -1 is adjusted number of keys.  The last key should use the length of the last break set.
#   adjusted number of keys / number of breaks should equal the keys per break.  Set key 0 to use the canonical length, and find the leftmost cannonical length input file to put there.  then work the way right and left.
#   prefered keys per break (not counting aboves and belows) are
#     60   for normal ranks
#     30   for split ranks
#     24   drop octaves for mutations
#     12   drop fifths for mutations
#     8    drop thirds for mutations



my $rankname = shift @ARGV or die "No rankname: create-rank.pl rankname pipespec length breakclass detune notes belows aboves\n";
my $basepipes = shift @ARGV or die "No pipe spec: create-rank.pl rankname pipespec length breakclass detune notes belows aboves\n";

my $lengthclass = shift @ARGV || 8;

my $lenfactor = 8;
if($lengthclass =~ m|^([0-9]+)$|) {
    $lenfactor = $1;
} elsif($lengthclass =~ m|^([0-9]+)-([0-9]+)/([0-9]+)$|) {
    $lenfactor = $1 + $2/$3;
} elsif($lengthclass =~ m|^([0-9]+)/([0-9]+)$|) {
    $lenfactor = $1/$2;
} else {
    die "Invalid length class specified.\n";
}

my $breakclass = shift @ARGV || 0;
my $bc="";
if($breakclass == 8) {
    $bc="b";
} elsif($breakclass ==0) {
    $bc ="";
} else {
    die "Don't currently support given breakclass. Only support 0 and 8.\n";
}

my $detune = shift @ARGV || 0.00;
my $dt = "";
if($detune < -0.00001) {
    $dt = "-";
} elsif($detune > 0.00001) {
    $dt = "+";
} else {
    $dt = "";
}

my $us = "";
if($bc ne "" || $dt ne "") {
    $us = "_";
}
my $notes = shift @ARGV || 61;
my $belows = shift @ARGV || 0;
my $aboves = shift @ARGV || 0;

#print "I\tLC\tBRC\tTD\tDEX\tBSC\tLF\tEL\tF\n";

open OPF, ">${rankname}_${lengthclass}${us}${bc}${dt}.ranktxt";


my $i = 0;
for($i = 0 - 12*$belows; $i < 0 + $notes + 12*$aboves; ++$i) {
    my $baseclass = 8.0;
 #   print "$i\t$lengthclass\t$breakclass\t";
    if($breakclass > 0) {
	my $td = $bctodiv{$breakclass};
	my $dex = POSIX::floor($i/$td);
	$baseclass = $bctobreak{$breakclass}->{$dex};
	#print "$td\t$dex\t";
    } else {
	#print "0\t0\t";
    }
    my $eff_len = $lenfactor * $baseclass / 8.0;
    my $freq = (8.0 * 110.0 / $eff_len) * 2.0**((-900.0 + 100.0*$i + $detune)/1200.0);
    #print "$baseclass\t$lenfactor\t$eff_len\t$freq\n";
    print "${rankname}_${lengthclass}${us}${bc}${dt}.rank ($i): ${basepipes}_${freq}.pipe\n";
    print OPF "$i: ${basepipes}_${freq}.pipe\n";
    system("make ${basepipes}_${freq}.raw");
    system("make ${basepipes}_${freq}.pipe");

}

close(OPF);







exit;
