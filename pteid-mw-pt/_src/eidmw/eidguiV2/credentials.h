/*
    The credentials for the CMD service should be in this file.
    To be able to build with support for CMD, please contact info.portaldocidadao@ama.pt
*/

#ifndef CREDENTIALS_H
#define CREDENTIALS_H

/* CMD */
#define CMD_SUPPORT 0                   // CHANGE TO 1 TO SUPPORT CMD
#if CMD_SUPPORT

    #define CMD_BASIC_AUTH_APPID        // INSERT CMD APP ID HERE  
    #define CMD_BASIC_AUTH_USERID       // INSERT CMD USER ID HERE 
    #define CMD_BASIC_AUTH_PASSWORD     // INSERT CMD PASSWORD HERE

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