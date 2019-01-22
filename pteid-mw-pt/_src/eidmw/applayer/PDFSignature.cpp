#include "poppler/Error.h"
#include "poppler/PDFDoc.h"
#include "poppler/Page.h"
#include "poppler/ErrorCodes.h"

#include "PDFSignature.h"
#include "MWException.h"
#include "eidErrors.h"
#include "MiscUtil.h"
#include "CardPteidDef.h"
#include "Log.h"
#include "Util.h"
#include "APLConfig.h"

#include <string>

//For the setSSO calls
#include "CardLayer.h"
#include "goo/GooString.h"

#include <openssl/bio.h>
#include <openssl/x509.h>
#include "sign-pkcs7.h"

namespace eIDMW
{

	const double PDFSignature::sig_height = 90;
	const double PDFSignature::sig_width = 178;
	const double PDFSignature::tb_margin = 40;


	PDFDoc * makePDFDoc(const char *utf8Filepath) {
#ifdef WIN32
		std::string utf8Filename(utf8Filepath);
		std::wstring utf16Filename = utilStringWiden(utf8Filename);

		return new PDFDoc((wchar_t *)utf16Filename.c_str(), utf16Filename.size());
#else
		return new PDFDoc(new GooString(utf8Filepath));
#endif
	}

	PDFSignature::PDFSignature()
	{

		m_visible = false;
		m_page = 1;
		m_sector = 0;
		//Illegal values to start with
		location_x = -1;
		location_y = -1;
		m_civil_number = NULL;
		m_citizen_fullname = NULL;
		m_attributeSupplier = NULL;
		m_attributeName = NULL;
		m_batch_mode = true;
		m_timestamp = false;
		m_isLandscape = false;
		m_small_signature = false;
		my_custom_image.img_data = NULL;
		m_doc = NULL;
        m_card = NULL;

        m_signerInfo = NULL;
        m_pkcs7 = NULL;
        m_outputName = NULL;
        m_signStarted = false;
        m_isExternalCertificate = false;
        m_isCC = true;
        m_incrementalMode = false;
	}

	PDFSignature::PDFSignature(const char *pdf_file_path): m_pdf_file_path(pdf_file_path)
	{
        m_visible = false;
        m_page = 1;
        m_sector = 0;
        //Illegal values to start with
        location_x = -1;
        location_y = -1;
        m_civil_number = NULL;
        m_citizen_fullname = NULL;
        m_attributeSupplier = NULL;
		m_attributeName = NULL;
        m_batch_mode = false;
        m_timestamp = false;
        m_isLandscape = false;
        m_small_signature = false;
        my_custom_image.img_data = NULL;

		m_doc = makePDFDoc(pdf_file_path);

        m_card = NULL;
        m_signerInfo = NULL;
        m_pkcs7 = NULL;
        m_outputName = NULL;
        m_signStarted = false;
        m_isExternalCertificate = false;
        m_isCC = true;
        m_incrementalMode = false;
	}

	void PDFSignature::setCustomImage(unsigned char *img_data, unsigned long img_length)
	{
		this->my_custom_image.img_data = (unsigned char*)malloc(img_length);
		memcpy(this->my_custom_image.img_data, img_data, img_length);
		this->my_custom_image.img_length = img_length;
	}

	PDFSignature::~PDFSignature()
	{
		free(m_citizen_fullname);
		free(m_civil_number);

		//Free the strdup'ed strings from batchAddFile
		for (int i = 0; i != m_files_to_sign.size(); i++)
			free(m_files_to_sign.at(i).first);

		if (my_custom_image.img_data != NULL) {
			free(my_custom_image.img_data);
		}

		if (m_doc != NULL)
			delete m_doc;
	}

	void PDFSignature::setFile(char *pdf_file_path)
	{
		m_visible = false;
		m_page = 1;
		m_sector = 0;
		//Illegal values to start with
		location_x = -1;
		location_y = -1;
		m_civil_number = NULL;
		m_citizen_fullname = NULL;
		m_attributeSupplier = NULL;
		m_attributeName = NULL;
		m_batch_mode = false;
		m_timestamp = false;
		m_isLandscape = false;
		m_small_signature = false;
		my_custom_image.img_data = NULL;
		m_pdf_file_path = strdup(pdf_file_path);
		m_doc = makePDFDoc(pdf_file_path);

		m_card = NULL;
		m_signerInfo = NULL;
		m_pkcs7 = NULL;
		m_outputName = NULL;
		m_signStarted = false;
		m_isExternalCertificate = false;
	}
	void PDFSignature::batchAddFile(char *file_path, bool last_page)
	{
		m_files_to_sign.push_back(std::make_pair(_strdup(file_path), last_page));

	}

	void PDFSignature::enableTimestamp()
	{
		m_timestamp = true;
	}

	void PDFSignature::enableSmallSignature()
	{
		m_small_signature = true;
	}


	void PDFSignature::setVisible(unsigned int page_number, int sector_number)
	{
		m_visible = true;
		m_page = page_number;
		m_sector = sector_number;
	}

	void PDFSignature::setVisibleCoordinates(unsigned int page, double coord_x, double coord_y)
	{
	   m_visible = true;
	   m_page = page;
	   location_x = coord_x;
	   location_y = coord_y;

	}

	bool PDFSignature::getBatch_mode(){
        return m_batch_mode;
	}/* PDFSignature::getBatch_mode() */

	void PDFSignature::setBatch_mode( bool batch_mode ){
        m_batch_mode = batch_mode;
	}/* PDFSignature::setBatch_mode() */

	bool PDFSignature::isLandscapeFormat()
	{
		if (m_doc)
        {
            if (!m_doc->isOk())
                return false;

            Page *p = m_doc->getPage(m_page);
            PDFRectangle *p_media = p->getMediaBox();

            double height = p_media->y2, width = p_media->x2;

            //Take into account page rotation
			int page_rotate = p->getRotate();
			if (page_rotate == 90 || page_rotate == 270)
            {
            	height = p_media->x2;
            	width = p_media->y2;
            }

            return width > height;
		}

		return false;

	}


	PDFRectangle PDFSignature::computeSigLocationFromSectorLandscape(double page_height, double page_width, int sector)
	{
		PDFRectangle sig_rect;
		double vert_align = 16; //Add this to vertically center inside each cell
		//Number of columns for portrait layout
		int columns = 4;
		double signature_height = m_small_signature ? sig_height / 2.0 : sig_height;
		int MAX_SECTOR = m_small_signature ? 40 : 20;
		const double n_lines = MAX_SECTOR / 4.0;

		double sig_width = (page_width - lr_margin*2) / columns;

		//Add left margin
		sig_rect.x1 = lr_margin;
		sig_rect.x2 = lr_margin;

		if (sector < 1 || sector > MAX_SECTOR)
			MWLOG(LEV_ERROR, MOD_APL, "Illegal value for signature page sector: %u Valid values [1-%d]",
					sector, MAX_SECTOR);

		if (sector < MAX_SECTOR - 3)
		{
			int line = sector / 4 + 1;
			if (sector % 4 == 0)
			   line = sector / 4;

			sig_rect.y1 += (page_height - 2*tb_margin) * (n_lines-line) / n_lines;
			sig_rect.y2 += (page_height - 2*tb_margin) * (n_lines-line) / n_lines;
		}

		int column = (sector-1) % 4;

		sig_rect.x1 += (column * sig_width);
		sig_rect.x2 += (column * sig_width);

		sig_rect.y1 += tb_margin + vert_align;

		//Define height and width of the rectangle
		sig_rect.x2 += sig_width;
		sig_rect.y2 += signature_height + tb_margin + vert_align;


		MWLOG(LEV_DEBUG, MOD_APL, "computeSigLocationFromSectorLandscape: Sector: %02d Location = (%f, %f) (%f, %f) \n", sector, sig_rect.x1, sig_rect.y1, sig_rect.x2, sig_rect.y2);

		return sig_rect;
	}


	PDFRectangle PDFSignature::computeSigLocationFromSector(double page_height, double page_width, int sector)
	{
		MWLOG(LEV_DEBUG, MOD_APL, "computeSigLocationFromSector called with sector=%d and m_small_signature = %d", sector, m_small_signature);

		int MAX_SECTOR = m_small_signature ? 36 : 18;
		double signature_height = m_small_signature ? sig_height / 2.0 : sig_height;
		const double n_lines = MAX_SECTOR / 3.0;
		// Add padding, adjust to subtly tweak the location
		// The units for x_pad, y_pad, sig_height and sig_width are postscript
		// points (1 px == 0.75 points)
		PDFRectangle sig_rect;
		double vert_align = 16; //Add this to vertically center inside each cell
		//Number of columns for portrait layout
		int columns = 3;

		double sig_width = (page_width - lr_margin*2) / columns;

		//Add left margin
		sig_rect.x1 = lr_margin;
		sig_rect.x2 = lr_margin;

		if (sector < 1 || sector > MAX_SECTOR)
			MWLOG(LEV_ERROR, MOD_APL, "Illegal value for signature page sector: %u Valid values [1-%d]",
					sector, MAX_SECTOR);

		if (sector < MAX_SECTOR -2)
		{
			int line = sector / 3 + 1;
			if (sector % 3 == 0)
			   line = sector / 3;

			sig_rect.y1 += (page_height - 2*tb_margin) * (n_lines-line) / n_lines;
			sig_rect.y2 += (page_height - 2*tb_margin) * (n_lines-line) / n_lines;
		}

		if (sector % 3 == 2 )
		{
			sig_rect.x1 += sig_width;
			sig_rect.x2 += sig_width;
		}

		if (sector % 3 == 0 )
		{
			sig_rect.x1 += sig_width * 2.0;
			sig_rect.x2 += sig_width * 2.0;
		}

		sig_rect.y1 += tb_margin + vert_align;

		//Define height and width of the rectangle
		sig_rect.x2 += sig_width;
		sig_rect.y2 += signature_height + tb_margin + vert_align;

		MWLOG(LEV_DEBUG, MOD_APL, "computeSigLocationFromSector: Sector: %02d Location = (%f, %f) (%f, %f) \n", sector, sig_rect.x1, sig_rect.y1, sig_rect.x2, sig_rect.y2);
		return sig_rect;
	}

	CByteArray PDFSignature::getCitizenCertificate()
	{
			MWLOG(LEV_DEBUG, MOD_APL, "DEBUG: getCitizenCertificate() This should only be called for Card Signature!");

            CByteArray certData;
           

            m_card->readFile(PTEID_FILE_CERT_SIGNATURE, certData);

            return certData;
	}

	void PDFSignature::setSCAPAttributes(const char * citizenName, const char * citizenId,
	                      const char * attributeSupplier, const char * attributeName) {

		m_attributeSupplier = attributeSupplier;
		m_attributeName = attributeName;
		m_citizen_fullname = (char *) citizenName;
		m_civil_number = (char *)citizenId;

	}

	void PDFSignature::parseCitizenDataFromCert(CByteArray &certData) {
		const int cert_field_size = 256;

		unsigned char * cert_data = certData.GetBytes();
		char *data_serial = (char *)malloc(cert_field_size);
        char *data_common_name = (char *)malloc(cert_field_size);

		X509 *x509 = d2i_X509(NULL, (const unsigned char**)&cert_data, certData.Size());

		if (x509 == NULL) {
			MWLOG(LEV_ERROR, MOD_APL, L"parseCitizenDataFromCert() Error decoding certificate data!");
			free(data_serial);
			free(data_common_name);
			return;
		}

		X509_NAME * subj = X509_get_subject_name(x509);
		X509_NAME_get_text_by_NID(subj, NID_serialNumber, data_serial, cert_field_size);
		X509_NAME_get_text_by_NID(subj, NID_commonName, data_common_name, cert_field_size);

		m_civil_number = data_serial;
		m_citizen_fullname = data_common_name;
		X509_free(x509);
	}

	int PDFSignature::getPageCount()  {
		if (m_doc->getErrorCode() == errEncrypted)
		{
			fprintf(stderr, "getPageCount(): Encrypted PDFs are unsupported at the moment\n");
			return -2;
		}
		if (!m_doc->isOk())
		{
			fprintf(stderr,"getPageCount(): Probably broken PDF...\n");
			return -1;
		}
		return m_doc->getNumPages();
	}

	char* PDFSignature::getOccupiedSectors(int page)
	{
		if (m_doc)
			return m_doc->getOccupiedSectors(page);
		else
			return "";
	}

	#ifdef WIN32
	#define PATH_SEP "\\"
	#else
	#define PATH_SEP "/"
	#endif
	std::string PDFSignature::generateFinalPath(const char *output_dir, const char *path)
	{
		char * pdf_filename = Basename((char*)path);
		std::string clean_filename = CPathUtil::remove_ext_from_basename(pdf_filename);

		int equal_filename_count = 0;
		for (unsigned int i = 0; i < unique_filenames.size(); i++)
		{
			std::string current_file_name = unique_filenames.at(i).first;
			if (clean_filename.compare(current_file_name) == 0) {
				//unique_filenames contains clean_filename
				equal_filename_count = ++unique_filenames.at(i).second;
				break;
			}
		}

		if (equal_filename_count == 0){
			//clean_filename is not part of the vector, make sure it's added to it
			unique_filenames.push_back(std::make_pair(clean_filename, equal_filename_count));
		}

		std::string final_path = string(output_dir) + PATH_SEP + clean_filename;

		if(equal_filename_count > 0){
			final_path += "_" + std::to_string(equal_filename_count);
		}

		final_path += "_signed.pdf";

		return final_path;
	}

	std::string PDFSignature::getDocName() {
		char * pdf_filename = Basename((char *)m_pdf_file_path);
		std::string clean_filename = CPathUtil::remove_ext_from_basename(pdf_filename);

		return clean_filename.size() > 44 ? clean_filename.substr(0, 44) : clean_filename;
	}

	int PDFSignature::signFiles(const char *location,
		const char *reason, const char *outfile_path)
	{
		int rc = 0;

		if (!m_batch_mode)
		{
			rc = signSingleFile(location, reason, outfile_path);
		}
		//PIN-Caching is ON after the first signature
		else
		{
			 for (unsigned int i = 0; i < m_files_to_sign.size(); i++)
			 {
				 char *current_file = m_files_to_sign.at(i).first;
				 std::string f = generateFinalPath(outfile_path,
						 current_file);

				 m_doc = makePDFDoc(current_file);
				 //Set page as the last
				 if (m_files_to_sign.at(i).second)
				 	m_page = m_doc->getNumPages();

				rc += signSingleFile(location, reason, f.c_str());
				if (i == 0)
					 m_card->getCalReader()->setSSO(true);

			 }
			 m_card->getCalReader()->setSSO(false);
		}

		return rc;

	}

	int PDFSignature::getOtherPageCount(const char *input_path)
	{
		GooString filename(input_path);
		PDFDoc doc(new GooString(input_path));

		if (doc.getErrorCode() == errEncrypted)
		{
		    fprintf(stderr,
		        "getOtherPageCount(): Encrypted PDFs are unsupported at the moment\n");
		    return -2;
		}
		if (!doc.isOk())
		{
			fprintf(stderr, "getOtherPageCount(): Probably broken PDF...\n");
			return -1;
		}

		return doc.getNumPages();
	}
	

	int PDFSignature::signSingleFile(const char *location,
		const char *reason, const char *outfile_path)
	{

		bool isLangPT = false;
		PDFDoc *doc = m_doc;
		//The class ctor initializes it to (0,0,0,0)
		//so we can use this for invisible sig
		PDFRectangle sig_location;
		const char * signature_contents = NULL;

		APL_Config config_language(CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE);

		//Default value of language config is PT
		if (wcscmp(config_language.getWString(),
			      CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE.csDefault) == 0)
			isLangPT = true;

		GooString *outputName;
		outputName = new GooString(outfile_path);

		if (!doc->isOk()) {
			fprintf(stderr, "Poppler returned error loading PDF document %s\n",
					doc->getFileName()->getCString());

			delete outputName;
			throw CMWEXCEPTION(EIDMW_PDF_INVALID_ERROR);
		}

		if (doc->isEncrypted())
		{
			fprintf(stderr, "Error: Encrypted PDF \n");
			delete outputName;
			throw CMWEXCEPTION(EIDMW_PDF_UNSUPPORTED_ERROR);
		}

		if (m_page > (unsigned int)doc->getNumPages())
		{
			fprintf(stderr, "Error: Signature Page %u is out of bounds for document %s",
				m_page, doc->getFileName()->getCString());
			throw CMWEXCEPTION(EIDMW_PDF_INVALID_PAGE_ERROR);
		}

		// A little ugly hack:
		//Force the parsing of the Page Tree structure to get the pagerefs loaded in Catalog object
		Page *p = doc->getPage(m_page);

		if (p == NULL)
		{
			fprintf(stderr, "Failed to get page from PDFDoc object\n");
			throw CMWEXCEPTION(EIDMW_PDF_INVALID_ERROR);
		}

		//By the spec, the visible/writable area can be cropped by the CropBox, BleedBox, etc...
		//We're assuming the most common case of MediaBox matching the visible area
		PDFRectangle *p_media = p->getMediaBox();

		double height = p_media->y2, width = p_media->x2;
		m_isLandscape = isLandscapeFormat();

		//Fix dimensions for the /Rotate case
		if (p->getRotate() == 90 || p->getRotate() == 270)
		{
			double dim1 = height;
			height = width;
			width = dim1;
		}

		if (m_visible)
		{
			MWLOG(LEV_DEBUG, MOD_APL, L"PDFSignature: Visible signature selected. Page mediaBox: (H: %f W:%f) Location_x: %f, location_y: %f",
				 height, width, location_x, location_y);

			//Sig Location by sector
			if (location_x == -1)
			{
				if (m_isLandscape)
					sig_location = computeSigLocationFromSectorLandscape(height, width, m_sector);
				else
					sig_location = computeSigLocationFromSector(height, width, m_sector);
			}
			else
			{
				//"Round down" to a legal value to make sure we don't get partially off-page signatures
				if (location_y > 1.0)
					location_y = 1.0;

			    double sig_width = PDFSignature::sig_width;
			    double actual_sig_height =  m_small_signature ? sig_height / 2.0 : sig_height;
			    //sig_location.x1 = lr_margin+ (width-lr_margin*2)*location_x;
			    sig_location.x1 = (width)*location_x;

			    //Coordinates from the GUI are inverted => (1- location_y)
			    sig_location.y1 = (height) *(1.0-location_y);

			    sig_location.x2 = sig_location.x1 + sig_width;
			    sig_location.y2 = sig_location.y1 + actual_sig_height;

				// When batch signing with some PDFs with rotated pages, the max location_x and location_y percentages in horizontal
				// and vertical pages are different. This may cause the visible signature to be off-page. If this happens,
				// translate the signature back to the page.
				if (sig_location.x2 > width) {
					sig_location.x1 = width-sig_width;
					sig_location.x2 = width;
				}
				if (sig_location.y2 > height) {
					sig_location.y1 = height-actual_sig_height;
					sig_location.y2 = height;
				}
			}

		}
		MWLOG(LEV_DEBUG, MOD_APL, "PDFSignature: Signature rectangle before rotation (if needed) (%f, %f, %f, %f)", sig_location.x1, sig_location.y1, sig_location.x2, sig_location.y2);

		if (p->getRotate() == 90)
		{
			//Apply Rotation of R: R' = [-y2, x1, -y1, x2]
			sig_location = PDFRectangle(height-sig_location.y2, sig_location.x1,
			              height-sig_location.y1, sig_location.x2);
			MWLOG(LEV_DEBUG, MOD_APL, "PDFSignature: Rotating rectangle to (%f, %f, %f, %f)", sig_location.x1, sig_location.y1, sig_location.x2, sig_location.y2);
		} else if (p->getRotate() == 270)
		{
			//Apply Rotation of R: R' = [y1, -x2, y2, -x1]
			sig_location = PDFRectangle(sig_location.y1, width-sig_location.x2,
			              sig_location.y2, width-sig_location.x1);
			MWLOG(LEV_DEBUG, MOD_APL, "PDFSignature: Rotating rectangle to (%f, %f, %f, %f)", sig_location.x1, sig_location.y1, sig_location.x2, sig_location.y2);
		}  else if (p->getRotate() == 180)
		{
			//Apply Rotation of R: R' = []
			sig_location = PDFRectangle(width-sig_location.x2, height-sig_location.y2,
			              width-sig_location.x1, height-sig_location.y1);
			MWLOG(LEV_DEBUG, MOD_APL, "PDFSignature: Rotating rectangle to (%f, %f, %f, %f)", sig_location.x1, sig_location.y1, sig_location.x2, sig_location.y2);
		}

		unsigned char *to_sign;

		if (isExternalCertificate() && m_attributeSupplier == NULL) {
			parseCitizenDataFromCert(m_externCertificate);
		}
		else {
			//Civil number and name should be only read once
			if (m_civil_number == NULL) {
		   		CByteArray signatureCert = getCitizenCertificate();
		   		parseCitizenDataFromCert(signatureCert);
		   	}
		}

		if (m_attributeSupplier != NULL) {
			doc->addSCAPAttributes(m_attributeSupplier, m_attributeName);
		}
		
        m_incrementalMode = doc->isSigned() || doc->isReaderEnabled();

		if (this->my_custom_image.img_data != NULL)
			doc->addCustomSignatureImage(my_custom_image.img_data, my_custom_image.img_length);

        doc->prepareSignature(m_incrementalMode, &sig_location, m_citizen_fullname, m_civil_number,
                                 location, reason, m_page, m_sector, isLangPT, isCC());
        unsigned long len = doc->getSigByteArray(&to_sign, m_incrementalMode);

		int rc = 0;
		try
		{
            m_outputName = outputName;

            /* Get certificate */
            CByteArray certificate;

            if (isExternalCertificate()) {
                certificate = m_externCertificate;
            }
            else {
                m_card->readFile(PTEID_FILE_CERT_SIGNATURE, certificate);
                /*
                    Trim the padding zero bytes which are useless
                    and affect the certificate digest computation
                */
                certificate.TrimRight( 0 );
            }

            computeHash(to_sign, len, certificate, m_ca_certificates);

            m_signStarted = true;
		}
		catch(CMWException e)
		{
			//Throw away the changed PDFDoc object because we might retry the signature
			//and this document is "half-signed"...
			if (e.GetError() == EIDMW_ERR_CARD_RESET)
			{
				delete m_doc;
				if (!m_batch_mode)
					m_doc = makePDFDoc(m_pdf_file_path);
			}
			throw;
		}

        if (to_sign) free(to_sign);

        if ( !isExternalCertificate()) {

            /* Get card signature from card */
            CByteArray signature = PteidSign( m_card, m_hash );
            rc = signClose( signature );
        }

        return rc;
	}

    bool PDFSignature::isCC(){
       return m_isCC;
    }

    void PDFSignature::setIsCC(bool in_IsCC){
        m_isCC = in_IsCC;
    }

    bool PDFSignature::isExternalCertificate(){
        return m_isExternalCertificate;
    }

    void PDFSignature::setIsExtCertificate(bool in_IsExternalCertificate) {
        m_isExternalCertificate = in_IsExternalCertificate;
    }

    void PDFSignature::setExternCertificate( CByteArray certificate) {
        m_externCertificate = certificate;

        setIsExtCertificate( true );
    }

    void PDFSignature::setExternCertificateCA(std::vector<CByteArray> &certificateCAS) {
        m_ca_certificates = certificateCAS;
    }

    /* Hash */
    CByteArray PDFSignature::getHash() {
        return m_hash;
    }

    void PDFSignature::setHash(CByteArray in_hash) {
        m_hash = in_hash;
    }

    void PDFSignature::computeHash(unsigned char *data, unsigned long dataLen,
                                     CByteArray certificate,
                                     std::vector<CByteArray> &certificate_cas) {

        OpenSSL_add_all_algorithms();
        if ( m_pkcs7 != NULL ) PKCS7_free( m_pkcs7 );

        /* Calculate hash */
        m_pkcs7 = PKCS7_new();

        CByteArray in_hash = computeHash_pkcs7( data, dataLen
                                                , certificate
                                                , certificate_cas
                                                , m_timestamp
                                                , m_pkcs7
                                                , &m_signerInfo );
        setHash(in_hash);
    }

    int PDFSignature::signClose(CByteArray signature) {

        if ( !m_signStarted ) {
            MWLOG( LEV_DEBUG, MOD_APL, "signClose: Signature not started" );
            return -1;
        }

        const char *signature_contents = NULL;

        if ( NULL == m_doc ) {
            fprintf( stderr, "NULL m_doc\n" );

            if ( m_pkcs7 != NULL ) 
            	PKCS7_free( m_pkcs7 );
            throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
        }

        int return_code =
            getSignedData_pkcs7( (unsigned char*)signature.GetBytes()
                                , signature.Size()
                                , m_signerInfo
                                , m_timestamp
                                , m_pkcs7
                                , &signature_contents );

        //Don't report "unknown error" exception in the case of timestamp error     
        if (return_code > 1)
        	throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);

        m_doc->closeSignature( signature_contents );

        PDFWriteMode pdfWriteMode =
            m_incrementalMode ? writeForceIncremental : writeForceRewrite;
#ifdef WIN32
		std::string utf8Filename(m_outputName->getCString());
		std::wstring utf16Filename = utilStringWiden(utf8Filename);

		int final_ret = m_doc->saveAs((wchar_t *)utf16Filename.c_str(), pdfWriteMode);
#else
		int final_ret = m_doc->saveAs(m_outputName, pdfWriteMode);
#endif
        m_signStarted = false;

        free((void *)signature_contents);

        delete m_outputName;
        m_outputName = NULL;

        delete m_doc;
        m_doc = NULL;

        PKCS7_free( m_pkcs7 );
        m_pkcs7 = NULL;

		if (final_ret == errPermission || final_ret == errOpenFile){
            throw CMWEXCEPTION(EIDMW_PERMISSION_DENIED);
        }
        else if (final_ret != errNone) {
            throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
        }

        if (return_code == 1) {
            throw CMWEXCEPTION(EIDMW_TIMESTAMP_ERROR);
        }
        return return_code;
    }
}
