set multiplot;
set size 1,0.5;
set origin 0,0.5;
plot [0:2] '~/rv4415.dat' using 1:2 with impulses;
set origin 0,0;
plot [0:2] '~/rv8015.dat' using 1:2 with impulses;
unset multiplot;
