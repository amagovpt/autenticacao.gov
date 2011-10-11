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
#include "../common/LogBase.h"

using namespace eIDMW;


TEST(Initialize)
{
//INITIALIZING THE LOGGER
	CLogger &logger = CLogger::instance();
    logger.init(L"C:.", L"Test", 100000, 5,LOG_LEVEL_ERROR, true);
}

TEST(LogWithLoggerObject)
{
	int i=1;
	CLogger &logger = CLogger::instance();

//SIMPLE EXAMPLE (NO GROUP, NO FILE TRACE)
	logger.write(LOG_LEVEL_ERROR,L"LoggerObject %d: %ls", i++, L"First test");

//SIMPLE EXAMPLE WITH FILE TRACE (NO GROUP)
	logger.write(LOG_LEVEL_ERROR,__LINE__,__WFILE__,L"LoggerObject %d: %ls", i++, L"Second test");

//EXAMPLE WITH GROUP (NO FILE TRACE)
	logger.writeToGroup(L"Group1",LOG_LEVEL_ERROR,L"LoggerObject %d: %ls", i++, L"Third test");

//EXAMPLE WITH GROUP AND FILE TRACE
	logger.writeToGroup(L"Group1",LOG_LEVEL_ERROR,__LINE__,__WFILE__,L"LoggerObject %d: %ls", i++, L"Fourth test");
}

TEST(LogWithLogObject)
{
	int i=1;
	CLogger &logger = CLogger::instance();

//SIMPLE EXAMPLE (NO GROUP, NO FILE TRACE)
	CLog &logA=logger.getLogW();
	logA.write(LOG_LEVEL_ERROR,L"LogObject %d: %ls", i++, L"First test");
	logA.writeError(L"LogObject %d: %ls", i++, L"First test");

//SIMPLE EXAMPLE WITH FILE TRACE (NO GROUP)
	CLog &logB=logger.getLogW();
	logB.write(LOG_LEVEL_ERROR,__LINE__,__WFILE__,L"LogObject %d: %ls", i++, L"Second test");
	logB.writeError(__LINE__,__WFILE__,L"LogObject %d: %ls", i++, L"Second test");

//EXAMPLE WITH GROUP (NO FILE TRACE)
	CLog &logGroup1A=logger.getLogW(L"Group1");
	logGroup1A.write(LOG_LEVEL_ERROR,L"LogObject %d: %ls", i++, L"Third test");
	logGroup1A.writeError(L"LogObject %d: %ls", i++, L"Third test");

//EXAMPLE WITH GROUP AND FILE TRACE
	CLog &logGroup1B=logger.getLogW(L"Group1");
	logGroup1B.write(LOG_LEVEL_ERROR,__LINE__,__WFILE__,L"LogObject %d: %ls", i++, L"Fourth test");
	logGroup1B.writeError(__LINE__,__WFILE__,L"LogObject %d: %ls", i++, L"Fourth test");
}

TEST(LogWithMacroGeneric)
{
	int i=1;
//SIMPLE EXAMPLE (NO GROUP, NO FILE TRACE)
	ZS_LOG(LOG_LEVEL_ERROR,L"MacroGeneric %d: %ls", i++, L"First test");

//SIMPLE EXAMPLE WITH FILE TRACE (NO GROUP)
	ZS_LOG(LOG_LEVEL_ERROR,__LINE__,__WFILE__,L"MacroGeneric %d: %ls", i++, L"Second test");

//EXAMPLE WITH GROUP (NO FILE TRACE)
	ZS_LOG_GROUP(L"Group1",LOG_LEVEL_ERROR,L"MacroGeneric %d: %ls", i++, L"Third test");

//EXAMPLE WITH GROUP AND FILE TRACE
	ZS_LOG_GROUP(L"Group1",LOG_LEVEL_ERROR,__LINE__,__WFILE__,L"MacroGeneric %d: %ls", i++, L"Fourth test");
}

TEST(LogWithMacroSpecific)
{
	int i=1;
//SIMPLE EXAMPLE (NO GROUP, NO FILE TRACE)
	ZS_LOG_ERROR(L"MacroSpecific %d: %ls", i++, L"First test");

//SIMPLE EXAMPLE WITH FILE TRACE (NO GROUP)
	ZS_LOG_ERROR_TRACE(L"MacroSpecific %d: %ls", i++, L"Second test");

//EXAMPLE WITH GROUP (NO FILE TRACE)
	ZS_LOG_GROUP1(LOG_LEVEL_ERROR,L"MacroSpecific %d: %ls", i++, L"Third test");
	ZS_LOG_GROUP1_ERROR(L"MacroSpecific %d: %ls", i++, L"Third test");

//EXAMPLE WITH GROUP AND FILE TRACE
	ZS_LOG_GROUP1(LOG_LEVEL_ERROR,__LINE__,__WFILE__,L"Example %d: %ls", i++, L"Fourth test");
	ZS_LOG_GROUP1_ERROR_TRACE(L"MacroSpecific %d: %ls", i++, L"Fourth test");

}

