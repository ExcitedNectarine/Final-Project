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
		void createEmpty(const glm::ivec2& size);
		void createFromImage(const Image& image);
		void resize(const glm::ivec2& size);
		void bind();
		void unbind();
		void cleanup();
		GLuint getID();
		glm::ivec2 getSize();

	private:
		GLuint id;
		glm::ivec2 size;
	};
}