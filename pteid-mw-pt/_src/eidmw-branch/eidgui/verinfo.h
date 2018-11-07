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
/////////////////////////////////////////////////////////////////////////////
/*
DESCRIPTION:
	CFileVersionInfo - Class for getting file version information
*/
/////////////////////////////////////////////////////////////////////////////

#ifndef __VERINFO_H__
#define __VERINFO_H__

#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////

/*
#ifndef	_INC_SHLWAPI
	#include < Shlwapi.h >
#endif
#pragma comment( lib, "shlwapi.lib" )
*/
#ifndef VER_H
	#include <Windows.h>
	#include <WinVer.h>
#endif
#pragma comment( lib, "Version.lib " )

#ifdef WIN32
#pragma warning(disable:4996)			// Don't warn for sptintf() and others to be unsafe
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/*
#ifndef DLLVER_MAJOR_MASK

	typedef struct _DLLVERSIONINFO2
    {
		DLLVERSIONINFO	info1;
		DWORD			dwFlags;				// No flags currently defined
		ULONGLONG		ullVersion;
	} DLLVERSIONINFO2;

#endif // DLLVER_MAJOR_MASK
*/

#ifndef MAKEDLLVERULL
	#define MAKEDLLVERULL( major, minor, build, qfe )\
		( ( (ULONGLONG)(major) << 48 ) |			 \
		(   (ULONGLONG)(minor) << 32 ) |			 \
		(   (ULONGLONG)(build) << 16 ) |			 \
		(   (ULONGLONG)(  qfe) <<  0 ) )
#endif // MAKEDLLVERULL
/////////////////////////////////////////////////////////////////////////////
typedef enum _VI_CP
{
	VI_CP_ASCII				= 0,	// 7-bit ASCII
	VI_CP_JAPAN				= 932,	// Japan (Shift - JIS X-0208)
	VI_CP_KOREA				= 949,	// Korea (Shift - KSC 5601)
	VI_CP_TAIWAN			= 950,	// Taiwan (Big5)
	VI_CP_UNICODE			= 1200,	// Unicode
	VI_CP_LATIN2			= 1250,	// Latin-2 (Eastern European)
	VI_CP_CYRILLIC			= 1251,	// Cyrillic
	VI_CP_MULTILNG			= 1252,	// Multilingual
	VI_CP_GREEK				= 1253,	// Greek
	VI_CP_TURKISH			= 1254,	// Turkish
	VI_CP_HEBREW			= 1255,	// Hebrew
	VI_CP_ARABIC			= 1256	// Arabic
} VI_CP;

typedef enum _VI_STR
{
	VI_STR_COMMENTS			= 0,	// Comments
	VI_STR_COMPANYNAME		= 1,	// CompanyName
	VI_STR_FILEDESCRIPTION	= 2,	// FileDescription
	VI_STR_FILEVERSION		= 3,	// FileVersion
	VI_STR_INTERNALNAME		= 4,	// InternalName
	VI_STR_LEGALCOPYRIGHT	= 5,	// LegalCopyright
	VI_STR_LEGALTRADEMARKS	= 6,	// LegalTrademarks
	VI_STR_ORIGINALFILENAME	= 7,	// OriginalFilename
	VI_STR_PRIVATEBUILD		= 8,	// PrivateBuild
	VI_STR_PRODUCTNAME		= 9,	// ProductName
	VI_STR_PRODUCTVERSION	= 10,	// ProductVersion
	VI_STR_SPECIALBUILD		= 11,	// SpecialBuild
	VI_STR_OLESELFREGISTER	= 12	// OLESelfRegister
} VI_STR;

class CFileVersionInfo
{
public:	// Construction/destruction:
	CFileVersionInfo( void )
		: m_lpbyVIB( NULL )
	{
		Close();
	}

	virtual ~CFileVersionInfo( void )
	{
		Close();
	}

public:	// Implementation:
	bool Open( const char* pFileName )
	{
		if( strlen(pFileName) == 0 )
			return( FALSE );
		Close();
		if( !GetVersionInfo( pFileName ) || !QueryVersionTrans() )
			Close();
		return m_bValid;
	};


	void Close( void )
	{
		m_nTransCnt  = 0;
		m_nTransCur  = 0;
		m_bValid	 = false;
		m_lpdwTrans  = NULL;
		::ZeroMemory( &m_vsffi, sizeof( VS_FIXEDFILEINFO ) );
		delete [] m_lpbyVIB;
	}

	bool QueryStringValue(const char* szItem, char* szValue) const
	{
		if( false == m_bValid || 0 == strlen(szItem) )
			return( false );

		szValue[0] = 0;

		char szSFI[ MAX_PATH ] = { 0 };

		sprintf( szSFI,  "\\StringFileInfo\\%04X%04X\\%s", GetCurLID(), GetCurCP(), szItem );

		bool   bRes    = false;
		UINT   uLen    = 0;
		char*  lpszBuf = NULL;

		if( ::VerQueryValueA( m_lpbyVIB, szSFI, (LPVOID*)&lpszBuf, &uLen ) )
		{
			strcpy(szValue,lpszBuf);
			//szValue = wxString::Format(wxT("%s"), lpszBuf);
			bRes = true;
		}
		return ( bRes );
	}

	bool QueryStringValue(INT nIndex, char* szValue) const
	{
		if( nIndex < VI_STR_COMMENTS ||
			nIndex > VI_STR_OLESELFREGISTER )
		{
			return( FALSE );
		}
		return QueryStringValue( s_ppszStr[ nIndex ], szValue );
	}

	char* GetVerStringName(INT nIndex )
	{
		if( nIndex < VI_STR_COMMENTS ||
			nIndex > VI_STR_OLESELFREGISTER )
		{
			return( NULL );
		}
		return s_ppszStr[ nIndex ];
	}

	bool SetTrans (LANGID wLID = LANG_NEUTRAL, WORD wCP = VI_CP_UNICODE )
	{
		if( m_bValid == false )
			return( false );
		if( GetCurLID() == wLID && GetCurCP() == wCP )
			return true;

		INT nPos = FindTrans( wLID, wCP );

		if( nPos != -1 )
			m_nTransCur = nPos;
		return ( m_nTransCur == (UINT)nPos );
	}

	bool SetTransIndex(UINT nIndex = 0 )
	{
		if( m_bValid == false )
			return( false );
		if( m_nTransCur == nIndex )
			return true;
		if( nIndex >= 0 && nIndex <= m_nTransCnt )
			m_nTransCur = nIndex;

		return ( m_nTransCur == nIndex );
	}

	INT	FindTrans( LANGID wLID, WORD wCP ) const
	{
		if( m_bValid == false )
			return( -1 );

		for( UINT n = 0; n < m_nTransCnt; n++ )
		{
			if( LOWORD( m_lpdwTrans[ n ] ) == wLID &&
				HIWORD( m_lpdwTrans[ n ] ) == wCP  )
			{
				return n;
			}
		}
		return -1;
	}
	DWORD GetTransByIndex( UINT nIndex  ) const
	{
		if( m_bValid == false || nIndex < 0 || nIndex > m_nTransCnt )
			return( 0 );
		return m_lpdwTrans[ nIndex ];
	}

public: // Static members:
	// If the LID identifier is unknown, it returns a
	// default string ("Language Neutral"):
	static bool	GetLIDName( WORD wLID, char* szName )
	{
		bool bRet = false;
		char pszName[256] = {0};

		if (::VerLanguageNameA( wLID, pszName, sizeof(pszName) ))
		{
			bRet = true;
		}

		if(bRet)
		{
			strcpy(szName,pszName);
			//szName = wxString::Format(wxT("%s"), pszName);
		}

		return bRet;
	}

	// If the CP identifier is unknown, it returns a
	// default string ("Unknown"):
	static bool		GetCPName(  WORD wCP,  char* szName )
	{
		bool bRes = true;
		switch ( wCP )
		{
		case VI_CP_ASCII:
			strcpy(szName,"7-bit ASCII" );
			break;
		case VI_CP_JAPAN:
			strcpy(szName,"Japan (Shift – JIS X-0208)" );
			break;
		case VI_CP_KOREA:
			strcpy(szName,"Korea (Shift – KSC 5601)" );
			break;
		case VI_CP_TAIWAN:
			strcpy(szName,"Taiwan (Big5)" );
			break;
		case VI_CP_UNICODE:
			strcpy(szName,"Unicode" );
			break;
		case VI_CP_LATIN2:
			strcpy(szName,"Latin-2 (Eastern European)" );
			break;
		case VI_CP_CYRILLIC:
			strcpy(szName,"Cyrillic" );
			break;
		case VI_CP_MULTILNG:
			strcpy(szName,"Multilingual" );
			break;
		case VI_CP_GREEK:
			strcpy(szName,"Greek" );
			break;
		case VI_CP_TURKISH:
			strcpy(szName,"Turkish" );
			break;
		case VI_CP_HEBREW:
			strcpy(szName,"Hebrew" );
			break;
		case VI_CP_ARABIC:
			strcpy(szName,"Arabic" );
			break;
		default:
			strcpy(szName,"Unknown" );
			bRes = false;
			break;
		}
		return bRes;
	}
public:
	const	VS_FIXEDFILEINFO& GetVSFFI( void ) const
	{
		return m_vsffi;
	}

	bool IsValid( void ) const
	{
		return m_bValid;
	}

	WORD GetFileVersionMajor( void ) const
	{  return HIWORD( m_vsffi.dwFileVersionMS ); }

	WORD GetFileVersionMinor( void ) const
	{  return LOWORD( m_vsffi.dwFileVersionMS ); }

	WORD GetFileVersionBuild( void ) const
	{  return HIWORD( m_vsffi.dwFileVersionLS ); }

	WORD GetFileVersionQFE( void )   const
	{  return LOWORD( m_vsffi.dwFileVersionLS ); }

	WORD GetProductVersionMajor( void ) const
	{  return HIWORD( m_vsffi.dwProductVersionMS ); }

	WORD GetProductVersionMinor( void ) const
	{  return LOWORD( m_vsffi.dwProductVersionMS ); }

	WORD GetProductVersionBuild( void ) const
	{  return HIWORD( m_vsffi.dwProductVersionLS ); }

	WORD GetProductVersionQFE( void )   const
	{  return LOWORD( m_vsffi.dwProductVersionLS ); }

	UINT GetTransCount( void ) const
	{ return m_nTransCnt; }

	UINT GetCurTransIndex( void ) const
	{  return m_nTransCur; }

	LANGID GetLIDByIndex(  UINT nIndex ) const
	{ return LOWORD( GetTransByIndex( nIndex ) ); }

	WORD GetCPByIndex(  UINT nIndex ) const
	{ return HIWORD( GetTransByIndex( nIndex ) ); }

	DWORD GetCurTrans( void ) const
	{ return GetTransByIndex( GetCurTransIndex() ); }

	LANGID GetCurLID( void ) const
	{ return GetLIDByIndex( GetCurTransIndex() ); }

	WORD GetCurCP( void ) const
	{ return GetCPByIndex( GetCurTransIndex() ); }


protected:
	bool GetVersionInfo(  const char* szFileName )
	{
		DWORD dwDummy = 0;
		DWORD dwSize  = ::GetFileVersionInfoSizeA(szFileName, &dwDummy);

		if ( dwSize > 0 )
		{
			m_lpbyVIB = new char [ dwSize ];
			if ( m_lpbyVIB != NULL && ::GetFileVersionInfoA(szFileName, 0, dwSize, m_lpbyVIB ) )
			{
				UINT   uLen    = 0;
				LPVOID lpVSFFI = NULL;
				if ( ::VerQueryValueA( m_lpbyVIB,  "\\", (LPVOID*)&lpVSFFI, &uLen ) )
				{
					::CopyMemory( &m_vsffi, lpVSFFI, sizeof( VS_FIXEDFILEINFO ) );
					m_bValid = ( m_vsffi.dwSignature == VS_FFI_SIGNATURE );
				}
			}
		}
		return m_bValid;
	}

	bool QueryVersionTrans( void )
	{
		if( m_bValid == false )
			return( false );
		UINT   uLen  = 0;
		LPVOID lpBuf = NULL;

		if( ::VerQueryValueA( m_lpbyVIB,  "\\VarFileInfo\\Translation", (LPVOID*)&lpBuf, &uLen ) )
		{
			m_lpdwTrans = (LPDWORD)lpBuf;
			m_nTransCnt = ( uLen / sizeof( DWORD ) );
		}
		return (bool)( m_lpdwTrans != NULL );
	}


protected: // Members variables
	static char*	 s_ppszStr[];	// String names
	VS_FIXEDFILEINFO m_vsffi;		// Fixed File Info (FFI)
	char*			 m_lpbyVIB;		// Pointer to version info block (VIB)
	LPDWORD			 m_lpdwTrans;	// Pointer to translation array in m_lpbyVIB, LOWORD = LangID and HIWORD = CodePage
	UINT			 m_nTransCur;	// Current translation index
	UINT			 m_nTransCnt;	// Translations count
	bool			 m_bValid;		// Version info is loaded
};



/////////////////////////////////////////////////////////////////////////////
#endif //__VER_H__
/////////////////////////////////////////////////////////////////////////////

