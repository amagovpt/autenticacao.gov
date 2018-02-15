/*
 * Pteidlib compatibility methods
 */

using System;
using System.Text;
using pt.portugal.eid;

namespace eidpt
{
	
	public enum PteidCardType
	{
		TYPE_ERR,
		TYPE_IAS07,
		TYPE_IAS101
	}
	
    public class Pteid {
    
    private static PTEID_ReaderContext readerContext = null;
    private static PTEID_ReaderSet readerSet = null;
    private static PTEID_EIDCard idCard = null;

    private static readonly int PTEID_AUTH_PIN = 0x81;
    private static readonly int PTEID_ADDRESS_PIN = 0x83;
    private static readonly int PTEID_SIGNATURE_PIN = 0x82;
    
    private static readonly int PTEID_PIN_COUNT  = 3;
    
    public static readonly int CARD_TYPE_ERR = 0;
    public static readonly int CARD_TYPE_IAS07 = 1;
    public static readonly int CARD_TYPE_IAS101 = 2;
	
	public static readonly uint UNBLOCK_FLAG_NEW_PIN = 1;
    public static readonly uint UNBLOCK_FLAG_PUK_MERGE = 2;

    private static readonly int SC_ERROR_AUTH_METHOD_BLOCKED = -1212;
    private static readonly int SC_ERROR_PIN_CODE_INCORRECT = -1214;
    private static readonly int SC_ERROR_KEYPAD_TIMEOUT = -1108;
    private static readonly int SC_ERROR_KEYPAD_CANCELLED = -1109;
    private static readonly int SC_ERROR_NO_READERS_FOUND = -1101;
    private static readonly int SC_ERROR_CARD_NOT_PRESENT = -1104;
    private static readonly int SC_ERROR_INVALID_CARD = -1210;

    //Flag used in the Activate method
    private static readonly uint MODE_ACTIVATE_BLOCK_PIN = 1;
    
    protected static readonly char[] hexChars = {
        '0', '1', '2', '3', '4', '5',
        '6', '7', '8', '9', 'A', 'B',
        'C', 'D', 'E', 'F'
    };
    
   private static String ashex(byte[] b){
       String st = "";
       
       for(int i=0; i<b.Length;i++){
          int temp = b[i] & 0x000000FF;
          st += hexChars[(temp >> 4)];
          st += hexChars[(temp & 0xf)];
       }
       
       return st;
   }
   
   
    private static int trimStart(byte[] array) {
        int trimmedSize = array.Length - 1;
        
        /*while( (trimmedSize>=0) && ( array[trimmedSize] == 0 ) )
            trimmedSize--;*/
        return (trimmedSize + 1);
    }

   
   public static void Init(String readerName) {
       try
       {
           PTEID_ReaderSet.initSDK();
           readerSet = PTEID_ReaderSet.instance();
           if (readerName == null || readerName == String.Empty)
               readerContext = readerSet.getReader();
           else
               readerContext = readerSet.getReaderByName(readerName);

           pteidlib_dotNet.setCompatReaderContext(readerContext);

           idCard = readerContext.getEIDCard();
       }
       catch (PTEID_ExNoReader)
       {
           throw new PteidException(SC_ERROR_NO_READERS_FOUND);
       }
       catch (PTEID_ExNoCardPresent)
       {
           throw new PteidException(SC_ERROR_CARD_NOT_PRESENT);
       }
       catch (PTEID_ExCardTypeUnknown)
       {
           throw new PteidException(SC_ERROR_INVALID_CARD);
       }
       catch (PTEID_Exception ex)
       {
           throw new PteidException(ex.GetError());
       }
       
   }
  
   public static void Exit(uint value) {
        try {
            PTEID_ReaderSet.releaseSDK();
        }
        catch (PTEID_Exception ex)
        {
            throw new PteidException(ex.GetError());
        }
   }
   
   
   public static PteidCardType GetCardType() {
        try {
            PTEID_CardType cardType = readerContext.getCardType();
			
			if (cardType == PTEID_CardType.PTEID_CARDTYPE_IAS07)
				return PteidCardType.TYPE_IAS07;
			else if (cardType == PTEID_CardType.PTEID_CARDTYPE_IAS101)
				return PteidCardType.TYPE_IAS101;		
			else
				return PteidCardType.TYPE_ERR;
          
        }
        catch (PTEID_Exception ex)
        {
            throw new PteidException(ex.GetError());
        }
   }
   
   
   public static PteidId GetID() { 
       try {
           return new PteidId(idCard.getID());
        } catch (PTEID_Exception ex) {
            throw new PteidException(ex.GetError());
        }
   }
   
   
    public static PteidAddr GetAddr() {
        try
        {
            uint ul = 0;
            PTEID_Pins pins = idCard.getPins();
            for (uint i = 0; i < pins.count(); i++)
            {
                PTEID_Pin pin = pins.getPinByNumber(i);
                if (pin.getPinRef() == PTEID_ADDRESS_PIN)
                {
                    if (pin.verifyPin("", ref ul, false))
                    {
                        return new PteidAddr(idCard.getAddr());
                    }
                    else
                    {
                        if (ul == 0)
                            throw new PteidException(SC_ERROR_AUTH_METHOD_BLOCKED);
                        else
                            throw new PteidException(SC_ERROR_PIN_CODE_INCORRECT);
                    }
                }
            }
        }
        catch (PTEID_Exception ex)
        {
            handleUnderlyingException(ex);
        }

        //Fix build: should never happen
        return null;
    }
    
    
    public static PteidPic GetPic(){
        PteidPic pic = null;
        
        try {
            pic = new PteidPic();
            pic.cbeff = new byte[(int)idCard.getID().getPhotoObj().getphotoCbeff().Size()];
            Array.Copy(idCard.getID().getPhotoObj().getphotoCbeff().GetBytes(), 0, pic.cbeff, 0, pic.cbeff.Length);
            pic.facialinfo = new byte[(int)idCard.getID().getPhotoObj().getphotoFacialinfo().Size()];
            Array.Copy(idCard.getID().getPhotoObj().getphotoFacialinfo().GetBytes(), 0, pic.facialinfo, 0, pic.facialinfo.Length);
            pic.facialrechdr = new byte[(int)idCard.getID().getPhotoObj().getphotoFacialrechdr().Size()];
            Array.Copy(idCard.getID().getPhotoObj().getphotoFacialrechdr().GetBytes(), 0, pic.facialrechdr, 0, pic.facialrechdr.Length);
            pic.imageinfo = new byte[(int)idCard.getID().getPhotoObj().getphotoImageinfo().Size()];
            Array.Copy(idCard.getID().getPhotoObj().getphotoImageinfo().GetBytes(), 0, pic.imageinfo, 0, pic.imageinfo.Length);
            pic.picture = new byte[(int)idCard.getID().getPhotoObj().getphotoRAW().Size()];
            Array.Copy(idCard.getID().getPhotoObj().getphotoRAW().GetBytes(), 0, pic.picture, 0, pic.picture.Length);
            pic.version = 0;
        } catch (PTEID_Exception ex) {
            throw new PteidException(ex.GetError());
        }
        
        return pic;
    }
    

    public static PteidCertif[] GetCertificates() {
        PteidCertif[] certs = null;
        PTEID_ByteArray ba = new PTEID_ByteArray();
        
        try {
            PTEID_Certificates certificates = idCard.getCertificates();
            certs = new PteidCertif[(int)certificates.countAll()];
            for(uint i=0; i<certs.Length; i++){
                certs[i] = new PteidCertif();
                certificates.getCert(i).getFormattedData(ba);
                certs[i].certif = new byte[(int)(ba.Size())];
                Array.Copy(ba.GetBytes(), 0, certs[i].certif, 0, (int)ba.Size());
                certs[i].certifLabel = certificates.getCert(i).getLabel();
            }
        }
        catch (PTEID_Exception ex)
        {
            throw new PteidException(ex.GetError());
        }

        return certs;
    }

    private static void handleUnderlyingException(PTEID_Exception ex)
    {
        if (ex.GetError() == pteidlib_dotNet.EIDMW_ERR_TIMEOUT)
            throw new PteidException(SC_ERROR_KEYPAD_TIMEOUT);
        else if (ex.GetError() == pteidlib_dotNet.EIDMW_ERR_PIN_CANCEL)
            throw new PteidException(SC_ERROR_KEYPAD_CANCELLED);
        else
            throw new PteidException(ex.GetError());
    }
    
    public static int VerifyPIN(byte pinId, String strPin) {
        uint tries_left = 0;
        int triesLeft =  0;
            
        if (readerContext != null) {
            //PIN ID = 1 is legacy from IAS 1.01 cards
            if (pinId != 1 && pinId != PTEID_AUTH_PIN && pinId != PTEID_SIGNATURE_PIN && pinId != PTEID_ADDRESS_PIN) {
                return 0;
            }
            try {
                PTEID_Pins pins = idCard.getPins();
                for (uint pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
                    PTEID_Pin pin = pins.getPinByNumber(pinIdx);
                    if (pin.getPinRef() == pinId) {

                        bool ret = pin.verifyPin(strPin, ref tries_left, false);
                        if (!ret)
                        {
                            if (tries_left == 0)
                                throw new PteidException(SC_ERROR_AUTH_METHOD_BLOCKED);
                            else
                                throw new PteidException(SC_ERROR_PIN_CODE_INCORRECT);
                        }
                        triesLeft = (int) tries_left;
                    }
                }
                return triesLeft;
            } catch (PTEID_Exception ex) {
                handleUnderlyingException(ex);
            }
        }
        return triesLeft;
    }
    
    
    public static int ChangePIN(byte pinId, String oldPin, String newPin){
        uint ul = 0;
        int triesLeft = 0;

        if (readerContext != null) {
            if (pinId != 1 && pinId != PTEID_AUTH_PIN && pinId != PTEID_SIGNATURE_PIN && pinId != PTEID_ADDRESS_PIN)
            {
                return 0;
            }
            try {
                PTEID_Pins pins = idCard.getPins();
                for (uint pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
                    PTEID_Pin pin = pins.getPinByNumber(pinIdx);
                    if (pin.getPinRef() == pinId)
                    {
                        if (!pin.changePin(oldPin, newPin, ref ul, pin.getLabel(), false))
                        {
                            if (ul == 0)
                                throw new PteidException(SC_ERROR_AUTH_METHOD_BLOCKED);
                            else
                                throw new PteidException(SC_ERROR_PIN_CODE_INCORRECT);
                        }
                        triesLeft = (int)ul;
                        break;
                    }
                        
                }
            }
            catch (PTEID_Exception ex)  {
                handleUnderlyingException(ex);
            }
        }
        return triesLeft;
    }
   
    
    public static PteidPin[] GetPINs() {
        PteidPin[] pinArray = null;
        int currentId;
        
        if (readerContext != null) {
            try {
                PTEID_Pins pins = idCard.getPins();
                pinArray = new PteidPin[PTEID_PIN_COUNT];
                for (uint pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
                    PTEID_Pin pin = pins.getPinByNumber(pinIdx);
                    if (pin.getId() == 1 || pin.getId() == 2 || pin.getId() == 3) {
                        currentId = (int)pin.getId()-1;
                        pinArray[currentId] = new PteidPin();
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
            }
            catch (PTEID_Exception ex)
            {
                throw new PteidException(ex.GetError());
            }
        }

        return pinArray;
    }
    
    
    public static PteidTokenInfo GetTokenInfo() {
        PteidTokenInfo token = null;

        if (readerContext != null) {
            try {
                PTEID_CardVersionInfo info = idCard.getVersionInfo();
                token = new PteidTokenInfo(info.getTokenLabel(), info.getSerialNumber());
            }
            catch (PTEID_Exception ex)
            {
                throw new PteidException(ex.GetError());
            }
        }

        return token;
    }
    
    
    public static byte[] ReadSOD() {
        byte[] sod = null;

        if (readerContext != null) {
            try {
                PTEID_ByteArray pba = idCard.getSod().getData();
               
                int trimmedSize = trimStart(pba.GetBytes());
                
                sod = new byte[trimmedSize];
                Array.Copy(pba.GetBytes(), 0, sod, 0, sod.Length);
            }
            catch (PTEID_Exception ex)
            {
                throw new PteidException(ex.GetError());
            }
        }
        return sod;
    }
    
    
    public static int UnblockPIN(byte pinId, String puk, String newPin) {
		return UnblockPIN_Ext(pinId, puk, newPin, UNBLOCK_FLAG_NEW_PIN);
    }
    
    
    public static int UnblockPIN_Ext(byte pinId, String puk, String newPin, uint flags)
    {
        uint ul = 0;
        int ret = -1;

        if (readerContext != null)
        {
            try
            {

                if (pinId != 1 && pinId != PTEID_AUTH_PIN && pinId != PTEID_SIGNATURE_PIN && pinId != PTEID_ADDRESS_PIN)
                    return 0;

                PTEID_Pins pins = idCard.getPins();
                for (uint pinIdx = 0; pinIdx < pins.count(); pinIdx++)
                {
                    PTEID_Pin pin = pins.getPinByNumber(pinIdx);
                    if (pin.getPinRef() == pinId)
                    {
                      if (!pin.unlockPin(puk, newPin, ref ul, flags))
                      {
                          if (ul == 0)
                              throw new PteidException(SC_ERROR_AUTH_METHOD_BLOCKED);
                          else
                              throw new PteidException(SC_ERROR_PIN_CODE_INCORRECT);
                      }
                      ret = (int)ul;
                    }
                }
            }
            catch (PTEID_Exception ex)
            {
                handleUnderlyingException(ex);
            }
        }

        return ret;
    }
  
  
    public static void SelectADF(byte[] bytes) {
        if (readerContext != null) {
            try {
                byte[] ap = {0x00, (byte) 0xA4, 0x00, 0x0C};
                PTEID_ByteArray apdu = new PTEID_ByteArray(ap, (uint)ap.Length);
                byte[] temp = new byte[1];
                temp[0] = (byte) bytes.Length;
                apdu.Append(temp, 1);
                apdu.Append(bytes, (uint)bytes.Length);
                PTEID_ByteArray apdu_result = idCard.sendAPDU(apdu);
                //TODO: check return code ?
            }
            catch (PTEID_Exception ex)
            {
                throw new PteidException(ex.GetError());
            }
        }
    }
    
    
    public static byte[] ReadFile(byte[] fileID, byte pinId) {
        PTEID_ByteArray pb = new PTEID_ByteArray();
        byte[] retArray = null;
        PTEID_Pin pin = null;
        
        if (readerContext != null) {
            try {

                if (pinId != 0) {
                    PTEID_Pins pins = idCard.getPins();
                    for (uint pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
                        pin = (pins.getPinByNumber(pinIdx));
                        if (pin.getPinRef() == pinId )
                            break;
                    }
                }

                idCard.readFile(ashex(fileID), pb, pin);
                  
                int trimmedSize = trimStart(pb.GetBytes());
                if ((trimmedSize == 0) && (pb.Size() > 0)) trimmedSize = (int)pb.Size();
               
                retArray = new byte[trimmedSize];
                Array.Copy(pb.GetBytes(), 0, retArray, 0, retArray.Length);
            }
            catch (PTEID_Exception ex)
            {
                handleUnderlyingException(ex);
            }
        }
        return retArray;
    }
    
    public static void WriteFile(byte[] file, byte[] data, byte pinId) {
        WriteFileInOffset(file, data, pinId, 0 /* inOffset */);
    }

    public static void WriteFileInOffset(byte[] file, byte[] data, byte pinId, int offset)    {
        PTEID_ByteArray pb = new PTEID_ByteArray(data, (uint)data.Length);
        PTEID_Pin pin = null;

        if (readerContext != null)
        {
            try
            {
                if (pinId != 0)
                {
                    PTEID_Pins pins = idCard.getPins();
                    for (uint pinIdx = 0; pinIdx < pins.count(); pinIdx++)
                    {
                        pin = (pins.getPinByNumber(pinIdx));
                        if (pin.getPinRef() == pinId)
                            break;
                    }
                }

                bool ret = idCard.writeFile(ashex(file), pb, pin, "", (uint)offset);

            }
            catch (PTEID_Exception ex)
            {
                handleUnderlyingException(ex);
            }
        }
    }

    public static uint IsActivated() {
        if (readerContext != null) {
            try {
                return idCard.isActive() ? (uint)1 : (uint) 0;
            }
            catch (PTEID_Exception ex) {
                throw new PteidException(ex.GetError());
            }
        }
        return 0;
    }
    
    
    public static void Activate(String actPin, byte[] bytes, uint activateMode) {
        PTEID_ByteArray pb = new PTEID_ByteArray(bytes, (uint)bytes.Length);
        if (readerContext != null) {
            try {
		        bool mode = activateMode == MODE_ACTIVATE_BLOCK_PIN;
                idCard.Activate(actPin, pb, mode);
            }
            catch (PTEID_Exception ex) {
				if (ex.GetError() == pteidlib_dotNet.EIDMW_ERR_PIN_CANCEL)
					throw new PteidException(SC_ERROR_PIN_CODE_INCORRECT);
				else
					throw new PteidException(ex.GetError());
            }
        }
    }
    
    
    public static void SetSODChecking(int doCheck) {
        if (readerContext != null) {
            try {
                bool mode = doCheck == 1;
                readerContext.getEIDCard().doSODCheck(mode);
            }
            catch (PTEID_Exception ex)
            {
                throw new PteidException(ex.GetError());
            }
        }
    }

    public static void SetSODCAs(PteidCertif[] certificates)
    {
        if (readerContext != null)
        {
            try
            {
                if (null == certificates)
                {
                    readerContext.getEIDCard().getCertificates().resetSODCAs();
                    return;
                }

                foreach (PteidCertif pcert in certificates)
                {
                    PTEID_ByteArray pba = new PTEID_ByteArray(pcert.certif, (uint)pcert.certif.Length);
                    readerContext.getEIDCard().getCertificates().addToSODCAs(pba);
                }
            }
            catch (PTEID_Exception ex)
            {
                throw new PteidException(ex.GetError());
            }
        }
    }
    
    public static PteidRSAPublicKey GetCardAuthenticationKey(){
        PteidRSAPublicKey key = null;

        if (readerContext != null) {
            try {
                PTEID_PublicKey cardKey = idCard.getID().getCardAuthKeyObj();
                key = new PteidRSAPublicKey();
                key.exponent = new byte[(int) cardKey.getCardAuthKeyExponent().Size()];
                key.modulus = new byte[(int) cardKey.getCardAuthKeyModulus().Size()];
                Array.Copy(cardKey.getCardAuthKeyExponent().GetBytes(), 0, key.exponent, 0, key.exponent.Length);
                Array.Copy(cardKey.getCardAuthKeyModulus().GetBytes(), 0, key.modulus, 0, key.modulus.Length);
            }
            catch (PTEID_Exception ex)
            {
                throw new PteidException(ex.GetError());
            }
        }
        
        return key;
    }
    
    
    public static PteidRSAPublicKey GetCVCRoot(){
        PteidRSAPublicKey key = null;

        if (readerContext != null) {
            try {
                PTEID_PublicKey rootCAKey = idCard.getRootCAPubKey();
                key = new PteidRSAPublicKey();
                key.exponent = new byte[(int) rootCAKey.getCardAuthKeyExponent().Size()];
                key.modulus = new byte[(int) rootCAKey.getCardAuthKeyModulus().Size()];
                Array.Copy(rootCAKey.getCardAuthKeyExponent().GetBytes(), 0, key.exponent, 0, key.exponent.Length);
                Array.Copy(rootCAKey.getCardAuthKeyModulus().GetBytes(), 0, key.modulus, 0, key.modulus.Length);
            }
            catch (PTEID_Exception ex)
            {
                throw new PteidException(ex.GetError());
            }
        }
        
        return key;
    }
    
    
    public static byte[] SendAPDU(byte[] bytes) {
        byte[] ret = null;
        if (readerContext != null) {
            try {
                PTEID_ByteArray sApdu = new PTEID_ByteArray(bytes, (uint)bytes.Length);
                PTEID_ByteArray resp;

                resp = idCard.sendAPDU(sApdu);

                ret = new byte[(int) resp.Size()];
                Array.Copy(resp.GetBytes(), 0, ret, 0, ret.Length);
            }
            catch (PTEID_Exception ex)
            {
                throw new PteidException(ex.GetError());
            }
        }
        
        return ret;
    }

    public static int IsPinpad()  {
        if (readerContext != null)
        {
            try
            {
                return (readerContext.isPinpad() ? 1 : 0);
            }
            catch (PTEID_Exception ex)
            {
                throw new PteidException(ex.GetError());
            }
        }
        return 0;
    }
	
	public static byte[] CVC_Init(byte[] cvc_cert)
	{
        byte[] ret = null;

        PTEID_ByteArray cvc = new PTEID_ByteArray(cvc_cert, (uint)cvc_cert.Length);
        //Make the Exception retro-compatible
        try
        {
            PTEID_ByteArray ba = pteidlib_dotNet.PTEID_CVC_Init(cvc);
            ret = new byte[(int)ba.Size()];
            Array.Copy(ba.GetBytes(), ret, ret.Length);
            
        }
        catch (PTEID_Exception ex)
        {
            throw new PteidException(ex.GetError());
        }

        return ret;
		
	}
	
	public static void CVC_Authenticate(byte[] signedChallenge)
	{
        PTEID_ByteArray signed_challenge = new PTEID_ByteArray(signedChallenge, (uint)signedChallenge.Length);
        try
        {
            pteidlib_dotNet.PTEID_CVC_Authenticate(signed_challenge);

        }
        catch (PTEID_Exception ex)
        {
            throw new PteidException(ex.GetError());
        }
	}
	
	public static byte[] CVC_ReadFile(byte[] fileID)
	{
        byte[] ret = null;

        PTEID_ByteArray ba_fileID = new PTEID_ByteArray(fileID, (uint)fileID.Length);
        //TODO: Make the Exception retro-compatible
        try
        {
            PTEID_ByteArray ba = pteidlib_dotNet.PTEID_CVC_ReadFile(ba_fileID);
            ret = new byte[(int)ba.Size()];
            Array.Copy(ba.GetBytes(), ret, ret.Length);
        }
        catch (PTEID_Exception ex)
        {
            throw new PteidException(ex.GetError());
        }
        return ret;
		
	}
	
	public static PteidAddr CVC_GetAddr()
	{
        byte[] address_fileID = { 0x3F, 0x00, 0x5F, 0x00, 0xEF, 0x05 };
        byte[] address_ba = CVC_ReadFile(address_fileID);

        return new PteidAddr(address_ba);
	}


   private static String findReaderNameWithCard(){
	long nrReaders  = readerSet.readerCount();
        
	for ( uint readerIdx=0; readerIdx<nrReaders; readerIdx++)
            if (readerContext.isCardPresent())
                return readerSet.getReaderName(readerIdx);
	
        return null;
   }
}

}
