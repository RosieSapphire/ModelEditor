#include "shader.h"

GLuint shaderLoad(void)
{
	GLuint s = glCreateProgram();

	const char *vertSrc = 
		"#version 330\n"
		"layout(location = 0) in vec3 a_pos;\n"
		"layout(location = 1) in vec2 a_uv;\n"
		"layout(location = 2) in vec3 a_norm;\n"
		"uniform mat4 proj;\n"
		"uniform mat4 view;\n"
		"out vec3 norm;\n"
		"void main() {\n"
		"gl_Position = proj * view * vec4(a_pos, 1.0);\n"
		"norm = a_norm;\n"
		"}\n";

	const char *fragSrc = 
		"#version 330\n"
		"in vec3 norm;\n"
		"out vec4 frag_col;\n"
		"void main() {\n"
			"frag_col = vec4(abs(norm), 1.0);\n"
		"}\n";

	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vert, 1, &vertSrc, NULL);
	glShaderSource(frag, 1, &fragSrc, NULL);
	glCompileShader(vert);
	glCompileShader(frag);

	int vert_stat, frag_stat;
	glGetShaderiv(vert, GL_COMPILE_STATUS, &vert_stat);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &frag_stat);

	if(!vert_stat) {
		char log[512];
		glGetShaderInfoLog(vert, 512, NULL, log);
		printf("%s\n", log);
		return 1;
	}

	if(!frag_stat) {
		char log[512];
		glGetShaderInfoLog(frag, 512, NULL, log);
		printf("%s\n", log);
		return 1;
	}

	glAttachShader(s, vert);
	glAttachShader(s, frag);
	glLinkProgram(s);
	glDeleteShader(vert);
	glDeleteShader(frag);

	return s;
}

void shaderUse(GLuint s)
{
	glUseProgram(s);
}

void shaderUniformMat4(GLuint s, const char *uni, const float val[4][4])
{
	glUniformMatrix4fv(glGetUniformLocation(s, uni), 1, GL_FALSE,
		    (const float *)val);
}

void shaderUniform3f(GLuint s, const char *uni, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(s, uni), x, y, z);
}

void shaderUnload(GLuint s) {
	glDeleteProgram(s);
}
