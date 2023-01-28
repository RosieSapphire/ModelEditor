#ifndef _SHADER_H_
#define _SHADER_H_

#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "mat4.h"

GLuint shaderLoad(void);
void shaderUse(GLuint s);
void shaderUniformMat4(GLuint s, const char *uni, Mat4 val);
void shaderUniform3f(GLuint s, const char *uni, float x, float y, float z);
void shaderUnload(GLuint s);

#endif
