#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

class VertexBuffer
{
public:
	VertexBuffer();
	void add(const glm::vec2& value);
	void add(const glm::vec3& value);
	void add(const glm::vec4& value);
	GLuint getID();
	int getComponents();
	int getDataSize();
	void clear();

private:
	GLuint id;
	bool dirty = false;
	int components = 0;
	std::vector<GLfloat> data;
};