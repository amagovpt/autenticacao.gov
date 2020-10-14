/*-****************************************************************************

 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2011-2014, 2016-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2018-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef __PTEID_VERSION_H__
#define __PTEID_VERSION_H__

/**
 * Versions for the Windows binaries
 * This versions are the real version of the middleware
 */

#pragma once
#include "../release_data.h"

// To specified in the .rc files

#define PTEID_COMPANY_NAME    	"Portuguese Government"
#define PTEID_PRODUCT_NAME    	"Portugal eID MiddleWare"

// Common Lib
#define WIN_CL_VERSION_STRING    BASE_VERSION_STRING REVISION_NUM_STRING
#define WIN_CL_VERSION1          BASE_VERSION1          
#define WIN_CL_VERSION2          BASE_VERSION2          
#define WIN_CL_VERSION3          BASE_VERSION3          
#define WIN_CL_VERSION4          REVISION_NUM

// Card Abstraction
#define WIN_CAL_VERSION_STRING   BASE_VERSION_STRING REVISION_NUM_STRING
#define WIN_CAL_VERSION1         BASE_VERSION1          
#define WIN_CAL_VERSION2         BASE_VERSION2          
#define WIN_CAL_VERSION3         BASE_VERSION3          
#define WIN_CAL_VERSION4         REVISION_NUM

// Dialogs
#define WIN_DLG_VERSION_STRING   BASE_VERSION_STRING REVISION_NUM_STRING
#define WIN_DLG_VERSION1         BASE_VERSION1          
#define WIN_DLG_VERSION2         BASE_VERSION2          
#define WIN_DLG_VERSION3         BASE_VERSION3          
#define WIN_DLG_VERSION4         REVISION_NUM

// CSP
#define WIN_CSP_VERSION_STRING    BASE_VERSION_STRING REVISION_NUM_STRING
#define WIN_CSP_VERSION1          BASE_VERSION1          
#define WIN_CSP_VERSION2          BASE_VERSION2          
#define WIN_CSP_VERSION3          BASE_VERSION3          
#define WIN_CSP_VERSION4          REVISION_NUM

// CSP
#define WIN_CSPLIB_VERSION_STRING    BASE_VERSION_STRING REVISION_NUM_STRING
#define WIN_CSPLIB_VERSION1          BASE_VERSION1          
#define WIN_CSPLIB_VERSION2          BASE_VERSION2          
#define WIN_CSPLIB_VERSION3          BASE_VERSION3          
#define WIN_CSPLIB_VERSION4          REVISION_NUM

// Application layer
#define WIN_APL_VERSION_STRING    BASE_VERSION_STRING REVISION_NUM_STRING
#define WIN_APL_VERSION1          BASE_VERSION1          
#define WIN_APL_VERSION2          BASE_VERSION2          
#define WIN_APL_VERSION3          BASE_VERSION3          
#define WIN_APL_VERSION4          REVISION_NUM

// SDK
#define WIN_SDK_VERSION_STRING    BASE_VERSION_STRING REVISION_NUM_STRING
#define WIN_SDK_VERSION1          BASE_VERSION1          
#define WIN_SDK_VERSION2          BASE_VERSION2          
#define WIN_SDK_VERSION3          BASE_VERSION3          
#define WIN_SDK_VERSION4          REVISION_NUM

// SDK_C
#define WIN_SDK_C_VERSION_STRING    BASE_VERSION_STRING REVISION_NUM_STRING
#define WIN_SDK_C_VERSION1          BASE_VERSION1          
#define WIN_SDK_C_VERSION2          BASE_VERSION2          
#define WIN_SDK_C_VERSION3          BASE_VERSION3          
#define WIN_SDK_C_VERSION4          REVISION_NUM

// SDK_CS
#define WIN_SDK_CS_VERSION_STRING    BASE_VERSION_STRING REVISION_NUM_STRING
#define WIN_SDK_CS_VERSION1          BASE_VERSION1          
#define WIN_SDK_CS_VERSION2          BASE_VERSION2          
#define WIN_SDK_CS_VERSION3          BASE_VERSION3          
#define WIN_SDK_CS_VERSION4          REVISION_NUM

// SDK_JAVA
#define WIN_SDK_JAVA_VERSION_STRING    BASE_VERSION_STRING REVISION_NUM_STRING
#define WIN_SDK_JAVA_VERSION1          BASE_VERSION1          
#define WIN_SDK_JAVA_VERSION2          BASE_VERSION2          
#define WIN_SDK_JAVA_VERSION3          BASE_VERSION3          
#define WIN_SDK_JAVA_VERSION4          REVISION_NUM

// GUI
#define WIN_GUI_VERSION_STRING    BASE_VERSION_STRING REVISION_NUM_STRING
#define WIN_GUI_VERSION1          BASE_VERSION1          
#define WIN_GUI_VERSION2          BASE_VERSION2          
#define WIN_GUI_VERSION3          BASE_VERSION3          
#define WIN_GUI_VERSION4          REVISION_NUM

// PKCS11
#define WIN_PKCS11_VERSION_STRING    BASE_VERSION_STRING REVISION_NUM_STRING
#define WIN_PKCS11_VERSION1          BASE_VERSION1          
#define WIN_PKCS11_VERSION2          BASE_VERSION2          
#define WIN_PKCS11_VERSION3          BASE_VERSION3          
#define WIN_PKCS11_VERSION4          REVISION_NUM

#endif //__PTEID_VERSION_H__
