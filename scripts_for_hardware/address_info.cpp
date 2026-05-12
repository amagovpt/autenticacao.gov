#include "address_info.h"

#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>

namespace ccapp {

static std::string escapeJson(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"' || c == '\\' || ('\x00' <= c && c <= '\x1f')) {
            if (c == '"') out += "\\\"";
            else if (c == '\\') out += "\\\\";
        } else {
            out += c;
        }
    }
    return out;
}

std::string readAddressInfo(eIDMW::PTEID_EIDCard& eidCard, const std::string& jsonPath) {
    eIDMW::PTEID_Pins& pins = eidCard.getPins();
    eIDMW::PTEID_Pin& addressPin = pins.getPinByPinRef(eIDMW::PTEID_Pin::ADDR_PIN);

    std::string myAddressPin;
    const char* pinEnv = std::getenv("EID_ADDRESS_PIN");
    
    if (pinEnv != nullptr && pinEnv[0] != '\0') {
        myAddressPin = pinEnv;
        std::cout << "Using Address PIN from environment variable." << std::endl;
    } else {
        std::cout << "Enter your Address PIN: ";
        std::cin >> myAddressPin;
    }

    unsigned long triesLeft = 0;
    bool isPinValid = addressPin.verifyPin(myAddressPin.c_str(), triesLeft, false);

    if (!isPinValid) {
        std::cerr << "Invalid Address PIN! Tries left: " << triesLeft << std::endl;
        return "";
    }

    std::cout << "Address PIN verified successfully!" << std::endl;

    eIDMW::PTEID_Address& addressInfo = eidCard.getAddr();
    
    std::string district = addressInfo.getDistrict();
    std::string municipality = addressInfo.getMunicipality();
    std::string parish = addressInfo.getCivilParish();
    std::string street = addressInfo.getStreetName();
    std::string doorNo = addressInfo.getDoorNo();
    std::string zip4 = addressInfo.getZip4();
    std::string zip3 = addressInfo.getZip3();

    std::cout << "\n--- Address Information ---" << std::endl;
    std::cout << "District:     " << district << std::endl;
    std::cout << "Municipality: " << municipality << std::endl;
    std::cout << "Parish:       " << parish << std::endl;
    std::cout << "Street:       " << street << std::endl;
    std::cout << "Door/Bld. No: " << doorNo << std::endl;
    std::cout << "Zip Code:     " << zip4 << "-" << zip3 << std::endl;
    std::cout << "---------------------------\n" << std::endl;

    // Save JSON
    std::string jsonAddressProperties = std::string();
        jsonAddressProperties.append("    \"district\": \" "+ escapeJson(district) + "\",\n");
        jsonAddressProperties.append("    \"municipality\": \""+ escapeJson(municipality) + "\",\n");
        jsonAddressProperties.append("    \"parish\": \"" + escapeJson(parish) + "\",\n");
        jsonAddressProperties.append("    \"street\": \"" + escapeJson(street) + "\",\n");
        jsonAddressProperties.append("    \"doorNo\": \"" + escapeJson(doorNo) + "\",\n");
        jsonAddressProperties.append("    \"zip4\": \"" + escapeJson(zip4) + "\",\n");
        jsonAddressProperties.append("    \"zip3\": \"" + escapeJson(zip3) + "\"\n");
    return jsonAddressProperties;
}

}