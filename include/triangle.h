#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vertex.h"

typedef struct {
	vertex_t verts[3];
} triangle_t;

void triangle_init_default(triangle_t *t);
// void triangles_calc_normals(triangle_t *tris, int num_tris);

#endif
