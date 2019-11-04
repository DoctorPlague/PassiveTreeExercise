

#define STB_IMAGE_IMPLEMENTATION
#include "common/stb_image.h"

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <filesystem>
#include <future>
#include <queue>
#include <mutex>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Render.h"
#include "renderables/Points.h"
#include "renderables/Line.h"
#include "renderables/BGQuad.h"
#include "common/Debug.h"
#include "Main Classes/PassiveTree.h"

#define M_PI 3.14159265358979323846  /* pi */

using namespace rapidjson;

bool bRunProgram = true;
Render* g_Renderer = Render::getInstance();

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset < 0.0)
	{
		g_Renderer->ZoomOut();
	}
	else if (yoffset > 0.0)
	{
		g_Renderer->ZoomIn();
	}
	g_Renderer->UpdateProjection();
}

int main()
{
	
	g_Renderer->Init();

	glfwSetScrollCallback(g_Renderer->window, scroll_callback);

	g_Renderer->ObjectMap.insert(std::pair<std::string, std::unique_ptr<RenderableObject>>
		("BGQuad", std::make_unique<BGQuad>(glm::vec3(0.0f)))
	);

	std::unique_ptr<PassiveTree> myPassiveTree = std::make_unique<PassiveTree>();
	myPassiveTree->InitializePassiveTree();
		
	//std::vector<std::future<void>> futures;
	//for (auto& itr : NodeMap)
	//{
	//	futures.push_back(std::async(std::launch::async, PopulateNodeTexture, std::ref(itr.second)));
	//}
	//
	//// Need to figure out how im gonna make sure the images are all loaded before i run the rest of the program

	while (bRunProgram)
	{
		g_Renderer->Update();
		g_Renderer->Draw();
	}
	
	g_Renderer->Cleanup();	

	return 0;
}
