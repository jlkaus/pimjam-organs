#!/usr/bin/perl -w

use strict;
use Getopt::Long;

my %options = ( "output" => "",
	"freq" => 0.0,
	"targetfreq" => 1.0,
	"decibel" => 0,
	"rect" => 1,
	"maxdb" => 0
	);

GetOptions(\%options, "output|o=s","freq|f=f");

my $inputfile = shift @ARGV || die "No input file specified\n";

if($options{output} eq "" && $inputfile =~ m|^(.+)\.ssf|) {
	$options{output} = "$1.fis";
}

if($options{output} =~ m|^(.+)_([0-9.]+)\.dft$|) {
	$options{targetfreq} = $2;
}


# open input file
# read all the lines into array.  
# close input file
# If options{freq} is 0, set it to the read in value of the first line.  If the amplitude in the first line has a dB, set decibel and unset rect, if there are 3 colums, unset rect.
# if things are polar, recalculate all amplitudes as rect
# if decibel is set, find the max db, then renormalize everything using that to rectangular.
# renormalize all frequencies from freq to targetfreq
# sort the list by frequency
# open output file
# write each line after packing as 3 floats (pack "fff",@list)
# close output file
	





exit;

