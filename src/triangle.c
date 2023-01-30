#include "triangle.h"

#include <string.h>

void TrianglesCalculateNormals(Triangle *tris, int numTris)
{
	for(int i = 0; i < numTris; i++) {
		Triangle *t = tris + i;

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

void TriangleInitDefault(Triangle *t)
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
