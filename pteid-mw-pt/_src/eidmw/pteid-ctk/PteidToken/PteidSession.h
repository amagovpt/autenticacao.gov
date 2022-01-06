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

@property PteidAuthState authState;

@end

@interface PteidAuthOperation : TKTokenSmartCardPINAuthOperation

- (nullable instancetype)initWithSession:(PteidTokenSession *)session;
@property (readonly) PteidTokenSession *session;

@end


NS_ASSUME_NONNULL_END
