#pragma once

#include "light.h"

uint32_t light_apply_intensity(uint32_t original_color, float factor) {
	uint32_t mask_r = 0x00FF0000;
	uint32_t mask_g = 0x0000FF00;
	uint32_t mask_b = 0x000000FF;

	uint32_t a = (original_color & 0xFF000000);
	uint32_t r = (original_color & mask_r) * factor;
	uint32_t g = (original_color & mask_g) * factor;
	uint32_t b = (original_color & mask_b) * factor;

	uint32_t new_color = a | (r & mask_r) | (g & mask_g) | (b & mask_b);
	return new_color;
}