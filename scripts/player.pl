#!/usr/bin/perl -w

use strict;

my @filestoplay = ();

foreach (@ARGV) {
    if(/_([0-9.]+)\.raw$/) {
	push @filestoplay, [$1,$_];
    } else {
	die "Don't know how to deal with $_\n";
    }
}

my @sfilestoplay = sort {$a->[0] <=> $b->[0];} @filestoplay;

foreach (@sfilestoplay) {
    print "Playing $_->[1]...";
    system("aplay -r44100 -c1 -traw -fS16_LE $_->[1]");
    print "\n";
}

exit;
