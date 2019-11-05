#pragma once
#include "RenderableObject.h"
class CircleMaskQuad :	public RenderableObject
{
public:
	CircleMaskQuad(unsigned int& _texture, unsigned int& _textureMask, glm::vec3 _position, int _width, int _height);
	~CircleMaskQuad();

	void Draw();
	void Update();
	void UpdateTexture(unsigned int& _texture);

	unsigned int textureMask;
};

