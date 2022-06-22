#pragma once

#include <cstdlib>
#include <vector>
#include <string>

namespace eIDMW {

	struct ReaderDeviceInfo {
	public:
		
		int vendorID;
		int productID;
		std::string manufacturer;
		std::string name;
		std::string driver;
	};

#ifdef WIN32
	//Implemented in Win32ReaderInfo.cpp
	std::vector<ReaderDeviceInfo> win32ReaderDevices();
#endif

}