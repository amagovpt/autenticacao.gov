#include "PDFSignatureCli.h"
#include "eidlib.h"
#include "eidErrors.h"

#include <iostream>
#include <iomanip>
#include <assert.h>
#include <cstdio>
#include <string>
#ifdef __GNUC__
#include <cstdlib>
#endif
#include "eidlib.h"
#include "eidlibdefines.h"
#include "eidlibException.h"

#define SIGNATURE_COORDINATES_XY
//#define SIGNATURE_SECTORS

using namespace std;
using namespace eIDMW;

int main(){
#if 1
    const int page = 1;
    int sector = 3;
    double coord_x = 0.65;
	double coord_y = 0.22;
	int ret = 0;

	char *in_file = (char *)"CartaoCidadao.pdf";
	const char *outfile_path = "./signed_LL_CartaoCidadao.pdf";

    string in_userId = "+351 914432445";
    string in_pin = "\x07\x06\x09\x05";
    std::string in_code = "111111";

    PTEID_InitSDK();

    PTEID_ReaderContext& readerContext = ReaderSet.getReader();

	//cout << "Using Card Reader: " << readerContext.getName() << endl;

	if ( !readerContext.isCardPresent() ){
		cout << "No card found in the reader!" << endl;
        PTEID_ReleaseSDK();
        return -1;
	}

	try{
        PTEID_EIDCard &card = readerContext.getEIDCard();
        PTEID_PDFSignature sig_handler( in_file );

        PDFSignatureCli client( &card, &sig_handler );
#if defined(SIGNATURE_COORDINATES_XY)
        ret = client.signOpen( in_userId, in_pin
                                , page
                                , coord_x, coord_y
                                , "LISBOA, PT", "Concordo com todo o conteudo - Teste", outfile_path );
#elif defined(SIGNATURE_SECTORS)
        ret = client.signOpen( in_userId, in_pin
                                , page
                                , sector, false
                                , "LISBOA, PT", "Concordo com todo o conteudo - Teste", outfile_path );
#else
    #error No signature type is defined
#endif

        if ( ret != 0 ){
            cout << "signOpen failed! - ret: " << ret << endl;
            PTEID_ReleaseSDK();
            return -2;
        }/* if ( ret != 0 ) */

        ret = client.signClose( in_code );
        if ( ret != 0 ){
            cout << "signClose failed!" << endl;
            PTEID_ReleaseSDK();
            return -3;
        }/* if ( ret != 0 ) */

		//card.SignPDF(p, page, sector, false, "LISBOA, PORTUGAL" , "Concordo com todo o conteÃºdo", outfile_path );
		//card.SignPDF(p, page, location_x, location_y, "LISBOA, PT", "Concordo com todo o conteudo", outfile_path );
		//card.SignPDF(p, page, location_x, location_y, NULL, NULL, argv[2]);

		//card.SignPDF(p, page, location_x+0.1, location_y+0.1, "LISBOA, PT", "Concordo com todo o conteudo", new_file.c_str());

	} catch (PTEID_Exception &e){
		cerr << "Caught exception in some SDK method. Error code: " << hex << e.GetError() << endl;
	}

    PTEID_ReleaseSDK();
    return 0;
#else
    char *in_file = (char *)"CartaoCidadao.pdf";
    const char *outfile_path = "./signed_LL_CartaoCidadao.pdf";

    PTEID_InitSDK();

    PTEID_ReaderContext& readerContext = ReaderSet.getReader();
    PTEID_EIDCard &card = readerContext.getEIDCard();
    PTEID_PDFSignature pdf_sig( in_file );

    const int page = 1;
    double coord_x = 0.65;
	double coord_y = 0.22;
	int ret = 0;

    ret = card.SignPDF( pdf_sig, page, coord_x, coord_y, "LISBOA, PT", "Concordo com todo o conteudo", outfile_path );

    PTEID_ReleaseSDK();
    return 0;
#endif // 0
}/* main() */
