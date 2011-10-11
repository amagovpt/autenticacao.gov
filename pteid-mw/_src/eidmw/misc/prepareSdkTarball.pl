#!/usr/bin/perl -w

#######################################################################
## This script packs the pteid middleware SDK files in a tarball.
## It contains the SDK include files and the necessary .so files
## The script has to be ran after:
## qmake eidmw.pro PKG_NAME=pteid
## make
#######################################################################

use strict;
use File::Path;
use File::Copy;

## set the following variable to '1' if the C-header files have to be included in the SDK
my $IncludeCFiles=0;

## set the following variable to '1' if the Misc/Applet sample has to be included in the SDK
my $IncludeMiscApplet=1;

## set the following variable to '1' if the Basic/Applet sample has to included all wrapper libraries
##    for all platforms
my $IncludeWinWrapper=1;
my $IncludeMacWrapper=1;

#######################################################################
# name of the file containing the maj.min.rev numbers
#######################################################################
my $versionFilename = "../_Builds/pteidversions.mak";
my $version_MAJ = -1;
my $version_MID = -1;
my $version_MIN = -1;

open VERSION, "$versionFilename" or die "[Error] Could not open $versionFilename: $!\n";

#######################################################################
## get the package release version nr
#######################################################################
while(<VERSION>)
{
  if(/^\s*REL_(MAJ|MIN|REV)\s*=\s*(\d+)$/)
  {
    $version_MAJ = $2 if($1 eq "MAJ");
    $version_MID = $2 if($1 eq "MIN");
    $version_MIN = $2 if($1 eq "REV");
  }
  last unless ($version_MAJ == -1 || $version_MID == -1 || $version_MIN == -1);
}
close VERSION;

my $version 	= $version_MAJ.".".$version_MID.".".$version_MIN;

print STDOUT "[Info ] Package version (as found in $versionFilename): $version\n";

#######################################################################
## get the machine architecture
#######################################################################
my $architecture     = `uname -m`;
chomp($architecture);

#######################################################################
## get the compiler version
## can be found in $compiler_version[0],$compiler_version[1],$compiler_version[2]
#######################################################################
my @full_compiler_version = split(/ /,`gcc --version`);
my @compiler_version      = split(/\./,$full_compiler_version[2]);
my $compiler		  = "gcc";

#######################################################################
## get the linux distribution version nr
## the result is in $distribution_version[0] and $distribution_version[1]
## the distribution name is in $full_distribution[4]
#######################################################################
my @full_distribution;
my $distribution_version;
my $distribution_name;

if (-e "/etc/fedora-release")
{
	@full_distribution    = split(/ /,`cat /etc/fedora-release`);
	$distribution_version = $full_distribution[2];
	$distribution_name    = $full_distribution[0];
}
elsif (-e "/etc/SuSE-release")
{
	@full_distribution    = split(/ /,`cat /etc/SuSE-release`);
	$distribution_version = $full_distribution[1];
	$distribution_name    = $full_distribution[0];
}
elsif (-e "/etc/debian_version")
{
	my $tmp = `cat /etc/debian_version`;
	chomp($tmp);	
	$distribution_name    = "debian-" . $tmp;
	$distribution_version = "etch";
}

my $svn_revision = `cat ../svn_revision | tr -d "\r"`;
chomp($svn_revision);
my $tarfile = "pteid-sdk";
$tarfile = $tarfile . "-" . $version;
$tarfile = $tarfile . "-" . lc($distribution_name) . "-" . $distribution_version;
$tarfile = $tarfile . "-" . $architecture;
$tarfile = $tarfile . "-" . $svn_revision;
$tarfile = $tarfile . ".tgz";

my @files_include =	("eidlib.h"
			,"eidlibdefines.h"
			,"eidlibException.h"
			);

#my @files_includeC =	("eidlibC.h"
#			,"eidlibCdefines.h"
#			);

my @files_common =	("eidErrors.h"
			);

# my @files_unsignedjar =	("PTEID_Applet.jar"
my @files_unsignedjar =	("pteid35libJava.jar"
			,"applet-launcher.jar"
			,"Applet-Launcher License.rtf"
			);

#my @files_wrap=	("libpteidlibJava_Wrapper.so.3.5.0"
my @files_wrap=		("libpteidlibJava_Wrapper.so.3.5.3"
			);

my @files_doc =		("readme.txt"
			);

my @files_p11_include =	("cryptoki.h"
			,"pkcs11.h"
			,"pkcs11f.h"
			,"pkcs11t.h"
			);
my @files_license =	("License_sdk_de.rtf"
			,"License_sdk_en.rtf"
			,"License_sdk_fr.rtf"
			,"License_sdk_nl.rtf"
			);
my @files_signed =	("applet-launcher.jar"
			,"pteid35libJava.jar"
			,"PTEID_Applet.jar"
			,"pteid35JavaWrapper-win.jar"
			,"pteid35JavaWrapper-linux.jar"
			,"pteid35JavaWrapper-mac.jar"
			);

#######################################################################
## Check if the package is built
## The directory 'lib' should exists
#######################################################################
my $dirToCheck;
my $baseDir;

$baseDir= "..";

$dirToCheck = "$baseDir/lib";

print STDOUT "[Info ] Checking existence of directory $dirToCheck\n";
opendir(SOME_DIR,"$dirToCheck") or die "[Error] Directory '$dirToCheck' does not exist. is the package built?\n";

$dirToCheck = "$baseDir/bin";

print STDOUT "[Info ] Checking existence of directory $dirToCheck\n";
opendir(SOME_DIR,"$dirToCheck") or die "[Error] Directory '$dirToCheck' does not exist. is the package built?\n";

#######################################################################
## Create the necessary directories
#######################################################################
my $dirToCreate;
my $sdkDir="pteidsdk";
$baseDir = "../install";

print STDOUT "[Info ] Creating directory $baseDir\n";

opendir (SOME_DIR, $baseDir) or mkpath($baseDir);

$dirToCreate = "$baseDir/$sdkDir";

if (-d $dirToCreate)
{
	print "[Error] Directory $dirToCreate exists. Please verify and remove before running this script.\n";
	exit(-1);
}
else 
{
	print "[Error] Cannot create directory $dirToCreate.\n" unless mkpath ($dirToCreate); 
}

$dirToCreate = "$baseDir/$sdkDir/doc";
print STDOUT "[Info ] Creating directory $dirToCreate\n";
mkpath ($dirToCreate) or die "[Error] Cannot create directory $dirToCreate.\n";

if ( $IncludeCFiles == 1 )
{
	$dirToCreate = "$baseDir/$sdkDir/pteidlib/C/include";
	print STDOUT "[Info ] Creating directory $dirToCreate\n";
	mkpath ($dirToCreate) or die "[Error] Cannot create directory $dirToCreate.\n";
}

$dirToCreate = "$baseDir/$sdkDir/pteidlib/C++/include";
print STDOUT "[Info ] Creating directory $dirToCreate\n";
mkpath ($dirToCreate) or die "[Error] Cannot create directory $dirToCreate.\n";

$dirToCreate = "$baseDir/$sdkDir/pteidlib/Java/bin";
print STDOUT "[Info ] Creating directory $dirToCreate\n";
mkpath ($dirToCreate) or die "[Error] Cannot create directory $dirToCreate.\n";

$dirToCreate = "$baseDir/$sdkDir/pteidlib/Java/unsigned";
print STDOUT "[Info ] Creating directory $dirToCreate\n";
mkpath ($dirToCreate) or die "[Error] Cannot create directory $dirToCreate.\n";

$dirToCreate = "$baseDir/$sdkDir/pkcs11/C++/include";
print STDOUT "[Info ] Creating directory $dirToCreate\n";
mkpath ($dirToCreate) or die "[Error] Cannot create directory $dirToCreate.\n";

$dirToCreate = "$baseDir/$sdkDir/samples";
print STDOUT "[Info ] Creating directory $dirToCreate\n";
mkpath ($dirToCreate) or die "[Error] Cannot create directory $dirToCreate.\n";

#######################################################################
## Copy the files in the target directories
#######################################################################
my $fromDir;
my $toDir;


#######################################################################
## Copy all the p11 include files
#######################################################################
$baseDir = "../install";
$fromDir = "../sdk/pkcs11/C++/include";
$toDir   = "$baseDir/$sdkDir/pkcs11/C++/include";

print STDOUT "[Info ] Copying files from $fromDir to $toDir\n";

foreach (@files_p11_include)
{
	copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
}

#######################################################################
## Copy all the eidlib 'C++' include files
#######################################################################
$baseDir = "../install";
$fromDir = "../eidlib";
$toDir   = "$baseDir/$sdkDir/pteidlib/C++/include";

print STDOUT "[Info ] Copying files from $fromDir to $toDir\n";

foreach (@files_include)
{
	copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
}

$baseDir = "../install";
$fromDir = "../common";
$toDir   = "$baseDir/$sdkDir/pteidlib/C++/include";

print STDOUT "[Info ] Copying files from $fromDir to $toDir\n";

foreach (@files_common)
{
	copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
}

if ( $IncludeCFiles == 1 )
{
	#######################################################################
	## Copy all the eidlib include C files
	#######################################################################
#	$baseDir = "../install";
#	$fromDir = "../eidlibC";
#	$toDir   = "$baseDir/$sdkDir/pteidlib/C/include";
#
#	print STDOUT "[Info ] Copying files from $fromDir to $toDir\n";
#
#	foreach (@files_includeC)
#	{
#		copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
#	}

	$baseDir = "../install";
	$fromDir = "../common";
	$toDir   = "$baseDir/$sdkDir/pteidlib/C/include";

	print STDOUT "[Info ] Copying files from $fromDir to $toDir\n";

	foreach (@files_common)
	{
		copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
	}
}
#######################################################################
## Copy all the necessary files to pteidlib/java/unsigned
#######################################################################
$baseDir = "../install";
$fromDir = "../jar";
$toDir   = "$baseDir/$sdkDir/pteidlib/Java/unsigned";

print STDOUT "[Info ] Copying files from $fromDir to $toDir\n";

foreach (@files_unsignedjar)
{
	copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
}

$baseDir = "../install";
$fromDir = "../lib";
$toDir   = "$baseDir/$sdkDir/pteidlib/Java/unsigned";

print STDOUT "[Info ] Copying files from $fromDir to $toDir\n";

foreach (@files_wrap)
{
	copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
}

my $cmd;

$cmd = "cd $toDir; ln -s libpteidlibJava_Wrapper.so.3.5.3 libpteidlibJava_Wrapper.so.3.5";
system($cmd);
$cmd = "cd $toDir; ln -s libpteidlibJava_Wrapper.so.3.5.3 libpteidlibJava_Wrapper.so.3";
system($cmd);
$cmd = "cd $toDir; ln -s libpteidlibJava_Wrapper.so.3.5.3 libpteidlibJava_Wrapper.so";
system($cmd);

#######################################################################
## Copy all the signed jar files
#######################################################################
$baseDir = "../install";
$fromDir = "../../ThirdParty/pteid_sdk/3.5/Java/SignedJar";
$toDir   = "$baseDir/$sdkDir/pteidlib/Java/bin";

print STDOUT "[Info ] Exporting signed jar files from $fromDir to $toDir\n";

foreach (@files_signed)
{
	copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
	#$cmd = "svn cat http://10.2.250.30/svn/ThirdParty/pteid_sdk/3.5/Java/SignedJar/$_ > $toDir/$_";
	#system($cmd);

}
## Make sure to copy the jnlp file
{
	$fromDir = "../jar";
	my $fileToCopy = "pteid.jnlp";
	copy("$fromDir/$fileToCopy","$toDir/$fileToCopy") or die "[Error] Cannot copy file: $fileToCopy.\n";
}
## Make sure to copy the jnlp file
{
	$fromDir = "../jar";
	my $fileToCopy = "PTEID_reader.html";
	copy("$fromDir/$fileToCopy","$toDir/$fileToCopy") or die "[Error] Cannot copy file: $fileToCopy.\n";
}
## Make sure to copy the applet-launcher license file
{
	$fromDir = "../jar";
	my $fileToCopy = "Applet-Launcher License.rtf";
	copy("$fromDir/$fileToCopy","$toDir/$fileToCopy") or die "[Error] Cannot copy file: $fileToCopy.\n";
}


#######################################################################
# for the samples we also need the Mac dylib, windows dll, etc
# these should be stored in SVN
#######################################################################
$baseDir = "../install";
$fromDir = "../../ThirdParty/pteid_sdk/3.5/Java";
$toDir   = "$baseDir/$sdkDir/pteidlib/Java/unsigned";

print STDOUT "[Info ] Exporting files from SVN to '$toDir'\n";


if ($IncludeWinWrapper == 1)
{	
## this is for packaging the windows DLL
#	$cmd = "svn cat http://10.2.250.30/svn/ThirdParty/pteid_sdk/3.5/Java/pteid35libJava_Wrapper.dll > $toDir/pteid35libJava_Wrapper.dll";
#	system($cmd);
	my $filetocopy = "pteid35libJava_Wrapper.dll";
	print STDOUT "[Info ] Copying $filetocopy from $fromDir to $toDir\n";

	$cmd = "cp $fromDir/$filetocopy $toDir";
	system($cmd) == 0 or die "[ERROR ] File $fromDir/$filetocopy not found for packaging\n";
#	exit -1;
}

if ($IncludeMacWrapper == 1)
{	# this is for packaging the Mac jnilib
#	$cmd = "svn cat http://10.2.250.30/svn/ThirdParty/pteid_sdk/3.5/Java/libpteidlibJava_Wrapper.jnilib > $toDir/libpteidlibJava_Wrapper.jnilib";
#	system($cmd);
#	my $filetocopy = "pteid35libJava_Wrapper.jnilib";
	my $filetocopy = "libpteidlibJava_Wrapper.jnilib";
	print STDOUT "[Info ] Copying $filetocopy from $fromDir to $toDir\n";

	$cmd = "cp $fromDir/$filetocopy $toDir";
	system($cmd) == 0 or die "[ERROR ] File $fromDir/$filetocopy not found for packaging\n";
#	exit -1;
}

#######################################################################
## Copy all doc files
#######################################################################
$baseDir = "../install";
$fromDir = "../sdk";
$toDir   = "$baseDir/$sdkDir";

print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";

foreach (@files_doc)
{
	copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
}

#######################################################################
## Copy all license files
#######################################################################
$baseDir = "../install";
$fromDir = "licenses_files";
$toDir   = "$baseDir/$sdkDir";

print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";

foreach (@files_license)
{
	copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
}

#######################################################################
## Copy all sample files
#######################################################################
# sample: events
#######################################################################
my $sample_name;
my $prog_lang;

$baseDir = "../install";

$sample_name="events";
{
	$prog_lang="C++";
	{
		$fromDir = "../sdk/samples/$sample_name/$prog_lang";
		$toDir   = "$baseDir/$sdkDir/samples/$sample_name/$prog_lang";
		mkpath ($toDir) or die "[Error] Cannot create directory $dirToCreate.\n";
	
		print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
	
		my @files_samples=(
			  "main.cpp"
			, "Makefile"
			, "Makefile.mac"
			);

		foreach (@files_samples)
		{
			copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
		}
	}
	$prog_lang="Java";
	{
		$fromDir = "../sdk/samples/$sample_name/$prog_lang";
		$toDir   = "$baseDir/$sdkDir/samples/$sample_name/$prog_lang";
		mkpath ($toDir) or die "[Error] Cannot create directory $dirToCreate.\n";
	
		print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
	
		my @files_samples=(
			  "EventCallback.java"
			, "EventData.java"
			, "ReaderRef.java"
			, "main.java"
			, "compile_events.sh"
			, "run_events.sh"
			);

		foreach (@files_samples)
		{
			copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
		}
		my $setexeflag="chmod +x $toDir/*.sh";
		system($setexeflag);
	}
}
$sample_name="exception";
{
	$prog_lang="C++";
	{
		$fromDir = "../sdk/samples/$sample_name/$prog_lang";
		$toDir   = "$baseDir/$sdkDir/samples/$sample_name/$prog_lang";
		mkpath ($toDir) or die "[Error] Cannot create directory $dirToCreate.\n";
	
		print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
	
		my @files_samples=(
			  "main.cpp"
			, "Makefile"
			, "Makefile.mac"
			);

		foreach (@files_samples)
		{
			copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
		}
	}
	$prog_lang="Java";
	{
		$fromDir = "../sdk/samples/$sample_name/$prog_lang";
		$toDir   = "$baseDir/$sdkDir/samples/$sample_name/$prog_lang";
		mkpath ($toDir) or die "[Error] Cannot create directory $dirToCreate.\n";
	
		print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
	
		my @files_samples=(
			  "get_exception.java"
			, "compile_exception.sh"
			, "run_exception.sh"
			);

		foreach (@files_samples)
		{
			copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
		}
		my $setexeflag="chmod +x $toDir/*.sh";
		system($setexeflag);
	}
}
$sample_name="pin_eid";
{
	if ( $IncludeCFiles == 1 )
	{
		$prog_lang="C";
		{
			$fromDir = "../sdk/samples/$sample_name/$prog_lang";
			$toDir   = "$baseDir/$sdkDir/samples/$sample_name/$prog_lang";
			mkpath ($toDir) or die "[Error] Cannot create directory $dirToCreate.\n";
	
			print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
	
			my @files_samples=(
				  "pin_eid.c"
				, "Makefile"
				, "Makefile.mac"
				);

			foreach (@files_samples)
			{
				copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
			}
		}
	}
	$prog_lang="C++";
	{
		$fromDir = "../sdk/samples/$sample_name/$prog_lang";
		$toDir   = "$baseDir/$sdkDir/samples/$sample_name/$prog_lang";
		mkpath ($toDir) or die "[Error] Cannot create directory $dirToCreate.\n";
	
		print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
	
		my @files_samples=(
			  "main.cpp"
			, "Makefile"
			, "Makefile.mac"
			);

		foreach (@files_samples)
		{
			copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
		}
	}
	$prog_lang="Java";
	{
		$fromDir = "../sdk/samples/$sample_name/$prog_lang";
		$toDir   = "$baseDir/$sdkDir/samples/$sample_name/$prog_lang";
		mkpath ($toDir) or die "[Error] Cannot create directory $dirToCreate.\n";
	
		print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
	
		my @files_samples=(
			  "main.java"
			, "compile.sh"
			, "run.sh"
			);

		foreach (@files_samples)
		{
			copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
		}
		my $setexeflag="chmod +x $toDir/*.sh";
		system($setexeflag);
	}
}
$sample_name="read_eid";
{
	if ( $IncludeCFiles == 1 )
	{
		$prog_lang="C";
		{
			$fromDir = "../sdk/samples/$sample_name/$prog_lang";
			$toDir   = "$baseDir/$sdkDir/samples/$sample_name/$prog_lang";
			mkpath ($toDir) or die "[Error] Cannot create directory $dirToCreate.\n";
	
			print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
	
			my @files_samples=(
				  "read_eid.c"
				, "Makefile"
				, "Makefile.mac"
				);

			foreach (@files_samples)
			{
				copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
			}
		}
	}
	$prog_lang="C++";
	{
		$fromDir = "../sdk/samples/$sample_name/$prog_lang";
		$toDir   = "$baseDir/$sdkDir/samples/$sample_name/$prog_lang";
		mkpath ($toDir) or die "[Error] Cannot create directory $dirToCreate.\n";
	
		print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
	
		my @files_samples=(
			  "main.cpp"
			, "Makefile"
			, "Makefile.mac"
			);

		foreach (@files_samples)
		{
			copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
		}
	}
	$prog_lang="Java";
	{
		$fromDir = "../sdk/samples/$sample_name/$prog_lang";
		$toDir   = "$baseDir/$sdkDir/samples/$sample_name/$prog_lang";
		mkpath ($toDir) or die "[Error] Cannot create directory $dirToCreate.\n";
	
		print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
	
		my @files_samples=(
			  "main.java"
			, "copy_binaries.sh"
			, "compile_read_eid.sh"
			, "run_read_eid.sh"
			);

		foreach (@files_samples)
		{
			copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
		}
		my $setexeflag="chmod +x $toDir/*.sh";
		system($setexeflag);
	}
}
$sample_name="wait_card";
{
	$prog_lang="C++";
	{
		$fromDir = "../sdk/samples/$sample_name/$prog_lang";
		$toDir   = "$baseDir/$sdkDir/samples/$sample_name/$prog_lang";
		mkpath ($toDir) or die "[Error] Cannot create directory $dirToCreate.\n";
	
		print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
	
		my @files_samples=(
			  "main.cpp"
			, "Makefile"
			, "Makefile.mac"
			);

		foreach (@files_samples)
		{
			copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
		}
	}
	$prog_lang="Java";
	{
		$fromDir = "../sdk/samples/$sample_name/$prog_lang";
		$toDir   = "$baseDir/$sdkDir/samples/$sample_name/$prog_lang";
		mkpath ($toDir) or die "[Error] Cannot create directory $dirToCreate.\n";
	
		print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
	
		my @files_samples=(
			  "main.java"
			, "compile_wait_card.sh"
			, "run_wait_card.sh"
			);

		foreach (@files_samples)
		{
			copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
		}
		my $setexeflag="chmod +x $toDir/*.sh";
		system($setexeflag);
	}
}
$sample_name="sign_p11";
{
	$prog_lang="C++";
	{
		$fromDir = "../sdk/samples/$sample_name/$prog_lang";
		$toDir   = "$baseDir/$sdkDir/samples/$sample_name/$prog_lang";
		mkpath ($toDir) or die "[Error] Cannot create directory $dirToCreate.\n";
	
		print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
	
		my @files_samples=(
			  "main.cpp"
			, "Makefile"
			);

		foreach (@files_samples)
		{
			copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
		}
	}
	$prog_lang="java";
	{
		$fromDir = "../sdk/samples/$sample_name/$prog_lang";
		$toDir   = "$baseDir/$sdkDir/samples/$sample_name/$prog_lang";
		mkpath ($toDir) or die "[Error] Cannot create directory $dirToCreate.\n";
	
		print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
	
		my @files_samples=(
			  "sign_p11.java"
			, "compile.sh"
			, "run.sh"
			);

		foreach (@files_samples)
		{
			copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
		}
		my $setexeflag="chmod +x $toDir/*.sh";
		system($setexeflag);
	}
}
if ( $IncludeMiscApplet == 1 )
{
	$sample_name="misc";
	{
		$prog_lang="Applet/Image/java";
		{
			$fromDir = "../sdk/samples/$sample_name/$prog_lang";
			$toDir   = "$baseDir/$sdkDir/samples/$sample_name/$prog_lang";
			mkpath ($toDir) or die "[Error] Cannot create directory $dirToCreate.\n";
		
			print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
		
			my @files_samples=(
				  "copy_binaries.sh"
				, "Base64.java"
				, "PTEID_ImgApplet.java"
				, "PTEID_sample_base64.html"
				, "compile.sh"
				, "pteid35test.store"
				, "excanvas.js"
				, "generate_jnlp.sh"
				, "Readme.txt"
				);
	
			foreach (@files_samples)
			{
				copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
			}
			my $setexeflag="chmod +x $toDir/*.sh";
			system($setexeflag);

			$fromDir = "../jar";
			my $fileToCopy = "Applet-Launcher License.rtf";
			copy("$fromDir/$fileToCopy","$toDir") or die "[Error] Cannot copy file: $fileToCopy.\n";
		}
		$prog_lang="Applet/Basic/java";
		{
			$fromDir = "../sdk/samples/$sample_name/$prog_lang";
			$toDir   = "$baseDir/$sdkDir/samples/$sample_name/$prog_lang";
			mkpath ($toDir) or die "[Error] Cannot create directory $dirToCreate.\n";
		
			print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
		
			my @files_samples=(
				  "copy_binaries.sh"
				, "generate_jnlp.sh"
				, "compile.sh"
				, "pteid35test.store"
				, "Readme.txt"
				);
	
			foreach (@files_samples)
			{
				copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
			}

			copy("../jar/PTEID_reader.html","$toDir") or die "[Error] Cannot copy file: PTEID_reader.html.\n";
			copy("../eidlibJava_Applet/PTEID_Applet.java","$toDir") or die "[Error] Cannot copy file: PTEID_Applet.java.\n";

			my $setexeflag="chmod +x $toDir/*.sh";
			system($setexeflag);

			$fromDir = "../jar";
			my $fileToCopy = "Applet-Launcher License.rtf";
			copy("$fromDir/$fileToCopy","$toDir") or die "[Error] Cannot copy file: $fileToCopy.\n";

		}
	}
}


#######################################################################
## Copy all SDK C++ documentation files
#######################################################################

$baseDir = "../install";
$fromDir = "../_DocsExternal/C++_Api";
$toDir   = "$baseDir/$sdkDir/doc/C++_Api";

print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";

$cmd = "cp -R " . $fromDir . " " . $toDir;
system($cmd);

#######################################################################
## Copy all SDK C documentation files
#######################################################################

if ( $IncludeCFiles == 1 )
{
	#######################################################################
	## Copy all SDK C documentation files
	#######################################################################
	$baseDir = "../install";
	$fromDir = "../_DocsExternal/C_Api";
	$toDir   = "$baseDir/$sdkDir/doc/C_Api";

	print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";

	$cmd = "cp -R " . $fromDir . " " . $toDir;
	system($cmd);
}

#######################################################################
## Copy all SDK Java documentation files
#######################################################################

$baseDir = "../install";
$fromDir = "../_DocsExternal/Java_Api";
$toDir   = "$baseDir/$sdkDir/doc/Java_Api";

print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";

$cmd = "cp -R " . $fromDir . " " . $toDir;
system($cmd);

#######################################################################
## Copy all Java applet sample files
#######################################################################
#$baseDir = "../install";
#$fromDir = "../eidlibJava_Applet";
#$toDir   = "$baseDir/$sdkDir/samples/applet/Java";
#
#print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
#
#foreach (@files_samples_applet_java)
#{
#	copy("$fromDir/$_","$toDir/$_") or die "[Error] Cannot copy file: $_.\n";
#}
#
#$fromDir = "../jar";
#print STDOUT "[Info ] Copying files from $fromDir to $toDir.\n";
#copy("$fromDir/PTEID_reader.html","$toDir") or die "[Error] Cannot copy file: $_.\n";


#######################################################################
## tar the install directory
#######################################################################

my $tardir = "../install";

print STDOUT "[Info ] Tar-ing to file $tarfile.\n";

chdir($tardir);
$cmd = "tar czf $tarfile $sdkDir";
system($cmd);

#######################################################################
## Remove the directory we tarred
#######################################################################
if (-e $tarfile)
{
	print STDOUT "[Info ] Removing tar-ed directories and files.\n";
	$cmd = "rm -r $sdkDir";
	system($cmd);
}
else
{
	print STDERR "[Error] Can not create $tarfile.\n";
	exit(-1);
}

print STDOUT "[Info ] Tarfile $tarfile created.\n";
print STDOUT "[Info ] Done...\n";

exit(0);

