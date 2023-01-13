#!/usr/bin/perl

use strict;     # use strict in case other module use this

my $NARG = $#ARGV+1;
my $exe  = "./prod.run";

if($NARG != 2 && $NARG != 3){
    print "\n";
    print "Usage 1): run.pl runno nsub\n";
    print "Usage 2): run.pl runno isub fsub\n";    
    exit;
}

my $runno = $ARGV[0] + 0;
my $isub;
my $fsub;
if($NARG == 2){
    $isub = 0;
    $fsub = $ARGV[1] - 1;
}elsif($NARG == 3){
    $isub = $ARGV[1] + 0;
    $fsub = $ARGV[2] + 0;

    if($isub >= $fsub){
	print "Usage : run.pl runno isub fsub\n";    
	exit;
    }
}

my $irun   = sprintf("%.4d", $runno);
my $rawdir = "data/r$irun";
my $type   = `ls $rawdir/out_${irun}_*_000.root |cut -d"_" -f3`;
$type =~s/\n//g;

my $pdir   = "prod/r$irun";
unless(-e $pdir){
    print "Directory is not found!. Make Directory $pdir\n";
    `mkdir -p $pdir`; 
}

for(my $iii = $isub; $iii <= $fsub; $iii++){
    print("Processing $runno $iii\n");
    system("$exe $type $runno $iii");
}


