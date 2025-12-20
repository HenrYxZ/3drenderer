#include "matrix.h"
#include "camera.h"

static camera_t camera = {
	.position = {.x = 0, .y = 0, .z = -5 },
	.direction = {.x = 0, .y = 0, .z = 1},
	.forward_velocity = {.x = 0, .y = 0, .z = 0},
	.yaw = 0.0
};

vec3_t get_camera_position(void)
{
	return camera.position;
}

vec3_t get_camera_direction(void) {
	return camera.direction;
}

void set_camera_direction(vec3_t direction)
{
	camera.direction = direction;
}

float get_camera_yaw(void)
{
	return camera.yaw;
}

float get_camera_pitch(void)
{
	return camera.pitch;
}

void add_camera_position_y(float value)
{
	camera.position.y += value;
}

void add_camera_yaw(float value)
{
	camera.yaw += value;
}

void add_camera_pitch(float value)
{
	camera.pitch += value;
}

void set_camera_forward_velocity(vec3_t forward_velocity)
{
	camera.forward_velocity = forward_velocity;
}

void update_camera_position(void)
{
	camera.position = vec3_add(camera.position, camera.forward_velocity);
}

vec3_t get_camera_target(void)
{
	vec3_t target = { 0, 0, 1 };
	mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
	mat4_t camera_pitch_rotation = mat4_make_rotation_x(camera.pitch);
	mat4_t rotation = mat4_mul_mat4(camera_yaw_rotation, camera_pitch_rotation);
	camera.direction = vec3_from_vec4(
		mat4_mul_vec4(rotation, vec4_from_vec3(target))
	);
	target = vec3_add(get_camera_position(), camera.direction);
	return target;
}


