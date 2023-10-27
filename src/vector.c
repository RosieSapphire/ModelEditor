#include <math.h>

#include "vector.h"

void vector_copy(float *x, float *dst, int size)
{
	for(int i = 0; i < size; i++)
		dst[i] = x[i];
}

void vector_add(float *a, float *b, float *dst, int size)
{
	for(int i = 0; i < size; i++)
		dst[i] = a[i] + b[i];
}

void vector_subtract(float *a, float *b, float *dst, int size)
{
	for(int i = 0; i < size; i++)
		dst[i] = a[i] - b[i];
}

void vector_scale(float *x, float s, int size)
{
	for(int i = 0; i < size; i++)
		x[i] *= s;
}

float vector_dot(float *a, float *b, int size)
{
	float dot = 0.0f;
	for(int i = 0; i < size; i++)
		dot += a[i] * b[i];

	return dot;
}

void vector3_cross(float *a, float *b, float *dst)
{
	dst[0] = (a[1] * b[2]) - (a[2] * b[1]);
	dst[1] = (a[2] * b[0]) - (a[0] * b[2]);
	dst[2] = (a[0] * b[1]) - (a[1] * b[0]);
}

void vector_normalize(float *x, int size)
{
	float mag = sqrtf(vector_dot(x, x, size));
	vector_scale(x, 1.0f / mag, size);
}

void vector_negate(float *x, int size)
{
	vector_scale(x, -1.0f, size);
}
