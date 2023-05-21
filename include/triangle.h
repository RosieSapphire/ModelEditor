#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vertex.h"

struct triangle {
	Vertex verts[3];
};

void triangle_init_default(struct triangle *t);
void triangles_calc_normals(struct triangle *tris, int num_tris);

#endif