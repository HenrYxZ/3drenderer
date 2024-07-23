#include "display.h"

int window_width = 800;
int window_height = 600;
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
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);

	window_width = display_mode.w;
	window_height = display_mode.h;

	// Create a SDL window
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_width,
		window_height,
		SDL_WINDOW_BORDERLESS
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
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	return true;
}


void draw_grid(void) {
	uint32_t color = 0xFF808080;
	for (int j = 0; j < window_height; j++) {
		for (int i = 0; i < window_width; i++) {
			if (i % 10 == 0 || j % 10 == 0)
				color_buffer[(window_width * j) + i] = color;
		}
	}
}


void draw_pixel(int x, int y, uint32_t color) {
	if (x < window_width && y < window_height)
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

