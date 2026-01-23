#ifndef MESH_H
#define MESH_H

#include "triangle.h"
#include "vector.h"
#include "upng.h"

typedef struct {
	vec3_t* vertices;		// dynamic array of vertices
	face_t* faces;			// dynamic array of faces
	upng_t* texture;
	vec3_t scale;
	vec3_t rotation;		// rotation with x, y, z values
	vec3_t translation;
} mesh_t;

extern mesh_t mesh;

void load_mesh(
	char* obj_filename,
	char* png_filename,
	vec3_t scale,
	vec3_t rotation,
	vec3_t translation
);
void load_obj_file(char* filename);
void load_obj_png_data(char* png_filename);
int get_num_meshes();
mesh_t* get_mesh_ptr(int index);
void free_meshes();


#endif // !MESH_H