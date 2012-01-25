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

#ifndef AUTOUPDATES_H_
#define AUTOUPDATES_H_

#include <QFutureWatcher>

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);

class AutoUpdates
{
public:
	AutoUpdates();
	~AutoUpdates();

	std::string CheckUpdates();
	bool VerifyUpdates();
	void DownloadBinary(const char *url, const char *outfilename);
	bool FileExists(const char *filename);
	//The Perl script can be run only one time to generate the file
	std::string VerifyOS(std::string param, bool runscript);
	void ChooseVersion(std::string distro, std::string arch);
	void DownloadDialog();

private:
	QFutureWatcher<void> FutureWatcher;
};

#endif /* AUTOUPDATES_H_ */
