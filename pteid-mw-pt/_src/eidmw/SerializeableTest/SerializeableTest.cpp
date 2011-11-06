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
// SerializeableTest.cpp : Defines the entry point for the console application.
//

#include <iostream> 

#include "../common/Log.h"

using namespace std;

#include "stdafx.h"

#include "../Common/ByteArray.h"
#include "../Common/ByteArrayReader.h"

#include "../common/Serializeable/Ser_TLV.h"
#include "../common/Serializeable/Ser_GetConfigRequest.h"
#include "../common/Serializeable/Ser_TLVTagDefinitions.h"
#include "../common/Serializeable/Ser_Factory.h"
#include "../common/Serializeable/Serializeable.h"
#include "../common/Serializeable/Ser_GetConfigRequest.h"

using namespace eIDMW;

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "Object serialization tests...\n\n";
	cout << "Please check the generated log file for the results of this test!\n\n";

	LogSetPrintHeader(false);
	LogSetPrintInfo(false);

	CByteArray theSerialStream;
	CByteArray theResultStream;

	// Build some test objects and serialize them into a CByteStream (theSerialStream)
	{
		MWLOG(LEV_WARN, MOD_TEST, "------------ Serialization input ------------\n");
		cout << "Serializing some objects...\n";

		// A TLV that holds the version as long
		unsigned char version = 0;
		CSer_TLV aSerTLV = CSer_TLV(TLV_TAG_LONG, &version, sizeof(unsigned char));
		aSerTLV.SerializeInto(theSerialStream);
		aSerTLV.LogObject();

		// A TLV that holds a simple string
		std::string title = "just a simple test";
		aSerTLV = CSer_TLV(TLV_TAG_STRING, reinterpret_cast<unsigned char *>(const_cast<char *>(title.c_str())), 
					static_cast<unsigned long>(title.length()));
		aSerTLV.SerializeInto(theSerialStream);
		aSerTLV.LogObject();

		// Request the version no from the tools settings in the configuration file
		CSer_GetConfigRequest aSerGetConfigRequest = CSer_GetConfigRequest("ToolVersionNo", "Tool Settings");
		aSerGetConfigRequest.SerializeInto(theSerialStream);
		aSerGetConfigRequest.LogObject();

	}

	// De-serialize above created CByteStream and call the Invoke() method for each object found 
	// in this stream, this will create a new CByteStream holding all resulting objects, if any
	{
		cout << "De-serializing these objects...\n";

		// Get a helper object used to read from CByteArray objects
		CByteArrayReader *aByteReader = new CByteArrayReader(&theSerialStream);

		// Read objects one after one from the stream
		try
		{
			while (!aByteReader->IsEmpty())
			{
				CSer_Factory *theFactory = CSer_Factory::GetSer_Factory();
				CSerializeable *aSerializeable = theFactory->ReadObject(aByteReader);
				aSerializeable->Invoke(theResultStream);
			}
		}

		catch (...)
		{
			cout << "*** Error during de-serialization ! ***\n";
		}

		delete aByteReader;

	}

	// De-serialize the result stream from previous de-serialization and call the LogObject 
	// method for each object found in this stream
	{
		MWLOG(LEV_WARN, MOD_TEST, "------------ Serialization output -----------\n");
		cout << "Writing out the results...\n";

		// Get a helper object used to read from CByteArray objects
		CByteArrayReader *aByteReader = new CByteArrayReader(&theResultStream);

		// Read objects one after one from the stream
		try
		{
			while (!aByteReader->IsEmpty())
			{
				CSer_Factory *theFactory = CSer_Factory::GetSer_Factory();
				CSerializeable *aSerializeable = theFactory->ReadObject(aByteReader);
				aSerializeable->LogObject();
			}
		}

		catch (...)
		{
			cout << "*** Error during writing results ! ***\n";
		}

		delete aByteReader;

	}

	cout << "\n";

	return 0;
}

