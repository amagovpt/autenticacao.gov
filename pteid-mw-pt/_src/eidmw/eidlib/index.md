## C++ SDK Examples
See [Classes list](./annotated.html)  for full documentation.

#### Initialization
 Add the initialization and release function in a appropriate place inside your application
```cpp
#include "eidlib.h"
(...)
int main(int argc, char **argv){
	PTEID_InitSDK();
	(...)
	PTEID_ReleaseSDK();
}
```

#### Access to smartcard reader
In case there are multiple readers one can access the one with the card present as follows
```cpp
PTEID_ReaderSet& readerSet = PTEID_ReaderSet::instance();
for( int i=0; i < readerSet.readerCount(); i++){
	PTEID_ReaderContext& context = readerSet.getReaderByNum(i);
	if (context.isCardPresent()){
		PTEID_EIDCard &card = context.getEIDCard();
		(...)
	}
}
```

Or accessing reader directly as follows
```cpp
PTEID_ReaderContext &readerContext = PTEID_ReaderSet.instance().getReader();
```

#### Get card holder information
Obtain the name or the address of a card holder
```cpp
PTEID_EIDCard &card = readerContext.getEIDCard();
PTEID_EId& eid = card.getID();

std::string nome = eid.getGivenName();
std::string nrCC = eid.getDocumentNumber();
(...)
```
#### Get picture of a card holder

```cpp
(...)
PTEID_EIDCard &card = readerContext.getEIDCard();
PTEID_EId& eid = card.getID();
PTEID_Photo& photoObj = eid.getPhotoObj();
PTEID_ByteArray& praw = photoObj.getphotoRAW();	//  jpeg2000 format
PTEID_ByteArray& ppng = photoObj.getphoto();	// PNG format
```

#### Get address information
Ask the user to introduce the address pin the obtain the address
```cpp
PTEID_EIDCard &card = readerContext.getEIDCard();
unsigned long triesLeft;
(...)
PTEID_Pins &pins = card.getPins();
PTEID_Pin &pin = pins.getPinByPinRef(PTEID_Pin.ADDR_PIN);	
if (pin.verifyPin(“”, &triesLeft, true)){
	PTEID_Address &addr =  card.getAddr();
	const char * municipio =  addr.getMunicipality();
}
```

#### Get identity information in XML format
It's also possible to obtain card holder information in XML format, as follows
```cpp
PTEID_EIDCard &card = readerContext.getEIDCard();
unsigned long triesLeft;
(...)
card.getPins().getPinByPinRef(PTEID_Pin::ADDR_PIN).verifyPin("", triesLeft, true);
PTEID_XmlUserRequestedInfo requestedInfo;
requestedInfo.add(XML_CIVIL_PARISH); 
(...) 
requestedInfo.add(XML_GENDER); 
PTEID_CCXML_Doc &ccxml = card.getXmlCCDoc(requestedInfo);
const char * resultXml = ccxml.getCCXML();
```

#### How to read or write personal notes
The SDK allows to write or read notes. Read doesn't require any permission, however when writing the authentication pin needs to be requested.
```cpp
PTEID_EIDCard &card  = readerContext.getEIDCard();
std::string notes("a few notes");
PTEID_ByteArray personalNotes(notes.c_str(), notes.size() + 1);
bool bOk;
(...)
// read
char *my_notes = card.readPersonalNotes(); 
// write
bOk = card.writePersonalNotes(personalNotes, card.getPins().getPinByPinRef(PTEID_Pin.AUTH_PIN)); 
```


#### Verify and Change PINs
For example, change the address pin. Ask to verify the pin then change the pin itself. 
```cpp
PTEID_EIDCard &card  = readerContext.getEIDCard();
unsigned long triesLeft;
(...)
PTEID_Pins &pins = card.getPins();
PTEID_Pin &pin = pins.getPinByPinRef(PTEID_Pin.ADDR_PIN);	
if (pin.verifyPin(“”, &triesLeft, true)){
	bool bResult = pin.changePin("","", triesLeft, pin.getLabel());
	if (!bResult && -1 == triesLeft) return;
}

```

## Signatures with eID card (citizen signature certificate)
Make sure to initialize the underlying classes that will be used to sign documents, as follows
#### How to sign PDF documents (PAdES)
```cpp

#include “eidlib.h”
(...)
PTEID_EIDCard &card = readerContext.getEIDCard();
//PDF file to sign
PTEID_PDFSignature signature(“/home/user/input.pdf”);

/* Optional settings for the signature */

// Apply a visible signature with half the height of regular visible signatures
signature.enableSmallSignatureFormat();  

/* Signature profile according to PAdES specification
PAdES-B:   PTEID_SignatureLevel::PTEID_LEVEL_BASIC (default level)
PAdES-T:   PTEID_SignatureLevel::PTEID_LEVEL_TIMESTAMP
PAdES-LT:  PTEID_SignatureLevel::PTEID_LEVEL_LT
PAdES-LTA: PTEID_SignatureLevel::PTEID_LEVEL_LTV */
signature.setSignatureLevel(PTEID_SignatureLevel::PTEID_LEVEL_BASIC);

// add custom image to the signature - it can't be used in combination with enableSmallSignatureFormat() method
// image_data should point to a JPEG format image with recommended dimensions 185x41 px
unsigned char *image_data;
unsigned long image_length;
signature.setCustomImage(image_data,image_length);

//Select the location of visible signature in the document
//X and Y coordinates of the signature location as percentage of page width and height [0-1]
//To apply an invisible signature negative values should be specified for both parameters, e.g. -1
double location_x = 0.2;
double location_y = 0.2;
int page = 1;

const char * location = “Lisboa, Portugal”;
const char * reason = “Agree with the content in the file”;

//If the instance of PTEID_PDFSignature is in "batch mode" (see next section) output should have the output directory path instead of a file path
const char * output = “/home/user/output_signed.pdf”;
card.SignPDF(signature,  page, sector, location_x, location_y, location, reason, output_file);

```

#### How to sign multiple PDF documents
It's possible to sign multiple PDF files in batch mode with a single PIN prompt, as follows
```cpp
#include “eidlib.h”
(...)
PTEID_EIDCard &card = readerContext.getEIDCard();
//Initialize the signature with a file as input
PTEID_PDFSignature signature(“/home/user/input.pdf”);

//Add other files to the batch signature before invoking card.SignPDF()
signature.addToBatchSigning( “Other_File.pdf” );
signature.addToBatchSigning( “Yet_Another_FILE.pdf” );
(...)

double location_x = 0.2;
double location_y = 0.2;
int page = 1;

const char * location = “Lisboa, Portugal”;
const char * reason = “I agree with the content in the file”;

//In this case the output is a destination directory
const char * output = “/home/user/signed_documents/”;
card.SignPDF(signature,  page, location_x, location_y, is_landscape, location, reason, output_file);

```
Note that when signing multiple files the signature profile selected with setSignatureLevel(), and the SignPDF() parameters `reason`, `location`, `page`, `location_x` and `location_y` are applied to all documents.

#### Verify a PAdES signature 

One can verify a PAdES signature using the Java iText SDK.
See [iText-example](https://github.com/itext/i7js-samples/tree/master/publications/signatures/src/test/java/com/itextpdf/samples/signatures/chapter05) for a concrete example of how to verify a signature.


#### How to sign any type of file (XAdES signature format)
Sign files in batch mode
```cpp

const char *files[] = {"test/File1.txt", 
					"test/File2.pdf", 
					"test/File3.md", 
					"test/File4.cpp"};
const char *destination =”test/files_signed.zip”; //the output file that will contain all signatures 
int n_paths = 4; // size of files array

// simple signature (1 unique signature for all files)
card.SignXades(destination, files, n_paths); 
(...)
// timestamp signature (1 unique signature of type T (Timestamp) for all files)
card.SignXadesT(destination, files, n_paths); 
(...)

// archival signature (1 unique signature of type A(archival) for all files)
card.SignXadesA(destination, files, n_paths); 
(...)

```
Sign each file individually 

```cpp
const char *destination =”test/signed_files/”; // directory where all files will be saved
card.SignXadesIndividual(destinationDir, files, n_paths);
card.SignXadesTIndividual(destinationDir, files, n_paths);
card.SignXadesAIndividual(destinationDir, files, n_paths);

```
##### XAdES signature configuration
The SDK allows to use a different Timestamp authority to fetch timestamp, can be achieved as follows
```cpp
PTEID_Config config(eIDMW::PTEID_PARAM_XSIGN_TSAURL);
config.setString("http://sha1timestamp.ws.symantec.com/sha1/timestamp");
```

#### Verify a XAdES signature

One can use the Java library [DSS-Xades](https://github.com/esig/dss/tree/master/dss-xades)  to validate a XAdES signature  .
