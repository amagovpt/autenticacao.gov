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
#include "APLCardSIS.h"
#include "CardSIS.h"
#include "CardSISDef.h"
#include "Util.h"

namespace eIDMW
{
/*****************************************************************************************
---------------------------------------- APL_SisFile_ID -----------------------------------------
*****************************************************************************************/
APL_SisFile_ID::APL_SisFile_ID(APL_Card *card):APL_CardFile(card,SIS_FILE_ID)
{
}

APL_SisFile_ID::~APL_SisFile_ID()
{
}

tCardFileStatus APL_SisFile_ID::VerifyFile()
{
	if(m_data.Size()!=SIS_FILE_ID_SIZE)
		return CARDFILESTATUS_ERROR_FORMAT;

	return CARDFILESTATUS_OK;
}
void APL_SisFile_ID::EmptyFields()
{
	m_Name.clear();
	m_Surname.clear();
	m_Initials.clear();
	m_Gender.clear();
	m_DateOfBirth.clear();
	m_SocialSecurityNumber.clear();
	m_LogicalNumber.clear();
	m_DateOfIssue.clear();
	m_ValidityBeginDate.clear();
	m_ValidityEndDate.clear();
}

bool APL_SisFile_ID::MapFields()
{
	CByteArray baBuffer;
	std::string sTemp;
	char buffer[11];
	char *stop;

	std::string in_utf8;

	//Surname
	baBuffer=m_data.GetBytes(SIS_FIELD_ID_POS_Surname,SIS_FIELD_ID_LEN_Surname);
	baBuffer.TrimRight(' ');
	m_Surname.assign((char*)baBuffer.GetBytes(),baBuffer.Size());

	in_utf8 = IBM850_toUtf8(m_Surname);
	m_Surname = in_utf8;

	//Name
	baBuffer=m_data.GetBytes(SIS_FIELD_ID_POS_Name,SIS_FIELD_ID_LEN_Name);
	baBuffer.TrimRight(' ');
	m_Name.assign((char*)(baBuffer.GetBytes()),baBuffer.Size());
	
	in_utf8 = IBM850_toUtf8(m_Name);
	m_Name = in_utf8;

	//Initials
	baBuffer=m_data.GetBytes(SIS_FIELD_ID_POS_Initials,SIS_FIELD_ID_LEN_Initials);
	baBuffer.TrimRight(' ');
	m_Initials.assign((char*)baBuffer.GetBytes(),baBuffer.Size());

	//Gender
	baBuffer=m_data.GetBytes(SIS_FIELD_ID_POS_Gender,SIS_FIELD_ID_LEN_Gender);
	baBuffer.TrimRight(' ');
	m_Gender.assign((char*)baBuffer.GetBytes(),baBuffer.Size());

	//DateOfBirth
	baBuffer=m_data.GetBytes(SIS_FIELD_ID_POS_DateOfBirth,SIS_FIELD_ID_LEN_DateOfBirth);
	if (baBuffer.Size() < 4)
		return false;
    sprintf_s(buffer, sizeof(buffer),"%02x/%02x/%02x%02x", baBuffer.GetByte(3), baBuffer.GetByte(2), baBuffer.GetByte(0), baBuffer.GetByte(1));
	m_DateOfBirth.assign(buffer);

	//SocialSecurityNumber
	baBuffer=m_data.GetBytes(SIS_FIELD_ID_POS_SocialSecurityNumber,SIS_FIELD_ID_LEN_SocialSecurityNumber);
	sTemp=baBuffer.ToString(false);
	sTemp.insert(7," ");
	sTemp.insert(11," ");
	if(sTemp.c_str()[0]=='0')
		m_SocialSecurityNumber=&sTemp.c_str()[1];
	else
		m_SocialSecurityNumber=sTemp;

	//LogicalNumber
	baBuffer=m_data.GetBytes(SIS_FIELD_ID_POS_LogicalNumber,SIS_FIELD_ID_LEN_LogicalNumber);
	sTemp=baBuffer.ToString(false);
	unsigned long ulLogicalNumber=strtoul(sTemp.c_str(),&stop,16);
    sprintf_s(buffer, sizeof(buffer),"%u", ulLogicalNumber);
	m_LogicalNumber.assign(buffer);

	//DateOfIssue
	baBuffer=m_data.GetBytes(SIS_FIELD_ID_POS_DateOfIssue,SIS_FIELD_ID_LEN_DateOfIssue);
	if (baBuffer.Size() < 4)
		return false;
    sprintf_s(buffer, sizeof(buffer),"%02x/%02x/%02x%02x", baBuffer.GetByte(3), baBuffer.GetByte(2), baBuffer.GetByte(0), baBuffer.GetByte(1));
	m_DateOfIssue.assign(buffer);

	//ValidityBeginDate
	baBuffer=m_data.GetBytes(SIS_FIELD_ID_POS_ValidityBeginDate,SIS_FIELD_ID_LEN_ValidityBeginDate);
	if (baBuffer.Size() < 4)
		return false;
    sprintf_s(buffer, sizeof(buffer),"%02x/%02x/%02x%02x", baBuffer.GetByte(3), baBuffer.GetByte(2), baBuffer.GetByte(0), baBuffer.GetByte(1));
	m_ValidityBeginDate.assign(buffer);

	//ValidityEndDate
	baBuffer=m_data.GetBytes(SIS_FIELD_ID_POS_ValidityEndDate,SIS_FIELD_ID_LEN_ValidityEndDate);
	if (baBuffer.Size() < 4)
		return false;
    sprintf_s(buffer, sizeof(buffer),"%02x/%02x/%02x%02x", baBuffer.GetByte(3), baBuffer.GetByte(2), baBuffer.GetByte(0), baBuffer.GetByte(1));
	m_ValidityEndDate.assign(buffer);

	return true;
}

const char *APL_SisFile_ID::getName()
{
	if(ShowData())
		return m_Name.c_str();

	return "";
}

const char *APL_SisFile_ID::getSurname()
{
	if(ShowData())
		return m_Surname.c_str();

	return "";
}

const char *APL_SisFile_ID::getInitials()
{
	if(ShowData())
		return m_Initials.c_str();

	return "";
}

const char *APL_SisFile_ID::getGender()
{
	if(ShowData())
		return m_Gender.c_str();

	return "";
}

const char *APL_SisFile_ID::getDateOfBirth()
{
	if(ShowData())
		return m_DateOfBirth.c_str();

	return "";
}

const char *APL_SisFile_ID::getSocialSecurityNumber()
{
	if(ShowData())
		return m_SocialSecurityNumber.c_str();

	return "";
}

const char *APL_SisFile_ID::getLogicalNumber()
{
	if(ShowData())
		return m_LogicalNumber.c_str();

	return "";
}

const char *APL_SisFile_ID::getDateOfIssue()
{
	if(ShowData())
		return m_DateOfIssue.c_str();

	return "";
}

const char *APL_SisFile_ID::getValidityBeginDate()
{
	if(ShowData())
		return m_ValidityBeginDate.c_str();

	return "";
}

const char *APL_SisFile_ID::getValidityEndDate()
{
	if(ShowData())
		return m_ValidityEndDate.c_str();

	return "";
}

}
