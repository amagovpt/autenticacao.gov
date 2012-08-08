#include "poppler/Error.h"
#include "poppler/PDFDoc.h"
#include "sign-pkcs7.h"
#include "goo/GooString.h"
#include "PDFSignature.h"
#include "MWException.h"
#include "eidErrors.h"

namespace eIDMW
{

	PDFSignature::PDFSignature(APL_Card &card, const char *pdf_file_path): m_card(card), m_pdf_file_path(pdf_file_path)
	{
		// Initialize this Poppler global object 
		// is mandatory I think
		//globalParams = new GlobalParams();
	
	}

	PDFSignature::~PDFSignature()
	{
	}

	void PDFSignature::signFile(const char *name, const char *location,
		const char *reason, const char *outfile_path)
	{


		PDFDoc *doc;
		GooString *inputName, *outputName;

		inputName = new GooString(m_pdf_file_path);

		outputName = new GooString(outfile_path);

		doc = new PDFDoc(inputName);

		if (!doc->isOk()) {
			delete doc;
			fprintf(stderr, "Error loading document !\n");
			return;
		}

		if (doc->isEncrypted())
		{
			//TODO: Add proper error code(s)
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
			return;
		}

		//FIXME: Another Ugly hack:
		//Force the parsing of the Page Tree structure to get the pageref for page 1 
		Page *p = doc->getPage(1);

		if (p == NULL)
			fprintf(stderr, "Oops...\n");
		unsigned char *to_sign;

		//TODO: Add the proper parameters
		doc->prepareSignature("Gervasio Palha", "Lisboa", "Concordo");

		unsigned long len = doc->getSigByteArray(&to_sign);
		const char * signature_contents = NULL;

		signature_contents = pteid_sign_pkcs7(&m_card, to_sign, len);

		doc->closeSignature(signature_contents);

		int res = doc->saveAs(outputName, writeForceRewrite);

		delete doc;
		delete outputName;

	}

}
