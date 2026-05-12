#include <iostream>
#include <string>
// Depending on your installation, you might need to include <eidlib/eidlib.h> instead.
#include <eidlib.h>
#include <eidlibException.h>


#include "public_info.h"

using namespace eIDMW;

int main() {
    try {
        std::cout << "Initializing SDK..." << std::endl;
        PTEID_InitSDK();

        PTEID_ReaderContext& readerContext = PTEID_ReaderSet::instance().getReader();

        if (!readerContext.isCardPresent()) {
            std::cerr << "No card found in the reader." << std::endl;
            PTEID_ReleaseSDK();
            return 1;
        }

        PTEID_EIDCard& eidCard = readerContext.getEIDCard();
        std::cout << "Card successfully connected!" << std::endl;

        ccapp::printPublicInfoAndSavePhoto(eidCard);
        //ccapp::readAddressInfo(eidCard);

        PTEID_ReleaseSDK();

    } catch (PTEID_Exception& e) {
        std::cerr << "SDK Exception: (Error code: " << e.GetError() << ")" << std::endl;
        std::cerr << "SDK message: " << e.GetMessage() << std::endl;
        if (e.GetError() == EIDMW_ERR_NO_READER) {
            std::cerr << "No smart card reader detected. Check the reader connection, pcscd, and middleware installation." << std::endl;
        }
        PTEID_ReleaseSDK();
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        PTEID_ReleaseSDK();
        return 1;
    }

    int a = 2;
    int *pa = &a;

    return 0;
}

