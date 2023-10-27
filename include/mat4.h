#ifndef _MAT4_H_
#define _MAT4_H_

#include "vector.h"

#define DEG_TO_RAD 0.01745329251994329577f
#define RAD_TO_DEG 57.29577951308232087680f

void mat4_zero(float m[4][4]);
void mat4_identity(float m[4][4]);
void mat4_perspective(float m[4][4], float fovDeg, float aspect,
		      float near, float far);
void mat4_ortho(float m[4][4], float l, float r, float t, float b);
void mat4_translate(float m[4][4], float v[3]);
void mat4_scale(float m[4][4], float v[3]);
void mat4_scale_uni(float m[4][4], float s);
void mat4_rotate(float m[4][4], float axis[3], float radang);
void mat4_multiply(float a[4][4], float b[4][4], float out[4][4]);
void mat4_from_quat(float q[4], float out[4][4]);
void mat4_lookat(float m[4][4], float eye[3], float focus[3], float up[3]);
void mat4_print(float m[4][4]);

#endif
