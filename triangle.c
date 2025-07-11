#include <stdint.h>
#include "display.h"
#include "triangle.h"
#include "swap.h"


void draw_triangle_pixel(
	int x, int y, uint32_t* color,
	vec4_t a, vec4_t b, vec4_t c
) {
	if (0 > x || x >= window_width || 0 > y || y >= window_height)
		return;
	vec2_t p = { x, y };
	vec2_t proj_a = vec2_from_vec4(a);
	vec2_t proj_b = vec2_from_vec4(b);
	vec2_t proj_c = vec2_from_vec4(c);
	if (
		(proj_a.x == proj_b.x && proj_b.x == proj_c.x) ||
		(proj_a.y == proj_b.y && proj_b.y == proj_c.y)
		) return;
	vec3_t weights = barycentric_weights(proj_a, proj_b, proj_c, p);
	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	// To have perspective correct uv interpolation we use 1 / w
	float interpolated_reciprocal_w = alpha / a.w + beta / b.w + gamma / c.w;

	int idx = x + (window_width * y);
	// HACK: using 1 - 1 / w so that less "depth" means closer to camera
	float depth = 1.0 - interpolated_reciprocal_w;
	if (depth < z_buffer[idx]) {
		draw_pixel(x, y, color);
		// update z-buffer
		z_buffer[idx] = depth;
	}
}

void draw_texel(
	int x, int y, uint32_t* texture,
	vec4_t a, vec4_t b, vec4_t c,
	tex2_t a_uv, tex2_t b_uv, tex2_t c_uv
) {
	vec2_t p = { x, y };
	vec2_t proj_a = vec2_from_vec4(a);
	vec2_t proj_b = vec2_from_vec4(b);
	vec2_t proj_c = vec2_from_vec4(c);
	if (
		(proj_a.x == proj_b.x && proj_b.x == proj_c.x) ||
		(proj_a.y == proj_b.y && proj_b.y == proj_c.y)
		) return;
	vec3_t weights = barycentric_weights(proj_a, proj_b, proj_c, p);
	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	float interpolated_u;
	float interpolated_v;
	float interpolated_reciprocal_w;
	
	// To have perspective correct uv interpolation we use 1 / w
	interpolated_u = alpha * (a_uv.u / a.w) + beta * (b_uv.u / b.w) + gamma * (c_uv.u / c.w);
	interpolated_v = alpha * (a_uv.v / a.w) + beta * (b_uv.v / b.w) + gamma * (c_uv.v / c.w);
	interpolated_reciprocal_w = alpha / a.w + beta / b.w + gamma / c.w;

	interpolated_u /= interpolated_reciprocal_w;
	interpolated_v /= interpolated_reciprocal_w;

	// HACK: use modulo to not overflow texture buffer when pixels get outside the triangle
	int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
	int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;

	int tex_idx = tex_x + (tex_y * texture_width);
	//if (index < 0 || index >= texture_height * texture_width) {
	//	sprintf("ERROR: index %d out of texture bounds", index);
	//}
	int idx = x + (window_width * y);
	// HACK: using 1 - 1 / w so that less "depth" means closer to camera
	float depth = 1.0 - interpolated_reciprocal_w;
	if (depth < z_buffer[idx]) {
		draw_pixel(x, y, texture[tex_idx]);
		// update z-buffer
		z_buffer[idx] = depth;
	}
}

void draw_filled_triangle(
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	uint32_t color
)
{
	// Sort vertices by y-coordinate ascending (y0 < y1 < y2)
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
	}
	if (y1 > y2) {
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
		float_swap(&z1, &z2);
		float_swap(&w1, &w2);

	}
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);

	}

	// Create vector points and texture coords
	vec4_t a = { x0, y0, z0, w0 };
	vec4_t b = { x1, y1, z1, w1 };
	vec4_t c = { x2, y2, z2, w2 };

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
				draw_triangle_pixel(x, y, color, a, b, c);
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
				draw_triangle_pixel(x, y, color, a, b, c);
			}
		}
	}
}

void draw_textured_triangle(
	int x0, int y0, float z0, float w0, float u0, float v0,
	int x1, int y1, float z1, float w1, float u1, float v1,
	int x2, int y2, float z2, float w2, float u2, float v2,
	uint32_t* texture
)
{
	// Sort vertices by y-coordinate ascending (y0 < y1 < y2)
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
	}
	if (y1 > y2) {
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
		float_swap(&z1, &z2);
		float_swap(&w1, &w2);
		float_swap(&u1, &u2);
		float_swap(&v1, &v2);
	}
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
	}

	v0 = 1.0 - v0;
	v1 = 1.0 - v1;
	v2 = 1.0 - v2;

	// Create vector points and texture coords
	vec4_t a = { x0, y0, z0, w0 };
	vec4_t b = { x1, y1, z1, w1 };
	vec4_t c = { x2, y2, z2, w2 };
	tex2_t a_uv = { u0, v0 };
	tex2_t b_uv = { u1, v1 };
	tex2_t c_uv = { u2, v2 };

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
				draw_texel(x, y, texture, a, b, c, a_uv, b_uv, c_uv);
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
				draw_texel(x, y, texture, a, b, c, a_uv, b_uv, c_uv);
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
