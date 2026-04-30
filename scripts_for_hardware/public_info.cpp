#include "public_info.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

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

static std::string bytesToHex(const unsigned char* bytes, unsigned long len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (unsigned long i = 0; i < len; ++i) {
        ss << std::setw(2) << static_cast<int>(bytes[i]);
    }
    return ss.str();
}

void printPublicInfoAndSavePhoto(eIDMW::PTEID_EIDCard& eidCard, const std::string& photoPath, const std::string& jsonPath) {
    eIDMW::PTEID_EId& idInfo = eidCard.getID();

    std::string firstName = idInfo.getGivenName();
    std::string lastName = idInfo.getSurname();
    std::string fatherFirstName = idInfo.getGivenNameFather();
    std::string fatherLastName = idInfo.getSurnameFather();
    std::string motherFirstName = idInfo.getGivenNameMother();
    std::string motherLastName = idInfo.getSurnameMother();
    std::string birthDate = idInfo.getDateOfBirth();
    std::string nationality = idInfo.getNationality();
    std::string mrz1 = idInfo.getMRZ1();
    std::string mrz2 = idInfo.getMRZ2();
    std::string mrz3 = idInfo.getMRZ3();
    std::string parents = idInfo.getParents();
    std::string accidentalIndications = idInfo.getAccidentalIndications();
    std::string pan = idInfo.getDocumentPAN();
    std::string docType = idInfo.getDocumentType();
    std::string docVersion = idInfo.getDocumentVersion();

    std::cout << "\n--- Public Identity Information ---" << std::endl;
    std::cout << "Name:        " << firstName << " " << lastName << std::endl;
    std::cout << "Birth Date:  " << birthDate << std::endl;
    std::cout << "Citizen ID (MRZ1): " << mrz1 << std::endl;
    std::cout << "-----------------------------------\n" << std::endl;

    eIDMW::PTEID_PublicKey &publicKey = idInfo.getCardAuthKeyObj();
    bool isECC = publicKey.isECCPublicKey();
    eIDMW::PTEID_ByteArray &byte_array = publicKey.getCardAuthKeyModulus();
    std::string pubKeyHex = bytesToHex(byte_array.GetBytes(), byte_array.Size());

    // Save Photo (Overwrites for the current card)
    eIDMW::PTEID_Photo& photoObj = idInfo.getPhotoObj();
    eIDMW::PTEID_ByteArray& pngPhoto = photoObj.getphoto();
    std::ofstream photoFile(photoPath, std::ios::binary);
    if (photoFile.is_open()) {
        photoFile.write(reinterpret_cast<const char*>(pngPhoto.GetBytes()), pngPhoto.Size());
        photoFile.close();
    }

    // Build the new JSON object string
    std::stringstream ss;
    ss << "  {\n";
    ss << "    \"mrz1\": \"" << escapeJson(mrz1) << "\",\n";
    ss << "    \"firstName\": \"" << escapeJson(firstName) << "\",\n";
    ss << "    \"lastName\": \"" << escapeJson(lastName) << "\",\n";
    ss << "    \"fatherFirstName\": \"" << escapeJson(fatherFirstName) << "\",\n";
    ss << "    \"fatherLastName\": \"" << escapeJson(fatherLastName) << "\",\n";
    ss << "    \"motherFirstName\": \"" << escapeJson(motherFirstName) << "\",\n";
    ss << "    \"motherLastName\": \"" << escapeJson(motherLastName) << "\",\n";
    ss << "    \"birthDate\": \"" << escapeJson(birthDate) << "\",\n";
    ss << "    \"nationality\": \"" << escapeJson(nationality) << "\",\n";
    ss << "    \"mrz2\": \"" << escapeJson(mrz2) << "\",\n";
    ss << "    \"mrz3\": \"" << escapeJson(mrz3) << "\",\n";
    ss << "    \"parents\": \"" << escapeJson(parents) << "\",\n";
    ss << "    \"accidentalIndications\": \"" << escapeJson(accidentalIndications) << "\",\n";
    ss << "    \"pan\": \"" << escapeJson(pan) << "\",\n";
    ss << "    \"docType\": \"" << escapeJson(docType) << "\",\n";
    ss << "    \"docVersion\": \"" << escapeJson(docVersion) << "\",\n";
    ss << "    \"isECC\": " << (isECC ? "true" : "false") << ",\n";
    ss << "    \"publicKeyHex\": \"" << pubKeyHex << "\",\n";
    ss << "    \"photoPath\": \"" << escapeJson(photoPath) << "\"\n";
    ss << "  }";
    std::string newEntry = ss.str();

    // Check for uniqueness and Manage JSON Array
    std::ifstream inFile(jsonPath);
    std::string content;
    bool alreadyExists = false;

    if (inFile.is_open()) {
        std::stringstream buffer;
        buffer << inFile.rdbuf();
        content = buffer.str();
        inFile.close();

        // Check if MRZ1 is already in the file
        if (content.find("\"mrz1\": \"" + mrz1 + "\"") != std::string::npos) {
            std::cout << "Citizen with MRZ1 " << mrz1 << " already exists in the database. Skipping save." << std::endl;
            alreadyExists = true;
        }
    }

    if (!alreadyExists) {
        std::ofstream outFile(jsonPath);
        if (outFile.is_open()) {
            // Trim whitespace from content to help with parsing
            size_t first = content.find_first_not_of(" \t\n\r");
            size_t last = content.find_last_not_of(" \t\n\r");
            if (std::string::npos != first && std::string::npos != last) {
                content = content.substr(first, (last - first + 1));
            }

            if (content.empty() || content == "[]") {
                outFile << "[\n" << newEntry << "\n]";
            } else {
                // Find the last closing bracket
                size_t lastBracket = content.find_last_of(']');
                if (lastBracket != std::string::npos) {
                    std::string head = content.substr(0, lastBracket);
                    // Add a comma if there's already an object
                    if (head.find('{') != std::string::npos) {
                        head += ",\n";
                    }
                    outFile << head << newEntry << "\n]";
                } else {
                    outFile << "[\n" << newEntry << "\n]";
                }
            }
            outFile.close();
            std::cout << "New citizen data appended to " << jsonPath << std::endl;
        }
    }
}

}