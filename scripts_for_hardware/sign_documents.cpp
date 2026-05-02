#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#include <eidlib.h>
#include <eidlibException.h>

namespace fs = std::filesystem;
using namespace eIDMW;

static bool shouldSign(const fs::directory_entry &entry, const fs::path &signedDir) {
    if (!entry.is_regular_file()) {
        return false;
    }

    const fs::path filePath = entry.path();
    if (filePath.parent_path() == signedDir) {
        return false;
    }

    if (filePath.extension() == ".asics") {
        return false;
    }

    return true;
}

int main(int argc, char **argv) {
    const fs::path baseDir = (argc > 1) ? fs::path(argv[1]) : fs::path("docs_for_signing");
    const fs::path signedDir = baseDir / "signed";

    try {
        if (!fs::exists(baseDir)) {
            std::cerr << "Input directory does not exist: " << baseDir << std::endl;
            return 1;
        }

        fs::create_directories(signedDir);

        std::vector<fs::path> filesToSign;
        for (const auto &entry : fs::directory_iterator(baseDir)) {
            if (shouldSign(entry, signedDir)) {
                filesToSign.push_back(entry.path());
            }
        }

        if (filesToSign.empty()) {
            std::cout << "No files found to sign in: " << baseDir << std::endl;
            return 0;
        }

        std::cout << "Initializing SDK..." << std::endl;
        PTEID_InitSDK();

        // Disable GUI elements through configuration
        try {
            PTEID_Config(PTEID_PARAM_GUITOOL_SHOWNOTIFICATION).setLong(0);
            PTEID_Config(PTEID_PARAM_GUITOOL_SHOWANIMATIONS).setLong(0);
            PTEID_Config(PTEID_PARAM_GUITOOL_SHOWSTARTUPHELP).setLong(0);
        } catch (...) {}

        PTEID_ReaderContext &readerContext = PTEID_ReaderSet::instance().getReader();
        if (!readerContext.isCardPresent()) {
            std::cerr << "No card found in the reader." << std::endl;
            PTEID_ReleaseSDK();
            return 1;
        }

        PTEID_EIDCard &eidCard = readerContext.getEIDCard();
        std::cout << "Card successfully connected." << std::endl;

        std::cout << "Locking card for signing transaction..." << std::endl;
        readerContext.BeginTransaction();

        try {
            const char *pinEnv = std::getenv("EID_SIGN_PIN");
            if (pinEnv != nullptr && pinEnv[0] != '\0') {
                PTEID_Pins &pins = eidCard.getPins();
                PTEID_Pin &signPin = pins.getPinByPinRef(PTEID_Pin::SIGN_PIN);
                unsigned long triesLeft = 0;

                if (!signPin.verifyPin(pinEnv, triesLeft, false)) {
                    std::cerr << "Signature PIN verification failed. Tries left: " << triesLeft << std::endl;
                    readerContext.EndTransaction();
                    PTEID_ReleaseSDK();
                    return 1;
                }
                std::cout << "Signature PIN verified successfully." << std::endl;
            }

            std::cout << "Signing " << filesToSign.size() << " file(s)..." << std::endl;

            int okCount = 0;
            int failCount = 0;

            for (const auto &filePath : filesToSign) {
                const std::string inPath = filePath.string();
                bool isPdf = (filePath.extension() == ".pdf");
                
                std::string outPath;
                if (isPdf) {
                    outPath = (signedDir / filePath.filename()).string();
                } else {
                    outPath = (signedDir / (filePath.filename().string() + ".asics")).string();
                }

                try {
                    if (isPdf) {
                        // PDF Signing (PAdES)
                        PTEID_PDFSignature pdfSig(inPath.c_str());
                        // page 0 = last page, sector 3 = top-right
                        eidCard.SignPDF(pdfSig, 0, 3, false, "Portugal", "Assinatura Digital", outPath.c_str());
                    } else {
                        // Generic Signing (ASiC-S / XAdES)
                        const char *paths[1] = {inPath.c_str()};
                        eidCard.SignXades(outPath.c_str(), paths, 1, PTEID_LEVEL_BASIC);
                    }
                    
                    std::cout << "OK: " << filePath.filename().string() << " -> " << outPath << std::endl;
                    ++okCount;
                } catch (PTEID_Exception &e) {
                    std::cerr << "FAILED: " << filePath.filename().string() << " (Error code: " << e.GetError() << ")" << std::endl;
                    ++failCount;
                }
            }

            readerContext.EndTransaction();
            PTEID_ReleaseSDK();

            std::cout << "Done. Success: " << okCount << ", Failed: " << failCount << std::endl;
            return failCount == 0 ? 0 : 2;

        } catch (...) {
            readerContext.EndTransaction();
            throw;
        }

    } catch (PTEID_Exception &e) {
        std::cerr << "SDK Exception (Error code: " << e.GetError() << ")" << std::endl;
        std::cerr << "SDK message: " << e.GetMessage() << std::endl;
        if (e.GetError() == EIDMW_ERR_NO_READER) {
            std::cerr << "No smart card reader detected. Check the reader connection, pcscd, and middleware installation." << std::endl;
        }
        PTEID_ReleaseSDK();
        return 1;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        PTEID_ReleaseSDK();
        return 1;
    }
}
