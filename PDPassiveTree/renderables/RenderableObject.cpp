#include "RenderableObject.h"

void RenderableObject::BindVBO()
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(BufferData) , &vertices[0], GL_STATIC_DRAW);
}

void RenderableObject::BindVAO()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
}

void RenderableObject::BindEBO()
{
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

RenderableObject::RenderableObject()
{

}

RenderableObject::~RenderableObject()
{

}
