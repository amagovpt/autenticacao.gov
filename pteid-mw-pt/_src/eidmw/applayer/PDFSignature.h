#ifndef __PDF_SIGNATURE_H
#define __PDF_SIGNATURE_H

#include "Export.h"
#include <vector>

class PDFRectangle;
class PDFDoc;


namespace eIDMW
{

	class CReader;

	typedef struct
	{
		unsigned char * img_data;
		unsigned long img_length;
		int img_height;
		int img_width;
	} Pixmap;

	class PDFSignature
	{
	public:
		EIDMW_APL_API PDFSignature();
		EIDMW_APL_API PDFSignature(const char *path);
		EIDMW_APL_API ~PDFSignature();
		
		//Batch Operations (with PIN caching)
		EIDMW_APL_API void batchAddFile(char *file_path);
		EIDMW_APL_API void enableTimestamp();

		EIDMW_APL_API void setVisible(unsigned int page, int sector, bool is_landscape);
		EIDMW_APL_API void setVisibleCoordinates(unsigned int page, double coord_x, double coord_y);
		EIDMW_APL_API char *getOccupiedSectors(int page);
		EIDMW_APL_API int getPageCount();
		void setCard(APL_Card *card) { m_card = card; };
		
		//General interface to signing in single file-mode or batch-mode
		EIDMW_APL_API int signFiles(const char *location, const char *reason,
			const char *outfile_path);
        EIDMW_APL_API bool isLandscapeFormat();
		EIDMW_APL_API void setCustomImage(unsigned char *img_data, unsigned long img_length);

	private:

		void getCitizenData();
		std::string generateFinalPath(const char *output_dir, const char *path);
		PDFRectangle computeSigLocationFromSector(double, double, int);
		PDFRectangle computeSigLocationFromSectorLandscape(double, double, int);
		int signSingleFile(const char *location, const char *reason,
			const char *outfile_path);

		APL_Card *m_card;
		PDFDoc *m_doc;

		const char * m_pdf_file_path;
		static const double sig_height;
		static const int lr_margin = 30;
		static const double tb_margin;

		char *m_civil_number;
		char *m_citizen_fullname;
		unsigned int m_page, m_sector;
		double location_x, location_y;
		bool m_visible;
		bool m_isLandscape;
		bool m_batch_mode;
		bool m_timestamp;
		std::vector<char *> m_files_to_sign;
		Pixmap my_custom_image;
		

	};

}

#endif
