#ifndef __PDF_SIGNATURE_H
#define __PDF_SIGNATURE_H

#include "Export.h"

class PDFRectangle;
namespace eIDMW
{


	class PDFSignature
	{
	public:
		EIDMW_APL_API PDFSignature(APL_Card &card, const char *path);
		EIDMW_APL_API ~PDFSignature();

		//The page is divided in 6 (or 8??) sectors
		EIDMW_APL_API void setVisible(unsigned int page, int sector);
		EIDMW_APL_API void signFile(const char *name, const char *location, const char *reason,
			const char *outfile_path);


	private:
		void getCitizenData();
		PDFRectangle getSignatureRectangle(double, double);
		APL_Card &m_card;

		const char * m_pdf_file_path;
		char *m_civil_number;
		char *m_citizen_fullname;
		unsigned int m_page, m_sector;
		bool m_visible;

	};

}

#endif
