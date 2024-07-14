#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

int window_width = 800;
int window_height = 600;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

// Declare a pointer to an array of uint32 elements
uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

bool is_running = false;


bool initialize_window(void) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}

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

	return true;
}

void setup(void) {
	color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);
}

void handle_input(void) {
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
				is_running = false;
			break;
		default:
			break;
	}
}

void update(void) {

}

void clear_color_buffer(uint32_t color) {
	for (int j = 0; j < window_height; j++) {
		for (int i = 0; i < window_width; i++) {
			color_buffer[(window_width * j) + i] = color;
		}
	}
}

void render(void) {
	SDL_SetRenderDrawColor(renderer, 180, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_RenderPresent(renderer);
}

void destroy_window(void) {
	free(color_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}


int main(int argc, char* argv[]) {
	is_running = initialize_window();

	setup();

	while (is_running) {
		handle_input();
		update();
		render();
	}

	destroy_window();

	return 0;
}