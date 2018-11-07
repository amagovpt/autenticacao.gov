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
//
// CDataFile Class Implementation
//
// The purpose of this class is to provide a simple, full featured means to
// store persistent data to a text file.  It uses a simple key/value paradigm
// to achieve this.  The class can read/write to standard Windows .ini files,
// and yet does not rely on any windows specific calls.  It should work as
// well in a linux environment (with some minor adjustments) as it does in
// a Windows one.
//
//
// CDataFile
// The purpose of this class is to provide the means to easily store key/value
// pairs in a config file, separated by independent sections. Sections may not
// have duplicate keys, although two or more sections can have the same key.
// Simple support for comments is included. Each key, and each section may have
// it's own multiline comment.
//
// An example might look like this;
//
// [UserSettings]
// Name=Joe User
// Date of Birth=12/25/01
//
// ;
// ; Settings unique to this server
// ;
// [ServerSettings]
// Port=1200
// IP_Address=127.0.0.1
// MachineName=ADMIN
//

#include "datafile.h"
#include "eidErrors.h"
#include "MWException.h"
#include "Thread.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <float.h>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <limits.h>

#ifdef WIN32
    #include <windows.h>
#endif

#include "Util.h"

// Compatibility Defines ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
  #define snprintf  _snprintf
  #define vsnprintf  _vsnprintf
#else
  #define _snprintf_s snprintf
  #define _vsnprintf_s vsnprintf
#endif

namespace eIDMW
{
	
CMutex CDataFile::sm_Mutex;
	
// CDataFile
// Our default constructor.  If it can load the file, it will do so and populate
// the section list with the values from the file.
CDataFile::CDataFile(t_Str szFileName)
{
	m_stream=NULL;
	m_bDirty = false;
	m_szFileName = szFileName;
	m_Flags = (AUTOCREATE_SECTIONS | AUTOCREATE_KEYS);
}

CDataFile::CDataFile()
{
	m_stream=NULL;
	m_bDirty = false;
	m_szFileName = t_Str(L"");
	m_Sections.clear();
	m_Flags = (AUTOCREATE_SECTIONS | AUTOCREATE_KEYS);
//	m_Sections.push_back( *(new t_Section) );
}

// ~CDataFile
// Saves the file if any values have changed since the last save.
CDataFile::~CDataFile()
{
  Close ();
}

// SetFileName
// Set's the m_szFileName member variable. For use when creating the CDataFile
// object by hand (-vs- loading it from a file
void CDataFile::SetFileName(t_Str szFileName)
{
  if(CompareNoCase(szFileName, m_szFileName) != 0)
  {
    Close ();
  }

  m_szFileName = szFileName;
}

// Load without locking just fro read
bool CDataFile::Load()
{
  return Load(false);
}

// Load with locking in order to write the file
bool CDataFile::LoadAndLock()
{
 return Load(true);
}

// Load
// Attempts to load in the text file. If successful it will populate the 
// Section list with the key/value pairs found in the file. Note that comments
// are saved so that they can be rewritten to the file later.
bool CDataFile::Load(bool bLock)
{
  if ( m_szFileName.size() == 0 )
    return false;

  if ( m_stream )
    return true; //The file is already open => nothing to do

  CAutoMutex autoMutex(&sm_Mutex);
  //We come here if the file is not open yet (m_stream==NULL)
  //If the file is opened, it should be locked 
	
  size_t noOfKeys = KeyCount();
  size_t noOfSections = SectionCount();

  int err=0;

  do
  {
#ifdef WIN32
    err = fopen_s(&m_stream, utilStringNarrow(m_szFileName).c_str(), "r");
#else
    err=0;
    m_stream = fopen(utilStringNarrow(m_szFileName).c_str(), "r");
    if (m_stream == NULL) err=errno;		
#endif
    if (err != 0 && err != EACCES && err != ENOENT ) return false;							// Added for unit testing

    if(err==EACCES) 
    {
      CThread::SleepMillisecs(100);
    }
    else if(err==ENOENT && bLock)
    {
#ifdef WIN32
      err = fopen_s(&m_stream, utilStringNarrow(m_szFileName).c_str(), "w");
#else
      err=0;
      m_stream = fopen(utilStringNarrow(m_szFileName).c_str(), "w");
      if (m_stream == NULL) err=errno;		
#endif
      if (err != 0 && err != EACCES && err != ENOENT ) return false;	

      if(m_stream) 
      {
        fclose(m_stream);
        m_stream=NULL;

        err=EACCES;	//To re-open the file in read mode
      }
      else
      {
        return false;
      }
    }
  } while(err==EACCES);

  //If the lock is not needed, we don't have to create the file
  if(m_stream==NULL)
    return false;

  if(bLock)
  {
#ifdef WIN32
    _lock_file(m_stream);		//Lock the file to avoid other process to access it
#else
    // on Linux/Mac we set an advisory lock, i.e. it prevents
    // other processes from using the file only if they are collaborative 
    // and check for the lock, otherwise they can do whatever they like ..

    m_tFl.l_whence = SEEK_SET; /* SEEK_SET, SEEK_CUR, SEEK_END */
    m_tFl.l_start  = 0;        /* Offset from l_whence         */
    m_tFl.l_len    = 0;        /* length, 0 = to EOF           */
    m_tFl.l_pid    = getpid(); /* our PID                      */
    m_tFl.l_type   = F_RDLCK;  /* F_RDLCK, F_WRLCK, F_UNLCK    */
	
    if( fcntl(fileno(m_stream), F_SETLKW, &m_tFl) == -1)
    {  /* set the lock, waiting if necessary */
      printf("datafile::Load: fcntl %s\n",strerror(errno));
      exit(1);
    }
#endif
  }

  bool bDone = false;
  bool bAutoKey = (m_Flags & AUTOCREATE_KEYS) == AUTOCREATE_KEYS;
  bool bAutoSec = (m_Flags & AUTOCREATE_SECTIONS) == AUTOCREATE_SECTIONS;
	
  t_Str szLine;
  t_Str szComment;
  wchar_t buffer[MAX_BUFFER_LEN]; 
  t_Section* pSection = GetSection(L"");

  // These need to be set, we'll restore the original values later.
  m_Flags |= AUTOCREATE_KEYS;
  m_Flags |= AUTOCREATE_SECTIONS;

  while ( !bDone )
    {
      memset(buffer, 0, MAX_BUFFER_LEN);
      wchar_t* tmp = fgetws( buffer, MAX_BUFFER_LEN, m_stream);
      tmp=tmp;		// avoid warning

      szLine = buffer;
      Trim(szLine);

      bDone = ferror( m_stream ) || feof(m_stream);

      if (!bDone)
	{

	  if ( szLine.find_first_of(CommentIndicators) == 0 )
	    {
	      szComment = L"\n";
	      szComment += szLine;
	    }
	  else
	    if ( szLine.find_first_of('[') == 0 ) // new section
	      {
		szLine.erase( 0, 1 );
		szLine.erase( szLine.find_last_of(']'), 1 );
		CreateSectionInt(szLine, szComment);
		pSection = GetSectionInt(szLine);
		szComment = t_Str(L"");
	      }
	    else 
	      if ( szLine.size() > 0 ) // we have a key, add this key/value pair
		{
		  t_Str szKey = GetNextWord(szLine);
		  t_Str szValue = szLine;

		  if ( szKey.size() > 0 && szValue.size() > 0 )
		    {
		      SetValueInt(szKey, szValue, szComment, pSection == NULL ? szLine : pSection->szName);
		      szComment = t_Str(L"");
		    }
		}
	}
  }

 // Restore the original flag values.
  if ( !bAutoKey )
    m_Flags &= ~AUTOCREATE_KEYS;

  if ( !bAutoSec )
    m_Flags &= ~AUTOCREATE_SECTIONS;

  if(bLock)
  {
//Reopen for writing and keep open for locking
#ifdef WIN32
    err = freopen_s(&m_stream, utilStringNarrow(m_szFileName).c_str(), "w",m_stream);
    if (err != 0) return false;							
#else
    m_stream = freopen(utilStringNarrow(m_szFileName).c_str(), "w",m_stream);
    if (m_stream == NULL) return false;	

    m_tFl.l_type   = F_UNLCK;  /* F_RDLCK, F_WRLCK, F_UNLCK    */
    if( fcntl(fileno(m_stream), F_SETLKW, &m_tFl) == -1)
    {  /* set the lock, waiting if necessary */
      printf("datafile::Load: fcntl %s\n",strerror(errno));
      exit(1);
    }
    m_tFl.l_type   = F_WRLCK;  /* F_RDLCK, F_WRLCK, F_UNLCK    */
    if( fcntl(fileno(m_stream), F_SETLKW, &m_tFl) == -1)
    {  /* set the lock, waiting if necessary */
      printf("datafile::Load: fcntl %s\n",strerror(errno));
      exit(1);
    }
#endif
  }
  else
  {
    fclose(m_stream);
    m_stream=NULL;
  }

  noOfKeys = KeyCount();
  noOfSections = SectionCount();

  return true;
}


// Save
// Attempts to save the Section list and keys to the file. Note that if Load
// was never called (the CDataFile object was created manually), then you
// must set the m_szFileName variable before calling save.
bool CDataFile::Save()
{
  if(m_szFileName.size() == 0)
     return false;

  if(!m_stream)
    return false;

  CAutoMutex autoMutex(&sm_Mutex);

  size_t noOfKeys = KeyCount();
  size_t noOfSections = SectionCount();
  if ( noOfKeys == 0 && noOfSections == 0 )
    {
      // no point in saving
      return false; 
    }

  //std::wofstream outStream(utilStringNarrow(m_szFileName).c_str());
  std::wstringstream outStream(m_szFileName.c_str());
  if( ! outStream) return false;

  SectionItor s_pos;
  KeyItor k_pos;
  t_Section Section;
  t_Key Key;

  for (s_pos = m_Sections.begin(); s_pos != m_Sections.end(); s_pos++)
    {
      Section = (*s_pos);
      bool bWroteComment = false;

      if ( Section.szComment.size() > 0 )
	{
	  bWroteComment = true;
	  outStream<<"\n"<<CommentStr(Section.szComment)<<"\n";
	  //	  WriteLn(outStream, L"\n%ls", CommentStr(Section.szComment).c_str());
	}

      if ( Section.szName.size() > 0 )
	{
	  outStream<< (bWroteComment ? L"" : L"\n")<<
	    "["<<Section.szName.c_str()<<"]"<<"\n";
	}

      for (k_pos = Section.Keys.begin(); k_pos != Section.Keys.end(); k_pos++)
	{
	  Key = (*k_pos);

	  if ( Key.szKey.size() > 0 && Key.szValue.size() > 0 )
	    {
	      outStream<<(Key.szComment.size() > 0 ? L"\n" : L"")
		       <<CommentStr(Key.szComment)
		       <<(Key.szComment.size() > 0 ? L"\n" : L"")
		       <<Key.szKey
		       <<EqualIndicators[0]
		       <<Key.szValue
		       <<"\n";
	    }
	}
    }
	
  outStream.flush();
  //outStream.close();

#ifdef WIN32
  fprintf_s(m_stream,"%ls",outStream.str().c_str());
#else
  fprintf(m_stream,"%ls",outStream.str().c_str());
#endif

#ifdef WIN32
  _unlock_file(m_stream);
#else	
  m_tFl.l_type   = F_UNLCK;  /* tell it to unlock the region */
  if( fcntl(fileno(m_stream), F_SETLKW, &m_tFl) == -1)
  {  /* set the lock, waiting if necessary */
    printf("CDataFile::Close: fcntl %s\n",strerror(errno));
    exit(1);
  }
#endif

  fclose(m_stream);
  m_stream=NULL;

  m_bDirty = false;

  return true;
}

bool CDataFile::Close()
{
  if(m_stream)
  {
    return Save();
  }
  else
  {
    return false;
  }
}

// SetKeyComment
// Set the comment of a given key. Returns true if the key is not found.
bool CDataFile::SetKeyComment(t_Str szKey, t_Str szComment, t_Str szSection)
{
	if(!LoadAndLock())
		return false;

	KeyItor k_pos;
	t_Section* pSection;

	if ( (pSection = GetSection(szSection)) == NULL )
		return false;

	for (k_pos = pSection->Keys.begin(); k_pos != pSection->Keys.end(); k_pos++)
	{
		if ( CompareNoCase( (*k_pos).szKey, szKey ) == 0 )
		{
			(*k_pos).szComment = szComment;
			m_bDirty = true;
			return true;
		}
	}

	return false;

}

// SetSectionComment
// Set the comment for a given section. Returns false if the section
// was not found.
bool CDataFile::SetSectionComment(t_Str szSection, t_Str szComment)
{
	if(!LoadAndLock())
		return false;

	SectionItor s_pos;

	for (s_pos = m_Sections.begin(); s_pos != m_Sections.end(); s_pos++)
	{
		if ( CompareNoCase( (*s_pos).szName, szSection ) == 0 ) 
		{
		    (*s_pos).szComment = szComment;
			m_bDirty = true;
			return true;
		}
	}

	return false;
}


// SetValue
// Given a key, a value and a section, this function will attempt to locate the
// Key within the given section, and if it finds it, change the keys value to
// the new value. If it does not locate the key, it will create a new key with
// the proper value and place it in the section requested.
bool CDataFile::SetValue(t_Str szKey, t_Str szValue, t_Str szComment, t_Str szSection)
{
	if(!LoadAndLock())
		return false;

	return SetValueInt(szKey,szValue,szComment,szSection);
}

bool CDataFile::SetValueInt(t_Str szKey, t_Str szValue, t_Str szComment, t_Str szSection)
{
        t_Key* pKey = GetKey(szKey, szSection);
	t_Section* pSection = GetSectionInt(szSection);

	if (pSection == NULL)
	{
		if ( !(m_Flags & AUTOCREATE_SECTIONS) || !CreateSectionInt(szSection,L""))
			return false;

		pSection = GetSectionInt(szSection);
	}

	// Sanity check...
	if ( pSection == NULL )
		return false;

	// if the key does not exist in that section, and the value passed 
	// is not t_Str(L"") then add the new key.
	if ( pKey == NULL && szValue.size() > 0 && (m_Flags & AUTOCREATE_KEYS))
	{
//		pKey = new t_Key;
//
//		pKey->szKey = szKey;
//		pKey->szValue = szValue;
//		pKey->szComment = szComment;
//		
//		m_bDirty = true;
//		
//		pSection->Keys.push_back(*pKey);
//
//		delete pKey;

		t_Key key(szKey,szValue,szComment);
		pSection->Keys.push_back(key);

		m_bDirty = true;

		return true;
	}

	if ( pKey != NULL )
	{
		pKey->szValue = szValue;
		pKey->szComment = szComment;

		m_bDirty = true;
		
		return true;
	}

	return false;
}

// SetFloat
// Passes the given float to SetValue as a string
bool CDataFile::SetFloat(t_Str szKey, float fValue, t_Str szComment, t_Str szSection)
{
	char szStr[64];

	_snprintf_s(szStr, 64, "%f", fValue);

	return SetValue(szKey, utilStringWiden(szStr), szComment, szSection);
}

// SetInt
// Passes the given int to SetValue as a string 
bool CDataFile::SetInt(t_Str szKey, int nValue, t_Str szComment, t_Str szSection)
{
	char szStr[64];

	_snprintf_s(szStr, 64, "%d", nValue);

	return SetValue(szKey, utilStringWiden(szStr), szComment, szSection);

}

// SetLong
// Passes the given long to SetValue as a string 
bool CDataFile::SetLong(t_Str szKey, long nValue, t_Str szComment, t_Str szSection)
{
	char szStr[64];

	_snprintf_s(szStr, 64, "%ld", nValue);

	return SetValue(szKey, utilStringWiden(szStr), szComment, szSection);

}

// SetBool
// Passes the given bool to SetValue as a string
bool CDataFile::SetBool(t_Str szKey, bool bValue, t_Str szComment, t_Str szSection)
{
	t_Str szValue = bValue ?  L"True" : L"False";

	return SetValue(szKey, szValue, szComment, szSection);
}

// GetValue
// Returns the key value as a t_Str object. A return value of
// t_Str(L"") indicates that the key could not be found.
t_Str CDataFile::GetValue(t_Str szKey, t_Str szSection) 
{
	t_Key* pKey = GetKey(szKey, szSection);

	return (pKey == NULL) ? t_Str(L"") : pKey->szValue;
}

// GetString
// Returns the key value as a t_Str object. A return value of
// t_Str(L"") indicates that the key could not be found.
t_Str CDataFile::GetString(t_Str szKey, t_Str szSection)
{
	return GetValue(szKey, szSection);
}

// GetFloat
// Returns the key value as a float type. Returns FLT_MIN if the key is
// not found.
float CDataFile::GetFloat(t_Str szKey, t_Str szSection)
{
	t_Str szValue = GetValue(szKey, szSection);

	if ( szValue.size() == 0 )
		return FLT_MIN;

	return (float)atof( utilStringNarrow(szValue).c_str() );
}

// GetInt
// Returns the key value as an integer type. Returns INT_MIN if the key is
// not found.
int	CDataFile::GetInt(t_Str szKey, t_Str szSection)
{
	t_Str szValue = GetValue(szKey, szSection);

	if ( szValue.size() == 0 )
		return INT_MIN;

	return atoi( utilStringNarrow(szValue).c_str() );
}

// GetLong
// Returns the key value as a long type. Returns LONG_MIN if the key is
// not found.
long CDataFile::GetLong(t_Str szKey, t_Str szSection)
{
	t_Str szValue = GetValue(szKey, szSection);

	if ( szValue.size() == 0 )
		return LONG_MIN;

	return atol( utilStringNarrow(szValue).c_str() );
}

// GetBool
// Returns the key value as a bool type. Returns false if the key is
// not found.
bool CDataFile::GetBool(t_Str szKey, t_Str szSection)
{
	bool bValue = false;
	t_Str szValue = GetValue(szKey, szSection);

	if ( szValue.find(L"1") == 0 
		|| CompareNoCase(szValue, L"true") 
		|| CompareNoCase(szValue, L"yes") )
	{
		bValue = true;
	}

	return bValue;
}

// DeleteSection
// Delete a specific section. Returns false if the section cannot be 
// found or true when sucessfully deleted.
bool CDataFile::DeleteSection(t_Str szSection)
{
	if(!LoadAndLock())
		return false;

	SectionItor s_pos;

	for (s_pos = m_Sections.begin(); s_pos != m_Sections.end(); s_pos++)
	{
	  if ( (*s_pos).szName.size() != 0 && CompareNoCase( (*s_pos).szName, szSection ) == 0 ) 
		{
			m_Sections.erase(s_pos);
			return true;
		}
	}

	return false;
}

// DeleteKey
// Delete a specific key in a specific section. Returns false if the key
// cannot be found or true when sucessfully deleted.
bool CDataFile::DeleteKey(t_Str szKey, t_Str szFromSection)
{
	if(!LoadAndLock())
		return false;

	KeyItor k_pos;
	t_Section* pSection;

	if ( (pSection = GetSection(szFromSection)) == NULL )
		return false;

	for (k_pos = pSection->Keys.begin(); k_pos != pSection->Keys.end(); k_pos++)
	{
	  if ( (*k_pos).szKey.size() != 0 && CompareNoCase( (*k_pos).szKey, szKey ) == 0 )
		{
			pSection->Keys.erase(k_pos);
			return true;
		}
	}

	return false;
}

// CreateKey
// Given a key, a value and a section, this function will attempt to locate the
// Key within the given section, and if it finds it, change the keys value to
// the new value. If it does not locate the key, it will create a new key with
// the proper value and place it in the section requested.
bool CDataFile::CreateKey(t_Str szKey, t_Str szValue, t_Str szComment, t_Str szSection)
{
	bool bAutoKey = (m_Flags & AUTOCREATE_KEYS) == AUTOCREATE_KEYS;
	bool bReturn  = false;

	m_Flags |= AUTOCREATE_KEYS;

	bReturn = SetValue(szKey, szValue, szComment, szSection);

	if ( !bAutoKey )
		m_Flags &= ~AUTOCREATE_KEYS;

	return bReturn;
}


// CreateSection
// Given a section name, this function first checks to see if the given section
// already exists in the list or not, if not, it creates the new section and
// assigns it the comment given in szComment.  The function returns true if
// successfully created, or false otherwise. 
bool CDataFile::CreateSection(t_Str szSection, t_Str szComment)
{
	if(!LoadAndLock())
		return false;

	return CreateSection(szSection,szComment);
}

bool CDataFile::CreateSectionInt(t_Str szSection, t_Str szComment)
{

	t_Section* pSection = GetSectionInt(szSection);

	if ( pSection )
	{
		return false;
	}

//	pSection = new t_Section;
//
//	pSection->szName = szSection;
//	pSection->szComment = szComment;
//	m_Sections.push_back(*pSection);
//
//	delete pSection;

	t_Section section;
	section.szName 	  = szSection;
	section.szComment = szComment;
	m_Sections.push_back(section);

	m_bDirty = true;

	return true;
}

// CreateSection
// Given a section name, this function first checks to see if the given section
// already exists in the list or not, if not, it creates the new section and
// assigns it the comment given in szComment.  The function returns true if
// successfully created, or false otherwise. This version accpets a KeyList 
// and sets up the newly created Section with the keys in the list.
bool CDataFile::CreateSection(t_Str szSection, t_Str szComment, KeyList Keys)
{
	if(!LoadAndLock())
		return false;

	if ( !CreateSection(szSection, szComment) )
		return false;

	t_Section* pSection = GetSection(szSection);

	if ( !pSection )
		return false;

	KeyItor k_pos;

	pSection->szName = szSection;
	for (k_pos = Keys.begin(); k_pos != Keys.end(); k_pos++)
	{
		t_Key pKey;
    
		pKey.szComment = (*k_pos).szComment;
		pKey.szKey = (*k_pos).szKey;
		pKey.szValue = (*k_pos).szValue;

		pSection->Keys.push_back(pKey);
	}

	m_Sections.push_back(*pSection);
	m_bDirty = true;

	return true;
}

// SectionCount
// Simply returns the number of sections in the list.
size_t CDataFile::SectionCount() 
{ 
	return m_Sections.size(); 
}

// KeyCount
// Returns the total number of keys contained within all the sections.
size_t CDataFile::KeyCount()
{
	size_t nCounter = 0;
	SectionItor s_pos;

	for (s_pos = m_Sections.begin(); s_pos != m_Sections.end(); s_pos++)
		nCounter += (*s_pos).Keys.size();

	return nCounter;
}


// Protected Member Functions ///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

// GetKey
// Given a key and section name, looks up the key and if found, returns a
// pointer to that key, otherwise returns NULL.
t_Key*	CDataFile::GetKey(t_Str szKey, t_Str szSection)
{
	Load();

	KeyItor k_pos;
	t_Section* pSection;

	// Since our default section has a name value of t_Str(L"") this should
	// always return a valid section, wether or not it has any keys in it is
	// another matter.
	if ( (pSection = GetSection(szSection)) == NULL )
		return NULL;

	for (k_pos = pSection->Keys.begin(); k_pos != pSection->Keys.end(); k_pos++)
	{
		if ( CompareNoCase( (*k_pos).szKey, szKey ) == 0 )
			return (t_Key*)&(*k_pos);
	}

	return NULL;
}

// GetSection
// Given a section name, locates that section in the list and returns a pointer
// to it. If the section was not found, returns NULL
t_Section* CDataFile::GetSection(t_Str szSection)
{
	Load();

	return GetSectionInt(szSection);
}

t_Section* CDataFile::GetSectionInt(t_Str szSection)
{
	SectionItor s_pos;
	for (s_pos = m_Sections.begin(); s_pos != m_Sections.end(); s_pos++)
	{
	  if ( (*s_pos).szName.size() != 0 && CompareNoCase( (*s_pos).szName, szSection ) == 0 )
			return (t_Section*)&(*s_pos);
	}
	return NULL;
}


t_Str CDataFile::CommentStr(t_Str szComment)
{
	t_Str szNewStr = t_Str(L"");

	Trim(szComment);

        if ( szComment.size() == 0 )
          return szComment;
	
	if ( szComment.find_first_of(CommentIndicators) != 0 )
	{
		szNewStr = CommentIndicators[0];
		szNewStr += L" ";
	}

	szNewStr += szComment;

	return szNewStr;
}



// Utility Functions ////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

// GetNextWord
// Given a key +delimiter+ value string, pulls the key name from the string,
// deletes the delimiter and alters the original string to contain the
// remainder.  Returns the key
t_Str GetNextWord(t_Str& CommandLine)
{
	size_t nPos = CommandLine.find_first_of(EqualIndicators);
	t_Str sWord = t_Str(L"");

	if ( nPos > 0 )
	{
		sWord = CommandLine.substr(0, nPos);
		CommandLine.erase(0, nPos+1);
	}
	else
	{
		sWord = CommandLine;
		CommandLine = t_Str(L"");
	}

	Trim(sWord);
	return sWord;
}


// CompareNoCase
// it's amazing what features std::string lacks.  This function simply
// does a lowercase compare against the two strings, returning 0 if they
// match.
int CompareNoCase(t_Str str1, t_Str str2)
{
#ifdef WIN32
  return _stricmp(utilStringNarrow(str1).c_str(), utilStringNarrow(str2).c_str());	
#else
  return strcasecmp(utilStringNarrow(str1).c_str(), utilStringNarrow(str2).c_str());
#endif
}

// Trim
// Trims whitespace from both sides of a string.
void Trim(t_Str& szStr)
{
	t_Str szTrimChars = WhiteSpace;
	
	szTrimChars += EqualIndicators;
	size_t nPos, rPos;

	// trim left
	nPos = szStr.find_first_not_of(szTrimChars);

	if ( nPos > 0 )
		szStr.erase(0, nPos);

	// trim right and return
	nPos = szStr.find_last_not_of(szTrimChars) + 1;
	rPos = szStr.find_last_of(szTrimChars);

	if ( rPos >= nPos && rPos > 0)
	{
		szStr.erase(nPos, rPos);
	}
}

// WriteLn
// Writes the formatted output to the file stream, returning the number of
// bytes written.
int WriteLn(FILE * stream, wchar_t* fmt, ...)
{
    if(!stream)
        return 0;

	wchar_t buf[MAX_BUFFER_LEN];
	int nLength;
	t_Str szMsg;

	memset(buf, 0, MAX_BUFFER_LEN);
	va_list args;

	va_start (args, fmt);
#ifdef WIN32
#pragma message( "*** Check parameters for vsnprintf_s() ..." __FILE__ )
	  nLength = _vsnwprintf_s(buf, MAX_BUFFER_LEN, MAX_BUFFER_LEN, fmt, args);
#else
	  nLength = vswprintf(buf, MAX_BUFFER_LEN, fmt, args);
#endif
	va_end (args);


	if ( buf[nLength] != '\n' && buf[nLength] != '\r' )
		buf[nLength++] = '\n';

    size_t tmp = fwrite(buf, sizeof( wchar_t ), nLength, stream);
    tmp = tmp;	// avoid warning

	return nLength;
}


} // namespace eIDMW
