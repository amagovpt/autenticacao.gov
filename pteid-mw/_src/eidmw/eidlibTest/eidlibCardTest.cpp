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
#include "UnitTest++/src/UnitTest++.h"
#include "../eidlib/eidlib.h"

using namespace eIDMW;

TEST(GetCard)
{
	PTEID_ReaderContext &reader0=ReaderSet.getReaderByNum(0);	
	PTEID_EIDCard &card0=reader0.getEIDCard();				
	CHECK(&card0);

	PTEID_ReaderContext &reader2=ReaderSet.getReaderByNum(2);	
	PTEID_EIDCard &card2=reader2.getEIDCard();				
	CHECK(&card2);

	PTEID_ReaderContext &reader3=ReaderSet.getReaderByNum(3);	
	PTEID_EIDCard &card3=reader3.getEIDCard();				
	CHECK(&card3);

	PTEID_ReaderContext &reader=ReaderSet.getReaderByNum(4);	
	PTEID_SISCard &card4=reader.getSISCard();				
	CHECK(&card4);
}

TEST(TestCard)
{
	PTEID_ReaderContext &reader0=ReaderSet.getReaderByNum(0);	
	PTEID_EIDCard &card0=reader0.getEIDCard();				
	CHECK_EQUAL(true,card0.isTestCard());
	card0.setAllowTestCard(false);
	CHECK_EQUAL(false,card0.getAllowTestCard());
	card0.setAllowTestCard(true);
	CHECK_EQUAL(true,card0.getAllowTestCard());

	PTEID_ReaderContext &reader2=ReaderSet.getReaderByNum(2);	
	PTEID_EIDCard &card2=reader2.getEIDCard();				
	CHECK_EQUAL(false,card2.isTestCard());
	card2.setAllowTestCard(false);
	CHECK_EQUAL(false,card2.getAllowTestCard());
	card2.setAllowTestCard(true);
	CHECK_EQUAL(true,card2.getAllowTestCard());

	PTEID_ReaderContext &reader3=ReaderSet.getReaderByNum(3);	
	PTEID_EIDCard &card3=reader3.getEIDCard();				
	CHECK_EQUAL(true,card3.isTestCard());
	card3.setAllowTestCard(false);
	CHECK_EQUAL(false,card3.getAllowTestCard());
	card3.setAllowTestCard(true);
	CHECK_EQUAL(true,card3.getAllowTestCard());
}

TEST(ChallengeResponse)
{
	PTEID_ReaderContext &reader2=ReaderSet.getReaderByNum(2);	
	PTEID_EIDCard &card2=reader2.getEIDCard();				

	PTEID_ByteArray challenge1=card2.getChallenge();
	PTEID_ByteArray response1=card2.getChallengeResponse();

	CHECK_EQUAL(true,card2.verifyChallengeResponse(challenge1,response1));

	PTEID_ByteArray challenge2=card2.getChallenge();
	PTEID_ByteArray response2=card2.getChallengeResponse();

	CHECK_EQUAL(true,challenge1.Equals(challenge2));
	CHECK_EQUAL(true,response1.Equals(response2));

	PTEID_ByteArray challenge3=card2.getChallenge(true);
	PTEID_ByteArray response3=card2.getChallengeResponse();

	CHECK_EQUAL(false,challenge1.Equals(challenge3));
	CHECK_EQUAL(false,response1.Equals(response3));

	CHECK_EQUAL(true,card2.verifyChallengeResponse(challenge3,response3));

	PTEID_ByteArray response4(response3);
	response4.Append((unsigned char *)"1",1);

	CHECK_EQUAL(false,card2.verifyChallengeResponse(challenge3,response4));

}
