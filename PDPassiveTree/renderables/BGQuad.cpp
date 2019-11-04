//#define STB_IMAGE_IMPLEMENTATION

#include "BGQuad.h"
#include "../Render.h"
#include "../common/stb_image.h"

BGQuad::BGQuad(glm::vec3 _position)
{	
	position = _position;

	// Define Vertices
	vertices.push_back(BufferData(
		glm::vec3(1.0f, 1.0f, 0.0f),   // pos
		glm::vec2(25.0f, 25.0f)		    // uv
	));  // top right
	vertices.push_back(BufferData(
		glm::vec3(1.0f, -1.0f, 0.0f),    // pos
		glm::vec2(25.0f, 0.0f)		    // uv
	)); // bottom right
	vertices.push_back(BufferData(
		glm::vec3(-1.0f, -1.0f, 0.0f),  // pos
		glm::vec2(0.0f, 0.0f)		    // uv
	)); // bottom left
	vertices.push_back(BufferData(
		glm::vec3(-1.0f, 1.0f, 0.0f),	// pos
		glm::vec2(0.0f, 25.0f)			// uv
	)); // top left

	// Define Indices
	indices = {
		0, 1, 3,
		1, 2, 3
	};

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("shaders/SimpleTexture.vertexshader", "shaders/SimpleTexture.fragmentshader");

	BindVAO();
	BindVBO();
	BindEBO();

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");

	model = glm::mat4(1.0f);
	MVP = glm::mat4(1.0f);

	int width;
	int height;
	Render::getInstance()->GenerateTexture(texture, "images/Background1.png", width, height);
}

BGQuad::~BGQuad()
{

}

void BGQuad::Draw()
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

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(
		2,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(BufferData), // stride
		(void*)(offsetof(BufferData, v2UV))            // array buffer offset
	);

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	glBindTexture(GL_TEXTURE_2D, texture);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

	glDisableVertexAttribArray(0);
}

void BGQuad::Update()
{

}
