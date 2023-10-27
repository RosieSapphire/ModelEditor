#ifndef MESH_H_
#define MESH_H_

#include <stdint.h>

#include "vertex.h"

typedef struct {
	uint32_t vao, vbo, ebo;
	uint16_t num_verts, num_indis;
	vertex_t *verts;
	uint16_t *indis;
} mesh_t;

#endif /* MESH_H_ */
