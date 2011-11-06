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
#ifndef _QTZIPPER_H
#define _QTZIPPER_H

#include <string>
#include <QByteArray>
#include <QFile>

using namespace std;

class QtZipper
{
public:
	QtZipper()
	{
	}
	~QtZipper()
	{
	}
	void zipFolder(QString const& folderPath, QString const& ArchiveFileName);

	QByteArray getFileBytes (QString filename);
	void getHash(QByteArray const& ba, std::string& hash);
	void getHash(QByteArray const& ba, QString& hash);
	void saveByteArrayToFile(QByteArray& ba, QFile& filename, QString& hash ) ;



};

#endif // EZZIPPER_H
