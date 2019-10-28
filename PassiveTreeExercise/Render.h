#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <stdlib.h>
#include <stdio.h>
#include <map>

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <common/shader.hpp>

class Render
{
private:
	static Render* instance;
	Render();
	~Render();
public:
	static Render* getInstance();
	static void destroyInstance();
	
	void Init();
	void Update();
	void Draw();
	void Cleanup();

	std::map<std::string, std::unique_ptr<class RenderableObject>> ObjectMap;

	GLFWwindow* window = nullptr;
	glm::mat4 Projection = glm::mat4(1.0f);
	glm::mat4 View = glm::mat4(1.0f);

	const float ScreenWidth = 1620.0f;
	const float ScreenHeight = 900.0f;
};

