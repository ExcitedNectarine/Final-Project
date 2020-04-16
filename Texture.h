#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Image.h"

namespace ENG
{
	/**
	* Simple texture class. Creates a texture that is uploaded to the GPU.
	*/
	class Texture
	{
	public:
		void create(const glm::ivec2& size);
		void createFromImage(const Image& image);
		void bind(GLuint unit = 0);
		void unbind();
		void cleanup();
		GLuint getID();
		glm::ivec2 getSize() { return size; }

	private:
		GLuint id;
		glm::ivec2 size;
	};
}