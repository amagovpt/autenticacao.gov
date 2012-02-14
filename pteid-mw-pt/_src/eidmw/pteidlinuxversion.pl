#!/usr/bin/env perl

# This script is intented to find which Linux distro PTeID is running
# this is useful to auto updates mechanism to find supported distros.
# Author: Luis Medinas <luis.medinas@caixamagica.pt>

use strict;

my $ARCH=`uname -m`;
my @full_distribution;
my $distro;
my $distribution_version;

print ("PTeID OS version!!!!\n\n");

sub getarch
{
    print ("Checking Arch...\n");
    chomp $ARCH;
    return $ARCH;
}

sub getlinuxdistro
{
    print ("Checking Linux Distro...\n");

    if ($^O =~ m/linux/)
    {
        if (-e "/etc/fedora-release")
        {
	    @full_distribution    = split(/ /,`cat /etc/fedora-release`);
	    $distribution_version = $full_distribution[2];
	    chomp $distribution_version;
	    $distro               = "fedora";
        }
	elsif (-e "/etc/redhat-release")
        {
            @full_distribution    = split(/ /,`cat /etc/redhat-release`);
            $distribution_version = $full_distribution[2];
	    chomp $distribution_version;
            $distro               = "redhat";
        }
        elsif (-e "/etc/SuSE-release")
        {
	    @full_distribution    = split(/ /,`cat /etc/SuSE-release`);
	    $distribution_version = $full_distribution[1];
	    chomp $distribution_version;
	    $distro               = "suse";
        }
	elsif (-e "/etc/mandriva-release")
        {
            @full_distribution    = split(/ /,`/etc/mandriva-release`);
            $distribution_version = $full_distribution[1];
	    chomp $distribution_version;
            $distro               = "mandriva";
	}
        elsif (-e "/etc/debian_version")
        {
	    my $tmp = `cat /etc/debian_version`;
	    chomp($tmp);    
	    $distribution_version = "squeeze";
	    chomp $distribution_version;
	    $distro               = "debian";
        }
	#Support for Ubuntu/Caixamagica
	elsif (-e "/etc/lsb-release")
        {
	    my $tmp = `cat /etc/lsb-release`;
	    my @values = ($tmp =~ m/=(\S+)/g);
	    join(',', @values), "\n";
	    
	    if ($values[0] =~ m/CaixaMagica/ || $values[0] =~ m/Ubuntu/)
	    {
		$distro = @values[0];
		$distribution_version = @values[1];
	    }

	}
        else
        {
	    $distro = "unsupported";
        }
    }

    return $distro;
}


my $linuxdistro = getlinuxdistro;

print "$linuxdistro\n";

my $linuxarch = getarch;

print "$linuxarch\n";

print "Writing to File\n";

open (LINUXVERSIONFILE, '>/tmp/linuxversion');
print LINUXVERSIONFILE $linuxdistro;
print LINUXVERSIONFILE " ; ";
print LINUXVERSIONFILE $linuxarch;
print LINUXVERSIONFILE " ; ";
print LINUXVERSIONFILE $distribution_version;
print LINUXVERSIONFILE " ; \n";
close (LINUXVERSIONFILE);
