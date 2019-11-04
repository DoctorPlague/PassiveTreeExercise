#pragma once
#include "RenderableObject.h"
class TexturedQuad : public RenderableObject
{
public:
	TexturedQuad(unsigned int& _texture, glm::vec3 _position, int _width, int _height);
	~TexturedQuad();

	void Draw();
	void Update();
	void UpdateTexture(unsigned int& _texture);
};

