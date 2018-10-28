#include "bmp.h"
#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

bool check_bound(struct image **img, float x, float y) {
	bool r = (x >= 0 && x <= (*img)->width) && (y >= 0 && y <= (*img)->height);
	if(!r) {
		puts("bound error");
	}
	return r;
}

// leak
// how to leak?
// heap feng shui: [ ... new_img ... ] [ unsorted chunk ]
// to make unsorted chunk, just make [img a] -> [img a freed] [img b fastbin alloc] -> [img c] [remainder unsorted] [img b fastbin freed]
bool resize_img(struct image** img, const char *args)
{
	float w, h;
	if(sscanf(args, " %f %f", &w, &h) != 2) return false;

	w = floor(w);
	h = floor(h);

	float x = 0, y = 0;

	auto *new_img = create_image(w, h);
	if(!new_img) return false;

	new_img->color = (*img)->color;

	float zoomX = (*img)->width / w;
	float zoomY = (*img)->height / h;

	for(float i=0; i < h; i++)
	{
		x = 0;
		for(float j=0; j < w; j++)
		{
			*XY(new_img, j, i) = *XY(*img, x, y);
			x += zoomX;
		}
		y += zoomY;
	}
	free(*img);
	*img = new_img;
	return true;
}

// oob
bool draw_a_line(struct image **img, const char *args)
{
	float x1, x2, y1, y2, delta;
	float x, y;
	float deltaX, deltaY;

	int i = 0, count;

	if(sscanf(args, " %f %f %f %f %f", &x1, &y1, &x2, &y2, &delta) != 5) {
		puts("format error! : line x1 y1 x2 y2 delta");
		return false;
	}

	if(!check_bound(img, x1, y1)) return false;
	if(!check_bound(img, x2, y2)) return false;

	count = ((y2 - y1) * (y2 - y1) + (x2 - x1) * (x2 - x1)) / delta;

	deltaX = (x2 - x1) / count;
	deltaY = (y2 - y1) / count;

	if(count > 100000) {
		puts("delta too small!");
		return false;
	}

	for(i = 0, x = x1, y = y1; i < count; i++, x += deltaX, y += deltaY) {
		*XY(*img, x, y) = (*img)->color;
	}
	return true;
}

// this doesn't have delta, should be easier than drawing a line to fill some data
bool draw_a_rectangle(struct image **img, const char *args)
{
	float x1, x2, y1, y2;
	float x, y;
	if(sscanf(args, " %f %f %f %f", &x1, &y1, &x2, &y2) != 4) {
		puts("format error! : rectangle x1 x2 y1 y2");
		return false;
	}

	if(x1 >= x2 || y1 >= y2) {
		puts("rectangle range error! must be x1 < x2, y1 < y2");
		return false;
	}

	if(!check_bound(img, x1, y1)) return false;
	if(!check_bound(img, x2, y2)) return false;

	for(y = y1; y < y2; y++) {
		for(x = x1; x < x2; x++) {
			*XY(*img, x, y) = (*img)->color;
		}
	}
	return true;
}

// value to write
bool select_a_color(struct image **img, const char *args) {
	uint8_t r, g, b;
	if(sscanf(args, " #%02hhX%02hhX%02hhX", &r, &g, &b) != 3) {
		puts("format error!: color #XXXXXX");
		return false;
	}
	(*img)->color = {r, g, b};
	return true;
}

bool dump_image(struct image **img, const char *args)
{
	fwrite((*img)->pixel_array, (*img)->width * (*img)->height * sizeof(color_t), 1, stdout);
	return true;
}