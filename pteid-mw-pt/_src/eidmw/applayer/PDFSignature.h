/*-****************************************************************************

 * Copyright (C) 2012-2014, 2017-2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2017-2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2018 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 * Copyright (C) 2019-2020 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef __PDF_SIGNATURE_H
#define __PDF_SIGNATURE_H

#include "Export.h"
#include <vector>
#include <utility>

#include "ByteArray.h"
#include "APLCard.h"
#include "PAdESExtender.h"
#include <openssl/pkcs7.h>

#define SEAL_DEFAULT_HEIGHT		90
#define SEAL_DEFAULT_WIDTH		178

#define SEAL_MINIMUM_HEIGHT		35
#define SEAL_MINIMUM_WIDTH		120

#define BIGGER(a,b) ((a)>(b)?(a):(b))

class PDFRectangle;
class PDFDoc;
class GooString;

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
		/* The no-argument constructor is the one we should use for signatures in batch mode */
		EIDMW_APL_API PDFSignature();
		EIDMW_APL_API PDFSignature(const char *path);
		PDFSignature(const PDFSignature &pdf_sig) = delete;
		void operator=(const PDFSignature&) = delete;
		EIDMW_APL_API ~PDFSignature();

		EIDMW_APL_API void setFile(const char *pdf_file_path);
		/* Batch signature operation (using in-memory PIN caching) */

		EIDMW_APL_API void batchAddFile(char *file_path, bool last_page);

        EIDMW_APL_API size_t getCurrentBatchSize();
        /* Build a "clone of" PDFSignature instance without m_files_to_sign and other options copied from this instance
           Used for batch CMD signature */
        EIDMW_APL_API PDFSignature * getSpecialCopy(size_t batch_index);

		EIDMW_APL_API void enableTimestamp();
		EIDMW_APL_API void setSignatureLevel(APL_SignatureLevel);

		EIDMW_APL_API void setVisible(unsigned int page, int sector);
		EIDMW_APL_API void setVisibleCoordinates(unsigned int page, double coord_x, double coord_y);
		EIDMW_APL_API char *getOccupiedSectors(int page);
		EIDMW_APL_API int getPageCount();
		EIDMW_APL_API int getOtherPageCount(const char *input_path);
		void setCard(APL_Card *card) { m_card = card; };

		//General interface to signing in single file-mode or batch-mode
		EIDMW_APL_API int signFiles(const char *location, const char *reason,
            const char *outfile_path, bool isCardSign);
        EIDMW_APL_API bool isLandscapeFormat();
		EIDMW_APL_API void setCustomImage(unsigned char *img_data, unsigned long img_length);
		/**
	     * Use this method to change the size of the visible signature (Minimum size: 120x35 px)
	     **/
		EIDMW_APL_API void setCustomSealSize(unsigned int width, unsigned int height);
		EIDMW_APL_API void enableSmallSignature();

		EIDMW_APL_API void setBatch_mode(bool batch_mode);

		//Returns basename without extension as required by CMD services
		EIDMW_APL_API std::string getDocName();

        bool isCC();
		EIDMW_APL_API void setIsCC( bool in_IsCC );

        /* Certificate */
		EIDMW_APL_API void setExternCertificate(CByteArray certificate);

        /* CA Certificates */
		EIDMW_APL_API void setExternCertificateCA(std::vector<CByteArray> &certificateCAS);

		EIDMW_APL_API char * getCitizenCertificateName() { return m_citizen_fullname; };

		EIDMW_APL_API char * getCitizenCertificateID() { return m_civil_number; } ;

        /* Hash */
		EIDMW_APL_API CByteArray getHash();

        void setHash( CByteArray in_hash );
        void computeHash(unsigned char *data, unsigned long dataLen,
                         CByteArray certificate,
                         std::vector<CByteArray> &CA_certificates,
                         bool isCardSign);


		EIDMW_APL_API int signClose(CByteArray signature);

		EIDMW_APL_API void setSCAPAttributes(const char * citizenName, const char * citizenId,
	                                         const char * attributeSupplier, const char * attributeName);

        EIDMW_APL_API bool addLtv();

	private:
		void parseCitizenDataFromCert(CByteArray &certData);
		CByteArray getCitizenCertificate();
		std::string generateFinalPath(const char *output_dir, const char *path);
		PDFRectangle computeSigLocationFromSector(double, double, int);
		PDFRectangle computeSigLocationFromSectorLandscape(double, double, int);
		int signSingleFile(const char *location, const char *reason,
                           const char *outfile_path, bool isCardSign);
        void save();
        void resetMembers();

		/* Certificate Data*/
		CByteArray m_certificate;

		APL_Card *m_card;
		PDFDoc *m_doc;

		std::string m_pdf_file_path;

		// Default values
		unsigned int m_sig_height = SEAL_DEFAULT_HEIGHT;
		unsigned int m_sig_width = SEAL_DEFAULT_WIDTH;
		unsigned int m_tb_margin = 40;
		static const int lr_margin = 30;

		char *m_civil_number;
		char *m_citizen_fullname;
		unsigned int m_page, m_sector;
		double location_x, location_y;
		bool m_visible;
		bool m_isLandscape;
		bool m_batch_mode;
		APL_SignatureLevel m_level;
		bool m_small_signature;
		std::vector< std::pair<char *, bool> > m_files_to_sign;
		std::vector< std::pair<std::string, int> > unique_filenames;
		Pixmap my_custom_image;

        PKCS7 *m_pkcs7;
        CByteArray m_externCertificate;
        std::vector<CByteArray> m_ca_certificates;

        CByteArray m_hash;
        PKCS7_SIGNER_INFO *m_signerInfo;
        GooString *m_outputName;
        bool m_signStarted;
        bool m_isExternalCertificate;
        bool m_isCC;
        bool m_incrementalMode;

        /* Fields for SCAP signature */
        const char * m_attributeSupplier;
        const char * m_attributeName;

        friend class PAdESExtender;
	};

}

#endif
