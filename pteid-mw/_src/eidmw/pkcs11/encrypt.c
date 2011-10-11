/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#include <stdlib.h>
#include <string.h>
#include "pteid_p11.h"
#include "log.h"
#include "util.h"



#define WHERE "C_EncryptInit()"
CK_RV C_EncryptInit(CK_SESSION_HANDLE hSession,    /* the session's handle */
                    CK_MECHANISM_PTR  pMechanism,  /* the encryption mechanism */
                    CK_OBJECT_HANDLE  hKey)        /* handle of encryption key */
{
log_trace(WHERE, "S: C_EncryptInit(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE



#define WHERE "C_Encrypt()"
CK_RV C_Encrypt(CK_SESSION_HANDLE hSession,            /* the session's handle */
                CK_BYTE_PTR       pData,               /* the plaintext data */
                CK_ULONG          ulDataLen,           /* bytes of plaintext data */
                CK_BYTE_PTR       pEncryptedData,      /* receives encrypted data */
                CK_ULONG_PTR      pulEncryptedDataLen) /* receives encrypted byte count */
{
log_trace(WHERE, "S: C_Encrypt(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE



#define WHERE "C_EncryptUpdate()"
CK_RV C_EncryptUpdate(CK_SESSION_HANDLE hSession,           /* the session's handle */
                      CK_BYTE_PTR       pPart,              /* the plaintext data */
                      CK_ULONG          ulPartLen,          /* bytes of plaintext data */
                      CK_BYTE_PTR       pEncryptedPart,     /* receives encrypted data */
                      CK_ULONG_PTR      pulEncryptedPartLen)/* receives encrypted byte count */
{
log_trace(WHERE, "S: C_EncryptUpdate(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE




#define WHERE "C_EncryptFinal()"
CK_RV C_EncryptFinal(CK_SESSION_HANDLE hSession,                /* the session's handle */
                     CK_BYTE_PTR       pLastEncryptedPart,      /* receives encrypted last part */
                     CK_ULONG_PTR      pulLastEncryptedPartLen) /* receives byte count */
{
log_trace(WHERE, "S: C_EncryptFinal(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE 




#define WHERE "C_DecryptInit()"
CK_RV C_DecryptInit(CK_SESSION_HANDLE hSession,    /* the session's handle */
                    CK_MECHANISM_PTR  pMechanism,  /* the decryption mechanism */
                    CK_OBJECT_HANDLE  hKey)        /* handle of the decryption key */
{
log_trace(WHERE, "S: C_DecryptInit(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE 




#define WHERE "C_Decrypt()"
CK_RV C_Decrypt(CK_SESSION_HANDLE hSession,           /* the session's handle */
                CK_BYTE_PTR       pEncryptedData,     /* input encrypted data */
                CK_ULONG          ulEncryptedDataLen, /* count of bytes of input */
                CK_BYTE_PTR       pData,              /* receives decrypted output */
                CK_ULONG_PTR      pulDataLen)         /* receives decrypted byte count */
{
log_trace(WHERE, "S: C_Decrypt(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE 



#define WHERE "C_DecryptUpdate()"
CK_RV C_DecryptUpdate(CK_SESSION_HANDLE hSession,            /* the session's handle */
                      CK_BYTE_PTR       pEncryptedPart,      /* input encrypted data */
                      CK_ULONG          ulEncryptedPartLen,  /* count of bytes of input */
                      CK_BYTE_PTR       pPart,               /* receives decrypted output */
                      CK_ULONG_PTR      pulPartLen)          /* receives decrypted byte count */
{
log_trace(WHERE, "S: C_DecryptUpdate(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE 



#define WHERE "C_DecryptFinal()"
CK_RV C_DecryptFinal(CK_SESSION_HANDLE hSession,       /* the session's handle */
                     CK_BYTE_PTR       pLastPart,      /* receives decrypted output */
                     CK_ULONG_PTR      pulLastPartLen)  /* receives decrypted byte count */
{
log_trace(WHERE, "S: C_DecryptFinal(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE



#define WHERE "C_DigestEncryptUpdate()"
CK_RV C_DigestEncryptUpdate(CK_SESSION_HANDLE hSession,            /* the session's handle */
                            CK_BYTE_PTR       pPart,               /* the plaintext data */
                            CK_ULONG          ulPartLen,           /* bytes of plaintext data */
                            CK_BYTE_PTR       pEncryptedPart,      /* receives encrypted data */
                            CK_ULONG_PTR      pulEncryptedPartLen) /* receives encrypted byte count */
{
log_trace(WHERE, "S: C_DigestEncryptUpdate(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE 



#define WHERE "C_DecryptDigestUpdate()"
CK_RV C_DecryptDigestUpdate(CK_SESSION_HANDLE hSession,            /* the session's handle */
                            CK_BYTE_PTR       pEncryptedPart,      /* input encrypted data */
                            CK_ULONG          ulEncryptedPartLen,  /* count of bytes of input */
                            CK_BYTE_PTR       pPart,               /* receives decrypted output */
                            CK_ULONG_PTR      pulPartLen)          /* receives decrypted byte count */
{
log_trace(WHERE, "S: C_DecryptDigestUpdate(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE 


#define WHERE "C_SignEncryptUpdate()" 
CK_RV C_SignEncryptUpdate(CK_SESSION_HANDLE hSession,            /* the session's handle */
                          CK_BYTE_PTR       pPart,               /* the plaintext data */
                          CK_ULONG          ulPartLen,           /* bytes of plaintext data */
                          CK_BYTE_PTR       pEncryptedPart,      /* receives encrypted data */
                          CK_ULONG_PTR      pulEncryptedPartLen) /* receives encrypted byte count */
{
log_trace(WHERE, "S: C_SignEncryptUpdate(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE




#define WHERE "C_DecryptVerifyUpdate()"
CK_RV C_DecryptVerifyUpdate(CK_SESSION_HANDLE hSession,            /* the session's handle */
                            CK_BYTE_PTR       pEncryptedPart,      /* input encrypted data */
                            CK_ULONG          ulEncryptedPartLen,  /* count of byes of input */
                            CK_BYTE_PTR       pPart,               /* receives decrypted output */
                            CK_ULONG_PTR      pulPartLen)          /* receives decrypted byte count */
{
log_trace(WHERE, "S: C_DecryptVerifyUpdate(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE 




#define WHERE "C_GenerateKey()"
CK_RV C_GenerateKey(CK_SESSION_HANDLE    hSession,    /* the session's handle */
                    CK_MECHANISM_PTR     pMechanism,  /* the key generation mechanism */
                    CK_ATTRIBUTE_PTR     pTemplate,   /* template for the new key */
                    CK_ULONG             ulCount,     /* number of attributes in template */
                    CK_OBJECT_HANDLE_PTR phKey)       /* receives handle of new key */
{
log_trace(WHERE, "S: C_GenerateKey(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE



#define WHERE "C_GenerateKeyPair()"
CK_RV C_GenerateKeyPair(CK_SESSION_HANDLE    hSession,                    /* the session's handle */
                        CK_MECHANISM_PTR     pMechanism,                  /* the key gen. mech. */
                        CK_ATTRIBUTE_PTR     pPublicKeyTemplate,          /* pub. attr. template */
                        CK_ULONG             ulPublicKeyAttributeCount,   /* # of pub. attrs. */
                        CK_ATTRIBUTE_PTR     pPrivateKeyTemplate,         /* priv. attr. template */
                        CK_ULONG             ulPrivateKeyAttributeCount,  /* # of priv. attrs. */
                        CK_OBJECT_HANDLE_PTR phPublicKey,                 /* gets pub. key handle */
                        CK_OBJECT_HANDLE_PTR phPrivateKey)                /* gets priv. key handle */
{
log_trace(WHERE, "S: C_GenerateKeyPair(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE


#define WHERE "C_WrapKey()"
CK_RV C_WrapKey(CK_SESSION_HANDLE hSession,        /* the session's handle */
                CK_MECHANISM_PTR  pMechanism,      /* the wrapping mechanism */
                CK_OBJECT_HANDLE  hWrappingKey,    /* handle of the wrapping key */
                CK_OBJECT_HANDLE  hKey,            /* handle of the key to be wrapped */
                CK_BYTE_PTR       pWrappedKey,     /* receives the wrapped key */
                CK_ULONG_PTR      pulWrappedKeyLen)/* receives byte size of wrapped key */
{
log_trace(WHERE, "S: C_WrapKey()");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE 


#define WHERE "C_UnwrapKey()"
CK_RV C_UnwrapKey(CK_SESSION_HANDLE    hSession,          /* the session's handle */
                  CK_MECHANISM_PTR     pMechanism,        /* the unwrapping mechanism */
                  CK_OBJECT_HANDLE     hUnwrappingKey,    /* handle of the unwrapping key */
                  CK_BYTE_PTR          pWrappedKey,       /* the wrapped key */
                  CK_ULONG             ulWrappedKeyLen,   /* bytes length of wrapped key */
                  CK_ATTRIBUTE_PTR     pTemplate,         /* template for the new key */
                  CK_ULONG             ulAttributeCount,  /* # of attributes in template */
                  CK_OBJECT_HANDLE_PTR phKey)             /* gets handle of recovered key */
{
log_trace(WHERE, "S: C_UnwrapKey()");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE




#define WHERE "C_DeriveKey()"
CK_RV C_DeriveKey(CK_SESSION_HANDLE    hSession,          /* the session's handle */
                  CK_MECHANISM_PTR     pMechanism,        /* the key derivation mechanism */
                  CK_OBJECT_HANDLE     hBaseKey,          /* handle of the base key */
                  CK_ATTRIBUTE_PTR     pTemplate,         /* template for the new key */
                  CK_ULONG             ulAttributeCount,  /* # of attributes in template */
                  CK_OBJECT_HANDLE_PTR phKey)             /* gets handle of derived key */
{
log_trace(WHERE, "S: C_DeriveKey()");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE 



#define WHERE "C_SeedRandom()"
CK_RV C_SeedRandom(CK_SESSION_HANDLE hSession,  /* the session's handle */
                   CK_BYTE_PTR       pSeed,     /* the seed material */
                   CK_ULONG          ulSeedLen) /* count of bytes of seed material */
{
log_trace(WHERE, "S: C_SeedRandom()");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE



#define WHERE "C_GenerateRandom()"
CK_RV C_GenerateRandom(CK_SESSION_HANDLE hSession,    /* the session's handle */
                       CK_BYTE_PTR       RandomData,  /* receives the random data */
                       CK_ULONG          ulRandomLen) /* number of bytes to be generated */
{
log_trace(WHERE, "S: C_GenerateRandom()");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE 



#define WHERE "C_GetFunctionStatus()"
CK_RV C_GetFunctionStatus(CK_SESSION_HANDLE hSession) /* the session's handle */
{
log_trace(WHERE, "S: C_GetFunctionStatus()");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE 


#define WHERE "C_CancelFunction()"
CK_RV C_CancelFunction(CK_SESSION_HANDLE hSession) /* the session's handle */
{
log_trace(WHERE, "S: C_CancelFunction()");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE



