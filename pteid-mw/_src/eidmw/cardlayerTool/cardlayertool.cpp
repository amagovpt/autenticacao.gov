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
#include <vector>
#include <string>
#include <stdio.h>

using namespace eIDMW;

typedef enum {
	CMD_NONE,
	CMD_HELP,
	CMD_LIST,
	CMD_WAIT,
	CMD_INFO,
	CMD_GET_FILE,
	CMD_UPDATE_FILE,
	CMD_PKCS15,
	CMD_DUMP,
	CMD_PIN_CMD,
	CMD_HASH,
	CMD_SIGN,
	CMD_APDU,
	CMD_TEST,
} tAction;

static long ParseCommandLine(int argc, char **argv);
static long PrintUsage(const char *csMesg);

// Help functions
static CByteArray GetData(const std::string & csData);
static tHashAlgo str2hashalgo(const char *csHashAlgo);
static unsigned long str2signalgo(const char *csSignAlgo);
static void WriteFile(const std::string & csSerial,
	const std::string & csPath, const CByteArray &oData);
static const char * Status2String(tCardStatus status);
static const char *PinOp2String(tPinOperation pinOp);
static const char * PinUsage2String(PinUsage usage);
static const char *cardtype2string(tCardType cardType);
static CByteArray HashAndSign(CReader & oReader, const tPrivKey & key,
	unsigned long signAlgo, const CByteArray & oData);
static long VerifyPIN(CReader &oReader, unsigned long ulAuthID);

// Commands
static long ListWaitReaders(tAction action);
static long CardInfo();
static long GetFile(const std::vector <std::string> & vcsPaths);
static long UpdateFile(const std::vector <std::string> & vcsPaths,
	const CByteArray & oData, unsigned long ulOffset);
static long DumpPKCS15();
static long Hash(tHashAlgo hashAlgo, const CByteArray & oData);
static long Sign(unsigned long signAlgo, unsigned long ulKeyIdx,
	const CByteArray & oData);
static long SendAPDUs(std::vector <std::string> & vcsAPDUs);
static long PinCmd(tPinOperation pinOperation, unsigned long ulPinIdx,
	const std::string & csPin, const std::string & csNewPin);

// Globals
CCardLayer g_oCardLayer;
static tAction g_action = CMD_NONE;
static bool g_bDoVerify = false;
static std::string g_csReaderName = "";
static std::vector <std::string> g_vcsPaths;
static unsigned long g_ulOffset = 0;
static tPinOperation g_pinOperation;
static unsigned long g_ulPinIdx = -1;
static std::string g_csPin = "";
static std::string g_csNewPin = "";
static bool g_bPinProvided = false;
static tHashAlgo g_hashAlgo;
static unsigned long g_signAlgo;
static unsigned long g_ulKeyIdx;
static CByteArray g_oData;
static std::vector <std::string> g_vcsAPDUs;

// See dumpcontents.cpp
long DumpContents(const std::string & csReaderName);

// See testcardlayer.cpp
long Test(const std::string & csReaderName, const std::string & csPin);

int main(int argc, char **argv) 
{
	int lRet = ParseCommandLine(argc, argv);
	if (lRet != 0)
		return lRet;

	if (g_action == CMD_NONE && g_bDoVerify)
		g_action = CMD_PIN_CMD;

	if (g_action != CMD_LIST && g_action != CMD_WAIT && g_csReaderName == "")
	{
		// Get the first reader in which a card is inserted
		CReadersInfo oReadersInfo = g_oCardLayer.ListReaders();
		for (unsigned long i = 0; i < oReadersInfo.ReaderCount(); i++)
		{
			if (oReadersInfo.CardPresent(i))
			{
				g_csReaderName = oReadersInfo.ReaderName(i);
				break;
			}
		}
	}

	try
	{
		switch (g_action)
		{
		case CMD_NONE:
			PrintUsage("No command specified, exiting..");
			break;
		case CMD_HELP:
			lRet = PrintUsage(NULL);
			break;
		case CMD_LIST:
		case CMD_WAIT:
			lRet = ListWaitReaders(g_action);
			break;
		case CMD_INFO:
			lRet = CardInfo();
			break;
		case CMD_GET_FILE:
			lRet = GetFile(g_vcsPaths);
			break;
		case CMD_UPDATE_FILE:
			lRet = UpdateFile(g_vcsPaths, g_oData, g_ulOffset);
			break;
		case CMD_PKCS15:
#ifdef VERBOSE
		  printf("will dump pkcs15 info\n");
#endif
			lRet = DumpPKCS15();
			break;
		case CMD_DUMP:
			lRet = DumpContents(g_csReaderName);
			break;
		case CMD_HASH:
			lRet = Hash(g_hashAlgo, g_oData);
			break;
		case CMD_SIGN:
			lRet = Sign(g_signAlgo, g_ulKeyIdx, g_oData);
			break;
		case CMD_APDU:
			lRet = SendAPDUs(g_vcsAPDUs);
			break;
		case CMD_TEST:
			lRet = Test(g_csReaderName, g_csPin);
			break;
		case CMD_PIN_CMD: // This should be the last action
			lRet = PinCmd(g_pinOperation, g_ulPinIdx, g_csPin, g_csNewPin);
			break;
		default:
			PrintUsage("Unknown command, exiting..");
		}
	}
	catch(CMWException &e)
	{
		lRet = e.GetError();
		printf("ERROR: MiddlewareException thrown: 0x%0x\n", e.GetError());
		printf("  in %s: %d\n", e.GetFile().c_str(), e.GetLine());
	}
	catch (std::exception &e)
	{
		lRet = -1;
		printf("ERROR: std::exception thrown: %s\n", e.what());
	}
	catch (...)
	{
		lRet = -1;
		printf("ERROR: unkown exception thrown\n");
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

	for (int iIndex = 1; iIndex < argc; iIndex++)
	{
		char *arg = argv[iIndex];

		if (!strcmp(arg, "--help") || !strcmp(arg, "-h"))
			g_action = CMD_HELP;
		else if (!strcmp(arg, "--list") || !strcmp(arg, "-l"))
			g_action = CMD_LIST;
		else if (!strcmp(arg, "--wait") || !strcmp(arg, "-w"))
			g_action = CMD_WAIT;
		else if (!strcmp(arg, "--info") || !strcmp(arg, "-i"))
			g_action = CMD_INFO;
		else if (!strcmp(arg, "--reader") || !strcmp(arg, "-r"))
		{
			INCREMENT_TEST_INDEX("--reader", "1");
			g_csReaderName = argv[iIndex];
		}
		else if (!strcmp(arg, "--get") || !strcmp(arg, "-g"))
		{
			g_action = CMD_GET_FILE;
			while (iIndex + 1 < argc && argv[iIndex + 1][0] != '-')
				g_vcsPaths.push_back(std::string(argv[++iIndex]));
			if (g_vcsPaths.size() == 0)
			{
				printf("ERR: no path(s) specified with \"--get\"\n");
				return -1;
			}
		}
		else if (!strcmp(arg, "--update") || !strcmp(arg, "-u"))
		{
			g_action = CMD_UPDATE_FILE;
			INCREMENT_TEST_INDEX("--update", "3");
			g_vcsPaths.push_back(std::string(argv[iIndex]));
			INCREMENT_TEST_INDEX("--update", "3");
			g_oData = GetData(argv[iIndex]);
			INCREMENT_TEST_INDEX("--update", "3");
			g_ulOffset = (unsigned long) atoi(argv[iIndex]);
		}
		else if (!strcmp(arg, "--pkcs15") || !strcmp(arg, "-p"))
			g_action = CMD_PKCS15;
		else if (!strcmp(arg, "--dump") || !strcmp(arg, "-d"))
			g_action = CMD_DUMP;
		else if (!strcmp(arg, "--verify") || !strcmp(arg, "-v"))
		{
			g_bDoVerify = true;
			g_pinOperation = PIN_OP_VERIFY;
			INCREMENT_TEST_INDEX("--verify", "1 or 2");
			g_ulPinIdx = (unsigned long) atoi(argv[iIndex]);
			if (iIndex + 1 < argc && argv[iIndex + 1][0] != '-')
				g_csPin = argv[++iIndex];
		}
		else if (!strcmp(arg, "--change") || !strcmp(arg, "-c"))
		{
			g_bPinProvided = true;
			if (g_action == CMD_NONE)
				g_action = CMD_PIN_CMD;
			g_pinOperation = PIN_OP_CHANGE;
			INCREMENT_TEST_INDEX("--change", "1 or 3");
			g_ulPinIdx = (unsigned long) atoi(argv[iIndex]);
			if (iIndex + 1 < argc && argv[iIndex + 1][0] != '-')
			{
				g_csPin = argv[++iIndex];
				INCREMENT_TEST_INDEX("--change", "1 or 3");
				g_csNewPin = argv[iIndex];
			}
		}
		else if (!strcmp(arg, "--sign") || !strcmp(arg, "-s"))
		{
			g_action = CMD_SIGN;
			INCREMENT_TEST_INDEX("--sign", "3");
			g_signAlgo = str2signalgo(argv[iIndex]);
			INCREMENT_TEST_INDEX("--sign", "3");
			g_ulKeyIdx = (unsigned long) atoi(argv[iIndex]);
			INCREMENT_TEST_INDEX("--sign", "3");
			g_oData = GetData(argv[iIndex]);
		}
		else if (!strcmp(arg, "--apdu") || !strcmp(arg, "-a"))
		{
			g_action = CMD_APDU;
			while (iIndex + 1 < argc && argv[iIndex + 1][0] != '-')
				g_vcsAPDUs.push_back(std::string(argv[++iIndex]));
			if (g_vcsAPDUs.size() == 0)
			{
				printf("ERR: no APDU(s) specified with \"--apdu\"\n");
				return -1;
			}
		}
		else if (!strcmp(arg, "--hash") || !strcmp(arg, "-h"))
		{
			g_action = CMD_HASH;
			INCREMENT_TEST_INDEX("--hash", "2");
			g_hashAlgo = str2hashalgo(argv[iIndex]);
			INCREMENT_TEST_INDEX("--hash", "2");
			g_oData = GetData(argv[iIndex]);
		}
		else if (!strcmp(arg, "--test") || !strcmp(arg, "-t"))
		{
			g_action = CMD_TEST;
			if (iIndex + 1 < argc && argv[iIndex + 1][0] != '-')
				g_csPin = argv[++iIndex];
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
	printf("  --list or -l : list readers\n");
	printf("  --wait or -w : list readers + wait for events\n");
	printf("  --info or -i : show the card status and ATR\n");
	printf("  --reader <name> or -r <name> : specify a reader (default = 1st reader)\n");
	printf("  --get <path1> {path2 {...}} or -g ... : read (a) file(s)\n");
	printf("  --update <path1> <data> <offset> or -u ... : update file\n");
	printf("  --pkcs15 or -p ... : dump the PKCS15 info (PINs, keys, certs)\n");
	printf("  --dump or -d ... : dump all files\n");
	printf("  --hash <hashalgo> <data> or -h ... : make a signature\n");
	printf("      <hashalgo> can be: MD5, SHA1, MD5_SHA1, SHA256\n");
	printf("  --sign <signalgo> <KeyIndex> <data> or -s ... : make a signature\n");
	printf("      <signalgo> can be: RSA_RAW, RSA_PKCS, MD5_RSA_PKCS, SHA1_RSA_PKCS,\n");
	printf("                         SHA256_RSA_PKCS, SIGN_ALGO_SHA1_PSS\n");
	printf("  --verify <PinIndex> {<PIN>} or -v ... : verify the PIN with index <PinIndex>\n");
	printf("      Note: this command can be used together with --sign\n");
	printf("  --change <PinIndex> {PIN NEWPIN} or -c ... : change the PIN with ..\n");
	printf("  --apdu <APDU1> {APDU2 {...}} or -a : send APDUs (hex encoded)\n");
	printf("  --test {<Pin>} or -t ... : card-dependent PIN tests\n");
	printf("Note: <data> is interpreted as a string unless it starts with file://\n");
	printf("      in which case the remainder is interpreted as a file that will be read\n");

	return 0;
}

//////////////////////////// Help functions ////////////////////////////////

CByteArray GetData(const std::string & csData)
{
	CByteArray oData;

	// If it starts with "file:", we have to read a file
	if (csData.substr(0, 7) == "file://")
	{
		const char *csFileName = csData.c_str() + 7;
#ifdef WIN32
		FILE *f;
		fopen_s(&f,csFileName, "rb");
#else
		FILE *f = fopen(csFileName, "rb");
#endif
		if (f != NULL)
		{
			unsigned char tucBuf[1000];
			size_t len = fread(tucBuf, 1, sizeof(tucBuf), f);
			while (len != 0)
			{
				oData.Append(tucBuf, (unsigned long) len);
				len = fread(tucBuf, 1, sizeof(tucBuf), f);
			}
			printf("Read %d bytes from file \"%s\"\n", oData.Size(), csFileName);
			fclose(f);
		}
		else
			printf("Couldn't open file \"%s\", using empty data\n", csFileName);
	}
	else
	{
		oData = CByteArray(csData.c_str());
		printf("Input: %d bytes\n", oData.Size());
	}

	return oData;
}

static tHashAlgo str2hashalgo(const char *csHashAlgo)
{
	if (!strcmp(csHashAlgo, "MD5"))
		return ALGO_MD5;
	else if (!strcmp(csHashAlgo, "SHA1"))
		return ALGO_SHA1;
	else if (!strcmp(csHashAlgo, "MD5_SHA1"))
		return ALGO_MD5_SHA1;
	else if (!strcmp(csHashAlgo, "SHA256"))
		return ALGO_SHA256;

	printf("Unkonwn hash algorith \"%s\", using SHA1\n", csHashAlgo);
	return ALGO_SHA1;
}

static unsigned long str2signalgo(const char *csSignAlgo)
{
	if (!strcmp(csSignAlgo, "RSA_RAW"))
		return SIGN_ALGO_RSA_RAW;
	else if (!strcmp(csSignAlgo, "RSA_PKCS"))
		return SIGN_ALGO_RSA_PKCS;
	else if (!strcmp(csSignAlgo, "MD5_RSA_PKCS"))
		return SIGN_ALGO_MD5_RSA_PKCS;
	else if (!strcmp(csSignAlgo, "SHA1_RSA_PKCS"))
		return SIGN_ALGO_SHA1_RSA_PKCS;
	else if (!strcmp(csSignAlgo, "SHA256_RSA_PKCS"))
		return SIGN_ALGO_SHA256_RSA_PKCS;
	else if (!strcmp(csSignAlgo, "SIGN_ALGO_SHA1_PSS"))
		return SIGN_ALGO_SHA1_RSA_PSS;

	printf("Unkonwn signature algorith \"%s\", using SHA1_RSA_PKCS\n", csSignAlgo);
	return SIGN_ALGO_SHA1_RSA_PKCS;
}

static void WriteFile(const std::string & csSerial, const std::string & csPath, const CByteArray &oData)
{
	std::string csFileName;
	if (csSerial != "")
		csFileName += csSerial + "_";
	csFileName += csPath;
#ifdef WIN32
	FILE *f;
	fopen_s(&f,csFileName.c_str(), "wb");
#else
	FILE *f = fopen(csFileName.c_str(), "wb");
#endif
	if (f != NULL)
	{
		fwrite(oData.GetBytes(), 1, oData.Size(), f);
		printf(" - Written %d bytes to %s\n", oData.Size(), csFileName.c_str());
		fclose(f);
	}
	else
		printf(" - ERR: couldn't write file %s\n", csFileName.c_str());
}

static const char * Status2String(tCardStatus status)
{
	switch (status)
	{
	case CARD_INSERTED: return "card inserted";
	case CARD_NOT_PRESENT: return "no card present";
	case CARD_STILL_PRESENT: return "card stil present";
	case CARD_REMOVED: return "card removed";
	case CARD_OTHER: return "card removed and (another) card inserted";
	default: return "unknown state?!?";
	}
}

static const char *PinOp2String(tPinOperation pinOp)
{
	switch(pinOp)
	{
	case PIN_OP_VERIFY: return "PIN verification";
	case PIN_OP_CHANGE: return "PIN change";
	default: return "unknown PIN command";
	}
}

static const char * PinUsage2String(PinUsage usage)
{
	switch(usage)
	{
	case DLG_USG_PIN_AUTH: return "authentication";
	case DLG_USG_PIN_SIGN: return "signature";
	case DLG_USG_PIN_ADDRESS: return "address";
	default: return "unknown";
	}
}

static const char *cardtype2string(tCardType cardType)
{
	static char csBuf[100];

	switch(cardType)
	{
	case CARD_BEID: return "PT eID";
	case CARD_SIS: return "SIS";
	case CARD_UNKNOWN: return "UNKNOWN";
	}
#ifdef WIN32
	sprintf_s(csBuf, sizeof(csBuf), "unknown type %d", cardType);
	csBuf[sizeof(csBuf) - 1] = '\0';
#else
	sprintf(csBuf, "unknown type %d\n", cardType);
#endif
	return csBuf;
}

static CByteArray HashAndSign(CReader & oReader, const tPrivKey & key,
	unsigned long signAlgo, const CByteArray & oData)
{
	tHashAlgo hashAlgo;
	if (signAlgo == SIGN_ALGO_MD5_RSA_PKCS)
		hashAlgo = ALGO_MD5;
	else if (signAlgo == SIGN_ALGO_SHA1_RSA_PKCS)
		hashAlgo = ALGO_SHA1;
	else if (signAlgo == SIGN_ALGO_SHA256_RSA_PKCS)
		hashAlgo = ALGO_SHA256;
	else
	{
		printf("Unsupport signature algorithm %d, can't sign\n", signAlgo);
		return CByteArray();
	}

	CHash oHash;
	oHash.Init(hashAlgo);
	oHash.Update(oData);

	return oReader.Sign(key, signAlgo, oHash);
}

static long VerifyPIN(CReader &oReader, unsigned long ulAuthID)
{
	long lRet = 0;

	// If no PIN specified, do nothing
	if (g_csPin != "")
	{
		if (g_ulPinIdx >= oReader.PinCount())
		{
			printf("ERR: PIN index (%d) too big\n", g_ulPinIdx);
			lRet = -4;
		}
		else
		{
			tPin pin = oReader.GetPin(g_ulPinIdx);

			if (ulAuthID != 0)
			{
				tPin pin2 = oReader.GetPinByID(ulAuthID);
				if (memcmp(&pin, &pin2, sizeof(tPin)) != 0)
				{
					printf("ERR, PIN corresponding to AuthID (%0x) differs from the\n", ulAuthID);
					printf("  specified PIN (ID = %0x), exiting\n", pin.ulID);
					lRet = -3;
				}
			}

			if (lRet == 0)
			{
				unsigned long ulRemaining;

				if (!oReader.PinCmd(PIN_OP_VERIFY, pin, g_csPin, "", ulRemaining))
				{
					printf("ERR: wrong PIN specified on the command line, %d remaining tries\n",
						ulRemaining);
					lRet = -2;
				}
				else
					printf("\nPIN verification succeeded\n");
			}
		}
	}

	return lRet;
}

/////////////////////////////// Commands ////////////////////////////////////

static long ListWaitReaders(tAction action)
{
	CReadersInfo oReadersInfo = g_oCardLayer.ListReaders();

	printf("%d reader(s) found\n", oReadersInfo.ReaderCount());
	for (unsigned long i = 0; i < oReadersInfo.ReaderCount(); i++)
	{
		printf(" - %s: %s card present\n", oReadersInfo.ReaderName(i).c_str(),
			oReadersInfo.CardPresent(i) ? "" : "no");
	}

	if (action == CMD_LIST)
		return 0;

	printf("\nInsert/remove card(s), press Ctrl-C to stop\n\n");

	while(true)
	{
		getchar();

		bool bChanged = oReadersInfo.CheckReaderEvents(TIMEOUT_INFINITE, ALL_READERS);

		if (!bChanged)
		{
			printf("Err: CheckReaderEvents() returned unnecessary\n");
			break;
		}

		for (unsigned long i = 0; i < oReadersInfo.ReaderCount(); i++)
		{
			if (oReadersInfo.ReaderStateChanged(i))
			{
				printf(" - %s: card %s\n", oReadersInfo.ReaderName(i).c_str(),
					oReadersInfo.CardPresent(i) ? "inserted" : "removed");
			}
			printf("\n");
		}
	}

	return 0;
}

static long CardInfo()
{
	CReader &oReader = g_oCardLayer.getReader(g_csReaderName);

	if (oReader.Connect())
	{
		printf("Reader \"%s\":\n", oReader.GetReaderName().c_str());
		CByteArray atr = oReader.GetATR();
		printf("  ATR: %s\n", atr.ToString().c_str());
		printf("  Type: %s\n", cardtype2string(oReader.GetCardType()));
		printf("  Serial Nr.: %s\n", oReader.GetSerialNr().c_str());
		printf("  Label: %s\n", oReader.GetCardLabel().c_str());

		printf("\nRemove/insert card and press ENTER or q to quit\n");
		char c = getchar();
		while (('q' != c) && (-1 != c))  // -1 means Ctrl-C
		{
			tCardStatus status = oReader.Status(true);
			printf(" - %s\n", Status2String(status));

			c = getchar();
		}

		oReader.Disconnect();
	}
	else
		printf("  No card found in the reader, exiting\n");

	return 0;
}

static long GetFile(const std::vector <std::string> & vcsPaths)
{
	long lRet = 0;

	CReader &oReader = g_oCardLayer.getReader(g_csReaderName);

	if (oReader.Connect())
	{
		printf("Reader \"%s\":\n", oReader.GetReaderName().c_str());

		// Verify PIN if specified
		lRet = VerifyPIN(oReader, 0);
		if (lRet == 0)
		{
			for (unsigned int i = 0; i < vcsPaths.size(); i++)
			{
				const std::string & csPath = vcsPaths[i].c_str();
				CByteArray oData = oReader.ReadFile(csPath, 0, FULL_FILE);

				if (i == 0)
					printf("\n%s:%s\n", csPath.c_str(), oData.ToString(true, false).c_str());

				WriteFile(oReader.GetSerialNr(), csPath, oData);
			}
		}

		oReader.Disconnect();
	}
	else
		printf("  No card found in the reader, exiting\n");

	return lRet;
}

static long UpdateFile(const std::vector <std::string> & vcsPaths,
	const CByteArray & oData, unsigned long ulOffset)
{
	long lRet = 0;

	CReader &oReader = g_oCardLayer.getReader(g_csReaderName);

	if (oReader.Connect())
	{
		printf("Reader \"%s\":\n", oReader.GetReaderName().c_str());

		// Verify PIN if specified
		lRet = VerifyPIN(oReader, 0);
		if (lRet == 0)
		{
			oReader.WriteFile(vcsPaths[0], ulOffset, oData);
			printf("Written %d bytes to file %s, starting at offset %d\n",
				oData.Size(), vcsPaths[0].c_str(), ulOffset);
		}

		oReader.Disconnect();
	}
	else
		printf("  No card found in the reader, exiting\n");

	return lRet;
}

static long DumpPKCS15()
{
	CReader &oReader = g_oCardLayer.getReader(g_csReaderName);

	if (oReader.Connect())
	{
		printf("\nLabel: %s\n", oReader.GetCardLabel().c_str());
		printf("Serial nr: %s\n", oReader.GetSerialNr().c_str());

		unsigned long ulPins = oReader.PinCount();
		printf("\nFound %d PIN(s)\n", ulPins);
		for (unsigned long i = 0; i < ulPins; i++)
		{
			tPin pin = oReader.GetPin(i);
			printf(" - %d: %s, id=%d, path=%s, pinref=0x%0x\n",
			       i, pin.csLabel.c_str(), pin.ulID, pin.csPath.c_str(), pin.ulPinRef);
			printf("      padchar=%X, encoding=%d, minlen=%d, maxlen=%d\n",
				pin.ucPadChar, pin.encoding, pin.ulMinLen, pin.ulMaxLen);
			printf("      key usage: %s\n", PinUsage2String(oReader.GetPinUsage(pin)));
		}

		unsigned long ulKeys = oReader.PrivKeyCount();
		printf("\nFound %d private key(s)\n", ulKeys);
		for (unsigned long i = 0; i < ulKeys; i++)
		{
			tPrivKey key = oReader.GetPrivKey(i);
			printf(" - %d: %s, id=%d, path=%s, length=%d bytes, AuthID=%d, keyref=0x%0x\n",
				i, key.csLabel.c_str(), key.ulID, key.csPath.c_str(),
				key.ulKeyLenBytes, key.ulAuthID, key.ulKeyRef);
		}

		unsigned long ulCerts = oReader.CertCount();
		printf("\nFound %d certs(s)\n", ulCerts);
		for (unsigned long i = 0; i < ulCerts; i++)
		{
			tCert cert = oReader.GetCert(i);
			printf(" - %d: %s, id=%d, path=%s\n",
				i, cert.csLabel.c_str(), cert.ulID, cert.csPath.c_str());
		}
	}
	else
		printf("  No card found in the reader, exiting\n");

	return 0;
}

static long Hash(tHashAlgo hashAlgo, const CByteArray & oData)
{
	CHash oHash;
	CByteArray oHashData = oHash.Hash(hashAlgo, oData);

	printf("Hash: %s\n", oHashData.ToString(true, false).c_str());
	WriteFile("", "hash.bin", oHashData);

	return 0;
}

static long Sign(unsigned long signAlgo, unsigned long ulKeyIdx,
	const CByteArray & oData)
{
	long lRet = 0;

	CReader &oReader = g_oCardLayer.getReader(g_csReaderName);

	if (oReader.Connect())
	{
		if (ulKeyIdx >= oReader.PrivKeyCount())
		{
			printf("ERR: pin index (%d) should be less then %d\n",
				ulKeyIdx, oReader.PrivKeyCount());
			lRet = -1;
		}
		else
		{
			tPrivKey key = oReader.GetPrivKey(ulKeyIdx);
			printf("Using key \"%s\"\n", key.csLabel.c_str());

			// Verify PIN if specified
			lRet = VerifyPIN(oReader, key.ulAuthID);
			if (lRet == 0)
			{
				CByteArray oSignature;

				if (signAlgo == SIGN_ALGO_RSA_RAW || signAlgo == SIGN_ALGO_RSA_PKCS)
					oSignature = oReader.Sign(key, signAlgo, oData);
				else
				{
					oSignature = HashAndSign(oReader, key, signAlgo, oData);
					oSignature = HashAndSign(oReader, key, signAlgo, oData);
				}
				printf("Signature: %s\n", oSignature.ToString(true, false).c_str());
				WriteFile("", "signature.bin", oSignature);
			}
		}

		oReader.Disconnect();
	}
	else
		printf("  No card found in the reader, exiting\n");

	return lRet;
}

static long SendAPDUs(std::vector <std::string> & vcsAPDUs)
{
	long lRet = 0;

	CReader &oReader = g_oCardLayer.getReader(g_csReaderName);

	if (oReader.Connect())
	{
		for (unsigned int i = 0; i < vcsAPDUs.size(); i++)
		{
			CByteArray oRequestAPDU(vcsAPDUs[i], true);
			printf("\nIN:  %s\n", oRequestAPDU.ToString().c_str());
			CByteArray oResponseAPDU = oReader.SendAPDU(oRequestAPDU);
			printf("OUT: %s\n", oResponseAPDU.Size() < 14 ? oResponseAPDU.ToString().c_str():
				oResponseAPDU.ToString(true, false).c_str());
		}

		oReader.Disconnect();
	}
	else
		printf("  No card found in the reader, exiting\n");

	return lRet;

}

static long PinCmd(tPinOperation pinOperation, unsigned long ulPinIdx,
	const std::string & csPin, const std::string & csNewPin)
{
	long lRet = 0;

	CReader &oReader = g_oCardLayer.getReader(g_csReaderName);

	if (oReader.Connect())
	{
		if (ulPinIdx >= oReader.PinCount())
		{
			printf("ERR: pin index (%d) should be less then %d\n",
				ulPinIdx, oReader.PinCount());
			lRet = -1;
		}
		else
		{
			tPin pin = oReader.GetPin(ulPinIdx);
			printf("Trying a %s with PIN \"%s\"\n",
				PinOp2String(pinOperation), pin.csLabel.c_str());

			unsigned long ulRemaining;
			bool bRet = oReader.PinCmd(pinOperation, pin,
				csPin, csNewPin, ulRemaining);
			if (bRet)
				printf("  PIN command succeeded\n");
			else
			{
				printf("  Pin command failed, %d remaining attempts\n", ulRemaining);
				lRet = -2;
			}
		}

		oReader.Disconnect();
	}
	else
		printf("  No card found in the reader, exiting\n");

	return lRet;
}
