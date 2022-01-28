/* ****************************************************************************
 *
 * PT eID Middleware Project.
 *
 * Copyright (C) 2021-2022 André Guerreiro - <aguerreiro1985@gmail.com>
*/

#import "Token.h"
#import "PteidSession.h"
#include <os/log.h>

@implementation PteidAuthOperation

- (nullable instancetype)initWithSession:(PteidTokenSession *)session {
    NSLog(@"PTEID initWithSession called");
    const unsigned int PinId_index = 3;
    
    if (self = [super init]) {
        _session = session;
        
        self.smartCard = session.smartCard;
        //XX: these variables are not used ATM
        //const UInt8 template[] = {self.smartCard.cla, 0x20, 0x00, 0x82, 0x08, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
        //self.APDUTemplate = [NSData dataWithBytes:template length:sizeof(template)];
        self.PINFormat = [[TKSmartCardPINFormat alloc] init];
        self.PINFormat.minPINLength = 4;
        self.PINFormat.maxPINLength = 8;
        //self.PINFormat.PINBitOffset = 5 * 8;
    }
    
    TKSmartCardPINFormat *PINFormat;
    TKSmartCardUserInteractionForSecurePINVerification *userInter;
    NSData *APDUTemplate;
    
    UInt8 template[] = {0x00, 0x20, 0x00, 0x00, 0x08, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    template[PinId_index] = session.use_auth_key ? 0x81 : 0x82;
    
    APDUTemplate = [NSData dataWithBytes:template length:sizeof(template)];
    PINFormat = [[TKSmartCardPINFormat alloc] init];
    PINFormat.PINBlockByteLength = 8;
    PINFormat.PINLengthBitOffset = 0;
    PINFormat.PINLengthBitSize = 0;
    PINFormat.PINBitOffset = 0;
    PINFormat.encoding = TKSmartCardPINEncodingASCII;
    PINFormat.minPINLength = 4;
    PINFormat.maxPINLength = 8;
    PINFormat.charset = TKSmartCardPINCharsetNumeric;
    PINFormat.PINJustification = TKSmartCardPINJustificationLeft;
    
    // try to Verify PIN on the card reader
    NSData *data = [NSData dataWithBytes:template length:sizeof template];
    // using PINByteOffset:0 (in stead of 5) as requested due to not currently used
    //(see https://developer.apple.com/documentation/cryptotokenkit/tksmartcard/1390289-userinteractionforsecurepinverif?language=objc)
    userInter = [self.smartCard userInteractionForSecurePINVerificationWithPINFormat:PINFormat APDU:data PINByteOffset:0];
    
    if (nil == userInter)
    {
#ifdef DEBUG
        os_log_error(OS_LOG_DEFAULT, "Not using a pinpad reader!");
#endif
        return self;
    }
    
    //reader is supporting secure PIN entry
    dispatch_semaphore_t sema = dispatch_semaphore_create(0);
    NSArray *messages = @[@0];//0 is Pin insertion prompt, and only this message is needed
    userInter.PINMessageIndices = messages;
    userInter.PINCompletion = TKSmartCardPINCompletionKey;//is 2
    
    userInter.initialTimeout = 300;
    userInter.interactionTimeout = 300;
    
#ifdef DEBUG
    os_log_info(OS_LOG_DEFAULT, "Enter the PIN on the pinpad");
#endif
    [userInter runWithReply:^(BOOL success, NSError *error)
     {
        NSLog(@"TokenSession Pinpad verification completed %@ %@ %04X", @(success), error, userInter.resultSW);
        
#ifdef DEBUG
        NSLog(@"Smartcard reader name: %@", self.session.smartCard.slot.name);
#endif
        switch (userInter.resultSW)
        {
            case 0x9000:
                // Mark card session sensitive, because we entered PIN into it and no session should access it in this state.
                self.smartCard.sensitive = YES;
                
                // Remember in card context that the card is authenticated.
                self.smartCard.context = @(YES);
                
                // Mark BEIDTokenSession as freshly authorized.
                self.session.authState = PteidAuthStateFreshlyAuthorized;
                break;
            case 0x63C1:
            case 0x63C2:
            case 0x6983:
            case 0x6984:
            {
                int triesLeft = (userInter.resultSW == 0x6983 || userInter.resultSW == 0x6984) ? 0 : userInter.resultSW & 0x3f;
                self.session.isCanceled = triesLeft == 0;
                
                if (triesLeft == 0) {
                    NSLog(@"Pinpad verification failed: PIN blocked!");
                }
                else {
                    NSLog(@"Pinpad verification failed: %d tries left", triesLeft);
                }
                self.smartCard.sensitive = NO;
                break;
            }
            case 0x6400: // Timeout
            case 0x6401: // Cancel
                self.session.isCanceled = YES;
            default:
                self.smartCard.sensitive = NO;
                break;
        }
        dispatch_semaphore_signal(sema);
    }];
    //Wait max 30 seconds for user to enter PIN on secure PIN pad reader
    dispatch_time_t  waitTime = dispatch_time(DISPATCH_TIME_NOW,  300000000000);
    dispatch_semaphore_wait(sema, waitTime);
    
    //NSLog(@"PIN handled by reader");
    
    //If we're using a pinpad reader the AuthOperation is done, we don't fallback to normal PIN prompt here
    return nil;
}

// Remove this as soon as PIVAuthOperation implements automatic PIN submission according to APDUTemplate.
- (BOOL)finishWithError:(NSError * _Nullable __autoreleasing *)error {
    
    if(self.PIN == nil){
        NSLog(@"PTEID finishWithError called PIN == nil");
        return NO;
    }
    
    // Format PIN as UTF-8, right padded with 0xff to 8 bytes.
    NSMutableData *PINData = [NSMutableData dataWithLength:8];
    memset(PINData.mutableBytes, 0xff, PINData.length);
    
    NSLog(@"PteidAuthOperation finishWithError() called");
    
    NSData * pin_bytes = [self.PIN dataUsingEncoding:NSASCIIStringEncoding];
    NSUInteger bytes_to_copy = pin_bytes.length;
    [pin_bytes getBytes:PINData.mutableBytes length:bytes_to_copy];
    
    // Send VERIFY command to the card.
    UInt16 sw;
    
    if ([self.smartCard sendIns:0x20 p1:0x00 p2: self.session.use_auth_key ? 0x81 : 0x82 data:PINData le:nil sw:&sw error:error] == nil) {
        os_log(OS_LOG_DEFAULT, "Verify PIN sendIns failed");
        return NO;
    }
    if ((sw & 0xff00) == 0x6300 || (sw == 0x6983) || (sw == 0x6984)) {
        int triesLeft = (sw == 0x6983 || sw == 0x6984) ? 0 : sw & 0x3f;
        NSLog(@ "Failed to verify PIN sw:0x%04x retries: %d", sw, triesLeft);
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeAuthenticationFailed userInfo:
                      @{NSLocalizedDescriptionKey: [NSString localizedStringWithFormat: NSLocalizedString(@"VERIFY_TRY_LEFT", nil), triesLeft]}];
        }
        return NO;
    }
    else if (sw != 0x9000) {
        NSLog(@"Failed to verify PIN unexpected error sw: 0x%04x", sw);
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeAuthenticationFailed userInfo:
                      @{NSLocalizedDescriptionKey: [NSString localizedStringWithFormat: NSLocalizedString(@"VERIFY_TRY_LEFT", nil), 0]}];
        }
        return NO;
    }
    
    // Mark card session sensitive, because we entered PIN into it and no session should access it in this state.
    self.smartCard.sensitive = YES;
    
    // Remember in card context that the card is authenticated.
    //self.session.smartCard.context = @(YES);
    self.smartCard.context = @(YES);
    
    // Mark PTEIDTokenSession as freshly authorized.
    self.session.authState = PteidAuthStateFreshlyAuthorized;
    
    return YES;
}

//TODO: missing support for secure PIN verification with PINpad readers

@end


@implementation PteidTokenSession

- (instancetype)initWithToken:(PteidToken *)token {
    PteidTokenSession * session = [super initWithToken:token];
    _isCanceled = NO;
    return session;
}

- (TKTokenAuthOperation *)tokenSession:(TKTokenSession *)session
                          beginAuthForOperation:(TKTokenOperation)operation
                          constraint:(TKTokenOperationConstraint)constraint
                          error:(NSError * _Nullable __autoreleasing *)error {
    
    //TODO: verify constraint and operation parameters
    NSLog(@"Building object PteidAuthOperation!....");
    if (![constraint isEqual:PteidConstraintPINAlways]) {
        os_log_error(OS_LOG_DEFAULT, "attempt to evaluate unsupported constraint %@", constraint);
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeBadParameter userInfo:@{NSLocalizedDescriptionKey: NSLocalizedString(@"WRONG_CONSTR", nil)}];
        }
        return nil;
    }
    TKTokenAuthOperation * tokenAuth = [[PteidAuthOperation alloc] initWithSession:self];
    if(tokenAuth == nil)
    {
        if (_isCanceled) {
            if (error != nil) {
                  *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeCanceledByUser userInfo:nil];
            }
            return nil;
        }
       return [[TKTokenAuthOperation alloc] init];
    }
        
    return tokenAuth;
}

- (BOOL)tokenSession:(TKTokenSession *)session supportsOperation:(TKTokenOperation)operation usingKey:(TKTokenObjectID)keyObjectID algorithm:(TKTokenKeyAlgorithm *)algorithm {
    // Indicate whether the given key supports the specified operation and algorithm.
    TKTokenKeychainKey *keyItem = [self.token.keychainContents keyForObjectID:keyObjectID error:nil];
     if (keyItem == nil) {
         NSLog(@"KeyItem not found!");
         return NO;
     }
    
    if (operation != TKTokenOperationSignData) {
        return NO;
    }
    NSLog(@"Signing key with KeyLabel: %@", keyItem.label);
    _use_auth_key = [keyItem.label compare:@PTEID_KEY1_LABEL] == NSOrderedSame;
    
    BOOL returnValue = [algorithm isAlgorithm:kSecKeyAlgorithmRSASignatureRaw] && [algorithm supportsAlgorithm:kSecKeyAlgorithmRSASignatureDigestPKCS1v15Raw];
    if (!returnValue && self.card_type == CARD_IAS_V4_OR_GREATER) {
        returnValue = [algorithm isAlgorithm:kSecKeyAlgorithmRSASignatureDigestPSSSHA256]
                        || [algorithm isAlgorithm:kSecKeyAlgorithmRSASignatureDigestPSSSHA384]
                        || [algorithm isAlgorithm:kSecKeyAlgorithmRSASignatureDigestPSSSHA512];
        
    }
    
    NSLog(@"PTEID supportsOperation returning %i", returnValue);

    return returnValue;
}

const char * hashAlgoToString(PteidHashAlgo algo) {
    switch (algo) {
        case PteidHashNone:
            return "No hash!";
        case PteidHashSHA1:
            return "SHA-1";
        case PteidHashSHA256:
            return "SHA-256";
        case PteidHashSHA384:
            return "SHA-384";
        case PteidHashSHA512:
            return "SHA-512";
    }

}


const char sha1_digestinfo[]   = {0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a, 0x05, 0x00, 0x04, 0x14 };
const char sha256_digestinfo[] = {0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20 };
const char sha384_digestinfo[] = {0x30, 0x41, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02, 0x05, 0x00, 0x04, 0x30 };
const char sha512_digestinfo[] = {0x30, 0x51, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03, 0x05, 0x00, 0x04, 0x40 };

void matchDigestAlgorithmInRawRSAInputData(NSData *data, unsigned long start_offset, PteidHashAlgo *algo) {

    const char *data_p = data.bytes+start_offset;

    if (memcmp(data_p, sha1_digestinfo, sizeof sha1_digestinfo) == 0) {
        NSLog(@"SHA-1 input hash detected! Legacy signature algorithm - application should be updated");
        *algo = PteidHashSHA1;
    }
    else if (memcmp(data_p, sha256_digestinfo, sizeof sha256_digestinfo) == 0) {
        *algo = PteidHashSHA256;
    }
    else if (memcmp(data_p, sha384_digestinfo, sizeof sha384_digestinfo) == 0) {
        *algo = PteidHashSHA384;
    }
    else if (memcmp(data_p, sha512_digestinfo, sizeof sha512_digestinfo) == 0) {
        *algo = PteidHashSHA512;
    }
 
}

- (NSData *)tokenSession:(TKTokenSession *)session signData:(NSData *)dataToSign usingKey:(TKTokenObjectID)keyObjectID algorithm:(TKTokenKeyAlgorithm *)algorithm error:(NSError **)error {
    __block NSData *signature;
    PteidHashAlgo hash_algo = PteidHashNone;
    unsigned char prefix_pso_hash[] = {0x90, 0x00};
    unsigned char dst_bytes[] = {0x80, 0x01, 0x00, 0x84, 0x01, 0x00};
    
    NSLog(@"PteidTokenSession signData was called with inputData length: %lu algorithm: %@", dataToSign.length, algorithm);
    TKTokenKeychainKey * signing_key = [self.token.keychainContents keyForObjectID:keyObjectID error:error];
    if (signing_key != nil) {
        NSLog(@"Signing key with ID: %@ KeyLabel: %@", signing_key.objectID, signing_key.label);
        _use_auth_key = [signing_key.label compare:@PTEID_KEY1_LABEL] == NSOrderedSame;
    }
    else {
        return nil;
    }
    
    if (self.authState == PteidAuthStateUnauthorized || self.smartCard.context == nil) {
        NSLog(@"DEBUG: Unauthorized signData attempt!!");
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeAuthenticationNeeded userInfo:nil];
        }
        return nil;
    }
       
    // Insert code here to sign data using the specified key and algorithm.
    signature = nil;
    
    NSData * hash_to_sign = nil;
    NSLog(@"signData input hash: %@", [dataToSign description]);
    
    if ([algorithm isAlgorithm:kSecKeyAlgorithmRSASignatureRaw]) {
        NSLog(@"Pteid TokenSession - skip the PKCS#1 1.5 padding bytes");
        //  00 01 FF FF 00 ....
        const char *data_p = dataToSign.bytes;
        char *e = strchr(&data_p[3], '\0'); // Start at pos 3
        if (e != NULL) {
            NSUInteger pos = (NSUInteger)(e - data_p) + 1;
            matchDigestAlgorithmInRawRSAInputData(dataToSign, pos, &hash_algo);
            switch (hash_algo) {
                case PteidHashSHA256:
                    dst_bytes[2] = 0x42;
                    pos += sizeof sha256_digestinfo;
                    break;
                case PteidHashSHA384:
                    dst_bytes[2] = 0x52;
                    pos += sizeof sha384_digestinfo;
                    break;
                case PteidHashSHA512:
                    dst_bytes[2] = 0x62;
                    pos += sizeof sha512_digestinfo;
                    break;
                case PteidHashSHA1:
                    dst_bytes[2] = 0x12;
                    pos += sizeof sha1_digestinfo;
                    break;
                case PteidHashNone:
                    NSLog(@"Failed to match input hash algo!");
                    return nil;
            }
    
            hash_to_sign = [dataToSign subdataWithRange:NSMakeRange(pos, dataToSign.length - pos)];
        }
        else {
            NSLog(@"Invalid PKCS#1 padding - couldn't find second zero byte");
            return signature;
        
        }
    }
    else if ([algorithm isAlgorithm:kSecKeyAlgorithmRSASignatureDigestPSSSHA256]) {
        NSLog(@"RSA-PSS SHA-256 selected");
        dst_bytes[2] = 0x45;
        hash_to_sign = dataToSign;
    }
    else if ([algorithm isAlgorithm:kSecKeyAlgorithmRSASignatureDigestPSSSHA384]) {
        NSLog(@"RSA-PSS SHA-384 selected");
        dst_bytes[2] = 0x55;
        hash_to_sign = dataToSign;
    }
    else if ([algorithm isAlgorithm:kSecKeyAlgorithmRSASignatureDigestPSSSHA512]) {
        NSLog(@"RSA-PSS SHA-512 selected");
        dst_bytes[2] = 0x65;
        hash_to_sign = dataToSign;
    }
    
    prefix_pso_hash[1] = (unsigned char) [hash_to_sign length];
    
    dst_bytes[5] = _use_auth_key ? 0x02: 0x01;
    NSMutableData * pso_hash_data = [NSMutableData alloc];
    [pso_hash_data appendBytes:prefix_pso_hash length:sizeof(prefix_pso_hash)];
    [pso_hash_data appendBytes:hash_to_sign.bytes length:hash_to_sign.length];
    
    NSData * dst_data = [NSData dataWithBytes:dst_bytes length:sizeof(dst_bytes)];
    
    UInt16 sw = 0;
    NSLog(@"MSE-Set use_auth_key: %d", _use_auth_key);
    
    //MSE:set with digital signature template specifying the signing key and algo
    [self.smartCard sendIns:0x22 p1:0x41 p2:0xB6 data:dst_data le:nil sw:&sw error:error];
    if (sw != 0x9000) {
        NSLog(@"MSE-Set command failed! SW: %04x", sw);
        return nil;
    }
    //NSLog(@"PSO:Hash input data len: %lu", pso_hash_data.length);
    sw = 0;
    [self.smartCard sendIns:0x2A p1:0x90 p2:0xA0 data:pso_hash_data le:nil sw:&sw error:error];
    
    if (sw != 0x9000) {
        NSLog(@"PSO:Hash command failed! SW: %04x", sw);
        if (error != nil && sw == 0x6985) {
            NSLog(@"Unsupported hash type for signing in this card: %s", hashAlgoToString(hash_algo));
            //TODO: maybe use the userInfo dictionary to aid future debugging
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeNotImplemented userInfo:nil];
        }
        return nil;
    }
    sw = 0;
    signature = [self.smartCard sendIns:0x2A p1:0x9E p2:0x9A data:nil le:@0 sw:&sw error:error];
    if (signature == nil) {
        NSLog(@"PteidTokenSession signData signature transmission error: %@", *error);
        return nil;
    }
    switch (sw)
    {
        case 0x6982:
            NSLog(@"PteidTokenSession signData Unauthenticated!");
            if (error != nil) {
                *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeAuthenticationNeeded userInfo:nil];
            }
            return nil;
        case 0x9000:
            if (signature != nil)
                break;
        default:
            NSLog(@"PteidTokenSession signData failed to sign SW: %04x", sw);
            return nil;
    }
    
    return signature;
}

/* Pteid Card doesn't support encryption so these are empty stubs */
- (NSData *)tokenSession:(TKTokenSession *)session decryptData:(NSData *)ciphertext usingKey:(TKTokenObjectID)keyObjectID algorithm:(TKTokenKeyAlgorithm *)algorithm error:(NSError **)error {
    NSData *secret = nil;

    *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeNotImplemented userInfo:@{NSLocalizedDescriptionKey: @"Decryption is not implemented in PteidToken!"}];

    return secret;
}

- (NSData *)tokenSession:(TKTokenSession *)session performKeyExchangeWithPublicKey:(NSData *)otherPartyPublicKeyData usingKey:(TKTokenObjectID)objectID algorithm:(TKTokenKeyAlgorithm *)algorithm parameters:(TKTokenKeyExchangeParameters *)parameters error:(NSError **)error {
    NSData *secret = nil;

    *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeNotImplemented userInfo:@{NSLocalizedDescriptionKey: @"KeyExchange is not implemented in PteidToken!"}];

    return secret;
}

@end
