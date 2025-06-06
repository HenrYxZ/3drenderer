#include <math.h>

#include "matrix.h"
#include "vector.h"


mat4_t mat4_identity(void) {
	mat4_t m = {{
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	}};
	return m;
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
	mat4_t m = mat4_identity();
	m.m[0][0] = sx;
	m.m[1][1] = sy;
	m.m[2][2] = sz;
	return m;
}

mat4_t mat4_make_translation(float tx, float ty, float tz)
{
	mat4_t m = mat4_identity();
	m.m[0][3] = tx;
	m.m[1][3] = ty;
	m.m[2][3] = tz;
	return m;
}

mat4_t mat4_make_rotation_x(float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	mat4_t m = mat4_identity();
	m.m[1][1] = c;
	m.m[1][2] = -s;
	m.m[2][1] = s;
	m.m[2][2] = c;
	return m;
}

mat4_t mat4_make_rotation_y(float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	mat4_t m = mat4_identity();
	m.m[0][0] = c;
	m.m[0][2] = s;
	m.m[2][0] = -s;
	m.m[2][2] = c;
	return m;
}

mat4_t mat4_make_rotation_z(float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	mat4_t m = mat4_identity();
	m.m[0][0] = c;
	m.m[0][1] = -s;
	m.m[1][0] = s;
	m.m[1][1] = c;
	return m;
}

mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar)
{
	mat4_t m = { { { 0 } } };
	float f = 1 / tan(fov / 2);
	m.m[0][0] = aspect * f;
	m.m[1][1] = f;
	float l = zfar / (zfar - znear);
	m.m[2][2] = l;
	m.m[2][3] = -l * znear;
	m.m[3][2] = 1.0;

	return m;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v)
{
	vec4_t result;
	result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
	result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
	result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
	result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

	return result;
}

vec4_t mat4_mul_vec4_project(mat4_t m, vec4_t v)
{
	vec4_t result = mat4_mul_vec4(m, v);
	// perform perspective divide
	if (result.w != 0.0) {
		result.x /= result.w;
		result.y /= result.w;
		result.z /= result.w;
	}
	return result;
}

mat4_t mat4_mul_mat4(mat4_t a, mat4_t b)
{
	mat4_t m;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m.m[i][j] = (
				a.m[i][0] * b.m[0][j] +
				a.m[i][1] * b.m[1][j] +
				a.m[i][2] * b.m[2][j] +
				a.m[i][3] * b.m[3][j]
				);
		}
	}
	return m;
}

mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up) {
	// Compute the forward (z), right (x), and up (y) vectors
	vec3_t z = vec3_sub(target, eye);
	vec3_normalize(&z);
	vec3_t x = vec3_cross(up, z);
	vec3_normalize(&x);
	vec3_t y = vec3_cross(z, x);

	// | x.x   x.y   x.z  -dot(x,eye) |
	// | y.x   y.y   y.z  -dot(y,eye) |
	// | z.x   z.y   z.z  -dot(z,eye) |
	// |   0     0     0            1 |
	mat4_t view_matrix = { {
			{ x.x, x.y, x.z, -vec3_dot(x, eye) },
			{ y.x, y.y, y.z, -vec3_dot(y, eye) },
			{ z.x, z.y, z.z, -vec3_dot(z, eye) },
			{   0,   0,   0,                 1 }
	} };
	return view_matrix;
}
