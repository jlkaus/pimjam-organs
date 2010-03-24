#!/usr/bin/perl

use strict;
use warnings;
use CGI;

# get parms from GET parms on request.  or command line args if not a GET request.

my $cgif = CGI->new;

# Name used to save things

my $Name = $cgif->param("Name");

# Info needed to generate an FIS
my $FundamentalStrength = $cgif->param("FundamentalStrength");
my $PrimeStrength = $cgif->param("PrimeStrength");
my $EvenStrength = $cgif->param("EvenStrength");
my $PeakHarmonic = $cgif->param("PeakHarmonic");
my $CutoffHarmonic = $cgif->param("CutoffHarmonic");
my $HarmonicRiseA = $cgif->param("HarmonicRiseA");
my $HarmonicRiseB = $cgif->param("HarmonicRiseB");
my $HarmonicDecayC = $cgif->param("HarmonicDecayC");
my $HarmonicDecayD = $cgif->param("HarmonicDecayD");
my $NoiseWidth = $cgif->param("NoiseWidth");
my $NoiseDensity = $cgif->param("NoiseDensity");
my $NoiseDecayE = $cgif->param("NoiseDecayE");
my $NoiseDecayF = $cgif->param("NoiseDecayF");
my $NoiseDecayG = $cgif->param("NoiseDecayG");

# Additional Info needed to test a PIPE
my $TestNote = $cgif->param("TestNote");

# Additional Info needed for both PIPE testing and RSF generation
my $Mutation = $cgif->param("Mutation");
my $Detune = $cgif->param("Detune");
my $A4Freq = $cgif->param("A4Freq");
my $Volume = $cgif->param("Volume");

# Additional Info needed to generate an RSF
my $VolumeFalloff = $cgif->param("VolumeFalloff");
my $RankBreaks = $cgif->param("RankBreaks");
my $RankSubOctaves = $cgif->param("RankSubOctaves");
my $RankSuperOctaves = $cgif->param("RankSuperOctaves");

# Computed parms:

my $TestFrequency = 0.00;
my $TestLength = 0.00;

# Function control

my $Command = $cgif->param("Command");

# commands:

# save cfg (Name, CFG) -> (0, saved cfg file)
# load cfg (Name) -> (CFG, 0)
# gen fis (Name, CFG) -> (0, saved fis file)
# gen rsf (Name, CFG, RSF) -> (0, saved rsf file)
# test pipe (Name, CFG, PIPE) -> (0, saved temp fis file/generate pipe/play pipe)
# gen img basic (Name, CFG) -> (JPG, 0)
# gen img noise (Name, CFG) -> (JPG, 0)
# gen img pipe (Name, CFG, PIPE) -> (JPG, 0)

# New Equations
# Y_k = S(rnd(k))*X(rnd(k))*N(k-rnd(k))*Z(k)
# S = strength modifier function
# X = harmonic rise/decay function
# N = noise decay function
# Z = cutoff function

# X(k) = |k-P|^(-A) * exp(-|k-P|*B)  for k > P
# X(k) = 1.0 for k = P
# X(k) = |k-P|^(-C) * exp(-|k-P|*D)  for k < P

# N(dk) = 1.0 for dk < epsilon
# N(dk) = E*|dk|^(-F) * exp(-|dk|*G) for dk > epsilon
# dk is within (0,0.5), and takes values n*NoiseWidth/NoiseDensity where n is in W.


# Old Equations

# S_0  (0,1) strength multiplier for fundamental
#            Y_1 = S_0 * X_1
# S_1  (0,1) strength multiplier for prime
#            Y_2 = S_1 * S_e * X_2
# S_e  (0,1) strength multiplier for all evens
#            Y_ek = S_e * X_ek
#            Y_ok = X_ok

#            Y_z,1,x_n = S_0 * X_(1+x_n) * Q_N(x_n)
#            Y_z,2,x_n = S_1 * S_e * X_(2+x_n) * Q_N(x_n)
#            Y_z,ek,x_n = S_e * X_(k+x_n) * Q_N(x_n)
#            Y_z,ok,x_n = X_(k+x_n) * Q_N(x_n)

print $cgif->header("image/png");

open(GNP, "| /usr/bin/gnuplot");
print GNP "set terminal png\n";
print GNP "plot sin(2*pi* $A4Freq * x)\n";
close(GNP);

exit;

__END__




# k, dontcutoff
sub spectralEnvelope {
    my $k = shift;
    my $dontcutoff = shift;

# C   Z>0 cutoff harmonic.
#         C-2(-) = 1.00 C-2 = .95, C-1 = .85, C = 0.65, C+1 = 0.30, C+2 = 0.05, C+3(+) = 0.00
# P   Z>0 peak harmonic.
#         X_P = 1.00

# H_f  (0,1) shape parameter for past peak
#            X_k = exp(-(k-P)/exp(20*H_f - 10))  for all k > P
# H_b  (0,1) shape parameter for before peak
#            X_k = exp((k-P)/exp(20*H_b - 10))  for all k < P
    my $X_k = 1.0;

    if($k == $PeakHarmonic) {
	$X_k = 1.0;
    } elsif($k > $PeakHarmonic) {
	$X_k = exp(-($k - $PeakHarmonic)/exp(20.0*$FrontShape - 10.0));
    } else {
	$X_k = exp(($k - $PeakHarmonic)/exp(20.0*$BackShape - 10.0));
    }

    if(defined $dontcutoff || $k < $CutoffHarmonic - 3) {
	$X_k *= 1.00;
    } elsif($k >= $CutoffHarmonic + 3) {
	$X_k *= 0.00;
    } elsif($k >= $CutoffHarmonic - 3 && $k < $CutoffHarmonic - 2) {
	$X_k *= 1.00 + (-0.05) * ($k - $CutoffHarmonic + 3);
    } elsif($k >= $CutoffHarmonic - 2 && $k < $CutoffHarmonic - 1) {
	$X_k *= 0.95 + (-0.10) * ($k - $CutoffHarmonic + 2);
    } elsif($k >= $CutoffHarmonic - 1 && $k < $CutoffHarmonic) {
	$X_k *= 0.85 + (-0.20) * ($k - $CutoffHarmonic + 1);
    } elsif($k >= $CutoffHarmonic && $k < $CutoffHarmonic + 1) {
	$X_k *= 0.65 + (-0.35) * ($k - $CutoffHarmonic);
    } elsif($k >= $CutoffHarmonic + 1 && $k < $CutoffHarmonic + 2) {
	$X_k *= 0.30 + (-0.25) * ($k - $CutoffHarmonic - 1);
    } else { # if($k >= $CutoffHarmonic + 2 && $k < $CutoffHarmonic + 3) {
	$X_k *= 0.05 + (-0.05) * ($k - $CutoffHarmonic - 2);
    }

    return $X_k;
}


my $pip2 = atan2(1.0,0.0)/2.0;

# k_dec
sub noiseEnvelope {
    my $k_dec = shift;

# N_w  (0,1) width of noise
# N_d  Z>=0 density of noise
# N_s  (0,1) shape parameter for noise
#            Q_N(x) = cos(pi/2 * N_w^(-exp(20*N_s -10))*abs(x)^(exp(20*N_s -10)), for each abs(x_n) <= 0.50
#            x_n = n * N_w/N_d for all n in Z, abs(n) <= N_d

    if(abs($k_dec) < 0.01) {
	return 1.0;
    }
    return cos($pip2 * ($NoiseWidth ** (-exp(20.0*$NoiseShape - 10.0))) * (abs($k_dec) ** exp(20.0*$NoiseShape - 10.0)));
}

# Cents from A4 = $notes->{note} + 1200*(octave - 4)
my $notes = { "C" => -900,
	      "C#" => -800,
	      "D" => -700,
	      "D#" => -600,
	      "E" => -500,
	      "F" => -400,
	      "F#" => -300,
	      "G" => -200,
	      "G#" => -100,
	      "A" => 0,
	      "A#" => 100,
	      "B" => 200
	      };

# k, note
sub findFrequency {
    my $k = shift;
    my $note = shift;
    my $centsFromA4 = (determineKeyNumber($note) - 33) * 100 + $Detune;
    my $basefreq = $A4Freq * (2.0 ** ($centsFromA4/1200));
    my $fundfreq = $basefreq * determineLengthFactor($Mutation);
    return $fundfreq * $k;
}

# mutation
sub determineLengthFactor {
    my $mutation = shift;

    my $mutflot = 8.0;
    if($mutation =~ /^(\d+)$/) {
	$mutflot = $1;
    } elsif($mutation =~ /^(\d+)-(\d+)\/(\d+)$/) {
	$mutflot = $1 + $2/$3;
    } elsif($mutation =~ /^(\d+)\/(\d+)$/) {
	$mutflot = $1/$2;
    }

    return (8.0/$mutflot);
}

# note
sub determineKeyNumber {
    my $note = shift;
    my $tkn = 0;
    if($note =~ /^([ABCDEFG]\#?)(-?\d+)$/) {
	$tkn = ($2 - 4) * 1200 + $notes->{$1};
    }

    return $tkn/100 + 33;
}

# note
sub findVolume {
    my $note = shift;

    return $Volume - $VolumeFalloff * determineKeyNumber($note);
}

# SpectrumArray
sub findNormalization {
    my @spectrum = @_;

    my $sum = 0.0;
    foreach (@spectrum) {
	$sum += $_;
    }

    return 1.0/$sum;
}


# start from k = 1.
# for each k, start from k - N_w and go up to k + N_w.
# using rules above, generate the strength of k, using S_0, S_1, S_e, spectralEnvelope() and noiseEnvelope(), between 0 and 100
my @k_vals = ();
my @y_vals = ();

for(my $k = 1; $k < 100; ++$k) {
    my $dk = 0.00;
    for(my $dk = -$NoiseWidth; $dk < $NoiseWidth; $dk+=($NoiseWidth/$NoiseDensity)) {
	if(abs($dk) < 0.500) {
	    push @k_vals, $k+$dk;
	    print $k+$dk;

	    my $Y_k = spectralEnvelope($k+$dk) * noiseEnvelope($dk);

	    if($k % 2 == 0) {
		$Y_k *= $EvenStrength;
	    }

	    if($k == 1) {
		$Y_k *= $FundamentalStrength;
	    } elsif($k == 2) {
		$Y_k *= $PrimeStrength;
	    }

	    push @y_vals, $Y_k;
	    print "  $Y_k\n";
	}
    }
}



###################
# generate plot for noise
###################
# plot assumes range between k=0 and k=2, and uses actual parameters for them.
# also, using rules above, generate the continuous noise function between k=0 and k=2

###################
# generate plot for spectrum
###################
# Displays for k=0 to 100.
# also, using rules above, generate the continuous strength function, without the noiseEnvelope() or cutoffs (infinite cutoff), from k=0 to k=100

###################
# generate spectrum plot for test pipe at appropriately dropped off volume
###################
# Displays for k=0 to 100 in log Hz to dB.  Scaled to max volume.  Lines show audibles:  vertical lines at 20Hz and 20000Hz.  Base of plot is 0dB, which is audible threshold.
# also, using actual frequency and volume stuff, generate the spectrum plot of the actual pipe. (findFrequency(), findVolume(), findNormalization())

###################
# generate fis
###################


###################
# generate rsf
###################



exit;

