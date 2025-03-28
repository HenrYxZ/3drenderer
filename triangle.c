#include <stdint.h>
#include "display.h"
#include "triangle.h"
#include "swap.h"

void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
	float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
	float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

	float x_start = x0;
	float x_end = x0;

	for (int y = y0; y < y1; y++) {
		draw_line(x_start, y, x_end, y, color);
		x_start += inv_slope_1;
		x_end += inv_slope_2;
	}
}

void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
	float inv_slope_1 = (float)(x2 - x0) / (y2 - y0);
	float inv_slope_2 = (float)(x2 - x1) / (y2 - y1);

	float x_start = x2;
	float x_end = x2;

	for (int y = y2; y >= y0; y--) {
		draw_line(x_start, y, x_end, y, color);
		x_start -= inv_slope_1;
		x_end -= inv_slope_2;
	}
}

void draw_texel(int x, int y, uint32_t* texture, vec2_t a, vec2_t b, vec2_t c, float u0, float v0, float u1, float v1, float u2, float v2)
{
	vec2_t p = { x, y };
	vec3_t weights = barycentric_weights(a, b, c, p);
	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	float interpolated_u = alpha * u0 + beta * u1 + gamma * u2;
	float interpolated_v = alpha * v0 + beta * v1 + gamma * v2;

	int tex_x = abs((int)(interpolated_u * texture_width));
	int tex_y = abs((int)(interpolated_v * texture_height));

	int index = tex_x + (tex_y * texture_width);
	//if (index < 0 || index >= texture_height * texture_width) {
	//	sprintf_s("ERROR: index %d out of texture bounds", index);
	//}
	draw_pixel(x, y, texture[index]);
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

	
	if (y1 == y2) {
		fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
	} else if (y0 == y1) {
		fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
	}
	else {
		int my = y1;
		int mx = ((float)(x2 - x0) * (y1 - y0)) / (float)(y2 - y0) + x0;
		fill_flat_bottom_triangle(x0, y0, x1, y1, mx, my, color);
		fill_flat_top_triangle(x1, y1, mx, my, x2, y2, color);
	}
}



void draw_textured_triangle(
	int x0, int y0, float u0, float v0,
	int x1, int y1, float u1, float v1,
	int x2, int y2, float u2, float v2,
	uint32_t* texture
)
{
	// Sort vertices by y-coordinate ascending (y0 < y1 < y2)
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
	}
	if (y1 > y2) {
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
		float_swap(&u1, &u2);
		float_swap(&v1, &v2);
	}
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
	}

	// Create vector points
	vec2_t a = { x0, y0 };
	vec2_t b = { x1, y1 };
	vec2_t c = { x2, y2 };

	// Render the upper part of the triangle (flat-bottom)
	float inv_slope_1 = 0;
	float inv_slope_2 = 0;

	if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
	if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y1 - y0 != 0) {
		for (int y = y0; y <= y1; y++) {
			int x_start = x1 + (y - y1) * inv_slope_1;
			int x_end = x0 + (y - y0) * inv_slope_2;

			if (x_end < x_start) {
				int_swap(&x_start, &x_end);
			}

			for (int x = x_start; x <= x_end; x++) {
				draw_texel(x, y, texture, a, b, c, u0, v0, u1, v1, u2, v2);
			}
		}
	}
	
	// Render the bottom part of the triangle (flat-top)
	inv_slope_1 = 0;
	inv_slope_2 = 0;

	if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
	if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y2 - y1 != 0) {
		for (int y = y1; y <= y2; y++) {
			int x_start = x1 + (y - y1) * inv_slope_1;
			int x_end = x0 + (y - y0) * inv_slope_2;

			if (x_end < x_start) {
				int_swap(&x_start, &x_end);
			}

			for (int x = x_start; x <= x_end; x++) {
				draw_texel(x, y, texture, a, b, c, u0, v0, u1, v1, u2, v2);
			}
		}
	}

}

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {
	// Find the vectors between the vertices ABC and point p
	vec2_t ac = vec2_sub(c, a);
	vec2_t ab = vec2_sub(b, a);
	vec2_t pc = vec2_sub(c, p);
	vec2_t pb = vec2_sub(b, p);
	vec2_t ap = vec2_sub(p, a);

	// Area of the full parallelogram ABC using the cross product
	float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x);
	float alpha = (pc.x * pb.y - pc.y * pb.x) / area_parallelogram_abc;
	float beta = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc;
	float gamma = 1.0 - alpha - beta;
	vec3_t weights = { alpha, beta, gamma };
	return weights;
}
