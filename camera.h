#pragma once

#include "vector.h"

typedef struct {
	vec3_t position;
	vec3_t direction;
	vec3_t forward_velocity;
	float yaw;
	float pitch;
} camera_t;

vec3_t get_camera_position(void);
vec3_t get_camera_direction(void);
void set_camera_direction(vec3_t direction);
float get_camera_yaw(void);
float get_camera_pitch(void);
void add_camera_position_y(float value);
void add_camera_yaw(float value);
void set_camera_forward_velocity(vec3_t forward_velocity);
void update_camera_position(void);
