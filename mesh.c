#include <stdio.h>
#include <string.h>
#include "array.h"
#include "mesh.h"

mesh_t mesh = {
  .vertices = NULL,
  .faces = NULL,
  .scale = {1, 1, 1},
  .rotation = { 0, 0, 0 },
  .translation = {0, 0, 0}
};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    {.x = -1, .y = -1, .z = -1 }, // 1
    {.x = -1, .y = 1, .z = -1 }, // 2
    {.x = 1, .y = 1, .z = -1 }, // 3
    {.x = 1, .y = -1, .z = -1 }, // 4
    {.x = 1, .y = 1, .z = 1 }, // 5
    {.x = 1, .y = -1, .z = 1 }, // 6
    {.x = -1, .y = 1, .z = 1 }, // 7
    {.x = -1, .y = -1, .z = 1 }  // 8
};

face_t cube_faces[N_CUBE_FACES] = {
  // front
  {.a = 1, .b = 2, .c = 3, .color = 0xFFFFFFFF },
  {.a = 1, .b = 3, .c = 4, .color = 0xFFFFFFFF },
  // right
  {.a = 4, .b = 3, .c = 5, .color = 0xFFFFFFFF },
  {.a = 4, .b = 5, .c = 6, .color = 0xFFFFFFFF },
  // back
  {.a = 6, .b = 5, .c = 7, .color = 0xFFFFFFFF },
  {.a = 6, .b = 7, .c = 8, .color = 0xFFFFFFFF},
  // left
  {.a = 8, .b = 7, .c = 2, .color = 0xFFFFFFFF },
  {.a = 8, .b = 2, .c = 1, .color = 0xFFFFFFFF },
  // top
  {.a = 2, .b = 7, .c = 5, .color = 0xFFFFFFFF },
  {.a = 2, .b = 5, .c = 3, .color = 0xFFFFFFFF },
  // bottom
  {.a = 6, .b = 8, .c = 1, .color = 0xFFFFFFFF },
  {.a = 6, .b = 1, .c = 4, .color = 0xFFFFFFFF }
};

void load_cube_mesh_data(void) {
  for (int i = 0; i < N_CUBE_VERTICES; i++) {
    vec3_t cube_vertex = cube_vertices[i];
    array_push(mesh.vertices, cube_vertex);
  }

  for (int j = 0; j < N_CUBE_FACES; j++) {
    face_t cube_face = cube_faces[j];
    array_push(mesh.faces, cube_face);
  }
}

void load_obj_file(char* filename) {
  FILE* fp;
  fopen_s(&fp, filename, "r");
  char line[1024];
  while (fgets(line, 1024, fp)) {
    // Vertex position
    if (strncmp(line, "v ", 2) == 0) {
      vec3_t v;
      sscanf_s(line, "v %f %f %f", &v.x, &v.y, &v.z);
      array_push(mesh.vertices, v);
    }

    // Face indices
    if (strncmp(line, "f ", 2) == 0) {
      int vertex_indices[3];
      int texture_indices[3];
      int normal_indices[3];
      sscanf_s(
        line,
        "f %d/%d/%d %d/%d/%d %d/%d/%d",
        &vertex_indices[0], &texture_indices[0], &normal_indices[0],
        &vertex_indices[1], &texture_indices[1], &normal_indices[1],
        &vertex_indices[2], &texture_indices[2], &normal_indices[2]
      );
      face_t f = {
        .a = vertex_indices[0],
        .b = vertex_indices[1],
        .c = vertex_indices[2],
        .color = 0xFFFFFFFF
      };
      array_push(mesh.faces, f);
    }
  }
  fclose(fp);
}