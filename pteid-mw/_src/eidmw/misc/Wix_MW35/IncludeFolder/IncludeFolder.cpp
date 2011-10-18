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
// IncludeFolder.cpp : Defines the entry point for the console application.
//

#ifdef WIN32
#include <Windows.h>
#endif

#include <string>

static long ParseCommandLine(int argc, char **argv);
static long PrintUsage(const char *csMesg);
static void BrowseFolder();

static int g_bHelp = 0;
static char *g_csComponent = "";
static char *g_csGuid = "";
static char *g_csFolder = "";
static char *g_csDirRef = "";
static char *g_csKeyPath = "";
static char *g_csOutput = "";
static char *g_csShortcutFile = "";
static char *g_csShortcutFolder = "";
static char *g_csShortcutName = "";
static char *g_csShortcutDescr = "";
static bool g_bUserProfile = false;

#ifndef WIN32
#define	fprintf_s fprintf
#endif

int main(int argc, char **argv)
{
	int lRet = ParseCommandLine(argc, argv);
	if (lRet != 0)
		return lRet;

	if(g_bHelp)
		lRet = PrintUsage(NULL);

	if(!g_bHelp && (strlen(g_csComponent)==0 || strlen(g_csGuid)==0 || strlen(g_csFolder)==0 || strlen(g_csKeyPath)==0 || strlen(g_csOutput)==0))
		PrintUsage("Parameters are missing...");

	BrowseFolder();
	
	return 0;
}
///////////////////////////// Parse command line //////////////////////////

#define INCREMENT_TEST_INDEX(cmd, cmdargs) \
	iIndex++; \
	if (iIndex >= argc) { \
		printf("ERR: \"%s\" needs %s argument(s)\n", cmd, cmdargs); \
		return -1; \
	}\

long ParseCommandLine(int argc, char **argv)
{
	int iIndex = 0;

	for (iIndex = 1; iIndex < argc; iIndex++)
	{
		char *arg = argv[iIndex];

		if (!strcmp(arg, "--help") || !strcmp(arg, "-h"))
			g_bHelp=1;

		else if (!strcmp(arg, "--userprofile") || !strcmp(arg, "-u"))
		{
			g_bUserProfile = true;
		}
		else if (!strcmp(arg, "--component") || !strcmp(arg, "-c"))
		{
			INCREMENT_TEST_INDEX("--component", "1");
			g_csComponent = argv[iIndex];
		}
		else if (!strcmp(arg, "--guid") || !strcmp(arg, "-g"))
		{
			INCREMENT_TEST_INDEX("--guid", "1");
			g_csGuid = argv[iIndex];
		}
		else if (!strcmp(arg, "--folder") || !strcmp(arg, "-f"))
		{
			INCREMENT_TEST_INDEX("--folder", "1");
			g_csFolder = argv[iIndex];
		}
		else if (!strcmp(arg, "--keypath") || !strcmp(arg, "-k"))
		{
			INCREMENT_TEST_INDEX("--keypath", "1");
			g_csKeyPath = argv[iIndex];
		}
		else if (!strcmp(arg, "--dirref") || !strcmp(arg, "-d"))
		{
			INCREMENT_TEST_INDEX("--dirref", "1");
			g_csDirRef = argv[iIndex];
		}
		else if (!strcmp(arg, "--output") || !strcmp(arg, "-o"))
		{
			INCREMENT_TEST_INDEX("--output", "1");
			g_csOutput = argv[iIndex];
		}
		else if (!strcmp(arg, "--shortcut") || !strcmp(arg, "-s"))
		{
			INCREMENT_TEST_INDEX("--shortcut", "4");
			g_csShortcutFile = argv[iIndex];
			g_csShortcutFolder = argv[++iIndex];
			g_csShortcutName = argv[++iIndex];
			g_csShortcutDescr = argv[++iIndex];
		}
		else
		{
			printf("unknown option \"%s\", exiting\n", arg);
			return -1;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
static long PrintUsage(const char *csMesg)
{
	printf("\n");
	if (csMesg)
		printf("%s\n\n", csMesg);
	printf("Command line parameters:\n");
	printf("  --help or -h : print help\n");
	printf("  --userprofile or -u : specify the component is install in user profile\n");
	printf("  --component or -c <name> : specify the component name\n");
	printf("  --guid or -g <guid> : specify the unique id\n");
	printf("  --folder or -f <folder> : specify the folder to browse\n");
	printf("  --keypath or -k <file> : specify the keypath file\n");
	printf("  --output or -o <file> : specify the output file\n");
	printf("  --shortcut or -s <file> <folder> <name> <description> : specify the file for a shortcut\n");

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
void scanDir(const char *Dir,FILE *f,const char *idBase,const char *DirRef,const char *ShortcutFile,const char *ShortcutFolder,const char *ShortcutName,const char *ShortcutDescr,void (* callback)(FILE *f,const char *Dir,const char *File,const char *Id,const char *DirRef,const char *ShortcutFile,const char *ShortcutFolder,const char *ShortcutName,const char *ShortcutDescr))
{
	WIN32_FIND_DATAA FindFileData; 
	std::string path;
	std::string subdir;
	HANDLE hFind;				  
	DWORD a = 0;	
	int count = 1;
	char id[50];

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

            if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {	
				std::string file=FindFileData.cFileName;
				sprintf_s(id,sizeof(id),"%s%4.4ld",idBase,count);
				callback(f,Dir,FindFileData.cFileName,id,DirRef,ShortcutFile,ShortcutFolder,ShortcutName,ShortcutDescr);
				count++;
			}
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

void scanDir(const char *Dir,
			FILE *f,
			const char *idBase,
			const char *DirRef,
			const char *ShortcutFile,
			const char *ShortcutFolder,
			const char *ShortcutName,
			const char *ShortcutDescr,
			void (* callback)(FILE *f,const char *Dir,const char *File,const char *Id,const char *DirRef,const char *ShortcutFile,const char *ShortcutFolder,const char *ShortcutName,const char *ShortcutDescr))
{
  std::string path = Dir;
  std::string subdir;
  int count = 1;
  char id[50];

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
	    if( ! S_ISDIR(buffer.st_mode) )
		{
	      // this is a regular file
	      std::string file = pFile->d_name;
		  sprintf(id,"%s%4.4ld",idBase,count);
		  callback(f,Dir,file.c_str(),id,DirRef,ShortcutFile,ShortcutFolder,ShortcutName,ShortcutDescr);	 
		  count++;
	    }
	  } else {
	    // log error
	    printf("scanDir stat failed: %s\n",strerror(errno) );
	  }
	}
      }
    closedir(pDir);

  } else {
    // log error
    printf("scanDir \"%s\" : %s\n",Dir,strerror(errno));
    return;
  }

}
#endif

/////////////////////////////////////////////////////////////////////////////
void PrintLine(FILE *f,const char *Dir,const char *File,const char *Id,const char *DirRef,const char *ShortcutFile,const char *ShortcutFolder,const char *ShortcutName,const char *ShortcutDescr)
{
	fprintf_s(f,"\t\t\t\t<File Id=\"%s\" Name=\"%s\" KeyPath=\"%s\" Source=\"%s%s\" />\n",Id,File,(!g_bUserProfile && strcmp(g_csKeyPath,File)==0?"yes":"no"),Dir,File);
	if(strcmp(File,ShortcutFile)==0)
	{		
		fprintf_s(f,"\t\t\t\t<Shortcut Id=\"%sShortcut\" Target=\"[%s]%s\" Directory=\"%s\" Name=\"%s\" WorkingDirectory=\"INSTALLLOCATION\" Description=\"%s\" />\n",Id,DirRef,ShortcutFile,ShortcutFolder,ShortcutName,ShortcutDescr); 
	}
}

/////////////////////////////////////////////////////////////////////////////
static void BrowseFolder()
{

	FILE *f=NULL;
	int err=0;

#ifdef WIN32
		err = fopen_s(&f, g_csOutput, "w");
#else
		f = fopen(g_csOutput, "w");
		if (f == NULL) err=errno;		
#endif

	if (err != 0) 
		return;

	fprintf_s(f,"<?xml version='1.0' encoding='windows-1252'?>\n");
	fprintf_s(f,"<Wix xmlns=\"http://schemas.microsoft.com/wix/2006/wi\" xmlns:util=\"http://schemas.microsoft.com/wix/UtilExtension\">\n");
	fprintf_s(f,"\t<Fragment Id='Fragment_%s'>\n",g_csComponent);
    fprintf_s(f,"\t\t<DirectoryRef Id=\"%s\">\n",g_csDirRef);
	fprintf_s(f,"\t\t\t<Component Id=\"%s\" Guid=\"%s\">\n",g_csComponent,g_csGuid);
	if(g_bUserProfile)
	{
		fprintf_s(f,"\t\t\t\t<RegistryValue Root=\"HKCU\" Key=\"%s\" Name=\"%s\" Type=\"integer\" Value=\"1\" KeyPath=\"yes\" />\n",g_csKeyPath,g_csComponent);
		fprintf_s(f,"\t\t\t\t<RemoveFolder Id=\"%s\" On=\"uninstall\"/>\n",g_csDirRef);
	}

	scanDir(g_csFolder,f,g_csComponent,g_csDirRef,g_csShortcutFile,g_csShortcutFolder,g_csShortcutName,g_csShortcutDescr,&PrintLine);

	fprintf_s(f,"\t\t\t</Component>\n");
    fprintf_s(f,"\t\t</DirectoryRef>\n");
	fprintf_s(f,"\t</Fragment>\n");
	fprintf_s(f,"</Wix>\n");
	fclose(f);
	f=NULL;

}