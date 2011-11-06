#!/usr/bin/perl -w

use strict;

## run only on Mac
##   since I'm not sure if $^O contains a specific word on mac, 
##   I check that this is not linux instead
&bailOutExit("This script is supposed to be used only on Mac") if($^O =~ m/linux/);

my $workdir = $ENV{"PWD"};

print STDERR "We are working in $workdir\n";
$workdir =~ s/(.*eidmw).*/$1/;

&bailOutExit("I could not find a working copy of eidmw!") unless ($workdir =~ /eidmw/);

chdir "$workdir" or die "Could not chdir to $workdir: $!\n" unless ($workdir eq $ENV{"PWD"});

print STDERR "We are working in $workdir\n";

my @libPath = ("$workdir/lib/");

##
## check if the cryptlib library is built and build it if not
##
print STDERR "Check cryptlib ... ";
my $libname = "libcl.so"; 
my $dirname = "$workdir/../ThirdParty/cryptlib331";
unless ( -e "$dirname/$libname" || -l "$dirname/$libname" || 1){
  print STDERR "cryptlib library not found, building .. \n";
  ## if the lib is there but not the link, we just set the link and do not build anything
  unless ( -e "libcl.so.3.3.0" ) {
    chdir "$dirname"  or die "Could not chdir to $dirname: $!\n";
    ## - unzip
    if (-e "cl331.zip") {
      system("unzip -a cl331.zip") 
    } else {
      &bailOutExit("cl331.zip not found in $dirname");
    }

    ## - edit random/unix.c
    ##   substitute the inclusion of linux/timex.h with the inclusion of sys/timex.h
    $^I = "";
    @ARGV=("random/unix.c");
    while (<>) {
      s#<linux/timex.h>#<sys/timex.h>#;
      print;
    }

    ## - compile
    system("make shared > make.log");
    &bailOutExit("build of $libname failed") unless( -e "libcl.so.3.3.0");
  }
  symlink "libcl.so.3.3.0",$libname;

  ## - get back to the working directory
  chdir "$workdir";
}
push @libPath,$dirname;
print STDERR " ....  OK\n";

## check if the unittest library is built and build it if not
print STDERR "Check UnitTest++ ... ";
$libname = "libUnitTest++.a";
$dirname = "$workdir/../ThirdParty/unittest-cpp-1.3/UnitTest++/";
unless ( -e "$dirname/$libname" ){
  chdir "$dirname"  or die "Could not chdir to $dirname: $!\n";

  ## - compile
  system("./build.pl");
  &bailOutExit("build of $libname failed") unless( -e "$libname");

  ## - get back to the working directory
  chdir "$workdir";
## - compile
## - get back to the working directory
}

## set some useful environment variables
## -- attention: the settings will not survive the script
push @libPath,$dirname;
print STDERR " ....  OK\n";

## set useful paths

## protect the special characters (e.g. UnitTest++)
my $testLibPath = join(":",@libPath);
$testLibPath =~ s/\+/\\+/g;

my $ldEnvVariable = "LD_LIBRARY_PATH";
$ldEnvVariable =~ s/^L/DYL/  unless ($^O =~ /linux/);

$ENV{"$ldEnvVariable"} .= join(":",@libPath)
  unless ( defined $ENV{"$ldEnvVariable"} &&
	   $ENV{"$ldEnvVariable"} =~ /$testLibPath/ );
print STDERR "$ldEnvVariable set to: $ENV{$ldEnvVariable}\n";


##proceed to build
my @projectToBuild = ("common","applayer","eidlib","dialogs/dialogsQT","cardlayer","cardlayerEmulation");
my @exeToBuild = ("commonTest","cardlayerTool");

foreach(my $p = 0; $p<@projectToBuild;++$p){
  chdir "$workdir/$projectToBuild[$p]" or die "Could not chdir to $workdir/$projectToBuild[$p]: \n";
  system("qmake ; make clean &> make-clean.log; make &> make.log;");
  chdir "$workdir";
  print STDERR "  .... OK \n";
}

foreach(my $p = 0; $p<@exeToBuild;++$p){
  chdir "$workdir/$exeToBuild[$p]" or die "Could not chdir to $workdir/$exeToBuild[$p]: \n";
  die if system("qmake; make clean &> make-clean.log; make &> make.log;");	
  chdir "$workdir";
  print STDERR "  .... OK \n";
}


exit;

sub bailOutExit(){
  bailOut($_[0]);
  chdir "$workdir";
  print STDERR "Abort\n";
  exit;
}

sub bailOut(){
  print STDERR "$0 Error: $_[0]\n";
}


