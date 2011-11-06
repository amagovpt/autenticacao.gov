#!/usr/bin/perl -w

use strict;

sub WriteRevision();
sub WriteRevisionH();

my $numArgs = $#ARGV + 1;

if ($numArgs>0)
{
	my $cmd = "echo " . $ARGV[0] . " > svn_info";
	system($cmd);
}
else
{
	system("svn info | grep Revision > svn_info");
}

open SVN_INFO, "< svn_info" or die "[Error] Could not open svn_info\n";
$_=<SVN_INFO>;
$_ =~ m/([0-9]+)/ or die "[Error] Revision not found\n";
my $Revision=$1;
$Revision+=6000;

close SVN_INFO;

WriteRevision();
WriteRevisionH();

##--------------------------------------------------------------------
sub WriteRevision()
{
  open FILE, "> svn_revision" or die "[Error] Could not open svn_revision\n";
  print FILE $Revision."\n";
  close FILE;
}

##--------------------------------------------------------------------
sub WriteRevisionH()
{
  open FILE, "> svn_revision.h" or die "[Error] Could not open svn_revision.h\n";

  print FILE "/* ****************************************************************************\n";
  print FILE "\n";
  print FILE " * eID Middleware Project.\n";
  print FILE " * Copyright (C) 2008-2009 FedICT.\n";
  print FILE " *\n";
  print FILE " * This is free software; you can redistribute it and/or modify it\n";
  print FILE " * under the terms of the GNU Lesser General Public License version\n";
  print FILE " * 3.0 as published by the Free Software Foundation.\n";
  print FILE " *\n";
  print FILE " * This software is distributed in the hope that it will be useful,\n";
  print FILE " * but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
  print FILE " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU\n";
  print FILE " * Lesser General Public License for more details.\n";
  print FILE " *\n";
  print FILE " * You should have received a copy of the GNU Lesser General Public\n";
  print FILE " * License along with this software; if not, see\n";
  print FILE " * http://www.gnu.org/licenses/.\n";
  print FILE "\n";
  print FILE "**************************************************************************** */\n";
  print FILE "#ifndef __SVN_REVISION_H__\n";
  print FILE "#define __SVN_REVISION_H__\n";
  print FILE "\n";
  print FILE "#define SVN_REVISION ".$Revision."\n";
  print FILE "#define SVN_REVISION_STR \"".$Revision."\"\n";
  print FILE "\n";
  print FILE "#endif //__SVN_REVISION_H__\n";

  close FILE;
}

