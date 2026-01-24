#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "upng.h"
#include "array.h"
#include "camera.h"
#include "clipping.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "mesh.h"
#include "vector.h"
#include "texture.h"
#include "triangle.h"


#define MAX_TRIANGLES_PER_MESH 10000

triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

float fov_factor = 640;

mat4_t view_matrix;
mat4_t proj_matrix;


bool is_running = false;
float delta_time = 0;
int previous_frame_time = 0;

void setup(void) {
	set_render_method(RENDER_FILL_TRIANGLE);
	set_cull_method(CULL_NONE);

	// Initialize the scene light direction
	init_light(vec3_new(0, 0, 1));

	// Initialize projection matrix
	int window_width = get_window_width();
	int window_height = get_window_height();
	float aspectx = (float)window_width / (float)window_height;
	float aspecty = (float)window_height / (float)window_width;
	float fovy = M_PI / 3.0; // 60° in radians
	float fovx = 2.0 * atan(tan(fovy / 2) * aspectx);

	
	float znear = 0.1;
	float zfar = 100.0;
	proj_matrix = mat4_make_perspective(fovy, aspecty, znear, zfar);

	// Initialize frustum planes with a point and a normal
	init_frustum_planes(fovx, fovy, znear, zfar);

	load_mesh("./assets/f22.obj", "./assets/f22.png", vec3_new(1, 1, 1), vec3_new(0, 0, 0), vec3_new(-3, 0, 5));
	load_mesh("./assets/efa.obj", "./assets/efa.png", vec3_new(1, 1, 1), vec3_new(0, 0, 0), vec3_new(+3, 0, 5));
}

void handle_input(void) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				is_running = false;
				break;
			}
			if (event.key.keysym.sym == SDLK_1)
			{
				set_render_method(RENDER_WIRE_VERTEX);
				break;
			}
			if (event.key.keysym.sym == SDLK_2)
			{
				set_render_method(RENDER_WIRE);
				break;
			}
			if (event.key.keysym.sym == SDLK_3)
			{
				set_render_method(RENDER_FILL_TRIANGLE);
				break;
			}
			if (event.key.keysym.sym == SDLK_4)
			{
				set_render_method(RENDER_FILL_TRIANGLE_WIRE);
				break;
			}
			if (event.key.keysym.sym == SDLK_5)
			{
				set_render_method(RENDER_TEXTURED);
				break;
			}
			if (event.key.keysym.sym == SDLK_6)
			{
				set_render_method(RENDER_TEXTURED_WIRED);
				break;
			}
			if (event.key.keysym.sym == SDLK_c)
			{
				set_cull_method(CULL_BACKFACE);
				break;
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				set_cull_method(CULL_NONE);
				break;
			}
			if (event.key.keysym.sym == SDLK_UP)
			{
				set_camera_forward_velocity(vec3_mul(get_camera_direction(), 5.0 * delta_time));
				update_camera_position();
				break;
			}
			if (event.key.keysym.sym == SDLK_DOWN)
			{
				set_camera_forward_velocity(vec3_mul(get_camera_direction(), -5.0 * delta_time));
				update_camera_position();
				break;
			}
			if (event.key.keysym.sym == SDLK_LEFT)
			{
				add_camera_yaw(-1.0 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_RIGHT)
			{
				add_camera_yaw(1.0 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_w) {
				add_camera_pitch(3.0 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_s) {
				add_camera_pitch(-3.0 * delta_time);
				break;
			}
			break;
		default:
			break;
		}
	}
}

void process_graphics_pipeline_stages(mesh_t* mesh) {

	// Create the view matrix
	vec3_t target = get_camera_target();
	vec3_t up_direction = { 0, 1, 0 };

	view_matrix = mat4_look_at(get_camera_position(), target, up_direction);

	// Create a scale matrix that will be used to multiply the mesh vertices
	mat4_t scale_matrix = mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);
	mat4_t translation_matrix = mat4_make_translation(
		mesh->translation.x, mesh->translation.y, mesh->translation.z
	);
	mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh->rotation.x);
	mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh->rotation.y);
	mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh->rotation.z);

	// Loop all triangle faces of our mesh
	int num_faces = array_length(mesh->faces);
	for (int i = 0; i < num_faces; i++) {
		face_t mesh_face = mesh->faces[i];

		vec3_t face_vertices[3];
		face_vertices[0] = mesh->vertices[mesh_face.a];
		face_vertices[1] = mesh->vertices[mesh_face.b];
		face_vertices[2] = mesh->vertices[mesh_face.c];

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
			transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

			// Save transformed vertex in the array of transformed vertices
			transformed_vertices[j] = transformed_vertex;
		}

		// Calculate the triangle face normal
		vec3_t face_normal = get_triangle_normal(transformed_vertices);

		if (is_cull_backface()) {
			// Find the vector between vertex A in the triangle and the camera origin
			vec3_t camera_ray = vec3_sub(
				vec3_new(0, 0, 0), vec3_from_vec4(transformed_vertices[0])
			);

			// Calculate how aligned the camera ray is with the face normal (using dot product)
			float dot_normal_camera = vec3_dot(face_normal, camera_ray);

			// Bypass the triangles that are looking away from the camera
			if (dot_normal_camera < 0) {
				continue;
			}
		}

		polygon_t polygon = create_polygon_from_triangle(
			vec3_from_vec4(transformed_vertices[0]),
			vec3_from_vec4(transformed_vertices[1]),
			vec3_from_vec4(transformed_vertices[2]),
			mesh_face.a_uv,
			mesh_face.b_uv,
			mesh_face.c_uv
		);

		clip_polygon(&polygon);

		triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
		int num_triangles_after_clipping = 0;

		triangles_from_polygon(
			&polygon, triangles_after_clipping, &num_triangles_after_clipping
		);

		// Loop all of the assembled triangles after clipping
		for (int t = 0; t < num_triangles_after_clipping; t++) {

			triangle_t triangle_after_clipping = triangles_after_clipping[t];

			// Loop all three vertices to perform projection
			vec4_t projected_points[3];
			for (int j = 0; j < 3; j++) {
				// Project the current vertex
				projected_points[j] = mat4_mul_vec4_project(proj_matrix, triangle_after_clipping.points[j]);

				// Scale and translate the projected points to the middle of the screen
				projected_points[j].x *= (get_window_width() / 2.0);
				projected_points[j].y *= (get_window_height() / 2.0);

				// Invert y axis to use screen coordinates
				projected_points[j].y *= -1;

				projected_points[j].x += (get_window_width() / 2.0);
				projected_points[j].y += (get_window_height() / 2.0);
			}

			// Calculate color from flat shading
			float lambert_factor = -vec3_dot(face_normal, get_light_direction());
			uint32_t triangle_color = light_apply_intensity(mesh_face.color, lambert_factor);

			triangle_t triangle_to_render = {
				.points = {
					{ projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
					{ projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
					{ projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w }
				},
				.texcoords = {
					{ triangles_after_clipping->texcoords[0].u, triangles_after_clipping->texcoords[0].v },
					{ triangles_after_clipping->texcoords[1].u, triangles_after_clipping->texcoords[1].v },
					{ triangles_after_clipping->texcoords[2].u, triangles_after_clipping->texcoords[2].v },
				},
				.color = triangle_color,
				.texture = mesh->texture
			};

			if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
				// Save the projected triangle in the array of triangles to render
				triangles_to_render[num_triangles_to_render] = triangle_to_render;
				num_triangles_to_render++;
			}
		}
	}	// end of for loop all triangle faces of our mesh
}

void update(void) {

	// Get delta time in seconds
	delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0;
	int time_to_wait = FRAME_TARGET_TIME - delta_time;

	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
		SDL_Delay(time_to_wait);
	}

	previous_frame_time = SDL_GetTicks();

	// initialize counter of triangles to render
	num_triangles_to_render = 0;

	for (int mesh_idx = 0; mesh_idx < get_num_meshes(); mesh_idx++) {
		mesh_t* mesh = get_mesh_ptr(mesh_idx);

		// mesh.rotation.x += 0.005;
		// mesh.rotation.y += 0.005;
		// mesh.rotation.z += 0.01;

		// mesh.scale.x += 0.002;
		// mesh.scale.y += 0.001;

		// mesh.translation.y += 0.01;
		// Translate the vertices away from the camera
		// mesh->translation.z = 5.0;

		// Process the graphics pipeline stages for every mesh of our 3D scene
		process_graphics_pipeline_stages(mesh);
	}
}


void render(void) {
	
	clear_color_buffer(0xFF000000);
	clear_z_buffer();
	
	draw_grid();

	for (int i = 0; i < num_triangles_to_render; i++) {
		triangle_t triangle = triangles_to_render[i];

		if (should_render_filled_triangles()) {
			// draw fill triangle
			draw_filled_triangle(
				triangle.points[0].x,
				triangle.points[0].y,
				triangle.points[0].z,
				triangle.points[0].w,
				triangle.points[1].x,
				triangle.points[1].y,
				triangle.points[1].z,
				triangle.points[1].w,
				triangle.points[2].x,
				triangle.points[2].y,
				triangle.points[2].z,
				triangle.points[2].w,
				triangle.color
			);
		}

		if (should_render_textured_triangles()) {
			draw_textured_triangle(
				triangle.points[0].x,
				triangle.points[0].y,
				triangle.points[0].z,
				triangle.points[0].w,
				triangle.texcoords[0].u,
				triangle.texcoords[0].v,
				triangle.points[1].x,
				triangle.points[1].y,
				triangle.points[1].z,
				triangle.points[1].w,
				triangle.texcoords[1].u,
				triangle.texcoords[1].v,
				triangle.points[2].x,
				triangle.points[2].y,
				triangle.points[2].z,
				triangle.points[2].w,
				triangle.texcoords[2].u,
				triangle.texcoords[2].v,
				triangle.texture
			);
		}
		
		if (should_render_wireframe()) {
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
		
		if (should_render_wire_vertex()) {
			int size = 6;
			draw_rect(triangle.points[0].x, triangle.points[0].y - size / 2, size, size, 0xFFFF0000);
			draw_rect(triangle.points[1].x, triangle.points[1].y - size / 2, size, size, 0xFFFF0000);
			draw_rect(triangle.points[2].x, triangle.points[2].y - size / 2, size, size, 0xFFFF0000);
		}
	}

	render_color_buffer();
}

void free_resources(void) {
	free_meshes();
	destroy_window();
}

int main(int argc, char* argv[]) {
	is_running = initialize_window();

	setup();

	while (is_running) {
		handle_input();
		update();
		render();
	}

	
	free_resources();
	
	return 0;
}