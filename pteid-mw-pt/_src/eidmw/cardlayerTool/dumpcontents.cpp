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
#include "../cardlayer/CardLayer.h"
#include "../common/Util.h"
#include <stdio.h>


// Can be used to make a dump in source code in order to make
// it easy to create a new Emulated card with the data that
// was read from the card
//#define DUMP_FOR_EMULATION

using namespace eIDMW;

extern CCardLayer g_oCardLayer;

static std::string g_csSerialNr;

static void DumpFile(CReader &oReader,
	const std::string & csTitle, const std::string & csPath);

static long DumpPteidContents(CReader &oReader);

long DumpContents(const std::string & csReaderName)
{
	long lRet = 0;

	try 
	{
		CCardLayer g_oCardLayer;
		CReader &oReader = g_oCardLayer.getReader(csReaderName);

 		if (oReader.Connect())
		{
			printf("Reader \"%s\":\n", oReader.GetReaderName().c_str());

			tCardType type = oReader.GetCardType();

			switch(type)
			{
			case CARD_PTEID_IAS07:
			case CARD_PTEID_IAS101:
				lRet = DumpPteidContents(oReader);
				break;
			default:
				printf("Unrecognized card type %d, no dump made\n", type);
			}

			oReader.Disconnect();
		}
	}
    catch(CMWException e)
    {
		printf("CMWExeption thrown, error code = 0x%0lx\n", e.GetError());
		return -1;
    }
	catch (...)
	{
		cerr << "Unknown exception thrown !\n";
	}

	return lRet;
}

static void DumpFile(CReader &oReader,
	const std::string & csTitle, const std::string & csPath)
{
	CByteArray oData;
	
	try {
		oData = oReader.ReadFile(csPath);
	}
	catch(CMWException e)
	{
		if (e.GetError() == EIDMW_ERR_FILE_NOT_FOUND)
		{
#ifndef DUMP_FOR_EMULATION
			printf("\n%s (path = %s):\n", csTitle.c_str(), csPath.c_str());
			printf("  ERR/WARNING: file not found!\n", csPath.c_str());
			return;
#endif
		}
		else
			throw e;
	}

#ifdef DUMP_FOR_EMULATION
	printf("\n    // %s\n", csTitle.c_str());
	printf("    unsigned char tuc%s[] = {", csPath.c_str());
	unsigned long ulLen = oData.Size();
	unsigned char *pucData = oData.GetBytes();
	for (unsigned long i = 0; i < ulLen; i++)
	{
		if (i % 16 == 0)
			printf("\n        0x%02x,", pucData[i]);
		else
			printf("0x%02x,", pucData[i]);
	}
	printf("\n    };\n");
	printf("    poEmulationCard->PutTransparentEF(\"%s\",\n        CByteArray(tuc%s, (unsigned long) sizeof(tuc%s)), ",
		csPath.c_str(), csPath.c_str(), csPath.c_str());
	if (csPath == "3F00DF014039")
		printf("AC_ALWAYS, 0, AC_PIN, 1);\n");
	else
		printf("AC_ALWAYS, 0, AC_NEVER, 0);\n");
#else
	std::string csFileSaveMsg;
	std::string csFileName = g_csSerialNr + "_" + csPath;
	FILE *f=NULL;
	int err=0;
	err = fopen_s(&f,csFileName.c_str(), "wb");
	if (f == NULL || err != 0)
		csFileSaveMsg = "couldn't save to file " + csFileName;
	else
	{
		fwrite(oData.GetBytes(), 1, oData.Size(), f);
		fclose(f);
		csFileSaveMsg = "saved to file " + csFileName;
	}

	printf("\n%s (path = %s):\n", csTitle.c_str(), csPath.c_str());
	printf("  %s  (%d bytes)\n", oData.ToString().c_str(), oData.Size());
	printf("  %s\n", csFileSaveMsg.c_str());
#endif
}

static long DumpPteidContents(CReader &oReader)
{
	printf("Type: PT eID\n");
	printf("Version: %0x\n", oReader.GetInfo().GetByte(21));

	std::string csPath;
	g_csSerialNr = oReader.GetSerialNr();
	printf("Serial nr: %s\n", g_csSerialNr.c_str());

	DumpFile(oReader, "EF(DIR)", csPath = "3F002F00");

	DumpFile(oReader, "EF(ODF)", csPath = "3F00DF005031");
	DumpFile(oReader, "EF(TokenInfo)", csPath = "3F00DF005032");
	DumpFile(oReader, "EF(AODF)", csPath = "3F00DF005034");
	DumpFile(oReader, "EF(PrKDF)", csPath = "3F00DF005035");
	DumpFile(oReader, "EF(CDF)", csPath = "3F00DF005037");
	DumpFile(oReader, "EF(Cert#2) (authentication)", csPath = "3F00DF005038");
	DumpFile(oReader, "EF(Cert#3) (non-repudiation)", csPath = "3F00DF005039");
	DumpFile(oReader, "EF(Cert#4) (CA)", csPath = "3F00DF00503A");
	DumpFile(oReader, "EF(Cert#6) (root)", csPath = "3F00DF00503B");
	DumpFile(oReader, "EF(Cert#8) (RN)", csPath = "3F00DF00503C");

	DumpFile(oReader, "EF(ID#RN)", csPath = "3F00DF014031");
	DumpFile(oReader, "EF(SGN#RN)", csPath = "3F00DF014032");
	DumpFile(oReader, "EF(ID#Address)", csPath = "3F00DF014033");
	DumpFile(oReader, "EF(SGN#Address)", csPath = "3F00DF014034");
	DumpFile(oReader, "EF(ID#Photo)", csPath = "3F00DF014035");
	DumpFile(oReader, "EF(PuK#7 ID) (CA role ID)", csPath = "3F00DF014038");
	DumpFile(oReader, "EF(Preferences)", csPath = "3F00DF014039");

	return 0;
}
