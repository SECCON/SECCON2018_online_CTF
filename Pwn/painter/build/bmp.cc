#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "bmp.h"

#pragma pack(push, 1)
//BMP header//
struct BmpHeader {
	char ident[2];
	uint32_t file_size;
	char reserve_1[2];
	char reserve_2[2];
	uint32_t pix_array_offset;
};

//DIB Header//
struct BITMAPINFOHEADER {
	uint32_t size;
	uint32_t width;
	uint32_t height;
	char colour_plain[2];
	uint16_t bbp;			//colour depth
	char compression_method[4];
	uint32_t img_size; 		//raw bitmap size
	char horizontal_res[4]; 
	char vertical_res[4]; 
	char num_colours[4]; 		//colours in colour pallet
	char important_colours[4]; 
};

#pragma pack(pop)

struct image *create_image(uint32_t width, uint32_t height) {
	if(width > MAX_LEN || height > MAX_LEN) {
		puts("image creation: image too large!");
		return NULL;
	}
	struct image *img = (struct image *)calloc((width * height + 1) * sizeof(color_t) + sizeof(uint32_t) * 2, 1);

	img->width = width;
	img->height = height;

	return img;
}

