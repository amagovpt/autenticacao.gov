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

#ifndef TRANSACTION_H
#define TRANSACTION_H

#ifdef UNICODE
#undef UNICODE
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <wintypes.h>
#include <stddef.h>
#include <sys/types.h>
#endif

namespace eidmw { namespace pcscproxy {
				  class Transaction {
public:
				  Transaction(const char* name);
				  void aquire();
				  void release();
				  bool justStarted();
				  ~Transaction();
private:
				  size_t cnt;
				  Transaction();                              //do not implement
				  Transaction(const Transaction&);            //do not implement
				  Transaction& operator=(const Transaction&); //do not implement
				  int hash(const char* str);
				  bool            isJustStarted;
		#ifdef _WIN32
				  HANDLE          hMutex;
		#else
				  pthread_mutex_t *hMutex;
				  int             shmid;
		#endif
				  };
				  } //pcscproxy
}                   //eidmw

#endif
