#pragma once
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <common/shader.hpp>
#include <vector>
#include <iostream>

struct BufferData
{
	glm::vec3 v3Pos;
	glm::vec3 v3Color;
};

class RenderableObject
{
protected:
	GLuint VBO;// = -1;
	GLuint VAO;// = -1;
	GLuint programID;// = -1;
	GLuint MatrixID;// = -1;
	glm::vec3 position;// = glm::vec3(0.0f);
	glm::mat4 MVP;// = glm::mat4(1.0f);
	glm::mat4 model;// = glm::mat4(1.0f);
	std::vector<BufferData> vertices;

	void BindVBO();
	void BindVAO();

public:	
	RenderableObject();
	~RenderableObject();
	
	virtual void Draw() = 0;
	virtual void Update() = 0;

	
};

