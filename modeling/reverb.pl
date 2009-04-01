#!/usr/bin/perl -w

use strict;

my $tD = $ARGV[0];
my $rD = $ARGV[1];
my $Sx = $ARGV[2] || 1.0;
my $cutoff = $ARGV[3] || 0.000001;

my $rO = 0.7;
my $rE = 0.8;

my $Ox = 0.0;
my $Dx = 2.0;
my $Rx = 18.0;
my $Ex = 28.0;
my $cc = 343.0;


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
my %pp_phs = ( "SDDO" => -1,
		"SDDR"=> 1,
		"SDDE"=> 1,
		"SOOD"=>-1,
		"DOOD"=>-1,
		"ODDO"=>-1,
		"ODDR"=>1,
		"ODDE"=>1,
		"DEER"=>-1,
		"DEED"=>-1,
		"EDDO"=>1,
		"EDDR"=>-1,
		"EDDE"=>-1
		);


# stack of state location, current signal strength, current toggle, and current elapsed time
my @op_stack = (["SD",0.5,1,0.0],["SO",0.5,1,0.0]);

# list of termination times and powers
my @heard=();

my $entry ;
while($entry = pop @op_stack) {
    my $cur_state = $entry->[0];
    my $cur_power = $entry->[1];
    my $cur_phase = $entry->[2];
    my $cur_time = $entry->[3];

    $cur_time+=$p_time{$cur_state};

    if($cur_power < $cutoff) {
#	print "Terminating with power $cur_power at time $cur_time in state $cur_state.\n";
    } elsif($cur_state eq "DR" || $cur_state eq "ER") {
#	print "Heard with power $cur_power at time $cur_time in state $cur_state.\n";
	push @heard, [$cur_time, $cur_power, $cur_phase];
    } else {
	my $i;
	for($i = 0; $i< scalar @{$p_trans{$cur_state}}; ++$i) {
	    my $next_state = $p_trans{$cur_state}->[$i];
	    my $transform = $pp_fact{$cur_state.$next_state};
	    my $shiftor = $pp_phs{$cur_state.$next_state};
	    push @op_stack, [$next_state, $cur_power*$transform, $cur_phase*$shiftor,$cur_time];
#	    print "Moving from $cur_state to $next_state at time $cur_time with power ".($cur_power*$transform)."\n";
	}
    }
}

my @sheard = sort {$a->[0] <=> $b->[0]} @heard;

my $sh;
my $lasttime = 0.0;
my $lastpower = 0.0;
while($sh = shift @sheard) {
    if(abs($lasttime - $sh->[0]) < 0.00001) {
	$lastpower += $sh->[1]*$sh->[2];
    } else {
	print "$lasttime $lastpower\n";
	$lasttime = $sh->[0];
	$lastpower = $sh->[1]*$sh->[2];
    }
}



exit;

