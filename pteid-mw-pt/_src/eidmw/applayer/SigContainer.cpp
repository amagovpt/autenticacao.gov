/*-****************************************************************************

 * Copyright (C) 2012, 2014, 2016-2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2016 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2018 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include "SigContainer.h"

#include <fstream>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>

#include <zip.h>

#include "ByteArray.h"
#include "eidErrors.h"
#include "Log.h"
#include "MiscUtil.h"
#include "Util.h"
#include "XercesUtils.h"

#ifdef WIN32
#define NL "\r\n"
//Replacement for this POSIX function is in strndup.c - no need to add a new header file just for this
extern "C" {
	char * strndup(const char *str, size_t maxlen);
}
#else
#define NL "\n"
#endif

namespace eIDMW {

static const char *SIG_INTERNAL_PATH = "META-INF/signatures001.xml";
static const char *MIMETYPE_ASIC_S = "application/vnd.etsi.asic-s+zip";
static const char *MIMETYPE_ASIC_E = "application/vnd.etsi.asic-e+zip";

static const char *README =
"############################################################" NL
"LEIA-ME" NL
"" NL
"Este ficheiro zip contém informação assinada com a(s) respectiva(s) assinatura(s) em META-INF/signatures*.xml" NL
"Esta assinatura foi criada através da Aplicação Oficial do Cartão de Cidadão." NL
"" NL
"Mais Informação:" NL
"" NL
"Download da Aplicação Oficial do Cartão de Cidadão:" NL
"https://www.autenticacao.gov.pt/cc-aplicacao" NL
"" NL
"Especificação Técnica da Assinatura Digital:" NL
"XAdES / XAdES-T / XAdES-LTA" NL
"https://www.etsi.org/deliver/etsi_en/319100_319199/31913201/01.01.01_60/en_31913201v010101p.pdf" NL
"" NL
"############################################################" NL
"README" NL
"" NL
"This zip file includes signed information. The signature file(s) can be found in META-INF/signatures.*xml" NL
"This signature was produced through Autenticação.gov application." NL
"" NL
"More Info:" NL
"" NL
"Download Portuguese ID Card Management application:" NL
"https://www.autenticacao.gov.pt/cc-aplicacao" NL
"" NL
"Signature technical specification:" NL
"XAdES / XAdES-T / XAdES-LTA" NL
"https://www.etsi.org/deliver/etsi_en/319100_319199/31913201/01.01.01_60/en_31913201v010101p.pdf" NL;

//Write 32-bit value to buffer `data` in little-endian format
int zip_buffer_put_uint32(zip_uint8_t *data, zip_uint32_t i) {

    if (data == NULL) {
        return -1;
    }

    data[0] = (zip_uint8_t)(i & 0xff);
    data[1] = (zip_uint8_t)((i >> 8) & 0xff);
    data[2] = (zip_uint8_t)((i >> 16) & 0xff);
    data[3] = (zip_uint8_t)((i >> 24) & 0xff);

    return 0;
}

#ifdef WIN32
#define stat _stat
#endif

//Add Unix extended timestamp field to each zip entry for compatibility with MacOS Archive Utility
static void addEntryExtendedTimestamp(zip_t *asic, zip_int64_t index, const char * new_file)
{

	zip_uint16_t extra_field_id = 0x5455;
	time_t current_time = time(NULL);

	//This represents an Extended Timestamp field with mtime, atime and ctime
	zip_uint8_t extra_field_data[13] = {0};

	/* Bit flags mean modification, acess and creation time are present
	   Documented in proginfo/extrafld.txt from zip package source */
	extra_field_data[0] = 0x07;

#ifdef WIN32
	struct _stat64 statBuf = { 0 };
	if (new_file != NULL) {
		std::wstring utf16FileName = utilStringWiden(std::string(new_file));
		if (_wstat64(utf16FileName.c_str(), &statBuf) != 0) {
			MWLOG(LEV_ERROR, MOD_APL, L"%s: Failed to stat input file: %s", __FUNCTION__, errno == ENOENT ? "Filename or path not found": "Invalid parameter");
		}
	}
#else
	struct stat statBuf = { 0 };
	if (new_file != NULL && stat(new_file, &statBuf)) {
		MWLOG(LEV_ERROR, MOD_APL, "%s: Failed to stat file %s!", __FUNCTION__, new_file);
		//XX: If stat fails we set all the timestamps to 0 (the Unix Epoch)
	}
#endif

	time_t mtime = new_file != NULL ? statBuf.st_mtime: current_time;
	time_t atime = new_file != NULL ? statBuf.st_atime: current_time;
	time_t ctime = new_file != NULL ? statBuf.st_ctime: current_time;

	//Fill the extented timestamp dates
	//Need to cast the timestamps as in most current systems time_t is 64-bit
	zip_buffer_put_uint32(&extra_field_data[1],   (zip_uint32_t)mtime);
	zip_buffer_put_uint32(&extra_field_data[1+4], (zip_uint32_t)atime);
	zip_buffer_put_uint32(&extra_field_data[1+8], (zip_uint32_t)ctime);

	if (zip_file_extra_field_set(asic, index, extra_field_id, 0, extra_field_data, (zip_uint16_t) sizeof(extra_field_data), ZIP_FL_CENTRAL | ZIP_FL_LOCAL) < 0)
	{
		MWLOG(LEV_ERROR, MOD_APL, "Failed to add extra timestamp field to file!");
	}
}

static void addReadme(zip_t *asic)
{
	zip_int64_t index;
	zip_source_t *source = zip_source_buffer(asic, README, strlen(README), 0);
	if (source == NULL || (index = zip_file_add(asic, "META-INF/README.txt", source, ZIP_FL_ENC_GUESS)) < 0) {
		zip_source_free(source);
		MWLOG(LEV_ERROR, MOD_APL, L"Failed to add README.txt to zip container");
		throw CMWEXCEPTION(EIDMW_XADES_UNKNOWN_ERROR);
	}
	addEntryExtendedTimestamp(asic, index, NULL);
}

static void addManifest(zip_t *asic, const char **paths, int path_count)
{
	// don't include manifest file for asic-s containers
	if (path_count == 1) {
		return;
	}
	zip_int64_t index;

	XMLPlatformUtils::Initialize();

	XMLCh *manifest_prefix = XMLString::transcode("manifest");
	XMLCh *manifest_ns = XMLString::transcode("urn:oasis:names:tc:opendocument:xmlns:manifest:1.0");

	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("Core"));

	safeBuffer manifest;
	makeQName(manifest, manifest_prefix, "manifest");
	XERCES_NS DOMDocument *doc = impl->createDocument(manifest_ns, manifest.rawXMLChBuffer(), NULL);

	DOMElement *rootElem = doc->getDocumentElement();

	safeBuffer file_entry, full_path;
	makeQName(file_entry, manifest_prefix, "file-entry");
	makeQName(full_path, manifest_prefix, "full-path");

	DOMElement *first_element = doc->createElementNS(manifest_ns, file_entry.rawXMLChBuffer());
	first_element->setAttributeNS(manifest_ns, full_path.rawXMLChBuffer(), XMLString::transcode("/"));
	rootElem->appendChild(first_element);

	for (int i = 0; i < path_count; ++i) {
		DOMElement *element = doc->createElementNS(manifest_ns, file_entry.rawXMLChBuffer());
		element->setAttributeNS(manifest_ns, full_path.rawXMLChBuffer(),
			XMLString::transcode(Basename((char *)paths[i])));
		rootElem->appendChild(element);
	}

	std::unique_ptr<CByteArray> xml_ba(DOMDocumentToByteArray(doc));
	doc->release();
	XMLPlatformUtils::Terminate();

	// xml_manifest is freed by libzip when it is no longer needed
	const char *xml_manifest = strndup((char *) xml_ba->GetBytes(), xml_ba->Size());
	zip_source_t *source = zip_source_buffer(asic, xml_manifest, strlen(xml_manifest), 1);
	if (source == NULL || (index = zip_file_add(asic, "META-INF/manifest.xml", source, ZIP_FL_ENC_GUESS)) < 0) {
		zip_source_free(source);
		MWLOG(LEV_ERROR, MOD_APL, L"Failed to add manifest.xml to zip container");
		throw CMWEXCEPTION(EIDMW_XADES_UNKNOWN_ERROR);
	}

	addEntryExtendedTimestamp(asic, index, NULL);
}


/*
 * Add a mimetype file as defined in the ASIC standard ETSI TS 102 918.
 * It needs to be stored first in the archive and uncompressed so it can be used as a kind of magic
 * number for systems that use them
 */
static void addMimetype(zip_t *asic, int path_count)
{
	zip_int64_t index;
	const char *mimetype = path_count > 1 ? MIMETYPE_ASIC_E : MIMETYPE_ASIC_S;

	zip_source_t *source = zip_source_buffer(asic, mimetype, strlen(mimetype), 0);
	if (source == NULL || (index = zip_file_add(asic, "mimetype", source, ZIP_FL_ENC_GUESS)) < 0) {
		zip_source_free(source);
		MWLOG(LEV_ERROR, MOD_APL, L"Failed to add mimetype file to zip container");
		throw CMWEXCEPTION(EIDMW_XADES_UNKNOWN_ERROR);
	}

	if (zip_set_file_compression(asic, index, ZIP_CM_STORE, 0) < 0) {
		MWLOG(LEV_ERROR, MOD_APL, L"Failed to set store compression of mimetype");
	}

	addEntryExtendedTimestamp(asic, index, NULL);
}

void SigContainer::createASiC(CByteArray& sig, const char **paths, unsigned int path_count, const char *output_file)
{
	if (path_count < 1) {
		MWLOG(LEV_ERROR, MOD_APL, L"ASiC container must have at least one input file / data object");
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	MWLOG(LEV_DEBUG, MOD_APL, L"createASiC() called with output_file = %s", output_file);

	int status;
	zip_int64_t file_index;
	zip_t *asic = NULL;
	if ((asic = zip_open(output_file, ZIP_CREATE | ZIP_TRUNCATE, &status)) == NULL) {
		zip_error_t error;
		zip_error_init_with_code(&error, status);
		MWLOG(LEV_ERROR, MOD_APL, L"createASiC(): failed to create container '%s'. Error: %s",
			output_file, zip_error_strerror(&error));
		throw CMWEXCEPTION(EIDMW_PERMISSION_DENIED);
	}

	addMimetype(asic, path_count);
	addReadme(asic);
	addManifest(asic, paths, path_count);

	// solve duplicate input filenames
	std::vector<std::string> input_filenames;
	for (unsigned int i = 0; i < path_count; ++i) {
		input_filenames.push_back(Basename((char *)paths[i]));
	}

	std::vector<std::string *> unique_filenames_ptrs;
	for (unsigned int i = 0; i < path_count; ++i) {
		unique_filenames_ptrs.push_back(&input_filenames[i]);
	}
	CPathUtil::generate_unique_filenames("", unique_filenames_ptrs);

 	// add input files
	for (unsigned int i = 0; i < path_count; ++i) {
		MWLOG(LEV_DEBUG, MOD_APL, L"Adding file %s to archive", paths[i]);

		const char* zip_entry_name = input_filenames[i].c_str();
		zip_source_t *source = zip_source_file(asic, paths[i], 0, -1);
		if (source == NULL || (file_index = zip_file_add(asic, zip_entry_name, source, ZIP_FL_ENC_GUESS)) < 0) {
			zip_source_free(source);
			MWLOG(LEV_ERROR, MOD_APL, L"Failed to add %s to zip container", zip_entry_name);
			throw CMWEXCEPTION(EIDMW_XADES_UNKNOWN_ERROR);
		}
		addEntryExtendedTimestamp(asic, file_index, paths[i]);
	}

	// add signature file
	zip_source_t *source = zip_source_buffer(asic, sig.GetBytes(), sig.Size(), 0);
	if (source == NULL || (file_index = zip_file_add(asic, SIG_INTERNAL_PATH, source, ZIP_FL_ENC_GUESS)) < 0) {
		zip_source_free(source);
		MWLOG(LEV_ERROR, MOD_APL, L"Failed to add signature to zip container");
		throw CMWEXCEPTION(EIDMW_XADES_UNKNOWN_ERROR);
	}

	addEntryExtendedTimestamp(asic, file_index, NULL);

	if (zip_close(asic) < 0) {
		MWLOG(LEV_ERROR, MOD_APL, L"zip_close() failed with error msg: %s",
			zip_error_strerror(zip_get_error(asic)));
		free(asic);
		throw CMWEXCEPTION(EIDMW_PERMISSION_DENIED);
	}

}

static bool check_mimetype(zip_t *container)
{
	zip_int64_t index = -1;
	if ((index = zip_name_locate(container, "mimetype", 0)) == -1) {
		MWLOG(LEV_DEBUG, MOD_APL, L"check_mimetype(): mimetype file is missing");
		return false;
	}

	zip_file_t *mimetype_file;
	if ((mimetype_file = zip_fopen_index(container, index, 0)) == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, L"check_mimetype(): failed to open mimetype from container");
		return false;
	}

	const size_t BUFSIZE = strlen(MIMETYPE_ASIC_S);
	std::vector<char> mimetype_buf(BUFSIZE + 1);
	if ((zip_fread(mimetype_file, &mimetype_buf[0], BUFSIZE)) < 0) {
		MWLOG(LEV_ERROR, MOD_APL, L"check_mimetype(): zip_fread() failed");
		zip_fclose(mimetype_file);
		return false;
	}

	if (strcmp(&mimetype_buf[0], MIMETYPE_ASIC_S) != 0 && strcmp(&mimetype_buf[0], MIMETYPE_ASIC_E) != 0) {
		MWLOG(LEV_DEBUG, MOD_APL, L"check_mimetype(): mimetype does not match ASIC-S or ASIC-E");
		zip_fclose(mimetype_file);
		return false;
	}

	zip_fclose(mimetype_file);
	return true;
}

static bool isASiC(const char *path, bool write_mode, zip_t **out_container)
{
	int status;
	if ((*out_container = zip_open(path, write_mode ? 0 : ZIP_RDONLY, &status)) == NULL) {
		zip_error_t error;
		zip_error_init_with_code(&error, status);
		MWLOG(LEV_DEBUG, MOD_APL, "isZip(): Failed to open '%s': %s", path, zip_error_strerror(&error));
		return false;
	}

	bool isASiC = check_mimetype(*out_container);

	if (!isASiC) {
		if (zip_close(*out_container) < 0) {
			MWLOG(LEV_ERROR, MOD_APL, "zip_close() failed with error msg: %s",
				zip_error_strerror(zip_get_error(*out_container)));
			free(*out_container);
		}

		*out_container = NULL;
	}

	return isASiC;
}

std::vector<std::string> SigContainer::listInputFiles()
{
	zip_t *container;
	if (!isASiC(m_path.c_str(), false, &container) || container == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, "SigContainer::listInputFiles() %s, is not an ASiC container",
			m_path.c_str());
		throw CMWEXCEPTION(EIDMW_XADES_INVALID_ASIC_ERROR);
	}

	std::vector<std::string> paths;
	const zip_int64_t entries = zip_get_num_entries(container, 0);

	const char *path = NULL;
	for (zip_int64_t i = 0; i < entries; ++i) {
		if ((path = zip_get_name(container, i, ZIP_FL_ENC_RAW)) == NULL) {
			MWLOG(LEV_ERROR, MOD_APL, "zip_get_name() failed to get file name");
			throw CMWEXCEPTION(EIDMW_XADES_UNKNOWN_ERROR);
		}
		if (!StartsWith(path, "META-INF/") && strcmp(path, "mimetype") != 0) {
			paths.push_back(path);
		}
	}

	if (zip_close(container) < 0) {
		MWLOG(LEV_ERROR, MOD_APL, "%s: zip_close() failed with error msg: %s", __FUNCTION__,
			zip_error_strerror(zip_get_error(container)));
		free(container);
		throw CMWEXCEPTION(EIDMW_XADES_UNKNOWN_ERROR);
	}

	return paths;
}

void SigContainer::extract(const char * filename, const char * out_dir)
{
	zip_t *container;
	bool write_mode = false;
	if (!isASiC(m_path.c_str(), write_mode, &container) || container == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, "%s: File %s is not an ASiC container", __FUNCTION__,
			m_path.c_str());
		throw CMWEXCEPTION(EIDMW_XADES_INVALID_ASIC_ERROR);
	}

	FILE * fp_out = NULL;
	
	const int BUFSIZE = 4*1024;
	
	zip_file_t *zf;
	zip_stat_t zstat = {0};
	if (zip_stat(container, filename, 0, &zstat) != 0) {
		MWLOG(LEV_ERROR, MOD_APL, "%s: zip_stat() failed", __FUNCTION__);
		throw CMWEXCEPTION(EIDMW_FILE_NOT_OPENED);
	}
	if ((zf = zip_fopen(container, filename, 0)) == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, "%s: zip_fopen_index() failed", __FUNCTION__);
		throw CMWEXCEPTION(EIDMW_FILE_NOT_OPENED);
	}
	char buffer[BUFSIZE];
	size_t ret = 0;
	zip_int64_t read = 0, sum = 0;
	fp_out = CPathUtil::openFileForWriting(out_dir, filename);

	while (sum != zstat.size) {
		
		if ((read = zip_fread(zf, buffer, BUFSIZE)) < 0) {
			MWLOG(LEV_ERROR, MOD_APL, "%s: zip_fread() failed", __FUNCTION__);
			fclose(fp_out);
			zip_fclose(zf);
			throw CMWEXCEPTION(EIDMW_XADES_UNKNOWN_ERROR);
		}
		ret = fwrite(buffer, sizeof(char), read, fp_out);
		if (ret < read) {
			MWLOG(LEV_ERROR, MOD_APL, "%s: I/O error writing to extracted file. Detail: %s",
			 __FUNCTION__, strerror(errno));
			fclose(fp_out);
			zip_fclose(zf);
			throw CMWEXCEPTION(EIDMW_XADES_UNKNOWN_ERROR);
		}

		sum += read;
	}
	zip_fclose(zf);
	fclose(fp_out);
	
	zip_close(container);

}

const char *SigContainer::getNextSignatureFileName()
{
	zip_t *container;
	if (!isASiC(m_path.c_str(), false, &container) || container == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, "SigContainer::getNextSignatureFileName() %s,"
			" is not an ASiC container", m_path.c_str());
		throw CMWEXCEPTION(EIDMW_XADES_INVALID_ASIC_ERROR);
	}

	unsigned int count = 1;
	const size_t LENGTH = 35;
	const char *format = "META-INF/signatures%03d.xml";
	char *path = (char *) calloc(LENGTH, sizeof(char));
	do {
		snprintf(path, LENGTH, format, count++);
	} while (zip_name_locate(container, path, 0) != -1);

	if (zip_close(container) < 0) {
		MWLOG(LEV_ERROR, MOD_APL, "zip_close() failed with error msg: %s",
			zip_error_strerror(zip_get_error(container)));
		free(container);
		throw CMWEXCEPTION(EIDMW_XADES_UNKNOWN_ERROR);
	}

	return path;
}

void SigContainer::addSignature(const char *signatureFilename, const CByteArray& signature)
{
	zip_t *container;
	if (!isASiC(m_path.c_str(), true, &container) || container == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, "SigContainer::addSignature(): %s"
			" is not an ASiC container", m_path.c_str());
		throw CMWEXCEPTION(EIDMW_XADES_INVALID_ASIC_ERROR);
	}

	zip_source_t *source = zip_source_buffer(container, signature.GetBytes(), signature.Size(), 0);
	if (source == NULL || zip_file_add(container, signatureFilename, source, ZIP_FL_ENC_GUESS) < 0) {
		zip_source_free(source);
		MWLOG(LEV_ERROR, MOD_APL, L"Failed to add signature to zip container");
		throw CMWEXCEPTION(EIDMW_XADES_UNKNOWN_ERROR);
	}

	if (zip_close(container) < 0) {
		MWLOG(LEV_ERROR, MOD_APL, "%s: zip_close() failed with error msg: %s", __FUNCTION__,
			zip_error_strerror(zip_get_error(container)));
		free(container);
		throw CMWEXCEPTION(EIDMW_PERMISSION_DENIED);
	}
}

};
