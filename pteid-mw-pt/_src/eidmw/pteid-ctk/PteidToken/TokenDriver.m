/* ****************************************************************************
 *
 * eID Middleware Project.
 *
 * Copyright (C) 2021-2022 André Guerreiro - <aguerreiro1985@gmail.com>
*/

#import "Token.h"

@implementation PteidTokenDriver

- (TKSmartCardToken *)tokenDriver:(TKSmartCardTokenDriver *)driver createTokenForSmartCard:(TKSmartCard *)smartCard AID:(NSData *)AID error:(NSError **)error {
    return [[PteidToken alloc] initWithSmartCard:smartCard AID:AID tokenDriver:self error:error];
    //TODO: Validate our ATRs
}

@end
