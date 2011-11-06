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

#ifndef __CDATAFILE_H__
#define __CDATAFILE_H__

#include <vector>
#include <string>
#include "Mutex.h"

#ifndef WIN32
#include <fcntl.h>
#endif

namespace eIDMW
{

// Globally defined structures, defines, & types
//////////////////////////////////////////////////////////////////////////////////

// AUTOCREATE_SECTIONS
// When set, this define will cause SetValue() to create a new section, if
// the requested section does not allready exist.
#define AUTOCREATE_SECTIONS     (1L<<1)

// AUOTCREATE_KEYS
// When set, this define causes SetValue() to create a new key, if the
// requested key does not allready exist.
#define AUTOCREATE_KEYS         (1L<<2)

// MAX_BUFFER_LEN
// Used simply as a max size of some internal buffers. Determines the maximum
// length of a line that will be read from or written to the file or the
// report output.
#define MAX_BUFFER_LEN				512

typedef std::wstring t_Str;


// CommentIndicators
// This constant contains the characters that we check for to determine if a 
// line is a comment or not. Note that the first character in this constant is
// the one used when writing comments to disk (if the comment does not allready
// contain an indicator)
#define CommentIndicators  t_Str(L";#")

// EqualIndicators
// This constant contains the characters that we check against to determine if
// a line contains an assignment ( key = value )
// Note that changing these from their defaults ("=:") WILL affect the
// ability of CDataFile to read/write to .ini files.  Also, note that the
// first character in this constant is the one that is used when writing the
// values to the file. (EqualIndicators[0])
#define EqualIndicators   t_Str(L"=:") 

// WhiteSpace
// This constant contains the characters that the Trim() function removes from
// the head and tail of strings.
#define WhiteSpace  t_Str(L" \t\n\r")

// st_key
// This structure stores the definition of a key. A key is a named identifier
// that is associated with a value. It may or may not have a comment.  All comments
// must PRECEDE the key on the line in the config file.
typedef struct st_key
{
	t_Str		szKey;
	t_Str		szValue;
	t_Str		szComment;

	st_key()
		: szKey(L"")
		, szValue(L"")
		, szComment(L"")
	{
	}
	st_key( t_Str const& key, t_Str const& value, t_Str const& comment)
		: szKey(key)
		, szValue(value)
		, szComment(comment)
	{
	}

} t_Key;

typedef std::vector<t_Key> KeyList;
typedef KeyList::iterator KeyItor;

// st_section
// This structure stores the definition of a section. A section contains any number
// of keys (see st_keys), and may or may not have a comment. Like keys, all
// comments must precede the section.
typedef struct st_section
{
	t_Str		szName;
	t_Str		szComment;
	KeyList		Keys;

	st_section()
	{
		szName = t_Str(L"");
		szComment = t_Str(L"");
		Keys.clear();
	}

} t_Section;

typedef std::vector<t_Section> SectionList;
typedef SectionList::iterator SectionItor;



/// General Purpose Utility Functions ///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
t_Str	GetNextWord(t_Str& CommandLine);
int		CompareNoCase(t_Str str1, t_Str str2);
void	Trim(t_Str& szStr);
int		WriteLn(FILE *stream, wchar_t* fmt, ...);


/// Class Definitions ///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


// CDataFile
class CDataFile
{
// Methods
public:
				// Constructors & Destructors
				/////////////////////////////////////////////////////////////////
				CDataFile();
				CDataFile(t_Str szFileName);
	virtual		~CDataFile();

				// File handling methods
				/////////////////////////////////////////////////////////////////
	bool		Load();		//Load m_szFileName without lock
	bool		LoadAndLock();	//Load m_szFileName with lock
	bool		Save();
	bool		Close(); //Close the file

				// Data handling methods
				/////////////////////////////////////////////////////////////////

				// GetValue: Our default access method. Returns the raw t_Str value
				// Note that this returns keys specific to the given section only.
	t_Str		GetValue(t_Str szKey, t_Str szSection = t_Str(L"")); 
				// GetString: Returns the value as a t_Str
	t_Str		GetString(t_Str szKey, t_Str szSection = t_Str(L"")); 
				// GetFloat: Return the value as a float
	float		GetFloat(t_Str szKey, t_Str szSection = t_Str(L""));
				// GetInt: Return the value as an int
	int			GetInt(t_Str szKey, t_Str szSection = t_Str(L""));
				// GetInt: Return the value as a long
	long 		GetLong(t_Str szKey, t_Str szSection = t_Str(L""));
				// GetBool: Return the value as a bool
	bool		GetBool(t_Str szKey, t_Str szSection = t_Str(L""));

				// SetValue: Sets the value of a given key. Will create the
				// key if it is not found and AUTOCREATE_KEYS is active.
	bool		SetValue(t_Str szKey, t_Str szValue, 
						 t_Str szComment = t_Str(L""), t_Str szSection = t_Str(L""));

				// SetFloat: Sets the value of a given key. Will create the
				// key if it is not found and AUTOCREATE_KEYS is active.
	bool		SetFloat(t_Str szKey, float fValue, 
						 t_Str szComment = t_Str(L""), t_Str szSection = t_Str(L""));

				// SetInt: Sets the value of a given key. Will create the
				// key if it is not found and AUTOCREATE_KEYS is active.
	bool		SetInt(t_Str szKey, int nValue, 
						 t_Str szComment = t_Str(L""), t_Str szSection = t_Str(L""));

				// SetLong: Sets the value of a given key. Will create the
				// key if it is not found and AUTOCREATE_KEYS is active.
	bool		SetLong(t_Str szKey, long nValue, 
				t_Str szComment = t_Str(L""), t_Str szSection = t_Str(L""));

				// SetBool: Sets the value of a given key. Will create the
				// key if it is not found and AUTOCREATE_KEYS is active.
	bool		SetBool(t_Str szKey, bool bValue, 
						 t_Str szComment = t_Str(L""), t_Str szSection = t_Str(L""));

				// Sets the comment for a given key.
	bool		SetKeyComment(t_Str szKey, t_Str szComment, t_Str szSection = t_Str(L""));

				// Sets the comment for a given section
	bool		SetSectionComment(t_Str szSection, t_Str szComment);

				// DeleteKey: Deletes a given key from a specific section
	bool		DeleteKey(t_Str szKey, t_Str szFromSection = t_Str(L""));

				// DeleteSection: Deletes a given section.
	bool		DeleteSection(t_Str szSection);
				
				// Key/Section handling methods
				/////////////////////////////////////////////////////////////////

				// CreateKey: Creates a new key in the requested section. The
	            // Section will be created if it does not exist and the 
				// AUTOCREATE_SECTIONS bit is set.
	bool		CreateKey(t_Str szKey, t_Str szValue, 
		                  t_Str szComment = t_Str(L""), t_Str szSection = t_Str(L""));
				// CreateSection: Creates the new section if it does not allready
				// exist. Section is created with no keys.
	bool		CreateSection(t_Str szSection, t_Str szComment = t_Str(L""));
				// CreateSection: Creates the new section if it does not allready
				// exist, and copies the keys passed into it into the new section.
	bool		CreateSection(t_Str szSection, t_Str szComment, KeyList Keys);

				// Utility Methods
				/////////////////////////////////////////////////////////////////
				// SectionCount: Returns the number of valid sections in the database.
	size_t		SectionCount();
				// KeyCount: Returns the total number of keys, across all sections.
	size_t		KeyCount();
				// SetFileName: For use when creating the object by hand
				// initializes the file name so that it can be later saved.
	void		SetFileName(t_Str szFileName);
				// CommentStr
				// Parses a string into a proper comment token/comment.
	t_Str		CommentStr(t_Str szComment);				

				// GetSection: Returns the requested section (if found), NULL otherwise.
	t_Section*	GetSection(t_Str szSection);

protected:
	t_Section*	GetSectionInt(t_Str szSection);
	bool		CreateSectionInt(t_Str szSection, t_Str szComment);
	bool		SetValueInt(t_Str szKey, t_Str szValue, 
						 t_Str szComment = t_Str(L""), t_Str szSection = t_Str(L""));

				// Note: I've tried to insulate the end user from the internal
				// data structures as much as possible. This is by design. Doing
				// so has caused some performance issues (multiple calls to a
				// GetSection() function that would otherwise not be necessary,etc).
				// But, I believe that doing so will provide a safer, more stable
				// environment. You'll notice that nothing returns a reference,
				// to modify the data values, you have to call member functions.
				// think carefully before changing this.

				// GetKey: Returns the requested key (if found) from the requested
				// Section. Returns NULL otherwise.
	t_Key*		GetKey(t_Str szKey, t_Str szSection);


	FILE*		m_stream;
	static CMutex 		sm_Mutex;	/**< Mutex for exclusive access */

#ifndef WIN32
	struct flock m_tFl;
#endif

// Data
public:
	long		m_Flags;		// Our settings flags.

protected:
	bool		Load(bool bLock);

	SectionList	m_Sections;		// Our list of sections
	t_Str		m_szFileName;	// The filename to write to
	bool		m_bDirty;		// Tracks whether or not data has changed.
};


}  // namespace eIDMW

#endif
