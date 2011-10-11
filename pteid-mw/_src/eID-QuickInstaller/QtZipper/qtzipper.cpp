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
#include <QtGlobal>
#include <QDirIterator>
#include <QDir>
#include <QCryptographicHash>
#include "zrclib.h"
#include "qtzipper.h"

QByteArray QtZipper::getFileBytes (QString filename) 
{
    QFile qf(filename);
	qf.open (QIODevice::ReadOnly);
    QByteArray ba;
	QDataStream in(&qf);
	ba = qf.readAll();
	qf.close();
	return ba;
}

void QtZipper::getHash(QByteArray const& ba, std::string& hash) 
{
	QString qstrHash;
	getHash(ba, qstrHash);
	hash = qstrHash.toStdString();
}
void QtZipper::getHash(QByteArray const& ba, QString& hash) 
{
	QByteArray theHash = QCryptographicHash::hash (ba,QCryptographicHash::Sha1);
	hash = theHash.toHex();
}

void QtZipper::saveByteArrayToFile(QByteArray& ba, QFile& outfile, QString& hash ) 
{
	outfile.open(QIODevice::WriteOnly);
	QDataStream dsout(&outfile);
	dsout.writeRawData(ba.data(),ba.length());
	outfile.close();
	getHash(ba,hash);
}

void QtZipper::zipFolder(QString const& folderPath, QString const& ArchiveFileName)
{
	std::cout << "[INFO] Archive file: " << ArchiveFileName.toStdString().c_str() << std::endl;

	ZrcArchive zarch(folderPath.toStdString() + "/" + ArchiveFileName.toStdString());
	zarch.CreateArchive();

	QFileInfo	 archive(ArchiveFileName);
	QFile		 tmpZip(folderPath+QString("/tmpzip.zip"));
	QDir		 qdd(folderPath,"",QDir::Name,QDir::AllEntries);
	QStringList	 filter;
	filter << "*.cat" << "*.inf" << "*.sys" << "*.dll" << "*.ini" << "*.cpl" << "*.BMP" << "*.txt";
	qdd.setNameFilters(filter); 

	QDirIterator it(qdd, QDirIterator::Subdirectories);

	//-------------------------------------------------
	// run over all items in the directory
	//-------------------------------------------------
	while (it.hasNext()) 
	{
		it.next();

		if (   it.fileInfo().isFile()				// must be a file
			&& it.fileName() != archive.fileName()	// omit the own archive file
		    ) 
		{
			QString x = it.filePath();			// get the file name

			std::cout << "[INFO] Processing file: " << x.toStdString().c_str() << std::endl;

			x.remove(0,folderPath.size()+1);
			
			try 
			{
				QByteArray baCompressed = qCompress (getFileBytes(it.filePath()));
				QFile	   tmpZip(folderPath+QString("/tmpzip.zip"));
				QString	   hash;
				saveByteArrayToFile(baCompressed,tmpZip,hash);

				QStringList qsl   = x.split("/");
				QString		qsos  = qsl[0];
				QString		qsrdr = qsl[1];
				string		fn;

				x  = x.mid((qsos+QString("/")+qsrdr+QString("/")).size());
				fn = x.toStdString();

				ifstream ifs;
				ifs.open(tmpZip.fileName().toStdString().c_str(), ios::binary);
				if (ifs.good()) 
				{
					// take size of file
					ifs.seekg(0, ios::end);
					long size = ifs.tellg();
					ifs.seekg(0, ios::beg);
					// read the content
					auto_vec<char> data(new char[size]);
					ifs.read(data.get(), size);
					ifs.close();
					// make a file header and add to archive
					string reader = qsrdr.toStdString();
					string os = qsos.toStdString();

					ZrcFileHeader zfh(reader, os, fn, size);
					zarch.AddFileHeader(&zfh);
					zarch.AddFileData(data.get());
				}
				ifs.close();
				tmpZip.remove();
			}
			catch (...) 
			{
			}
		}
	}
}

/*
void EZZipper::on_pbTestUnzip_clicked()
{
	ui.textEdit->clear();
	QFileDialog::Options options = QFileDialog::DontResolveSymlinks ;
	QString zipFile = QFileDialog::getOpenFileName(this,tr("Kies zipfile"),ui.leFolder->text(),"*.zrc",0,options);
	if (!zipFile.isEmpty()) {

		QString unzipFolder = ui.leFolder->text()+QString("/unziptestfolder/");
		string tmpUnzipfile = unzipFolder.toStdString()+"tmpUnzipfile";
		QDir qd(ui.leFolder->text());

//werktniet		qd.rmpath(unzipFolder);
		qd.mkpath(unzipFolder);


        // open weer maar voor input
		ZrcArchive zarch(zipFile.toStdString());
        zarch.OpenArchive();
        ZrcFileHeader *hdr = NULL;

        // lees de file headers
        while ((hdr = zarch.ReadFileHeader()) != NULL) {

         //   if (hdr->get_Filename().compare("f2") == 0) {
                // lees de file data van "f2"
                char* data = zarch.ReadFileData(hdr->get_Filesize());
                // en terug wegschrijven ter controle
                ofstream ofs;
                string xfn = hdr->get_Filename();

                

				//ofs.open(xfn.c_str(), ios::binary);
                ofs.open(tmpUnzipfile.c_str(), ios::binary);
                ofs.write(data, hdr->get_Filesize());
                ofs.close();

				QString extractToFileName = unzipFolder+QString(hdr->get_OS().c_str())+QString("/")+QString(hdr->get_Reader().c_str())+QString("/")+   QString(xfn.c_str());
				QString tmpFolder = extractToFileName.mid(0,extractToFileName.lastIndexOf("/"));

			    qd.mkpath(tmpFolder);

				QByteArray ba = qUncompress(getFileBytes(QString(tmpUnzipfile.c_str())));
				saveByteArrayToFile(ba,extractToFileName);
				
				appendString("Extracting "+extractToFileName.toStdString()+".",QFont::Normal);

                // schoonmaak
                delete[] data;
            // schoonmaak
            delete hdr;

        }
        // sluit archive
        zarch.CloseArchive();
	    QFile::remove(QString(tmpUnzipfile.c_str()));
	    appendString("DONE",QFont::Bold);
	}
     
}
*/