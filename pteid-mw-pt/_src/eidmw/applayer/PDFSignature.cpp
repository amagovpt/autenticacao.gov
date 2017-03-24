#include "poppler/Error.h"
#include "poppler/PDFDoc.h"
#include "poppler/Page.h"
#include "poppler/ErrorCodes.h"

#include "sign-pkcs7.h"
#include "PDFSignature.h"
#include "MWException.h"
#include "eidErrors.h"
#include "MiscUtil.h"
#include "CardPteidDef.h"
#include "Log.h"

#include <string>

//For the setSSO calls
#include "CardLayer.h"
#include "goo/GooString.h""

#include <openssl/bio.h>
#include <openssl/x509.h>

namespace eIDMW
{

	const double PDFSignature::sig_height = 90;
	const double PDFSignature::tb_margin = 40.0;

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
		m_batch_mode = true;
		m_timestamp = false;
		m_isLandscape = false;
		m_small_signature = false;
		my_custom_image.img_data = NULL;

        m_signerInfo = NULL;
        m_pkcs7 = NULL;
        m_outputName = NULL;
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
		m_batch_mode = false;
		m_timestamp = false;
		m_isLandscape = false;
		m_small_signature = false;
		my_custom_image.img_data = NULL;
		m_doc = new PDFDoc(new GooString(pdf_file_path));

        m_signerInfo = NULL;
        m_pkcs7 = NULL;
        m_outputName = NULL;
	}

	void PDFSignature::setCustomImage(unsigned char *img_data, unsigned long img_length)
	{

		this->my_custom_image.img_data = img_data;
		this->my_custom_image.img_length = img_length;
		// this->my_custom_image.img_height = img_height;
		// this->my_custom_image.img_width = img_width;

	}

	PDFSignature::~PDFSignature()
	{
		free(m_citizen_fullname);
		free(m_civil_number);

		//Free the strdup'ed strings from batchAddFile
		for (int i = 0; i != m_files_to_sign.size(); i++)
			free(m_files_to_sign.at(i).first);

		if (m_doc != NULL)
			delete m_doc;
	}

	void PDFSignature::batchAddFile(char *file_path, bool last_page)
	{
		m_files_to_sign.push_back(std::make_pair(strdup(file_path), last_page));

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

	void PDFSignature::getCitizenData()
	{
		CByteArray certData;
		char *data_serial = (char *)malloc(256);
		char *data_common_name = (char *)malloc(256);


		m_card->readFile(PTEID_FILE_CERT_SIGNATURE, certData);

		unsigned char * cert_data = certData.GetBytes();
		X509 *x509 = d2i_X509(NULL, (const unsigned char**)&cert_data, certData.Size());

		if (x509 == NULL)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"loadCert() Error decoding certificate data!");
			free(data_serial);
			free(data_common_name);
			return;
		}
		X509_NAME * subj = X509_get_subject_name(x509);
		X509_NAME_get_text_by_NID(subj, NID_serialNumber, data_serial, 256);
		X509_NAME_get_text_by_NID(subj, NID_commonName, data_common_name, 256);

		m_civil_number = data_serial;
		m_citizen_fullname = data_common_name;
		X509_free(x509);
	}

	int PDFSignature::getPageCount()
	{
		if (!m_doc->isOk())
		{
			fprintf(stderr, "getPageCount(): Probably broken PDF...\n");
			return -1;
		}
		if (m_doc->isEncrypted())
		{
			fprintf(stderr,
				"getPageCount(): Encrypted PDFs are unsupported at the moment\n");
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

		std::string final_path = string(output_dir) + PATH_SEP + clean_filename + "_signed.pdf";

		return final_path;
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

				 m_doc = new PDFDoc(new GooString(current_file));
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

		if (!doc.isOk())
		{
			fprintf(stderr, "getOtherPageCount(): Probably broken PDF...\n");
			return -1;
		}
		if (doc.isEncrypted())
		{
			fprintf(stderr,
				"getOtherPageCount(): Encrypted PDFs are unsupported at the moment\n");
			return -1;
		}

		return doc.getNumPages();
	}


	int PDFSignature::signSingleFile(const char *location,
		const char *reason, const char *outfile_path)
	{

		PDFDoc *doc = m_doc;
		//The class ctor initializes it to (0,0,0,0)
		//so we can use this for invisible sig
		PDFRectangle sig_location;
		const char * signature_contents = NULL;

		GooString *outputName;
		outputName = new GooString(outfile_path);

		if (!doc->isOk()) {
			fprintf(stderr, "Poppler returned error loading PDF document %s\n",
					doc->getFileName()->getCString());

			delete doc;
			delete outputName;
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
		}

		if (doc->isEncrypted())
		{
			fprintf(stderr, "Encrypted PDF: This is in the TODO List\n");
			//TODO: Add proper error code(s)
			delete outputName;
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
		}

		if (m_page > (unsigned int)doc->getNumPages())
		{
			fprintf(stderr, "Error: Signature Page %u is out of bounds for document %s",
				m_page, doc->getFileName()->getCString());
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
		}

		// A little ugly hack:
		//Force the parsing of the Page Tree structure to get the pagerefs loaded in Catalog object
		Page *p = doc->getPage(m_page);

		if (p == NULL)
		{
			fprintf(stderr, "Failed to get page from PDFDoc object\n");
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
		}

		//By the spec, the visible/writable area can be cropped by the CropBox, BleedBox, etc...
		//We're assuming the most common case of MediaBox matching the visible area
		PDFRectangle *p_media = p->getMediaBox();

		double height = p_media->y2, width = p_media->x2;
		m_isLandscape = isLandscapeFormat();

		//Fix dimensions for the /Rotate case
		if (m_isLandscape && height > width)
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

				//2 different ways to calculate the same value: sig_width
			    double sig_width = width > height ? (height -lr_margin*2) / 3.0 : (width - lr_margin*2) / 3.0;
			    double actual_sig_height =  m_small_signature ? sig_height / 2.0 : sig_height;
			    //sig_location.x1 = lr_margin+ (width-lr_margin*2)*location_x;
			    sig_location.x1 = (width)*location_x;

			    //Coordinates from the GUI are inverted => (1- location_y)
			    //sig_location.y1 = tb_margin + (height-tb_margin*2) *(1.0-location_y)- actual_sig_height;
			    sig_location.y1 = (height) *(1.0-location_y);

			    sig_location.x2 = sig_location.x1 + sig_width;
			    sig_location.y2 = sig_location.y1 + actual_sig_height;
			}

		}
		MWLOG(LEV_DEBUG, MOD_APL, "PDFSignature: Signature rectangle before rotation (if needed) (%f, %f, %f, %f)", sig_location.x1, sig_location.y1, sig_location.x2, sig_location.y2);

		if (p->getRotate() == 90 || p->getRotate() == 270)
		{
			//Apply Rotation of R: R' = [-y2, x1, -y1, x2]
			sig_location = PDFRectangle(height-sig_location.y2, sig_location.x1,
			              height-sig_location.y1, sig_location.x2);
			MWLOG(LEV_DEBUG, MOD_APL, "PDFSignature: Rotating rectangle to (%f, %f, %f, %f)", sig_location.x1, sig_location.y1, sig_location.x2, sig_location.y2);
		}

		unsigned char *to_sign;

		//Civil number and name should be only read once
		if (m_civil_number == NULL)
		   getCitizenData();

		bool incremental = doc->isSigned() || doc->isReaderEnabled();

		if (this->my_custom_image.img_data != NULL)
			doc->addCustomSignatureImage(my_custom_image.img_data, my_custom_image.img_length);

		doc->prepareSignature(incremental, &sig_location, m_citizen_fullname, m_civil_number,
			location, reason, m_page, m_sector);
		unsigned long len = doc->getSigByteArray(&to_sign, incremental);

		int rc = 0;
		try
		{
            m_outputName = outputName;

            /* Get certificate */
            CByteArray certData = getCertificate();
            if ( 0 == certData.Size() ){
                m_card->readFile( PTEID_FILE_CERT_SIGNATURE, certData );
                /*
                    Encode certificate to internal format:
                    Trim the padding zero bytes which are useless
                    and affect the certificate digest computation
                */
                certData.TrimRight( 0 );
            }/* if ( 0 == certData.Size() ) */

            computeHash( to_sign, len, certData );
		}
		catch(CMWException e)
		{
			//Throw away the changed PDFDoc object because we might retry the signature
			//and this document is "half-signed"...
			if (e.GetError() == EIDMW_ERR_CARD_RESET)
			{
				delete m_doc;
				if (!m_batch_mode)
					m_doc = new PDFDoc(new GooString(m_pdf_file_path));

			}
			throw;
		}

		if (to_sign) free(to_sign);

        if ( 0 == getCertificate().Size() ){
            /* Get card signature from card */
            CByteArray signature = PteidSign( m_card, m_hash );
            rc = PDF_close( signature );
        }/* if ( 0 == getCertificate().Size() ) */

		return rc;

	}/* PDFSignature::signSingleFile() */

    /* Certificate */
    CByteArray PDFSignature::getCertificate(){
        return m_certificate;
    }/* PDFSignature::getCertificate() */

    void PDFSignature::setCertificate( CByteArray certificate ){
        m_certificate = certificate;
    }/* PDFSignature::setCertificate() */

    /* Hash */
    CByteArray PDFSignature::getHash(){
        return m_hash;
    }/* PDFSignature::getHash() */

    void PDFSignature::setHash( CByteArray in_hash ){
        m_hash = in_hash;
    }/* PDFSignature::setHash() */

    void PDFSignature::computeHash( unsigned char *data
                                , unsigned long dataLen
                                , CByteArray certData ){

        OpenSSL_add_all_algorithms();
        if ( m_pkcs7 != NULL ) PKCS7_free( m_pkcs7 );

        /* Calculate hash */
        m_pkcs7 = PKCS7_new();
        bool isCardAction = ( getCertificate().Size() == 0 );
        CByteArray in_hash = computeHash_pkcs7( m_card
                                                , data, dataLen
                                                , certData
                                                , m_timestamp
                                                , m_pkcs7
                                                , &m_signerInfo
                                                , isCardAction );
        setHash( in_hash );
    }/* PDFSignature::computeHash() */

    int PDFSignature::PDF_close( CByteArray signature ){
        const char *signature_contents = NULL;

        if ( NULL == m_doc ){
            fprintf( stderr, "NULL m_doc\n" );

            if ( m_pkcs7 != NULL ) PKCS7_free( m_pkcs7 );
            throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
        }/* if ( NULL == m_doc ) */

        bool incremental = m_doc->isSigned() || m_doc->isReaderEnabled();

        int return_code =
            getSignedData_pkcs7( (unsigned char*)signature.GetBytes()
                                , signature.Size()
                                , m_signerInfo
                                , m_timestamp
                                , m_pkcs7
                                , &signature_contents );

        if ( return_code != errNone ) throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);

        m_doc->closeSignature( signature_contents );

        PDFWriteMode pdfWriteMode =
            incremental ? writeForceIncremental : writeForceRewrite;

        int final_ret = m_doc->saveAs( m_outputName, pdfWriteMode );
        if ( final_ret != errNone ) throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);

        free((void *)signature_contents);

        delete m_outputName;
        m_outputName = NULL;

        delete m_doc;
        m_doc = NULL;

        PKCS7_free( m_pkcs7 );
        m_pkcs7 = NULL;

        return return_code;
    }/* PDFSignature::PDF_close() */
}
