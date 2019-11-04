#pragma once
#include "RenderableObject.h"
class Line : public RenderableObject
{
protected:

public:
	Line(std::vector<BufferData>& _vertices);
	~Line();

	void Draw();
	void Update();
};

