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
#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
 *
 * Definitions
 *
 ******************************************************************************/
#define LOG_LEVEL_INFO        3
#define LOG_LEVEL_SPY         2
#define LOG_LEVEL_WARNING     1
#define LOG_LEVEL_ERROR       0



typedef struct P11_MAP_TYPE 
{ 
 CK_ULONG           ultype;  
 const char         *ctype; 
 unsigned int       logtype; 
} P11_MAP_TYPE; 

#define T_BOOL   1
#define T_UL     2
#define T_TYPE   3
#define T_STRING 4


static P11_MAP_TYPE const P11_CLASS_TYPES[]= 
{  
   { CKO_DATA,              "CKO_DATA"                }, 
   { CKO_CERTIFICATE,       "CKO_CERTIFICATE"         },  
   { CKO_PUBLIC_KEY,        "CKO_PUBLIC_KEY"          },  
   { CKO_PRIVATE_KEY,       "CKO_PRIVATE_KEY"         },  
   { CKO_SECRET_KEY,        "CKO_SECRET_KEY"          },  
   { CKO_HW_FEATURE,        "CKO_HW_FEATURE"          },  
   { CKO_DOMAIN_PARAMETERS, "CKO_DOMAIN_PARAMETERS"   },  
   { CKO_VENDOR_DEFINED,    "CKO_VENDOR_DEFINED"      },
   {0,0}  
};

static P11_MAP_TYPE const P11_CERTIFICATE_TYPES[]= 
{   
   { CKC_X_509,            "CKC_X_509"             },  
   { CKC_X_509_ATTR_CERT,  "CKC_X_509_ATTR_CERT"   },  
   {0,0}
};

static P11_MAP_TYPE const P11_KEY_TYPES[]= 
{  
   { CKK_RSA,        "CKK_RSA"      },  
	{ CKK_DSA,        "CKK_DSA"      },  
	{ CKK_DH,         "CKK_DH"       },  
	{ CKK_ECDSA,      "CKK_ECDSA"    },  
	{ CKK_EC,         "CKK_EC"       },  
	{ CKK_RC2,        "CKK_RC2"      },  
	{ CKK_RC4,        "CKK_RC4"      },  
	{ CKK_RC5,        "CKK_RC5"      },  
	{ CKK_DES,        "CKK_DES"      },  
	{ CKK_DES3,       "CKK_DES3"     },  
	{ CKK_CAST,       "CKK_CAST"     },  
	{ CKK_CAST3,      "CKK_CAST3"    },  
	{ CKK_CAST128,    "CKK_CAST128"  },  
	{ CKK_IDEA,       "CKK_IDEA"     },  
	{ CKK_AES,        "CKK_AES"      },  
	{0,0}
};

static P11_MAP_TYPE const P11_ATTR_TYPES[]= {  
   { CKA_CLASS,            "CKA_CLASS",               T_TYPE       },  
   { CKA_TOKEN,            "CKA_TOKEN",               T_BOOL       },  
   { CKA_PRIVATE,          "CKA_PRIVATE",             T_BOOL       },  
   { CKA_LABEL,            "CKA_LABEL",               T_STRING     },  
	{ CKA_APPLICATION,      "CKA_APPLICATION",         0            },  
	{ CKA_VALUE,            "CKA_VALUE",               0            },  
	{ CKA_OBJECT_ID,        "CKA_OBJECT_ID",           0            },  
   { CKA_CERTIFICATE_TYPE, "CKA_CERTIFICATE_TYPE",    T_TYPE       },  
	{ CKA_ISSUER,           "CKA_ISSUER",              0            },  
	{ CKA_SERIAL_NUMBER,    "CKA_SERIAL_NUMBER",       0            },  
	{ CKA_AC_ISSUER,        "CKA_AC_ISSUER",           0            },  
	{ CKA_OWNER,            "CKA_OWNER",               0            },  
	{ CKA_ATTR_TYPES,       "CKA_ATTR_TYPES",          0            },  
   { CKA_TRUSTED,          "CKA_TRUSTED",             T_BOOL       },  
   { CKA_KEY_TYPE,         "CKA_KEY_TYPE",            T_TYPE       },  
	{ CKA_SUBJECT,          "CKA_SUBJECT",             0            },  
	{ CKA_ID,               "CKA_ID",                  0            },  
	{ CKA_SENSITIVE,        "CKA_SENSITIVE",           T_BOOL       },  
	{ CKA_ENCRYPT,          "CKA_ENCRYPT",             T_BOOL       },  
	{ CKA_DECRYPT,          "CKA_DECRYPT",             T_BOOL       },  
	{ CKA_WRAP,             "CKA_WRAP",                T_BOOL       },  
	{ CKA_UNWRAP,           "CKA_UNWRAP",              T_BOOL       },  
	{ CKA_SIGN,             "CKA_SIGN",                T_BOOL       },  
	{ CKA_SIGN_RECOVER,     "CKA_SIGN_RECOVER",        T_BOOL       },  
	{ CKA_VERIFY,           "CKA_VERIFY",              T_BOOL       },  
	{ CKA_VERIFY_RECOVER,   "CKA_VERIFY_RECOVER",      T_BOOL       },  
	{ CKA_DERIVE,           "CKA_DERIVE",              T_BOOL       },  
	{ CKA_START_DATE,       "CKA_START_DAT",           0            },  
	{ CKA_END_DATE,         "CKA_END_DATE",            0            },  
	{ CKA_MODULUS,          "CKA_MODULUS",             0            },  
   { CKA_MODULUS_BITS,     "CKA_MODULUS_BITS",        T_UL         },  
	{ CKA_PUBLIC_EXPONENT,  "CKA_PUBLIC_EXPONENT",     0            },  
	{ CKA_PRIVATE_EXPONENT, "CKA_PRIVATE_EXPONENT",    0            },  
	{ CKA_PRIME,            "CKA_PRIME",               0            },  
	{ CKA_PRIME,            "CKA_PRIME",               0            },  
	{ CKA_EXPONENT_1,       "CKA_EXPONENT_1",          0            },  
	{ CKA_EXPONENT_2,       "CKA_EXPONENT_2",          0            },  
	{ CKA_COEFFICIENT,      "CKA_COEFFICIENT",         0            },  
	{ CKA_PRIME,            "CKA_PRIME",               0            },  
	{ CKA_SUBPRIME,         "CKA_SUBPRIME",            0            },  
	{ CKA_BASE,             "CKA_BASE",                0            },  
	{ CKA_PRIME_BITS,       "CKA_PRIME_BITS",          0            },  
	{ CKA_SUB_PRIME_BITS,   "CKA_SUB_PRIME_BITS",      0            },  
	{ CKA_VALUE_BITS,       "CKA_VALUE_BITS",          0            },  
	{ CKA_VALUE_LEN,        "CKA_VALUE_LEN",           0            },  
   { CKA_EXTRACTABLE,      "CKA_EXTRACTABLE",         T_BOOL       },  
   { CKA_LOCAL,            "CKA_LOCAL",               T_BOOL       },  
   { CKA_NEVER_EXTRACTABLE, "CKA_NEVER_EXTRACTABLE",  T_BOOL       },  
   { CKA_ALWAYS_SENSITIVE, "CKA_ALWAYS_SENSITIVE",    T_BOOL       },  
	{ CKA_KEY_GEN_MECHANISM, "CKA_KEY_GEN_MECHANISM",  0            },  
   { CKA_MODIFIABLE,       "CKA_MODIFIABLE",          T_BOOL       },  
	{ CKA_ECDSA_PARAMS,     "CKA_ECDSA_PARAMS",        0            },  
	{ CKA_EC_PARAMS,        "CKA_EC_PARAMS",           0            },  
	{ CKA_EC_POINT,         "CKA_EC_POINT",            0            },  
	{ CKA_SECONDARY_AUTH,   "CKA_SECONDARY_AUTH",      0            },  
   { CKA_AUTH_PIN_FLAGS,   "CKA_AUTH_PIN_FLAGS",      T_UL         },  
	{ CKA_HW_FEATURE_TYPE,  "CKA_HW_FEATURE_TYPE",     0            },  
	{ CKA_RESET_ON_INIT,    "CKA_RESET_ON_INIT",       0            },  
	{ CKA_HAS_RESET,        "CKA_HAS_RESET",           0            },  
	{ CKA_VENDOR_DEFINED,   "CKA_VENDOR_DEFINED",      0            },  
	{0,0}  
};




/******************************************************************************
 *
 * Prototypes
 *
 ******************************************************************************/
extern void log_init(char *pszLogFile, unsigned int uiLogLevel);
extern void log_trace(const char *where, const char *string,... );
extern void log_xtrace(const char *where, char *string,void *data,int len);
void _log_xtrace(char *text, void *data, int l_data);

void log_attr(CK_ATTRIBUTE_PTR pAttr);
void log_template(const char *string, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG count);
char* log_map_error(int err);

#ifdef __cplusplus
   }
#endif


#endif

