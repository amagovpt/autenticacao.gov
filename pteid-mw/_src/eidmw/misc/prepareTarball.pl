#!/usr/bin/perl -w

## This script packs in a tarball the files needed to build the project. The
## tarball is intended for distribution to users, therefore the test and emulation
## subproject are not included.
##
## The package name (e.g. beid) is required in input. Optionally the name
## of the master project file (that is the one with TEMPLATE=subdirs, which steers the
## build of the whole project) is required in case more than one is present in the 
## top directory.
##
## The script scans the master project file, then the sub-project files in the subdirectories
## listed therein to gather the names of the files which have to be included in the
## tarball. Test and emulation sub-projects are discarded according to the value
## of variables $discardTest and $discardEmulation.
## The tarball name is <package name>_<version>.tgz, where the version is in the form
## REL_MAJ.REL_MIN.REL_REV and the value is read out from the file _Builds/<package name>versions.mak.
##
## It is intended that the script is started from the directory eidmw/_Builds, where it resides.

use strict;

#######################################################################
## Preliminary checks on command line arguments and working directory
#######################################################################

my $discardTest = 1;
my $discardEmulation = 1;

if(@ARGV < 1)
{
  print STDERR "[Error] $0: Error! The package name was not given.\n" .
    "Please specify for which package we are packing the tarball (e.g. \"beid\").\n".
    "The name has to be the same as the value of the PKG_NAME variable you\n".
    "used to build the package with qmake.\n";
  exit;
}

my $packageName = $ARGV[0];
my $startDir    = $ENV{"PWD"};
my $currDir     = $startDir;

#unless ($startDir =~ m#/eidmw/misc#)
unless ($startDir =~ m#/misc#)

{
  print STDERR "[Error] $0: The script has to be executed from where it resides.\n";
  exit(1);
}
#######################################################################
## we work in the upper dir (assuming that it is eidmw/)
#######################################################################
print STDOUT "[Info] Changing to $currDir/..\n";
chdir ".." or die "[Error] Could not chdir to ..: $!\n";

#######################################################################
## find the master project file in this directory
#######################################################################
my $masterProject = &findMasterProjectFile();

print STDOUT "[Info] Using master project file $masterProject\n";

#######################################################################
## Scan the master project file
#######################################################################

my @listOfDirs = &scanMasterProject();

if(@listOfDirs < 1)
{
  print STDERR "[Error] $0: No subprojects found in $masterProject!\n";
  exit(1);
}

#######################################################################
## The additional files that have to be included in the tarfile to be
## complete.
## The elements of this array are arrays with the 
## following elements:
## [0] the location of the file to add
## [1] the name that file will have in the distribution
#######################################################################

my @additionalFiles = ( [ "$masterProject"  ],
			["configure"],
			["_Builds/eidcommon.mak"],
			["_Builds/" . $packageName ."versions.mak"],
			["dialogs/dialogsQTsrv/Resources/*"],
			["build.txt", "INSTALL"],
			["eidgui/eidmw_de.qm"],
			["eidgui/eidmw_en.qm"],
			["eidgui/eidmw_fr.qm"],
			["eidgui/eidmw_nl.qm"],
			["eidgui/Images/*"]
		      );

#######################################################################
## loop over the subdirs and scan for the files which will be included
## -- for example by scanning the project file and choosing all the mentioned files
##    and adding the project file
#######################################################################
my @listOfFiles = ();

for(my $dir = 0;$dir< @listOfDirs; ++$dir)
{

  #######################################################################
  ## look if a project file called like the directory is present,
  ## exit otherwise
  #######################################################################

  #######################################################################
  ## compose the filename
  #######################################################################
  my $projectFilename = $listOfDirs[$dir];
  $projectFilename =~ s#.*/(.*)#$1#;   					## in case of nested dirs takes the name of the last one
  $projectFilename = $listOfDirs[$dir] ."/" . $projectFilename .".pro"; ## append path and ".pro" extension

  if( -e $projectFilename)
  {
    @listOfFiles = &scanProjectFile(@listOfFiles,$listOfDirs[$dir], $projectFilename);

    #######################################################################
    ## add the project file itself to the list
    #######################################################################
    push @listOfFiles, $projectFilename;
  } 
  else 
  {
    print STDERR "[Error] Could not find a project file for $listOfDirs[$dir] in the dir with the same name\n".
      "Are you sure that $listOfDirs[$dir] should be included in the release?\n";
    exit;
  }
}

#######################################################################
## Add the additional files we need
## add the readme, install and configure file
#######################################################################
for(my $index = 0; $index < @additionalFiles; ++$index)
{
  if(@{$additionalFiles[$index]} > 1 && $additionalFiles[$index]->[0] ne $additionalFiles[$index]->[1])
  {
    system("cp $additionalFiles[$index]->[0] $additionalFiles[$index]->[1]");
    #######################################################################
    ## eventually make some changes to the new file
    ## ...
    #######################################################################
    push @listOfFiles,$additionalFiles[$index]->[1];
  } 
  else 
  {
    #######################################################################
    ## rename _Builds/eidcommon.mak and add a line to enforce the RELEASE_BUILD 
    #######################################################################
    if($additionalFiles[$index]->[0] =~ m/eidcommon\.mak/ )
    {
      #######################################################################
      ## add a line to enforce the RELEASE_BUILD and 
      ## set PKG_NAME (in case it is not set explicitly)
      #######################################################################
      $^I = ".bak";
      @ARGV=($additionalFiles[$index]->[0]);
      my $line = 0;
      while(<>)
      {
	++$line;
	if($line == 1)
        {
	  print "PKG_NAME=$packageName\n" ;
	  print "RELEASE_BUILD=yes\n\n" ;
	}
	print;
      }
    }
    push @listOfFiles,$additionalFiles[$index]->[0];
  }
}

#######################################################################
## set the name of the tarball including the release version 
## -- parse the version tag from _Builds/beidversions.mak instead
## *** it is assumed that the maj.min.rev tags are integers!
#######################################################################

#######################################################################
# name of the file containing the maj.min.rev numbers
#######################################################################
my $versionFilename = "_Builds/beidversions.mak";
my $maj = -1;
my $min = -1;
my $rev = -1;

open VERSION, "$versionFilename" or die "[Error] Could not open $versionFilename: $!\n";

#######################################################################
## get the package release version nr
#######################################################################
while(<VERSION>)
{
  if(/^\s*REL_(MAJ|MIN|REV)\s*=\s*(\d+)$/)
  {
    $maj = $2 if($1 eq "MAJ");
    $min = $2 if($1 eq "MIN");
    $rev = $2 if($1 eq "REV");
  }
  last unless ($maj == -1 || $min == -1 || $rev == -1);
}
close VERSION;

my $version = join(".",($maj,$min,$rev));
my $tarballName = $packageName ."_". $version;

#######################################################################
## prepare the tarball in all the needed formats (.tar.gz, what else?)
#######################################################################

#######################################################################
## we prepend an empty element to the list of files to avoid that in
## the "join" which follows the first file of the list remains without
## the 'delimiter', that is a directory name
#######################################################################
unshift(@listOfFiles," ");

#######################################################################
## the change of dir with -C and the 'delimiter' defined as "$tarballName/"
## are introduced because we are working in the eidmw dir but
## we want the files to be tarred into the directory beid_x.y.z/
#######################################################################
system(" cd ..;  ! [ -h $tarballName ] && ln -s eidmw $tarballName ; pwd ; ".
       " tar czf $tarballName.tgz  " . join(" $tarballName/", @listOfFiles) ." ; cd - ;".
       " mv ../$tarballName.tgz . ");

print STDOUT "[Info] Packed " . @listOfFiles . " files  in $tarballName.tgz\n";

#######################################################################
## clean up
#######################################################################
foreach(@additionalFiles)
{
    system("rm $_->[1]") if(@{$_} > 1 && $_->[0] ne $_->[1]);
}

#######################################################################
## rename eidcommon.mak.bak to eidcommon.mak again
#######################################################################
system("mv _Builds/eidcommon.mak.bak _Builds/eidcommon.mak");

exit;

#############################################################
## find master project file
## The master project file is assumed to reside in the current directory
## Only one .pro file is accepted
#############################################################
sub findMasterProjectFile()
{
  print STDERR "[Info] Looking for master project file\n";
  my $projFile = "";
  if(@ARGV>1)
  {
    print STDERR "$ARGV[1]\n";
    $projFile = $ARGV[1];
    ## check that it exists
    if( ! -e $projFile )
    {
      print STDERR "$0: Error! Project file $projFile not found in " . $ENV{"PWD"} ."!\n";
      exit(1);
    }
  } 
  else 
  {
    ## If there is only one in this directory, take that one.
    ## Otherwise ask the user and exit.
    my @possibleFiles = glob("*.pro");
    if(@possibleFiles == 1)
    {
      $projFile = $possibleFiles[0];
    } 
    elsif ( @possibleFiles == 0) 
    {
      print STDERR "$0: Error! No project file found!\nPlease specify the path to the master project file.\n";
      exit(1);
    } 
    else 
    {
      print STDERR "$0: Error! More than one project file found in " . $ENV{"PWD"} .":\n".
	join("\n",@possibleFiles) ."\n".
	  "Please specify which one has to be used.\n";
      exit(1);
    }
  }
  print STDERR "[Info] Master project file: $projFile\n";
  return $projFile;
}

#############################################################
##
## scan master project file
## The master .pro file is scanned for subdirectories. All the
## subdirectories found in this master .pro are taken into account
## a list of subprojects (directories) is returned.
#############################################################
sub scanMasterProject(){

  my @tmpList = ();
  open MASTER,"$masterProject" or die "Could not open $masterProject: $!\n";
  my $interestingSection = 0;

  my $templateFound = 0;
  while (<MASTER>) 
  {
    chomp;
    #############################################################
    ## eliminate lines starting with '#' (comments)
    #############################################################
    next if(/^\s*#/);
    #############################################################
    ## eliminate comments (i.e. find the first occurrence of '#' and keep only what precedes it)
    #############################################################
    s/([^#]+).*$/$1/ if(/#/);
    next unless(/[^\s]+/);

    #############################################################
    ## check that the template is subdirs
    #############################################################
    if ( m/TEMPLATE\s*=\s*(.*)/) 
    {
      if( $1 ne "subdirs") 
      {
	print STDERR "[Error] $0: $masterProject is not a master project file (The template is not subdirs)!\n";
	exit(1);
      } 
      else 
      {
	$templateFound = 1;
      }
    }
    #############################################################
    ## only read lines from the sections listing SUBDIRS
    #############################################################
    if (m/([^#][a-zA-Z_,()\s]*:\s*)?SUBDIRS\s*\+?=/) 
{
      my $line = $_;
      #############################################################
      ## check if SUBDIRS is preceded by 'contains(PKG_NAME,<..>)'
      ## and the package name is not ours
      #############################################################
      if (defined $1 && $1 ne "") 
      {
	my $condition = $1;
	if ($condition =~ m/contains\s*\(\s*PKG_NAME\s*,\s*([^\s]*?)\)/
	    && $1 eq $packageName) 
        {
	  $interestingSection = 1;
	  ## remove the "contains.." from the line
	  s/.*?://;
	}
      } else {
	$interestingSection = 1 ;
      }
    }
    next unless ( $interestingSection > 0 );
    #############################################################
    ## no continuation of line found in an interesting section:
    ## this is the last line we are reading here
    #############################################################
    $interestingSection = 0 if($interestingSection && ! (m#\\\s*$#) );

    #############################################################
    ## remove the line-continuation character if present
    #############################################################
    s#\\##;

    #############################################################
    ## remove SUBDIRS, +=/= and leading spaces
    #############################################################
    s#SUBDIRS##;
    s#\+?=##;
    s#^\s+##;

    #############################################################
    ## remove test and emulation subprojects if requested
    #############################################################
    s#[^\s]+Test.*\b##ig if($discardTest);
    s#[^\s]+Emulation.*\b##ig if($discardEmulation);

    #############################################################
    ## it could be that the user has specified more than one subdir for each line
    #############################################################
    my @list = split(/\s+/);
    push @tmpList,  @list;
  }

  unless ($templateFound)
  {
    print STDERR "[Error] $0: $masterProject is not a master project file (No \"TEMPLATE=subdirs\" line found)!\n";
    exit(1);
  }

  print STDERR "[Info] Scan the following dirs for project files:\n".
    join("\n",@tmpList) ."\n" if( @tmpList>0 );

  return @tmpList;
}

#############################################################
##
## scan subproject file (containing headers, sources, etc.)
##
#############################################################

sub scanProjectFile(){

  if(@_<2){
    printf STDERR "$0: Error! scanProjectFile needs 2 arguments and an array!\n";
    exit;
  }

  ## last argument
  my $projectFilename = pop(@_);
  ## one before the last
  my $dir = pop(@_);
  ## remaining arguments
  my @tmpList = (@_);

  ## scan the file and store the file names
  open PROJECT,"$projectFilename" or die "Could not open $projectFilename : $!\n";
  my $interestingSection = 0;
  while (<PROJECT>) 
  {
    chomp;

    #############################################################
    ## eliminate lines starting with '#' (comments)
    #############################################################
    next if(/^\s*#/);

    #############################################################
    ## eliminate comments (i.e. find the first occurrence of '#' and keep only what precedes it)
    #############################################################
    s/([^#]+).*$/$1/ if(/#/);
    next unless(/[^\s]+/);

    #############################################################
    ## only read lines from the sections listing
    ## - HEADERS
    ## - SOURCES
    ## - FORMS
    ## - RESOURCES
    ## The files mentioned in these sections will be added to the list of 
    ## files we have to include in the tar file.
    ## This avoids that file names mentioned elsewhere 
    ## (e.g. in messages or system calls) are included
    ## in our list.
    #############################################################

    if (m/([^#]!?[a-zA-Z_,()\s]*:\s*)?\s*(:?HEADERS|RESOURCES|SOURCES|FORMS)\s*?\+?=/)
    # try to match something like:
    #   unix:!macx: SOURCES += .....
    #if (m/([a-zA-Z_,()\s]:*\s*)?\s*(:?HEADERS|RESOURCES|SOURCES|FORMS)\s*?\+?=/)
    {
      my $line = $_;

      #print STDOUT "matched line: [$line] with [$1]\n";

      #############################################################
      ## check if the keyword is preceded by 'contains(PKG_NAME,<..>)'
      ## and the package name is not ours
      #############################################################

      if (defined $1 && $1 ne "") 
      {
	my $condition = $1;

	#############################################################
	# check if we have something like 'contains(blabla):...'
	#############################################################
	if ($condition =~ m/contains\s*\(\s*PKG_NAME\s*,\s*([^\s]*?)\)/
	    && $1 eq $packageName) 
        {
	  #print STDOUT "We're interested_1...\n";
	  $interestingSection = 1;
	  ## remove the "contains.." from the line
	  s/.*?://;
	} 
        elsif ($condition =~ m/^\s+$/) 
        {
	  #print STDOUT "We're interested_2...\n";
	  $interestingSection = 1 ;
	}
	elsif ($condition =~ m/[a-z]:\s+/)
	{
	  #print STDOUT "We're interested_3...\n";
	  $interestingSection = 1 ;
	}
	else
	{
	   #print STDOUT "We're NOT interested\n";
	}
      } 
      else 
      {
	#print STDOUT "We're interested_4...\n";
	$interestingSection = 1 ;
      }
    }

    next unless ( $interestingSection > 0 );


    #############################################################
    ## no continuation of line found in an interesting section:
    ## this is the last line we are reading here
    #############################################################
    $interestingSection = 0 if($interestingSection && ! (m#\\\s*$#) );

    #############################################################
    ## remove the line-continuation character if present
    #############################################################
    s#\\##;

    #############################################################
    ## remove "<keyword>+=" if present
    #############################################################
    s#.*\+?=##;

    #############################################################
    ## it could be that the user has specified more than a file for each line
    #############################################################
    my @list = split(/\s+/);
    for (my $file=0;$file<@list;++$file) 
    {

      ## store this file if its extension is .h, .cpp, .cxx, .c, 
      ##  .ui (for the qt forms) and .qrc (for the qt resources)
      ## and it is not yet in the list
      ## -- do not store relative paths to this dir but 
      ##    the original paths to the files
      my $thisFile = $dir . "/" . $list[$file];

      ## modify the relative paths with respect to this directory
      ## substituting recursively "<dir1>/../<dir2>" with "<dir2>"
      while ($thisFile =~ m%\.\./% ) 
      {
	$thisFile =~ s#[^/]+/?\.\./([^/]*?)#$1# ;
      }

      push @tmpList,  $thisFile
	if ( $list[$file] =~ m/\.(h+|cpp|cxx|c|qrc|ui)$/
	     && (grep (/\b$thisFile\b/,@tmpList)) == 0 );
    }
  }
  close PROJECT;
  return @tmpList;
}
