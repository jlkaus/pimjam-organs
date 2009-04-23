#!/usr/bin/perl -w

use strict;

while(<>) {
chomp;
    if(s/^\t(.*)\.pipe\s*.*$/$1.raw/) {
	system("make $_");
    }
}
exit;
