#ifndef VECTOR_H_
#define VECTOR_H_

void vector_copy(float *x, float *dst, int size);
void vector_add(float *a, float *b, float *dst, int size);
void vector_subtract(float *a, float *b, float *dst, int size);
void vector_scale(float *x, float s, int size);
float vector_dot(float *a, float *b, int size);
void vector3_cross(float *a, float *b, float *dst);
void vector_normalize(float *x, int size);
void vector_negate(float *x, int size);

#endif /* VECTOR_H_ */
