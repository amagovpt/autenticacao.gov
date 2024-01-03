#include <stdio.h>
#include <iostream>
#include <cstdio>
#include "../applayer/cryptoFramework.h"
#include "../applayer/APLCertif.h"
#include "../applayer/APLReader.h"
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <vector>

using namespace eIDMW;
using namespace std;

/*
    Gets a Revocation reason from a reason code
*/
char* getRevocationReason(long reason_code){
    switch (reason_code){
    case(0):
        return "unspecified";
    case(1):
        return "keyCompromise";
    case(2):
        return "cACompromise";
    case(3):
        return "affiliationChanged";
    case(4):
        return "superseded";
    case(5):
        return "cessationOfOperation";
    case(6):
        return "certificateHold";
    case(8):
        return "removeFromCRL";
    case(9):
        return "privilegeWithdrawn";
    case(10):
        return "aACompromise";
    default:
        return "UNKOWN";
    }
}

/*
    Logs the CRL
*/
int logsCRL(X509_CRL* crl){
	// Creates a stack
	STACK_OF(X509_REVOKED) *pRevokeds = NULL;
	pRevokeds = X509_CRL_get_REVOKED(crl);

	// Creates crit
	int crit = 0;

	// Creates int64_t
	uint64_t serial_int64 = 0;

    // Number of Certificates
    int number_of_certificates = 0;

    // If there are revoked certificates
	if (pRevokeds){
		// Prints the number of certificates
		cout << "This CRL has " << sk_X509_REVOKED_num(pRevokeds) << " entries" << endl;

        // Updates the number of certificates
        number_of_certificates = sk_X509_REVOKED_num(pRevokeds);

	    // For each revoked certificate in the CRL	
		for(int i = 0; i < sk_X509_REVOKED_num(pRevokeds); i++){
			// Loads the current revoked certificate in the delta_crl
			X509_REVOKED* current_revoked = sk_X509_REVOKED_value(pRevokeds, i);

			// Gets the revoked CRL serial number 
			ASN1_INTEGER* serial_number =  (ASN1_INTEGER *)X509_REVOKED_get0_serialNumber(current_revoked);

			// Puts the ptr var to 0 and converts to int
			serial_int64 = 0;
			ASN1_INTEGER_get_uint64(&serial_int64, serial_number);
			
			// Gets the Revocation reason
			ASN1_ENUMERATED * Revocation_reason = (ASN1_ENUMERATED *)X509_REVOKED_get_ext_d2i(current_revoked, NID_crl_reason, &crit, NULL);
			
			// Converts to long
			long Revocation_reason_long = ASN1_ENUMERATED_get(Revocation_reason);
			
			// Prints the certificates and the Revocation reason
			cout << "Certificate Serial Number = " << serial_int64 << " | Revocation Reason = " << getRevocationReason(Revocation_reason_long) << endl;

		}
	}
    return number_of_certificates;
}

/*
    Loads CRL from a path
*/
void loadCRL(const char* crl_path, X509_CRL** crl){
 	/* Loads a CRL file */
    /* Creates the File Pointer*/   
    FILE* crl_fp = fopen(crl_path, "r");
    cout << "Created FILE POINTERS Successfully" << endl;

    // LOAD CRL
    *crl = d2i_X509_CRL_fp(crl_fp, NULL);
    cout << "Loaded " << crl_path << " successfully" << endl;

    // Closes both file pointers
    fclose(crl_fp);
    cout << "Closed FILE POINTER Successfully" << endl;
}


int main(int argc, char *argv[]){

    // Inits APP Layer and starts all services
    CAppLayer* app_layer = &CAppLayer::instance();
    app_layer->startAllServices();

    // Gets crypto framework
    APL_CryptoFwk * fwk = (APL_CryptoFwk *) app_layer->getCryptoFwk();
    cout << "Loaded CryptoFramework Successfully" << endl;

    // Gets test descriptions
    vector<const char*> test_descriptions = {"CRL: Hold -> Delta: Revoked", "CRL: None -> Delta: Revoked",  "CRL: None -> Delta: Hold", "CRL: Hold -> Delta: RemoveFromCRL", "Serial number not in CRL"};

    // Gets serial numbers
    vector<uint64_t> serial_nums = {5709674716273064001, 1273658538371892772, 3993204469349930623, 6961655884629467962, 42};

    // Get expected results
    vector<FWK_CertifStatus> expected_results = {FWK_CERTIF_STATUS_REVOKED, FWK_CERTIF_STATUS_REVOKED, FWK_CERTIF_STATUS_SUSPENDED, FWK_CERTIF_STATUS_VALID, FWK_CERTIF_STATUS_VALID};

    // Creates the CRL pointers
    X509_CRL* crl;
    X509_CRL* delta_crl;

    // Loads CRL
    loadCRL("crl/test.crl", &crl);
    loadCRL("crl/delta_test.crl", &delta_crl);

    // Logs those CRLs
    logsCRL(crl);
    logsCRL(delta_crl);

    // Updates the CRL
    fwk->updateCRL(crl, delta_crl);

    // Logs the updated CRL
    logsCRL(crl);

    // Runs tests
    for(int i = 0; i < test_descriptions.size(); i++){
        cout << test_descriptions.at(i) << endl;
        uint64_t serial_number = serial_nums.at(i);        
        FWK_CertifStatus expected_status =  expected_results.at(i);
        FWK_CertifStatus obtained_status =  fwk->CRLValidation(serial_number, crl);
        if(obtained_status == expected_status){
            cout << "Test passed!" << endl;
        }
        else{
            cout << "Test failed!" << endl;
            cout << "Expected " << expected_status << " but got " << obtained_status << endl;
        }
    }

    // Frees the CRL
    X509_CRL_free(crl);
    cout << "Freed production CRL" << endl;

    // Frees the delta CRL
    X509_CRL_free(delta_crl);
    cout << "Freed production delta CRL" << endl;

    // Release
    app_layer->release();
    cout << "App Layer relased" << endl;

    return 0;
}