#include "display.h"

int window_width = 640;
int window_height = 480;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

// Declare a pointer to an array of uint32 elements
uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;


bool initialize_window(void) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}

	// Use SDL to query what is the fullscreen max. width and height
	// SDL_DisplayMode display_mode;
	// SDL_GetCurrentDisplayMode(0, &display_mode);

	// window_width = display_mode.w;
	// window_height = display_mode.h;

	// Create a SDL window
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_width,
		window_height,
		SDL_WINDOW_VULKAN
		//SDL_WINDOW_BORDERLESS
	);
	if (!window) {
		fprintf(stderr, "Error creating SDL window.\n");
		return false;
	}

	// Create a SDL renderer
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		fprintf(stderr, "Error creating SDL renderer.\n");
		return false;
	}
	// SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	return true;
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color)
{
	int delta_x = x1 - x0;
	int delta_y = y1 - y0;

	int longest_side_length = (abs(delta_x) > abs(delta_y)) ? abs(delta_x) : abs(delta_y);

	float x_inc = delta_x / (float)longest_side_length;
	float y_inc = delta_y / (float)longest_side_length;

	float current_x = x0;
	float current_y = y0;
	for (int i = 0; i <= longest_side_length; i++) {
		draw_pixel(round(current_x), round(current_y), color);
		current_x += x_inc;
		current_y += y_inc;
	}
}


void draw_grid(void) {
	uint32_t color = 0xFF808080;
	for (int j = 0; j < window_height; j += 10) {
		for (int i = 0; i < window_width; i += 10) {
				color_buffer[(window_width * j) + i] = color;
		}
	}
}


void draw_pixel(int x, int y, uint32_t color) {
	if (0 < x && x < window_width && 0 < y && y < window_height)
		color_buffer[(window_width * y) + x] = color;
}


void draw_rect(int x, int y, int w, int h, uint32_t color) {
	// Check x and y are inside the 
	if (x < 0)
		x = 0;
	else if (x >= window_width)
		x = window_width - 1;
	if (y < 0)
		y = 0;
	else if (y >= window_height)
		y = window_height;

	// check w and h
	int max_w = window_width - 1 - x;
	int max_h = window_height - 1 - y;

	if (w > max_w)
		w = max_w;
	if (h > max_h)
		h = max_h;

	for (int j = y; j < y + h; j++) {
		for (int i = x; i < x + w; i++) {
			color_buffer[(window_width * j) + i] = color;
		}
	}
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	draw_line(x0, y0, x1, y1, color);
	draw_line(x1, y1, x2, y2, color);
	draw_line(x2, y2, x0, y0, color);
}


void render_color_buffer(void) {
	SDL_UpdateTexture(
		color_buffer_texture,
		NULL,
		color_buffer,
		(int)(window_width * sizeof(uint32_t))
	);
	SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}


void clear_color_buffer(uint32_t color) {
	for (int j = 0; j < window_height; j++) {
		for (int i = 0; i < window_width; i++) {
			color_buffer[(window_width * j) + i] = color;
		}
	}
}


void destroy_window(void) {
	free(color_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

