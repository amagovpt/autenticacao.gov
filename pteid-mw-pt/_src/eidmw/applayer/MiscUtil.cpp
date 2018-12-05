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

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/pem.h>

#ifdef WIN32
#include <Windows.h>
#include <io.h> //For _sopen and _chsize
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
//For utilStringWiden()
#include "Util.h"
#endif

#include <cstdio>
#include <cstring>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <errno.h>

#include "MWException.h"
#include "eidErrors.h"
#include "ByteArray.h"

#define STR_BEGIN_CERTIFICATE   "-----BEGIN CERTIFICATE-----"
#define STR_END_CERTIFICATE     "-----END CERTIFICATE-----"

namespace eIDMW
{

/*  *********************************************************
    ***          getCPtr()                                ***
    ********************************************************* */
char *getCPtr( std::string inStr, int *outLen ) {
    char *c_str;

    c_str = (char *)malloc( inStr.length() + 1 );
    strcpy( c_str, inStr.c_str() );

    if ( outLen != NULL ) *outLen = strlen( c_str );

    return c_str;
}

const void *memmem(const void *haystack, size_t n, const void *needle, size_t m)
{
    if (m > n || !m || !n)
        return NULL;

    if (m > 1) {
        const unsigned char*  y = (const unsigned char*) haystack;
        const unsigned char*  x = (const unsigned char*) needle;
        size_t                j = 0;
        size_t                k = 1, l = 2;

        if (x[0] == x[1]) {
            k = 2;
            l = 1;
        }
        while (j <= n-m) {
            if (x[1] != y[j+1]) {
                j += k;
            } else {
                if (!memcmp(x+2, y+j+2, m-2) && x[0] == y[j])
                    return (void*) &y[j];
                j += l;
            }
        }
    } else {
        /* degenerate case */
        return memchr(haystack, ((unsigned char*)needle)[0], n);
    }
    return NULL;
}

#ifdef WIN32
	char *Basename(char *absolute_path)
	{
		char filename[_MAX_FNAME];
		char ext[_MAX_EXT];
		char *basename = (char *)malloc(2*_MAX_FNAME);

		_splitpath(absolute_path, NULL, NULL, filename, ext);
		strcpy(basename, filename);
		strcat(basename, ext);
		return basename;
	}

	int Truncate(const char *path)
	{
		int fh = 0, result = 0;

		/* This replicates the use of truncate() on Unix */
		if ((fh = _sopen(path, _O_RDWR, _S_IWRITE)) == NULL)
		{
			if (( result = _chsize(fh, 0)) == 0)
			_close(fh);
		}

		return result;
	}

#else
	char * Basename(char *absolute_path)
	{
		return basename(absolute_path); //POSIX basename()
	}

	int Truncate(const char *path)
	{
		return truncate(path, 0); //POSIX truncate()
	}

#endif

	//Quick fix: Unreadable snippet to convert typical western languages characters
	//to UTF-8, blame stackoverflow: http://stackoverflow.com/a/4059934/9906
	void latin1_to_utf8(unsigned char * in, unsigned char *out)
	{
		while (*in)
		{
			if (*in<128)
				*out++=*in++;
			else
			{
				*out++=0xc2+(*in>0xbf);
				*out++=(*in++&0x3f)+0x80;
			}
		}
		*out = '\0';
	}

	//adapted from https://codereview.stackexchange.com/a/78539
	std::string urlEncode(unsigned char *data, int len)
	{
		char hexmap[] = { '0', '1', '2', '3', '4', '5', '6', '7',
			'8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
		std::string s(len * 3, ' ');
		for (int i = 0; i < len; ++i) {
			s[3 * i] = '%';
			s[3 * i + 1] = hexmap[(data[i] & 0xF0) >> 4];
			s[3 * i + 2] = hexmap[data[i] & 0x0F];
		}
		return s;
	}


void replace_lastdot_inplace(char* str_in)
{
	// We can only search forward because memrchr and strrchr
	// are not available on Windows *sigh*
	char ch = '.';
	char * pdest = str_in, *last_dot= NULL;
       	while ((pdest = strchr(pdest, ch)) != NULL)
	{
	     last_dot = pdest;
	     pdest++;
	}

	// Don't replace '.' if its a UNIX dotfile
	if (last_dot != NULL && *(last_dot-1) != '/')
		*last_dot = '_';
}


std::vector<std::string> toPEM(char *p_certificate, int certificateLen) {

	std::vector<std::string> certs;

    string strCertificate( p_certificate, certificateLen );

    if (strCertificate.empty()) {
        throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
    }

    std::size_t found_init = 0;
    std::size_t found_after = 0;
    std::size_t found_end = 0;
    while(true)
    {
    	std::string certificate;
    	
	    /*
	      Search for STR_BEGIN_CERTIFICATE
	    */
	    found_init = strCertificate.find(STR_BEGIN_CERTIFICATE, found_init);
	    if (found_init == string::npos) {
	        break;
	    }

	    /*
	      Add newline after STR_BEGIN_CERTIFICATE
	    */
	    found_after = found_init + strlen(STR_BEGIN_CERTIFICATE);
	    if ( strCertificate.substr( found_after, 1 ) != "\n") {
	        strCertificate.insert(found_after, "\n" );
	    }

	    /*
	        Search for STR_END_CERTIFICATE
	    */
	    found_end = strCertificate.find(STR_END_CERTIFICATE, found_init);
	    if ( found_end == string::npos ) {
	        throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	    }

	    /*
	        Add newline before STR_END_CERTIFICATE
	    */
	    if ( strCertificate.substr( (found_end - 1), 1 ) != "\n") {
	        strCertificate.insert( found_end, "\n" );
	    }
	    int pos_end = found_end + strlen(STR_END_CERTIFICATE) + 1;

	    certificate = strCertificate.substr(found_init, pos_end-found_init);
	    certs.push_back(certificate);

	    found_init = found_end;
    }

    return certs;
}

/*  *********************************************************
    ***          X509_to_PEM()                            ***
    ********************************************************* */
char *X509_to_PEM(X509 *x509) {

    BIO *bio = NULL;
    char *pem = NULL;

    if ( NULL == x509 ) {
        return NULL;
    }

    bio = BIO_new( BIO_s_mem() );
    if ( NULL == bio ) {
        return NULL;
   	}

    if ( 0 == PEM_write_bio_X509(bio, x509)) {
        BIO_free( bio );
        return NULL;
    }

    pem = (char *) malloc( bio->num_write + 1 );
    if ( NULL == pem ){
        BIO_free(bio);
        return NULL;
    }

    memset( pem, 0, bio->num_write + 1 );
    BIO_read( bio, pem, bio->num_write );
    BIO_free( bio );

    return pem;
}

/*  *********************************************************
    ***          PEM_to_X509()                            ***
    ********************************************************* */
X509 *PEM_to_X509(char *pem) {
    X509 *x509 = NULL;
    BIO *bio = NULL;

    if ( NULL == pem ){
        return NULL;
    }

    bio = BIO_new_mem_buf(pem, strlen(pem));
    if ( NULL == bio ){
        return NULL;
    }

    x509 = PEM_read_bio_X509( bio, NULL, NULL, NULL );
    BIO_free( bio );

    return x509;
}

/*  *********************************************************
    ***          X509_to_DER()                            ***
    ********************************************************* */
int X509_to_DER( X509 *x509, unsigned char **der ) {

    if ( NULL == der ) return -1;
    *der = NULL;

    int len = i2d_X509( x509, der );

    return len;
}

/*  *********************************************************
    ***          DER_to_X509()                            ***
    ********************************************************* */
X509 *DER_to_X509( unsigned char *der, int len ){
    unsigned char *p = der;
    X509 *x509 = d2i_X509( NULL, (const unsigned char**)&p, len );

    return x509;
}

/*  *********************************************************
    ***          DER_to_PEM()                            ***
    ********************************************************* */
char *DER_to_PEM( unsigned char *der, int len ){

    X509 *x509 = DER_to_X509( der, len );
    if ( NULL == x509) return (char *)NULL;

    return X509_to_PEM( x509 );
}

/*  *********************************************************
    ***          DER_to_PEM()                            ***
    ********************************************************* */
int PEM_to_DER( char *pem, unsigned char **der ){

    X509 *x509 = PEM_to_X509( pem );
    if ( NULL == x509) return ((int )-1);

    return X509_to_DER( x509, der );
}

/*
Base64 encode binary-data: it can be used also for C-style strings if we ignore the 0x0 terminator
*/

char *Base64Encode(const unsigned char *input, long length)
{
	BIO *bmem, *b64;
	BUF_MEM *bptr;

	b64 = BIO_new(BIO_f_base64());
	bmem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bmem);
	BIO_write(b64, input, length);
	BIO_flush(b64);
	BIO_get_mem_ptr(b64, &bptr);

	char *buff = (char *)malloc(bptr->length);
	memcpy(buff, bptr->data, bptr->length-1);
	buff[bptr->length-1] = 0;

	BIO_free_all(b64);

	return buff;
}

void Base64Decode(const char *array, unsigned int inlen, unsigned char *&decoded, unsigned int &decoded_len)
{

    BIO *bio, *b64;
    unsigned int outlen = 0;
    unsigned char *inbuf = new unsigned char[512];
    memset(inbuf, 0, 512);

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    bio = BIO_new_mem_buf((void *)array, inlen);
    bio = BIO_push(b64, bio);

    outlen = BIO_read(bio, inbuf, 512);
    decoded = inbuf;

    decoded_len = outlen;

    BIO_free_all(bio);
}

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

	std::wstring utf16FileName = utilStringWiden(std::string(filePath));

	DWORD dwAttr = GetFileAttributesW(utf16FileName.c_str());
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

	fprintf(stderr, "checkDir() was called with %s\n", dirIn);

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
			fprintf(stderr, "The path '%s' does not exist.\nCreate it or change the config parameter\n",dirIn);
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
	      std::string ext = Ext;
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

std::string CPathUtil::remove_ext_from_basename(const char *base)
{
	std::string base_name = base;
	std::string::size_type ext_at = base_name.rfind('.', base_name.size());

	if (ext_at == std::string::npos)
	{
		return base_name; //no extension
	}
	else
	{
		return base_name.substr(0, ext_at);
	}
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
