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
#include "MiscUtil.h"

#ifdef WIN32
#include <Windows.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "MWException.h"
#include "eidErrors.h"
#include "ByteArray.h"


namespace eIDMW
{

/*****************************************************************************************
------------------------------------ CTimestampUtil ---------------------------------------
*****************************************************************************************/
void CTimestampUtil::getTimestamp(std::string &timestamp,long delay,const char *format)
{
	time_t rawtime;
	struct tm timeinfo;
	char buffer [20];

	time ( &rawtime );
	rawtime+=delay;
#ifdef WIN32
	localtime_s(&timeinfo, &rawtime );
#else
	timeinfo = *(localtime(&rawtime ));
#endif

	strftime (buffer,20,format,&timeinfo);

	timestamp.assign(buffer);
}

bool CTimestampUtil::checkTimestamp(std::string &timestamp,const char *format)
{
	//The line is not valid if timestamp expired
	std::string now;

	getTimestamp(now,0L,format);

	if(now.compare(timestamp)<0)
		return true;
	else
		return false;

}

/*****************************************************************************************
------------------------------------ CPathUtil ---------------------------------------
*****************************************************************************************/
std::string CPathUtil::getWorkingDir()
{
	char *directory=NULL;;

#ifdef WIN32
	DWORD lLen=GetCurrentDirectoryA(0,directory);
	directory=new char[lLen+1];
	GetCurrentDirectoryA(lLen+1,directory);
#else
	// TODO: check if this is OK??
	directory = new char[2];
	directory[0] = '.';
	directory[1] = '\0';
#endif

	std::string strDirectory=directory;

	if(directory)
		delete[] directory;

	return strDirectory;
}

#ifdef WIN32
#define PATH_SEP_STR    "\\"
#define PATH_SEP_CHAR   '\\'
#else
#define PATH_SEP_STR    "/"
#define PATH_SEP_CHAR   '/'
#endif

std::string CPathUtil::getDir(const char *filePath)
{
	char *directory=new char[strlen(filePath)+1];
	std::string strDirectory;

#ifdef WIN32
	strcpy_s(directory,strlen(filePath)+1,filePath);
#else
	strcpy(directory,filePath);
#endif

	if(strlen(directory)>1)
	{
		bool bFound=false;

		for(char *pStr=directory+strlen(directory)-2;pStr!=directory;pStr--)
		{
			if(*pStr==PATH_SEP_CHAR)
			{
				// In case of multiple path separators, e.g. /tmp//crl
				for ( ;pStr != directory && *pStr==PATH_SEP_CHAR; pStr--)
					*pStr=0;
				bFound= pStr != directory;
				break;
			}
		}
		if(bFound)
			strDirectory=directory;
	}

	delete[] directory;

	return strDirectory;
}

bool CPathUtil::existFile(const char *filePath)
{
	if(strlen(filePath)==0)
		return false;

#ifdef WIN32
	DWORD dwError = 0;
	DWORD dwAttr = GetFileAttributesA(filePath);
	if(dwAttr == INVALID_FILE_ATTRIBUTES) dwError = GetLastError();
	if(dwError == ERROR_FILE_NOT_FOUND || dwError == ERROR_PATH_NOT_FOUND)
	{
#else		
	struct stat buffer;
	if ( stat(filePath,&buffer))
	{
#endif
		return false;
	}

	return true;
}

void CPathUtil::checkDir(const char *dirIn)
{
	if(strlen(dirIn)==0)
		return;

	std::string directory = std::string(dirIn) + PATH_SEP_STR;
#ifdef WIN32
	DWORD dwError = 0;
	DWORD dwAttr = GetFileAttributesA(directory.c_str());
	if(dwAttr == INVALID_FILE_ATTRIBUTES) dwError = GetLastError();
	if(dwError == ERROR_FILE_NOT_FOUND || dwError == ERROR_PATH_NOT_FOUND)
	{
#else		
	struct stat buffer;
	if ( stat(dirIn,&buffer))
	{
#endif
		char *parentDir=new char[directory.size()+1];
#ifdef WIN32
		strcpy_s(parentDir,directory.size()+1,directory.c_str());
#else
		strcpy(parentDir,directory.c_str());
#endif
		if(strlen(parentDir)>1)
		{
			bool bFound=false;

			for(char *pStr=parentDir+strlen(parentDir)-2;pStr!=parentDir;pStr--)
			{
				if(*pStr==PATH_SEP_CHAR)
				{
					// In case of multiple path separators, e.g. /tmp//crl
					for ( ;pStr != parentDir && *pStr==PATH_SEP_CHAR; pStr--)
						*pStr=0;
					bFound= pStr != parentDir;
					break;
				}
			}
			if(bFound)
				checkDir(parentDir);
		}

		delete[] parentDir;
#ifdef WIN32
		dwError=NO_ERROR;
		if(!CreateDirectoryA(directory.c_str(),NULL))
			dwError=GetLastError();
		if(dwError!=NO_ERROR && dwError!=ERROR_ALREADY_EXISTS)
#else
		// set read/write/search permissions for everyone.
		if( mkdir(directory.c_str(),S_IRWXU | S_IRWXG | S_IRWXO) != 0)
#endif
		{
			printf("The path '%s' does not exist.\nCreate it or change the config parameter\n",dirIn);
			throw CMWEXCEPTION(EIDMW_INVALID_PATH);
		}
	}
}

#ifdef WIN32
void CPathUtil::scanDir(const char *Dir,const char *SubDir,const char *Ext,bool &bStopRequest,void *param,void (* callback)(const char *SubDir, const char *File, void *param))
{
	WIN32_FIND_DATAA FindFileData; 
	std::string path;
	std::string subdir;
	HANDLE hFind;				  
	DWORD a = 0;				  

	path=Dir;
	path+="\\*.*";

	//Get the first file
    hFind = FindFirstFileA(path.c_str(), &FindFileData);
    if (hFind==INVALID_HANDLE_VALUE)
        return;

	while (a != ERROR_NO_MORE_FILES)
    {
		if (strcmp(FindFileData.cFileName,".")!=0 && strcmp(FindFileData.cFileName,"..")!=0)
        {
  			path=Dir;
			path+="\\";
			path+=FindFileData.cFileName;

            if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{		
				subdir=SubDir;
				if(strlen(SubDir)!=0)
					subdir+="\\";
				subdir+=FindFileData.cFileName;
				scanDir(path.c_str(),subdir.c_str(),Ext,bStopRequest,param,callback);
			}
            else
            {	
				std::string file=FindFileData.cFileName;
				std::string ext=".";
				ext+=Ext;
				if(strlen(Ext)==0
					|| (file.size()>ext.size() && file.compare(file.size()-ext.size(),ext.size(),ext)==0))
				{
					callback(SubDir,FindFileData.cFileName,param);
				}

			}

			if(bStopRequest)
				break;
        }
        
		//Get next file
        if (!FindNextFileA(hFind, &FindFileData))
            a = GetLastError();
    }
    FindClose(hFind);
}

#else
#include <sys/stat.h>
#include "dirent.h"
#include "errno.h"

void CPathUtil::scanDir(const char *Dir,
			const char *SubDir,
			const char *Ext,
			bool &bStopRequest,
			void *param,
			void (* callback)(const char *SubDir, const char *File, void *param))
{
  std::string path = Dir;
  std::string subdir;

  DIR *pDir = opendir(Dir);

  // If pDir is NULL then the dir doesn't exist
  if(pDir != NULL) {
    struct dirent *pFile = readdir(pDir);

    for ( ;pFile != NULL; pFile = readdir(pDir))
      {

	// skip the . and .. directories
	if( strcmp(pFile->d_name,".") !=0 &&
	    strcmp(pFile->d_name,"..") != 0 ) {

	  path = Dir;
	  path += "/";
	  path +=  pFile->d_name;
	  
	  // check file attributes
	  struct stat buffer;
	  if ( ! stat(path.c_str(),&buffer) ){
	    if( S_ISDIR(buffer.st_mode) ){
	      // this is a directory: recursive scan
	      subdir=SubDir;
	      if(strlen(SubDir)!=0)  subdir+="/";
	      subdir += pFile->d_name;

	      scanDir(path.c_str(),subdir.c_str(),Ext,bStopRequest,param,callback);

	    }  else  {	
	      // this is a regular file
	      std::string file = pFile->d_name;
	      std::string ext=".";
	      ext+=Ext;
	      // check if the file has the requested extension
	      if(strlen(Ext)==0
		 || (file.size()>ext.size() && file.compare(file.size()-ext.size(),ext.size(),ext)==0))
		{
		  callback(SubDir,file.c_str(),param);
		}
	      
	    }
	  } else {
	    // log error
	    printf("APL_CrlDownloadingCache::scanDir stat failed: %s\n",strerror(errno) );
	  }
	}
	if (bStopRequest)
		break;
      }
    closedir(pDir);

  } else {
    // log error
    printf("APL_CrlDownloadingCache::scanDir \"%s\" : %s\n",Dir,strerror(errno));
    return;
  }

}
#endif
std::string CPathUtil::getFullPathFromUri(const char *rootPath,const char *uri)
{
	std::string relativePath=getRelativePath(uri);
	std::string fullPath=getFullPath(rootPath,relativePath.c_str());

	return fullPath;
}

std::string CPathUtil::getFullPath(const char *rootPath,const char *relativePath)
{
	std::string file=rootPath;

#ifdef WIN32
	file+="\\";
#else
	file+="/";
#endif

	file+=relativePath;

	return file;
}

std::wstring CPathUtil::getFullPath(const wchar_t *rootPath,const wchar_t *relativePath)
{
	std::wstring file=rootPath;

#ifdef WIN32
	file+=L"\\";
#else
	file+=L"/";
#endif

	file+=relativePath;

	return file;
}

std::string CPathUtil::getRelativePath(const char *uri)
{
	std::string file;

	char *buffer = new char[strlen(uri)+1];
#ifdef WIN32
	strcpy_s(buffer,strlen(uri)+1,uri);
#else
	strcpy(buffer,uri);
#endif

	char *pStr=strstr(buffer,"://");		//We look for the protocole
	if(pStr && pStr!=buffer)
	{
		*pStr='\0';								//The first relative directory is the protocole name
		file+=buffer;

#ifdef WIN32
		file+="\\";
#else
		file+="/";
#endif
		pStr+=3;

#ifdef WIN32
		for(char *pStr2=pStr;*pStr2!=0;pStr2++)
			if(*pStr2=='/') *pStr2='\\';
#endif
		file+=pStr;

	}
	delete[] buffer;

	return file;
}

std::string CPathUtil::getUri(const char *relativePath)
{
	std::string uri;
	
	char *buffer = new char[strlen(relativePath)+1];
#ifdef WIN32
	strcpy_s(buffer,strlen(relativePath)+1,relativePath);
#else
	strcpy(buffer,relativePath);
#endif

#ifdef WIN32
	char *pStr=strstr(buffer,"\\");
#else
	char *pStr=strstr(buffer,"/");
#endif

	if(pStr && pStr!=buffer)
	{
		*pStr='\0';
		uri+=buffer;			//The first relative directory is the protocole name

		uri+="://";

		pStr++;

#ifdef WIN32
		for(char *pStr2=pStr;*pStr2!=0;pStr2++)
			if(*pStr2=='\\') *pStr2='/';
#endif
		uri+=pStr;
	}

	return uri;
}

/*****************************************************************************************
------------------------------------ CSVParser ---------------------------------------
*****************************************************************************************/
CSVParser::CSVParser(const CByteArray &data, unsigned char separator)
{
	parse(data,separator);
}

CSVParser::~CSVParser()
{
	std::vector<CByteArray *>::iterator itr;

	itr = m_vector.begin();
	for ( ; itr!=m_vector.end(); itr++)
	{
		delete *itr;
		//itr=m_vector.erase(itr); // not needed
	} 

}

unsigned long CSVParser::count()
{
	return (unsigned long)m_vector.size();
}

const CByteArray &CSVParser::getData(unsigned long idx)
{
	if(idx>=count())
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

	return *m_vector[idx];
}

void CSVParser::parse(const CByteArray &data, unsigned char separator)
{

	unsigned long i;
	unsigned long begin=0;
	unsigned long len=0;

	for(i=0;i<data.Size();i++)
	{
		if(data.GetByte(i)==separator)
		{
			m_vector.push_back(new CByteArray(data.GetBytes(begin,len)));
			begin=i+1;
			len=0;
		}
		else
		{
			len++;
		}
	}
	if(begin<data.Size())
		m_vector.push_back(new CByteArray(data.GetBytes(begin)));

}

/*****************************************************************************************
------------------------------------ TLVParser ---------------------------------------
*****************************************************************************************/
TLVParser::TLVParser():CTLVBuffer()
{
}

TLVParser::~TLVParser()
{
	std::map<unsigned char,CTLVBuffer *>::iterator itr;
	
	itr = m_subfile.begin();
	for ( ; itr!=m_subfile.end(); itr++)
	{
		delete itr->second;
		//itr=m_subfile.erase(itr); // not needed
	} 
}

CTLV *TLVParser::GetSubTagData(unsigned char ucTag,unsigned char ucSubTag)
{
	std::map<unsigned char,CTLVBuffer *>::iterator itr;
	CTLVBuffer *subfile=NULL;

	itr = m_subfile.find(ucTag);
	if(itr==m_subfile.end())
	{
		CTLV *tlv=GetTagData(ucTag);
		if(tlv)
		{
			subfile= new CTLVBuffer();
			subfile->ParseFileTLV(tlv->GetData(),tlv->GetLength());
			m_subfile[ucTag]=subfile;
		}
		else
		{
			m_subfile[ucTag]=NULL;
		}
	}
	subfile=m_subfile[ucTag];

	if(subfile)
		return subfile->GetTagData(ucSubTag);
	else
		return NULL;
}

}
