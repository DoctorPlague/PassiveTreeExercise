#include "Render.h"
#include "renderables/RenderableObject.h"

using namespace glm;

Render* Render::instance = 0;

Render::Render()
{

}

Render::~Render()
{

}

Render* Render::getInstance()
{
	if (instance == 0)
	{
		instance = new Render();
	}

	return instance;
}

void Render::destroyInstance()
{
	if (instance != 0)
	{
		instance->~Render();
	}
}

void Render::Init()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(ScreenWidth, ScreenHeight, "PassiveTreeExercise", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.035f, 0.035f, 0.035f, 0.0f);	

	Projection = glm::ortho(-ScreenWidth * 9.5f, ScreenWidth * 9.5f, ScreenHeight * 9.5f, -ScreenHeight * 9.5f, -1.0f, 1.0f); // In world coordinates

	// Camera matrix
	View = glm::lookAt(
		glm::vec3(0, 0, 1), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
}

void Render::Update()
{

}

void Render::Draw()
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto& itr : ObjectMap)
	{		
		itr.second->Draw();				
	}

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Render::Cleanup()
{	
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}