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
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "PrintBasic.h"
#include "PrintStruct.h"

#include "FileUtil.h"

///////////////////////////// File functions //////////////////////////
FILE *FileOpen(const char *name	, const char *folder, int bVerify, int ocsp, int crl)
{
	FILE *f=NULL;
	char *buff=NULL;
	char *suffix=NULL;
	size_t len=0;
	errno_t err=0;

	if(bVerify)
		suffix=SUFFIX_NEW;
	else
		suffix=SUFFIX_OLD;

	len=strlen(folder)+strlen(name)+strlen(suffix)+5;
	buff=(char *)malloc(len);

	sprintf_s(buff,len,"%s\\%s%d%d_%s",folder,name,ocsp,crl,suffix);

	if(0 != (err=fopen_s(&f,buff,"w")))
		printf("ERROR : could not open file : %s\n",buff);

	free(buff);

	return f;
}

void FileClose(FILE *f)
{
	fclose(f);
}

int FileVerify(const char *name, const char *folder, int bVerify, int ocsp, int crl)
{
	FILE *fOld=NULL;
	FILE *fNew=NULL;
	char *buffOld=NULL;
	char *buffNew=NULL;
	char *buffResult=NULL;
	size_t lenOld=0;
	size_t lenNew=0;
	unsigned char *contentOld=NULL;
	unsigned char *contentNew=NULL;
	int lRet = 0;
	errno_t err=0;

#ifdef WIN32
    struct _stat sizeOld = {0};
    struct _stat sizeNew = {0};
#else
	struct stat sizeOld = {0};
	struct stat sizeNew = {0};
#endif
 
	if(!bVerify)
		return 0;

	//Open the reference file
	lenOld=strlen(folder)+strlen(name)+strlen(SUFFIX_OLD)+5;
	buffOld=(char *)malloc(lenOld);

	sprintf_s(buffOld,lenOld,"%s\\%s%d%d_%s",folder,name,ocsp,crl,SUFFIX_OLD);

	if(0 != (err=fopen_s(&fOld,buffOld,"rb")))
	{
		printf("ERROR : could not open file : %s\n",buffOld);
		free(buffOld);
		return -1;
	}

	free(buffOld);

	//Open the result file
	lenNew=strlen(folder)+strlen(name)+strlen(SUFFIX_NEW)+5;
	buffNew=(char *)malloc(lenNew);

	sprintf_s(buffNew,lenNew,"%s\\%s%d%d_%s",folder,name,ocsp,crl,SUFFIX_NEW);

	if(0 != (err=fopen_s(&fNew,buffNew,"rb")))
	{
		printf("ERROR : could not open file : %s\n",buffNew);
		fclose(fOld);
		free(buffNew);
		return -1;
	}

#ifdef WIN32
    if(0 != _fstat(_fileno(fOld), &sizeOld))
#else
    if(0 != fstat(fileno(fOld), &sizeOld))
#endif
    {
		printf("ERROR : file %s_%s could not be read\n",name,SUFFIX_OLD);
		free(buffNew);
		fclose(fOld);
		fclose(fNew);
		return -1;
	}

    contentOld = (unsigned char *)malloc(sizeOld.st_size);
	if( sizeOld.st_size != fread(contentOld, sizeof(unsigned char), sizeOld.st_size, fOld))
	{
		printf("ERROR : file %s_%s could not be read\n",name,SUFFIX_OLD);
		free(buffNew);
		free(contentOld);
		fclose(fOld);
		fclose(fNew);
		return -1;
	}

#ifdef WIN32
    if(0 != _fstat(_fileno(fNew), &sizeNew))
#else
    if(0 != fstat(fileno(fNew), &sizeNew))
#endif
    {
		printf("ERROR : file %s_%s could not be read\n",name,SUFFIX_NEW);
		free(buffNew);
		free(contentOld);
		fclose(fOld);
		fclose(fNew);
		return -1;
	}

    contentNew = (unsigned char *)malloc(sizeNew.st_size);
	if( sizeNew.st_size != fread(contentNew, sizeof(unsigned char), sizeNew.st_size, fNew))
	{
		printf("ERROR : file %s_%s could not be read\n",name,SUFFIX_NEW);
		free(buffNew);
		free(contentOld);
		free(contentNew);
		fclose(fOld);
		fclose(fNew);
		return -1;
	}

	if(sizeOld.st_size!=sizeNew.st_size)
	{
		PrintERROR("The result files have NOT the same SIZE");
		lRet = -1;
	}
	else
	{
		if(memcmp(contentOld,contentNew,sizeNew.st_size)!=0)
		{
			PrintERROR("The result files have NOT the same CONTENT");
			lRet = -1;
		}
	}

	free(contentOld);
	free(contentNew);

	fclose(fOld);
	fclose(fNew);

	if(lRet==0)
	{
		PrintSUCCESS("Test runs successfully");
	}
	else
	{
		buffResult=(char *)malloc(lenNew+6);
		sprintf_s(buffResult,lenNew+6,"%s_error",buffNew);

		rename(buffNew,buffResult);

		free(buffResult);
	}

	free(buffNew);

	return lRet;
}
