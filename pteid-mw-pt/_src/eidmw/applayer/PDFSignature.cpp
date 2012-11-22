#include "poppler/Error.h"
#include "poppler/PDFDoc.h"
#include "poppler/Page.h"

#include "sign-pkcs7.h"
#include "goo/GooString.h"
#include "PDFSignature.h"
#include "MWException.h"
#include "eidErrors.h"
#include "MiscUtil.h"
#include "CardPteidDef.h"
#include "Log.h"

#include <string>

//For the setSSO calls
#include "CardLayer.h"

#include <openssl/bio.h>
#include <openssl/x509.h>

namespace eIDMW
{

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
		m_doc = new PDFDoc(new GooString(pdf_file_path));
	
	}

	PDFSignature::~PDFSignature()
	{
		free(m_citizen_fullname);
		free(m_civil_number);

		//Free the strdup'ed strings from batchAddFile
		for (int i = 0; i != m_files_to_sign.size(); i++)
			free(m_files_to_sign.at(i));
	}

	void PDFSignature::batchAddFile(char *file_path)
	{
		m_files_to_sign.push_back(strdup(file_path));

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

	PDFRectangle PDFSignature::getSignatureRectangle(double page_height, double page_width)
	{
		// Add padding, adjust to subtly tweak the location 
		// The units for x_pad, y_pad, sig_height and sig_width are postscript
		// points (1 px == 0.75 points)
		PDFRectangle sig_rect;
		double vert_align = 16; //Add this to vertically center inside each cell

		double sig_width = (page_width - lr_margin*2) / 3.0;
		
		//Add left margin
		sig_rect.x1 = lr_margin;
		sig_rect.x2 = lr_margin;

		if (m_sector < 1 || m_sector > 18)
			fprintf (stderr, "Illegal value for signature page sector: %d Valid values [1-18]\n", 
					m_sector);
		
		if (m_sector < 16)
		{
			int line = m_sector / 3 + 1;
			if (m_sector % 3 == 0)
			   line = m_sector / 3;

			sig_rect.y1 += (page_height - 2*tb_margin) * (6-line) / 6.0;
			sig_rect.y2 += (page_height - 2*tb_margin) * (6-line) / 6.0;
		}

		if (m_sector % 3 == 2 )
		{
			sig_rect.x1 += sig_width;
			sig_rect.x2 += sig_width;
		}

		if (m_sector % 3 == 0 )
		{
			sig_rect.x1 += sig_width * 2.0;
			sig_rect.x2 += sig_width * 2.0;
		}
		
		sig_rect.y1 += tb_margin + vert_align;

		//Define height and width of the rectangle
		sig_rect.x2 += sig_width;
		sig_rect.y2 += sig_height + tb_margin + vert_align;
		

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
			return;
		}
		X509_NAME * subj = X509_get_subject_name(x509);
		X509_NAME_get_text_by_NID(subj, NID_serialNumber, data_serial, 256);
		X509_NAME_get_text_by_NID(subj, NID_commonName, data_common_name, 256);

		m_civil_number = data_serial;
		m_citizen_fullname = data_common_name;

	}

	int PDFSignature::getPageCount()
	{
		if (!m_doc->isOk()) {
				throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
		}
		if (m_doc->isEncrypted())
		{
			fprintf(stderr,
				"Error in getPageCount(): Encrypted PDFs are unsupported At the moment\n");
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
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

		std::string final_path = string(output_dir) + PATH_SEP +pdf_filename+ "_signed.pdf";

		return final_path;
	}

	void PDFSignature::signFiles(const char *location,
		const char *reason, const char *outfile_path)
	{

		if (!m_batch_mode)
		{
			signSingleFile(location, reason, outfile_path);

		}
		//PIN-Caching is ON after the first signature
		else
		{
			 for (int i = 0; i < m_files_to_sign.size(); i++)
			 {
				 char *current_file = m_files_to_sign.at(i);
				 std::string f = generateFinalPath(outfile_path,
						 current_file);
				 m_doc = new PDFDoc(new GooString(current_file));

				 signSingleFile(location, reason, f.c_str());
				 if (i == 0)
					 m_card->getCalReader()->setSSO(true);

			 }
			 m_card->getCalReader()->setSSO(false);
		}

	}


	void PDFSignature::signSingleFile(const char *location,
		const char *reason, const char *outfile_path)
	{
		
		PDFDoc *doc = m_doc;
		//The class ctor initializes it to (0,0,0,0)
		//so we can use this for invisible sig
		PDFRectangle sig_location;

		GooString *outputName;
		outputName = new GooString(outfile_path);
		
		doc = m_doc;

		if (!doc->isOk()) {
			delete doc;
			fprintf(stderr, "Poppler returned error loading PDF document %s\n", 
					doc->getFileName()->getCString());
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
		}

		if (doc->isEncrypted())
		{
			fprintf(stderr, "Encrypted PDF: This is in the TODO List\n");
			//TODO: Add proper error code(s)
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
		}

		if (m_page > doc->getNumPages())
		{
			fprintf(stderr, "Error: Signature Page %d is out of bounds for document %s",
				m_page, doc->getFileName()->getCString());
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
		}

		// A little ugly hack:
		//Force the parsing of the Page Tree structure to get the pagerefs loaded in Catalog object
		Page *p = doc->getPage(m_page);

		//By the spec, the visible/writable area can be cropped by the CropBox, BleedBox, etc...
		//We're assuming the most common case of MediaBox matching the visible area
		PDFRectangle *p_media = p->getMediaBox();

		double height = p_media->y2, width = p_media->x2;

		if (m_visible)
		{
			//Sig Location by sector
			if (location_x == -1)
				sig_location = getSignatureRectangle(height, width);
			else
			{
			    double sig_width = (width - lr_margin*2) / 3.0;
			    sig_location.x1 = (width-lr_margin*2)*location_x;
			    
			    //Coordinates from the GUI are inverted => (1- location_y)
			    sig_location.y1 = (height-tb_margin*2) * (1.0-location_y);  
			    sig_location.x2 = sig_location.x1 + sig_width;
			    sig_location.y2 = sig_location.y1 + sig_height;

			}

		}

		if (p == NULL)
			fprintf(stderr, "Failed to read page MediaBox\n");

		unsigned char *to_sign;
		
		//Civil number and name should be only read once
		if (m_civil_number == NULL)
		   getCitizenData();

		bool incremental = doc->isSigned();

		doc->prepareSignature(incremental, &sig_location, m_citizen_fullname, m_civil_number,
				location, reason, m_page, m_sector);
		unsigned long len = doc->getSigByteArray(&to_sign, incremental);

                const char * signature_contents = pteid_sign_pkcs7(m_card, to_sign, len);

		doc->closeSignature(signature_contents);

		if (incremental)
			doc->saveAs(outputName, writeForceIncremental);
		else
			doc->saveAs(outputName, writeForceRewrite);

		delete doc;
		delete outputName;

	}

}
