#include <stdio.h>
#include "vapp.h"
#include "vutils.h"
#include "vmath.h"
#include "LoadShaders.h"

BEGIN_APP_DECLARATION(DrawCommandExample)
	virtual void Initialize(const char* title);
	virtual void Display(bool auto_redraw);
	virtual void Finalize(void);
	virtual void Resize(int width, int height);

	//用枚举表示起始索引和容器长度
	enum { VAO, NumVAOs };					/*顶点数组对象*/
	enum { VBO, NumVBOs };					/*顶点缓存对象*/
	enum { EBO, NumEBOs };					/*索引缓存对象*/
	float aspect;
	GLuint render_prog;
	GLuint vao[NumVAOs];
	GLuint vbo[NumVBOs];
	GLuint ebo[NumEBOs];

	GLuint render_model_matrix_loc;
	GLuint render_projection_matrix_loc;
END_APP_DECLARATION()

DEFINE_APP(DrawCommandExample,"Draw Command Example")

void DrawCommandExample::Initialize(const char* title) {
	base::Initialize(title);

	ShaderInfo shader_info[] = {
		{GL_VERTEX_SHADER,"media/shaders/primitive_restart/primitive_restart.vs.glsl"},
		{GL_FRAGMENT_SHADER,"media/shaders/primitive_restart/primitive_restart.fs.glsl"},
		{GL_NONE,NULL}
	};
	render_prog = LoadShaders(shader_info);
	glUseProgram(render_prog);

	render_model_matrix_loc = glGetUniformLocation(render_prog, "model_matrix");
	render_projection_matrix_loc = glGetUniformLocation(render_prog, "projection_matrix");


	static const GLfloat vertex_positions[] = {
		-1.0f, -1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  0.0f, 1.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 1.0f, 
	};
	static const GLfloat vertex_colors[] = {
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f
	};

	static const GLushort vertex_indices[] = {
		0,1,2
	};

	glGenBuffers(NumEBOs, &ebo[EBO]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[EBO]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(vertex_indices),vertex_indices,GL_STATIC_DRAW);

	glGenVertexArrays(NumVAOs,&vao[VAO]);
	glBindVertexArray(vao[VAO]);

	glGenBuffers(VBO, &vbo[VBO]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions) + sizeof(vertex_colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0,sizeof(vertex_positions), vertex_positions);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_positions), sizeof(vertex_colors), vertex_colors);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,0, (const GLvoid*)sizeof(vertex_positions));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void DrawCommandExample::Display(bool auto_redraw) {
	float t = float(app_time() & 0x1FFF) / float(0x1FFF);
	static float q = 0.0f;
	static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
	static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
	static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);
	static const vmath::vec4 black = vmath::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	vmath::mat4 model_matrix;

	glEnable(GL_CULL_FACE);/*开启裁剪*/
	glDisable(GL_DEPTH_TEST);/*关闭深度测试*/
	glClearBufferfv(GL_COLOR, 0, black);		/*清理颜色缓存*/
	glUseProgram(render_prog);
	vmath::mat4 projection_matrix(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 500.0f));/*用参数描述锥体视图的特征，生成投影矩阵*/
	glUniformMatrix4fv(render_projection_matrix_loc, 1, GL_FALSE, projection_matrix);/*传递投影矩阵给着色程序中的uniform变量*/

	glBindVertexArray(vao[VAO]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[EBO]);
	model_matrix = vmath::translate(-3.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	base::Display();
}

void DrawCommandExample::Finalize() {
	glUseProgram(0);
	glDeleteProgram(render_prog);
	glDeleteVertexArrays(NumVAOs,vao);
	glDeleteBuffers(NumVBOs, vbo);
}

void DrawCommandExample::Resize(int width, int height) {
	glViewport(0, 0, width, height);
	aspect = float(height) / float(width);
}