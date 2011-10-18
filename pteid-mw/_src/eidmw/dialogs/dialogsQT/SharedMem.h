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
#ifndef SHAREDMEM_H
#define SHAREDMEM_H

#include "sys/ipc.h"
#include "sys/shm.h"
#include <string>

namespace eIDMW
{

  class SharedMem
  {
  private:
    int m_iShmid;
    unsigned int m_iAttachedProcs;
    key_t m_tKey;
    std::string m_csFilename;
    
    
  public:
    SharedMem();
    ~SharedMem();

    void Attach(size_t tMemorySize, 
		const char *csReadableFilePath,
		void ** content);

    void Detach(void* content);  

    static void Delete(int iSegmentID);
    static int getNAttached(int iSegmentID);

    int getID(){return m_iShmid;};
    
    std::string getFilename() {return m_csFilename;};
  };

}
#endif
