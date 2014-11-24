#include "OpenCLBase.h"

OpenCLBase::OpenCLBase(void) {
}

OpenCLBase::~OpenCLBase(void) {
}


/**
 *
 */
void OpenCLBase::ErrorCheck(char* functionName, cl_int errorCode) {
	if(errorCode != CL_SUCCESS) {
		cout << functionName << "() Failed: result = " << errorCode << endl;
		exit(1);
	}
}

/**
 *
 */
cl_platform_id* OpenCLBase::GetPlatformIds(cl_uint &num_platforms) {
	cl_platform_id* platforms;
	cl_int errorCode;

	errorCode = clGetPlatformIDs(0, NULL, &num_platforms);
	ErrorCheck("clGetPlatformIDs", errorCode);

	// Malloc for platforms object
	platforms = reinterpret_cast<cl_platform_id*>(malloc(sizeof(cl_platform_id) * num_platforms));
	if(platforms == NULL) {
		cout << "Malloc Failed." << endl;
		return NULL;
	}

	// 
	errorCode = clGetPlatformIDs(num_platforms, platforms, NULL);
	ErrorCheck("clGetPlatformIDs", errorCode);

	return platforms;
}

/**
 *
 */
void OpenCLBase::GetPlatformInfo(cl_platform_id id) {
	// 
	size_t length;
	cl_int errorCode;

	errorCode = clGetPlatformInfo(id, CL_PLATFORM_PROFILE, 0, NULL, &length);
	ErrorCheck("clGetPlatformInfo", errorCode);

	char* param = reinterpret_cast<char*>(malloc(length * sizeof(char)));
	errorCode = clGetPlatformInfo(id, CL_PLATFORM_PROFILE, length, param, NULL);
	ErrorCheck("clGetPlatformInfo", errorCode);
		
	cout << param << endl;

	free(param);
}

/**
 *
 */
cl_device_id* OpenCLBase::GetDeviceIds(cl_platform_id id, cl_device_type type, cl_uint &num_devices) {
	cl_device_id* devices;
	cl_int errorCode;

	// 
	errorCode = clGetDeviceIDs(id, type, 0, NULL, &num_devices);
	ErrorCheck("clGetDeviceIDs", errorCode);

	devices = reinterpret_cast<cl_device_id*>(malloc(sizeof(cl_device_id) * num_devices));
	if(devices == NULL) {
		cout << "Malloc Failed." << endl;
		exit(1);
	}

	errorCode = clGetDeviceIDs(id, type, num_devices, devices, 0);
	if(errorCode != CL_SUCCESS) {
		cout << "clGetDeviceIDs() faild: result = " << errorCode << endl;
		free(devices);
		exit(1);
	}

	return devices;
}

/**
 *
 */
void OpenCLBase::GetDeviceInfo(cl_device_id id) {
	size_t length;
	cl_int errorCode;
	errorCode = clGetDeviceInfo(id, CL_DEVICE_TYPE, 0, NULL, &length);
	ErrorCheck("clGetDeviceInfo", errorCode);

	cl_device_type* param = reinterpret_cast<cl_device_type*>(malloc(sizeof(cl_device_type)*length));
	errorCode = clGetDeviceInfo(id, CL_DEVICE_TYPE, length, param, NULL);
	ErrorCheck("clGetDeviceInfo", errorCode);

	if(*param & CL_DEVICE_TYPE_CPU) {
		cout << id << ": CPU" << endl;
	}
	else if(*param & CL_DEVICE_TYPE_GPU) {
		cout << id << ": GPU" << endl;
	}

	free(param);
}