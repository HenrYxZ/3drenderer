#include <stdio.h>
#include <string.h>
#include "array.h"
#include "mesh.h"

#define MAX_NUM_MESHES 10

static mesh_t meshes[MAX_NUM_MESHES];
static int mesh_count = 0;

void load_mesh(char* obj_filename, char* png_filename, vec3_t scale, vec3_t rotation, vec3_t translation)
{
  // Load the OBJ file to our mesh
  load_obj_file(obj_filename);

  // Load the PNG file info
  load_obj_png_data(png_filename);

  // Initialize scale, translation and rotation with the parameters
  meshes[mesh_count].scale = scale;
  meshes[mesh_count].rotation = rotation;
  meshes[mesh_count].translation = translation;

  // Add the nre mesh to the array of meshes
  mesh_count++;
}

void load_obj_file(char* filename) {
  mesh_t* mesh = &meshes[mesh_count];
  FILE* fp;
  fopen_s(&fp, filename, "r");
  char line[1024];

  tex2_t* tex_coords = NULL;

  while (fgets(line, 1024, fp)) {
    // Vertex position
    if (strncmp(line, "v ", 2) == 0) {
      vec3_t v;
      sscanf_s(line, "v %f %f %f", &v.x, &v.y, &v.z);
      array_push(mesh->vertices, v);
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
      array_push(mesh->faces, f);
    }
  }

  array_free(tex_coords);
  fclose(fp);
}

void load_obj_png_data(char* filename)
{
  upng_t* png_image = upng_new_from_file(filename);
  if (png_image != NULL) {
    upng_decode(png_image);
    if (upng_get_error(png_image) == UPNG_EOK) {
      meshes[mesh_count].texture = png_image;
    }
  }
}

int get_num_meshes()
{
  return mesh_count;
}

mesh_t* get_mesh_ptr(int index)
{
  if (index < 0 || index >= mesh_count)
    return NULL;
  return &meshes[index];
}

void free_meshes()
{
  for (int i = 0; i < mesh_count; i++) {
    upng_free(meshes[i].texture);
    array_free(meshes[i].faces);
    array_free(meshes[i].vertices);
  }
}
