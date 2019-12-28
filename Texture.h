#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Image.h"

namespace ENG
{
	class Texture
	{
	public:
		void createFromImage(const Image& image);
		void bind();
		void unbind();
		void cleanup();

	private:
		GLuint id;
		glm::ivec2 size;
	};
}