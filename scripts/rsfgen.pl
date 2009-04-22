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
	canlen => 8.0
	);
GetOptions(\%options, "keys|k=i","aboves|a=i","belows|b=i","detune|d=f","output|o=s", "length|l=s");

sub computeLength {
    my $lengthclass = shift || "";

    my $lenfactor = shift || 8.0;
    if($lengthclass =~ m|^([0-9]+)\s*$|) {
	$lenfactor = $1;
    } elsif($lengthclass =~ m|^([0-9]+)-([0-9]+)/([0-9]+)\s*$|) {
	$lenfactor = $1 + $2/$3;
    } elsif($lengthclass =~ m|^([0-9]+)/([0-9]+)\s*$|) {
	$lenfactor = $1/$2;
    }

    return $lenfactor;
}

$options{canlen} = computeLength($options{length},$options{canlen});



# process remaining argv to get input fis files, and lengths
# [[file][:[len]]*]*
# nothing    -> base:8
# :	     -> last:8
# file       -> file:8
# :len       -> last:len
# file:      -> file:8
# file:len   -> file:len
# :l1:l2     -> last:l1 and last:l2
# file:l1:l2 -> file:l1 and file:l2

my @breaks = ();
my @lengths = ();
my $nextname = $options{output};
$nextname =~ s/^(.*)\.rsf$/$1/;

foreach(@ARGV) {
    if(/^([^:]*)$/) {
	$nextname = "$1";
	$nextname =~ s/\.fis$//;
    } elsif(/^([^:]*):(.*)$/) {
	if("$1" ne "") {
	    $nextname = "$1";
	    $nextname =~ s/\.fis$//;
	}
	@lengths = split(':', "$2 ",-1);
	foreach(@lengths) {
	    if($nextname eq "") {
		die "Couldn't figure out what name to use for inputs.  Aborting.\n";
	    }
	    push @breaks,["$nextname", computeLength("$_",$options{canlen})];
	}
    }
    if($options{output} eq "") {
	if("$nextname" ne "") {
	    $options{output} = "$nextname";
	    $options{output} =~ s/^(.*)\.fis$/$1.rsf/;
	}
    }
}
if(scalar @breaks == 0) {
    push @breaks, [$nextname, computeLength($_,$options{canlen})];
}

#print "Output: $options{output}\n";
#print "  $_->[1] $_->[0]\n" foreach(@breaks);

# compute num keys and num breaks, keys/break, etc
my $numkeys = $options{keys} + $options{aboves}*12 + $options{belows}*12;
my $cankeys = $numkeys -1;
my $subkeys = $options{belows}*12;
my $numbrks = scalar @breaks;
my $keysbrk = $cankeys/$numbrks;
my $canbrk = $keysbrk;

#     60   for normal ranks
#     30   for split ranks
#     24   drop octaves for mutations
#     12   drop fifths for mutations
#     8    drop thirds for mutations

if($keysbrk > 45) {
    $canbrk = 60;
} elsif($keysbrk > 27) {
    $canbrk = 30;
} elsif($keysbrk > 18) {
    $canbrk = 24;
} elsif($keysbrk > 10) {
    $canbrk = 12;
} else {
    $canbrk = 8;
}

#print "Keys:\n";
#print "  $numkeys ($subkeys) $cankeys\n";
#print "  $numbrks ($keysbrk > $canbrk)\n";

my @keylab=();
my $index = 0;
for($index =0;$index < $options{belows}*12;++$index) {
    push @keylab, "s$index";
}
for($index =0;$index < $options{keys}; ++$index) {
    push @keylab, "m$index";
}
for($index =0;$index < $options{aboves}*12;++$index) {
    push @keylab, "a$index";
}

my @octlab=();
my $curind = 0;
my $curoct = -1;
for($index = $options{belows}*12 -1; $index >=0; --$index) {
    $octlab[$index] = $curoct;
    ++$curind;
    if($curind == 12) {
	$curind = 0;
	--$curoct;
    }
}
$curoct = 0;
for($index = $options{belows}*12; $index < $numkeys; ++$index) {
    $octlab[$index] = $curoct;
    ++$curind;
    if($curind == 12) {
	$curind = 0;
	++$curoct;
    }
}

my @brklab = ();
$curind = 0;
my $curbrk = int($options{belows}*12/$canbrk) -1;
if($curbrk <0) {
    $curbrk = 0;
}
for($index = $options{belows}*12 -1; $index >=0; --$index) {
    $brklab[$index] = $curbrk;
    ++$curind;
    if($curind == $canbrk) {
	$curind = 0;
	--$curbrk;
	if($curbrk == -1) {
	    ++$curbrk;
	}
    }
}
$curind = 0;
$curbrk = int($options{belows}*12/$canbrk);
if($curbrk > $numbrks-1) {
    $curbrk = $numbrks -1;
}
for($index = $options{belows}*12; $index < $numkeys; ++$index) {
    $brklab[$index] = $curbrk;
    ++$curind;
    if($curind == $canbrk) {
	$curind = 0;
	++$curbrk;
	if($curbrk == $numbrks) {
	    --$curbrk;
	}
    }
}

#print "\n";
#print "$_\t" foreach(@keylab);
#print "\n";
#print "$_\t" foreach(@octlab);
#print "\n";
#print "$_\t" foreach(@brklab);
#print "\n";

open OPF, ">$options{output}";
my $rname = $options{output};
$rname =~ s/\.rsf$/.rank/;
print OPF "${rname}: \t#$subkeys\\\n";

my $i = 0;
for($i = 0 - 12*$options{belows}; $i < 0 + $options{keys} + 12*$options{aboves}; ++$i) {
    my $eff_len = $breaks[$brklab[$i+12*$options{belows}]]->[1];
    my $eff_rank = $breaks[$brklab[$i+12*$options{belows}]]->[0];
    my $freq = (8.0 * 110.0 / $eff_len) * 2.0**((-900.0 + 100.0*$i + $options{detune})/1200.0);

    my $res = sprintf("\t%s_%.3f.pipe \t#%d \t%f\\\n",${eff_rank},${freq},$i,$eff_len);
    print OPF $res;
}

print OPF "\t$options{output}\n";

close(OPF);







exit;
