#ifndef PDF_SIGNATURE_CLI_H
#define PDF_SIGNATURE_CLI_H

#include "PDFSignature.h"
#include "CMDServices.h"

//using namespace  std;

namespace eIDMW{

    class PDFSignatureCli{
        public:
            PDFSignatureCli( PDFSignature *in_pdf );
            ~PDFSignatureCli();
            int signOpen( std::string in_userId
                        , std::string in_pin
                        , const char *location
                        , const char *reason
                        , const char *outfile_path );
            int signClose( std::string in_code );

        private:
            CMDServices cmdService;
            PDFSignature *pdf;
    };
}

#endif /* PDF_SIGNATURE_CLI_H */
