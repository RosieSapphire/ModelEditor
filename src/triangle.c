#include <string.h>

#include "vector.h"
#include "triangle.h"

void triangle_init_default(triangle_t *t)
{
	vertex_t verts[3] = {
		{{-0.5f, 0.0f, -0.5f}, {0.0f, 0.0f}, {0, 1, 0}},
		{{ 0.5f, 0.0f, -0.5f}, {1.0f, 0.0f}, {0, 1, 0}},
		{{ 0.0f, 0.0f,  0.5f}, {0.5f, 1.0f}, {0, 1, 0}},
	};

	for(int i = 0; i < 3; i++) {
		vector_copy(verts[i].pos,  t->verts[i].pos,  3);
		vector_copy(verts[i].uv,   t->verts[i].uv,   2);
		vector_copy(verts[i].norm, t->verts[i].norm, 3);
	}
}

/*
void triangles_calc_normals(triangle_t *tris, int num_tris)
{
	for(int i = 0; i < num_tris; i++) {
		triangle_t *t = tris + i;

		float u[3], v[3], norm[3];
		vector_subtract(t->verts[1].pos, t->verts[0].pos, u, 3);
		vector_subtract(t->verts[2].pos, t->verts[0].pos, v, 3);
		vector_normalize(u, 3);
		vector_normalize(v, 3);
		vector3_cross(u, v, norm);
		vector_normalize(norm, 3);

		for(int j = 0; j < 3; j++) {
			vector_negate(norm, 3);
			vector_copy(norm, t->verts[j].norm, 3);
		}
	}
}
*/
