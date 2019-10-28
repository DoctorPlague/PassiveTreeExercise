#pragma once
#include "RenderableObject.h"
class Points : public RenderableObject
{
protected:

public:
	Points(std::vector<BufferData>& _vertices);
	~Points();

	void Draw();
	void Update();
};

