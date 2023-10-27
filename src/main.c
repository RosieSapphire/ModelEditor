#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>

#include "shader.h"
#include "triangle.h"
#include "config.h"
#include "mat4.h"

#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

static struct nk_glfw glfw = {0};
static GLFWwindow *win;
static struct nk_context *ctx;
static int width, height;
static float aspect_ratio;
static int vert_selected = 0;
static nk_bool show_normals = 1;
static nk_bool highlight_verts = 1;
static nk_bool backface_culling = 1;
static float look[2] = {0, 0};
static double mouse_last[2] = {0, 0};

static void glfw_and_nk_init(void);
static int triangle_list_setup(struct triangle *tris, int num_tris,
			       nk_bool *verts_is_selected);
static void triangle_normals_render(struct triangle *tris, int num_tris);
static void save_model_button(struct triangle *tris, int num_tris);
static void load_model_button(struct triangle *tris, int num_tris);
static void user_move_vertices_along_axis(struct triangle *tris);
static void user_determine_selected_vertex(nk_bool *is_selected_arr, int tri_ind,
					   int vert_ind, int num_tris);

int main(void)
{
	glfw_and_nk_init();

	int num_tris = 1;
	nk_bool *verts_is_selected = calloc(num_tris * 3, sizeof(nk_bool));
	
	struct triangle *tris = calloc(num_tris, sizeof(struct triangle));
	triangle_init_default(tris);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,
			sizeof(struct triangle) * num_tris,
			tris, GL_DYNAMIC_DRAW);
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
	Mat4Perspective(proj, 60.0f, aspect_ratio, 0.1f, 8.0f);

	// float timeLast = glfwGetTime();
	while(!glfwWindowShouldClose(win))
	{
		// float timeNow = glfwGetTime();
		// float deltaTime = timeNow - timeLast;
		// timeLast = timeNow;

		glfwPollEvents();
		if(glfwGetKey(win, GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(win, GLFW_TRUE);

		char vert_list_str[64];
		sprintf(vert_list_str, "struct triangle List: (%d)", num_tris);

		triangles_calc_normals(tris, num_tris);

		nk_glfw3_new_frame(&glfw);

		if(!nk_begin(ctx, vert_list_str, nk_rect(0, 0, 420, height),
					NK_WINDOW_BORDER |
					NK_WINDOW_TITLE))
			continue;

		nk_layout_row_dynamic(ctx, 30, 2);
		if(nk_button_label(ctx, "Add struct triangle")) {
			num_tris++;
			tris =
			realloc(tris, sizeof(struct triangle) * num_tris);
			memset(tris + (num_tris - 1), 0,
				sizeof(struct triangle));

			verts_is_selected = realloc(
				verts_is_selected,
				sizeof(nk_bool)
				* 3 * num_tris);
			
			memset(verts_is_selected, 0,
					sizeof(nk_bool) * 3 * num_tris);
		}

		nk_layout_row_dynamic(ctx, 1024, 1);

		if(nk_group_begin(ctx, "Triangle List", NK_WINDOW_BORDER)) {
			num_tris = triangle_list_setup(tris,
					num_tris, verts_is_selected);
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER,
				sizeof(struct triangle) * num_tris,
				tris, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		nk_layout_row_dynamic(ctx, 30, 1);

		user_move_vertices_along_axis(tris);

		char vsStr[512];
		sprintf(vsStr, "Vertex Selected: %d", vert_selected);
		nk_label(ctx, vsStr, NK_TEXT_LEFT);

		nk_layout_row_dynamic(ctx, 30, 3);
		if(nk_button_label(ctx, "New Model")) {
			num_tris = 1;
			tris = realloc(tris,
					num_tris * sizeof(struct triangle));
			memset(tris, 0, sizeof(struct triangle));
		}

		save_model_button(tris, num_tris);
		load_model_button(tris, num_tris);

		nk_layout_row_dynamic(ctx, 30, 1);

		nk_checkbox_label(ctx, "Show Normals", &show_normals);
		nk_checkbox_label(ctx, "Highlight Vertices", &highlight_verts);
		nk_checkbox_label(ctx, "Backface Culling", &backface_culling);

		nk_layout_row_dynamic(ctx, 30, 3);
		if(nk_button_label(ctx, "Reset Rotation"))
			look[0] = look[1] = 0.0f;

		double mouse_now[2];
		glfwGetCursorPos(win, mouse_now + 0, mouse_now + 1);
		double mouse_delta[2] = {
			(mouse_now[0] - mouse_last[0]) * 0.01,
			(mouse_now[1] - mouse_last[1]) * 0.01,
		};

		mouse_last[0] = mouse_now[0];
		mouse_last[1] = mouse_now[1];

		bool rightClickDownNow =
			glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT);

		if(rightClickDownNow) {
			look[0] += mouse_delta[0];
			look[1] += mouse_delta[1];
		}

		nk_end(ctx);

		Mat4 view;
		Mat4Identity(view);
		Mat4TranslateZ(view, -3);

		Mat4 model;
		Mat4Identity(model);
		Mat4RotateY(model, look[0]);
		Mat4RotateX(model, look[1]);
		
		glDisable(GL_CULL_FACE);

		if(backface_culling) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);
		}

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		struct nk_colorf bg = {0.10f, 0.18f, 0.24f, 1.0f};
		glClearColor(bg.r, bg.g, bg.b, bg.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		shaderUse(shader);
		shaderUniformMat4(shader, "model", model);
		shaderUniformMat4(shader, "proj", proj);
		shaderUniformMat4(shader, "view", view);
		glDrawArrays(GL_TRIANGLES, 0, num_tris * 3);

		if(highlight_verts) {
			glPointSize(8.0f);
			glDrawArrays(GL_POINTS, 0, num_tris * 3);
		}

		/*
		 * Visualising Normals
		 */
		if(show_normals) {
			triangle_normals_render(tris, num_tris);
		}

		glBindVertexArray(0);

		nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON,
				CONF_VERT_BUF_MAX, CONF_ELEM_BUF_MAX);
		glfwSwapBuffers(win);
	}

	shaderUnload(shader);
	
	free(verts_is_selected);
	free(tris);
	nk_glfw3_shutdown(&glfw);
	glfwTerminate();
	return 0;
}

static void glfw_and_nk_init(void)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(),
			NULL, NULL, &width, &height);

	aspect_ratio = (float)width / (float)height;

	win = glfwCreateWindow(width, height, "3D Model Editor", NULL, NULL);
	glfwMakeContextCurrent(win);
	
	glewExperimental = 1;
	glewInit();
	glViewport(0, 0, width, height);

	ctx = nk_glfw3_init(&glfw, win, NK_GLFW3_INSTALL_CALLBACKS);

	struct nk_font_atlas *atlas;
	nk_glfw3_font_stash_begin(&glfw, &atlas);
	nk_glfw3_font_stash_end(&glfw);
}

static int triangle_list_setup(struct triangle *tris, int num_tris,
		nk_bool *verts_is_selected)
{
	for(int j = 0; j < num_tris; j++) {
		char vert_str[32];
		sprintf(vert_str, "Tri %d", j);

		if(nk_tree_push(ctx, NK_TREE_NODE, vert_str, NK_MAXIMIZED)) {
			nk_layout_row_dynamic(ctx, 26, 4);

			for(int k = 0; k < 3; k++) {
				user_determine_selected_vertex(
					verts_is_selected, j, k, num_tris);
	
				for(int i = 0; i < 3; i++) {
					float *val_cur =
					&tris[j].verts[k].pos[i];
	
					const char *names[3] = {
						"X", "Y", "Z"
					};

					nk_property_float(ctx, names[i],
							-16.0f, val_cur,
							16.0f, 0.1f,
							1.0f);
				}
			}
	
			nk_layout_row_dynamic(ctx, 26, 2);
			if(nk_button_label(ctx, "Delete")) {
				num_tris--;
				if(num_tris) {
					for(int i = j;
						i < num_tris + 1;
							i++) {
						memcpy(tris + i,
							tris
							+ i + 1,
							sizeof(
							struct triangle
							));
					}
	
					tris = realloc(tris,
						num_tris *
						sizeof(
						struct triangle));
				} else {
					memset(tris, 0,
						sizeof(
						struct triangle));
					num_tris++;
				}

				verts_is_selected = realloc(
						verts_is_selected,
						sizeof(nk_bool)
						* 3 * num_tris);

				memset(verts_is_selected, 0,
					sizeof(nk_bool) *
					3 * num_tris);
			}

			if(nk_button_label(ctx, "Duplicate")) {
				num_tris++;
				tris = realloc(tris,
					num_tris *
					sizeof(
					struct triangle));

				for(int i = num_tris - 1; i > j; i--) {
					memcpy(tris + i, tris + i - 1,
						sizeof(struct triangle));
				}
			}
	
			nk_tree_pop(ctx);
		}
	
	}

	nk_group_end(ctx);

	return num_tris;
}

static void triangle_normals_render(struct triangle *tris, int num_tris)
{
	for(int i = 0; i < num_tris; i++) {
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

static void save_model_button(struct triangle *tris, int num_tris)
{
	char save_txt[32];
	static int save_timer = 0;

	save_timer--;
	if(save_timer < 0) {
		save_timer = 0;
	}

	sprintf(save_txt, !save_timer ? "Save Model" : "Model Saved!");
	if(nk_button_label(ctx, save_txt)) {
		FILE *out = fopen("out.mdl", "wb");
		fwrite(tris, sizeof(struct triangle), num_tris, out);

		printf("Wrote %ld bytes to 'out.mdl'.\n",
				sizeof(struct triangle) * num_tris);
		save_timer = 320;
		fclose(out);
	}
}

static void load_model_button(struct triangle *tris, int num_tris)
 {
	char load_txt[32];
	static int load_timer = 0;

	load_timer--;
	if(load_timer < 0) {
		load_timer = 0;
	}

	sprintf(load_txt, !load_timer ? "Load Model" : "Model Loaded!");
	if(nk_button_label(ctx, load_txt)) {
		FILE *in = fopen("out.mdl", "rb");
		if(in) {
			load_timer = 320;
			fseek(in, 0, SEEK_END);
			long len = ftell(in);
			num_tris = len / sizeof(struct triangle);
			tris = realloc(tris, num_tris * sizeof(struct triangle));

			rewind(in);
			fread(tris, sizeof(struct triangle), num_tris, in);

			fclose(in);
		} else {
			fprintf(stderr, "Failed to open file from 'out.mdl'.\n");
		}
	}
 }

static void user_move_vertices_along_axis(struct triangle *tris)
{
	static bool axisDownLast[3] = {0};
	bool axisDownNow[3] = {
		glfwGetKey(win, GLFW_KEY_X),
		glfwGetKey(win, GLFW_KEY_Y),
		glfwGetKey(win, GLFW_KEY_Z),
	};

	bool axisPress[3] = {
		axisDownNow[0] && !axisDownLast[0],
		axisDownNow[1] && !axisDownLast[1],
		axisDownNow[2] && !axisDownLast[2],
	};

	memcpy(axisDownLast, axisDownNow, sizeof(bool) * 3);

	static int mouseXOnPress = 0;
	static bool isTranslating[3] = {0};
	static float startPos;

	for(int i = 0; i < 3; i++) {
		double x;
		glfwGetCursorPos(win, &x, NULL);

		if(axisPress[i]) {
			mouseXOnPress = (int)x;
			startPos = tris->verts[vert_selected].pos[i];
			isTranslating[i] = !isTranslating[i];
		}

		if(isTranslating[i]) {
			tris->verts[vert_selected].pos[i] = startPos -
				((x - mouseXOnPress) * -0.02f);
		}
	}
}

static void user_determine_selected_vertex(nk_bool *is_selected_arr, int tri_ind,
					   int vert_ind, int num_tris)
{
	if(nk_selectable_label(ctx, "Position:", NK_TEXT_LEFT,
			is_selected_arr + vert_ind + (tri_ind * 3))) {
		int totalSelected = 0;

		for(int i = 0; i < 3 * num_tris; i++) {
			totalSelected += is_selected_arr[i];
		}

		if(totalSelected > 1) {
			memset(is_selected_arr, 0,
					sizeof(nk_bool) * 3 * num_tris);
			is_selected_arr[(tri_ind * 3) + vert_ind] = 1;
		}

		/*
		 * TODO: Might not work
		 */
		for(int i = 0; i < 3 * num_tris && !is_selected_arr[i]; i++) {
			vert_selected = i;
		}
	}
}
