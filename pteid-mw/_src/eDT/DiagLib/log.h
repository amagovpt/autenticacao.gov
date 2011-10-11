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
#ifndef __DIAGLIB_LOG_H__
#define __DIAGLIB_LOG_H__

#define LOG_OPEN_ATTEMPT_COUNT 5

#ifdef WIN32
	#define LOG_ERROR_STRING_PREFIX LOG(L"ERROR: %hs - ",__FUNCTION__)
#elif defined __APPLE__
	#define LOG_ERROR_STRING_PREFIX LOG(L"ERROR: %s - ",__FUNCTION__)
	#define LOG_CFERROR(format,message)	{LOG_ERROR_STRING_PREFIX; LOG(format L"\n",string_From_CFStringRef(message).c_str());}
#endif


#define LOG_ERROR(param) {LOG_ERROR_STRING_PREFIX; LOG(param L"\n");}
#define LOG_LASTERROR(message) {LOG_ERROR_STRING_PREFIX; LOG(message L" (LastError=%ld - 0x%x)\n", GetLastError(), GetLastError());}
#define LOG_LASTERROR1(format,message1) {LOG_ERROR_STRING_PREFIX; LOG(format L" (LastError=%ld - 0x%x)\n", message1, GetLastError(), GetLastError());}
#define LOG_ERRORCODE(message,code) {LOG_ERROR_STRING_PREFIX; LOG(message L" (Error code=%ld - 0x%x)\n", code, code);}
#define LOG_PCSCERROR(message,code,SW1,SW2) {LOG_ERROR_STRING_PREFIX; LOG(message L" (Error code=%ld - SW1=0x%x - SW2=0x%x )\n", code, SW1, SW2);}
#define LOG_ERROR1(format,message1)	{LOG_ERROR_STRING_PREFIX; LOG(format L"\n",message1);}

#define RETURN_LOG_ERROR(code) (LOG_ERROR_STRING_PREFIX, LOG(L"return code=%ld\n",code), iReturnCode=code)
#define RETURN_LOG_BAD_FUNCTION_CALL (LOG_ERROR_STRING_PREFIX, LOG(L"return on BAD FUNCTION CALL\n"), iReturnCode=DIAGLIB_ERR_BAD_CALL)
#define RETURN_LOG_INTERNAL_ERROR (LOG_ERROR_STRING_PREFIX, LOG(L"return on INTERNAL ERROR\n"), iReturnCode=DIAGLIB_ERR_INTERNAL)

int logGetFileName(const wchar_t **file);
int logInit(const wchar_t *wzLogFile);
int LOG(const wchar_t *format, ...);
int LOG_TIME(const wchar_t *format, ...);
int logFinalize();

int logUnloadResources();

#endif //__DIAGLIB_LOG_H__
