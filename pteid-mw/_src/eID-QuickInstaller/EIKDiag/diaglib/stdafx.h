
#pragma once

#undef UNICODE
#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0500	// Change this to the appropriate value to target other versions of Windows.
#endif	
#include <windows.h>

// TODO: reference additional headers your program requires here

#include <iostream>
#include <tchar.h>

#include <string>
#include <vector>
using namespace std;

#include "autovec.h"
using namespace sc;


