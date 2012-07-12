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
#include "errno.h"

#include "SharedMem.h"
#include "Log.h"
#include "MWException.h"
#include "eidErrors.h"

namespace eIDMW 
{

SharedMem::SharedMem()
{
}

SharedMem::~SharedMem()
{
	Delete(m_iShmid);
}

void SharedMem::Attach(size_t tMemorySize, 
					   const char *csReadableFilePath,
					   void ** content)
{
	m_csFilename = csReadableFilePath;

	// create a key on the base of the file name passed
	m_tKey = ftok(csReadableFilePath, 0666 | IPC_CREAT);
	if(m_tKey == (key_t)-1)
	{
		MWLOG(LEV_ERROR, MOD_DLG, L"  SharedMem::Attach ftok: %s", strerror(errno) );
		throw CMWEXCEPTION(EIDMW_ERR_MEMORY);
	}

	// create the memory segment and get its ID
	m_iShmid = shmget(m_tKey,tMemorySize, 0666 | IPC_CREAT );
	if(m_iShmid == -1)
	{
		MWLOG(LEV_ERROR, MOD_DLG,   L"  SharedMem::Attach shmid: %s",strerror(errno));
		throw CMWEXCEPTION(EIDMW_ERR_MEMORY);
	}

	// attach the pointer
	if ((*content = (void*)shmat(m_iShmid, 0, 0)) == (void*)-1) {
		MWLOG(LEV_ERROR, MOD_DLG,   L"  SharedMem::Attach shmat: %s",strerror(errno));
		throw CMWEXCEPTION(EIDMW_ERR_MEMORY);
	}
	
	MWLOG(LEV_DEBUG, MOD_DLG, L"  SharedMem::Attach attached segment with ID %d",m_iShmid);
}

void SharedMem::Detach(void* content)
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"  SharedMem::Detach segment with ID %d",m_iShmid);
	
	if( shmdt(content) == -1)
	{
		MWLOG(LEV_ERROR, MOD_DLG, L"  SharedMem::Detach shmdt: %s", strerror(errno) );
		throw CMWEXCEPTION(EIDMW_ERR_MEMORY);
    }
}


int SharedMem::getNAttached(int iSegmentID)
{
	shmid_ds tResult;
    if( shmctl(iSegmentID, IPC_STAT, &tResult) == -1)
	{
		MWLOG(LEV_DEBUG, MOD_DLG, L"  SharedMem::getNAttached shmctl: %s",strerror(errno) ); 
		return -1;
    }
    return tResult.shm_nattch;
}

void SharedMem::Delete(int iSegmentID)
{

	// delete the shared memory area
    // once all processes have detached from this segment

    int iAttachedProcs = getNAttached(iSegmentID);

    if( iAttachedProcs > -1 ) 
	{

		MWLOG(LEV_DEBUG, MOD_DLG, L"  SharedMem::Delete : request to delete memory segment with ID %d. N proc = %d", iSegmentID,iAttachedProcs);

		// check that no process is attached to this segment
		if( iAttachedProcs == 0)
		{
			if( shmctl(iSegmentID,IPC_RMID,NULL) == 1) 
			{
				MWLOG(LEV_ERROR, MOD_DLG,   L"  SharedMem::Delete shmctl: %s", strerror(errno) );
				throw CMWEXCEPTION(EIDMW_ERR_MEMORY);
			}
			MWLOG(LEV_DEBUG, MOD_DLG, L"  SharedMem::Delete : deleted memory segment with ID %d", iSegmentID);
		}
	}
}

}
