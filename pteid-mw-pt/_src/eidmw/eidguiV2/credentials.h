/*
    The credentials for the CMD service should be in this file.
    To be able to build with support for CMD, please contact info.portaldocidadao@ama.pt
*/

#ifndef CREDENTIALS_H
#define CREDENTIALS_H

/* CMD */
#define CMD_SUPPORT 1                   // CHANGE TO 1 TO SUPPORT CMD
#if CMD_SUPPORT

    #define CMD_BASIC_AUTH_APPID        "2192354e-4b1f-4401-9631-d5b2bdd7e4c8"  
    #define CMD_BASIC_AUTH_USERID       "tx3hmWas" 
    #define CMD_BASIC_AUTH_PASSWORD     "4pIXikQzfxrQt73wLiO2"

#else

    #define CMD_BASIC_AUTH_USERID       "" 
    #define CMD_BASIC_AUTH_PASSWORD     "" 
    #define CMD_BASIC_AUTH_APPID        "" 
    #pragma message ("\n\n\
#############################################################################\n\
##                              WARNING                                    ##\n\
#############################################################################\n\
Esta build não possui suporte para CMD. \
Para mais informações contacte info.portaldocidadao@ama.pt.\
\n\n")

#endif
 
#endif