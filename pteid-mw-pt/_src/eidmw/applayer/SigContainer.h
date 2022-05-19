/*-****************************************************************************

 * Copyright (C) 2012, 2014, 2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2018 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef SIGCONTAINER_H
#define SIGCONTAINER_H

#include <vector>
#include <string>

#include "ByteArray.h"

namespace eIDMW {

/*
 * Signature container for XAdES signature files and associated signed file(s) -
 * It should be compliant with the ASiC specification EN 319 162-1 -
 * https://www.etsi.org/deliver/etsi_en/319100_319199/31916201/01.01.01_60/en_31916201v010101p.pdf
 */
class SigContainer {
public:
	EIDMW_APL_API SigContainer(const char* path) : m_path(path) {};

	/*
	 * Create and save to disk ASiC container with signature file and associated signed files
	 */
	EIDMW_APL_API static void createASiC(CByteArray& sig, const char **paths, unsigned int num_paths, const char *output_file);

	/*
	 * Return the filenames of the input files in this container
	*/
	EIDMW_APL_API std::vector<std::string> listInputFiles();

	EIDMW_APL_API void extract(const char * filename, const char * out_dir);

	EIDMW_APL_API static bool isValidASiC(const char *filename);

	/*
	 * Return the filename of an eventual new signature xml file for this container
	 * ex: signatures002.xml, signatures003.xml
	*/
	const char *getNextSignatureFileName();

	void addSignature(const char *signatureFilename, const CByteArray& signature);

private:
	std::string m_path;
	//std::vector<std::string> m_ContainerFiles;
};

}

#endif
