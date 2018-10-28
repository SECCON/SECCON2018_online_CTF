#include <stdint.h>

//structure to hold all necessary image data
typedef struct {
	uint8_t r, g, b;
} color_t;

struct image {
	uint32_t width;
	uint32_t height;
	color_t  color;
	color_t  pixel_array[];
};

static inline color_t *XY(struct image *img, float x, float y) {
	return &img->pixel_array[(int)(y * img->width + x)];
}

struct image *create_image(uint32_t width, uint32_t height);
//free the memory used by the images pixel data
void free_img(unsigned char *pixel_array, struct image *img);

#define MAX_LEN 8192
