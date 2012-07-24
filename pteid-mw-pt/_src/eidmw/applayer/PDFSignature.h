#ifndef __PDF_SIGNATURE_H
#define __PDF_SIGNATURE_H

#include "Export.h"

namespace eIDMW
{

	class PDFSignature
	{
	public:
		EIDMW_APL_API PDFSignature(APL_Card &card, const char *path);
		EIDMW_APL_API ~PDFSignature();
		EIDMW_APL_API void signFile(const char *name, const char *location, const char *reason,
			const char *outfile_path);


	private:
		APL_Card &m_card;
		const char * m_pdf_file_path;


	};

}

#endif