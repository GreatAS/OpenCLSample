#include "OpenCLSample.h"

OpenCLSample::OpenCLSample(void) {}
OpenCLSample::~OpenCLSample(void){}

void errorCheck(char* functionName, cl_int ret) {
	if(ret != CL_SUCCESS) {
		cout << functionName << "() faild: result = " << ret << endl;
		exit(1);
	}
}

cl_platform_id* getPlatformIds(cl_uint &num_platforms) {
	// ƒvƒ‰ƒbƒgƒtƒH[ƒ€‚Ì”‚ðŠm”F‚·‚é
	cl_platform_id* platforms;
	//cl_uint num_platforms;

	cl_int result = clGetPlatformIDs(0, NULL, &num_platforms);
	errorCheck("clGetPlatformIDs", result);

	// Malloc for platforms object
	platforms = reinterpret_cast<cl_platform_id*>(malloc(sizeof(cl_platform_id) * num_platforms));
	if(platforms == NULL) {
		cout << "Malloc Failed." << endl;
		return NULL;
	}

	// ƒvƒ‰ƒbƒgƒtƒH[ƒ€‚ÌID‚ðŠm”F‚·‚é
	result = clGetPlatformIDs(num_platforms, platforms, NULL);
	errorCheck("clGetPlatformIDs", result);

	return platforms;
}

// ƒvƒ‰ƒbƒgƒtƒH[ƒ€‚Ìî•ñ‚ðŠm”F‚·‚é
void getPlatformInfo(cl_platform_id id) {
	// 1‰ñ–Ú‚ÌŒÄ‚Ño‚µ‚Å•Ô‚è’l‚Ì’·‚³‚ðA2‰ñ–Ú‚ÌŒÄ‚Ño‚µ‚ÅŽÀÛ‚Ì’†g‚ðŽæ“¾‚Å‚«‚é
	size_t length;
	cl_int result = clGetPlatformInfo(id, CL_PLATFORM_PROFILE, 0, NULL, &length);
	errorCheck("clGetPlatformInfo", result);

	char* param = reinterpret_cast<char*>(malloc(length * sizeof(char)));
	result = clGetPlatformInfo(id, CL_PLATFORM_PROFILE, length, param, NULL);
	errorCheck("clGetPlatformInfo", result);
		
	cout << param << endl;

	free(param);
}

cl_device_id* getDeviceIds(cl_platform_id id, cl_device_type type, cl_uint &num_devices) {
	cl_device_id* devices;

	// device_idƒIƒuƒWƒFƒNƒg—p‚Ì—Ìˆæ‚ðŠm•Û
	cl_int result = clGetDeviceIDs(id, type, 0, NULL, &num_devices);
	errorCheck("clGetDeviceIDs", result);

	devices = reinterpret_cast<cl_device_id*>(malloc(sizeof(cl_device_id) * num_devices));
	if(devices == NULL) {
		cout << "Malloc Failed." << endl;
		exit(1);
	}

	result = clGetDeviceIDs(id, type, num_devices, devices, 0);
	if(result != CL_SUCCESS) {
		cout << "clGetDeviceIDs() faild: result = " << result << endl;
		free(devices);
		exit(1);
	}

	return devices;
}

void getDeviceInfo(cl_device_id id) {
	size_t length;
	cl_int result = clGetDeviceInfo(id, CL_DEVICE_TYPE, 0, NULL, &length);
	errorCheck("clGetDeviceInfo", result);

	cl_device_type* param = reinterpret_cast<cl_device_type*>(malloc(sizeof(cl_device_type)*length));
	result = clGetDeviceInfo(id, CL_DEVICE_TYPE, length, param, NULL);
	errorCheck("clGetDeviceInfo", result);

	if(*param & CL_DEVICE_TYPE_CPU) {
		cout << id << ": CPU" << endl;
	}
	else if(*param & CL_DEVICE_TYPE_GPU) {
		cout << id << ": GPU" << endl;
	}

	free(param);
}

/**
 * see here: http://the-united-front.blogspot.jp/2014/05/gpgpu_15.html
 * see also here: http://seesaawiki.jp/w/mikk_ni3_92/d/OpenCL%CA%D402_2
 */

int main(int argc, char* argv[]) {

	/*****************************************************************************
	 * 0. Get Device Info 
	 ****************************************************************************/
	// Platform
	cl_platform_id* platforms;
	cl_uint num_platforms;
	// Device
	cl_device_id* device_ids;
	cl_uint num_devices;
	// device_ids = reinterpret_cast<cl_device_id*>(malloc(sizeof(cl_device_id)));	

	platforms = getPlatformIds(num_platforms);
	cout << "Number of Platforms: " << num_platforms << endl;

	// ƒvƒ‰ƒbƒgƒtƒH[ƒ€‚Ì”‚ª‚í‚©‚Á‚½‚Ì‚ÅA‚»‚Ì”‚¾‚¯ƒ‹[ƒv‚ð‰ñ‚·
	if(num_platforms > 0) {
		for(cl_uint i = 0; i < num_platforms; i++) {
			// ƒvƒ‰ƒbƒgƒtƒH[ƒ€‚ÌID‚ðŠm”F‚·‚é
			cout << "--------------------" << endl;
			cout << "Platform No: " << platforms[i] << endl;

			// ƒvƒ‰ƒbƒgƒtƒH[ƒ€î•ñ‚ÌŠm”F
			getPlatformInfo(platforms[i]);

			// ƒfƒoƒCƒX‚ÌID‚ÌŠm”F
			device_ids = getDeviceIds(platforms[i], CL_DEVICE_TYPE_ALL, num_devices);
			for(cl_uint j = 0; j < num_devices; j++) {
				getDeviceInfo(device_ids[j]);
			}
		}
	}

	/*****************************************************************************
	 * 1. Create Context 
	 ****************************************************************************/
	cl_context clContext;
	cl_int errcode;

	clContext = clCreateContext(0, 1, &device_ids[0], NULL, NULL, &errcode);
	if(clContext == 0) {
		cout << "Failed to craete context." << endl;
		exit(1);
	}

	/*****************************************************************************
	 * 2. Create CommandQueue
	 ****************************************************************************/
	cl_command_queue clCommandQueue;
	clCommandQueue = clCreateCommandQueue(clContext, device_ids[0], 0, &errcode);

	/*****************************************************************************
	 * 3. Load CL Program
	 ****************************************************************************/
	string filename = "sample.cl";
	ifstream ifs(filename);
	if(ifs.fail()) {
		cout << "Failed to open file: " << filename.c_str() << endl;
		exit(1);
	}

	istreambuf_iterator<char> begin(ifs);
	istreambuf_iterator<char> last;
	string readData(begin, last);

	const char *programString = readData.c_str();
	const size_t programSize = readData.size();

	cl_program clProgram = clCreateProgramWithSource(clContext, 1, &programString, &programSize, &errcode);
	if(errcode != CL_SUCCESS) {
		cerr << "Failed to create program: " << programString << endl;
		return -1;
	}

	/*****************************************************************************
	 * 4. Build CL Program
	 ****************************************************************************/
	errcode = clBuildProgram(clProgram, 0, NULL, NULL, NULL, NULL);
	if (errcode != CL_SUCCESS) {
		cerr << "Failed to build program: " << programString << endl;
		return -1;
	}

	/*****************************************************************************
	 * 5. Create Kernel Object
	 ****************************************************************************/
	cl_kernel clKernel;
	clKernel = clCreateKernel(clProgram, "gpuMain", &errcode);
	if(errcode != CL_SUCCESS) {
		cerr << "Failed to create kernel" << endl;
		return -1;
	}

	/*****************************************************************************
	 * 6. Create Memory Object (Allocate Device Memory)
	 ****************************************************************************/
	// memory allocate on CPU
	float in0[128];
	float in1[128];
	float out[128];
	for(int i = 0; i < 128; i++) {
		in0[i] = (float)i * 100.0f;
		in1[i] = (float)i / 100.0f;
		out[i] = 0.0f;
	}

	// input data (read only)
	cl_mem clMemoryIn0 = clCreateBuffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(in0), in0, &errcode);
	cout << errcode << endl;
	cl_mem clMemoryIn1 = clCreateBuffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(in1), in1, &errcode);
	cout << errcode << endl;
	// output data (write only)
	cl_mem clMemoryOut = clCreateBuffer(clContext, CL_MEM_WRITE_ONLY,
		sizeof(out), NULL, &errcode);
	cout << errcode << endl;

	/* CL_MEM_USE_HOST_PTRで同期するときは、後からデータをセットする？
	clEnqueueWriteBuffer(clCommandQueue, clMemory, CL_TRUE, 0, 
		sizeof(sampleData), sampleData, 0, NULL, NULL);
	*/

	/*****************************************************************************
	 * 7. Set Kernel Arguments
	 ****************************************************************************/
	clSetKernelArg(clKernel, 0, sizeof(cl_mem), (void*)&clMemoryIn0);
	clSetKernelArg(clKernel, 1, sizeof(cl_mem), (void*)&clMemoryIn1);
	clSetKernelArg(clKernel, 2, sizeof(cl_mem), (void*)&clMemoryOut);

	/*****************************************************************************
	 * 8. Execute Kernel
	 ****************************************************************************/
	size_t WorkSize[] = {128};
	clEnqueueNDRangeKernel(clCommandQueue, clKernel, 1, NULL, WorkSize, NULL, 0, NULL, NULL);

	/*****************************************************************************
	 * 9. Get Result from Device Memory
	 ****************************************************************************/
	clEnqueueReadBuffer(clCommandQueue, clMemoryOut, CL_BLOCKING, 0,
		sizeof(cl_float) * 128, &out, 0, NULL, NULL);

	for(int i = 0; i < 128; i++) {
		printf("%6.6f %6.6f %6.6f %6.6f\n", in0[i], in1[i], out[i], (in0[i]+in1[i]));
	}

	/*****************************************************************************
	 * 10. Release Used Objects
	 ****************************************************************************/	
	clReleaseContext(clContext);
	clReleaseCommandQueue(clCommandQueue);
	clReleaseProgram(clProgram);
	clReleaseKernel(clKernel);
	clReleaseMemObject(clMemoryIn0);
	clReleaseMemObject(clMemoryIn1);
	clReleaseMemObject(clMemoryOut);

	free(platforms);
	free(device_ids);

	return 0;
}
