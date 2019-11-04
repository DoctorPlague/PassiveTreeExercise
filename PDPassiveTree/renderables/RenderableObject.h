#pragma once
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../common/shader.hpp"
#include <vector>
#include <iostream>

struct BufferData
{
	BufferData(){}

	BufferData(
		glm::vec3 _v3Pos,
		glm::vec2 _v2UV
	) : v3Pos(_v3Pos), v2UV(_v2UV) {}

	BufferData(
		glm::vec3 _v3Pos,
		glm::vec3 _v3Color
	) : v3Pos(_v3Pos), v3Color(_v3Color) {}

	BufferData(
		glm::vec3 _v3Pos, 
		glm::vec3 _v3Color,
		glm::vec2 _v2UV
	) : v3Pos(_v3Pos), v3Color(_v3Color), v2UV(_v2UV) {}

	glm::vec3 v3Pos;
	glm::vec3 v3Color;
	glm::vec2 v2UV;
};

class RenderableObject
{
protected:
	GLuint VBO;
	GLuint VAO;
	GLuint EBO;
	GLuint programID;
	GLuint MatrixID;
	GLuint textureID;
	unsigned int texture;
	glm::vec3 position;
	glm::mat4 MVP;
	glm::mat4 model;
	std::vector<BufferData> vertices;
	std::vector<unsigned int> indices;
	int width;
	int height;

	void BindVBO();
	void BindVAO();
	void BindEBO();

public:	
	RenderableObject();
	~RenderableObject();
	
	virtual void Draw() = 0;
	virtual void Update() = 0;

	
};

