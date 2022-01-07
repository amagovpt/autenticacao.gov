/* ****************************************************************************
 *
 * PT eID Middleware Project.
 *
 * Copyright (C) 2021-2022 André Guerreiro - <aguerreiro1985@gmail.com>
*/

#import <CryptoTokenKit/CryptoTokenKit.h>

NS_ASSUME_NONNULL_BEGIN

static const TKTokenOperationConstraint PteidConstraintPINAlways = @"PINAlways";
static const UInt64 auth_key_object_id = 0x81, sign_key_object_id=0x82;

@class PteidTokenDriver;
@class PteidToken;
@class PteidTokenSession;

@interface PteidTokenSession : TKSmartCardTokenSession<TKTokenSessionDelegate>
- (instancetype)initWithToken:(TKToken *)token delegate:(id<TKTokenSessionDelegate>)delegate NS_UNAVAILABLE;

- (instancetype)initWithToken:(PteidToken *)token;
@property (readonly) PteidToken *PteidToken;
@property (readonly) BOOL use_auth_key;

@end

#define PTEID_KEY1_LABEL "CC_AUTH_KEY"
#define PTEID_KEY2_LABEL "CC_SIGNATURE_KEY"


@interface PteidToken : TKSmartCardToken<TKTokenDelegate>

- (instancetype)initWithSmartCard:(TKSmartCard *)smartCard AID:(NSData *)AID tokenDriver:(PteidTokenDriver *)tokenDriver error:(NSError **)error;

@end

@interface PteidTokenDriver : TKSmartCardTokenDriver<TKSmartCardTokenDriverDelegate>

@end

NS_ASSUME_NONNULL_END
