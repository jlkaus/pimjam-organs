#!/usr/bin/perl

use strict;
use warnings;
use CGI;

# get parms from GET parms on request.  or command line args if not a GET request.

my $cgif = CGI->new;

# Name used to save things

my $Name = $cgif->param("Name");
my $MaxK = $cgif->param("MaxK") || 100;

# Info needed to generate an FIS
my $FundamentalStrength = $cgif->param("FundamentalStrength") || 1.0;
my $PrimeStrength = $cgif->param("PrimeStrength") || 1.0;
my $EvenStrength = $cgif->param("EvenStrength") || 1.0;
my $PeakHarmonic = $cgif->param("PeakHarmonic") || 1;
my $CutoffHarmonic = $cgif->param("CutoffHarmonic") || 80;
my $HarmonicRiseA = $cgif->param("HarmonicRiseA") || 1.0;
my $HarmonicRiseB = $cgif->param("HarmonicRiseB") || 0.0;
my $HarmonicDecayC = $cgif->param("HarmonicDecayC") || 1.0;
my $HarmonicDecayD = $cgif->param("HarmonicDecayD") || 0.0;
my $NoiseWidth = $cgif->param("NoiseWidth") || 0.5;
my $NoiseDensity = $cgif->param("NoiseDensity") || 25;
my $NoiseDecayE = $cgif->param("NoiseDecayE") || 20.0;
my $NoiseDecayF = $cgif->param("NoiseDecayF") || 2.0;
my $NoiseDecayG = $cgif->param("NoiseDecayG") || 0.0;

# Additional Info needed to test a PIPE
my $TestNote = $cgif->param("TestNote") || "C2";

# Additional Info needed for both PIPE testing and RSF generation
my $Mutation = $cgif->param("Mutation") || "8";
my $Detune = $cgif->param("Detune") || 0.0;
my $A4Freq = $cgif->param("A4Freq") || 440.0;
my $Volume = $cgif->param("Volume") || 30.0;

# Additional Info needed to generate an RSF
my $VolumeFalloff = $cgif->param("VolumeFalloff") || 0.0;
my $RankBreaks = $cgif->param("RankBreaks") || 0;
my $RankSubOctaves = $cgif->param("RankSubOctaves") || 0;
my $RankSuperOctaves = $cgif->param("RankSuperOctaves") || 0;

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

sub generatePngPlots {
  my $args = shift;

  my $keyon = "";
  foreach (@{$args->{plots}}) {
    $keyon = 1 if defined $_->{title};
  }

  my $gfh = do { local *GFH };
  open($gfh, "| /usr/bin/gnuplot");
#my $gfh = *STDOUT;
  print $gfh "set terminal png";
  print $gfh " size $args->{width},$args->{height}" if (defined $args->{width} && defined $args->{height});
  print $gfh "\n";
  print $gfh "set samples $args->{samples}\n" if defined $args->{samples};
  print $gfh "set title \"$args->{title}\"\n" if defined $args->{title};
  print $gfh "set xlabel \"$args->{xlabel}\"\n" if defined $args->{xlabel};
  print $gfh "set ylabel \"$args->{ylabel}\"\n" if defined $args->{ylabel};
  print $gfh "set key off\n" if !$keyon;
  print $gfh "set logscale x $args->{log}\n" if defined $args->{log};
  # args->plots is array of hashrefs: source (function/datafile including using clause), title, with, data (an arrayref of all the datapoints for this particular thing, or undef if not needed)
  my @datalines = ();
  my $first = 1;
  print $gfh "plot ";
  print $gfh "$args->{ranges} " if defined $args->{ranges};
  foreach (@{$args->{plots}}) {
    print $gfh "," if !$first;
    $first = "";
    print $gfh " $_->{source} ";
    print $gfh "title \"$_->{title}\" " if defined $_->{title};
    print $gfh "with $_->{with}" if defined $_->{with}; 
    push @datalines, @{$_->{data}} if defined $_->{data};
    push @datalines, "e" if defined $_->{data};
  }
  print $gfh "\n";
  print $gfh "$_\n" foreach @datalines;
  close($gfh);
}

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
	$X_k = ($k-$PeakHarmonic+1.0)**(-$HarmonicDecayC) * exp(-$HarmonicDecayD*($k - $PeakHarmonic +1.0));
    } else {
	$X_k = ($PeakHarmonic-$k+1.0)**(-$HarmonicRiseA) * exp(-$HarmonicRiseB * ($PeakHarmonic-$k + 1.0));
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
    return ($NoiseDecayE * abs($k_dec)+1.0) ** (-$NoiseDecayF) * exp(-$NoiseDecayG * ($NoiseDecayE * abs($k_dec) +1.0));;
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
    # volume falloff is in dB/octave.  Rated Volume is at played C2, so lower notes would be louder for positive falloff. (should only be relevant for suboctaves)
    return $Volume - $VolumeFalloff * determineKeyNumber($note)/12;
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
my @imp_height = ();
my @env_data = ();
my @imp_data = ();
my @noise_data = ();

for(my $k = 1; $k < 100; ++$k) {
    my $dk = 0.00;
    for(my $dk = -$NoiseWidth; $dk < $NoiseWidth; $dk+=($NoiseWidth/$NoiseDensity)) {
	if(abs($dk) < 0.500) {
	    push @k_vals, $k+$dk;

	    my $ne = noiseEnvelope($dk);
	    my $se = spectralEnvelope($k+$dk);
	    my $senc = spectralEnvelope($k+$dk,1);
	    my $sk = spectralEnvelope($k);
	    
	    my $factor = 1.0;
	    
	    if($k % 2 == 0) {
	        $factor *= $EvenStrength*(1.0-abs($dk))+(abs($dk));
	    } else {
		$factor *= (1.0-abs($dk))+$EvenStrength*(abs($dk));
	    }

	    if($k == 1) {
	        if($dk < 0) {
			# merging with 0 (Null)
			$factor *= $FundamentalStrength*(1.0-abs($dk));
		} else {
			# merging with 2 (Prime)
			$factor *= $FundamentalStrength*(1.0-abs($dk))+$PrimeStrength*(abs($dk));
		}
	    } elsif($k == 2) {
	    	if($dk < 0) {
			# merging with 1 (Fund)
			$factor *= $PrimeStrength*(1.0-abs($dk)) + $FundamentalStrength*(abs($dk));
		} else {
			# merging with 3 (1.0)
			$factor *= $PrimeStrength*(1.0-abs($dk)) + (abs($dk));
		}
	    } elsif($k == 3) {
		if($dk < 0) {
			# merging with 2 (Prime)
			$factor *= (1.0-abs($dk)) + $PrimeStrength *(abs($dk));
		} else {
			# merging with 4 (1.0) (no change)
		}
	    }

	    if($k == 1) {
		push @noise_data, ($k+$dk)." $ne";
	    }

	    push @k_vals, $k+$dk; 
 	    push @env_data, ($k+$dk)." ".($senc);
	    push @imp_data, ($k+$dk)." ".($se*$ne*$factor);
	    push @imp_height, $se*$ne*$factor;
	}
    }
}



###################
# generate plot for noise
###################
# plot assumes range between k=0 and k=2, and uses actual parameters for them.
# also, using rules above, generate the continuous noise function between k=0 and k=2
if($Command eq "genimgnoise") {
  print $cgif->header("image/png");

  generatePngPlots({
      width=>300,
      height=>150,
      title=>"Noise plot for $Name",
      xlabel=>"Harmonic",
      ylabel=>"Relative Strength",
      ranges=>"[0.5:1.5] [0:1]",
      plots=>[{
          source=>"'-' using 1:2",
	  with=>"impulses",
	  data=>\@noise_data
	},{
	  source=>"'-' using 1:2",
	  with=>"lines lt 18",
	  data=>\@noise_data
	}]
    });

}

###################
# generate plot for spectrum
###################
# Displays for k=0 to 100.
# also, using rules above, generate the continuous strength function, without the noiseEnvelope() or cutoffs (infinite cutoff), from k=0 to k=100
if($Command eq "genimgbasic") {
  print $cgif->header("image/png");
  
  generatePngPlots({
      width=>1000,
      height=>300,
      title=>"Spectrum plot for $Name",
      xlabel=>"Harmonic",
      ylabel=>"Relative Strength",
      ranges=>"[0:$MaxK] [0:1]",
      plots=>[{
          source=>"'-' using 1:2",
	  with=>"impulses",
	  data=>\@imp_data
	},{
	  source=>"'-' using 1:2",
	  with=>"lines lt 18",
	  data=>\@env_data
	}]
    });
}


###################
# generate spectrum plot for test pipe at appropriately dropped off volume
###################
# Displays for k=0 to 100 in log Hz to dB.  Scaled to max volume.  Lines show audibles:  vertical lines at 20Hz and 20000Hz.  Base of plot is 0dB, which is audible threshold.
# also, using actual frequency and volume stuff, generate the spectrum plot of the actual pipe. (findFrequency(), findVolume(), findNormalization())
if($Command eq "genimgpipe") {

  my @spec_data = ();
  my $vfactor = findNormalization(@imp_data) * findVolume($TestNote);

  my $i = 0;
  foreach (@k_vals) {
    my $freq = findFrequency($_, $TestNote);
    my $power = $vfactor * $imp_height[$i];

    push @spec_data, "$freq $power";

    ++$i;
  }


  print $cgif->header("image/png");

  my $maxfreq = findFrequency(int($CutoffHarmonic*1.2), $TestNote);

  generatePngPlots({
      width=>1000,
      height=>300,
      title=>"Real spectrum for the $TestNote pipe in rank [$Name $Mutation']".((abs($Detune) < 0.01)?"":" (Detuned by $Detune cents)").((abs($A4Freq-440.0)<0.01)?"":" (With A4 @ $A4Freq Hz)"),
      xlabel=>"Frequency (log Hz)",
      ylabel=>"Volume (dB)",
      log=>10,
      ranges=>"[1:$maxfreq] [0:$Volume]",
      plots=>[{
          source=>"'-' using 1:2",
	  with=>"impulses",
	  data=>\@spec_data
	}]
    });
}

###################
# generate fis
###################
if($Command eq "genfis") {

}


###################
# generate rsf
###################
if($Command eq "genrsf") {


}


exit;

