#include "OpenCLImage2D.h"
#include <Windows.h> // for Bitmap I/O

OpenCLImage2D::OpenCLImage2D(void) {}

OpenCLImage2D::~OpenCLImage2D(void) {}

int OpenCLImage2D::run(void) {
	/*****************************************************************************
	 * 0. Get Device Info 
	 ****************************************************************************/
	// Platform
	cl_platform_id* platforms;
	cl_uint num_platforms;
	// Device
	cl_device_id* device_ids;
	cl_uint num_devices;

	platforms = GetPlatformIds(num_platforms);
	cout << "Number of Platforms: " << num_platforms << endl;

	// 
	if(num_platforms > 0) {
		for(cl_uint i = 0; i < num_platforms; i++) {
			// 
			cout << "--------------------" << endl;
			cout << "Platform No: " << platforms[i] << endl;

			// 
			GetPlatformInfo(platforms[i]);

			// 
			device_ids = GetDeviceIds(platforms[i], CL_DEVICE_TYPE_ALL, num_devices);
			for(cl_uint j = 0; j < num_devices; j++) {
				GetDeviceInfo(device_ids[j]);
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
	string filename = "image2d.cl";
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
	// input data
	cl_mem clMemorySrc;
	errcode = CreateImage2D(clContext, 1920, 1080, &clMemorySrc);
	cout << errcode << endl;
	// input and output data
	cl_mem clMemoryDst;
	errcode = CreateImage2D(clContext, 1920, 1080, &clMemoryDst);
	cout << errcode << endl;

	// memory allocate on CPU

	FILE *fp;
	int ret;
	if((ret = fopen_s(&fp, "sasuke.bmp", "r")) != 0) {
		cout << "Failed to open file: sasuke.bmp" << endl; 
		return -1;
	}
	cl_uchar* buffer = (cl_uchar*)malloc(sizeof(cl_uchar)*1920*1080*3);
	fseek(fp, 54, SEEK_SET);
	fread(buffer, sizeof(cl_uchar), 1920*1080*3, fp);

	// data write to device
	WriteImage2D(clCommandQueue, clMemorySrc, 1920, 1080, 3, (void*)buffer);

	/*****************************************************************************
	 * 7. Set Kernel Arguments
	 ****************************************************************************/
	clSetKernelArg(clKernel, 0, sizeof(cl_mem), (void*)&clMemorySrc);
	clSetKernelArg(clKernel, 1, sizeof(cl_mem), (void*)&clMemoryDst);

	/*****************************************************************************
	 * 8. Execute Kernel
	 ****************************************************************************/
	size_t global_work_size[2]  ={1920, 1080};
	size_t  local_work_size[2]  ={1,1};
	errcode = clEnqueueNDRangeKernel(clCommandQueue, clKernel, 2,
		NULL, global_work_size, local_work_size, 0, NULL, NULL);
	cout << errcode << endl;

	/*****************************************************************************
	 * 9. Get Result from Device Memory
	 ****************************************************************************/
	cl_uchar* pOut = (cl_uchar*)malloc(sizeof(cl_uchar)*1920*1080*3);
	clEnqueueReadBuffer(clCommandQueue, clMemoryDst, CL_BLOCKING, 0,
		sizeof(cl_uchar)*1920*1080*3, (void*)pOut, 0, NULL, NULL);

	// save as bitmap image
	DWORD dw;
	HANDLE hFile;
	hFile = CreateFile(L"gray.bmp", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	BITMAPFILEHEADER bmpFileHeader;
	bmpFileHeader.bfType = 0x4d42;
	bmpFileHeader.bfReserved1 = 0;
	bmpFileHeader.bfReserved2 = 0;
	bmpFileHeader.bfOffBits = 54;
	bmpFileHeader.bfSize = 1920*1080*3 + bmpFileHeader.bfOffBits;

	BITMAPINFOHEADER *pBmpInfoHdr;
	unsigned char pHeaderBuffer[40];
	pBmpInfoHdr = (BITMAPINFOHEADER*)pHeaderBuffer;
	pBmpInfoHdr->biBitCount = 24;
	pBmpInfoHdr->biSize = sizeof(BITMAPINFOHEADER);
	pBmpInfoHdr->biWidth  = 1920;
	pBmpInfoHdr->biHeight = 1080;
	pBmpInfoHdr->biPlanes = 1;
	pBmpInfoHdr->biCompression = BI_RGB;
	pBmpInfoHdr->biSizeImage = 0;
	pBmpInfoHdr->biXPelsPerMeter = 0;
	pBmpInfoHdr->biYPelsPerMeter = 0;
	pBmpInfoHdr->biClrUsed = 0;
	pBmpInfoHdr->biClrImportant = 0;

	WriteFile(hFile, &bmpFileHeader, sizeof(BITMAPFILEHEADER), &dw, NULL);
	WriteFile(hFile, pHeaderBuffer, sizeof(BITMAPINFOHEADER), &dw, NULL);
	WriteFile(hFile, pOut, sizeof(unsigned char)*1920*1080*3, &dw, NULL);
	CloseHandle(hFile);

	/*****************************************************************************
	 * 10. Release Used Objects
	 ****************************************************************************/	
	clReleaseContext(clContext);
	clReleaseCommandQueue(clCommandQueue);
	clReleaseProgram(clProgram);
	clReleaseKernel(clKernel);
	clReleaseMemObject(clMemorySrc);
	clReleaseMemObject(clMemoryDst);

	free(platforms);
	free(device_ids);
	free(buffer);

	return 0;
}

cl_int OpenCLImage2D::CreateImage2D(cl_context clContext, size_t imageWidth, size_t imageHeight, cl_mem *image) {
	cl_image_format imageFormat;
	imageFormat.image_channel_order = CL_RGB;
	imageFormat.image_channel_data_type = CL_UNORM_INT8;

	cl_int err;
	cl_mem m = clCreateImage2D(
		clContext,
		CL_MEM_READ_WRITE,
		&imageFormat,
		imageWidth,
		imageHeight,
		0,
		NULL,
		&err
	);

	if(err != CL_SUCCESS) {
		cout << "Failed to create Image2D memory object" << endl;
		return err;
	}

	*image = m;

	return CL_SUCCESS;
}

cl_int OpenCLImage2D::WriteImage2D(cl_command_queue clCommandQueue,
	cl_mem image, size_t imageWidth, size_t imageHeight, size_t image_row_pitch,
	const void *host_ptr) {

	size_t origin[3] = {0};
	size_t region[3] = {imageWidth, imageHeight, 1};
	cl_int err = clEnqueueWriteImage(
		clCommandQueue,
		image,
		CL_TRUE,
		origin,
		region,
		image_row_pitch,
		0,
		host_ptr,
		0,
		NULL,
		NULL
	);

	if(err != CL_SUCCESS) {
		cout << "Failed to write data to device memory" << endl;
		return err;
	}

	return CL_SUCCESS;
}

int main(int argc, char* argv[]) {
	OpenCLImage2D instance;
	instance.run();
}