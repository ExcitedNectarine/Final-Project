#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace ENG
{
	class Texture
	{
	public:
		void load(const std::string& filename);
		void bind();
		void unbind();
		void cleanup();

	private:
		GLuint id;
		glm::ivec2 size;
		int channels = 0;
	};
}