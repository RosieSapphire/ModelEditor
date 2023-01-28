#include "vec3.h"

#include <math.h>

void Vec3Copy(Vec3 x, Vec3 dest)
{
	for(int i = 0; i < 3; i++) {
		dest[i] = x[i];
	}
}

void Vec3Add(Vec3 a, Vec3 b, Vec3 out)
{
	for(int i = 0; i < 3; i++) {
		out[i] = a[i] + b[i];
	}
}

void Vec3Subtract(Vec3 a, Vec3 b, Vec3 out)
{
	for(int i = 0; i < 3; i++) {
		out[i] = a[i] - b[i];
	}
}

void Vec3Scale(Vec3 x, float s)
{
	x[0] *= s;
	x[1] *= s;
	x[2] *= s;
}

float Vec3Dot(Vec3 a, Vec3 b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void Vec3Cross(Vec3 a, Vec3 b, Vec3 out)
{
	out[0] = (a[1] * b[2]) - (a[2] * b[1]);
	out[1] = (a[2] * b[0]) - (a[0] * b[2]);
	out[2] = (a[0] * b[1]) - (a[1] * b[0]);
}

void Vec3Normalize(Vec3 x)
{
	float mag = sqrtf(Vec3Dot(x, x));
	Vec3Scale(x, 1.0f / mag);
}
