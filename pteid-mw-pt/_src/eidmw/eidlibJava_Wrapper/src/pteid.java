/*
 * Pteidlib compatibility methods
 */

package pteidlib;
import pt.gov.cartaodecidadao.*;
import pt.gov.cartaodecidadao.pteidlibJava_WrapperConstants;

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

    public static final int PTEID_EXIT_LEAVE_CARD = 0;
    public static final int PTEID_EXIT_UNPOWER = 2;

    public static final int UNBLOCK_FLAG_NEW_PIN = 1;
    public static final int UNBLOCK_FLAG_PUK_MERGE = 2;
    public static final int MODE_ACTIVATE_BLOCK_PIN = 1;

    //Error codes inherited from Pteid Middleware V1: documented in CC_Technical_Reference_1.61
    private static int SC_ERROR_AUTH_METHOD_BLOCKED = -1212;
    private static int SC_ERROR_INVALID_CARD = -1210;
    private static int SC_ERROR_PIN_CODE_INCORRECT = -1214;
    private static int SC_ERROR_KEYPAD_TIMEOUT = -1108;
    private static int SC_ERROR_KEYPAD_CANCELLED = -1109;
    private static int SC_ERROR_NO_READERS_FOUND = -1101;
    private static int SC_ERROR_CARD_NOT_PRESENT = -1104;

        private static String GEMPC_PINPAD = "GemPC Pinpad";

    protected static final char[] hexChars = {
        '0', '1', '2', '3', '4', '5',
        '6', '7', '8', '9', 'A', 'B',
        'C', 'D', 'E', 'F'
    };


    private static String ashex(byte[] b) {
       String st = "";

       for(int i=0; i<b.length;i++) {
          int temp = b[i] & 0x000000FF;
          st += hexChars[(temp >> 4)];
          st += hexChars[(temp & 0xf)];
       }

       return st;
    }


    private static int trimStart(byte[] array) {
        int trimmedSize = array.length - 1;

        /*while( (trimmedSize>=0) && ( array[trimmedSize] == 0 ) )
            trimmedSize--;*/


        return (trimmedSize+1);
    }


    public static void Init(String readerName) throws PteidException {
        try {
            PTEID_ReaderSet.initSDK();
            readerSet = PTEID_ReaderSet.instance();
            if (readerName == null || readerName.isEmpty())
                readerContext = readerSet.getReader();
            else
                readerContext = readerSet.getReaderByName(readerName);

                        pteidlibJava_Wrapper.setCompatReaderContext(readerContext);
            idCard = readerContext.getEIDCard();
        }
        catch(PTEID_ExNoReader ex) {
            throw new PteidException(SC_ERROR_NO_READERS_FOUND);
        }
        catch (PTEID_ExNoCardPresent ex) {
            throw new PteidException(SC_ERROR_CARD_NOT_PRESENT);
        }
        catch (PTEID_ExCardTypeUnknown ex) {
            throw new PteidException(SC_ERROR_INVALID_CARD);
        }
        catch (Exception ex) {
            //ex.printStackTrace();
            throw new PteidException();
        }
   }

   /* TODO: mode parameter is unused. It should reach the call to SCardDisconnect() in cardlayer/PCSC.cpp */
   public static void Exit(int mode) throws PteidException{
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

    private static void handleUnderlyingException(PTEID_Exception ex) throws PteidException {
        if (ex.GetError() == pteidlibJava_WrapperConstants.EIDMW_ERR_TIMEOUT) {
            throw new PteidException(SC_ERROR_KEYPAD_TIMEOUT);
        } else if (ex.GetError() == pteidlibJava_WrapperConstants.EIDMW_ERR_PIN_CANCEL) {
            throw new PteidException(SC_ERROR_KEYPAD_CANCELLED);
        } else {
            throw new PteidException(ex.GetError());
        }  
    }

    public static PTEID_ADDR GetAddr() throws PteidException {

        try {
                        //Workaround for GemPC issue with 3072-bits cards
                        if (readerContext.getName().contains(GEMPC_PINPAD)) {
                                //Just need to read SOD file, we don't actually need the data
                                PTEID_ByteArray throwaway_sod = new PTEID_ByteArray();
                                idCard.readFile("3F005F00EF06", throwaway_sod);
                        }
            PTEID_ulwrapper ul = new PTEID_ulwrapper(-1);
            PTEID_Pins pins = idCard.getPins();
            for (long i = 0; i < pins.count(); i++) {
                pt.gov.cartaodecidadao.PTEID_Pin pin = pins.getPinByNumber(i);
                if (pin.getPinRef() == PTEID_ADDRESS_PIN) {
                    if (pin.verifyPin("", ul, false)) {
                        return new PTEID_ADDR(idCard.getAddr());
                    }
                    else
                    {
                        if (ul.m_long == 0)
                            throw new PteidException(SC_ERROR_AUTH_METHOD_BLOCKED);
                        else
                            throw new PteidException(SC_ERROR_PIN_CODE_INCORRECT);
                    }
                }
            }
            throw new PteidException();
        } catch (PTEID_Exception ex) {
            handleUnderlyingException(ex);
        }
        throw new PteidException();
    }

    public static PTEID_PIC GetPic() throws PteidException {
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



    public static int VerifyPIN(byte pin_id, String string) throws PteidException {
        PTEID_ulwrapper tries_left = new PTEID_ulwrapper(-1);

        // Convert byte to long
        long pinId = pin_id & 0x00000000000000FF;

        if (readerContext != null) {
            if (pinId != 1 && pinId != 129 && pinId != 130 && pinId != 131) {
                return 0;
            }
            try {
                PTEID_Pins pins = idCard.getPins();
                for (long pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
                    pt.gov.cartaodecidadao.PTEID_Pin pin = pins.getPinByNumber(pinIdx);
                    if (pin.getPinRef() == pinId) {
                        boolean rc = pin.verifyPin("", tries_left, false);
                        if (!rc)
                        {
                            if (tries_left.m_long == 0)
                                throw new PteidException(SC_ERROR_AUTH_METHOD_BLOCKED);
                            else
                                throw new PteidException(SC_ERROR_PIN_CODE_INCORRECT);
                        }
                    }
                }
                return (int)(tries_left.m_long);
            }
            catch (PTEID_Exception ex) {
               handleUnderlyingException(ex);
            }
        }
        throw new PteidException();
    }


    public static int ChangePIN(byte pin_id, String oldPin, String newPin) throws PteidException {
        PTEID_ulwrapper tries_left = new PTEID_ulwrapper(-1);

        // Convert byte to long
        long pinId = pin_id & 0x00000000000000FF;

        if (readerContext != null) {
            if (pinId != 1 && pinId != 129 && pinId != 130 && pinId != 131) {
                return 0;
            }
            try {
                PTEID_Pins pins = idCard.getPins();
                for (long pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
                    pt.gov.cartaodecidadao.PTEID_Pin pin = pins.getPinByNumber(pinIdx);
                    if (pin.getPinRef() == pinId) {

                       boolean rc = pin.changePin(oldPin, newPin, tries_left, pin.getLabel(), false);
                       if (!rc)
                       {
                         if (tries_left.m_long == 0)
                            throw new PteidException(SC_ERROR_AUTH_METHOD_BLOCKED);
                         else
                            throw new PteidException(SC_ERROR_PIN_CODE_INCORRECT);
                       }
                   }
                }
            }
            catch (PTEID_Exception ex) {
                handleUnderlyingException(ex);
            }
            return (int)(tries_left.m_long);
        }

        return -1;
    }

    public static PTEID_Pin[] GetPINs() throws PteidException {
        PTEID_Pin[] pinArray = null;
        int currentId;

        if (readerContext != null) {
            try {
                PTEID_Pins pins = idCard.getPins();
                pinArray = new PTEID_Pin[PTEID_PIN_COUNT];
                for (int pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
                    pt.gov.cartaodecidadao.PTEID_Pin pin = pins.getPinByNumber(pinIdx);
                    if (pin.getId() == 1 || pin.getId() == 2 || pin.getId() == 3) {
                        currentId = (int)pin.getId()-1;
                        pinArray[currentId] = new PTEID_Pin();
                        pinArray[currentId].flags = (int) pin.getFlags();
                        // rmartinho: might not be the intended use, but gives the expected compatible result.
                        pinArray[currentId].usageCode = (int) pin.getId();
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


    public static int UnblockPIN(byte pin_id, String puk, String newPin) throws PteidException {

       return UnblockPIN_Ext(pin_id, puk, newPin, UNBLOCK_FLAG_NEW_PIN);
    }

    public static int UnblockPIN_Ext(byte pin_id, String puk, String newPin, int flags) throws PteidException {
        PTEID_ulwrapper tries_left = new PTEID_ulwrapper(-1);

        //Convert byte to long
        long pinId = pin_id & 0x00000000000000FF;

        if (readerContext!=null) {
            try {
                                if (pinId != 1 && pinId != 129 && pinId != 130 && pinId != 131)
                    return 0;

                                //Workaround for GemPC issue with 3072-bits cards
                                if (readerContext.getName().contains(GEMPC_PINPAD)) {
                                        //Just need to read SOD file, we don't actually need the data
                                        PTEID_ByteArray throwaway_sod = new PTEID_ByteArray();
                                        idCard.readFile("3F005F00EF06", throwaway_sod);
                                }

                PTEID_Pins pins = idCard.getPins();
                for (long pinIdx=0; pinIdx < pins.count(); pinIdx++) {

                    pt.gov.cartaodecidadao.PTEID_Pin pin = pins.getPinByNumber(pinIdx);

                    if (pin.getPinRef() == pinId) {
                        boolean ret = pin.unlockPin(puk, newPin, tries_left, flags);

                        if (!ret)
                        {
                            if (tries_left.m_long == 0)
                                throw new PteidException(SC_ERROR_AUTH_METHOD_BLOCKED);
                            else
                                throw new PteidException(SC_ERROR_PIN_CODE_INCORRECT);
                        }
                    }
                }
            }
            catch (PTEID_Exception ex) {
                handleUnderlyingException(ex);
            }
        }

        return (int)tries_left.m_long;
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


    public static byte[] ReadFile(byte[] bytes, byte pin_id) throws PteidException {
        PTEID_ByteArray pb = new PTEID_ByteArray();
        byte[] retArray = null;
        pt.gov.cartaodecidadao.PTEID_Pin pin = null;

        // Convert byte to long
        long pinId = pin_id & 0x00000000000000FF;

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
            }
            catch (PTEID_Exception ex) {
                handleUnderlyingException(ex);
            }
            catch (Exception ex) {
                ex.printStackTrace();
                throw new PteidException();
            }
        }
        return retArray;
    }

    public static void WriteFile(byte[] file, byte[] data, byte bpin) throws PteidException {
        WriteFileInOffset( file, data, bpin, 0/*inOffset*/);
    }

    public static void WriteFileInOffset(byte[] file, byte[] data, byte bpin, int inOffset) throws PteidException {
        PTEID_ByteArray pb = new PTEID_ByteArray(data,data.length);
        pt.gov.cartaodecidadao.PTEID_Pin pin = null;

        // Convert byte to long
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
            } catch (PTEID_Exception ex) {
                handleUnderlyingException(ex);
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
            }
            catch (PTEID_Exception ex) {
                if (ex.GetError() == pteidlibJava_WrapperConstants.EIDMW_ERR_PIN_CANCEL)
                    throw new PteidException(SC_ERROR_PIN_CODE_INCORRECT);
                else
                    throw new PteidException(ex.GetError());
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
            }
            catch (PTEID_Exception ex) {
                throw new PteidException(ex.GetError());
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

                throw new PteidException(ex.GetError());
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
                        System.arraycopy(ba.GetBytes(), 0, ret, 0,  ret.length);
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
                        System.arraycopy(ba.GetBytes(), 0, ret, 0, ret.length);
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
