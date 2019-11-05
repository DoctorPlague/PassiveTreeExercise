#include "Render.h"
#include "renderables/RenderableObject.h"

#define CURL_STATICLIB
#include <curl\curl.h>

#include <filesystem>
#include <algorithm>
#include "common/stb_image.h"
#include "common/Debug.h"

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

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	Projection = glm::ortho((-ScreenWidth * 9.5f) * zoomLevel, (ScreenWidth * 9.5f) * zoomLevel, (ScreenHeight * 9.5f) * zoomLevel, (-ScreenHeight * 9.5f) * zoomLevel, -1.0f, 1.0f); // In world coordinates

	// Camera matrix
	View = glm::lookAt(
		glm::vec3(0, 0, 1), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
}

void Render::Update()
{
	for (auto& itr : ObjectMap)
	{
		itr.second->Update();
	}
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

size_t callbackfunction(void* ptr, size_t size, size_t nmemb, void* userdata)
{
	FILE* stream = (FILE*)userdata;
	if (!stream)
	{
		printf("!!! No stream\n");
		return 0;
	}

	size_t written = fwrite((FILE*)ptr, size, nmemb, stream);
	return written;
}

void Render::PrepareImageFile(const char* _imageName, const char* _directory, const char* _url)
{
	std::error_code errorCode;

	std::string imagePathStr = "";
	imagePathStr.append(_directory);
	imagePathStr.append(_imageName);

	if (!std::filesystem::exists(imagePathStr, errorCode))
	{
		// File doesn't exist, download it
		FILE* fp = fopen(imagePathStr.c_str(), "wb");
		if (!fp)
		{
			printf("!!! Failed to create file on the disk\n");
			return;
		}

		CURL* curlCtx = curl_easy_init();
		curl_easy_setopt(curlCtx, CURLOPT_URL, _url);
		curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, callbackfunction);
		curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);

		CURLcode rc = curl_easy_perform(curlCtx);
		if (rc)
		{
			printf("!!! Failed to download: %s\n", _url);
			return;
		}

		long res_code = 0;
		curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
		if (!((res_code == 200 || res_code == 201) && rc != CURLE_ABORTED_BY_CALLBACK))
		{
			printf("!!! Response code: %d\n", res_code);
			return;
		}

		curl_easy_cleanup(curlCtx);
		fclose(fp); 
	}
}

bool Render::GenerateTexture(unsigned int& _texture, std::string _imagePath, int& _width, int& _height)
{
	// Generate texture
	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);

	int nrChannels;
	unsigned char* data = stbi_load(_imagePath.c_str(), &_width, &_height, &nrChannels, 0);
	if (data && sizeof(data) > sizeof(""))
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
		return true;
	}
	else
	{
		std::string message = "Failed to load Node image data, ImagePath: ";
		message.append(_imagePath);
		Debug::Log("Error", "Render::GenerateTexture", message.c_str());

		stbi_image_free(data);
		return false;
	}	
}

void Render::UpdateProjection()
{
	Projection = glm::ortho((-ScreenWidth * 9.5f) * zoomLevel, (ScreenWidth * 9.5f) * zoomLevel, (ScreenHeight * 9.5f) * zoomLevel, (-ScreenHeight * 9.5f) * zoomLevel, -1.0f, 1.0f); // In world coordinates
}

void Render::ZoomIn()
{
	zoomLevel = std::clamp(zoomLevel - zoomSpeed, minZoom, maxZoom);
}

void Render::ZoomOut()
{
	zoomLevel = std::clamp(zoomLevel + zoomSpeed, minZoom, maxZoom);
}

