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

	//��ö�ٱ�ʾ��ʼ��������������
	enum { VAO, NumVAOs };					/*�����������*/
	enum { VBO, NumVBOs };					/*���㻺�����*/
	enum { EBO, NumEBOs };					/*�����������*/
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

	static const GLfloat vertex_positions[] = {		/*�����ĸ���������*/
		-1.0f, -1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  0.0f, 1.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 1.0f, 
	};
	static const GLfloat vertex_colors[] = {		/*�����ĸ�������ɫ*/
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f
	};

	static const GLushort vertex_indices[] = {    /*������������ */
		0,1,2
	};

	glGenBuffers(NumEBOs, &ebo[EBO]);/*����NumEBOs��Ԫ�ػ������*/
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[EBO]);/*��GLָ��ʹ��Ԫ�ػ������ʱҪ�õ�Ԫ�ػ������*/
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(vertex_indices),vertex_indices,GL_STATIC_DRAW);/*Ϊָ���Ķ��󿽱�����*/

	//glGenVertexArrays(NumVAOs,&vao[VAO]);	/*����NumVAOs�������������*/
	//glBindVertexArray(vao[VAO]);			/*��GLָ��ʹ�ö����������ʱҪ�õĶ����������*/

	glGenBuffers(NumVBOs, &vbo[VBO]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO]);		/*��GLָ�����㻺�����*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions) + sizeof(vertex_colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0,sizeof(vertex_positions), vertex_positions);						/*���涥��λ��*/
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_positions), sizeof(vertex_colors), vertex_colors);	/*���涥����ɫ*/

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,0, (const GLvoid*)sizeof(vertex_positions));
	glEnableVertexAttribArray(0);		/*���ö����������飬ΪglDraw*()�ṩ����*/
	glEnableVertexAttribArray(1);
}

void DrawCommandExample::Display(bool auto_redraw) {
	if ((time++*TR) >= 360)
		time = 0;
	float degree = time * TR;
	static const vmath::vec4 black = vmath::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	//glEnable(GL_CULL_FACE);/*�����ü�,������Ĭ�ϲü�����α���*/
	glDisable(GL_DEPTH_TEST);/*�ر���Ȳ���*/
	glClearBufferfv(GL_COLOR, 0, black);		/*������ɫ����*/
	glUseProgram(render_prog);

	/*glBindVertexArray(vao[VAO]);*/
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[EBO]);

	
	/*����ģ�;���*/
	vmath::mat4 model_matrix;
	vmath::mat4 rotate_matrix;
	vmath::mat4 scale_matrix;
	vmath::mat4 translate_matrix;
	rotate_matrix = vmath::rotate(0.0f, 0.0f, 0.0f);
	scale_matrix = vmath::scale(1.0f);

	translate_matrix = vmath::translate(-14.0f, -0.0f, -2.0f);
	rotate_matrix = vmath::rotate(0.0f, 0.0f, 0.0f);
	model_matrix = translate_matrix * rotate_matrix * scale_matrix;
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);		/*����ģ�;������ɫ�����е�uniform����*/
	glDrawArrays(GL_TRIANGLES,0, 3);											/*ֱ�ӻ��Ʒ�ʽ��ֱ�Ӵ����õĶ������������л�ȡ�������ݣ���ָ�������������ȡ��ʹ��*/

	translate_matrix = vmath::translate(-10.0f, -0.0f, -2.0f);
	rotate_matrix = vmath::rotate( 0.0f, 0.0f, 0.0f);
	model_matrix = translate_matrix * rotate_matrix * scale_matrix;
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glDrawElements(GL_TRIANGLES,3, GL_UNSIGNED_SHORT,NULL);						/*Ԫ��������Ʒ�ʽ���������������õĶ������������л�ȡ�������ݣ���ָ�������������ȡ��ʹ��*/

	translate_matrix = vmath::translate(-6.0f, -0.0f, -2.0f);
	rotate_matrix = vmath::rotate(0.0f, 0.0f, 0.0f);
	model_matrix = translate_matrix * rotate_matrix * scale_matrix;
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glDrawElementsBaseVertex(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL, 1);		 /*������ƫ�Ƶ�Ԫ��������Ʒ�ʽ*/

	translate_matrix = vmath::translate(-3.0f, -0.0f, -2.0f);
	rotate_matrix = vmath::rotate(degree, 0.0f,  0.0f);
	model_matrix = translate_matrix * rotate_matrix * scale_matrix;
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glDrawArraysInstanced(GL_TRIANGLES,0,3,1);/*ģʽ����ʼ��������������ʵ����*/
	

	/*������ͼ����*/
	/*x������Ϊ��,y����Ϊ��,z������Ļ��Ϊ��,����������ϵ,Z->[-1,-500]*/
	/*�������ͣ���ƽ��֮��߽硢�ұ߽硢�±߽硢�ϱ߽硢��ƽ�浽׶�����룬Զƽ���׶������*/
	rotate_matrix = vmath::rotate(0.0f, degree, 0.0f );
	vmath::mat4 projection_matrix(vmath::frustum(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 500.0f));		/*�ò�������׶����ͼ������������ͶӰ����*/
	projection_matrix = rotate_matrix * projection_matrix;
	glUniformMatrix4fv(render_projection_matrix_loc, 1, GL_FALSE, projection_matrix);				/*����ͶӰ�������ɫ�����е�uniform����*/
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