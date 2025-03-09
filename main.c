#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "array.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "mesh.h"
#include "vector.h"
#include "triangle.h"

triangle_t* triangles_to_render = NULL;

float fov_factor = 640;

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };
directional_light_t light = { .direction = { .x =  2, .y = -4, .z = 1 } };
mat4_t proj_matrix;


bool is_running = false;
int previous_frame_time = 0;

void setup(void) {
	render_method = RENDER_FILL_TRIANGLE;
	cull_method = CULL_BACKFACE;


	color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);

	// Initialize projection matrix
	float fov = M_PI / 3.0; // 60� in radians
	float aspect = (float)window_height / (float)window_width;
	float znear = 0.1;
	float zfar = 100.0;
	proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

	load_obj_file("./assets/f22.obj");
	//load_cube_mesh_data();
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
			if (event.key.keysym.sym == SDLK_1)
				render_method = RENDER_WIRE_VERTEX;
			if (event.key.keysym.sym == SDLK_2)
				render_method = RENDER_WIRE;
			if (event.key.keysym.sym == SDLK_3)
				render_method = RENDER_FILL_TRIANGLE;
			if (event.key.keysym.sym == SDLK_4)
				render_method = RENDER_FILL_TRIANGLE_WIRE;
			if (event.key.keysym.sym == SDLK_c)
				cull_method = CULL_BACKFACE;
			if (event.key.keysym.sym == SDLK_d)
				cull_method = CULL_NONE;
			break;
		default:
			break;
	}
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

	mesh.rotation.x += 0.005;
	mesh.rotation.y += 0.005;
	// mesh.rotation.z += 0.01;

	// mesh.scale.x += 0.002;
	// mesh.scale.y += 0.001;

	// mesh.translation.y += 0.01;
	// Translate the vertices away from the camera
	mesh.translation.z = 5.0;

	// Create a scale matrix that will be used to multiply the mesh vertices
	mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
	mat4_t translation_matrix = mat4_make_translation(
		mesh.translation.x, mesh.translation.y, mesh.translation.z
	);
	mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
	mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
	mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

	// Loop all triangle faces of our mesh
	int num_faces = array_length(mesh.faces);
	for (int i = 0; i < num_faces; i++) {
		face_t mesh_face = mesh.faces[i];

		vec3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[mesh_face.a - 1];
		face_vertices[1] = mesh.vertices[mesh_face.b - 1];
		face_vertices[2] = mesh.vertices[mesh_face.c - 1];

		vec4_t transformed_vertices[3];

		// Loop all three vertices of this current face and apply transformations
		for (int j = 0; j < 3; j++) {
			vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

			mat4_t world_matrix = mat4_identity();
			// Use a matrix to scale
			world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
			// Use matrix to rotate
			mat4_t rotation_matrix = mat4_identity();
			rotation_matrix = mat4_mul_mat4(rotation_matrix_z, rotation_matrix);
			rotation_matrix = mat4_mul_mat4(rotation_matrix_y, rotation_matrix);
			rotation_matrix = mat4_mul_mat4(rotation_matrix_x, rotation_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix, world_matrix);
			// Use matrix to translate
			world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);
			
			transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

			// Save transformed vertex in the array of transformed vertices
			transformed_vertices[j] = transformed_vertex;
		}

		// Check backface culling
		vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
		vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
		vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]); /* C---B */

		// Get the vector subtraction of B-A and C-A
		vec3_t vector_ab = vec3_sub(vector_b, vector_a);
		vec3_t vector_ac = vec3_sub(vector_c, vector_a);
		vec3_normalize(&vector_ab);
		vec3_normalize(&vector_ac);

		// Compute the face normal (using cross product to find perpendicular)
		vec3_t normal = vec3_cross(vector_ab, vector_ac);
		vec3_normalize(&normal);

		// Find the vector between vertex A in the triangle and the camera origin
		vec3_t camera_ray = vec3_sub(camera_position, vector_a);

		// Calculate how aligned the camera ray is with the face normal (using dot product)
		float dot_normal_camera = vec3_dot(normal, camera_ray);

		if (cull_method == CULL_BACKFACE) {
			// Bypass the triangles that are looking away from the camera
			if (dot_normal_camera < 0) {
				continue;
			}
		}

		// Loop all three vertices to perform projection
		vec4_t projected_points[3];
		for (int j = 0; j < 3; j++) {
			// Project the current vertex
			projected_points[j] = mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);

			// Scale and translate the projected points to the middle of the screen
			projected_points[j].x *= (window_width / 2.0);
			projected_points[j].y *= (window_height / 2.0);

			// Invert y axis to use screen coordinates
			projected_points[j].y *= -1;

			projected_points[j].x += (window_width / 2.0);
			projected_points[j].y += (window_height / 2.0);
		}

		// Calculate average depth of each face on the vertices after transformation
		float avg_depth = (
			transformed_vertices[0].z +
			transformed_vertices[1].z +
			transformed_vertices[2].z
		) / 3.0;

		// Calculate color from flat shading
		vec3_t l = {
			.x = -light.direction.x, .y = -light.direction.y,  .z = -light.direction.z
		};
		vec3_normalize(&l);
		float lambert_factor = vec3_dot(l, normal);
		lambert_factor = lambert_factor > 0.0 ? lambert_factor : 0.0;
		uint32_t triangle_color = light_apply_intensity(mesh_face.color, lambert_factor);

		triangle_t projected_triangle = {
			.points = {
				{ projected_points[0].x, projected_points[0].y },
				{ projected_points[1].x, projected_points[1].y },
				{ projected_points[2].x, projected_points[2].y }
			},
			.color = triangle_color,
			.avg_depth = avg_depth
		};

		// Save the projected triangle in the array of triangles to render
		array_push(triangles_to_render, projected_triangle);
	}	// end of for

	// sort triangles by avg_depth
	if (triangles_to_render != NULL) {
		int num_triangles = array_length(triangles_to_render);
		for (int i = 0; i < num_triangles - 1; i++) {
			for (int j = i; j < num_triangles; j++) {
				if (triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth) {
					// Swap triangles if the ith has less avg depth than the jth
					triangle_t temp = triangles_to_render[i];
					triangles_to_render[i] = triangles_to_render[j];
					triangles_to_render[j] = temp;
				}
			}
		}
	}
}


void render(void) {
	
	draw_grid();

	int num_triangles = array_length(triangles_to_render);
	for (int i = 0; i < num_triangles; i++) {
		triangle_t triangle = triangles_to_render[i];

		if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE) {
			// draw fill triangle
			draw_filled_triangle(
				triangle.points[0].x,
				triangle.points[0].y,
				triangle.points[1].x,
				triangle.points[1].y,
				triangle.points[2].x,
				triangle.points[2].y,
				triangle.color
			);
		}
		
		if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE) {
			// draw wireframe
			draw_triangle(
				triangle.points[0].x,
				triangle.points[0].y,
				triangle.points[1].x,
				triangle.points[1].y,
				triangle.points[2].x,
				triangle.points[2].y,
				0xFFFFFFFF
			);
		}
		
		if (render_method == RENDER_WIRE_VERTEX) {
			int size = 6;
			draw_rect(triangle.points[0].x, triangle.points[0].y - size / 2, size, size, 0xFFFF0000);
			draw_rect(triangle.points[1].x, triangle.points[1].y - size / 2, size, size, 0xFFFF0000);
			draw_rect(triangle.points[2].x, triangle.points[2].y - size / 2, size, size, 0xFFFF0000);
		}
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