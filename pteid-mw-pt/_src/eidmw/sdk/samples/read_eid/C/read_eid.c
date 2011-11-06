#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eidlibC.h"

int main(int argc, char* argv[])
{
    PTEID_Status tStatus = {0};
    PTEID_ID_Data idData = {0};
    PTEID_Address adData = {0};
    PTEID_Certif_Check tCheck = {0};
	PTEID_Bytes Picture = {0};
	FILE* pPictureFile = NULL;

    long lHandle = 0;
    tStatus = PTEID_Init("", 0, 0, &lHandle);
	if(PTEID_OK != tStatus.general)
	{
		tStatus = PTEID_Exit();
		printf("Initialisation failed\n");
		printf("Exiting...\n");
		return -1;
	}

    // Read ID Data
    tStatus = PTEID_GetID(&idData, &tCheck);
	printf(  "\n################################### IDENTITY ###################################\n");
	if(PTEID_OK == tStatus.general)
    {
		printf("CARD NUMBER:             : %s\n", idData.cardNumber);
		printf("CHIP NUMBER:             : %s\n", idData.chipNumber);
		printf("VALIDITY                 : %s - %s\n", idData.validityDateBegin, idData.validityDateEnd);
		printf("MUNICIALITY              : %s\n", idData.municipality);
		printf("NATIONAL NUMBER          : %s\n", idData.nationalNumber);
		printf("LASTNAME                 : %s\n", idData.name);
		printf("FISTNAMES                : %s %s %s\n", idData.firstName1, idData.firstName2, idData.firstName3);
		printf("NATIONALITY              : %s\n", idData.nationality);
		printf("BIRTHPLACE               : %s\n", idData.birthLocation);
		printf("BIRTHDATE                : %s\n", idData.birthDate);
		printf("GENDER                   : %s\n", idData.sex);
    }
	else
	{
		tStatus = PTEID_Exit();
		printf("Retrieveing identity data failed\n");
		printf("Exiting...\n");
		return -1;
	}

    memset(&tCheck, 0, sizeof(PTEID_Certif_Check));

    // Read Address Data
    tStatus = PTEID_GetAddress(&adData, &tCheck);
	printf(  "\n################################### ADDRESS ####################################\n");
	if(PTEID_OK == tStatus.general)
    {
        printf("STREET:                  : %s %s %s\n", adData.street, adData.streetNumber, adData.boxNumber);
		printf("ZIP:                     : %s\n", adData.zip);
		printf("MUNICIPALITY:            : %s\n", adData.municipality);
		printf("COUNTRY:                 : %s\n", adData.country);
    }
	else
	{
		tStatus = PTEID_Exit();
		printf("Retrieveing address data failed\n");
		printf("Exiting...\n");
		return -1;
	}

	// Read Picture
	Picture.data = (BYTE*)malloc(10*sizeof(BYTE));
	Picture.length = 10;
	tStatus = PTEID_GetPicture(&Picture, &tCheck);

	if (tStatus.general == PTEID_E_INSUFFICIENT_BUFFER)
	{
		free(Picture.data);
		Picture.data = (BYTE*)malloc(Picture.neededlength);
		Picture.length = Picture.neededlength;
	}

	tStatus = PTEID_GetPicture(&Picture, &tCheck);

	printf(  "\n################################### PICTURE ####################################\n");
	if(PTEID_OK == tStatus.general)
	{
		char fileName[256];
		strcpy(fileName,idData.nationalNumber);
		strcat(fileName,".jpg");
		pPictureFile = fopen(fileName,"w+b");
		fwrite(Picture.data,sizeof(BYTE),Picture.length,pPictureFile);
		fclose(pPictureFile);
		pPictureFile = NULL;
		printf("PICTURE FILE             : %s\n",fileName);
	}
	else
	{
		tStatus = PTEID_Exit();
		printf("Retrieveing picture failed\n");
		printf("Exiting...\n");
		return -1;
	}

    tStatus = PTEID_Exit();

	return 0;
}

