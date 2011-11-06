/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#include <iostream>
#include <QtGui/QApplication>
#include <QFileInfo>
#include "qtzipper.h"

#define OPT_FOLDERNAME "-f"
#define OPT_ZIPFILE    "-o"

char* CmdOptions[]=
{
	  OPT_FOLDERNAME
	, OPT_ZIPFILE
	, ""
};

void Usage( char* exeName )
{
	std::cerr << "Usage:" << std::endl;
	std::cerr << exeName 
			  << " " 
			  << CmdOptions[0]
			  << " <foldername>"
			  << " " 
			  << CmdOptions[1]
			  << " <path\\zipfilename>"
			  << std::endl;
}

int main(int argc, char *argv[])
{
QString folderPath;
QString archiveFileName("ArchiveFile.zrc");

	if (argc <= 1)
	{
		Usage(argv[0]);
		return -1;
	}

	for (int idx=1; idx<argc;idx++)
	{
		if ( 0 == strcmp(argv[idx],OPT_FOLDERNAME) )
		{
			idx++;
			folderPath = argv[idx];
		}
		else if ( 0 == strcmp(argv[idx],OPT_ZIPFILE) )
		{
			idx++;
			archiveFileName = argv[idx];;
		}
		else
		{
			std::cerr << "[Error] Incorrect option: " << argv[idx] << std::endl;
			Usage(argv[0]);
			return -1;
		}
	}

	if (folderPath.length()==0)
	{
		Usage(argv[0]);
		return -1;
	}

	QFileInfo fileinfo(folderPath);
	if (!fileinfo.isDir())
	{
		Usage(argv[0]);
		return -1;
	}

	QtZipper qtZipper;
	qtZipper.zipFolder(folderPath, archiveFileName);
	return 0;
}
