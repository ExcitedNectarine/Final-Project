#include "VertexBuffer.h"

VertexBuffer::VertexBuffer()
{
	glGenBuffers(1, &id);

	/*if (!id)
		throw ApplicationException("Failed to generate vertex buffer.");*/
}

void VertexBuffer::add(const glm::vec2& value)
{
	//if (components != 2 && components != 0)
	//	throw ApplicationException("Incorrect components assigned to vertex buffer.");

	data.push_back(value.x);
	data.push_back(value.y);

	components = 2;
	dirty = true;
}

void VertexBuffer::add(const glm::vec3& value)
{
	//if (components != 3 && components != 0)
	//	throw ApplicationException("Incorrect components assigned to vertex buffer.");

	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.z);

	components = 3;
	dirty = true;
}

void VertexBuffer::add(const glm::vec4& value)
{
	//if (components != 4 && components != 0)
	//	throw ApplicationException("Incorrect components assigned to vertex buffer.");

	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.z);
	data.push_back(value.w);

	components = 4;
	dirty = true;
}

GLuint VertexBuffer::getID()
{
	if (dirty)
	{
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), &data.at(0), GL_STATIC_DRAW);
		dirty = false;
	}

	return id;
}

int VertexBuffer::getComponents()
{
	return components;
}

int VertexBuffer::getDataSize()
{
	return data.size();
}

void VertexBuffer::clear()
{
	data.clear();
	components = 0;
	dirty = true;
}
