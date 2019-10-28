#include "RenderableObject.h"

void RenderableObject::BindVBO()
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(BufferData) , &vertices[0], GL_DYNAMIC_DRAW);
}

void RenderableObject::BindVAO()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
}

RenderableObject::RenderableObject()
{
	
}

RenderableObject::~RenderableObject()
{

}
