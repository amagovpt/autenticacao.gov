#include "CMDSignature.h"
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

using namespace std;
using namespace eIDMW;

int main()
{
	const int page = 1;
	int sector = 3;
	double coord_x = 0.65;
	double coord_y = 0.22;
	int ret = 0;

	char *in_file = (char *)"CartaoCidadao.pdf";
	const char *outfile_path = "./signed_CMD_CartaoCidadao.pdf";

	string in_userId = "+351 914432445";
	string in_pin = "0000";

	//string in_userId = "+351 987654321";
	//string in_pin = "\x01\x02\x03\x04";

	std::string sms_token = "111111";

	try {
		PTEID_PDFSignature sig_handler( in_file );

		CMDSignature cmd_signature(&sig_handler);

                CMDProxyInfo proxyinfo;

                ret = cmd_signature.signOpen( proxyinfo, in_userId, in_pin,
				page,
				coord_x, coord_y,
				"LISBOA, PT", "Concordo com todo o conteudo - Teste", outfile_path );

		if ( ret != 0 ) {
			cout << "signOpen failed! - ret: " << ret << endl;
			return -2;
		}

		ret = cmd_signature.signClose( sms_token );
		if ( ret != 0 ) {
			cout << "signClose failed!" << endl;
			return -3;
		}

	} catch (PTEID_Exception &e) {
		cerr << "Caught exception in some SDK method. Error code: " << hex << e.GetError() << endl;
	}

	return 0;
}
