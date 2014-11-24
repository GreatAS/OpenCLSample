#pragma once

#include <iostream>
using namespace std;

#include "..\OpenCLBase\OpenCLBase.h"

class OpenCLImage2D : public OpenCLBase {
public:
	OpenCLImage2D(void);
	~OpenCLImage2D(void);

	int run(void) override;

private:
	// TODO move to OpenCLBase
	cl_int CreateImage2D(cl_context clContext, size_t imageWidth,
		size_t imageHeight, cl_mem *image);
	cl_int WriteImage2D(cl_command_queue clCommandQueue, cl_mem image,
		size_t imageWidth, size_t imageHeight, size_t image_row_pitch,
		const void *host_ptr);
};

