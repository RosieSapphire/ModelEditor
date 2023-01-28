#include "vec4.h"

float Vec4Dot(Vec4 a, Vec4 b)
{
	float dot = 0.0f;
	for(int i = 0; i < 4; i++) {
		dot += a[i] * b[i];
	}

	return dot;
}
