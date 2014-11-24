#pragma once

#include <iostream>
using namespace std;

#include "..\OpenCLBase\OpenCLBase.h"

class GrayScale : public OpenCLBase {
public:
	GrayScale(void);
	~GrayScale(void);

	int run(void) override;
};