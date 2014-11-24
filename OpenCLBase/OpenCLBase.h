#pragma once

#include <iostream>
#include <fstream>
using namespace std;

#include <CL\cl.h>

class OpenCLBase {
public:
	OpenCLBase(void);
	~OpenCLBase(void);

	void ErrorCheck(char* functionName, cl_int errorCode);

	// for platform info
	cl_platform_id* GetPlatformIds(cl_uint &num_platforms);
	void GetPlatformInfo(cl_platform_id id);
	// for device info
	cl_device_id* GetDeviceIds(cl_platform_id id, cl_device_type type, cl_uint &num_devices);
	void GetDeviceInfo(cl_device_id id);

	virtual int run(void) = 0;
};
