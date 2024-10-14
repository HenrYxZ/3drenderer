#include <stdint.h>
#include "triangle.h"


void int_swap(int* a, int* b) {
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
	}
	if (y1 > y2) {
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
	}
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
	}

	int my = y1;
	int mx = ((float)(x2 - x0) * (y1 - y0)) / (float)(y2 - y0) + x0;

	// draw flat bottom triangle


	// draw flat top triangle
}
