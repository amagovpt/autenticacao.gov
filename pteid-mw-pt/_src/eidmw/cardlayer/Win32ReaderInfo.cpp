#include <windows.h>

#include <setupapi.h>
#include <initguid.h>

#include <cstdio>
#include <vector>
#include <string>
#include <regex>

#include "Log.h"
#include "ReaderDeviceInfo.h"

	// This is the GUID for the USB device class
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE,
		0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);
	// (A5DCBF10-6530-11D2-901F-00C04FB951ED)

namespace eIDMW {

	void parseHardwareIDs(char *device_hardwareID, int * vendorID, int *productID) {
		std::cmatch cm;
		std::regex expression("VID_([A-F0-9]+)&PID_([A-F0-9]+)");

		if (!std::regex_search(device_hardwareID, cm, expression)) {
			MWLOG(LEV_DEBUG, MOD_CAL, "Failed to match VID and PID in hardwareID string %s", device_hardwareID);
			return;
		}
		for (unsigned i = 0; i < cm.size(); i++) {
			switch (i)
			{
				//case 0 is ignored as it matches the full regex pattern
			case 1:
				//Convert to integer assuming hexadecimal string
				*vendorID = std::stoi(cm[i].str(), nullptr, 16);
				break;
			case 2:
				*productID = std::stoi(cm[i].str(), nullptr, 16);
				break;
			}
		}
	}

	void readSingleDevicePropertyString(HDEVINFO hDevInfo, SP_DEVINFO_DATA *devData, DWORD propertyName, std::string *destination_str) {
		BOOL ret = 0;
		DWORD requiredSize = 0;
		DWORD regDataType = 0;
		char property_buffer[256] = { 0 };

		const char * buf = &property_buffer[0];
		//First call to get the required size for the string buffer
		ret = SetupDiGetDeviceRegistryPropertyA(hDevInfo, devData, propertyName, &regDataType,(PBYTE) buf, sizeof(property_buffer), &requiredSize);
		//Check the expected error code
		if (!ret && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			buf = (char *)calloc(requiredSize, sizeof(char));
			ret = SetupDiGetDeviceRegistryPropertyA(hDevInfo, devData, propertyName, &regDataType, (PBYTE)buf, requiredSize, &requiredSize);
		}
		if (!ret) {
			MWLOG(LEV_WARN, MOD_CAL, "SetupDiGetDeviceRegistryProperty failed!: Error code: %d", GetLastError());
		}
		else {
			if (regDataType != REG_SZ) {
				MWLOG(LEV_WARN, MOD_CAL, "USB device property data type is not string! regDataType: %d", regDataType);
			}
			else {
				destination_str->append(buf);
			}
		}
			
	}

	/* Enumerate connected USB devices and their Vendor and Product IDs */

	std::vector<ReaderDeviceInfo> win32ReaderDevices() {
		HDEVINFO                         hDevInfo;
		SP_DEVINFO_DATA                  DevData;
		const std::string CARD_READER_DEV_CLASS = "SmartCardReader";

		char dev_property_buffer[128] = { 0 };

		std::vector<ReaderDeviceInfo> readers;
		std::string manufacturer, name;
		int vendorID = 0, productID = 0;

		DWORD dwMemberIdx;

		// We will try to get device information set for all USB devices that have a
		// device interface and are currently present on the system (plugged in).
		hDevInfo = SetupDiGetClassDevs(
			&GUID_DEVINTERFACE_USB_DEVICE, NULL, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

		if (hDevInfo != INVALID_HANDLE_VALUE)
		{

			dwMemberIdx = 0;

			DevData.cbSize = sizeof(DevData);
			//Start enumerating USB devices
			SetupDiEnumDeviceInfo(hDevInfo, dwMemberIdx, &DevData);

			while (GetLastError() != ERROR_NO_MORE_ITEMS)
			{

				BOOL ret = SetupDiGetDeviceRegistryPropertyA(hDevInfo, &DevData, SPDRP_CLASS, NULL, (PBYTE)dev_property_buffer, sizeof(dev_property_buffer), NULL);
				if (!ret) {
					MWLOG(LEV_DEBUG, MOD_CAL, "SetupDiGetDeviceRegistryProperty failed to get device class! Error code: %d\n", GetLastError());
				}
				std::string device_class(dev_property_buffer);
				if (device_class == CARD_READER_DEV_CLASS) {
					ReaderDeviceInfo reader_devinfo;
					memset(dev_property_buffer, 0, sizeof(dev_property_buffer));
					//Get device name, manufacturer and hardware IDs
					readSingleDevicePropertyString(hDevInfo, &DevData, SPDRP_DEVICEDESC, &reader_devinfo.name);
					readSingleDevicePropertyString(hDevInfo, &DevData, SPDRP_MFG, &reader_devinfo.manufacturer);
					readSingleDevicePropertyString(hDevInfo, &DevData, SPDRP_SERVICE, &reader_devinfo.driver);
					ret = SetupDiGetDeviceRegistryPropertyA(hDevInfo, &DevData, SPDRP_HARDWAREID, NULL, (PBYTE)dev_property_buffer, sizeof(dev_property_buffer), NULL);
					if (ret) {
						//SPDRP_HARDWAREID property is a multi-string but the first one should be enough for us...
						parseHardwareIDs(dev_property_buffer, &reader_devinfo.vendorID, &reader_devinfo.productID);
					}
					readers.push_back(reader_devinfo);
				}
				memset(dev_property_buffer, 0, sizeof(dev_property_buffer));

				// Continue looping
				SetupDiEnumDeviceInfo(hDevInfo, ++dwMemberIdx, &DevData);
			}

			SetupDiDestroyDeviceInfoList(hDevInfo);
		}

		return readers;
	}

}