#include <stdio.h>
#include "vapp.h"
#include "vutils.h"
#include "vmath.h"
#include "LoadShaders.h"


#define TR 0.05f	/*Time Radio*/

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

	int time = 0;
END_APP_DECLARATION()

DEFINE_APP(DrawCommandExample,"Draw Command Example")

void DrawCommandExample::Initialize(const char* title) {

	base::Initialize(title);

	ShaderInfo shader_info[] = {
		{GL_VERTEX_SHADER,"../media/shaders/primitive_restart/primitive_restart.vs.glsl"},
		{GL_FRAGMENT_SHADER,"../media/shaders/primitive_restart/primitive_restart.fs.glsl"},
		{GL_NONE,NULL}
	};
	render_prog = LoadShaders(shader_info);
	glUseProgram(render_prog);

	render_model_matrix_loc = glGetUniformLocation(render_prog, "model_matrix");
	render_projection_matrix_loc = glGetUniformLocation(render_prog, "projection_matrix");

	static const GLfloat vertex_positions[] = {		/*定义四个顶点坐标*/
		-1.0f, -1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  0.0f, 1.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 1.0f, 
	};
	static const GLfloat vertex_colors[] = {		/*定义四个顶点颜色*/
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f
	};

	static const GLushort vertex_indices[] = {    /*定义三个索引 */
		0,1,2
	};

	glGenBuffers(NumEBOs, &ebo[EBO]);/*生成NumEBOs个元素缓存对象*/
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[EBO]);/*给GL指定使用元素缓存绘制时要用的元素缓存对象*/
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(vertex_indices),vertex_indices,GL_STATIC_DRAW);/*为指定的对象拷贝数据*/

	//glGenVertexArrays(NumVAOs,&vao[VAO]);	/*生成NumVAOs个顶点数组对象*/
	//glBindVertexArray(vao[VAO]);			/*给GL指定使用顶点数组绘制时要用的顶点数组对象*/

	glGenBuffers(NumVBOs, &vbo[VBO]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO]);		/*给GL指定顶点缓存对象*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions) + sizeof(vertex_colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0,sizeof(vertex_positions), vertex_positions);						/*缓存顶点位置*/
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_positions), sizeof(vertex_colors), vertex_colors);	/*缓存顶点颜色*/

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,0, (const GLvoid*)sizeof(vertex_positions));
	glEnableVertexAttribArray(0);		/*启用顶点属性数组，为glDraw*()提供数据*/
	glEnableVertexAttribArray(1);
}

void DrawCommandExample::Display(bool auto_redraw) {
	if ((time++*TR) >= 360)
		time = 0;
	float degree = time * TR;
	static const vmath::vec4 black = vmath::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	//glEnable(GL_CULL_FACE);/*开启裁剪,开启后默认裁剪多变形背面*/
	glDisable(GL_DEPTH_TEST);/*关闭深度测试*/
	glClearBufferfv(GL_COLOR, 0, black);		/*清理颜色缓存*/
	glUseProgram(render_prog);

	/*glBindVertexArray(vao[VAO]);*/
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[EBO]);

	
	/*设置模型矩阵*/
	vmath::mat4 model_matrix;
	vmath::mat4 rotate_matrix;
	vmath::mat4 scale_matrix;
	vmath::mat4 translate_matrix;
	rotate_matrix = vmath::rotate(0.0f, 0.0f, 0.0f);
	scale_matrix = vmath::scale(1.0f);

	translate_matrix = vmath::translate(-14.0f, -0.0f, -2.0f);
	rotate_matrix = vmath::rotate(0.0f, 0.0f, 0.0f);
	model_matrix = translate_matrix * rotate_matrix * scale_matrix;
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);		/*传递模型矩阵给着色程序中的uniform变量*/
	glDrawArrays(GL_TRIANGLES,0, 3);											/*直接绘制方式：直接从启用的顶点属性数组中获取顶点数据，按指定规则排序后提取并使用*/

	translate_matrix = vmath::translate(-10.0f, -0.0f, -2.0f);
	rotate_matrix = vmath::rotate( 0.0f, 0.0f, 0.0f);
	model_matrix = translate_matrix * rotate_matrix * scale_matrix;
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glDrawElements(GL_TRIANGLES,3, GL_UNSIGNED_SHORT,NULL);						/*元素数组绘制方式：根据索引从启用的顶点属性数组中获取顶点数据，按指定规则排序后提取并使用*/

	translate_matrix = vmath::translate(-6.0f, -0.0f, -2.0f);
	rotate_matrix = vmath::rotate(0.0f, 0.0f, 0.0f);
	model_matrix = translate_matrix * rotate_matrix * scale_matrix;
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glDrawElementsBaseVertex(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL, 1);		 /*带基础偏移的元素数组绘制方式*/

	translate_matrix = vmath::translate(-3.0f, -0.0f, -2.0f);
	rotate_matrix = vmath::rotate(degree, 0.0f,  0.0f);
	model_matrix = translate_matrix * rotate_matrix * scale_matrix;
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glDrawArraysInstanced(GL_TRIANGLES,0,3,1);/*模式、起始索引、定点数、实例数*/
	

	/*设置视图矩阵*/
	/*x轴向右为正,y向上为正,z轴向屏幕外为正,是右手坐标系,Z->[-1,-500]*/
	/*参数解释：近平面之左边界、右边界、下边界、上边界、近平面到锥顶距离，远平面距锥顶距离*/
	rotate_matrix = vmath::rotate(0.0f, degree, 0.0f );
	vmath::mat4 projection_matrix(vmath::frustum(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 500.0f));		/*用参数描述锥体视图的特征，生成投影矩阵。*/
	projection_matrix = rotate_matrix * projection_matrix;
	glUniformMatrix4fv(render_projection_matrix_loc, 1, GL_FALSE, projection_matrix);				/*传递投影矩阵给着色程序中的uniform变量*/
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