#!/usr/bin/perl -w

use strict;

my @files = <THP_*/*/*.dft>;
my $file;

foreach $file (@files) {
    
    $file =~ s/dft$/pipe/;
    system("make $file");


}
exit;
