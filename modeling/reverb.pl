#!/usr/bin/perl -w

use strict;

my $tD = $ARGV[0];
my $rD = $ARGV[1];
my $Sx = $ARGV[2];
my $cutoff = $ARGV[3];

my $rO = 0.7;
my $rE = 0.7;

my $Ox = 0.0;
my $Dx = 3.0;
my $Rx = 15.0;
my $Ex = 60.0;
my $cc = 300.0;


my %p_trans = ( "SD" => ["DO","DR","DE"],
		"SO" => ["OD"],
		"DO" => ["OD"],
		"OD" => ["DO","DR","DE"],
		"DE" => ["ER","ED"],
		"ED" => ["DO","DR","DE"],
		"DR" => [],
		"ER" => []
    );
my %p_time = ( "SD" => abs($Sx - $Dx)/$cc,
	       "SO" => abs($Sx - $Ox)/$cc,
	       "DO" => abs($Dx - $Ox)/$cc,
	       "OD" => abs($Ox - $Dx)/$cc,
	       "DE" => abs($Dx - $Ex)/$cc,
	       "ED" => abs($Ex - $Dx)/$cc,
	       "DR" => abs($Dx - $Rx)/$cc,
	       "ER" => abs($Ex - $Rx)/$cc
    );
my %pp_fact = ( "SDDO" => $rD,
		"SDDR" => $tD,
		"SDDE" => $tD,
		"SOOD" => $rO,
		"DOOD" => $rO,
		"ODDO" => $rD,
		"ODDR" => $tD,
		"ODDE" => $tD,
		"DEER" => $rE,
		"DEED" => $rE,
		"EDDO" => $tD,
		"EDDR" => $rD,
		"EDDE" => $rD
    );



# stack of state location, current signal strength, and current elapsed time
my @op_stack = (["SD",0.5,0.0],["SO",0.5,0.0]);

# list of termination times and powers
my @heard=();

my $entry ;
while($entry = pop @op_stack) {
    my $cur_state = $entry->[0];
    my $cur_power = $entry->[1];
    my $cur_time = $entry->[2];

    $cur_time+=$p_time{$cur_state};

    if($cur_power < $cutoff) {
#	print "Terminating with power $cur_power at time $cur_time in state $cur_state.\n";
    } elsif($cur_state eq "DR" || $cur_state eq "ER") {
#	print "Heard with power $cur_power at time $cur_time in state $cur_state.\n";
	push @heard, [$cur_time, $cur_power];
    } else {
	my $i;
	for($i = 0; $i< scalar @{$p_trans{$cur_state}}; ++$i) {
	    my $next_state = $p_trans{$cur_state}->[$i];
	    my $transform = $pp_fact{$cur_state.$next_state};
	    push @op_stack, [$next_state, $cur_power*$transform, $cur_time];
#	    print "Moving from $cur_state to $next_state at time $cur_time with power ".($cur_power*$transform)."\n";
	}
    }
}

my @sheard = sort {$a->[0] <=> $b->[0]} @heard;

my $sh;
my $lasttime = 0.0;
my $lastpower = 0.0;
while($sh = shift @sheard) {
    if($lasttime == $sh->[0]) {
	$lastpower += $sh->[1];
    } else {
	print "$lasttime $lastpower\n";
	$lasttime = $sh->[0];
	$lastpower = $sh->[1];
    }
}



exit;

