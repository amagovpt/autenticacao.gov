/* ****************************************************************************
 *
 * PT eID Middleware Project.
 *
 * Copyright (C) 2021-2022 André Guerreiro - <aguerreiro1985@gmail.com>
*/

NS_ASSUME_NONNULL_BEGIN

@interface PteidTokenSession()

typedef NS_ENUM(NSInteger, PteidAuthState) {
    PteidAuthStateUnauthorized = 0,
    PteidAuthStateFreshlyAuthorized = 1,
    PteidAuthStateAuthorizedButAlreadyUsed = 2,
};

typedef enum {
    PteidHashNone,
    PteidHashSHA1,
    PteidHashSHA256,
    PteidHashSHA384,
    PteidHashSHA512
} PteidHashAlgo;

@property PteidAuthState authState;
@property BOOL isCanceled;


@end

@interface PteidAuthOperation : TKTokenSmartCardPINAuthOperation

- (nullable instancetype)initWithSession:(PteidTokenSession *)session;
@property (readonly) PteidTokenSession *session;

@end


NS_ASSUME_NONNULL_END
