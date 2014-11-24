__kernel void gpuMain(__global uchar *in_image) {
	size_t index_x = 3 * ( get_global_id(0) + 1920 * get_global_id(1) );

	unsigned char grayscale = ( in_image[ 0 + index_x ] + in_image[ 1 + index_x ] + in_image[ 2 + index_x ] ) / 3;
	in_image[index_x] = grayscale;
	in_image[index_x+1] = grayscale;
	in_image[index_x+2] = grayscale;
}