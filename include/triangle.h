#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "vertex.h"

typedef struct {
	Vertex verts[3];
} Triangle;

void TrianglesCalculateNormals(Triangle *tris, int numTris);

#endif
