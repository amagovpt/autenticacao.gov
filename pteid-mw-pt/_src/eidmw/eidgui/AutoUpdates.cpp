/* ****************************************************************************

 * PTeID Middleware Project.
 * Copyright (C) 2011-2012 Caixa Mágica Software.
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
 *
 * Author: Luis Medinas <luis.medinas@caixamagica.pt>
 **************************************************************************** */

#include <QString>
#include <QProgressDialog>
#include <QFutureWatcher>
#include <QtCore>

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/types.h>

#include "eidlib.h"
#include "AutoUpdates.h"

#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#include "verinfo.h"
#else
#include "pteidversions.h"
#endif

using namespace eIDMW;

QProgressDialog *ddialog;

AutoUpdates::AutoUpdates()
{
}

AutoUpdates::~AutoUpdates()
{
	curl_global_cleanup();
}

int progress(void* data, double dltotal, double dlnow, double ultotal, double ulnow)
{

	  ddialog->setMinimum(0);
	  ddialog->setMaximum(0);
	  //ddialog->setValue(20);

	  //ddialog->update

	  return 0;


}
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

struct MemoryStruct {
  char *memory;
  size_t size;
};


static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory == NULL) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    exit(EXIT_FAILURE);
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

std::string AutoUpdates::CheckUpdates()
{
	CURL *curl_handle;

	  struct MemoryStruct chunk;

	  chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
	  chunk.size = 0;    /* no data at this point */

	  curl_global_init(CURL_GLOBAL_ALL);

	  curl_handle = curl_easy_init();

	  curl_easy_setopt(curl_handle, CURLOPT_URL, "http://people.caixamagica.pt/lmedinas/autoupdates/version.txt");

	  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

	  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

	  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	  curl_easy_perform(curl_handle);

	  curl_easy_cleanup(curl_handle);

	  //printf("%lu bytes retrieved\n", (long)chunk.size);

	  std::string text = chunk.memory;

	  std::cout << "remote version strings " << text << std::endl;

	  if(chunk.memory)
	    free(chunk.memory);

	  curl_global_cleanup();

	  return text;
}

bool AutoUpdates::VerifyUpdates()
{
	QString strVersion (WIN_GUI_VERSION_STRING);
	std::string ver = strVersion.toStdString();
	ver.replace(2,1,"");
	ver.replace(3,1,"");
	ver.replace(3,1,"");
	ver.replace(4,6, "");

	double localverd = atof(ver.c_str());
	//printf ("value %f\n", localverd);

	std::string remoteversion = CheckUpdates();
	double remoteversiond = atof(remoteversion.c_str());

	//return true;
	if (localverd < remoteversiond)
	{
		return true;
	} else {
		return false;
	}
}

bool AutoUpdates::FileExists(const char *filename)
{
	std::ifstream ifile(filename);
	ifile.close();
	return ifile;
}

std::string AutoUpdates::VerifyOS(std::string param, bool runscript)
{
#ifdef WIN32

	//check if it's Windows 32 or 64 bits

#else
	bool chkfile;
	std::string distrostr;
	std::string archstr;

	if (runscript)
	{
		int sysret;
		sysret = system("/usr/local/bin/pteidlinuxversion.pl");
		if (sysret != 0)
			PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Problem trying to run pteidlinuxversion.pl");
	}

	try
	{
		if (FileExists("/tmp/linuxversion"))
		{
			std::ifstream linuxversion("/tmp/linuxversion");
			std::string line;

			while(std::getline(linuxversion, line))
			{
				std::stringstream   linestream(line);
				std::string         value;

				getline(linestream,value, ';');
				std::istringstream distro(value);
				distro >> distrostr;

				getline(linestream, value, ';');
				std::istringstream arch(value);
				arch >> archstr;
			}
		}
	} catch (...) {
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "can't open file /tmp/linuxversion");
	}

	if (param == "distro")
		return distrostr;
	else
		return archstr;

#endif

}

void AutoUpdates::ChooseVersion(std::string distro, std::string arch)
{
	std::cout << "Choose Version " << distro << " arch " << arch << std::endl;
	if (arch == "x86_64")
	{
		if (distro == "debian")
		{
			DownloadDialog();
			//DownloadBinary("http://people.caixamagica.pt/lmedinas/autoupdates/PteidMW35-Basic-en.msi", "/tmp/PteidMW35-Basic-en.msi");
		}
		else if (distro == "redhat")
		{

		}
	}
}

void AutoUpdates::DownloadDialog()
{
	////GUI

	ddialog = new QProgressDialog();
	ddialog->setLabelText("Downloading...");

	std::string teste = "http://people.caixamagica.pt/lmedinas/autoupdates/PteidMW35-Basic-en.msi";
	std::string teste2 = "/tmp/PteidMW35-Basic-en.msi";

	QFuture<void> future = QtConcurrent::run(this, &AutoUpdates::DownloadBinary, teste.c_str(), teste2.c_str());
	this->FutureWatcher.setFuture(future);
	ddialog->exec();
	if (future.isFinished())
	{
		future.cancel();
	}

}

void AutoUpdates::DownloadBinary(const char *url, const char *outfilename)
{
	CURL *curl;
	FILE *fp;
	CURLcode res;
	curl = curl_easy_init();
	if (curl) {
		fp = fopen(outfilename,"wb");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0 );
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(fp);
	}
	return;
}
