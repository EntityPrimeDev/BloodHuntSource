#include "kdmapper.hpp"
#include "driver.h"

int main()
{
	HANDLE iqvw64e_device_handle = intel_driver::Load();

	if (!iqvw64e_device_handle || iqvw64e_device_handle == INVALID_HANDLE_VALUE)
	{
		std::cout << "[-] Couldn't Load Driver iqvw64e.sys" << std::endl;
		return -1;
	}

	if (!kdmapper::MapDriver(iqvw64e_device_handle, RawData))
	{
		std::cout << "[-] Failder To Map Driver" << std::endl;
		intel_driver::Unload(iqvw64e_device_handle);
		return -1;
	}

	intel_driver::Unload(iqvw64e_device_handle);
}