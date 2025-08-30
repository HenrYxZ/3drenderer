#include "clipping.h"

#define NUM_PLANES 6

plane_t frustrum_planes[NUM_PLANES];

void init_frustrum_planes(float fov, float z_near, float z_far)
{
	vec3_t origin = { 0, 0, 0 };
	
	frustrum_planes[LEFT_FRUSTRUM_PLANE].point = origin;
	frustrum_planes[LEFT_FRUSTRUM_PLANE].normal.x = cos(fov / 2);
	frustrum_planes[LEFT_FRUSTRUM_PLANE].normal.y = 0;
	frustrum_planes[LEFT_FRUSTRUM_PLANE].normal.z = sin(fov / 2);

	frustrum_planes[RIGHT_FRUSTRUM_PLANE].point = origin;
	frustrum_planes[RIGHT_FRUSTRUM_PLANE].normal.x = -cos(fov / 2);
	frustrum_planes[RIGHT_FRUSTRUM_PLANE].normal.y = 0;
	frustrum_planes[RIGHT_FRUSTRUM_PLANE].normal.z = sin(fov / 2);

	frustrum_planes[TOP_FRUSTRUM_PLANE].point = origin;
	frustrum_planes[TOP_FRUSTRUM_PLANE].normal.x = 0;
	frustrum_planes[TOP_FRUSTRUM_PLANE].normal.y = -cos(fov / 2);
	frustrum_planes[TOP_FRUSTRUM_PLANE].normal.z = sin(fov / 2);

	frustrum_planes[BOTTOM_FRUSTRUM_PLANE].point = origin;
	frustrum_planes[BOTTOM_FRUSTRUM_PLANE].normal.x = 0;
	frustrum_planes[BOTTOM_FRUSTRUM_PLANE].normal.y = cos(fov / 2);
	frustrum_planes[BOTTOM_FRUSTRUM_PLANE].normal.z = sin(fov / 2);

	frustrum_planes[NEAR_FRUSTRUM_PLANE].point = vec3_new(0, 0, z_near);
	frustrum_planes[NEAR_FRUSTRUM_PLANE].normal.x = 0;
	frustrum_planes[NEAR_FRUSTRUM_PLANE].normal.y = 0;
	frustrum_planes[NEAR_FRUSTRUM_PLANE].normal.z = 1;

	frustrum_planes[FAR_FRUSTRUM_PLANE].point = vec3_new(0, 0, z_far);
	frustrum_planes[FAR_FRUSTRUM_PLANE].normal.x = 0;
	frustrum_planes[FAR_FRUSTRUM_PLANE].normal.y = 0;
	frustrum_planes[FAR_FRUSTRUM_PLANE].normal.z = -1;
}

polygon_t create_polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2)
{
	polygon_t result = {
		.vertices = { v0, v1, v2 },
		.num_vertices = 3
	};
	return result;
}

void clip_polygon_against_plane(polygon_t* polygon, int plane)
{
	// TODO:
}

void clip_polygon(polygon_t* polygon)
{
	// Clip by each of the planes
	clip_polygon_against_plane(polygon, LEFT_FRUSTRUM_PLANE);
	clip_polygon_against_plane(polygon, RIGHT_FRUSTRUM_PLANE);
	clip_polygon_against_plane(polygon, TOP_FRUSTRUM_PLANE);
	clip_polygon_against_plane(polygon, BOTTOM_FRUSTRUM_PLANE);
	clip_polygon_against_plane(polygon, NEAR_FRUSTRUM_PLANE);
	clip_polygon_against_plane(polygon, FAR_FRUSTRUM_PLANE);
}


