#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "texture.h"
#include "vector.h"

typedef struct {
	int a;
	int b;
	int c;

	tex2_t a_uv;
	tex2_t b_uv;
	tex2_t c_uv;
	uint32_t color;
} face_t;

typedef struct {
	vec4_t points[3];
	tex2_t texcoords[3];
	uint32_t color;
} triangle_t;

void draw_triangle_pixel(
	int x, int y, uint32_t* color,
	vec4_t a, vec4_t b, vec4_t c
);

void draw_filled_triangle(
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	uint32_t color
);

void draw_texel(
	int x, int y, uint32_t* texture,
	vec4_t a, vec4_t b, vec4_t c,
	tex2_t a_uv, tex2_t b_uv, tex2_t c_uv
);

void draw_textured_triangle(
	int x0, int y0, float z0, float w0, float u0, float v0,
	int x1, int y1, float z1, float w1, float u1, float v1,
	int x2, int y2, float z2, float w2, float u2, float v2,
	uint32_t* texture
);

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p);

#endif
