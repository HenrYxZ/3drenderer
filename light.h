#pragma once

#include <stdint.h>
#include "vector.h"

uint32_t light_apply_intensity(uint32_t original_color, float factor);

typedef struct {
	vec3_t direction;
} directional_light_t;
