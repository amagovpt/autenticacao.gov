#ifndef __PDF_SIGNATURE_H
#define __PDF_SIGNATURE_H

#include "Export.h"

class PDFRectangle;
class PDFDoc;

namespace eIDMW
{


	class PDFSignature
	{
	public:
		EIDMW_APL_API PDFSignature(const char *path);
		EIDMW_APL_API ~PDFSignature();

		//The page is divided in 6 (or 8??) sectors
		EIDMW_APL_API void setVisible(unsigned int page, int sector);
		EIDMW_APL_API char *getOccupiedSectors(int page);
		EIDMW_APL_API int getPageCount();

		void setCard(APL_Card *card) { m_card = card; };

		EIDMW_APL_API void signFile(const char *location, const char *reason,
			const char *outfile_path);


	private:
		void getCitizenData();
		PDFRectangle getSignatureRectangle(double, double);
		APL_Card *m_card;
		PDFDoc *m_doc;

		const char * m_pdf_file_path;

		char *m_civil_number;
		char *m_citizen_fullname;
		unsigned int m_page, m_sector;
		bool m_visible;

	};

}

#endif
