#ifndef _MAT4_H_
#define _MAT4_H_

#include "vec3.h"
#include "vec4.h"

#define DEG_TO_RAD 0.01745329251994329577f
#define RAD_TO_DEG 57.29577951308232087680f

typedef float Mat4[4][4];

void Mat4Zero(Mat4 m);
void Mat4Identity(Mat4 m);
void Mat4Perspective(Mat4 m, float fovDeg, float aspect, float near, float far);
void Mat4Ortho(Mat4 m, float l, float r, float t, float b);
void Mat4TranslateX(Mat4 m, float x);
void Mat4TranslateY(Mat4 m, float y);
void Mat4TranslateZ(Mat4 m, float z);
void Mat4Translate(Mat4 m, Vec3 v);
void Mat4Scale(Mat4 m, Vec3 v);
void Mat4ScaleUni(Mat4 m, float s);
void Mat4Rotate(Mat4 m, Vec3 axis, float angleRad);
void Mat4RotateX(Mat4 m, float angleRad);
void Mat4RotateY(Mat4 m, float angleRad);
void Mat4RotateZ(Mat4 m, float angleRad);
void Mat4Multiply(Mat4 a, Mat4 b, Mat4 out);
void Mat4FromQuat(Vec4 q);
void Mat4LookAt(Mat4 m, Vec3 eye, Vec3 focus);
void Mat4Print(Mat4 m);

#endif
