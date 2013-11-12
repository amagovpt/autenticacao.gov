using System;
using System.Text;

namespace pt.portugal.eid
{
    public class Pteid {
    
    private static PTEID_ReaderContext readerContext = null;
    private static PTEID_ReaderSet readerSet = null;
    private static PTEID_EIDCard idCard = null;
    
    private static readonly int PTEID_ADDRESS_PIN = 131;
    private static readonly int PTEID_PIN_COUNT  = 3;
    
    public static readonly int CARD_TYPE_ERR = 0;
    public static readonly int CARD_TYPE_IAS07 = 1;
    public static readonly int CARD_TYPE_IAS101 = 2;
    
    protected static readonly char[] Hexhars = {
        '0', '1', '2', '3', '4', '5',
        '6', '7', '8', '9', 'A', 'B',
        'C', 'D', 'E', 'F'
    };
    
    
    
   private static String ashex(byte[] b){
       String st = "";
       
       for(int i=0; i<b.Length;i++){
          int temp = b[i] & 0x000000FF;
          st +=Hexhars[(temp >> 4)];
          st +=Hexhars[(temp & 0xf)];
       }
       
       return st;
   }
   
   
    private static int trimStart(byte[] array) {
        int trimmedSize = array.Length - 1;
        
        while (array[trimmedSize] == 0) 
            trimmedSize--;

        return trimmedSize;
    }

   
   public static void Init(String readerName){
        try {
            PTEID_ReaderSet.initSDK();
            readerSet = PTEID_ReaderSet.instance();
            if (readerName == null || readerName == String.Empty)
                readerContext = readerSet.getReaderByNum(0);
            else 
                readerContext = readerSet.getReaderByName(readerName);
            
            idCard = readerContext.getEIDCard();
        } catch (Exception ex) {
            throw new PteidException(0);
        }
   }
  
   
   public static void Exit(int value){
        try {
            PTEID_ReaderSet.releaseSDK();
        } catch (Exception ex) {
            throw new PteidException(0);
        }
   }
   
   
   public static int GetCardType(){
        try {
            PTEID_CardType cardType = readerContext.getCardType();
            
            return (int)cardType;
        } catch (Exception ex) {
            throw new PteidException(0);
        }
   }
   
   
   public static PTEID_ID GetID(){ 
       try {
           return new PTEID_ID(idCard.getID());
        } catch (Exception ex) {
            throw new PteidException(0);
        }
   }
   
   
    public static PTEID_ADDR GetAddr(){
        try {
            uint ul = 0;
            PTEID_Pins pins = idCard.getPins();
            for (uint i = 0; i < pins.count(); i++) {
                PTEID_Pin pin = pins.getPinByNumber(i);
                if (pin.getPinRef() == 131) {
                    if (pin.verifyPin("", ref ul,true)) {
                        return new PTEID_ADDR(idCard.getAddr());
                    }
                }
            }
        } catch (PteidException ex) {
            throw ex;
        } catch (Exception) {
            throw new PteidException(0);
        }
        throw new PteidException(0);
    }
    
    
    public static PTEID_PIC GetPic(){
        PTEID_PIC pic = null;
        
        try {
            pic = new PTEID_PIC();
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
        } catch (Exception ex) {
            throw new PteidException(0);
        }
        
        return pic;
    }
    

    public static PTEID_Certif[] GetCertificates(){
        PTEID_Certif[] certs = null;
        PTEID_ByteArray ba = new PTEID_ByteArray();
        
        try {
            PTEID_Certificates certificates = idCard.getCertificates();
            certs = new PTEID_Certif[(int)certificates.countAll()];
            for(uint i=0;i<certs.Length;i++){
                certs[i] = new PTEID_Certif();
                certificates.getCert(i).getFormattedData(ba);
                certs[i].certif = new byte[(int)(ba.Size())];
                Array.Copy(ba.GetBytes(), 0, certs[i].certif, 0, (int)ba.Size());
                certs[i].certifLabel = certificates.getCert(i).getLabel();
            }
        } catch (Exception ex) {
            throw new PteidException(0);
        }

        return certs;
    }
    
    
    
    public static int VerifyPIN(byte pinId, String str){
        uint ul = 0;
        int triesLeft =  0;
            
        if (readerContext != null) {
            if (pinId != 1 && pinId != 129 && pinId != 130 && pinId != 131) {
                return 0;
            }
            try {
                PTEID_Pins pins = idCard.getPins();
                for (uint pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
                    PTEID_Pin pin = pins.getPinByNumber(pinIdx);
                    if (pin.getPinRef() == pinId) {
                        pin.verifyPin("", ref ul,true);
                        //martinho: verify pin is not working properly for readers without pinpad at this moment,
                        //this is a workaround
                        triesLeft = pin.getTriesLeft();
                    }
                }
                return triesLeft;
            } catch (Exception ex) {
                throw new PteidException(0);
            }
        }
        return triesLeft;
    }
    
    
    public static int ChangePIN(byte pinId, String oldPin, String newPin){
        uint ul = 0;
        int triesLeft = 0;

        if (readerContext != null) {
            if (pinId != 1 && pinId != 129 && pinId != 130 && pinId != 131) {
                return 0;
            }
            try {
                PTEID_Pins pins = idCard.getPins();
                for (uint pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
                    PTEID_Pin pin = pins.getPinByNumber(pinIdx);
                    if (pin.getPinRef() == pinId)
                        if (pin.changePin(oldPin, newPin, ref ul, pin.getLabel(),true)) {
                            triesLeft = pin.getTriesLeft();
                            return 0;
                        } 
                }
            } catch (Exception ex) {
                throw new PteidException(0);
            }
        }
        return 0;
    }
   
    
    public static PTEIDPin[] GetPINs(){
        PTEIDPin[] pinArray = null;
        int currentId;
        
        if (readerContext != null) {
            try {
                PTEID_Pins pins = idCard.getPins();
                pinArray = new PTEIDPin[PTEID_PIN_COUNT];
                for (uint pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
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
                throw new PteidException(0);
            }
        }

        return pinArray;
    }
    
    
    public static PTEID_TokenInfo GetTokenInfo(){
        PTEID_TokenInfo token = null;

        if (readerContext != null) {
            try {
                PTEID_CardVersionInfo info = idCard.getVersionInfo();
                token = new PTEID_TokenInfo(info.getTokenLabel(), info.getSerialNumber());
            } catch (Exception ex) {
                throw new PteidException(0);
            }
        }

        return token;
    }
    
    
    public static byte[] ReadSOD(){
        byte[] sod = null;

        if (readerContext != null) {
            try {
                PTEID_ByteArray pba = idCard.getSod().getData();
               
                int trimmedSize = trimStart(pba.GetBytes());
                
                sod = new byte[trimmedSize];
                Array.Copy(pba.GetBytes(), 0, sod, 0, sod.Length);
            } catch (Exception ex) {
                throw new PteidException(0);
            }
        }
        return sod;
    }
    
    
    public static int UnblockPIN(byte pinId, String puk, String newPin){
        uint ul = 0;

        if (readerContext!=null){
            try {
            
		if (pinId != 1 && pinId != 129 && pinId != 130 && pinId != 131)
			return 0;

		PTEID_Pins pins = idCard.getPins();
		for (uint pinIdx=0; pinIdx < pins.count(); pinIdx++){
			PTEID_Pin pin = pins.getPinByNumber(pinIdx);
			if (pin.getPinRef() == pinId){
				pin.unlockPin(puk, newPin, ref ul);
			}
		}
            } catch (Exception ex) {
                throw new PteidException(0);
            }
	}

	return (int)ul;
    }
    
    
    public static int UnblockPIN_Ext(byte b, String puk, String pin, int i){
    	return UnblockPIN(b,puk, pin);
    }
    
    
    public static void SelectADF(byte[] bytes){
        if (readerContext != null) {
            try {
                byte[] ap = {0x00, (byte) 0xA4, 0x00, 0x0C};
                PTEID_ByteArray apdu = new PTEID_ByteArray(ap, (uint)ap.Length);
                byte[] temp = new byte[1];
                temp[0] = (byte) bytes.Length;
                apdu.Append(temp, 1);
                apdu.Append(bytes, (uint)bytes.Length);
                PTEID_ByteArray sendAPDU = idCard.sendAPDU(apdu);
                //verificar se correu tudo bem... ?
            } catch (Exception ex) {
                throw new PteidException(0);
            }
        }
    }
    
    
    public static byte[] ReadFile(byte[] bytes, byte pinId){
        PTEID_ByteArray pb = new PTEID_ByteArray();
        byte[] retArray = null;
        PTEID_Pin pin = null;
        
        if (readerContext != null) {
            try {

                if (pinId != 0) {
                    PTEID_Pins pins = idCard.getPins();
                    for (uint pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
                        pin = (pins.getPinByNumber(pinIdx));
                        if (pin.getPinRef() == PTEID_ADDRESS_PIN)
                            break;
                    }
                }

                idCard.readFile(ashex(bytes), pb, pin);
                  
                int trimmedSize = trimStart(pb.GetBytes());
               
                retArray = new byte[trimmedSize];
                Array.Copy(pb.GetBytes(), 0, retArray, 0, retArray.Length);
            } catch (Exception ex) {
                throw new PteidException(0);
            }
        }
        return retArray;
    }
    
    
    public static void WriteFile(byte[] file, byte[] data, byte pinId){
        PTEID_ByteArray pb = new PTEID_ByteArray(data, (uint)data.Length);
        PTEID_Pin pin = null;

        if (readerContext != null) {
            try {

                if (pinId != 0) {
                    PTEID_Pins pins = idCard.getPins();
                    for (uint pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
                        pin = (pins.getPinByNumber(pinIdx));
                        if (pin.getPinRef() == PTEID_ADDRESS_PIN)
                            break;
                    }
                }

                idCard.writeFile(ashex(file),pb,pin);
                
            } catch (Exception ex) {
                throw new PteidException(0);
            }
        }
    }
    
    public static int IsActivated(){
        if (readerContext != null) {
            try {
                return idCard.isActive() ? 1 : 0;
            } catch (Exception ex) {
                throw new PteidException(0);
            }
        }
        return 0;
    }
    
    
    public static void Activate(String actPin, byte[] bytes, int i){
        PTEID_ByteArray pb = new PTEID_ByteArray(bytes, (uint)bytes.Length);
        if (readerContext != null) {
            try {
                idCard.Activate(actPin, pb);
            } catch (Exception ex) {
                throw new PteidException(0);
            }
        }
    }
    
    
    public static void SetSODChecking(bool bln){
        if (readerContext != null) {
            try {
                readerContext.getEIDCard().doSODCheck(bln);
            } catch (Exception ex) {
                throw new PteidException(0);
            }
        }
    }


    public static void SetSODCAs(PTEID_Certif[] pteidcs){
        if (readerContext != null) {
            try {
                foreach (PTEID_Certif pcert in  pteidcs) {
                    PTEID_ByteArray pba = new PTEID_ByteArray(pcert.certif, (uint)pcert.certif.Length);
                    readerContext.getEIDCard().getCertificates().addCertificate(pba);
                }
            } catch (Exception ex) {
                throw new PteidException(0);
            }
        }
    }
    
    
    public static PTEID_RSAPublicKey GetCardAuthenticationKey(){
        PTEID_RSAPublicKey key = null;

        if (readerContext != null) {
            try {
                PTEID_PublicKey cardKey = idCard.getID().getCardAuthKeyObj();
                key = new PTEID_RSAPublicKey();
                key.exponent = new byte[(int) cardKey.getCardAuthKeyExponent().Size()];
                key.modulus = new byte[(int) cardKey.getCardAuthKeyModulus().Size()];
                Array.Copy(cardKey.getCardAuthKeyExponent().GetBytes(), 0, key.exponent, 0, key.exponent.Length);
                Array.Copy(cardKey.getCardAuthKeyModulus().GetBytes(), 0, key.modulus, 0, key.modulus.Length);
            } catch (Exception ex) {
                throw new PteidException(0);
            }
        }
        
        return key;
    }
    
    
    public static PTEID_RSAPublicKey GetCVCRoot(){
        PTEID_RSAPublicKey key = null;

        if (readerContext != null) {
            try {
                PTEID_PublicKey rootCAKey = idCard.getRootCAPubKey();
                key = new PTEID_RSAPublicKey();
                key.exponent = new byte[(int) rootCAKey.getCardAuthKeyExponent().Size()];
                key.modulus = new byte[(int) rootCAKey.getCardAuthKeyModulus().Size()];
                Array.Copy(rootCAKey.getCardAuthKeyExponent().GetBytes(), 0, key.exponent, 0, key.exponent.Length);
                Array.Copy(rootCAKey.getCardAuthKeyModulus().GetBytes(), 0, key.modulus, 0, key.modulus.Length);
            } catch (Exception ex) {
                throw new PteidException(0);
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
            } catch (Exception) {
            }
        }
        
        return ret;
    }
    
    public static long CAP_ChangeCapPin(String server_unused, byte[] bytes, PTEID_Proxy_Info ptdp, String string1, String string2){
        uint ul = 0;
        try {
            if (idCard.ChangeCapPinCompLayer(string1, string2, ref ul))
                return ul;
        } catch (PTEID_Exception ex) {
        	//TODO:
        }
        return -1;
    }

    public static int CAP_GetCapPinChangeProgress(){
        return 0;
    }

    public static void CAP_CancelCapPinChange(){
        return;
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
