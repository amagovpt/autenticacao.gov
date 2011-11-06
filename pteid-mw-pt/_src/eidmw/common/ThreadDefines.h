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

#ifndef _FILE_THREAD
#define _FILE_THREAD

/* Thread handling: limit threads for single-thread sensitive code.
   Mutexes are used to protect single-thread code against multiple threads.
   Available marco's:

    THREAD_OBJ  			-- Handle for threads
    THREAD_INITFUNCTION		-- Define thread function
    THREAD_START			-- Create thread
    THREAD_END				-- Exit from thread
	THREAD_CLEANUP			-- close the handles (cfr for windows)

 */

#ifdef WIN32
#define THREAD_CLEANUP(sync) if(sync){CloseHandle((HANDLE)sync);}
#else
#define THREAD_CLEANUP(sync)
#endif


/*-------------------------------------	UNIX --------------------------------*/

#if ( defined( __UNIX__ ) || defined( __XMK__ ) ) && defined( USE_THREADS )

#if defined( __osf__ ) || defined( __alpha__ )
  #define __C_ASM_H
#endif /* __osf__  __alpha__*/

#include <pthread.h>
#include <sys/time.h>
#ifdef __XMK__  /* Xilinx XMK */
  #include <sys/process.h>
  #include <sys/timer.h>
#endif /* __XMK__ */


#define THREAD_OBJ			pthread_t
#define THREAD_ID		pthread_t
#define THREAD_INITFUNCTION( parm1, arg )	void *parm1( void *arg )
#define THREAD_START( function, arg, hThread, syncHandle, status ) { \
            status = pthread_create( &hThread, NULL, function, arg ) ? \
                     CRYPT_ERROR : CRYPT_OK; \
            syncHandle = hThread; }

#define THREAD_END( sync )		pthread_exit( ( void * ) 0 )

#if ( defined( __osf__ ) || defined( __alpha__ ) ) && \
    !defined( pthread_self )
  extern pthread_t pthread_self( void );
#endif /* __osf__ */


#ifdef _MPRAS
  #undef THREAD_START
  #define THREAD_START( function, arg, hThread, syncHandle, status ) \
            { \
            status = pthread_create( &hThread, pthread_attr_default, \
                                     function, arg ) ? \
                     CRYPT_ERROR : CRYPT_OK ); \
            syncHandle = ( long ) hThread; \
            }
#endif /* _MPRAS */





/*-------------------------------------	VxWorks --------------------------------*/

#elif defined( __VXWORKS__ )

#include <vxWorks.h>
#include <semLib.h>
#include <taskLib.h>


#define THREAD_ID			int
#define THREAD_OBJ			int

/* if building for PPC: create tasks with FP register saving enabled */

#ifdef __ppc__
  #define TASK_ATTRIBUTES	VX_FP_TASK
#else
  #define TASK_ATTRIBUTES	0
#endif /* __ppc__ */


#define THREAD_INITFUNCTION( parm1, arg )	thread_id parm1( void *arg )
#define THREAD_START( function, arg, hThread, syncHandle, status ) \
            { \
            syncHandle = semBCreate( SEM_Q_FIFO, SEM_EMPTY ); \
            hThread = taskSpawn( NULL, T_PRIORITY, TASK_ATTRIBUTES, 16384, \
                                      function, ( int ) arg, 0, 0, 0, 0, \
                                      0, 0, 0, 0, 0 ); \
            if( hThread == ERROR ) \
                { \
                semDelete( syncHandle ); \
                status = CRYPT_ERROR; \
                } \
            else \
                status = CRYPT_OK; \
            }
#define THREAD_END( sync )		semGive( sync ); \
                                exit( 0 )




/*-------------------------------------	Win32/WINCE --------------------------------*/

#elif ( defined( __WIN32__ ) && !defined( NT_DRIVER ) ) || defined( __WINCE__ )

#ifndef __WINCE__
  #include <process.h>
#endif /* __WINCE__ */


//#define THREAD_OBJ			DWORD           //object handle
#define THREAD_ID			unsigned int          //object handle
#define THREAD_OBJ			uintptr_t           //object handle
#define MUTEX_OBJ			unsigned int    // STH: HANDLE is a void * on VS2005 - WinXP

/*
  InitializeCriticalSection(): throws exception STATUS_NO_MEMORY
  EnterCriticalSection():      throws exception EXCEPTION_POSSIBLE_DEADLOCK if the critical section is corrupted
 */



#if defined( __WIN32__ )
  #define THREAD_INITFUNCTION( parm1, arg ) \
                unsigned __stdcall parm1( void *arg )
  #if defined( _MSC_VER ) && VC_GE_2005( _MSC_VER )
    #define THREAD_START( function, arg, hThread, syncHandle, status ) \
                { \
                uintptr_t oThread; \
                oThread = _beginthreadex( NULL, 0, ( function ), ( arg ), 0, \
                                          &hThread ); \
                syncHandle = oThread; \
                status = ( oThread == 0 ) ? CRYPT_ERROR : CRYPT_OK; \
                }
  #else
    #define THREAD_START( function, arg, hThread, syncHandle, status ) \
                { \
                unsigned long oThread; \
                oThread = _beginthreadex( NULL, 0, ( function ), ( arg ), 0, \
                                          &hThread ); \
                syncHandle = ( MUTEX_OBJ ) oThread; \
                status = ( oThread == 0 ) ? CRYPT_ERROR : CRYPT_OK; \
                }
  #endif /* _MSC_VER */
#define THREAD_END( sync ) _endthreadex( 0 ); return( 0 )
#elif defined( __WINCE__ )
  #define THREAD_INITFUNCTION( parm1, arg ) \
                DWORD WINAPI parm1( void *arg )
  #define THREAD_START( function, arg, hThread, syncHandle, status ) \
                { \
                HANDLE oThread; \
                oThread = CreateThread( NULL, 0, ( function ), ( arg ), 0, \
                                        &hThread ); \
                syncHandle = oThread; \
                status = ( oThread == NULL ) ? CRYPT_ERROR : CRYPT_OK; \
                }
  #define THREAD_END( sync )	ExitThread( 0 ); return( 0 )
#endif /* Win32 - WINCE */

#elif defined( __WIN32__ ) && defined( NT_DRIVER )

#define THREAD_ID				HANDLE
#define THREAD_OBJ				HANDLE
#define MUTEX_OBJ				HANDLE



#else /* OS-switch */
/*-------------------------------------	default --------------------------------*/
/* for OS's without threading */

#define MUTEX_OBJ							int
#define THREAD_OBJ							int
#define THREAD_ID						int
#define THREAD_START( function, arg, hThread, syncHandle, status ) \
                                                status = CRYPT_ERROR
#define THREAD_END( sync )

#endif /* OS-switch */

#endif /* _FILE_THREAD */
