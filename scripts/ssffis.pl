#!/usr/bin/perl -w

use strict;
use Getopt::Long;

my %options = ( "output" => "",
		"freq" => 0.0,
		"targetfreq" => 1.0,
		"decibel" => 0,
		"rect" => 1,
		"maxdb" => 0,
		"divisor" => 1.0,
		"help" => 0
    );

GetOptions(\%options, "output|o=s","freq|f=f","help|h");

if($options{help}) {
    print "ssffis.pl - Convert a ssf file to a fis or dft file.\n";
    print "  -o,--output=FILE	Output file specifier\n";
    print "  -f,--freq=FREQ     Source frequency base of the ssf file\n";
    print "  -h,--help		Display this help\n";
    print "  INPUTFILE		Unnamed argument is input ssf filename.\n";
    exit(1);

}

my $inputfile = shift @ARGV || die "No input file specified\n";

if($options{output} eq "" && $inputfile =~ m|^(.+)\.ssf|) {
    $options{output} = "$1.fis";
}

if($options{output} =~ m|^(.+)_([0-9.]+)\.dft$|) {
    $options{targetfreq} = $2;
}


open(INPF, "<$inputfile");
my @inlines = <INPF>;
close(INPF);
chomp(@inlines);

my @things = split(/,/ , $inlines[0]);

if($options{freq} == 0) {
    $options{freq} = $things[0];
}
if(scalar @things == 3) {
    $options{rect} = 0;
}
if($things[1] =~ /^\s*([-0-9.]+)\s*[dD][bB]\s*([-0-9.]+)?\/?([-0-9.]+)?\s*$/) {
    $options{rect} = 0;
    $options{decibel} = 1;
    $options{maxdb} = $1;
    if($2 ne "") {
	if($3 ne "") {
	    $options{divisor} = $2/$3;
	} else {
	    $options{divisor} = $2;
	}
    } else {
	$options{divisor} = 1.0;
    }
}

if($options{decibel} == 1) {
    foreach(@inlines) {
	@things = split(/,/,$_);
	if($things[1] =~ /^\s*([-0-9.]+)\s*([dD][bB])?\s*$/) {
	    if($1 > $options{maxdb}) {
		$options{maxdb} = $1;
	    }
	}
    }
}

my @outrecs = ();
my $cf= 0.0;
my $cr = 0.0;
my $ci = 0.0;
foreach(@inlines) {
    @things = split(/,/,$_);
    $cf = $options{targetfreq} * $things[0]/$options{freq};
    if($options{decibel} && $things[1] =~ /^\s*([-0-9.]+)\s*([dD][bB])?\s*([-0-9.\/]*)$/) {
	$cr = 10.0**(($1 - $options{maxdb})/20/$options{divisor});
#	print "Max: $options{maxdb}, divisor: $options{divisor}, data: $1, computed: $cr\n";
	$ci = 0.0;
    } elsif($options{rect} && $things[1] =~ /^\s*([-0-9.]*)\s*([-+])\s*j\s*([0-9.]*)\s*$/) {
	$cr = $1;
	$ci = "${2}1.0" * $3;
    } elsif($options{rect} && $things[1] =~ /^\s*([-0-9.]+)\s*$/) {
	$cr = $1;
	$ci = 0.0;
    } elsif(!$options{rect} && !$options{decibel} && scalar @things == 3) {
	$cr = $things[1] * cos($things[2]);
	$ci = $things[1] * sin($things[2]);
    } else {
	die "Didn't understand input line $_\n";
    }
    push @outrecs,[$cf, $cr, $ci];
}

my @soutrecs = sort {$a->[0] <=> $b->[0]} @outrecs;

open(OUTPF, ">${options{output}}");

foreach(@soutrecs) {
    print OUTPF pack("fff",@{$_});
}

close(OUTPF);	

exit;

