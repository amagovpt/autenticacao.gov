/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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
#include "globmdrv.h"

#ifndef __PTEIDMDRV__H__
#define __PTEIDMDRV__H__

/*
 * ID card properties
 */
#define CP_ID_FILE_STRUCTURE_VERSION          L"File_Structure_Version"
#define CP_ID_CARD_NUMBER                     L"Card_Number"
#define CP_ID_CHIP_NUMBER                     L"Chip_Number"
#define CP_ID_CARD_VALIDITY_BEGIN             L"Card_Validity_Begin"
#define CP_ID_CARD_VALIDITY_END               L"Card_Validity_End"
#define CP_ID_CARD_DELIVIRY_MUNICIPALITY      L"Card_Delivery_Municipality"
#define CP_ID_NATIONAL_NUMBER                 L"National_Number"
#define CP_ID_NAME                            L"Name"
#define CP_ID_FIRST_TWO_GIVEN_NAMES           L"First_Two_Given_Names"
#define CP_ID_FIRST_LETTER_THIRD_GIVEN_NAME   L"First_Letter_Third_Given_Name"
#define CP_ID_NATIONALITY                     L"Nationality"
#define CP_ID_BIRTH_LOCATION                  L"Birth_Location"
#define CP_ID_BIRTH_DATE                      L"Birth_Date"
#define CP_ID_GENDER                          L"Gender"
#define CP_ID_NOBILITY                        L"Nobility"
#define CP_ID_DOCUMENT_TYPE                   L"DocumentvType"
#define CP_ID_SPECIAL_STATUS                  L"Special_Status"
#define CP_ID_PHOTO_HASH                      L"Photo_Hash"

#define CP_ID_ADDRESS_STREET_AND_NUMBER       L"Address_StreetvNumber"
#define CP_ID_ADDRESS_ZIP                     L"Address_Zip"
#define CP_ID_ADDRESS_MUNICIPALITY            L"Address_Municipality"

DWORD WINAPI   CardGetContainerProperty
               (
                  __in                                         PCARD_DATA     pCardData,
                  __in                                         BYTE           bContainerIndex,
                  __in                                         LPCWSTR        wszProperty,
                  __out_bcount_part_opt(cbData, *pdwDataLen)   PBYTE          pbData,
                  __in                                         DWORD          cbData,
                  __out                                        PDWORD         pdwDataLen,
                  __in                                         DWORD          dwFlags
               );
DWORD WINAPI   CardSetContainerProperty
               (
                  __in                    PCARD_DATA  pCardData,
                  __in                    BYTE        bContainerIndex,
                  __in                    LPCWSTR     wszProperty,
                  __in_bcount(cbDataLen)  PBYTE       pbData,
                  __in                    DWORD       cbDataLen,
                  __in                    DWORD       dwFlags
               );
DWORD CardGetFreeSpace(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetFreeSpace(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetCapabilities(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetCapabilities(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetKeysizes(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetKeysizes(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetReadOnly(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetReadOnly(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetCacheMode(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetCacheMode(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetSupportsWinX509Enrollment(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetSupportsWinX509Enrollment(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetGuid(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetGuid(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetSerialNo(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetSerialNo(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetPinInfo(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetPinInfo(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetListPins(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetListPins(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetAuthenticatedState(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetAuthenticatedState(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetPinStrengthVerify(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetPinStrengthVerify(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetPinStrengthChange(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetPinStrengthChange(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetPinStrengthUnblock(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetPinStrengthUnblock(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetParentWindow(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetParentWindow(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetPinContextString(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetPinContextString(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);


DWORD CardSetPropertyUnsupported(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetCardNumber(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);

DWORD CardGetChipNumber (PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetValidityBegin(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetValidityEnd(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetDeliveryMunicipality(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetNationalNumber(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetName(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetFirstTwoGivenNames(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetFirstLetterThirdGivenName(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetNationality(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetBirthLocation(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetBirthDate(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetGender(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetNobility(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetDocumentType(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetSpecialStatus(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetPhotoHash(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetAddressStreetAndNumber(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetAddressZip(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardGetAddressMunicipality(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD WINAPI   CardGetProperty
(
   __in                                         PCARD_DATA  pCardData,
   __in                                         LPCWSTR     wszProperty,
   __out_bcount_part_opt(cbData, *pdwDataLen)   PBYTE       pbData,
   __in                                         DWORD       cbData,
   __out                                        PDWORD      pdwDataLen,
   __in                                         DWORD       dwFlags
);
DWORD WINAPI   CardSetProperty
               (
                  __in                    PCARD_DATA     pCardData,
                  __in                    LPCWSTR        wszProperty,
                  __in_bcount(cbDataLen)  PBYTE          pbData,
                  __in                    DWORD          cbDataLen,
                  __in                    DWORD          dwFlags
               );
#endif
