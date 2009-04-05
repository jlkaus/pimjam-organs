#!/usr/bin/perl -w

use strict;
use POSIX;

# rankname basepipes length breakclass detune notes belows aboves
#Except for the the fundamental, the name of the pipe spectra is used to determine the rank. Another name can be given as a memory aid. 
#We also can specify a delta, in cents, to detune the pipes from equal temperament.
#We specify a length class of the pipes, in fractional feet.
#The final thing we can specify how many notes to generate, and also if we want to add an octave below, or an octave above to allow for offset coupling.

# break interval indicates how much to drop each pitch at the breaks, if this were a mixture.  0 indicates no drops throughout. 3 indicates to drop a third every 8 keys. 5 indicates to drop a fifth every 12 keys. 8 indicates to drop an octave every 24 keys.

# NOTE: Only breakclass 0 and 8 are supported at this time because the higher ones aren't regular and are kinda sucky.  Also, the math doesn't work for them here.  Especially if the lengthclass isn't an octave, fifth, or third.  (and it doesn't work for those either...)

# Breakclass 8 indicates to drop an octave every 24 keys, so we use floordiv24 to map:
my %break8 = (-1 => 4,
	      0 =>  8,
	      1 => 16,
	      2 => 32,
	      3 => 64
    );
# breakclass 5 indicates to drop a fifth every 12 keys, so we use floordiv12 to map:
my %break5 = (-2 => 4,
	      -1 => 5+1/3,
	      0 => 8,
	      1 => 10+2/3,
	      2 => 16,
	      3 => 21+1/3,
	      4 => 32,
	      5 => 42+2/3,
	      6 => 64
    );
# breakclass3 indicates to drop a third every 8 keys, so we use floordiv8 to map:
my %break3 = (-3 => 4,
	      -2 => 5+1/3,
	      -1 => 6+2/5,
	      0 => 8,
	      1 => 10+2/3,
	      2 => 12+4/5,
	      3 => 16,
	      4 => 21+1/3,
	      5 => 25+3/5,
	      6 => 32,
	      7 => 42+2/3,
	      8 => 51+1/5,
	      9 => 64
    );

my %bctodiv = (8 => 24,
	       5 => 12,
	       3 => 8
    );

my %bctobreak = (8 => \%break8,
		 5 => \%break5,
		 3 => \%break3
    );



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

open OPF, ">${rankname}_${lengthclass}${us}${bc}${dt}.rank";


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

}

close(OPF);







exit;
