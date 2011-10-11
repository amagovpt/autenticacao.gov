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
#include <ctype.h>

#include "PrintBasic.h"

void PrintLine(FILE *f, const char *text,char c, int screen, int uppercase)
{
	unsigned long i=0;
	size_t iMax=0;
	size_t iLen=0;

	char *buff=NULL;

	buff=(char*)malloc(strlen(text)+1);
	strcpy_s(buff,strlen(text)+1,text);

	if(uppercase)
	{
		for(i=0;i<strlen(text);i++)
			buff[i]=toupper(buff[i]);
	}

	iLen=strlen(buff);
	if(iLen)
		iLen+=2;

	//Print on screen
	if(screen)
	{
		iMax=(WIDTH_SCREEN - iLen) / 2;
		for(i=0;i<iMax;i++) printf("%c",c);

		if(iLen)
			printf(" %s ",buff);

		iMax=WIDTH_SCREEN - iLen - iMax;
		for(i=0;i<iMax;i++) printf("%c",c);

		printf("\n");
	}

	//Print in File
	iMax=(WIDTH_FILE - iLen) / 2;
	for(i=0;i<iMax;i++) fprintf(f,"%c",c);

	if(iLen)
		fprintf(f," %s ",buff);

	iMax=WIDTH_FILE - iLen - iMax;
	for(i=0;i<iMax;i++) fprintf(f,"%c",c);

	fprintf(f,"\n");

	free(buff);
}

void PrintHex(FILE *f, const char *title, const unsigned char *buffer, unsigned long len)
{
	unsigned long i=0;
	unsigned long line=0;

	for(i=0,line=0;i<len;i++,line++)
	{
		if(line==WIDTH_FILE/3)
		{
			fprintf(f,"\n");
			line=0;
		}

		fprintf(f,"%02x ",buffer[i]);
	}

	fprintf(f,"\n");
}

void PrintTestHeader(FILE *f, const char *text)
{
	PrintLine(f, "",'*', 0, 0);
	PrintLine(f, text,'*', 0, 1);
	PrintLine(f, "",'*', 0, 0);
}

void PrintHeader(FILE *f, const char *text)
{
	PrintLine(f, text,'#', 1, 1);
}

void PrintTestFunction(FILE *f, const char *text)
{
	PrintLine(f, text,'%', 0, 0);
}

void PrintTitle(FILE *f, const char *text)
{
	PrintLine(f, text,'=', 0, 1);
}

void PrintBlock(FILE *f, const char *text)
{
	PrintLine(f, text,'-', 0, 1);
}

void PrintSeperator(FILE *f)
{
	PrintLine(f, "",'~', 0, 0);
}

void PrintComment(FILE *f, const char *text)
{
	fprintf(f,"---> %s\n", text);
}

void PrintWARNING(FILE *f, const char *text)
{
	printf("WARNING : %s\n", text);
	fprintf(f,"WARNING : %s\n", text);
}

void PrintERROR(const char *text)
{
	printf("ERROR   : %s\n\n", text);
}

void PrintSUCCESS(const char *text)
{
	printf("SUCCESS : %s\n\n", text);
}
