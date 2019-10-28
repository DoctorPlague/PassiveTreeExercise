#include "Line.h"
#include "../Render.h"

Line::Line(std::vector<BufferData>& _vertices)
{
	BindVAO();

	vertices = _vertices;

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("shaders/SimpleTransform.vertexshader", "shaders/SimpleColor.fragmentshader");

	BindVBO();

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");

	glm::mat4 Projection = Render::getInstance()->Projection;
	glm::mat4 View = Render::getInstance()->View;

	model = glm::mat4(1.0f);
	MVP = Projection * View * model;
}

Line::~Line()
{
	// Cleanup VBO and shader
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VAO);
}

void Line::Draw()
{
	glUseProgram(programID);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(BufferData), // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(
		1,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(BufferData), // stride
		(void*)(offsetof(BufferData, v3Color))            // array buffer offset
	);

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	glLineWidth(0.5f);
	glDrawArrays(GL_LINES, 0, static_cast<int>(vertices.size()));

	glDisableVertexAttribArray(0);
}

void Line::Update()
{
}
