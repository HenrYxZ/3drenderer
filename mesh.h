#ifndef MESH_H
#define MESH_H

#include "triangle.h"
#include "vector.h"

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6 * 2)

extern vec3_t cube_vertices[N_CUBE_VERTICES];
extern face_t cube_faces[N_CUBE_FACES];

typedef struct {
	vec3_t* vertices;		// dynamic array of vertices
	face_t* faces;			// dynamic array of faces
	vec3_t scale;
	vec3_t rotation;		// rotation with x, y, z values
	vec3_t translation;
} mesh_t;

extern mesh_t mesh;

void load_obj_file(char* filename);

#endif // !MESH_H