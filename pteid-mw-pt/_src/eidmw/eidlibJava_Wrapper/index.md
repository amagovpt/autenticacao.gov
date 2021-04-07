### Java SDK Examples.
Make sure that the JNI library is explicitly loaded using the following code (this implements the wrapper functions for Java)
```java
package pteidsample;
import pt.gov.cartaodecidadao.*;
(...)
static {
    try {
        System.loadLibrary("pteidlibj");
    } catch (UnsatisfiedLinkError e) {
        System.err.println("Native code library failed to load. \n" + e);
        System.exit(1);
    }
}
```

Add the initialization and release function in a appropriate place inside your application
```java
public class SamplePTEID {
    public static void main(String[] args) {
        PTEID_ReaderSet.initSDK();
        (...)
        PTEID_ReaderSet.releaseSDK();
    }
}
```

Access to the card can be achieved as follows
```java
PTEID_EIDCard card;
PTEID_ReaderContext context;
PTEID_ReaderSet readerSet;
readerSet = PTEID_ReaderSet.instance();
for( int i=0; i < readerSet.readerCount(); i++){
    context = readerSet.getReaderByNum(i);
    if (context.isCardPresent()){
        card = context.getEIDCard();
        (...)
    }
}
```
Another method of obtaining `card` is to use `getReader` directly as follows
```java
PTEID_ReaderContext readerContext = PTEID_ReaderSet.instance().getReader();
PTEID_EIDCard card = readerContext.getEIDCard();
```

#### Get events for insertion and removal of cards
The SDK allows to receive events when a card is inserted or removed using callbacks
- For that make sure to use `SetEventCallback` on `PTEID_ReaderContext` associated with the reader that you want to receive events

```java
class PteidCardCallback implements Callback {
    public void cardEvent(long lRet, long ulState, Object callbackData) {
        int cardState = (int)ulState & 0x0000FFFF;
        int eventCounter = ((int)ulState) >> 16;
        System.err.println("DEBUG: Card Event:" + " cardState: "+cardState + " Event Counter: "+ eventCounter);
        if ((cardState & 0x0100) != 0) {
            System.out.println("Card inserted");
        }
        else {
            System.out.println("Card removed");
        }
    }
}
(...)
PTEID_ReaderSet readerSet = PTEID_ReaderSet.instance();
PTEID_ReaderContext context = readerSet.getReader();
context.SetEventCallback(new PteidCardCallback(), null);
```

- The parameter `ulState` is a combination of two values
    1. event counter of the reader used
    2. a flag that contains the state of the event received, i.e., inserted or removed 
- The parameter `lRet` is an error code returned in case of failure (successful access to the reader returns 0)
- `callbackData` is a reference to the object associated with `SetEventCallback()`

#### Get identification information


```java
(...)
PTEID_EIDCard card = context.getEIDCard();
PTEID_EId eid = card.getID();
String nome = eid.getGivenName();
String nrCC = eid.getDocumentNumber();
(...)
```

#### Get citizen picture file


```java
(...)
PTEID_EIDCard card = context.getEIDCard();
PTEID_EId eid = card.getID();
PTEID_Photo photoObj = eid.getPhotoObj();
PTEID_ByteArray praw = photoObj.getphotoRAW(); // jpeg2000 format 
PTEID_ByteArray ppng = photoObj.getphoto(); // PNG format
(...)
```

#### Get address information


```java
PTEID_EIDCard card;
PTEID_ulwrapper triesLeft = new PTEID_ulwrapper(-1);
PTEID_Address address;
(...)
PTEID_Pins pins = card.getPins();
PTEID_Pin pin = pins.getPinByPinRef(PTEID_Pin.ADDR_PIN);
if (pin.verifyPin("", triesLeft, true)){
    address = card.getAddr();
    String countryCode = address.getCountryCode();
    String district = address.getDistrict();
    String municipality = address.getMunicipality();
    String street = address.getStreet();
    (...)
}
```

#### How to read or write personal notes


```java
PTEID_EIDCard card = context.getEIDCard();
(...)
// read personal notes
String pdata = card.getID().getPersoData();
// write personal notes
String notes = “a few notes”;
PTEID_ByteArray pb = new PTEID_ByteArray(notes.getBytes(), notes.getBytes().length);
boolean bOk = card.writePersonalNotes(personalNotes, card.getPins().getPinByPinRef(PTEID_Pin.AUTH_PIN));
(...)
```




## Signatures with eID card (citizen signature certificate)
Make sure to initialize the underlying classes that will be used to sign documents, as follows
```java
PTEID_ReaderSet readerSet = PTEID_ReaderSet.instance();
PTEID_ReaderContext readerContext = readerSet.getReader();
pteidlibJava_Wrapper.setCompatReaderContext(readerContext);
PTEID_EIDCard card = readerContext.getEIDCard();
```
#### How to sign PDF documents (PAdES)


```java
//Select the location of visible signature in the document.
//X and Y coordinates of the signature location as percentage of page width and height [0-1]
//To apply an invisible signature negative values should be specified for both parameters, e.g. -1
double location_x = 0.2;
double location_y = 0.2;
int page = 1;

String location = "Lisbon, Portugal"; 
String reason = "I agree with the content of this document";

/* Signature profile according to PAdES specification
PAdES-B:   PTEID_SignatureLevel.PTEID_LEVEL_BASIC (default level)
PAdES-T:   PTEID_SignatureLevel.PTEID_LEVEL_TIMESTAMP
PAdES-LT:  PTEID_SignatureLevel.PTEID_LEVEL_LT
PAdES-LTA: PTEID_SignatureLevel.PTEID_LEVEL_LTV */
signature.setSignatureLevel(PTEID_SignatureLevel.PTEID_LEVEL_BASIC);


//jpeg_data is a byte[] containing a JPEG image file with recommended dimensions 185x41 px
PTEID_ByteArray image_data = new PTEID_ByteArray(jpeg_data, jpeg_data.length);
signature.setCustomImage(image_data);

// Signing a batch of documents with only one PIN prompt
PTEID_PDFSignature signature = new PTEID_PDFSignature();
String[] files = new String[] {'/home/user/document.pdf', '/home/user/another_document.pdf'};
for (String inputFile : files) {
    signature.addToBatchSigning(inputFile);
}
String output = 'home/user/signed_documents/';

// sign only one document
PTEID_PDFSignature signature = new PTEID_PDFSignature('/home/user/document.pdf');
String output = 'home/user/document_signed.pdf';

// Perform the actual signature
int returnCode = card.SignPDF(signature, page, location_x, location_y, location, reason, output);

```

#### How to sign any type of file (XAdES signature format)


```java
(...)
String[] files = new String[] {'/home/user/file1.txt', '/home/user/file2.txt'};

// signing each file individually creating multiple signed files
String output = '/home/user/signed_folder';
card.SignXadesIndividual(output, files, files.length);

// combining all files resulting in a single signed file
String output = 'home/user/signed_file.zip';
card.SignXades(output, files, files.length);

```
The SDK also supports other XAdES signature profiles such as signature with timestamp `SignXadesT()` or archival `SignXadesA()`, etc..
For more details see at [PTEID_EIDCard](./classpt_1_1gov_1_1cartaodecidadao_1_1PTEID__EIDCard.html) 


##### Timestamp Server configuration 
The SDK allows to configure timestamp server instead of default one (which is limited to 20 requests to 20 minutes).
For example, to change the Time Stamping Authority (TSA) by changing the url or the server request timeout, as follows
```java
PTEID_Config config = new PTEID_Config(PTEID_PARAM_XSIGN_TSAURL);
config.setString("http://sha256timestamp.ws.symantec.com/sha256/timestamp"); //use a third party TSA

```

#### Verify a PAdES signature 

One can verify the a PDF signature using the iText SDK.
See [iText-example](https://github.com/itext/i7js-samples/tree/master/publications/signatures/src/test/java/com/itextpdf/samples/signatures/chapter05) for a concrete example of how to verify a signature.


#### Verify a XAdES signature

One can use the library [DSS-Xades](https://github.com/esig/dss/tree/master/dss-xades)  to validate a XAdES signature.
