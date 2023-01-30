#include "mat4.h"

#include <string.h>
#include <math.h>
#include <stdio.h>

void Mat4Zero(Mat4 m)
{
	memset(m, 0, sizeof(Mat4));
}

void Mat4Identity(Mat4 m)
{
	Mat4Zero(m);
	for(int i = 0; i < 4; i++) {
		m[i][i] = 1;
	}
}

void Mat4Perspective(Mat4 m, float fovDeg, float aspect, float near, float far)
{
	Mat4Zero(m);

	float fov_itan = 1.0f / tanf(fovDeg * DEG_TO_RAD * 0.5f);
	m[0][0] = fov_itan / aspect;
	m[1][1] = fov_itan;

	float z_range = 1.0f / (near - far);
	m[2][2] = (near + far) * z_range;
	m[2][3] = -1.0f;
	m[3][2] = 2.0f * near * far * z_range;
}

void Mat4Ortho(Mat4 m, float l, float r, float t, float b)
{
	Mat4Zero(m);

	float rl = 1 / (r - l);
	float tb = 1 / (t - b);
	m[0][0] = 2 * rl;
	m[1][1] = 2 * tb;
	m[2][2] = -1;
	m[3][0] = -(r + l) * rl;
	m[3][1] = -(t + b) * tb;
	m[3][2] = 0;
	m[3][3] = 1;
}

void Mat4TranslateX(Mat4 m, float x)
{
	m[3][0] += x;
}

void Mat4TranslateY(Mat4 m, float y)
{
	m[3][1] += y;
}

void Mat4TranslateZ(Mat4 m, float z)
{
	m[3][2] += z;
}

void Mat4Translate(Mat4 m, Vec3 v)
{
	Mat4TranslateX(m, v[0]);
	Mat4TranslateY(m, v[1]);
	Mat4TranslateZ(m, v[2]);
}

void Mat4Scale(Mat4 m, Vec3 v)
{
	m[0][0] *= v[0];
	m[1][1] *= v[1];
	m[2][2] *= v[2];
}

void Mat4ScaleUni(Mat4 m, float s)
{
	Vec3 sv = {s, s, s};
	Mat4Scale(m, sv);
}

void Mat4Rotate(Mat4 m, Vec3 axis, float angleRad)
{
	const float angleSin = sinf(angleRad);
	const float angleCos = cosf(angleRad);

	Mat4 rot;
	Mat4Zero(rot);

	rot[0][0] = angleCos + (axis[0] * axis[0]) * (1 - angleCos);
	rot[1][0] = axis[0] * axis[1] * (1 - angleCos) - axis[2] * angleSin;
	rot[2][0] = axis[0] * axis[2] * (1 - angleCos) + axis[1] * angleSin;

	rot[0][1] = axis[1] * axis[0] * (1 - angleCos) + axis[2] * angleSin;
	rot[1][1] = angleCos + (axis[1] * axis[1]) * (1 - angleCos);
	rot[2][1] = axis[1] * axis[2] * (1 - angleCos) - axis[0] * angleSin;

	rot[0][2] = axis[2] * axis[0] * (1 - angleCos) - axis[1] * angleSin;
	rot[1][2] = axis[2] * axis[1] * (1 - angleCos) + axis[0] * angleSin;
	rot[2][2] = angleCos + (axis[2] * axis[2]) * (1 - angleCos);

	rot[3][3] = 1.0f;

	Mat4Multiply(rot, m, m);
}

void Mat4RotateX(Mat4 m, float angleRad)
{
	Vec3 axis = {1, 0, 0};
	Mat4Rotate(m, axis, angleRad);
}

void Mat4RotateY(Mat4 m, float angleRad)
{
	Vec3 axis = {0, 1, 0};
	Mat4Rotate(m, axis, angleRad);
}

void Mat4RotateZ(Mat4 m, float angleRad)
{
	Vec3 axis = {0, 0, 1};
	Mat4Rotate(m, axis, angleRad);
}

void Mat4Multiply(Mat4 a, Mat4 b, Mat4 out)
{
	Mat4 tmp;
	Mat4Zero(tmp);
	for(int row = 0; row < 4; row++) {
		for(int col = 0; col < 4; col++) {
			for(int ind = 0; ind < 4; ind++) {
				tmp[col][row] += a[ind][row] * b[col][ind];
			}
		}
	}

	memcpy(out, tmp, sizeof(Mat4));
}

void Mat4FromQuaternion(Vec4 q, Mat4 out)
{
	Mat4Zero(out);

	float norm = sqrtf(Vec4Dot(q, q));
	float s = 0.0f;
	if(norm > 0) {
 		s = (2.0f / norm);
	}

	out[0][0] = 1.0f - (s * q[1] * q[1]) - (s * q[2] * q[2]);
	out[0][1] = (s * q[0] * q[1]) + (s * q[3] * q[2]);
	out[0][2] = (s * q[0] * q[2]) - (s * q[3] * q[1]),
	out[0][3] = 0.0f;

	out[1][0] = (s * q[0] * q[1]) - (s * q[3] * q[2]);
	out[1][1] = 1.0f - (s * q[0] * q[0]) - (s * q[2] * q[2]);
	out[1][2] = (s * q[1] * q[2]) + (s * q[3] * q[0]);
	out[1][3] = 0.0f;

	out[2][0] = (s * q[0] * q[2]) + (s * q[3] * q[1]),
	out[2][1] = (s * q[1] * q[2]) - (s * q[3] * q[0]),
	out[2][2] = 1.0f - (s * q[0] * q[0]) - (s * q[1] * q[1]),
	out[2][3] = 0.0f,

	out[3][0] = 0.0f;
	out[3][1] = 0.0f;
	out[3][2] = 0.0f;
	out[3][3] = 1.0f;
}                    
                     
void Mat4LookAt(Mat4 m, Vec3 eye, Vec3 focus)
{
	Mat4Zero(m);

	Vec3 f;
	Vec3Subtract(focus, eye, f);
	Vec3Normalize(f);

	Vec3 s;
	Vec3 up = {0, 1, 0};
	Vec3Cross(up, f, s);
	Vec3Normalize(s);

	Vec3 u;
	Vec3Cross(f, s, u);

	m[0][0] =  s[0];
	m[0][1] =  u[0];
	m[0][2] = -f[0];
	m[0][3] = 0.0f;

	m[1][0] =  s[1];
	m[1][1] =  u[1];
	m[1][2] = -f[1];
	m[1][3] = 0.0f;

	m[2][0] = s[2];
	m[2][1] = u[2];
	m[2][2] = f[2];
	m[2][3] = 0.0f;

	m[3][0] = s[0] * eye[0] + s[1] * eye[1] + s[2] * eye[2];
	m[3][1] = u[0] * eye[0] + u[1] * eye[1] + u[2] * eye[2];
	m[3][2] = f[0] * eye[0] + f[1] * eye[1] + f[2] * eye[2];
	m[3][3] = 1.0f;
}

void Mat4Print(Mat4 m)
{
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			printf("%.6f\t", m[j][i]);
		}
		printf("\n");
	}
	printf("\n");
}
