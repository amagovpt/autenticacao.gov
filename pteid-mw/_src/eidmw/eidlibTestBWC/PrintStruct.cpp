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

#include "PrintBasic.h"

#include "PrintStruct.h"

void PrintStatus(FILE *f, const char *fctname, const PTEID_Status *status)
{
	PrintTitle(f,"Status");
	fprintf(f,"%s => Status.general=%ld\n",fctname,status->general);
	fprintf(f,"%*.*s Status.pcsc   =0x%08x\n",strlen(fctname)+3,strlen(fctname)+3,"",status->pcsc);
	fprintf(f,"%*.*s Status.system =%ld\n",strlen(fctname)+3,strlen(fctname)+3,"",status->system);
	fprintf(f,"%*.*s Status.cardSW =0x%02x%02x\n",strlen(fctname)+3,strlen(fctname)+3,"",status->cardSW[0],status->cardSW[1]);
	fprintf(f,"\n");
}

void PrintBytes(FILE *f, const char *text, const PTEID_Bytes *bytes)
{
	PrintBlock(f,text);
	PrintHex(f,text,bytes->data,bytes->length);
	fprintf(f,"LENGTH = %ld\n\n", bytes->length);
}

void PrintBytesLenght(FILE *f, const char *text, const BYTE *bytes, unsigned long length)
{
	PrintBlock(f,text);
	PrintHex(f,text,bytes,length);
	fprintf(f,"LENGTH = %ld\n\n", length);
}


void PrintPins(FILE *f, const PTEID_Pins *pins)
{
	long i = 0;

	PrintTitle(f,"PINS");

	fprintf(f,"PINS LENGTH     : %ld\n\n", pins->pinsLength);

	for(i=0;i<pins->pinsLength;i++)
	{
		PrintSeperator(f);
		fprintf(f,"PIN NUMBER      : %ld\n\n", i);
		fprintf(f,"ID              : %d\n", pins->pins[i].id);
		fprintf(f,"LABEL           : %s\n", pins->pins[i].label);
		fprintf(f,"FLAGS           : %ld\n", pins->pins[i].flags);
		fprintf(f,"PIN TYPE        : %ld\n", pins->pins[i].pinType);
		fprintf(f,"USAGE CODE      : %ld\n", pins->pins[i].usageCode);
		fprintf(f,"TRIES LEFT      : %ld\n", pins->pins[i].triesLeft);
	}
}

void PrintPin(FILE *f, const PTEID_Pin *pin,long lTriesLeft)
{
	PrintTitle(f,"PIN");

	fprintf(f,"ID              : %d\n", pin->id);
	fprintf(f,"TYPE            : %ld\n", pin->pinType);
	fprintf(f,"USAGE CODE      : %ld\n", pin->usageCode);
	fprintf(f,"SHORT USAGE     : %s\n", pin->shortUsage);
	fprintf(f,"LONG USAGE      : %s\n", pin->longUsage);
	fprintf(f,"TRIES LEFT      : %ld\n", lTriesLeft);
}

void PrintCertificates(FILE *f, const PTEID_Certif_Check *tCheck)
{
	long i = 0;

	PrintTitle(f,"CERTIFICATES");
	fprintf(f,"CERTS LENGHT     : %ld\n", tCheck->certificatesLength);
	fprintf(f,"SIGNATURE CHECK  : %ld\n", tCheck->signatureCheck);
	fprintf(f,"USED POLICY      : %ld\n", tCheck->usedPolicy);

	for(i=0;i<tCheck->certificatesLength;i++)
	{
		PrintSeperator(f);
		fprintf(f,"CERTIF NUMBER   : %ld\n\n", i);
		fprintf(f,"LABEL           : %s\n", tCheck->certificates[i].certifLabel);
		fprintf(f,"STATUS          : %ld\n", tCheck->certificates[i].certifStatus);
		PrintBytesLenght(f,"CERTIF",tCheck->certificates[i].certif,tCheck->certificates[i].certifLength);
	}
}

void PrintId(FILE *f, const PTEID_ID_Data *idData)
{
	PrintTitle(f,"ID_Data");
	fprintf(f,"CARD NUMBER:             : %s\n", idData->cardNumber);
	fprintf(f,"CHIP NUMBER:             : %s\n", idData->chipNumber);
	fprintf(f,"VALIDITY                 : %s - %s\n", idData->validityDateBegin, idData->validityDateEnd);
	fprintf(f,"MUNICIALITY              : %s\n", idData->municipality);
	fprintf(f,"NATIONAL NUMBER          : %s\n", idData->nationalNumber);
	fprintf(f,"LASTNAME                 : %s\n", idData->name);
	fprintf(f,"FISTNAMES                : %s %s %s\n", idData->firstName1, idData->firstName2, idData->firstName3);
	fprintf(f,"NATIONALITY              : %s\n", idData->nationality);
	fprintf(f,"BIRTHPLACE               : %s\n", idData->birthLocation);
	fprintf(f,"BIRTHDATE                : %s\n", idData->birthDate);
	fprintf(f,"GENDER                   : %s\n", idData->sex);
	fprintf(f,"EXTENDED MINORITY        : %d\n", idData->extendedMinority);
	fprintf(f,"NOBLE CONDITION          : %s\n", idData->nobleCondition);
	fprintf(f,"WHITE CANE               : %d\n", idData->whiteCane);
	fprintf(f,"YELLOW CANE              : %d\n", idData->yellowCane);
	fprintf(f,"VERSION                  : %d\n", idData->version);
	PrintBytesLenght(f,"PHOTO HASH",idData->hashPhoto,20);
}

void PrintAddress(FILE *f, const PTEID_Address *adData)
{
	PrintTitle(f,"Address");
    fprintf(f,"STREET                   : %s %s %s\n", adData->street, adData->streetNumber, adData->boxNumber);
	fprintf(f,"ZIP                      : %s\n", adData->zip);
	fprintf(f,"MUNICIPALITY             : %s\n", adData->municipality);
	fprintf(f,"COUNTRY                  : %s\n", adData->country);
	fprintf(f,"VERSION                  : %d\n", adData->version);
}

void PrintVersionInfo(FILE *f, const PTEID_VersionInfo *vdData)
{
	PrintTitle(f,"VersionInfo");
    fprintf(f,"APPLET INTERFACE VERSION : %02x\n", vdData->AppletInterfaceVersion);
    fprintf(f,"APPLET VERSION           : %02x\n", vdData->AppletVersion);
    fprintf(f,"APPLICATION LIFE CYCLE   : %02x\n", vdData->ApplicationLifeCycle);
    fprintf(f,"COMPONENT CODE           : %02x\n", vdData->ComponentCode);
    fprintf(f,"ELEC PERSO               : %02x\n", vdData->ElecPerso);
    fprintf(f,"ELEC PERSO INTERFACE     : %02x\n", vdData->ElecPersoInterface);
    fprintf(f,"GLOBAL OS VERSION        : %02x\n", vdData->GlobalOSVersion);
    fprintf(f,"GRAPH PERSO              : %02x\n", vdData->GraphPerso);
    fprintf(f,"KEY EXCHANGE VERSION     : %02x\n", vdData->KeyExchangeVersion);
    fprintf(f,"OS NUMBER                : %02x\n", vdData->OSNumber);
    fprintf(f,"OS VERSION               : %02x\n", vdData->OSVersion);
    fprintf(f,"PKCS SUPPORT             : %02x\n", vdData->PKCS1Support);
    fprintf(f,"RESERVED                 : %02x\n", vdData->Reserved);
    fprintf(f,"SOFTMASK NUMBER          : %02x\n", vdData->SoftmaskNumber);
    fprintf(f,"SOFTMASK VERSION         : %02x\n", vdData->SoftmaskVersion);
    PrintBytesLenght(f,"SERIAL NUMBER", vdData->SerialNumber, 16);
}

void PrintRawData(FILE *f, const PTEID_Raw *rawData)
{
	PrintTitle(f,"RawData");
    PrintBytesLenght(f,"ADDRESS", rawData->addrData, rawData->addrLength);
    PrintBytesLenght(f,"ADDR SIG", rawData->addrSigData, rawData->addrSigLength);
    PrintBytesLenght(f,"CARD DATA", rawData->cardData, rawData->cardDataLength);
    PrintBytesLenght(f,"CERT RN", rawData->certRN, rawData->certRNLength);
    PrintBytesLenght(f,"ID DATA", rawData->idData, rawData->idLength);
    PrintBytesLenght(f,"ID SIG", rawData->idSigData, rawData->idSigLength);
    PrintBytesLenght(f,"PICTURE", rawData->pictureData, rawData->pictureLength);
    PrintBytesLenght(f,"TOKEN INFO", rawData->tokenInfo, rawData->tokenInfoLength);
	PrintComment(f,"CHALLENGE COULD NOT BE COMPARED");
    //PrintBytesLenght(f,"CHALLENGE", rawData->challenge, rawData->challengeLength);
	PrintComment(f,"RESPONSE COULD NOT BE COMPARED");
    //PrintBytesLenght(f,"RESPONSE", rawData->response, rawData->responseLength);
}