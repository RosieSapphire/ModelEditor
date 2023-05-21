#include "triangle.h"

#include <string.h>

void triangle_init_default(struct triangle *t)
{
	Vec3 verts[3] = {
		{-0.5f, -0.5f, 0.0f},
		{ 0.5f, -0.5f, 0.0f},
		{ 0.0f,  0.5f, 0.0f},
	};

	for(int i = 0; i < 3; i++) {
		Vec3Copy(verts[i], t->verts[i].pos);
		memset(t->verts[i].norm, 0, sizeof(Vec3));
	}
}

void triangles_calc_normals(struct triangle *tris, int num_tris)
{
	for(int i = 0; i < num_tris; i++) {
		struct triangle *t = tris + i;

		Vec3 u, v;
		Vec3Subtract(t->verts[1].pos, t->verts[0].pos, u);
		Vec3Subtract(t->verts[2].pos, t->verts[0].pos, v);
		Vec3Normalize(u);
		Vec3Normalize(v);

		Vec3 norm;
		Vec3Cross(u, v, norm);
		Vec3Normalize(norm);

		for(int j = 0; j < 3; j++) {
			Vec3Copy(norm, t->verts[j].norm);
		}
	}
}