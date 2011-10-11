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
// GetSVNVersion.cpp : Defines the entry point for the console application.
//

#ifdef WIN32
#include "stdafx.h"
#endif

#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

using namespace std;

#ifndef WIN32
#define fprintf_s fprintf
#endif

#ifdef WIN32
int main(int argc, char** argv)
#else
int main()
#endif
{
	FILE *f=NULL;
	long version=0;
	
	string pathout="";
	string fileout;

	if(argc==3)
	{
		version=atol(argv[1]);
		pathout=argv[2];
		pathout+="\\";
	}
	else
	{
#ifdef WIN32
		fopen_s(&f,".svn/entries", "rb");
#else
		f = fopen(".svn/entries", "rb");
#endif

		if(f == NULL)
			return 1;

#ifdef WIN32
		struct _stat buf = {0};
		if(0 == _fstat(_fileno(f), &buf))
#else
		struct stat buf = {0};
		if(0 == fstat(fileno(f), &buf))
#endif
		{
			size_t sizeread = 0;
			unsigned char *pBuffer = (unsigned char *)malloc(buf.st_size);
			sizeread = fread(pBuffer, sizeof(unsigned char), buf.st_size, f);

			if(sizeread != buf.st_size)
			{
				free(pBuffer);
				return 1;
			}

			//check for the begining
			bool bFound=false;
			unsigned long begin=0;
			unsigned long count=0;
			for(unsigned long i=0;i<sizeread;i++)
			{
				if(pBuffer[i]==0x0A)
				{
					count++;
					if(count==3)
					{
						begin=i+1;
					}
					else if(count==4)
					{
						pBuffer[i]=0;
						bFound=true;
						break;
					}
				}
			}

			if(!bFound)
			{
				free(pBuffer);
				return 1;
			}

			char *stop=NULL;
			version=strtol((char *)&pBuffer[begin],&stop,10);

 			free(pBuffer);
	   }

		fclose(f); 
	}

///////////
//.h     //
///////////

	fileout=pathout+"svn_revision.h";
	version+=6000;
#ifdef WIN32
	fopen_s(&f,fileout.c_str(), "w");
#else
	f = fopen(fileout.c_str(), "w");
#endif

	if(f == NULL)
		return 1;

	fprintf_s(f, "/* ****************************************************************************\n");
	fprintf_s(f,"\n");
	fprintf_s(f, " * eID Middleware Project.\n");
	fprintf_s(f, " * Copyright (C) 2008-2009 FedICT.\n");
	fprintf_s(f, " *\n");
	fprintf_s(f, " * This is free software; you can redistribute it and/or modify it\n");
	fprintf_s(f, " * under the terms of the GNU Lesser General Public License version\n");
	fprintf_s(f, " * 3.0 as published by the Free Software Foundation.\n");
	fprintf_s(f, " *\n");
	fprintf_s(f, " * This software is distributed in the hope that it will be useful,\n");
	fprintf_s(f, " * but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
	fprintf_s(f, " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU\n");
	fprintf_s(f, " * Lesser General Public License for more details.\n");
	fprintf_s(f, " *\n");
	fprintf_s(f, " * You should have received a copy of the GNU Lesser General Public\n");
	fprintf_s(f, " * License along with this software; if not, see\n");
	fprintf_s(f, " * http://www.gnu.org/licenses/.\n");
	fprintf_s(f,"\n");
	fprintf_s(f, "**************************************************************************** */\n");
	fprintf_s(f,"#ifndef __SVN_REVISION_H__\n");
	fprintf_s(f,"#define __SVN_REVISION_H__\n");
	fprintf_s(f,"\n");
	fprintf_s(f,"#define SVN_REVISION %ld\n",version);
	fprintf_s(f,"#define SVN_REVISION_STR \"%ld\"\n",version);
	fprintf_s(f,"\n");
	fprintf_s(f,"#endif //__SVN_REVISION_H__\n");

	fclose(f);

//////////////
//Text file //
//////////////
	fileout=pathout+"svn_revision";

#ifdef WIN32
	fopen_s(&f,fileout.c_str(), "w");
#else
	f = fopen(fileout.c_str(), "w");
#endif

	if(f == NULL)
		return 1;

	fprintf_s(f,"%ld\n",version);

	fclose(f);

/*
////////////////////////////////////////
//Batch for rename Windows setup file //
////////////////////////////////////////
	fileout=pathout+"rename-win-setup.bat";

#ifdef WIN32
	fopen_s(&f,fileout.c_str(), "w");

	if(f == NULL)
		return 1;

	fprintf_s(f,"call upload-win is %ld\n",version);

	fclose(f);
#endif

////////////////////////////////////////////
//Batch for rename Windows setup sdk file //
////////////////////////////////////////////
	fileout=pathout+"rename-win-setup-sdk.bat";

#ifdef WIN32
	fopen_s(&f,fileout.c_str(), "w");

	if(f == NULL)
		return 1;

	fprintf_s(f,"call upload-win is-sdk %ld\n",version);

	fclose(f);
#endif

////////////////////////////////////////////
//Batch for rename Windows setup 3.5 file //
////////////////////////////////////////////
	fileout=pathout+"rename-win-setup-3.5.bat";

#ifdef WIN32
	fopen_s(&f,fileout.c_str(), "w");

	if(f == NULL)
		return 1;

	fprintf_s(f,"call upload-win is35 %ld\n",version);

	fclose(f);
#endif

////////////////////////////////////////////////
//Batch for rename Windows setup 3.5 pro file //
////////////////////////////////////////////////
	fileout=pathout+"rename-win-setup-pro-3.5.bat";

#ifdef WIN32
	fopen_s(&f,fileout.c_str(), "w");

	if(f == NULL)
		return 1;

	fprintf_s(f,"call upload-win is35-pro %ld\n",version);

	fclose(f);
#endif

////////////////////////////////////////////////
//Batch for rename Windows setup 3.5 sdk file //
////////////////////////////////////////////////
	fileout=pathout+"rename-win-setup-sdk-3.5.bat";

#ifdef WIN32
	fopen_s(&f,fileout.c_str(), "w");

	if(f == NULL)
		return 1;

	fprintf_s(f,"call upload-win is35-sdk %ld\n",version);

	fclose(f);
#endif

////////////////////////////////////////////
//Batch for rename Windows msi 3.5 file //
////////////////////////////////////////////
	fileout=pathout+"rename-win-msi-3.5.bat";

#ifdef WIN32
	fopen_s(&f,fileout.c_str(), "w");

	if(f == NULL)
		return 1;

	fprintf_s(f,"call upload-win msi35 %ld\n",version);

	fclose(f);
#endif

////////////////////////////////////////////////
//Batch for rename Windows msi 3.5 pro file //
////////////////////////////////////////////////
	fileout=pathout+"rename-win-msi-pro-3.5.bat";

#ifdef WIN32
	fopen_s(&f,fileout.c_str(), "w");

	if(f == NULL)
		return 1;

	fprintf_s(f,"call upload-win msi35-pro %ld\n",version);

	fclose(f);
#endif

////////////////////////////////////////////////
//Batch for rename Windows msi 3.5 sdk file //
////////////////////////////////////////////////
	fileout=pathout+"rename-win-msi-sdk-3.5.bat";

#ifdef WIN32
	fopen_s(&f,fileout.c_str(), "w");

	if(f == NULL)
		return 1;

	fprintf_s(f,"call upload-win msi35-sdk %ld\n",version);

	fclose(f);
#endif
*/

/////////////////////////
//Include file for WiX //
/////////////////////////
	fileout=pathout+"svn_revision.wxs";

#ifdef WIN32
	fopen_s(&f,fileout.c_str(), "w");

	if(f == NULL)
		return 1;

	fprintf_s(f,"<Include>\n");
	fprintf_s(f,"<?define RevisionNumber=%ld?>\n",version);
	fprintf_s(f,"</Include>\n");

	fclose(f);
#endif

	return 0;
}

