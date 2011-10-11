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
#ifndef __DIAGLIB_ERROR_H__
#define __DIAGLIB_ERROR_H__

#define DIAGLIB_OK							0
#define DIAGLIB_REBOOT_NEEDED				1

#define DIAGLIB_ERR_INTERNAL				10
#define DIAGLIB_ERR_BAD_CALL				11
#define DIAGLIB_ERR_NOT_AVAILABLE			12
#define DIAGLIB_ERR_LIBRARY_NOT_FOUND		13
#define DIAGLIB_ERR_NOT_ALLOWED_BY_USER		14

#define DIAGLIB_ERR_FILE_CREATE_FAILED		100
#define DIAGLIB_ERR_FILE_OPEN_FAILED		101
#define DIAGLIB_ERR_FILE_READ_FAILED		102
#define DIAGLIB_ERR_FILE_LOCKED				103
#define DIAGLIB_ERR_FILE_DELETE_FAILED		104	
#define DIAGLIB_ERR_FILE_ALREADY_EXIST		105
#define DIAGLIB_ERR_FILE_NOT_FOUND			106

#define DIAGLIB_ERR_PROCESS_KILL_FAILED		200
#define DIAGLIB_ERR_PROCESS_START_FAILED	201
#define DIAGLIB_ERR_PROCESS_WAIT_TIMEOUT	202
#define DIAGLIB_ERR_PROCESS_NOT_FOUND		203
#define DIAGLIB_ERR_PROCESS_ACCESS_DENIED	204

#define DIAGLIB_ERR_SERVICE_STOP_FAILED		301	
#define DIAGLIB_ERR_SERVICE_STOP_TIMEOUT	302
#define DIAGLIB_ERR_SERVICE_DELETE_FAILED	303	
#define DIAGLIB_ERR_SERVICE_START_FAILED	304	
#define DIAGLIB_ERR_SERVICE_START_TIMEOUT	305

#define DIAGLIB_ERR_DEVICE_NOT_FOUND		400

#define DIAGLIB_ERR_REGISTRY_READ_FAILED	500
#define DIAGLIB_ERR_REGISTRY_WRITE_FAILED	501
#define DIAGLIB_ERR_REGISTRY_DELETE_FAILED	502
#define DIAGLIB_ERR_REGISTRY_NOT_FOUND		503

#define DIAGLIB_ERR_PCSC_CONTEXT_FAILED		600
#define DIAGLIB_ERR_PCSC_CONNECT_FAILED		601
#define DIAGLIB_ERR_PCSC_TRANSMIT_FAILED	602
#define DIAGLIB_ERR_PCSC_CHECK_TIMING_FAILED 603

#define DIAGLIB_ERR_READER_NOT_FOUND		700

#define DIAGLIB_ERR_CARD_NOT_FOUND			800
#define DIAGLIB_ERR_CARD_BAD_TYPE			801

#define DIAGLIB_ERR_CSP_FAILED				900
#define DIAGLIB_ERR_CSP_CONTEXT_FAILED		901
#define DIAGLIB_ERR_CSP_REGISTRATION_FAILED	902

#define DIAGLIB_ERR_PKCS_FAILED				1000
#define DIAGLIB_ERR_PKCS_INIT_FAILED		1001
#define DIAGLIB_ERR_PKCS_SIGNING_FAILED		1002
#define DIAGLIB_ERR_PKCS_KEY_NOT_FOUND		1003

#define DIAGLIB_ERR_PIN_CANCEL				1100
#define DIAGLIB_ERR_PIN_BLOCKED				1101
#define DIAGLIB_ERR_PIN_WRONG				1102
#define DIAGLIB_ERR_PIN_FAILED				1103

#define DIAGLIB_ERR_SOFTWARE_NOT_FOUND		1200

#endif //__DIAGLIB_ERROR_H__

