#ifndef _VEC3_H_
#define _VEC3_H_

typedef float Vec3[3];

void Vec3Copy(Vec3 x, Vec3 dest);
void Vec3Add(Vec3 a, Vec3 b, Vec3 out);
void Vec3Subtract(Vec3 a, Vec3 b, Vec3 out);
void Vec3Scale(Vec3 x, float s);
float Vec3Dot(Vec3 a, Vec3 b);
void Vec3Cross(Vec3 a, Vec3 b, Vec3 out);
void Vec3Normalize(Vec3 x);

#endif
