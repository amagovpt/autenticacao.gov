/*-****************************************************************************

 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

/*
 * SODParser.cpp
 *
 *  Created on: Feb 14, 2012
 *      Author: ruim
 */

#include "SODParser.h"
#include "eidErrors.h"
#include "MWException.h"
#include "../pkcs11/asn1.h"
#include <algorithm>
extern "C" {
#include "../pkcs11/asn1.c"
}

namespace eIDMW {

/* convert variable length binary bit-stream into int-type */
static unsigned int bin2int(const unsigned char *p_ucDat, unsigned int iLen)
{
    unsigned int uiResult = 0;

    //parameter check
    if (iLen > 4)
        throw CMWEXCEPTION(EIDMW_WRONG_ASN1_FORMAT);

    //add all bytes
    while(iLen--)
    {
        uiResult = uiResult<<8 | *(p_ucDat++);
    }
    return uiResult;
}


SODParser::~SODParser(){
	if (attr)
		delete attr;
}

void SODParser::ParseSodEncapsulatedContent(const CByteArray & contents, const std::vector<int>& valid_tags){
	ASN1_ITEM           xLev0Item;
	ASN1_ITEM           xLev1Item;
	ASN1_ITEM           xLev2Item;
	ASN1_ITEM           xLev3Item;
	ASN1_ITEM           xLev4Item;

	xLev0Item.p_data = (unsigned char*)contents.GetBytes();
	xLev0Item.l_data = contents.Size();

	if ((asn1_next_item(&xLev0Item, &xLev1Item)!= 0) ||(xLev1Item.tag != ASN_SEQUENCE))
		throw CMWEXCEPTION(EIDMW_SOD_UNEXPECTED_ASN1_TAG);

	if ((xLev1Item.l_data < 2)|| (asn1_next_item(&xLev1Item, &xLev2Item)!= 0) ||(xLev2Item.tag != ASN_INTEGER))
		throw CMWEXCEPTION(EIDMW_SOD_UNEXPECTED_ASN1_TAG);

	if (bin2int(xLev2Item.p_data, xLev2Item.l_data)!=0)
		throw CMWEXCEPTION(EIDMW_SOD_UNEXPECTED_VALUE);

	if ((xLev1Item.l_data < 2)|| (asn1_next_item(&xLev1Item, &xLev2Item)!= 0) ||(xLev2Item.tag != ASN_SEQUENCE))
		throw CMWEXCEPTION(EIDMW_SOD_UNEXPECTED_ASN1_TAG);

	if ((xLev2Item.l_data < 2)|| (asn1_next_item(&xLev2Item, &xLev3Item)!= 0) ||(xLev3Item.tag != ASN_OID))
		throw CMWEXCEPTION(EIDMW_SOD_UNEXPECTED_ASN1_TAG);

	// not the intended way, but for now it will do just fine.
	if (memcmp(xLev3Item.p_data, OID_SHA256_ALGORITHM, xLev3Item.l_data) != 0)
		throw CMWEXCEPTION(EIDMW_SOD_UNEXPECTED_ALGO_OID);

	// xlevel2 data is pointing to the optional 05 00 entry
	// TODO (DEV-CC2): to test with final cards
	if(xLev2Item.l_data >= 2 && (asn1_next_item(&xLev2Item, &xLev3Item)!= 0 || (xLev3Item.tag != ASN_NULL)))
			throw CMWEXCEPTION(EIDMW_SOD_UNEXPECTED_ASN1_TAG);

	if ((xLev1Item.l_data < 2)|| (asn1_next_item(&xLev1Item, &xLev2Item)!= 0) ||(xLev2Item.tag != ASN_SEQUENCE))
		throw CMWEXCEPTION(EIDMW_SOD_UNEXPECTED_ASN1_TAG);


	attr = new SODAttributes();
	int i=0;
	while(xLev2Item.l_data > 0){

		if ((xLev2Item.l_data < 2)|| (asn1_next_item(&xLev2Item, &xLev3Item)!= 0) ||(xLev3Item.tag != ASN_SEQUENCE))
			throw CMWEXCEPTION(EIDMW_SOD_UNEXPECTED_ASN1_TAG);

		if ((xLev3Item.l_data < 2)|| (asn1_next_item(&xLev3Item, &xLev4Item)!= 0) ||(xLev4Item.tag != ASN_INTEGER))
			throw CMWEXCEPTION(EIDMW_SOD_UNEXPECTED_ASN1_TAG);

		// validate the found tag
        const auto read_tag = bin2int(xLev4Item.p_data, xLev4Item.l_data);
		if (std::find(valid_tags.begin(), valid_tags.end(), read_tag) == valid_tags.end())
			throw CMWEXCEPTION(EIDMW_SOD_UNEXPECTED_VALUE);

		if ((xLev3Item.l_data < 2)|| (asn1_next_item(&xLev3Item, &xLev4Item)!= 0) ||(xLev4Item.tag != ASN_OCTET_STRING))
			throw CMWEXCEPTION(EIDMW_SOD_UNEXPECTED_ASN1_TAG);

		attr->hashes[i].Append(xLev4Item.p_data,xLev4Item.l_data);
		i++;
	}
}

SODAttributes &SODParser::getHashes(){
	return *attr;
}


} /* namespace eIDMW */
