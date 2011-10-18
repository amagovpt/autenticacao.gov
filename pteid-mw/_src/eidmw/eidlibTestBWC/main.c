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

#include "eidlib.h"

#include "FileUtil.h"
#include "PrintBasic.h"
#include "PrintStruct.h"

#include "TestRead.h"
#include "TestPin.h"
#include "TestCert.h"
#include "TestOther.h"
#include "TestRawFile.h"
#include "TestRawData.h"


static long ParseCommandLine(int argc, char **argv);
static long PrintUsage(const char *csMesg);

static int g_bHelp = 0;
static int g_bTestRead = 0;
static int g_bTestPin = 0;
static int g_bTestCert = 0;
static int g_bTestVirtual = 0;
static int g_bTestOther = 0;
static int g_bVerify = 0;
static char *g_csFolder = ".";
static char *g_csReader = "";
static int g_Ocsp = -1;
static int g_Crl = -1;

int main(int argc, char* argv[])
{
	int lRet = ParseCommandLine(argc, argv);
	if (lRet != 0)
		return lRet;

	if(g_bHelp)
		lRet = PrintUsage(NULL);

	if(!g_bHelp && !g_bTestRead && !g_bTestPin && !g_bTestCert && !g_bTestVirtual && !g_bTestOther)
		PrintUsage("No test specified, exiting..");

	if(g_bTestRead)
		lRet = test_Read(g_csFolder,g_csReader,g_bVerify,g_Ocsp,g_Crl);

	if(g_bTestPin)
		lRet = test_Pin(g_csFolder,g_csReader,g_bVerify,g_Ocsp,g_Crl);

	if(g_bTestCert)
		lRet = test_Cert(g_csFolder,g_csReader,g_bVerify,g_Ocsp,g_Crl);

	if(g_bTestOther)
		lRet = test_Other(g_csFolder,g_csReader,g_bVerify,g_Ocsp,g_Crl);

	if(g_bTestVirtual)
	{
		lRet = test_RawData(g_csFolder,g_csReader,g_bVerify,g_Ocsp,g_Crl);
		lRet = test_RawFile(g_csFolder,g_csReader,g_bVerify,g_Ocsp,g_Crl);
	}

	return (int) lRet;
}

///////////////////////////// Parse command line //////////////////////////

#define INCREMENT_TEST_INDEX(cmd, cmdargs) \
	iIndex++; \
	if (iIndex >= argc) { \
		printf("ERR: \"%s\" needs %s argument(s)\n", cmd, cmdargs); \
		return -1; \
	}\

long ParseCommandLine(int argc, char **argv)
{
	int iIndex = 0;

	for (iIndex = 1; iIndex < argc; iIndex++)
	{
		char *arg = argv[iIndex];

		if (!strcmp(arg, "--help") || !strcmp(arg, "-h"))
			g_bHelp=1;

		else if (!strcmp(arg, "--all") || !strcmp(arg, "-a"))
		{
			g_bTestRead		= (g_bTestRead?0:1);
			g_bTestPin		= (g_bTestPin?0:1);
			g_bTestCert		= (g_bTestCert?0:1);
			g_bTestVirtual	= (g_bTestVirtual?0:1);
			g_bTestOther	= (g_bTestOther?0:1);
		}
		else if (!strcmp(arg, "--read") || !strcmp(arg, "-r"))
			g_bTestRead		= (g_bTestRead?0:1);

		else if (!strcmp(arg, "--pin") || !strcmp(arg, "-p"))
			g_bTestPin		= (g_bTestPin?0:1);

		else if (!strcmp(arg, "--cert") || !strcmp(arg, "-c"))
			g_bTestCert		= (g_bTestCert?0:1);

		else if (!strcmp(arg, "--virtual") || !strcmp(arg, "-v"))
			g_bTestVirtual	= (g_bTestVirtual?0:1);

		else if (!strcmp(arg, "--other") || !strcmp(arg, "-o"))
			g_bTestOther	= (g_bTestOther?0:1);

		else if (!strcmp(arg, "--Reader") || !strcmp(arg, "-R"))
		{
			INCREMENT_TEST_INDEX("--Reader", "1");
			g_csReader = argv[iIndex];
		}
		else if (!strcmp(arg, "--Folder") || !strcmp(arg, "-F"))
		{
			INCREMENT_TEST_INDEX("--Folder", "1");
			g_csFolder = argv[iIndex];
		}
		else if (!strcmp(arg, "--Ocsp") || !strcmp(arg, "-O"))
		{
			INCREMENT_TEST_INDEX("--Ocsp", "1");
			if(strcmp(argv[iIndex],"0")==0 || strcmp(argv[iIndex],"1")==0 || strcmp(argv[iIndex],"2")==0)
			{
				g_Ocsp=atoi(argv[iIndex]);
			}
			else
			{
				printf("ERR: \"--Ocsp\" needs value 0, 1 or 2\n"); 
				return -1;
			}
		}
		else if (!strcmp(arg, "--Crl") || !strcmp(arg, "-C"))
		{
			INCREMENT_TEST_INDEX("--Crl", "1");
			if(strcmp(argv[iIndex],"0")==0 || strcmp(argv[iIndex],"1")==0 || strcmp(argv[iIndex],"2")==0)
			{
				g_Crl=atoi(argv[iIndex]);
			}
			else
			{
				printf("ERR: \"--Crl\" needs value 0, 1 or 2\n"); 
				return -1;
			}
		}
		else if (!strcmp(arg, "--Verify") || !strcmp(arg, "-V"))
		{
			g_bVerify = 1;
		}
		else
		{
			printf("unknown option \"%s\", exiting\n", arg);
			return -1;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////

static long PrintUsage(const char *csMesg)
{
	printf("\n");
	if (csMesg)
		printf("%s\n\n", csMesg);
	printf("Command line parameters:\n");
	printf("  --help or -h : print help\n");
	printf("  --read or -r : run read test\n");
	printf("  --pin or -p : run test about verify/change\n");
	printf("  --cert or -c : run test about certificates\n");
	printf("  --virtual or -v : run test using virtual reader\n");
	printf("  --other or -o : run other tests (apdu, read/write files,...)\n");
	printf("  --all or -a : run all tests\n");
	printf("       if combined with other test parameters turn these test off\n");
	printf("       ex : -a -p run all test but not pin test\n");
	printf("  --Reader <name> or -R ... : specify a reader name\n");
	printf("  --Folder <folder> or -F ... : specify the folder for the output files \n");
	printf("  --Verify or -V  : verify the result.\n");
	printf("       if this flag is NOT set, result file name is postfix with '%s'\n",SUFFIX_OLD);
	printf("       if this flag is set, result file name is postfix with '%s'\n",SUFFIX_NEW);
	printf("          AND a comparaison is made between '%s' and '%s' result files.\n",SUFFIX_OLD,SUFFIX_NEW);
	printf("  --Ocsp or -O  : ocsp validation level (0=not used, 1=optional, 2=mandatory)\n");
	printf("  --Crl or -C  : crl validation level (0=not used, 1=optional, 2=mandatory)\n");
	printf("       if Ocsp/Crl validation level is not set, all level are tested\n");
	printf("\nexample:\n");
	printf("   first create the files : beidlibTestBWC.exe -r -f ./result\n");
	printf("   then check the files : beidlibTestBWC.exe -V -r -f ./result\n");

	return 0;
}