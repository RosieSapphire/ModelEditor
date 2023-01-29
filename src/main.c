#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>

#include "shader.h"

#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#include "mat4.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

typedef struct {
	Vec3 pos;
	Vec3 norm;
} Vertex;

typedef struct {
	Vertex verts[3];
} Triangle;

int main(void)
{
	struct nk_glfw glfw = {0};
	static GLFWwindow *win;
	struct nk_context *ctx;
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int width, height;
	glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(),
			NULL, NULL, &width, &height);

	float aspectRatio = (float)width / (float)height;

	win = glfwCreateWindow(width, height, "3D Model Editor", NULL, NULL);
	glfwMakeContextCurrent(win);
	
	glViewport(0, 0, width, height);
	glewExperimental = 1;
	glewInit();

	ctx = nk_glfw3_init(&glfw, win, NK_GLFW3_INSTALL_CALLBACKS);
	{
		struct nk_font_atlas *atlas;
		nk_glfw3_font_stash_begin(&glfw, &atlas);
		nk_glfw3_font_stash_end(&glfw);
	}
	
	struct nk_colorf bg = {0.10f, 0.18f, 0.24f, 1.0f};

	int numTris = 1;
	Triangle *tris = calloc(numTris, sizeof(Triangle));
	tris->verts[0].pos[0] = -0.5f;
	tris->verts[0].pos[1] = -0.5f;
	tris->verts[0].pos[2] =  0.0f;

	tris->verts[1].pos[0] =  0.5f;
	tris->verts[1].pos[1] = -0.5f;
	tris->verts[1].pos[2] =  0.0f;

	tris->verts[2].pos[0] =  0.0f;
	tris->verts[2].pos[1] =  0.5f;
	tris->verts[2].pos[2] =  0.0f;

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,
			sizeof(Triangle) * numTris, tris, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT,
			GL_FALSE, sizeof(Vertex),
			(void *)offsetof(Vertex, pos));
	glVertexAttribPointer(1, 3, GL_FLOAT,
			GL_FALSE, sizeof(Vertex),
			(void *)offsetof(Vertex, norm));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	GLuint shader = shaderLoad();

	Mat4 proj;
	Mat4Perspective(proj, 60.0f, aspectRatio, 0.1f, 8.0f);

	float timeLast = glfwGetTime();
	while (!glfwWindowShouldClose(win))
	{
		float timeNow = glfwGetTime();
		float deltaTime = timeNow - timeLast;
		timeLast = timeNow;

		glfwPollEvents();
		if(glfwGetKey(win, GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(win, GLFW_TRUE);

		char vert_list_str[64];
		sprintf(vert_list_str, "Triangle List: (%d)", numTris);

		for(int i = 0; i < numTris; i++) {
			Triangle *tri = tris + i;

			Vec3 u, v;
			Vec3Subtract(tri->verts[1].pos,
					tri->verts[0].pos, u);
			Vec3Subtract(tri->verts[2].pos,
					tri->verts[0].pos, v);
			Vec3Normalize(u);
			Vec3Normalize(v);

			Vec3 norm;
			Vec3Cross(u, v, norm);
			Vec3Normalize(norm);

			for(int j = 0; j < 3; j++) {
				Vec3Copy(norm, tri->verts[j].norm);
			}
		}

		nk_glfw3_new_frame(&glfw);

		if(!nk_begin(ctx, vert_list_str, nk_rect(0, 0, 320, height),
					NK_WINDOW_BORDER |
					NK_WINDOW_TITLE))
			continue;

		nk_layout_row_dynamic(ctx, 30, 2);
		if(nk_button_label(ctx, "Add Triangle")) {
			numTris++;
			tris = realloc(tris, sizeof(Triangle) * numTris);
			memset(tris + (numTris - 1), 0, sizeof(Triangle));
		}

		nk_layout_row_dynamic(ctx, 1024, 1);
		if(nk_group_begin(ctx, "Test", NK_WINDOW_BORDER)) {
			for(int j = 0; j < numTris; j++) {
				char vert_str[32];
				sprintf(vert_str, "Triangle %d", j);
				if(nk_tree_push(ctx, NK_TREE_NODE, vert_str,
							NK_MAXIMIZED)) {
					nk_layout_row_dynamic(ctx, 26, 4);
					for(int k = 0; k < 3; k++) {
					nk_label(ctx, "Position:",
							NK_TEXT_LEFT);
		
					for(int i = 0; i < 3; i++) {
						float *val_cur =
						&tris[j].verts[k].pos[i];
			
						nk_slider_float(ctx, -0.5f,
								val_cur, 0.5f,
								0.1f);
					}
					}

					for(int k = 0; k < 3; k++) {
					nk_label(ctx, "Normal:",
							NK_TEXT_LEFT);
		
					for(int i = 0; i < 3; i++) {
						float *val_cur =
						&tris[j].verts[k].norm[i];
			
						nk_property_float(ctx, "",
								-16, val_cur,
								16, 0.1f, 0);
					}
					}
	
					nk_layout_row_dynamic(ctx, 26, 2);
					if(nk_button_label(ctx, "Delete")) {
						numTris--;
						if(numTris) {
							for(int i = j;
								i < numTris + 1;
									i++) {
								memcpy(tris + i,
									tris
									+ i + 1,
									sizeof(
									Triangle
									));
							}
	
							tris = realloc(tris,
								numTris *
								sizeof(
								Triangle));
						} else {
							memset(tris, 0,
								sizeof(
								Triangle));
							numTris++;
						}
					}

					if(nk_button_label(ctx, "Duplicate")) {
						numTris++;
						tris = realloc(tris,
							numTris *
							sizeof(
							Triangle));

						for(int i = numTris - 1;
								i > j;
								i--) {
							memcpy(tris + i,
								tris + i - 1,
								sizeof(Triangle)
							      );
						}
					}
	
					nk_tree_pop(ctx);
				}
	
			}

			nk_group_end(ctx);
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * numTris,
				tris, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		nk_layout_row_dynamic(ctx, 30, 3);
		if(nk_button_label(ctx, "New Model")) {
			numTris = 1;
			tris = realloc(tris, numTris * sizeof(Triangle));
			memset(tris, 0, sizeof(Triangle));
		}

		char save_txt[32];
		static int save_timer = 0;

		save_timer--;
		if(save_timer < 0) {
			save_timer = 0;
		}

		sprintf(save_txt, !save_timer ?
				"Save Model" : "Model Saved!");
		if(nk_button_label(ctx, save_txt)) {
			FILE *out = fopen("out.mdl", "wb");
			fwrite(tris, sizeof(Triangle), numTris, out);

			printf("Wrote %ld bytes to 'out.mdl'.\n",
					sizeof(Triangle) * numTris);
			save_timer = 320;
			fclose(out);
		}

		char load_txt[32];
		static int load_timer = 0;

		load_timer--;
		if(load_timer < 0) {
			load_timer = 0;
		}

		sprintf(load_txt, !load_timer ?
				"Load Model" : "Model Loaded!");
		if(nk_button_label(ctx, load_txt)) {
			FILE *in = fopen("out.mdl", "rb");
			if(in) {
				load_timer = 320;
				fseek(in, 0, SEEK_END);
				long len = ftell(in);
				numTris = len / sizeof(Triangle);
				tris = realloc(tris,
						numTris * sizeof(Triangle));

				rewind(in);
				fread(tris, sizeof(Triangle), numTris, in);

				fclose(in);
			} else {
				fprintf(stderr, "Failed to open "
						"file from "
						"'out.mdl'.\n");
			}
		}

		nk_layout_row_dynamic(ctx, 30, 2);

		static nk_bool showNormals = 1;
		nk_checkbox_label(ctx, "Show Normals", &showNormals);

		static nk_bool highlightVerts = 1;
		nk_checkbox_label(ctx, "Highlight Vertices", &highlightVerts);

		static float xLook = 0.0f, yLook = 0.0f;
		if(nk_button_label(ctx, "Reset Rotation")) {
			xLook = yLook = 0.0f;
		}

		float xDelta = (float)glfwGetKey(win, GLFW_KEY_D) * deltaTime;
		xDelta -= (float)glfwGetKey(win, GLFW_KEY_A) * deltaTime;

		float yDelta = (float)glfwGetKey(win, GLFW_KEY_S) * deltaTime;
		yDelta -= (float)glfwGetKey(win, GLFW_KEY_W) * deltaTime;

		if(glfwGetKey(win, GLFW_KEY_LEFT_SHIFT)) {
			xDelta *= 2;
			yDelta *= 2;
		}

		xLook += xDelta;
		yLook += yDelta;

		nk_end(ctx);

		Mat4 view;
		Mat4Identity(view);
		Mat4TranslateZ(view, -3);

		Mat4 model;
		Mat4Identity(model);
		Mat4RotateY(model, xLook);
		Mat4RotateX(model, yLook);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glClearColor(bg.r, bg.g, bg.b, bg.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		shaderUse(shader);
		shaderUniformMat4(shader, "proj", proj);
		shaderUniformMat4(shader, "view", view);
		shaderUniformMat4(shader, "model", model);
		// shaderUniform3f(shader, "color", 1.0f, 0.5f, 0.25f);
		glDrawArrays(GL_TRIANGLES, 0, numTris * 3);

		if(highlightVerts) {
			// shaderUniform3f(shader, "color", 0.25f, 0.5f, 1.0f);
			glPointSize(8.0f);
			glDrawArrays(GL_POINTS, 0, numTris * 3);
		}

		/*
		 * Visualising Normals
		 */
		if(showNormals) {
			for(int i = 0; i < numTris; i++) {
				Vertex *v = tris[i].verts;
				Vertex line[2];

				line[0].pos[0] =
					(v[0].pos[0] +
					 v[1].pos[0] +
					 v[2].pos[0]) / 3.0f;

				line[0].pos[1] =
					(v[0].pos[1] +
					 v[1].pos[1] +
					 v[2].pos[1]) / 3.0f;

				line[0].pos[2] =
					(v[0].pos[2] +
					 v[1].pos[2] +
					 v[2].pos[2]) / 3.0f;

				Vec3 normVec;
				Vec3Copy(v->norm, normVec);

				Vec3Copy(normVec, line->norm);
				line->norm[0] = 1.0f - fabs(line->norm[0]);
				line->norm[1] = 1.0f - fabs(line->norm[1]);
				line->norm[2] = 1.0f - fabs(line->norm[2]);

				memcpy(line + 1, line, sizeof(Vertex));
				Vec3Scale(normVec, 0.2f);
				Vec3Add(line->pos, normVec, line[1].pos);

				GLuint va, vb;
				glGenVertexArrays(1, &va);
				glBindVertexArray(va);
				glGenBuffers(1, &vb);
				glBindBuffer(GL_ARRAY_BUFFER, vb);
				glBufferData(GL_ARRAY_BUFFER,
						sizeof(Vertex) * 2,
						line, GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
						sizeof(Vertex),
						(void *)offsetof(Vertex, pos));
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
						sizeof(Vertex),
						(void *)offsetof(Vertex, norm));
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				glLineWidth(4.0f);
				glDrawArrays(GL_LINES, 0, 2);
				glDeleteBuffers(1, &vb);
				glDeleteVertexArrays(1, &va);
			}
		}

		glBindVertexArray(0);

		nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON,
				MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
		glfwSwapBuffers(win);
	}

	shaderUnload(shader);
	
	free(tris);
	nk_glfw3_shutdown(&glfw);
	glfwTerminate();
	return 0;
}
