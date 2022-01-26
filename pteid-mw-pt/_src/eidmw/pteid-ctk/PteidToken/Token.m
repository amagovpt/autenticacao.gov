/* ****************************************************************************
 *
 * PT eID Middleware Project.
 *
 * Copyright (C) 2021-2022 André Guerreiro - <aguerreiro1985@gmail.com>
*/

#import "Token.h"
#import "PteidSession.h"
#import <CryptoTokenKit/CryptoTokenKit.h>
#import <os/log.h>
#include <stdio.h>
#include <stdarg.h>

@implementation PteidToken

/*
BOOL selectApplication(TKSmartCard *card, NSError ** error) {
    UInt16 sw = 0;
    uint8_t gemsafe_aid[] = {0x60, 0x46, 0x32, 0xFF, 0x00, 0x00, 0x02};
    NSData *aid = [NSData dataWithBytes:gemsafe_aid length:sizeof gemsafe_aid];
    
    [card sendIns:0xA4 p1:0x04 p2:0x00 data:aid le:nil sw:&sw error:error];
    if (sw == 0x9000) {
        return TRUE;
    }
    NSLog(@"SelectApplication failed with SW=%04x", sw);
    if (error != nil) {
        *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeObjectNotFound userInfo:nil];
    }
    return FALSE;
} */

/* Select card DF or EF identified by file param  */
BOOL selectFile(TKSmartCard *card, UInt8 p1, UInt8 p2, NSData * file_id, NSError ** error) {
    UInt16 sw = 0;
    NSLog(@"Current card communication protocol: %lu CLA byte: %0hhx", [card currentProtocol], [card cla]);
    [card sendIns:0xA4 p1:p1 p2:p2 data:file_id le:nil sw:&sw error:error];
    if (sw == 0x9000) {
        return YES;
    }
    NSLog(@"SelectFile failed to select %@ with SW=%04x", file_id, sw);
       
    if (error != nil) {
        *error = (sw == 0x6A82) ? [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeObjectNotFound userInfo:nil] :
                 [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeBadParameter userInfo:nil];
    }
    return NO;
}

NSData * readBinary(TKSmartCard *card, UInt16 pos, NSNumber * le_byte, NSError ** error) {
        UInt16 sw = 0;
        NSData *data = [card sendIns:0xB0 p1:(pos >> 8) p2:pos data:nil le:le_byte sw:&sw error:error];
        if (sw == 0x9000) {
            return data;
        }
        NSLog(@"readBinary failed to read binary at pos %@", @(pos));
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeObjectNotFound userInfo:nil];
        }
        return nil;
}


NSData* readCompleteFile(TKSmartCard *card, NSData* file, NSError ** error) {
    NSNumber * le_byte = @0;
   
    BOOL ret = selectFile(card, 0x02, 0x0C, file, error);
    if (!ret) {
        return nil;
    }
    
    /* Read first block of file data to get the ASN.1 sequence length and then read enough blocks
       to fetch the remaining meaningful bytes
     */
    NSData *data = readBinary(card, 0, le_byte, error);
    if (data == nil) {
        return nil;
    }
    //Max certificate length in smart card
    unsigned int length = 2800;
    const unsigned char* CertBytes = (const unsigned char*)data.bytes;
    if ((data.length > 4) && (CertBytes[0] == 0x30) && (CertBytes[1] == 0x82)){
        length = (256 * CertBytes[2]) + CertBytes[3] + 4;
        NSLog(@"realCertLength = %d", length);
    }
              
    NSMutableData *fileData = [[NSMutableData alloc] init];
    [fileData appendData:data];
    while (fileData.length < length) {
        unsigned int bytes_left = (unsigned int)(length-fileData.length);
        data = readBinary(card, fileData.length,  bytes_left <= 256 ? [NSNumber numberWithUnsignedInt:bytes_left] : le_byte , error);
        if (data == nil) {
            return nil;
        }
        [fileData appendData:data];
    }
    return fileData;
}

NSString *readCardSerialNumber(TKSmartCard * card, NSError ** error) {
    uint8_t df_id_5f00[] = {0x5f, 0x00};
    uint8_t fileid_ef_id[] = {0xef, 0x02};
    uint8_t offset_pan   = 0xB6;
    NSNumber * len_pan   = @16;
    NSString * serial = NULL;
                     
    NSData *file_id = [NSData dataWithBytes:fileid_ef_id length:sizeof fileid_ef_id];
    NSData *df_id = [NSData dataWithBytes:df_id_5f00 length:sizeof df_id_5f00];
    //Select 5F00 DF
    if (!selectFile(card, 0x00, 0x0C, df_id, error)) {
          return nil;
    }
    //Select ID file
    if (!selectFile(card, 0x02, 0x0C, file_id, error)) {
        return nil;
    }
    
    UInt16 sw = 0;
    NSData * data = [card sendIns:0xB0 p1:00 p2:offset_pan data:nil le:len_pan sw:&sw error:error];
    if (sw == 0x9000) {
        serial = [[NSString alloc] initWithBytes:[data bytes] length: [len_pan unsignedIntValue] encoding:NSUTF8StringEncoding];
    }
    else {
        NSLog(@"ReadBinary for serial number failed with SW=%04x", sw);
    }
    
    return serial;
}

- (BOOL)populateIdentityFromSmartCard:(TKSmartCard *)smartCard into:(NSMutableArray<TKTokenKeychainItem *> *)items data:(NSData *)certificateData certificateTag:(UInt64)certificateTag name:(NSString *)certificateName keyTag:(TKTLVTag)keyTag name:(NSString *)keyName sign:(BOOL)sign keyManagement:(BOOL)keyManagement alwaysAuthenticate:(BOOL)alwaysAuthenticate error:(NSError **)error {
    

    NSLog(@"PTEID populateIdentityFromSmartCard called");

    TKTokenObjectID certificateID = @(certificateTag);

    if (certificateData == nil) {
        // If certificate cannot be found, just silently skip the operation, otherwise report an error.
        return (error != nil && [(*error).domain isEqual:TKErrorDomain] && (*error).code == TKErrorCodeObjectNotFound);
    }

    // Create certificate item.
    id certificate = CFBridgingRelease(SecCertificateCreateWithData(kCFAllocatorDefault, (CFDataRef)certificateData));
    if (certificate == NULL) {
        if (error != nil) {
            *error = [NSError errorWithDomain:TKErrorDomain code:TKErrorCodeCorruptedData userInfo:@{NSLocalizedDescriptionKey: NSLocalizedString(@"CORRUPTED_CERT", nil)}];
        }
        return NO;
    }
    TKTokenKeychainCertificate *certificateItem = [[TKTokenKeychainCertificate alloc] initWithCertificate:(__bridge SecCertificateRef)certificate objectID:certificateID];
    if (certificateItem == nil) {
        return NO;
    }
    
    [certificateItem setLabel:certificateName];
    
    // CA certificates don't have private key in the smartcard
    if (keyTag != 0) {
        // Create key item.
        TKTokenKeychainKey *keyItem = [[TKTokenKeychainKey alloc] initWithCertificate:(__bridge SecCertificateRef)certificate objectID:@(keyTag)];
        if (keyItem == nil) {
            return NO;
        }
        [keyItem setLabel:keyName];
        
        NSMutableDictionary<NSNumber *, TKTokenOperationConstraint> *constraints = [NSMutableDictionary dictionary];
        keyItem.canSign = sign;
        keyItem.suitableForLogin = sign;
        TKTokenOperationConstraint constraint = PteidConstraintPINAlways;
        if (sign) {
            constraints[@(TKTokenOperationSignData)] = constraint;
        }
        if ([keyItem.keyType isEqual:(id)kSecAttrKeyTypeRSA]) {
            keyItem.canDecrypt = FALSE;
        }
        keyItem.constraints = constraints;
        [items addObject:keyItem];
    }
    [items addObject:certificateItem];
    
    return YES;
}

BOOL check_nonnull_objects(int n, ...) {
    va_list vl;
    BOOL res = YES;
    NSData *val;
    va_start(vl, n);
    for (unsigned i=0; i<n; i++)
    {
       val=va_arg(vl, NSData *);
        if (val == NULL) {
           res = NO;
           break;
        }
    }
    va_end(vl);
    
    return res;
}

- (instancetype)initWithSmartCard:(TKSmartCard *)smartCard AID:(NSData *)AID tokenDriver:(PteidTokenDriver *)tokenDriver error:(NSError **)error {
    __block NSString *instanceID = nil;
    os_log_info(OS_LOG_DEFAULT, "initWithSmartCard called in PteidToken object: smartCard=%p", smartCard);

    uint8_t fileid_auth_cert[] = {0xef, 0x09};
    uint8_t fileid_sign_cert[] = {0xef, 0x08};
    //Intermediate CA certificates
    uint8_t fileid_auth_ca_cert[] = {0xef, 0x10};
    uint8_t fileid_sign_ca_cert[] = {0xef, 0x0F};
    
    instanceID = readCardSerialNumber(smartCard, error);
                     
    NSData *cert_file_id1 = [NSData dataWithBytes:fileid_auth_cert length:sizeof fileid_auth_cert];
    NSData *cert_file_id2 = [NSData dataWithBytes:fileid_sign_cert length:sizeof fileid_sign_cert];
    NSData *cert_file_id3 = [NSData dataWithBytes:fileid_auth_ca_cert length:sizeof fileid_auth_ca_cert];
    NSData *cert_file_id4 = [NSData dataWithBytes:fileid_sign_ca_cert length:sizeof fileid_sign_ca_cert];
    
    NSLog(@"instanceID= %@", instanceID);
    
    if (self = [super initWithSmartCard:smartCard AID:AID instanceID:instanceID tokenDriver:tokenDriver]) {
        NSMutableArray<TKTokenKeychainItem *> *items = [NSMutableArray arrayWithCapacity:4];
        
        NSData* cert_file_auth = readCompleteFile(smartCard, cert_file_id1, error);
        NSData* cert_file_sign = readCompleteFile(smartCard, cert_file_id2, error);
        NSData *cert_file_ca_auth = readCompleteFile(smartCard, cert_file_id3, error);
        NSData *cert_file_ca_sign = readCompleteFile(smartCard, cert_file_id4, error);
        
        if (check_nonnull_objects(4, cert_file_auth, cert_file_sign, cert_file_ca_auth, cert_file_ca_sign)) {
            NSLog(@"Read certificates files with size: %lu, %lu bytes", [cert_file_auth length], [cert_file_sign length]);
            if (![self populateIdentityFromSmartCard:smartCard into:items data:cert_file_auth certificateTag: 0xEF08 name:@"CC AUTH CERTIFICATE" keyTag:auth_key_object_id name:@PTEID_KEY1_LABEL sign:YES keyManagement:NO alwaysAuthenticate:YES error:error] ||
                ![self populateIdentityFromSmartCard:smartCard into:items data:cert_file_sign certificateTag: 0xEF09 name:@"CC SIGNATURE CERTIFICATE" keyTag:sign_key_object_id name:@PTEID_KEY2_LABEL sign:YES keyManagement:NO alwaysAuthenticate:YES error:error] ||
                ![self populateIdentityFromSmartCard:smartCard into:items data:cert_file_ca_sign certificateTag: 0xEF0F name:@"SIGNATURE SUBCA CERTIFICATE" keyTag:0 name:@"NO_KEY" sign:NO keyManagement:NO alwaysAuthenticate:NO error:error] ||
                ![self populateIdentityFromSmartCard:smartCard into:items data:cert_file_ca_auth certificateTag: 0xEF10 name:@"AUTHENTICATION SUBCA CERTIFICATE" keyTag:0 name:@"NO_KEY" sign:NO keyManagement:NO alwaysAuthenticate:NO error:error])
            {
                NSLog(@"Failed to populate Identity from smartcard (1)! Error: %@", *error);
                return nil;
           
            }
        }
        else {
            NSLog(@"Error reading auth or signature certificate file: %@", *error);
            return nil;
        }
        
        [self.keychainContents fillWithItems:items];
    }
    return self;
}

- (TKTokenSession *)token:(TKToken *)token createSessionWithError:(NSError **)error {
    return [[PteidTokenSession alloc] initWithToken:self];
}

@end
