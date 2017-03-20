/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package pteidlib;
import pt.gov.cartaodecidadao.*;

/**
 *
 * @author ruim
 */
public class pteid {

    private static PTEID_ReaderContext readerContext = null;
    private static PTEID_ReaderSet readerSet = null;
    private static PTEID_EIDCard idCard = null;

    private static int PTEID_ADDRESS_PIN = 131;
    private static int PTEID_AUTH_PIN = 129;
    private static int PTEID_PIN_COUNT  = 3;

    public static final int CARD_TYPE_ERR = 0;
    public static final int CARD_TYPE_IAS07 = 1;
    public static final int CARD_TYPE_IAS101 = 2;

    private static final int MODE_ACTIVATE_BLOCK_PIN = 1;

    protected static final char[] Hexhars = {
        '0', '1', '2', '3', '4', '5',
        '6', '7', '8', '9', 'A', 'B',
        'C', 'D', 'E', 'F'
    };



   private static String ashex(byte[] b){
       String st = "";

       for(int i=0; i<b.length;i++){
          int temp = b[i] & 0x000000FF;
          st +=Hexhars[(temp >> 4)];
          st +=Hexhars[(temp & 0xf)];
       }

       return st;
   }


    private static int trimStart(byte[] array) {
        int trimmedSize = array.length - 1;

        /*while( (trimmedSize>=0) && ( array[trimmedSize] == 0 ) )
            trimmedSize--;*/


        return (trimmedSize+1);
    }


   public static void Init(String readerName) throws PteidException{
        try {
            PTEID_ReaderSet.initSDK();
            readerSet = PTEID_ReaderSet.instance();
            if (readerName == null || readerName.isEmpty())
                readerContext = readerSet.getReaderByNum(0);
            else
                readerContext = readerSet.getReaderByName(readerName);

            idCard = readerContext.getEIDCard();
        } catch (Exception ex) {
            throw new PteidException();
        }
   }


   public static void Exit(int value) throws PteidException{
        try {
            PTEID_ReaderSet.releaseSDK();
        } catch (Exception ex) {
            throw new PteidException();
        }
   }


   public static int GetCardType() throws PteidException{
        try {
            PTEID_CardType cardType = readerContext.getCardType();

            return cardType.swigValue();
        } catch (Exception ex) {
            throw new PteidException();
        }
   }


   public static PTEID_ID GetID() throws PteidException{
       try {
            return new PTEID_ID(idCard.getID());
        } catch (Exception ex) {
            throw new PteidException();
        }
   }


    public static PTEID_ADDR GetAddr() throws PteidException {
        try {
            PTEID_ulwrapper ul = new PTEID_ulwrapper(-1);
            PTEID_Pins pins = idCard.getPins();
            for (long i = 0; i < pins.count(); i++) {
                PTEID_Pin pin = pins.getPinByNumber(i);
                if (pin.getPinRef() == 131) {
                    if (pin.verifyPin("", ul,true)) {
                        return new PTEID_ADDR(idCard.getAddr());
                    }
                }
            }
            throw new PteidException();
        } catch (PteidException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new PteidException();
        }
    }


    public static PTEID_PIC GetPic() throws PteidException{
        PTEID_PIC pic = null;

        try {
            pic = new PTEID_PIC();
            pic.cbeff = new byte[(int)idCard.getID().getPhotoObj().getphotoCbeff().Size()];
            System.arraycopy(idCard.getID().getPhotoObj().getphotoCbeff().GetBytes(), 0, pic.cbeff, 0, pic.cbeff.length);
            pic.facialinfo = new byte[(int)idCard.getID().getPhotoObj().getphotoFacialinfo().Size()];
            System.arraycopy(idCard.getID().getPhotoObj().getphotoFacialinfo().GetBytes(), 0, pic.facialinfo, 0, pic.facialinfo.length);
            pic.facialrechdr = new byte[(int)idCard.getID().getPhotoObj().getphotoFacialrechdr().Size()];
            System.arraycopy(idCard.getID().getPhotoObj().getphotoFacialrechdr().GetBytes(), 0, pic.facialrechdr, 0, pic.facialrechdr.length);
            pic.imageinfo = new byte[(int)idCard.getID().getPhotoObj().getphotoImageinfo().Size()];
            System.arraycopy(idCard.getID().getPhotoObj().getphotoImageinfo().GetBytes(), 0, pic.imageinfo, 0, pic.imageinfo.length);
            pic.picture = new byte[(int)idCard.getID().getPhotoObj().getphotoRAW().Size()];
            System.arraycopy(idCard.getID().getPhotoObj().getphotoRAW().GetBytes(), 0, pic.picture, 0, pic.picture.length);
            pic.version = 0;
        } catch (Exception ex) {
            throw new PteidException();
        }

        return pic;
    }


    public static PTEID_Certif[] GetCertificates() throws PteidException {
        PTEID_Certif[] certs = null;
        PTEID_ByteArray ba = new PTEID_ByteArray();

        try {
            PTEID_Certificates certificates = idCard.getCertificates();
            certs = new PTEID_Certif[(int)certificates.countAll()];
            for(int i=0;i<certs.length;i++){
                certs[i] = new PTEID_Certif();
                certificates.getCert(i).getFormattedData(ba);
                certs[i].certif = new byte[(int)(ba.Size())];
                System.arraycopy(ba.GetBytes(), 0, certs[i].certif, 0, (int)ba.Size());
                certs[i].certifLabel = certificates.getCert(i).getLabel();
            }
        } catch (Exception ex) {
            throw new PteidException();
        }

        return certs;
    }



    public static int VerifyPIN(byte b, String string) throws PteidException {
        PTEID_ulwrapper ul = new PTEID_ulwrapper(-1);
        int triesLeft =  0;

        // martinho: artista
        long pinId = b & 0x00000000000000FF;

        if (readerContext != null) {
            if (pinId != 1 && pinId != 129 && pinId != 130 && pinId != 131) {
                return 0;
            }
            try {
                PTEID_Pins pins = idCard.getPins();
                for (long pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
                    PTEID_Pin pin = pins.getPinByNumber(pinIdx);
                    if (pin.getPinRef() == pinId) {
                        pin.verifyPin("", ul,true);
                        //martinho: verify pin is not working properly for readers without pinpad at this moment,
                        //this is a workaround
                        triesLeft = pin.getTriesLeft();
                    }
                }
                return triesLeft;
            } catch (Exception ex) {
                throw new PteidException();
            }
        }
        return triesLeft;
    }


    public static int ChangePIN(byte b, String oldPin, String newPin) throws PteidException {
        PTEID_ulwrapper ul = new PTEID_ulwrapper(-1);
        int triesLeft = 0;

        // martinho: artista
        long pinId = b & 0x00000000000000FF;

        if (readerContext != null) {
            if (pinId != 1 && pinId != 129 && pinId != 130 && pinId != 131) {
                return 0;
            }
            try {
                PTEID_Pins pins = idCard.getPins();
                for (long pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
                    PTEID_Pin pin = pins.getPinByNumber(pinIdx);
                    if (pin.getPinRef() == pinId)
                        if (pin.changePin(oldPin, newPin, ul, pin.getLabel(),true)) {
                            triesLeft = pin.getTriesLeft();
                            break;
                        }
                }
            } catch (Exception ex) {
                throw new PteidException();
            }
        }
        return triesLeft;
    }


    public static PTEIDPin[] GetPINs() throws PteidException {
        PTEIDPin[] pinArray = null;
        int currentId;

        if (readerContext != null) {
            try {
                PTEID_Pins pins = idCard.getPins();
                pinArray = new PTEIDPin[PTEID_PIN_COUNT];
                for (int pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
                    PTEID_Pin pin = pins.getPinByNumber(pinIdx);
                    if (pin.getId() == 1 || pin.getId() == 2 || pin.getId() == 3) {
                        currentId = (int)pin.getId()-1;
                        pinArray[currentId] = new PTEIDPin();
                        pinArray[currentId].flags = (int) pin.getFlags();
                        pinArray[currentId].usageCode = (int) pin.getId(); // martinho: might not be the intended use, but gives the expected compatible result.
                        pinArray[currentId].pinType = (int) pin.getType();
                        pinArray[currentId].label = pin.getLabel();
                        pinArray[currentId].triesLeft = pin.getTriesLeft();
                        pinArray[currentId].id = (byte) pin.getPinRef();
                        pinArray[currentId].shortUsage = null;
                        pinArray[currentId].longUsage = null;
                    }
                }
            } catch (Exception ex) {
                throw new PteidException();
            }
        }

        return pinArray;
    }


    public static PTEID_TokenInfo GetTokenInfo() throws PteidException {
        PTEID_TokenInfo token = null;

        if (readerContext != null) {
            try {
                PTEID_CardVersionInfo info = idCard.getVersionInfo();
                token = new PTEID_TokenInfo(info.getTokenLabel(), info.getSerialNumber());
            } catch (Exception ex) {
                ex.printStackTrace();
                throw new PteidException();
            }
        }

        return token;
    }


    public static byte[] ReadSOD() throws PteidException {
        byte[] sod = null;
        byte[] trimmable;

        if (readerContext != null) {
            try {
                PTEID_ByteArray pba = idCard.getSod().getData();

                int trimmedSize = trimStart(pba.GetBytes());

                sod = new byte[trimmedSize];
                System.arraycopy(pba.GetBytes(), 0, sod, 0, sod.length);
            } catch (Exception ex) {
                ex.printStackTrace();
                throw new PteidException();
            }
        }
        return sod;
    }


    public static int UnblockPIN(byte b, String puk, String newPin) throws PteidException{
        PTEID_ulwrapper ul = new PTEID_ulwrapper(-1);

		// martinho: artista
        long pinId = b & 0x00000000000000FF;

        if (readerContext!=null){
            try {

		if (pinId != 1 && pinId != 129 && pinId != 130 && pinId != 131)
			return 0;

		PTEID_Pins pins = idCard.getPins();
		for (long pinIdx=0; pinIdx < pins.count(); pinIdx++){
			PTEID_Pin pin = pins.getPinByNumber(pinIdx);
			if (pin.getPinRef() == pinId){
				pin.unlockPin(puk, newPin, ul);
			}
		}
            } catch (Exception ex) {
                throw new PteidException();
            }
	}

	return (int)ul.m_long;
    }


    public static int UnblockPIN_Ext(byte b, String string, String string1, int i) throws PteidException{
    	return UnblockPIN(b,string, string1);
    }


    public static void SelectADF(byte[] bytes) throws PteidException {
        if (readerContext != null) {
            try {
                byte[] ap = {0x00, (byte) 0xA4, 0x00, 0x0C};
                PTEID_ByteArray apdu = new PTEID_ByteArray(ap, ap.length);
                byte[] temp = new byte[1];
                temp[0] = (byte) bytes.length;
                apdu.Append(temp, 1);
                apdu.Append(bytes, bytes.length);
                PTEID_ByteArray sendAPDU = idCard.sendAPDU(apdu);
                //verificar se correu tudo bem... ?
            } catch (Exception ex) {
                throw new PteidException();
            }
        }
    }


    public static byte[] ReadFile(byte[] bytes, byte b) throws PteidException {
        PTEID_ByteArray pb = new PTEID_ByteArray();
        byte[] retArray = null;
        PTEID_Pin pin = null;

        // martinho: artista
        long pinId = b & 0x00000000000000FF;

        if (readerContext != null) {
            try {

                if (pinId != 0) {
                    PTEID_Pins pins = idCard.getPins();
                    for (long pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
                        pin = (pins.getPinByNumber(pinIdx));
                        if (pin.getPinRef() == pinId )
                            break;
                    }
                }

                idCard.readFile(ashex(bytes), pb, pin);

                int trimmedSize = trimStart(pb.GetBytes());
                if ( ( trimmedSize == 0 ) && ( pb.Size() > 0 ) ) trimmedSize = (int)pb.Size();

                retArray = new byte[trimmedSize];

                System.arraycopy(pb.GetBytes(), 0, retArray, 0, retArray.length);
            } catch (Exception ex) {
                System.err.println("Erro no ReadFile: " + ex.getMessage() );
                throw new PteidException();
            }
        }
        return retArray;
    }


    public static void WriteFile(byte[] file, byte[] data, byte bpin) throws PteidException{
        WriteFileInOffset( file, data, bpin, 0/*inOffset*/);
    }

    public static void WriteFileInOffset(byte[] file, byte[] data, byte bpin, int inOffset) throws PteidException{
        PTEID_ByteArray pb = new PTEID_ByteArray(data,data.length);
        PTEID_Pin pin = null;

        // martinho: artista
        long pinId = bpin & 0x00000000000000FF;

        if (readerContext != null) {
            try {

                if (pinId != 0) {
                    PTEID_Pins pins = idCard.getPins();
                    for (long pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
                        pin = (pins.getPinByNumber(pinIdx));
                        if (pin.getPinRef() == pinId)
                            break;
                    }
                }

                idCard.writeFile(ashex(file),pb,pin, "", inOffset);

            } catch (Exception ex) {
                throw new PteidException();
            }
        }
    }

    public static int IsActivated() throws PteidException {
        if (readerContext != null) {
            try {
                return idCard.isActive() ? 1 : 0;
            } catch (Exception ex) {
                throw new PteidException();
            }
        }
        return 0;
    }


    public static void Activate(String actPin, byte[] bytes, int activateMode) throws PteidException {
        PTEID_ByteArray pb = new PTEID_ByteArray(bytes, bytes.length);
        if (readerContext != null) {
            try {
		boolean mode = activateMode == MODE_ACTIVATE_BLOCK_PIN;
                idCard.Activate(actPin, pb, mode);
            } catch (Exception ex) {
                throw new PteidException();
            }
        }
    }


    public static void SetSODChecking(boolean bln) throws PteidException {
        if (readerContext != null) {
            try {
                readerContext.getEIDCard().doSODCheck(bln);
            } catch (Exception ex) {
                throw new PteidException();
            }
        }
    }


    public static void SetSODCAs(PTEID_Certif[] pteidcs) throws PteidException {
        if (readerContext != null) {
            try {
                if (null == pteidcs){
                    readerContext.getEIDCard().getCertificates().resetSODCAs();
                    return;
                }/* if (null == pteidcs) */

                for(PTEID_Certif pcert : pteidcs)
                {
					PTEID_ByteArray pba = new PTEID_ByteArray(pcert.certif, pcert.certif.length);
                    readerContext.getEIDCard().getCertificates().addToSODCAs(pba);
                }
            } catch (Exception ex) {
                ex.printStackTrace();
                throw new PteidException();
            }
        }
    }


    public static PTEID_RSAPublicKey GetCardAuthenticationKey() throws PteidException {
        PTEID_RSAPublicKey key = null;

        if (readerContext != null) {
            try {
                PTEID_PublicKey cardKey = idCard.getID().getCardAuthKeyObj();
                key = new PTEID_RSAPublicKey();
                key.exponent = new byte[(int) cardKey.getCardAuthKeyExponent().Size()];
                key.modulus = new byte[(int) cardKey.getCardAuthKeyModulus().Size()];
                System.arraycopy(cardKey.getCardAuthKeyExponent().GetBytes(), 0, key.exponent, 0, key.exponent.length);
                System.arraycopy(cardKey.getCardAuthKeyModulus().GetBytes(), 0, key.modulus, 0, key.modulus.length);
            } catch (Exception ex) {
                throw new PteidException();
            }
        }

        return key;
    }


    public static PTEID_RSAPublicKey GetCVCRoot() throws PteidException {
        PTEID_RSAPublicKey key = null;

        if (readerContext != null) {
            try {
                PTEID_PublicKey rootCAKey = idCard.getRootCAPubKey();
                key = new PTEID_RSAPublicKey();
                key.exponent = new byte[(int) rootCAKey.getCardAuthKeyExponent().Size()];
                key.modulus = new byte[(int) rootCAKey.getCardAuthKeyModulus().Size()];
                System.arraycopy(rootCAKey.getCardAuthKeyExponent().GetBytes(), 0, key.exponent, 0, key.exponent.length);
                System.arraycopy(rootCAKey.getCardAuthKeyModulus().GetBytes(), 0, key.modulus, 0, key.modulus.length);
            }
            catch (PTEID_Exception ex) {
                System.err.println("Error in GetCVCRoot(): "+ex.GetError());
                throw new PteidException();
            }
        }

        return key;
    }


    public static byte[] SendAPDU(byte[] bytes) throws PteidException {
        byte[] ret = null;
        if (readerContext != null) {
            try {
                PTEID_ByteArray sApdu = new PTEID_ByteArray(bytes, bytes.length);
                PTEID_ByteArray resp;

                resp = idCard.sendAPDU(sApdu);

                ret = new byte[(int) resp.Size()];
                System.arraycopy(resp.GetBytes(), 0, ret, 0, ret.length);
            } catch (Exception ex) {
                throw new PteidException();
            }
        }

        return ret;
    }

    public static byte[] CVC_Init(byte[] cert) throws PteidException
    {
        byte[] ret = null;

        PTEID_ByteArray cvc = new PTEID_ByteArray(cert, cert.length);
        //Make the Exception retro-compatible
        try
        {
            PTEID_ByteArray ba = pteidlibJava_Wrapper.PTEID_CVC_Init(cvc);
            ret = new byte[(int) ba.Size()];
        }
        catch (Exception ex) {
                System.err.println("Error in CVC_Init: " + ex.getMessage());
                throw new PteidException();
        }

        return ret;
    }

    public static void CVC_Authenticate(byte[] cert) throws PteidException
    {
        PTEID_ByteArray cvc = new PTEID_ByteArray(cert, cert.length);
        //Make the Exception retro-compatible
        try
        {
            pteidlibJava_Wrapper.PTEID_CVC_Authenticate(cvc);
        } catch (Exception ex) {
                System.err.println("Error in CVC_Authenticate: " + ex.getMessage());
                throw new PteidException();
        }
    }

    public static PTEID_ADDR CVC_GetAddr() throws PteidException {
        //PTEID_ADDR addr = null;
        byte[] fileID={ (byte)0x3F, (byte)0x00, (byte)0x5F, (byte)0x00, (byte)0xEF, (byte)0x05 };

        try {
            byte[] addressRaw = CVC_ReadFile( fileID );

            return new PTEID_ADDR( addressRaw );

        } catch (PteidException ex) {
            System.err.println("Error in CVC_GetAddr: " + ex.getMessage());
            throw ex;
        } catch (Exception ex) {
            System.err.println("Error in CVC_GetAddr: " + ex.getMessage());
            throw new PteidException();
        }
    }

    public static byte[] CVC_ReadFile(byte[] fileID) throws PteidException
    {
        byte[] ret = null;

        PTEID_ByteArray ba_fileID = new PTEID_ByteArray(fileID, fileID.length);
        //Make the Exception retro-compatible
        try
        {
            PTEID_ByteArray ba = pteidlibJava_Wrapper.PTEID_CVC_ReadFile(ba_fileID);
            ret = new byte[(int) ba.Size()];
        }
        catch (Exception ex) {
            System.err.println("Error in CVC_ReadFile: "+ex.getMessage());
            throw new PteidException();
        }
        return ret;
    }


    public static long CAP_ChangeCapPin(String paramString1, byte[] paramArrayOfByte, PTEID_Proxy_Info paramPTEID_Proxy_Info, String paramString2, String paramString3){
        return 0;
    }

    public static int CAP_GetCapPinChangeProgress(){
        return 0;
    }

    public static void CAP_CancelCapPinChange(){
        return;
    }


   private static String findReaderNameWithCard() throws Exception{
	long nrReaders  = readerSet.readerCount();

	for ( int readerIdx=0; readerIdx<nrReaders; readerIdx++)
            if (readerContext.isCardPresent())
                return readerSet.getReaderName(readerIdx);

        return null;
   }
}
