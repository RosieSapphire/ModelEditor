#include "mat4.h"

#include <string.h>
#include <math.h>
#include <stdio.h>

void mat4_zero(float m[4][4])
{
	memset(m, 0, sizeof(float) * 4 * 4);
}

void mat4_identity(float m[4][4])
{
	mat4_zero(m);
	for(int i = 0; i < 4; i++)
		m[i][i] = 1;
}

void mat4_perspective(float m[4][4], float fov_deg,
		      float aspect, float near, float far)
{
	mat4_zero(m);

	float fov_itan = 1.0f / tanf(fov_deg * DEG_TO_RAD * 0.5f);
	m[0][0] = fov_itan / aspect;
	m[1][1] = fov_itan;

	float z_range = 1.0f / (near - far);
	m[2][2] = (near + far) * z_range;
	m[2][3] = -1.0f;
	m[3][2] = 2.0f * near * far * z_range;
}

void mat4_ortho(float m[4][4], float l, float r, float t, float b)
{
	mat4_zero(m);

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

void mat4_translate(float m[4][4], float v[3])
{
	for(int i = 0; i < 3; i++)
		m[3][i] += v[i];
}

void mat4_scale(float m[4][4], float v[3])
{
	for(int i = 0; i < 3; i++)
		m[i][i] *= v[i];
}

void mat4_scale_uni(float m[4][4], float s)
{
	float sv[3] = {s, s, s};
	mat4_scale(m, sv);
}

void mat4_rotate(float m[4][4], float axis[3], float radang)
{
	const float sinang = sinf(radang);
	const float cosang = cosf(radang);
	float rot[4][4];
	mat4_zero(rot);

	rot[0][0] = cosang + (axis[0] * axis[0]) * (1 - cosang);
	rot[1][0] = axis[0] * axis[1] * (1 - cosang) - axis[2] * sinang;
	rot[2][0] = axis[0] * axis[2] * (1 - cosang) + axis[1] * sinang;

	rot[0][1] = axis[1] * axis[0] * (1 - cosang) + axis[2] * sinang;
	rot[1][1] = cosang + (axis[1] * axis[1]) * (1 - cosang);
	rot[2][1] = axis[1] * axis[2] * (1 - cosang) - axis[0] * sinang;

	rot[0][2] = axis[2] * axis[0] * (1 - cosang) - axis[1] * sinang;
	rot[1][2] = axis[2] * axis[1] * (1 - cosang) + axis[0] * sinang;
	rot[2][2] = cosang + (axis[2] * axis[2]) * (1 - cosang);

	rot[3][3] = 1.0f;

	mat4_multiply(rot, m, m);
}

void mat4_multiply(float a[4][4], float b[4][4], float out[4][4])
{
	float tmp[4][4];
	mat4_zero(tmp);
	for(int row = 0; row < 4; row++)
		for(int col = 0; col < 4; col++)
			for(int ind = 0; ind < 4; ind++)
				tmp[col][row] += a[ind][row] * b[col][ind];

	memcpy(out, tmp, sizeof(float) * 4 * 4);
}

void mat4_from_quat(float q[4], float out[4][4])
{
	mat4_zero(out);

	float norm = sqrtf(vector_dot(q, q, 4));
	float s = 0.0f;
	if(norm > 0)
 		s = (2.0f / norm);

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
                     
void mat4_lookat(float m[4][4], float eye[3], float focus[3], float up[3])
{
	mat4_zero(m);

	float f[3], s[3], u[3];
	vector_subtract(focus, eye, f, 3);
	vector_normalize(f, 3);
	vector3_cross(up, f, s);
	vector_normalize(s, 3);
	vector3_cross(f, s, u);

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

void mat4_print(float m[4][4])
{
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++)
			printf("%.6f\t", m[j][i]);

		printf("\n");
	}
	printf("\n");
}
