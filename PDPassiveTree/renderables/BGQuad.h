#pragma once
#include "RenderableObject.h"


class BGQuad : public RenderableObject
{
public:
	BGQuad(glm::vec3 _position);
	~BGQuad();

	void Draw();
	void Update();
};

