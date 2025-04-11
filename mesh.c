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

void load_obj_file(char* filename) {
  FILE* fp;
  fopen_s(&fp, filename, "r");
  char line[1024];

  tex2_t* tex_coords = NULL;

  while (fgets(line, 1024, fp)) {
    // Vertex position
    if (strncmp(line, "v ", 2) == 0) {
      vec3_t v;
      sscanf_s(line, "v %f %f %f", &v.x, &v.y, &v.z);
      array_push(mesh.vertices, v);
    }

    // Texture coordinate
    if (strncmp(line, "vt ", 3) == 0) {
      tex2_t tex_coord;
      sscanf_s(line, "vt %f %f", &tex_coord.u, &tex_coord.v);
      array_push(tex_coords, tex_coord);
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
        .a = vertex_indices[0] - 1,
        .b = vertex_indices[1] - 1,
        .c = vertex_indices[2] - 1,
        .a_uv = tex_coords[texture_indices[0] - 1],
        .b_uv = tex_coords[texture_indices[1] - 1],
        .c_uv = tex_coords[texture_indices[2] - 1],
        .color = 0xFFFFFFFF
      };
      array_push(mesh.faces, f);
    }
  }

  array_free(tex_coords);
  fclose(fp);
}