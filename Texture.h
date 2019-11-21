#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Texture
{
public:
	Texture();
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	void load(const std::string& filename);
	void bind();
	void unbind();

private:
	GLuint id;
	glm::ivec2 size;
	int channels = 0;
};