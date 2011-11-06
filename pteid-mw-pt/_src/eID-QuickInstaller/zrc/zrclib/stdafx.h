

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef WIN32
#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0500	// Change this to the appropriate value to target other versions of Windows.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#else
#include <stdint.h>
typedef uint32_t UINT32;
typedef int32_t  INT32;
#endif

#include <ios>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#include "autovec.h"
using namespace sc;

// TODO: reference additional headers your program requires here
