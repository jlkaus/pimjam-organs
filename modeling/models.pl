#!/usr/bin/perl -w

use strict;

system("./reverb.pl 1.0 0.0 > great.dat");
system("./reverb.pl 0.875 0.088 > swell00.dat");
system("./reverb.pl 0.620 0.266 > swell15.dat");
system("./reverb.pl 0.392 0.426 > swell30.dat");
system("./reverb.pl 0.205 0.557 > swell45.dat");
system("./reverb.pl 0.071 0.650 > swell60.dat");
system("./reverb.pl 0.002 0.699 > swell75.dat");
system("./reverb.pl 0.000 0.700 > swell90.dat");






exit;

