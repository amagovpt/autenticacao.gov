/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2009-2010 FedICT.
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

/*
 * Logging for the integration test for the PKCS#11 library.
 */

#include <stdio.h>
#include "logtest.h"

void initLog()
{
  pkcs11test_logfile = fopen (pkcs11test_filename,"w");
}

void endLog()
{
  if (pkcs11test_logfile!=NULL)
  {
	fclose (pkcs11test_logfile);
  }
}

void logPrefix(PKCS11TEST_LOG_LEVEL level)
{
	switch (level)
	{
	case LVL_DEBUG:
		printlogprefic(pkcs11test_logfile,"    DEBUG: ");
		break;
	case LVL_INFO:
		printlogprefic(pkcs11test_logfile,"  INFO: ");
		break;
	case LVL_WARNING:
		printlogprefic(pkcs11test_logfile,"WARNING: ");
		break;
	case LVL_ERROR:
		printlogprefic(pkcs11test_logfile,"<<ERROR>>: ");
		break;
	case LVL_NOLEVEL:
	default:
		break;
	}
}


