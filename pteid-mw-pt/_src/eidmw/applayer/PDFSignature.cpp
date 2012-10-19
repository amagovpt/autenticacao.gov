#include "poppler/Error.h"
#include "poppler/PDFDoc.h"
#include "poppler/Page.h"

#include "sign-pkcs7.h"
#include "goo/GooString.h"
#include "PDFSignature.h"
#include "MWException.h"
#include "eidErrors.h"
#include "CardPteidDef.h"
#include "Log.h"

#include <openssl/bio.h>
#include <openssl/x509.h>

namespace eIDMW
{

	PDFSignature::PDFSignature(const char *pdf_file_path): m_pdf_file_path(pdf_file_path)
	{
		// Initialize this Poppler global object 
		// is mandatory I think
		//globalParams = new GlobalParams();
		m_visible = false;
		m_page = 1;
		m_sector = 0;
		m_civil_number = NULL;
		m_citizen_fullname = NULL;
		m_doc = new PDFDoc(new GooString(pdf_file_path));
	
	}

	PDFSignature::~PDFSignature()
	{
	}

	void PDFSignature::setVisible(unsigned int page_number, int sector_number)
	{
		m_visible = true;
		m_page = page_number;
		m_sector = sector_number;

	}
	PDFRectangle PDFSignature::getSignatureRectangle(double page_height, double page_width)
	{
		// Add padding, adjust to subtly tweak the location 
		// The units for x_pad, y_pad, sig_height and sig_width are postscript
		// points (1 px == 0.75 points)
		PDFRectangle sig_rect;
	 	int lr_margin = 30;	
		double sig_height = 85; 

		double sig_width = (page_width - lr_margin*2) / 3.0;
		//double x_pad = 0, y_pad = (page_height/3.0-sig_height)/2.0; //Vertically Center the Signature on each sector
		double x_pad = 0, y_pad = 30.0; //Vertically Center the Signature on each sector
		
		//Add left margin
		sig_rect.x1 = lr_margin;
		sig_rect.x2 = lr_margin;




		if (m_sector < 1 || m_sector > 9)
			fprintf (stderr, "Illegal value for signature page sector: %d Valid values [1-6]\n", 
					m_sector);

		
		if (m_sector < 7)
		{
			if (m_sector >= 4) // Sectors 4 - 6
			{
				sig_rect.y1 += page_height / 3.0;
				sig_rect.y2 += page_height / 3.0;
			}
			else if (m_sector >= 1) // Sectors 1 - 3
			{
				sig_rect.y1 += page_height * 2.0 / 3.0;
				sig_rect.y2 += page_height * 2.0 / 3.0;
			}
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
		
		sig_rect.y1 += y_pad;

	//Define height and width of the rectangle
		sig_rect.x2 += sig_width;
		sig_rect.y2 += sig_height + y_pad;
		

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
		return m_doc->getNumPages();

	}

	char* PDFSignature::getOccupiedSectors(int page)
	{
		return m_doc->getOccupiedSectors(page);	
	}


	void PDFSignature::signFile(const char *location,
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
					m_pdf_file_path);
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
		}

		if (doc->isEncrypted())
		{
			fprintf(stderr, "Encrypted PDF: This is the TODO List\n");
			//TODO: Add proper error code(s)
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
		}

		if (m_page > doc->getNumPages())
		{
			fprintf(stderr, "Error: Signature Page %d is out of bounds for document %s",
				m_page, m_pdf_file_path);
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
		}

		// A little ugly hack:
		//Force the parsing of the Page Tree structure to get the pagerefs loaded in Catalog class
		Page *p = doc->getPage(m_page);

		//By the spec, the visible/writable area can be cropped by the CropBox, BleedBox, etc...
		//We're assuming the most common case of MediaBox matching the visible area
		PDFRectangle *p_media = p->getMediaBox();

		double height = p_media->y2, width = p_media->x2;

		if (m_visible)
			sig_location = getSignatureRectangle(height, width);

		if (p == NULL)
			fprintf(stderr, "Pteid Oops...\n");

		unsigned char *to_sign;

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
