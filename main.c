#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "array.h"
#include "display.h"
#include "mesh.h"
#include "vector.h"

triangle_t* triangles_to_render = NULL;

float fov_factor = 640;

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };


bool is_running = false;
int previous_frame_time = 0;

void setup(void) {
	color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);
	load_obj_file("./assets/f22.obj");
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

////////////////////////////////////////////////////////////////////////////////
// Function that receives a 3D vector and returns a projected 2D point
////////////////////////////////////////////////////////////////////////////////
vec2_t project(vec3_t point) {
	vec2_t projected_point = {
		.x = (fov_factor * point.x) / point.z,
		.y = (fov_factor * point.y) / point.z
	};
	return projected_point;
}

void update(void) {

	int delta_time = SDL_GetTicks() - previous_frame_time;
	int time_to_wait = FRAME_TARGET_TIME - delta_time;

	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
		SDL_Delay(time_to_wait);
	}

	previous_frame_time = SDL_GetTicks();

	// Initialize the array of projected triangles
	triangles_to_render = NULL;

	// Cube animation
	mesh.rotation.x += 0.01;
	//mesh.rotation.y += 0.01;
	//mesh.rotation.z += 0.01;

	// Loop all triangle faces of our mesh
	int num_faces = array_length(mesh.faces);
	for (int i = 0; i < num_faces; i++) {
		face_t mesh_face = mesh.faces[i];
		vec3_t face_vertices[3] = {
			mesh.vertices[mesh_face.a - 1],
			mesh.vertices[mesh_face.b - 1],
			mesh.vertices[mesh_face.c - 1]
		};

		triangle_t projected_triangle;

		// Loop all three vertices of this current face and apply transformations
		for (int j = 0; j < 3; j++) {
			vec3_t transformed_vertex = vec3_rotate_x(face_vertices[j], mesh.rotation.x);
			transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
			transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

			// Translate point away from the camera
			transformed_vertex.z -= camera_position.z;

			// Project current vertex
			vec2_t projected_point = project(transformed_vertex);

			// Scale and translate the projected points to the middle of the screen
			projected_point.x += (window_width / 2);
			projected_point.y += (window_height / 2);
			projected_triangle.points[j] = projected_point;
		}
		
		// Save the projected triangle in the array of triangles to render
		array_push(triangles_to_render, projected_triangle);
	}
}


void render(void) {
	
	draw_grid();

	int num_triangles = array_length(triangles_to_render);
	for (int i = 0; i < num_triangles; i++) {
		triangle_t triangle = triangles_to_render[i];

		draw_triangle(
			triangle.points[0].x,
			triangle.points[0].y,
			triangle.points[1].x,
			triangle.points[1].y,
			triangle.points[2].x,
			triangle.points[2].y,
			0xFF00F800
		);
	}

	// Clear the array of triangles to render every frame loop
	array_free(triangles_to_render);

	render_color_buffer();
	clear_color_buffer(0xFF000000);

	SDL_RenderPresent(renderer);
}

void free_resources(void) {
	array_free(mesh.faces);
	array_free(mesh.vertices);
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
	free_resources();

	return 0;
}